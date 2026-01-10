#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include "qnes_c.hpp"

namespace QNes::platform {

struct ReadFileResult {
  std::unique_ptr<u8[]> data = nullptr;
  size_t size = 0;
};

struct PlatformServicesFunctions {
  std::function<std::string()> OpenFileDialog;
  std::function<ReadFileResult(std::string_view filename)> ReadFile;
};

class PlatformServices {
 public:
  static PlatformServices& Get() {
    auto& instance = GetInternal();
    if (!instance.initialized) {
      throw std::runtime_error("PlatformServices not initialized");
    }
    return instance;
  }

  static void Initialize(PlatformServicesFunctions functions) {
    auto& instance = GetInternal();
    if (instance.initialized) {
      return;
    }
    instance.initialized = true;
    instance.functions = std::move(functions);
  }

  PlatformServices(const PlatformServices&) noexcept = delete;
  PlatformServices& operator=(const PlatformServices&) noexcept = delete;
  PlatformServices(PlatformServices&&) noexcept = delete;
  PlatformServices& operator=(PlatformServices&&) noexcept = delete;
  PlatformServicesFunctions functions;

  [[nodiscard]] std::string OpenFileDialog() const {
    return functions.OpenFileDialog();
  }
  [[nodiscard]] ReadFileResult ReadFile(std::string_view filename) const {
    return functions.ReadFile(filename);
  }

 private:
  PlatformServices() noexcept = default;
  ~PlatformServices() noexcept = default;
  bool initialized = false;
  static PlatformServices& GetInternal() {
    static PlatformServices instance;
    return instance;
  }
};

}  // namespace QNes::platform