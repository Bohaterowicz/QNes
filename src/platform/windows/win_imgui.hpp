#pragma once

#include <windows.h>

namespace QNES::platform::windows {
bool WinInitializeImGui(HWND window_handle);
void WinImGuiNewFrame();

}  // namespace QNES::platform::windows