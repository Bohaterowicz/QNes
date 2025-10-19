#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class AbsoluteAddressingTest : public ::testing::TestWithParam<int> {
 public:
  AbsoluteAddressingTest() : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    p_value = GetParam();
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::CPU cpu;

  int p_value = 0;
};

class LogicalOperationsAbsoluteTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  LogicalOperationsAbsoluteTest() : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    auto [a_value, b_value] = GetParam();
    this->a_value = a_value;
    this->b_value = b_value;
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::CPU cpu;

  int a_value = 0;
  int b_value = 0;
};

TEST_P(AbsoluteAddressingTest, LoadsCorrectValueA) {
  // Arrange
  const u16 test_address = 0x1234;
  const u8 test_value = p_value;
  memory.Write(test_address, test_value);

  // Simulate the instruction fetch cycle for LDA Absolute
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // LDA Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // Low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // High byte of address

  // Act
  // Simulate the CPU cycles for LDA Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteAddressingTest, LoadsCorrectValueX) {
  // Arrange
  const u16 test_address = 0x1234;
  const u8 test_value = p_value;
  memory.Write(test_address, test_value);

  // Simulate the instruction fetch cycle for LDA Absolute
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // LDA Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // Low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // High byte of address

  // Act
  // Simulate the CPU cycles for LDA Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteAddressingTest, LoadsCorrectValueY) {
  // Arrange
  const u16 test_address = 0x1234;
  const u8 test_value = p_value;
  memory.Write(test_address, test_value);

  // Simulate the instruction fetch cycle for LDA Absolute
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // LDA Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // Low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // High byte of address

  // Act
  // Simulate the CPU cycles for LDA Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteAddressingTest, StoresCorrectValueA) {
  // Arrange
  const u16 test_address = 0x1234;
  const u8 test_value = p_value;
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(test_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // STA Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // Low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // High byte of address

  // Act
  // Simulate the CPU cycles for STA Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteAddressingTest, StoresCorrectValueX) {
  // Arrange
  const u16 test_address = 0x1234;
  const u8 test_value = p_value;
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(test_address, 0xEE);
  QNes::CPU_Testing::SetX(cpu, test_value);
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STX<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // STX Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // Low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // High byte of address

  // Act
  // Simulate the CPU cycles for STX Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // X register should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteAddressingTest, StoresCorrectValueY) {
  // Arrange
  const u16 test_address = 0x1234;
  const u8 test_value = p_value;
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(test_address, 0xEE);
  QNes::CPU_Testing::SetY(cpu, test_value);
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STY<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // STY Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // Low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // High byte of address

  // Act
  // Simulate the CPU cycles for STY Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Y register should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(LoadFromMemory_Absolute,  // Instance name
                         AbsoluteAddressingTest,   // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);

TEST_P(LogicalOperationsAbsoluteTest, LogicalAND) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // AND
                                                                // Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for AND Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a & b;

  // Assert
  EXPECT_EQ(cpu_state.a, result);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsAbsoluteTest, LogicalEOR) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // EOR
                                                                // Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for EOR Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a ^ b;

  // Assert
  EXPECT_EQ(cpu_state.a, result);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsAbsoluteTest, LogicalORA) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // ORA
                                                                // Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for ORA Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a | b;

  // Assert
  EXPECT_EQ(cpu_state.a, result);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsAbsoluteTest, LogicalBIT) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::BIT<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // BIT
                                                                // Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for BIT Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  // Assert
  EXPECT_EQ(cpu_state.a, a);
  EXPECT_EQ(cpu_state.status.zero, (a & b) == 0);
  EXPECT_EQ(cpu_state.status.negative, (b & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.overflow, (b & 0x40) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    LogicalOperations_Absolute,     // Instance name
    LogicalOperationsAbsoluteTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);
