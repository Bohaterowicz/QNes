#include "interface.hpp"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

namespace QNES::renderer {

void InterfaceNewFrame(RendererPlatformBackend &backend) {
  backend.InterfaceNewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
}

void InterfaceRender() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void InterfaceDrawDemoWindow() { ImGui::ShowDemoWindow(); }
}  // namespace QNES::renderer