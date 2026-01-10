#include "renderer.hpp"

#include <GL/glew.h>

#include <format>

#include "draw_interface.hpp"

namespace QNes::renderer {

static constexpr f32 quadVertices[] = {
    // Positions (loc 0) // TexCoords (loc 1)
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // Top Left
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Bottom Left
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom Right

    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // Top Left
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom Right
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f   // Top Right
};

static constexpr const char *frame_texture_shader_vertex_source = R"(

#version 330 core
layout (location = 0) in vec3 aPos;     // Input vertex position (usually screen space quad)
layout (location = 1) in vec2 aTexCoord; // Input texture coordinates

out vec2 TexCoord; // Output to Fragment Shader

void main()
{
    gl_Position = vec4(aPos, 1.0); // Directly use the input position (assuming NDC)
    TexCoord = aTexCoord;        // Pass texture coordinates
}

)";

static constexpr const char *frame_texture_shader_fragment_source = R"(

#version 330 core
out vec4 FragColor;

in vec2 TexCoord; // Input from Vertex Shader

uniform sampler2D ourTexture; // The texture sampler (uniform variable)

void main()
{
    // Sample the texture at the interpolated coordinate and output the color
    FragColor = texture(ourTexture, TexCoord);
}

)";

void CreateOpenGLTexture(u32 &texture_glid, size_t width, size_t height) {
  texture_glid = INVALID_OPENGL_ID;
  glCreateTextures(GL_TEXTURE_2D, 1, &texture_glid);
  glTextureStorage2D(texture_glid, 1, GL_RGBA8, (GLsizei)width,
                     (GLsizei)height);
  glTextureParameteri(texture_glid, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(texture_glid, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(texture_glid, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(texture_glid, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

bool CreateFrameTextureShader(u32 &frame_texture_shader_program) {
  frame_texture_shader_program = glCreateProgram();
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertexShader, 1, &frame_texture_shader_vertex_source, nullptr);
  glShaderSource(fragmentShader, 1, &frame_texture_shader_fragment_source,
                 nullptr);
  glCompileShader(vertexShader);
  GLint success = GL_FALSE;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    DBG_PRINT(
        std::format("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog));
    return false;
  }
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    DBG_PRINT(std::format("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}",
                          infoLog));
    return false;
  }
  glAttachShader(frame_texture_shader_program, vertexShader);
  glAttachShader(frame_texture_shader_program, fragmentShader);
  glLinkProgram(frame_texture_shader_program);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(frame_texture_shader_program);
  glGetProgramiv(frame_texture_shader_program, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    char infoLog[512];
    glGetProgramInfoLog(frame_texture_shader_program, 512, nullptr, infoLog);
    DBG_PRINT(
        std::format("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog));
    return false;
  }
  return true;
}

bool CreateFrameQuad(u32 &frame_quad_vao, u32 &frame_quad_vbo) {
  glGenVertexArrays(1, &frame_quad_vao);
  glGenBuffers(1, &frame_quad_vbo);
  glBindVertexArray(frame_quad_vao);
  glBindBuffer(GL_ARRAY_BUFFER, frame_quad_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
  return true;
}

bool Renderer::Initialize(NESTexture *framebuffer) noexcept {
  if (!platform_backend.Initialize()) {
    return false;
  }

  // Create and initialize framebuffer texture
  u32 framebuffer_texture_glid = INVALID_OPENGL_ID;
  u32 pattern_table_texture_glid = INVALID_OPENGL_ID;
  auto [framebuffer_width, framebuffer_height] = framebuffer->GetDimensions();
  auto [pattern_table_width, pattern_table_height] =
      pattern_table.GetDimensions();
  CreateOpenGLTexture(framebuffer_texture_glid, framebuffer_width,
                      framebuffer_height);

  CreateOpenGLTexture(pattern_table_texture_glid, pattern_table_width,
                      pattern_table_height);

  if (framebuffer_texture_glid == INVALID_OPENGL_ID ||
      pattern_table_texture_glid == INVALID_OPENGL_ID) {
    return false;
  }

  framebuffer->SetGLID(framebuffer_texture_glid);
  pattern_table.SetGLID(pattern_table_texture_glid);

  if (!CreateFrameTextureShader(frame_texture_shader_program)) {
    return false;
  }

  if (!CreateFrameQuad(frame_quad_vao, frame_quad_vbo)) {
    return false;
  }
  return true;
}

void Renderer::DrawFrameBuffer(const QNes::NESTexture &framebuffer) noexcept {
  auto [width, height] = framebuffer.GetDimensions();
  glTextureSubImage2D(framebuffer.GetGLID(), 0, 0, 0, (GLsizei)width,
                      (GLsizei)height, GL_RGBA, GL_UNSIGNED_BYTE,
                      framebuffer.GetData());
  glUseProgram(frame_texture_shader_program);
  GLint textureLocation =
      glGetUniformLocation(frame_texture_shader_program, "ourTexture");
  glUniform1i(textureLocation,
              0);  // Tell the sampler 'ourTexture' to use texture unit 0
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTextureUnit(0, framebuffer.GetGLID());
  glBindVertexArray(frame_quad_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  glUseProgram(0);
}
void Renderer::DrawInterface() noexcept {
  InterfaceNewFrame(*this);
  InterfaceDrawBuild(*this);
  InterfaceRender();
}
void Renderer::SwapBuffers() noexcept { platform_backend.SwapBuffers(); }
}  // namespace QNes::renderer
