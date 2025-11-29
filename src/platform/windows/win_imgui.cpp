#include "win_imgui.hpp"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

namespace QNES::platform::windows {

bool WinInitializeImGui(HWND window_handle) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(window_handle);

  const char* glsl_version = "#version 330 core";
  ImGui_ImplOpenGL3_Init(glsl_version);
  return true;
}

void WinImGuiNewFrame() { ImGui_ImplWin32_NewFrame(); }

}  // namespace QNES::platform::windows