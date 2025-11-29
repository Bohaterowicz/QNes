set(RENDERER_SOURCES renderer.cpp interface.cpp)

add_library(qnes_renderer_lib STATIC ${RENDERER_SOURCES}
                                     ${IMGUI_SOURCES_API_BACKENDS})

target_include_directories(
  qnes_renderer_lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src
         ${CMAKE_SOURCE_DIR}/src/common ${GLEW_INCLUDE_DIRS}
         ${IMGUI_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
target_link_libraries(qnes_renderer_lib PRIVATE ${GLEW_LIBRARIES} OpenGL::GL
                                                imgui_lib)
