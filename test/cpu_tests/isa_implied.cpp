#include <gtest/gtest.h>

#include <vector>

#include "cpu_isa.hpp"
#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"


class IncrementDecrementImpliedTest : public ::testing::TestWithParam<int> {
 public:
  IncrementDecrementImpliedTest()
      : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    initial_value = GetParam();
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;

  int initial_value = 0;
};

// Test class for flag manipulation instructions
class FlagManipulationTest : public ::testing::Test {
 public:
  FlagManipulationTest() : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;
};

// Test class for NOP, RTI, and BRK instructions
class SpecialInstructionsTest : public ::testing::Test {
 public:
  SpecialInstructionsTest() : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;
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

TEST_P(IncrementDecrementImpliedTest, ShiftAccumulatorLeft_ASL_Implied) {
  // Arrange
  const u8 start_value = static_cast<u8>(initial_value);
  const u8 expected_result = static_cast<u8>((start_value << 1) & 0xFF);
  const bool expected_carry = (start_value & 0x80) != 0;

  QNes::CPU_Testing::SetA(cpu, start_value);

  // Simulate the instruction fetch cycle for ASL A (implied)
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ASL<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for ASL A (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

INSTANTIATE_TEST_SUITE_P(IncrementDecrementOperations_Implied,
                         IncrementDecrementImpliedTest,
                         ::testing::Values(0, 1, 2, 0x42, 0x7F, 0x80, 254, 255,
                                           0xFF));

TEST_P(IncrementDecrementImpliedTest, ShiftAccumulatorRight_LSR_Implied) {
  // Arrange
  const u8 start_value = static_cast<u8>(initial_value);
  const u8 expected_result = static_cast<u8>((start_value >> 1) & 0xFF);
  const bool expected_carry = (start_value & 0x01) != 0;

  QNes::CPU_Testing::SetA(cpu, start_value);

  // Simulate the instruction fetch cycle for LSR A (implied)
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::LSR<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for LSR A (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest,
       RotateAccumulatorLeft_ROL_Implied_NoCarry) {
  // Arrange
  const u8 start_value = static_cast<u8>(initial_value);
  QNes::CPU_Testing::SetA(cpu, start_value);
  QNes::CPU_Testing::SetCarry(cpu, false);
  const u8 expected_result = static_cast<u8>(((start_value << 1) | 0) & 0xFF);
  const bool expected_carry = (start_value & 0x80) != 0;

  // Simulate the instruction fetch cycle for ROL A (implied)
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROL<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest, RotateAccumulatorLeft_ROL_Implied_Carry) {
  // Arrange
  const u8 start_value = static_cast<u8>(initial_value);
  QNes::CPU_Testing::SetA(cpu, start_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  const u8 expected_result = static_cast<u8>(((start_value << 1) | 1) & 0xFF);
  const bool expected_carry = (start_value & 0x80) != 0;

  // Simulate the instruction fetch cycle for ROL A (implied)
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROL<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest,
       RotateAccumulatorRight_ROR_Implied_NoCarry) {
  // Arrange
  const u8 start_value = static_cast<u8>(initial_value);
  QNes::CPU_Testing::SetA(cpu, start_value);
  QNes::CPU_Testing::SetCarry(cpu, false);
  const u8 expected_result =
      static_cast<u8>(((start_value >> 1) | 0x00) & 0xFF);
  const bool expected_carry = (start_value & 0x01) != 0;

  // Simulate the instruction fetch cycle for ROR A (implied)
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROR<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementImpliedTest,
       RotateAccumulatorRight_ROR_Implied_Carry) {
  // Arrange
  const u8 start_value = static_cast<u8>(initial_value);
  QNes::CPU_Testing::SetA(cpu, start_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  const u8 expected_result =
      static_cast<u8>(((start_value >> 1) | 0x80) & 0xFF);
  const bool expected_carry = (start_value & 0x01) != 0;

  // Simulate the instruction fetch cycle for ROR A (implied)
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROR<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearCarry_CLC_FlagSet) {
  // Arrange - Set carry flag
  QNes::CPU_Testing::SetCarry(cpu, true);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLC<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLC (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.carry, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearCarry_CLC_FlagAlreadyClear) {
  // Arrange - Carry flag already clear
  QNes::CPU_Testing::SetCarry(cpu, false);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLC<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLC (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.carry, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearDecimal_CLD_FlagSet) {
  // Arrange - Set decimal flag
  auto status = cpu.GetState().status;
  status.decimal_mode = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLD<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLD (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.decimal_mode, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearDecimal_CLD_FlagAlreadyClear) {
  // Arrange - Decimal flag already clear
  auto status = cpu.GetState().status;
  status.decimal_mode = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLD<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLD (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.decimal_mode, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearInterrupt_CLI_FlagSet) {
  // Arrange - Set interrupt disable flag
  auto status = cpu.GetState().status;
  status.interrupt_disable = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLI<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLI (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.interrupt_disable, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearInterrupt_CLI_FlagAlreadyClear) {
  // Arrange - Interrupt disable flag already clear
  auto status = cpu.GetState().status;
  status.interrupt_disable = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLI<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLI (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.interrupt_disable, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearOverflow_CLV_FlagSet) {
  // Arrange - Set overflow flag
  auto status = cpu.GetState().status;
  status.overflow = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLV<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLV (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.overflow, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, ClearOverflow_CLV_FlagAlreadyClear) {
  // Arrange - Overflow flag already clear
  auto status = cpu.GetState().status;
  status.overflow = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::CLV<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute CLV (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.overflow, false);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, SetCarry_SEC_FlagClear) {
  // Arrange - Carry flag clear
  QNes::CPU_Testing::SetCarry(cpu, false);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::SEC<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute SEC (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.carry, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, SetCarry_SEC_FlagAlreadySet) {
  // Arrange - Carry flag already set
  QNes::CPU_Testing::SetCarry(cpu, true);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::SEC<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute SEC (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.carry, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, SetDecimal_SED_FlagClear) {
  // Arrange - Decimal flag clear
  auto status = cpu.GetState().status;
  status.decimal_mode = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::SED<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute SED (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.decimal_mode, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, SetDecimal_SED_FlagAlreadySet) {
  // Arrange - Decimal flag already set
  auto status = cpu.GetState().status;
  status.decimal_mode = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::SED<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute SED (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.decimal_mode, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, SetInterrupt_SEI_FlagClear) {
  // Arrange - Interrupt disable flag clear
  auto status = cpu.GetState().status;
  status.interrupt_disable = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::SEI<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute SEI (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.interrupt_disable, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(FlagManipulationTest, SetInterrupt_SEI_FlagAlreadySet) {
  // Arrange - Interrupt disable flag already set
  auto status = cpu.GetState().status;
  status.interrupt_disable = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::SEI<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute SEI (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.interrupt_disable, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

// NOP (No Operation) Tests
TEST_F(SpecialInstructionsTest, NOP_DoesNotModifyRegisters) {
  // Arrange - Set all registers to known values
  QNes::CPU_Testing::SetA(cpu, 0x42);
  QNes::CPU_Testing::SetX(cpu, 0x84);
  QNes::CPU_Testing::SetY(cpu, 0x99);
  QNes::CPU_Testing::SetSP(cpu, 0xFD);
  QNes::CPU_Testing::SetCarry(cpu, true);

  constexpr u16 start_address = 0x1234;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::NOP<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute NOP (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert - All registers should remain unchanged
  EXPECT_EQ(cpu_state.a, 0x42);
  EXPECT_EQ(cpu_state.x, 0x84);
  EXPECT_EQ(cpu_state.y, 0x99);
  EXPECT_EQ(cpu_state.sp, 0xFD);
  EXPECT_EQ(cpu_state.status.carry, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(SpecialInstructionsTest, NOP_DoesNotModifyFlags) {
  // Arrange - Set all flags to specific values
  auto status = cpu.GetState().status;
  status.carry = true;
  status.zero = true;
  status.interrupt_disable = false;
  status.decimal_mode = true;
  status.overflow = true;
  status.negative = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::NOP<QNes::AddressingMode::Implied>::OPCODE);

  // Act - Execute NOP (2 cycles)
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert - All flags should remain unchanged
  EXPECT_EQ(cpu_state.status.carry, true);
  EXPECT_EQ(cpu_state.status.zero, true);
  EXPECT_EQ(cpu_state.status.interrupt_disable, false);
  EXPECT_EQ(cpu_state.status.decimal_mode, true);
  EXPECT_EQ(cpu_state.status.overflow, true);
  EXPECT_EQ(cpu_state.status.negative, true);
  EXPECT_EQ(cpu_state.pc, start_address + 1);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(SpecialInstructionsTest, NOP_AtVariousAddresses) {
  // Test NOP at different PC addresses
  std::vector<u16> test_addresses = {0x0000, 0x0100, 0x1234, 0x7FFF, 0xFFFF};

  for (u16 start_address : test_addresses) {
    QNes::CPU_Testing::SetPC(cpu, start_address);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    memory.Write(start_address,
                 QNes::ISA::NOP<QNes::AddressingMode::Implied>::OPCODE);

    // Act
    for (int cycle = 0; cycle < 2; ++cycle) {
      cpu.Step();
    }

    auto cpu_state = cpu.GetState();

    // Assert
    EXPECT_EQ(cpu_state.pc, static_cast<u16>(start_address + 1))
        << "Failed at address 0x" << std::hex << start_address;
    EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
  }
}

// RTI (Return from Interrupt) Tests
TEST_F(SpecialInstructionsTest, RTI_RestoresStatusAndPC) {
  // Arrange - Set up stack with saved status and PC
  const u8 saved_status = 0b10101010;  // Various flags set
  const u8 saved_pcl = 0x34;
  const u8 saved_pch = 0x12;
  const u16 saved_pc = (static_cast<u16>(saved_pch) << 8) | saved_pcl;

  // Set initial SP (stack grows downward)
  QNes::CPU_Testing::SetSP(cpu, 0xFA);

  // Push status, PCL, PCH onto stack (stack: SP+1 = status, SP+2 = PCL, SP+3 =
  // PCH)
  memory.Write(0x0100 + 0xFB, saved_status);
  memory.Write(0x0100 + 0xFC, saved_pcl);
  memory.Write(0x0100 + 0xFD, saved_pch);
  QNes::CPU_Testing::SetSP(cpu, 0xFA);  // SP points to the byte before status

  constexpr u16 start_address = 0x5678;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::RTI<QNes::AddressingMode::Implied>::OPCODE);

  // Set current status to different values to verify restoration
  auto current_status = cpu.GetState().status;
  current_status.status = 0x00;
  QNes::CPU_Testing::SetStatus(cpu, current_status);

  // Act - Execute RTI (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert - Status and PC should be restored
  EXPECT_EQ(cpu_state.status.status, saved_status);
  EXPECT_EQ(cpu_state.pc, saved_pc);
  EXPECT_EQ(cpu_state.sp, 0xFD);  // SP should be incremented 3 times
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(SpecialInstructionsTest, RTI_RestoresAllFlags) {
  // Arrange - Test restoration of each flag bit
  for (int test_status_int = 0; test_status_int < 256; ++test_status_int) {
    u8 test_status = static_cast<u8>(test_status_int);
    // Set up stack
    QNes::CPU_Testing::SetSP(cpu, 0xFA);
    memory.Write(0x0100 + 0xFB, test_status);
    memory.Write(0x0100 + 0xFC, 0x00);  // PCL
    memory.Write(0x0100 + 0xFD, 0x00);  // PCH

    QNes::CPU_Testing::SetPC(cpu, 0x0000);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    memory.Write(0x0000, QNes::ISA::RTI<QNes::AddressingMode::Implied>::OPCODE);

    // Act
    for (int cycle = 0; cycle < 6; ++cycle) {
      cpu.Step();
    }

    auto cpu_state = cpu.GetState();

    // Assert
    EXPECT_EQ(cpu_state.status.status, test_status)
        << "Failed to restore status 0x" << std::hex
        << static_cast<int>(test_status);
    EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);

    // Reset for next iteration
    memory.Clear();
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
  }
}

TEST_F(SpecialInstructionsTest, RTI_RestoresPCAtVariousAddresses) {
  // Test RTI with various PC values
  std::vector<u16> test_pcs = {0x0000, 0x0100, 0x1234, 0x7FFF, 0xFFFF};

  for (u16 saved_pc : test_pcs) {
    const u8 saved_pcl = QNes::U16Low(saved_pc);
    const u8 saved_pch = QNes::U16High(saved_pc);

    // Set up stack
    QNes::CPU_Testing::SetSP(cpu, 0xFA);
    memory.Write(0x0100 + 0xFB, 0x00);  // Status
    memory.Write(0x0100 + 0xFC, saved_pcl);
    memory.Write(0x0100 + 0xFD, saved_pch);

    QNes::CPU_Testing::SetPC(cpu, 0x0000);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    memory.Write(0x0000, QNes::ISA::RTI<QNes::AddressingMode::Implied>::OPCODE);

    // Act
    for (int cycle = 0; cycle < 6; ++cycle) {
      cpu.Step();
    }

    auto cpu_state = cpu.GetState();

    // Assert
    EXPECT_EQ(cpu_state.pc, saved_pc)
        << "Failed to restore PC 0x" << std::hex << saved_pc;
    EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
  }
}

TEST_F(SpecialInstructionsTest, RTI_DoesNotModifyRegisters) {
  // Arrange - Set registers to known values
  QNes::CPU_Testing::SetA(cpu, 0x42);
  QNes::CPU_Testing::SetX(cpu, 0x84);
  QNes::CPU_Testing::SetY(cpu, 0x99);

  // Set up stack
  QNes::CPU_Testing::SetSP(cpu, 0xFA);
  memory.Write(0x0100 + 0xFB, 0x00);  // Status
  memory.Write(0x0100 + 0xFC, 0x00);  // PCL
  memory.Write(0x0100 + 0xFD, 0x00);  // PCH

  QNes::CPU_Testing::SetPC(cpu, 0x0000);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(0x0000, QNes::ISA::RTI<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert - Registers should not be modified
  EXPECT_EQ(cpu_state.a, 0x42);
  EXPECT_EQ(cpu_state.x, 0x84);
  EXPECT_EQ(cpu_state.y, 0x99);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

// BRK (Break - Software Interrupt) Tests
TEST_F(SpecialInstructionsTest, BRK_PushesPCAndStatusToStack) {
  // Arrange
  constexpr u16 start_address = 0x1234;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetSP(cpu, 0xFD);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

  // Set status flags
  auto status = cpu.GetState().status;
  status.carry = true;
  status.zero = false;
  status.interrupt_disable = false;
  status.decimal_mode = true;
  status.overflow = false;
  status.negative = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  // Set up interrupt vector
  constexpr u16 interrupt_handler = 0xABCD;
  memory.Write(0xFFFE, QNes::U16Low(interrupt_handler));
  memory.Write(0xFFFF, QNes::U16High(interrupt_handler));

  // Act - Execute BRK (7 cycles)
  for (int cycle = 0; cycle < 7; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert - Check stack contents (BRK pushes PCH, PCL, then status)
  // PC after incrementing past BRK instruction is start_address + 2
  // PushStack writes to 0x0100 + current_sp, then decrements SP
  // With initial SP = 0xFD: PCH -> 0x01FD, PCL -> 0x01FC, Status -> 0x01FB
  const u16 pc_after_brk = start_address + 2;
  EXPECT_EQ(memory.Read(0x0100 + 0xFD),
            QNes::U16High(pc_after_brk));  // PCH pushed first at 0x01FD
  EXPECT_EQ(memory.Read(0x0100 + 0xFC),
            QNes::U16Low(pc_after_brk));  // PCL pushed second at 0x01FC

  // Status pushed with B flag and unused bit set
  u8 expected_status = status.status;
  expected_status |= (1 << 4);  // Set B flag (bit 4)
  expected_status |= (1 << 5);  // Set unused flag (bit 5)
  EXPECT_EQ(memory.Read(0x0100 + 0xFB), expected_status);  // Status at 0x01FB

  // SP should be decremented 3 times
  EXPECT_EQ(cpu_state.sp, 0xFA);

  // PC should jump to interrupt handler
  EXPECT_EQ(cpu_state.pc, interrupt_handler);

  // Interrupt disable flag should be set
  EXPECT_EQ(cpu_state.status.interrupt_disable, true);

  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(SpecialInstructionsTest, BRK_SetsBreakFlag) {
  // Arrange
  QNes::CPU_Testing::SetPC(cpu, 0x0000);
  QNes::CPU_Testing::SetSP(cpu, 0xFD);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(0x0000, QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

  // Clear break flag initially
  auto status = cpu.GetState().status;
  status.break_command = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  // Set up interrupt vector
  memory.Write(0xFFFE, 0x00);
  memory.Write(0xFFFF, 0x00);

  // Act
  for (int cycle = 0; cycle < 7; ++cycle) {
    cpu.Step();
  }

  // Assert - B flag and unused bit should be set in the pushed status
  // Status was pushed when SP was 0xFB, so it's at 0x01FB
  u8 pushed_status = memory.Read(0x0100 + 0xFB);
  EXPECT_TRUE((pushed_status & (1 << 4)) != 0);  // B flag should be set
  EXPECT_TRUE((pushed_status & (1 << 5)) != 0);  // Unused flag should be set
}

TEST_F(SpecialInstructionsTest, BRK_JumpsToInterruptVector) {
  // Test with various interrupt handler addresses
  std::vector<u16> test_handlers = {0x0000, 0x0100, 0x1234, 0x7FFF, 0xFFFF};

  for (u16 handler : test_handlers) {
    QNes::CPU_Testing::SetPC(cpu, 0x0000);
    QNes::CPU_Testing::SetSP(cpu, 0xFD);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    memory.Write(0x0000, QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

    // Set up interrupt vector
    memory.Write(0xFFFE, QNes::U16Low(handler));
    memory.Write(0xFFFF, QNes::U16High(handler));

    // Act
    for (int cycle = 0; cycle < 7; ++cycle) {
      cpu.Step();
    }

    auto cpu_state = cpu.GetState();

    // Assert
    EXPECT_EQ(cpu_state.pc, handler)
        << "Failed to jump to handler 0x" << std::hex << handler;
    EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
  }
}

TEST_F(SpecialInstructionsTest, BRK_SetsInterruptDisableFlag) {
  // Arrange - Start with interrupt disable flag clear
  QNes::CPU_Testing::SetPC(cpu, 0x0000);
  QNes::CPU_Testing::SetSP(cpu, 0xFD);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(0x0000, QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

  auto status = cpu.GetState().status;
  status.interrupt_disable = false;
  QNes::CPU_Testing::SetStatus(cpu, status);

  memory.Write(0xFFFE, 0x00);
  memory.Write(0xFFFF, 0x00);

  // Act
  for (int cycle = 0; cycle < 7; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.status.interrupt_disable, true);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(SpecialInstructionsTest, BRK_DoesNotModifyRegisters) {
  // Arrange
  QNes::CPU_Testing::SetA(cpu, 0x42);
  QNes::CPU_Testing::SetX(cpu, 0x84);
  QNes::CPU_Testing::SetY(cpu, 0x99);

  QNes::CPU_Testing::SetPC(cpu, 0x0000);
  QNes::CPU_Testing::SetSP(cpu, 0xFD);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(0x0000, QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

  memory.Write(0xFFFE, 0x00);
  memory.Write(0xFFFF, 0x00);

  // Act
  for (int cycle = 0; cycle < 7; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, 0x42);
  EXPECT_EQ(cpu_state.x, 0x84);
  EXPECT_EQ(cpu_state.y, 0x99);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(SpecialInstructionsTest, BRK_AtVariousAddresses) {
  // Test BRK at different PC addresses
  std::vector<u16> test_addresses = {0x0000, 0x0100, 0x1234, 0x7FFF};

  constexpr u16 interrupt_handler = 0xABCD;

  for (u16 start_address : test_addresses) {
    QNes::CPU_Testing::SetPC(cpu, start_address);
    QNes::CPU_Testing::SetSP(cpu, 0xFD);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    memory.Write(start_address,
                 QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

    memory.Write(0xFFFE, QNes::U16Low(interrupt_handler));
    memory.Write(0xFFFF, QNes::U16High(interrupt_handler));

    // Act
    for (int cycle = 0; cycle < 7; ++cycle) {
      cpu.Step();
    }

    auto cpu_state = cpu.GetState();

    // Assert - PC should be saved correctly on stack
    // PushStack writes to 0x0100 + current_sp, then decrements SP
    // With initial SP = 0xFD: PCH -> 0x01FD, PCL -> 0x01FC
    const u16 pc_after_brk = start_address + 2;
    EXPECT_EQ(memory.Read(0x0100 + 0xFD), QNes::U16High(pc_after_brk))
        << "Failed at address 0x" << std::hex << start_address;
    EXPECT_EQ(memory.Read(0x0100 + 0xFC), QNes::U16Low(pc_after_brk));
    EXPECT_EQ(cpu_state.pc, interrupt_handler);
    EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
  }
}

TEST_F(SpecialInstructionsTest, BRK_PreservesStatusFlagsInStack) {
  // Arrange - Test all combinations of status flags
  for (int test_status_int = 0; test_status_int < 256; ++test_status_int) {
    u8 test_status = static_cast<u8>(test_status_int);
    QNes::CPU_Testing::SetPC(cpu, 0x0000);
    QNes::CPU_Testing::SetSP(cpu, 0xFD);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    memory.Write(0x0000, QNes::ISA::BRK<QNes::AddressingMode::Implied>::OPCODE);

    auto status = cpu.GetState().status;
    status.status = test_status;
    QNes::CPU_Testing::SetStatus(cpu, status);

    memory.Write(0xFFFE, 0x00);
    memory.Write(0xFFFF, 0x00);

    // Act
    for (int cycle = 0; cycle < 7; ++cycle) {
      cpu.Step();
    }

    // Assert - Status on stack should have B flag and unused bit set
    // Status was pushed when SP was 0xFB, so it's at 0x01FB
    u8 pushed_status = memory.Read(0x0100 + 0xFB);
    u8 expected_status =
        test_status | (1 << 4) |
        (1 << 5);  // Set B flag (bit 4) and unused flag (bit 5)
    EXPECT_EQ(pushed_status, expected_status)
        << "Failed with status 0x" << std::hex << static_cast<int>(test_status);

    memory.Clear();
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
  }
}