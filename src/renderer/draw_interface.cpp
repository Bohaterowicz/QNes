#include "draw_interface.hpp"

#include "emulator_core/qnes_cartridge.hpp"
#include "emulator_core/qnes_emu.hpp"
#include "gui/interface_controller.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "platform/common/platform_services.hpp"
#include "qnes_c.hpp"

namespace QNes::renderer {

void InterfaceNewFrame(Renderer& renderer) {
  renderer.GetPlatformBackend().InterfaceNewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
}

void InterfaceRender() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void InterfaceDrawBuild(Renderer& renderer) {
  auto& ic = gui::InterfaceController::Get();
  if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
    ic.ToggleMainMenuBar();
  }

  if (ic.IsMainMenuBarVisible()) {
    if (ImGui::BeginMainMenuBar()) {
      const char* buttonLabel = "Load NES ROM...";

      if (ImGui::Button(buttonLabel)) {
        std::string filename =
            platform::PlatformServices::Get().OpenFileDialog();

        if (!filename.empty()) {
          DBG_PRINT(std::format("Loading file: {}", filename));
          auto file = platform::PlatformServices::Get().ReadFile(filename);
          QNes::Emulator::Get().SetCartridge(
              LoadCartridge(
                  DataBuffer{.data = std::move(file.data), .size = file.size})
                  .value());
        }
      }

      if (ImGui::Button("Show Pattern Table")) {
        if (!ic.IsPatternTableVisible()) {
          DecodePatternTable(QNes::Emulator::Get().GetCartridge(),
                             renderer.pattern_table, 0);
          ic.TogglePatternTable();
        } else {
          ic.TogglePatternTable();
        }
      }
      ImGui::EndMainMenuBar();
    }
  }

  if (ic.IsPatternTableVisible()) {
    renderer.DrawFrameBuffer(renderer.pattern_table);
  }
}
}  // namespace QNes::renderer