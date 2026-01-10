#pragma once

#include <memory>

#include "qnes_c.hpp"

namespace QNes {

class NESTexture {
 public:
  NESTexture() noexcept : NESTexture(256, 240) {}

  NESTexture(size_t width, size_t height) noexcept
      : width(width), height(height), size(width * height * 4) {
    framebuffer = std::make_unique<u8[]>(size);
    Clear();
  }
  ~NESTexture() noexcept = default;

  NESTexture(const NESTexture &) noexcept = delete;
  NESTexture &operator=(const NESTexture &) noexcept = delete;
  NESTexture(NESTexture &&) noexcept = default;
  NESTexture &operator=(NESTexture &&) noexcept = default;

  void Clear() { std::memset(framebuffer.get(), 0xFF, size); }

  [[nodiscard]] std::pair<size_t, size_t> GetDimensions() const {
    return {width, height};
  }

  [[nodiscard]] u8 *GetData() const { return framebuffer.get(); }

  void SetGLID(u32 id) { glid = id; }
  [[nodiscard]] u32 GetGLID() const { return glid; }

 private:
  std::unique_ptr<u8[]> framebuffer;
  size_t width = 256;
  size_t height = 240;
  size_t size = width * height * 4;

  u32 glid = 0xFFFFFFFF;
};
}  // namespace QNes