#pragma once
#include <Windows.h>
#include <windef.h>

#include "qnes_c.hpp"

namespace QNes::platform::windows {

// Opengl init parameters

// OpenGL major version
static constexpr i32 OPENGL_DESIRED_MAJOR_VERION = 4;
// OpenGL minor version
static constexpr i32 OPENGL_DESIRED_MINOR_VERION = 5;

enum class OpenGLDebugLevel : u8 {
  None = 0,
  ErrorsOnly = 1,         // High severity only
  WarningsAndErrors = 2,  // High + Medium
  All = 3                 // All severities
};

bool WinInitializeGLEW();

void SetOpenGLDebugLevel(OpenGLDebugLevel level);

[[nodiscard]] bool WinInitializeOpenGL(
    HWND window_handle, i32 opengl_major_version = OPENGL_DESIRED_MAJOR_VERION,
    i32 opengl_minor_version = OPENGL_DESIRED_MINOR_VERION);

void WinSetVSync(bool vsync);

}  // namespace QNes::platform::windows