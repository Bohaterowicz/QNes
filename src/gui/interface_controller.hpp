#pragma once

#include "renderer/renderer.hpp"

namespace QNes::gui {

class InterfaceController {
 public:
  static InterfaceController& Get() {
    auto& instance = GetInternal();
    if (!instance.initialized) {
      throw std::runtime_error("PlatformServices not initialized");
    }
    return instance;
  }

  static void Initialize(QNes::renderer::Renderer* renderer) {
    auto& instance = GetInternal();
    if (instance.initialized) {
      return;
    }
    instance.initialized = true;
    instance.renderer = renderer;
  }

  InterfaceController(const InterfaceController&) noexcept = delete;
  InterfaceController& operator=(const InterfaceController&) noexcept = delete;
  InterfaceController(InterfaceController&&) noexcept = delete;
  InterfaceController& operator=(InterfaceController&&) noexcept = delete;

  void ToggleMainMenuBar() { show_main_menu_bar = !show_main_menu_bar; }
  [[nodiscard]] bool IsMainMenuBarVisible() const { return show_main_menu_bar; }

  void TogglePatternTable() { show_pattern_table = !show_pattern_table; }
  [[nodiscard]] bool IsPatternTableVisible() const {
    return show_pattern_table;
  }

 private:
  InterfaceController() noexcept = default;
  ~InterfaceController() noexcept = default;
  bool initialized = false;
  static InterfaceController& GetInternal() {
    static InterfaceController instance;
    return instance;
  }

  QNes::renderer::Renderer* renderer = nullptr;

  bool show_main_menu_bar = true;
  bool show_pattern_table = false;
};

}  // namespace QNes::gui