#include "win_window.hpp"

#include <minwindef.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

namespace QNes::platform::windows {

PIXELFORMATDESCRIPTOR WinQNesWindow::GetDefaultPixelFormat() {
  PIXELFORMATDESCRIPTOR pixel_format_desc = {};
  pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pixel_format_desc.nVersion = 1;
  pixel_format_desc.dwFlags =
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
  pixel_format_desc.cColorBits = 32;
  pixel_format_desc.cAlphaBits = 8;
  pixel_format_desc.cDepthBits = 24;
  pixel_format_desc.cStencilBits = 8;
  pixel_format_desc.iLayerType = PFD_MAIN_PLANE;
  return pixel_format_desc;
}

static bool Win32SetPixelFormat(HWND window,
                                PIXELFORMATDESCRIPTOR &pixel_format_desc) {
  HDC window_dc = GetDC(window);
  i32 pixel_format_idx = ChoosePixelFormat(window_dc, &pixel_format_desc);
  DescribePixelFormat(window_dc, pixel_format_idx, sizeof(pixel_format_desc),
                      &pixel_format_desc);
  // Check if we got a valid suggestion of pixel format
  if (pixel_format_idx != 0) {
    // continue only if we succeed in setting the pixel format
    if (SetPixelFormat(window_dc, pixel_format_idx, &pixel_format_desc) != 0) {
      ReleaseDC(window, window_dc);
      return true;
    }
  }
  ReleaseDC(window, window_dc);
  return false;
}

LRESULT CALLBACK WinQNESWindowProc(HWND hwnd, UINT msg, WPARAM wparam,
                                   LPARAM lparam) {
  if (auto result = ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
    return result;
  }
  LRESULT result = 0;
  switch (msg) {
    case WM_CREATE: {
      auto *create = reinterpret_cast<CREATESTRUCT *>(lparam);  // NOLINT
      auto *window = static_cast<WinQNesWindow *>(create->lpCreateParams);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
      PIXELFORMATDESCRIPTOR pixel_format_desc = window->GetPixelFormat();
      if (!Win32SetPixelFormat(hwnd, pixel_format_desc)) {
        // Could not set pixel format or initialize opengl.. abort
        PostQuitMessage(-1);
      }
    } break;
    case WM_DESTROY: {
      PostQuitMessage(0);
    } break;
    default: {
      result = DefWindowProcA(hwnd, msg, wparam, lparam);
    } break;
  }
  return result;
}

bool WinQNesWindow::Create(HINSTANCE instance) {
  WNDCLASSEXA window_class = {};
  window_class.cbSize = sizeof(WNDCLASSEXA);
  window_class.style = CS_OWNDC;
  window_class.lpfnWndProc = WinQNESWindowProc;
  window_class.hInstance = instance;
  window_class.hCursor = LoadCursorA(nullptr, IDC_ARROW);  // NOLINT
  window_class.lpszClassName = "QNESWindowClass";

  // Registering window class
  if (RegisterClassExA(&window_class) == 0U) {
    // failed to register window -> return error
    return false;
  }

  RECT init_window_size;
  init_window_size.left = WinQNesWindow::DEFAULT_WINDOW_POS_X;
  init_window_size.right = WinQNesWindow::DEFAULT_WINDOW_POS_X + width;
  init_window_size.top = WinQNesWindow::DEFAULT_WINDOW_POS_Y;
  init_window_size.bottom = WinQNesWindow::DEFAULT_WINDOW_POS_Y + height;
  AdjustWindowRectEx(&init_window_size, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0);

  // Create window
  HWND window_handle = CreateWindowExA(
      0, window_class.lpszClassName, "QNES",
      (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) &
          ~WS_THICKFRAME,
      init_window_size.left, init_window_size.top,
      (init_window_size.right - init_window_size.left),
      (init_window_size.bottom - init_window_size.top), nullptr, nullptr,
      instance, this);

  if (window_handle == nullptr) {
    // failed to create window
    return false;
  }

  handle = window_handle;
  return true;
}

void WinQNesWindow::ProcessMessages() {  // NOLINT
  MSG msg;
  while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
    TranslateMessage(&msg);
    DispatchMessageA(&msg);
  }
}

}  // namespace QNes::platform::windows
