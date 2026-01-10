#pragma once

#include <windows.h>

namespace QNes::platform::windows {
bool WinInitializeImGui(HWND window_handle);
void WinImGuiNewFrame();

}  // namespace QNes::platform::windows