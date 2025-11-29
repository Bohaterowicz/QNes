#include <Windows.h>
#include <windef.h>
#include <wingdi.h>
#include <winuser.h>

#include <thread>

#include "emulator_core/qnes_frame_buffer.hpp"
#include "qnes_c.hpp"
#include "renderer/renderer.hpp"
#include "win_imgui.hpp"
#include "win_opengl.hpp"
#include "win_window.hpp"

namespace QNES::platform::windows {

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

}  // namespace QNES::platform::windows

// WinMain must be at global scope - Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd) {
  using namespace QNES::platform::windows;

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
  WinQNESWindow window("QNES");
  window.SetPixelFormat(WinQNESWindow::GetDefaultPixelFormat());
  if (!window.Create(hInstance)) {
    return 1;
  }

  HWND window_handle = window.GetHandle();
  QNES::renderer::RendererPlatformBackend platform_backend = {
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
  QNES::NESFrameBuffer framebuffer;

  // Initialize renderer
  auto [width, height] = framebuffer.GetDimensions();
  QNES::renderer::Renderer renderer(platform_backend, width, height);
  if (!renderer.Initialize()) {
    DBG_PRINT("Failed to initialize renderer");
    return 1;
  }

  // Start emulator in a separate thread
  std::thread emulator_thread([&]() {
    // TODO: Start emulator
  });

  while (!window.ShutdownRequested()) {
    // Process window messages
    window.ProcessMessages();

    // copy framebuffer to opengl texture
    renderer.DrawFrameBuffer(framebuffer);
    renderer.DrawInterface();
    renderer.SwapBuffers();
  }

  // Restore timer resolution
  timeEndPeriod(1);
  return 0;
}