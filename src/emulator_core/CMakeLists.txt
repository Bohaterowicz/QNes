set(QNES_SOURCES
    qnes_cpu.cpp
    qnes_emu.cpp
    cpu_isa.cpp
    qnes_bus.cpp
    qnes_ppu.cpp
    qnes_cartridge.cpp
    qnes_mapper.cpp
    dma_controller.cpp)

add_library(qnes_lib STATIC ${QNES_SOURCES})

target_include_directories(qnes_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
                                           ${CMAKE_SOURCE_DIR}/src/common)
