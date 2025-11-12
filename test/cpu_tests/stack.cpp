#include <gtest/gtest.h>

#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"

class CPUStackTest : public ::testing::Test {
 public:
  CPUStackTest() : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    cpu.Reset();
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    QNes::CPU_Testing::ExecuteReset(cpu);
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;
};

TEST_F(CPUStackTest, InitialSPValue) {
  // Arrange & Act
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Assert
  EXPECT_EQ(initial_sp, 0xFD);  // Stack Pointer should initialize to 0xFD
}

TEST_F(CPUStackTest, IncrementSP) {
  // Arrange
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Act
  QNes::CPU_Testing::IncrementSP(cpu);
  auto state_after_increment = cpu.GetState();
  u8 sp_after_increment = state_after_increment.sp;

  // Assert
  EXPECT_EQ(sp_after_increment, static_cast<u8>((initial_sp + 1) & 0xFF));
}

TEST_F(CPUStackTest, DecrementSP) {
  // Arrange
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Act
  QNes::CPU_Testing::DecrementSP(cpu);
  auto state_after_decrement = cpu.GetState();
  u8 sp_after_decrement = state_after_decrement.sp;

  // Assert
  EXPECT_EQ(sp_after_decrement, static_cast<u8>((initial_sp - 1) & 0xFF));
}

TEST_F(CPUStackTest, WrapAroundSP) {
  // Arrange
  QNes::CPU_Testing::SetSP(cpu, 0x00);  // Set SP to 0x00

  // Act
  QNes::CPU_Testing::DecrementSP(cpu);  // Decrement should wrap around
  auto state_after_decrement = cpu.GetState();
  u8 sp_after_decrement = state_after_decrement.sp;

  // Assert
  EXPECT_EQ(sp_after_decrement, 0xFF);  // SP should wrap around to 0xFF
}

TEST_F(CPUStackTest, PushAndPopValue) {
  // Arrange
  const u8 test_value = 0x42;
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Act
  QNes::CPU_Testing::PushStack(cpu, test_value);
  auto state_after_push = cpu.GetState();
  u8 sp_after_push = state_after_push.sp;
  u8 popped_value = QNes::CPU_Testing::PopStack(cpu);
  auto state_after_pop = cpu.GetState();
  u8 sp_after_pop = state_after_pop.sp;

  // Assert
  EXPECT_EQ(sp_after_push,
            initial_sp - 1);  // SP should decrement by 1 after push
  EXPECT_EQ(popped_value,
            test_value);  // Popped value should match pushed value
  EXPECT_EQ(sp_after_pop,
            initial_sp);  // SP should return to initial value after pop
}

TEST_F(CPUStackTest, PushAndPop2) {
  // Arrange
  const u8 test_value1 = 0x42;
  const u8 test_value2 = 0x84;
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Act
  QNes::CPU_Testing::PushStack(cpu, test_value1);
  QNes::CPU_Testing::PushStack(cpu, test_value2);
  auto state_after_push = cpu.GetState();
  u8 sp_after_push = state_after_push.sp;
  u8 popped_value2 = QNes::CPU_Testing::PopStack(cpu);
  u8 popped_value1 = QNes::CPU_Testing::PopStack(cpu);
  auto state_after_pop = cpu.GetState();
  u8 sp_after_pop = state_after_pop.sp;

  // Assert
  EXPECT_EQ(sp_after_push,
            initial_sp - 2);  // SP should decrement by 2 after two pushes
  EXPECT_EQ(popped_value2,
            test_value2);  // First popped value should match last pushed value
  EXPECT_EQ(
      popped_value1,
      test_value1);  // Second popped value should match first pushed value
  EXPECT_EQ(sp_after_pop,
            initial_sp);  // SP should return to initial value after pops
}
