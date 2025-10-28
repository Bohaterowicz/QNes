#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class IncrementDecrementImpliedTest : public ::testing::TestWithParam<int> {
 public:
  IncrementDecrementImpliedTest() : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    initial_value = GetParam();
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::CPU cpu;

  int initial_value = 0;
};

TEST_P(IncrementDecrementImpliedTest, IncrementX_INX) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value + 1;

  QNes::CPU_Testing::SetX(cpu, start_value);

  // Simulate the instruction fetch cycle for INX
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::INX<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for INX (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.x, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest, IncrementY_INY) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value + 1;

  QNes::CPU_Testing::SetY(cpu, start_value);

  // Simulate the instruction fetch cycle for INY
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::INY<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for INY (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.y, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest, DecrementX_DEX) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value - 1;

  QNes::CPU_Testing::SetX(cpu, start_value);

  // Simulate the instruction fetch cycle for DEX
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::DEX<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for DEX (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.x, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest, DecrementY_DEY) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value - 1;

  QNes::CPU_Testing::SetY(cpu, start_value);

  // Simulate the instruction fetch cycle for DEY
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::DEY<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for DEY (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.y, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

INSTANTIATE_TEST_SUITE_P(IncrementDecrementOperations_Implied,
                         IncrementDecrementImpliedTest,
                         ::testing::Values(0, 1, 2, 0x42, 0x7F, 0x80, 254, 255,
                                           0xFF));
