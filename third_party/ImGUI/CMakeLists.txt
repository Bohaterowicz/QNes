include(FetchContent)

FetchContent_Declare(
  imgui_lib
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90)

FetchContent_MakeAvailable(imgui_lib)

if(NOT EXISTS ${imgui_lib_SOURCE_DIR})
  message(
    FATAL_ERROR
      "FetchContent failed to download the ImGui source files. "
      "Check network connection, proxy settings, and the repository URL.")
endif()

set(IMGUI_INCLUDE_DIRS
    "${imgui_lib_SOURCE_DIR};${imgui_lib_SOURCE_DIR}/backends"
    CACHE STRING "ImGUI include directories" FORCE)

set(IMGUI_SOURCES
    ${imgui_lib_SOURCE_DIR}/imgui.cpp
    ${imgui_lib_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_lib_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_lib_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_lib_SOURCE_DIR}/imgui_demo.cpp
    CACHE STRING "ImGUI source files" FORCE)

set(IMGUI_SOURCES_API_BACKENDS
    ${imgui_lib_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    CACHE STRING "ImGUI API backend source files" FORCE)

# platfrom backend - initialize to not specified
set(IMGUI_SOURCES_PLATFORM_BACKENDS
    "NOT SPECIFIED"
    CACHE STRING "ImGUI platform backend source files" FORCE)
if(WIN32)
  # if compiling for windows, add the win32 backend
  set(IMGUI_SOURCES_PLATFORM_BACKENDS
      ${imgui_lib_SOURCE_DIR}/backends/imgui_impl_win32.cpp
      CACHE STRING "Windows ImGUI platform backend source files" FORCE)
  # if compiling for oter platform raise error (for now only windows is
  # supported)
else()
  message(FATAL_ERROR "Only Windows is supported for now")
endif()

add_library(imgui_lib STATIC ${IMGUI_SOURCES} ${IMGUI_SOURCES_API_BACKENDS})
target_include_directories(imgui_lib PUBLIC ${IMGUI_INCLUDE_DIRS})
target_compile_definitions(imgui_lib PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
target_compile_definitions(imgui_lib PUBLIC IMGUI_DISABLE_OBSOLETE_KEYIO)
target_compile_definitions(imgui_lib PUBLIC IMGUI_DISABLE_OBSOLETE_KEYIO)
