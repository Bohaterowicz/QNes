#pragma once
#include <Windows.h>
#include <handleapi.h>
#include <windef.h>

#include <string>

#include "qnes_c.hpp"

namespace QNES::platform::windows {
class WinQNESWindow {
 public:
  static PIXELFORMATDESCRIPTOR GetDefaultPixelFormat();

  static constexpr i32 DEFAULT_WINDOW_WIDHT = 800;
  static constexpr i32 DEFAULT_WINDOW_HEIGHT = 600;
  static constexpr i32 DEFAULT_WINDOW_POS_X = 600;
  static constexpr i32 DEFAULT_WINDOW_POS_Y = 200;

 public:
  WinQNESWindow(std::string_view name) : name{name} {}

  WinQNESWindow &SetSize(i32 width, i32 height) {
    this->width = width;
    this->height = height;
    return *this;
  }

  WinQNESWindow &SetPixelFormat(
      const PIXELFORMATDESCRIPTOR &pixel_format_desc) {
    this->pixel_format_desc = pixel_format_desc;
    return *this;
  }

  bool Create(HINSTANCE instance);
  void ProcessMessages();

  [[nodiscard]] const PIXELFORMATDESCRIPTOR &GetPixelFormat() const {
    return pixel_format_desc;
  }

  [[nodiscard]] std::pair<i32, i32> GetDimensions() const {
    return {width, height};
  }

  void Shutdown() { shutdown_requested = true; }
  [[nodiscard]] bool ShutdownRequested() const { return shutdown_requested; }

  [[nodiscard]] HWND GetHandle() const { return handle; }

 private:
  std::string name;
  HWND handle = nullptr;
  i32 width = DEFAULT_WINDOW_WIDHT;
  i32 height = DEFAULT_WINDOW_HEIGHT;
  PIXELFORMATDESCRIPTOR pixel_format_desc = {};

  bool shutdown_requested = false;
};
}  // namespace QNES::platform::windows