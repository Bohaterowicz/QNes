#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class ImmediateAddressingTest : public ::testing::TestWithParam<int> {
 public:
  ImmediateAddressingTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class LogicalOperationsImmediateTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  LogicalOperationsImmediateTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class ArithmeticOperationsImmediateTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  ArithmeticOperationsImmediateTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class CompareOperationsImmediateTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  CompareOperationsImmediateTest() : memory(Kilobytes(64)), cpu(memory) {}

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

TEST_P(ImmediateAddressingTest, LoadsCorrectValueA) {
  // Arrange
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA Immediate
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<QNes::AddressingMode::Immediate>::OPCODE);  // Opcode for
                                                                 // LDA
                                                                 // Immediate
  memory.Write(start_address + 1, test_value);  // immiediate value
  // Act
  // Simulate the CPU cycles for LDA Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ImmediateAddressingTest, LoadsCorrectValueX) {
  // Arrange
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDX Immediate
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<QNes::AddressingMode::Immediate>::OPCODE);  // Opcode for
                                                                 // LDX
                                                                 // Immediate
  memory.Write(start_address + 1, test_value);  // immediate value

  // Act
  // Simulate the CPU cycles for LDX Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ImmediateAddressingTest, LoadsCorrectValueY) {
  // Arrange
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDY Immediate
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<QNes::AddressingMode::Immediate>::OPCODE);  // Opcode for
                                                                 // LDY
                                                                 // Immediate
  memory.Write(start_address + 1, test_value);  // immiediate value

  // Act
  // Simulate the CPU cycles for LDY Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(LoadFromMemory_Immediate,  // Instance name
                         ImmediateAddressingTest,   // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);

TEST_P(LogicalOperationsImmediateTest, LogicalAND) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<QNes::AddressingMode::Immediate>::OPCODE);  // Opcode for
                                                                 // AND
                                                                 // Immediate
  memory.Write(start_address + 1, b);  // immediate value

  // Act
  // Simulate the CPU cycles for AND Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a & b;

  // Assert
  EXPECT_EQ(cpu_state.a, result);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsImmediateTest, LogicalEOR) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<QNes::AddressingMode::Immediate>::OPCODE);  // Opcode for
                                                                 // EOR
                                                                 // Immediate
  memory.Write(start_address + 1, b);  // immediate value

  // Act
  // Simulate the CPU cycles for EOR Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a ^ b;

  // Assert
  EXPECT_EQ(cpu_state.a, result);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsImmediateTest, LogicalORA) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<QNes::AddressingMode::Immediate>::OPCODE);  // Opcode for
                                                                 // ORA
                                                                 // Immediate
  memory.Write(start_address + 1, b);  // immediate value

  // Act
  // Simulate the CPU cycles for ORA Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a | b;

  // Assert
  EXPECT_EQ(cpu_state.a, result);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    LogicalOperations_Immediate,     // Instance name
    LogicalOperationsImmediateTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(ArithmeticOperationsImmediateTest, ArithmeticADC_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::ADC<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for ADC Immediate
  memory.Write(start_address + 1, b);       // immediate value

  // Act
  // Simulate the CPU cycles for ADC Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a + b;

  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(cpu_state.status.overflow,
            (~(a ^ b) & (a ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsImmediateTest, ArithmeticADC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::ADC<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for ADC Immediate
  memory.Write(start_address + 1, b);       // immediate value

  // Act
  // Simulate the CPU cycles for ADC Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a + b + 1;

  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(cpu_state.status.overflow,
            (~(a ^ b) & (a ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsImmediateTest, ArithmeticSBC_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::SBC<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for SBC Immediate
  memory.Write(start_address + 1, b);       // immediate value

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u8 value = ~b;
  u16 result = a + value + carry_value;

  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(cpu_state.status.overflow,
            (~(a ^ value) & (a ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsImmediateTest, ArithmeticSBC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::SBC<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for SBC Immediate
  memory.Write(start_address + 1, b);       // immediate value

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u8 value = ~b;
  u16 result = a + value + carry_value;

  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(cpu_state.status.overflow,
            (~(a ^ value) & (a ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    ArithmeticOperations_Immediate,     // Instance name
    ArithmeticOperationsImmediateTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(0x50, 0x50), std::make_pair(0xD0, 0x90),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(CompareOperationsImmediateTest, CompareCMP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::CMP<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for CMP Immediate
  memory.Write(start_address + 1, b);       // immediate value

  // Act
  // Simulate the CPU cycles for CMP Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a - b;

  // Assert
  EXPECT_EQ(cpu_state.a, a);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, result >= 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsImmediateTest, CompareCPX) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetX(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::CPX<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for CPX Immediate
  memory.Write(start_address + 1, b);       // immediate value

  // Act
  // Simulate the CPU cycles for CPX Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a - b;

  // Assert
  EXPECT_EQ(cpu_state.x, a);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, result >= 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsImmediateTest, CompareCPY) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  QNes::CPU_Testing::SetY(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(start_address, QNes::ISA::CPY<QNes::AddressingMode::Immediate>::
                                  OPCODE);  // Opcode for CPY Immediate
  memory.Write(start_address + 1, b);       // immediate value

  // Act
  // Simulate the CPU cycles for CPY Immediate
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a - b;

  // Assert
  EXPECT_EQ(cpu_state.y, a);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, result >= 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(CompareOperations_Immediate,
                         CompareOperationsImmediateTest,
                         ::testing::Values(std::tuple<u8, u8>{0x50, 0x30},
                                           std::tuple<u8, u8>{0x50, 0x50},
                                           std::tuple<u8, u8>{0x50, 0x60},
                                           std::tuple<u8, u8>{0x10, 0x08},
                                           std::tuple<u8, u8>{0x10, 0x10},
                                           std::tuple<u8, u8>{0x10, 0x20},
                                           std::tuple<u8, u8>{0x80, 0x40},
                                           std::tuple<u8, u8>{0x80, 0x80},
                                           std::tuple<u8, u8>{0x80, 0xC0}));
