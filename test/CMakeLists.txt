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
  cpu_tests/isa_register_transfer.cpp)

target_link_libraries(qnes_test qnes_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(qnes_test)
enable_testing()
