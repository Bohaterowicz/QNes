set(GUI_SOURCES interface_controller.cpp)

add_library(qnes_gui_lib STATIC ${GUI_SOURCES})

target_include_directories(
  qnes_gui_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src
                      ${CMAKE_SOURCE_DIR}/src/common)

target_link_libraries(qnes_gui_lib PRIVATE qnes_renderer_lib qnes_lib)
