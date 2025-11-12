set(QNES_SOURCES qnes_cpu.cpp qnes_emu.cpp cpu_isa.cpp qnes_bus.cpp
                 qnes_ppu.cpp)

add_library(qnes_lib STATIC ${QNES_SOURCES})

target_include_directories(qnes_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(qnes qnes_main.cpp)

target_link_libraries(qnes PRIVATE qnes_lib)
