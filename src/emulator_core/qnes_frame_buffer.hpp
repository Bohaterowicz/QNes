#pragma once

#include <memory>

#include "qnes_c.hpp"

namespace QNES {

class NESFrameBuffer {
 public:
  NESFrameBuffer() noexcept {
    framebuffer = std::make_unique<u8[]>(size);
    Clear();
  }
  ~NESFrameBuffer() noexcept = default;

  NESFrameBuffer(const NESFrameBuffer &) noexcept = delete;
  NESFrameBuffer &operator=(const NESFrameBuffer &) noexcept = delete;
  NESFrameBuffer(NESFrameBuffer &&) noexcept = delete;
  NESFrameBuffer &operator=(NESFrameBuffer &&) noexcept = delete;

  void Clear() { std::memset(framebuffer.get(), 128, size); }

  [[nodiscard]] std::pair<size_t, size_t> GetDimensions() const {
    return {width, height};
  }

  [[nodiscard]] u8 *GetData() const { return framebuffer.get(); }

 private:
  std::unique_ptr<u8[]> framebuffer;
  size_t width = 256;
  size_t height = 240;
  size_t size = width * height * 3;
};
}  // namespace QNES