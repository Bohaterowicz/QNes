#include <Windows.h>
#include <windef.h>
#include <wingdi.h>
#include <winuser.h>

#include <thread>

#include "emulator_core/qnes_emu.hpp"
#include "emulator_core/qnes_texture.hpp"
#include "gui/interface_controller.hpp"
#include "platform/common/platform_services.hpp"
#include "qnes_c.hpp"
#include "renderer/renderer.hpp"
#include "win_imgui.hpp"
#include "win_opengl.hpp"
#include "win_window.hpp"

namespace QNes::platform::windows {

ReadFileResult WinReadFile(std::string_view filepath) {
  // convert to null terminated string
  std::string filepath_str = std::string(filepath) + '\0';
  ReadFileResult result = {};
  // Open handle to file as read-only
  HANDLE file_handle =  // NOLINT
      CreateFileA(filepath_str.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                  OPEN_EXISTING, 0, nullptr);
  if (file_handle != INVALID_HANDLE_VALUE)  // NOLINT
  {
    // Get size of the file
    LARGE_INTEGER file_size;
    if (GetFileSizeEx(file_handle, &file_size) != 0) {
      // Truncate the 64bit file size into 32bit
      // This is because ReadFile call can only take a DWORD (32bit) size to
      // read, which would require multiple reads for files bigger than maximum
      // 32bit number. Since it is a debug call, we should never read files this
      // big (>4GB) using this call
      u32 file_size32 = SafeU64ToU32(file_size.QuadPart);
      // Allocate buffer needed to store the file contents
      result.data = std::make_unique<u8[]>(file_size32);
      if (result.data) {
        // Read the content of the file into the buffer
        DWORD bytes_read = 0;
        if ((::ReadFile(file_handle, result.data.get(), file_size32,
                        &bytes_read, nullptr) != 0) &&
            (bytes_read == file_size32)) {
          result.size = file_size32;
        } else {
          // if reading file failed
          result = ReadFileResult{};
        }
      }
    }
    // Close the handle to the file
    CloseHandle(file_handle);
  }
  return result;
}

std::string WinOpenFileDialog() {
  OPENFILENAMEA ofn;
  CHAR szFile[MAX_PATH] = {0};
  ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
  ofn.lStructSize = sizeof(OPENFILENAMEA);
  ofn.hwndOwner = nullptr;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

  if (GetOpenFileName(&ofn) == TRUE) {
    return {ofn.lpstrFile};
  }
  return {};
}

static bool OpenDebugConsole() {
  if (AllocConsole() == 0) {
    return false;
  }
  FILE *fDummy = nullptr;
  if (freopen_s(&fDummy, "CONIN$", "r", stdin) != 0) {
    return false;
  }
  if (freopen_s(&fDummy, "CONOUT$", "w", stdout) != 0) {
    return false;
  }
  if (freopen_s(&fDummy, "CONOUT$", "w", stderr) != 0) {
    return false;
  }
  return true;
}

}  // namespace QNes::platform::windows

// WinMain must be at global scope - Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
  using namespace QNes::platform::windows;

  // Set timer resolution to 1ms for better sleep precision
  timeBeginPeriod(1);

#ifndef NDEBUG
  const auto debug_console_ok = OpenDebugConsole();
  if (debug_console_ok) {
    DBG_PRINT("DEBUG BUILD");
  } else {
    return 1;
  }
#endif  // NDEBUG
  // Create window
  WinQNesWindow window("QNes");
  window.SetPixelFormat(WinQNesWindow::GetDefaultPixelFormat());
  if (!window.Create(hInstance)) {
    return 1;
  }

  QNes::platform::PlatformServicesFunctions functions = {
      .OpenFileDialog = []() { return WinOpenFileDialog(); },
      .ReadFile =
          [](std::string_view filename) { return WinReadFile(filename); },
  };
  QNes::platform::PlatformServices::Initialize(functions);

  HWND window_handle = window.GetHandle();
  QNes::renderer::RendererPlatformBackend platform_backend = {
      .Initialize =
          [window_handle]() {
            return WinInitializeOpenGL(window_handle) &&
                   WinInitializeImGui(window_handle);
          },
      .SwapBuffers =
          [window_handle]() {
            HDC window_dc = GetDC(window_handle);
            ::SwapBuffers(window_dc);
            ReleaseDC(window_handle, window_dc);
          },

      .InterfaceNewFrame = []() { WinImGuiNewFrame(); },
  };

  // Create framebuffer - this is where the emulator will output pixel data
  QNes::NESTexture framebuffer;

  // Initialize renderer
  auto [width, height] = framebuffer.GetDimensions();
  QNes::renderer::Renderer renderer(platform_backend, width, height);
  if (!renderer.Initialize(&framebuffer)) {
    DBG_PRINT("Failed to initialize renderer");
    return 1;
  }

  QNes::gui::InterfaceController::Initialize(&renderer);
  QNes::Emulator::Initialize(&framebuffer);
  QNes::Emulator::Get().Pause(true);

  // Run emulator in a separate thread
  std::thread emulator_thread([&]() {
    auto &emulator = QNes::Emulator::Get();
    while (!emulator.IsShutdownRequested()) {
      if (!emulator.IsPaused()) {
        emulator.Run();
      }
    }
  });

  while (!window.ShutdownRequested()) {
    // Process window messages
    window.ProcessMessages();

    renderer.DrawFrameBuffer(framebuffer);
    renderer.DrawInterface();
    renderer.SwapBuffers();
  }

  // Restore timer resolution
  timeEndPeriod(1);
  return 0;
}