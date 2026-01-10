#pragma once

#include <functional>
#include <utility>

#include "emulator_core/qnes_texture.hpp"
#include "renderer_constants.hpp"

namespace QNes::renderer {

struct RendererPlatformBackend {
  std::function<bool()> Initialize;
  std::function<void()> SwapBuffers;

  // ImGui backend
  std::function<void()> InterfaceNewFrame;
  std::function<std::string()> OpenFileDialog;
};

class Renderer {
 public:
  Renderer(RendererPlatformBackend platform_backend, size_t width,
           size_t height) noexcept
      : pattern_table(128, 128),
        platform_backend(std::move(platform_backend)) {}
  ~Renderer() noexcept = default;

  Renderer(const Renderer &) noexcept = delete;
  Renderer &operator=(const Renderer &) noexcept = delete;
  Renderer(Renderer &&) noexcept = delete;
  Renderer &operator=(Renderer &&) noexcept = delete;

  bool Initialize(NESTexture *framebuffer) noexcept;
  void DrawFrameBuffer(const QNes::NESTexture &framebuffer) noexcept;
  void DrawInterface() noexcept;
  void SwapBuffers() noexcept;

  [[nodiscard]] const RendererPlatformBackend &GetPlatformBackend()
      const noexcept {
    return platform_backend;
  }

  QNes::NESTexture pattern_table;
  // QNes::NESTexture *framebuffer = nullptr;

 private:
  RendererPlatformBackend platform_backend;

  u32 frame_texture_shader_program = INVALID_OPENGL_ID;
  u32 frame_quad_vao = INVALID_OPENGL_ID;
  u32 frame_quad_vbo = INVALID_OPENGL_ID;
};

}  // namespace QNes::renderer