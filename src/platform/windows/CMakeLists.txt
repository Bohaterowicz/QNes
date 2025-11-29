# Windows platform executable
set(WINDOWS_SOURCES win_main.cpp win_window.cpp win_opengl.cpp win_imgui.cpp)

find_package(OpenGL REQUIRED)

message(
  STATUS "IMGUI_SOURCES_PLATFORM_BACKENDS: ${IMGUI_SOURCES_PLATFORM_BACKENDS}")

add_executable(qnes_windows ${WINDOWS_SOURCES}
                            ${IMGUI_SOURCES_PLATFORM_BACKENDS})
target_compile_definitions(qnes_windows PUBLIC GLEW_STATIC)

target_include_directories(
  qnes_windows
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/src/common ${GLEW_INCLUDE_DIRS}
          ${IMGUI_INCLUDE_DIRS})

# Link with unified qnes libraries
target_link_libraries(qnes_windows PRIVATE qnes_lib qnes_renderer_lib imgui_lib)

# Link with GLEW and OpenGL libraries
target_link_libraries(qnes_windows PRIVATE ${GLEW_LIBRARIES} OpenGL::GL
                                           OpenGL::GLU)

# Windows-specific settings Set subsystem to Windows (not console)
set_target_properties(qnes_windows PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(
  qnes_windows
  PRIVATE kernel32
          user32
          gdi32
          winspool
          shell32
          ole32
          oleaut32
          uuid
          comdlg32
          advapi32
          winmm
          xinput)
