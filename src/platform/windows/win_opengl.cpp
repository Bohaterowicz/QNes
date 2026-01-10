#include "win_opengl.hpp"

#include <array>
#include <string>

#include "GL/glew.h"
#include "GL/wglew.h"
#include "qnes_c.hpp"

namespace QNes::platform::windows {

bool IsRenderDocAttached() {
  auto *render_doc_module = GetModuleHandleA("renderdoc.dll");
  if (render_doc_module != nullptr) {
    DBG_PRINT("RenderDoc Attached!");
  }
  return render_doc_module != nullptr;
}

bool WinInitializeGLEW() {
  // Initialize GLEW for this context
  GLenum glew_result = glewInit();
  if (glew_result != GLEW_OK) {
    DBG_PRINT(std::format(
        "Failed to initialize GLEW: {}",
        reinterpret_cast<const char *>(glewGetErrorString(glew_result))));
    return false;
  }
  DBG_PRINT("GLEW initialized successfully");
  return true;
}

// Function to set debug level
void SetOpenGLDebugLevel(OpenGLDebugLevel level) {
  if (!GLEW_KHR_debug) {
    return;
  }

  // Disable all first
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_FALSE);

  switch (level) {
    case OpenGLDebugLevel::None:
      DBG_PRINT("OpenGL debug output disabled");
      break;

    case OpenGLDebugLevel::ErrorsOnly:
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH,
                            0, nullptr, GL_TRUE);
      DBG_PRINT("OpenGL debug output: Errors only");
      break;

    case OpenGLDebugLevel::WarningsAndErrors:
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH,
                            0, nullptr, GL_TRUE);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                            GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
      DBG_PRINT("OpenGL debug output: Warnings and Errors");
      break;

    case OpenGLDebugLevel::All:
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                            nullptr, GL_TRUE);
      DBG_PRINT("OpenGL debug output: All messages");
      break;
  }
}

void GLAPIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar *message,
                                    const void *userParam) {
  // Convert source to string
  const char *source_str = "Unknown";
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      source_str = "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      source_str = "Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      source_str = "Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      source_str = "Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      source_str = "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      source_str = "Other";
      break;
    default:
      source_str = "Unknown";
      break;
  }

  // Convert type to string
  const char *type_str = "Unknown";
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      type_str = "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      type_str = "Deprecated";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      type_str = "Undefined Behavior";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      type_str = "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      type_str = "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      type_str = "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      type_str = "Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      type_str = "Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      type_str = "Other";
      break;
    default:
      type_str = "Unknown";
      break;
  }

  // Convert severity to string
  const char *severity_str = "Unknown";
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      severity_str = "High";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      severity_str = "Medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      severity_str = "Low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      severity_str = "Notification";
      break;
    default:
      severity_str = "Unknown";
      break;
  }

  // Log with categorization
  DBG_PRINT(std::format("OpenGL Debug [{}] [{}] [{}] (ID: {}): {}", source_str,
                        type_str, severity_str, id, message));
}

bool WinInitializeOpenGL(HWND window_handle, i32 opengl_major_version,
                         i32 opengl_minor_version) {
  bool result = false;

  auto is_render_doc_attached = IsRenderDocAttached();

  HDC window_dc = GetDC(window_handle);
  // Creat opengl context
  HGLRC opengl_rendering_context = wglCreateContext(window_dc);

  // Check if we got valid rendering context
  if (opengl_rendering_context != nullptr) {
    if (wglMakeCurrent(window_dc, opengl_rendering_context) != 0) {
      // initialize glew, if we fail, we should stop initialization of opengl,
      // as we need glew right now.
      if (WinInitializeGLEW()) {
        const u32 WGL_CONTEXT_ATTRIBS_COUNT = 9;
        // Context attribs, we specify the version we want to use...
        std::array<i32, WGL_CONTEXT_ATTRIBS_COUNT> wgl_context_attribs = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            opengl_major_version,
            WGL_CONTEXT_MINOR_VERSION_ARB,
            opengl_minor_version,
            WGL_CONTEXT_FLAGS_ARB,
            is_render_doc_attached
                ? 0
                : WGL_CONTEXT_DEBUG_BIT_ARB,  // Add debug flag
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0};

        // Check for "WGL_ARB_create_context" as we may want to specify our
        // own version of opengl
        if (wglewIsSupported("WGL_ARB_create_context") == GL_TRUE) {
          // Create context with specified attributes
          HGLRC opengl_rendering_context_arb = wglCreateContextAttribsARB(
              window_dc, nullptr, wgl_context_attribs.data());
          if (opengl_rendering_context_arb != nullptr) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(opengl_rendering_context);
            wglMakeCurrent(window_dc, opengl_rendering_context_arb);

            // Re-initialize GLEW for the new ARB context
            if (!WinInitializeGLEW()) {
              wglMakeCurrent(nullptr, nullptr);
              wglDeleteContext(opengl_rendering_context_arb);
              DBG_PRINT("Failed to initialize GLEW for ARB context");
              return false;
            }
          }
        } else {
          DBG_PRINT(
              "----------> wglCreateContextAtribs not specified (WARNING)");
        }
        // We scucceded in initializing opengl in our window
        // Print out opengl version...
        const auto *gl_version = glGetString(GL_VERSION);
        std::string ogl_text("OpenGL VERSION: ");
        std::string gl_version_str(reinterpret_cast<const char *>(gl_version));
        DBG_PRINT(ogl_text + gl_version_str + "\n");

        // NOTE: IMPORTANT: Activates vsync (swap buffers on vertical blank)!
        wglSwapIntervalEXT(1);

        // After GLEW initialization, enable debug output
        if (GLEW_KHR_debug && !is_render_doc_attached) {
          glEnable(GL_DEBUG_OUTPUT);
          glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
          glDebugMessageCallback(OpenGLDebugCallback, nullptr);

          // Enable all messages for now
          SetOpenGLDebugLevel(OpenGLDebugLevel::WarningsAndErrors);

          DBG_PRINT("OpenGL debug output enabled with full message control");
        } else {
          DBG_PRINT("OpenGL debug output not supported");
        }

        // After context creation, verify debug context
        GLint context_flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
        if ((context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0) {
          DBG_PRINT("OpenGL debug context created successfully");
        } else {
          DBG_PRINT("Warning: OpenGL debug context not available");
        }

        result = TRUE;
      } else {
        // deinitialize the current rendering context
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(opengl_rendering_context);
        DBG_PRINT("GLEW could not be initialized... (ABORTING)");
      }
    }
  }
  ReleaseDC(window_handle, window_dc);
  return result;
}

void WinSetVSync(bool vsync) { wglSwapIntervalEXT(vsync ? 1 : 0); }

}  // namespace QNes::platform::windows
