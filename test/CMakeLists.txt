include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
  qnes_test
  qnes_test.cpp
  cpu_tests/isa_absolute.cpp
  cpu_tests/isa_immediate.cpp
  cpu_tests/isa_implied.cpp
  cpu_tests/isa_zeropage.cpp
  cpu_tests/isa_zeropage_indexed.cpp
  cpu_tests/isa_absolute_indexed.cpp
  cpu_tests/isa_indirect_indexed.cpp
  cpu_tests/stack.cpp
  cpu_tests/isa_stack.cpp
  cpu_tests/isa_indirect.cpp
  cpu_tests/isa_relative.cpp
  cpu_tests/reset.cpp
  cpu_tests/isa_register_transfer.cpp
  nes_main/nes_memory_mirroring.cpp
  nes_main/nes_ppu_register_mirroring.cpp
  nes_main/nes_ppu_registers.cpp)

# Klaus 6502 functional test - standalone executable
add_executable(qnes_functional_test test_roms/cpu_functional_test.cpp)
target_link_libraries(qnes_functional_test qnes_lib)

# Copy ROM binary to build directory
set(ROM_BINARY_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/test_roms/6502_functional_tests.bin")
set(ROM_BINARY_DEST
    "${CMAKE_BINARY_DIR}/bin/test_roms/6502_functional_tests.bin")

# Create the destination directory and copy the file
add_custom_command(
  TARGET qnes_functional_test
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/test_roms"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ROM_BINARY_SOURCE}"
          "${ROM_BINARY_DEST}"
  COMMENT "Copying 6502_functional_tests.bin to build directory")

target_link_libraries(qnes_test qnes_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(qnes_test)
enable_testing()
