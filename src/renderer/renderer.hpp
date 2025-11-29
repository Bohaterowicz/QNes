#pragma once

#include <functional>
#include <limits>
#include <utility>

#include "emulator_core/qnes_frame_buffer.hpp"

namespace QNES::renderer {

constexpr u32 INVALID_OPENGL_ID{(std::numeric_limits<u32>::max)()};

struct RendererPlatformBackend {
  std::function<bool()> Initialize;
  std::function<void()> SwapBuffers;

  // ImGui backend
  std::function<void()> InterfaceNewFrame;
};

class Renderer {
 public:
  Renderer(RendererPlatformBackend platform_backend, size_t width,
           size_t height) noexcept
      : platform_backend(std::move(platform_backend)),
        framebuffer_texture_width(width),
        framebuffer_texture_height(height) {}
  ~Renderer() noexcept = default;

  Renderer(const Renderer &) noexcept = delete;
  Renderer &operator=(const Renderer &) noexcept = delete;
  Renderer(Renderer &&) noexcept = delete;
  Renderer &operator=(Renderer &&) noexcept = delete;

  bool Initialize() noexcept;
  void DrawFrameBuffer(const NESFrameBuffer &framebuffer) noexcept;
  void DrawInterface() noexcept;
  void SwapBuffers() noexcept;

 private:
  RendererPlatformBackend platform_backend;

  size_t framebuffer_texture_width = 0;
  size_t framebuffer_texture_height = 0;
  u32 framebuffer_texture_glid = INVALID_OPENGL_ID;
  u32 frame_texture_shader_program = INVALID_OPENGL_ID;
  u32 frame_quad_vao = INVALID_OPENGL_ID;
  u32 frame_quad_vbo = INVALID_OPENGL_ID;
};

}  // namespace QNES::renderer