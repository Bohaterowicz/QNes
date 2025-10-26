#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class ZeroPageAddressingTest : public ::testing::TestWithParam<int> {
 public:
  ZeroPageAddressingTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class LogicalOperationsZeroPageTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  LogicalOperationsZeroPageTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class ArithmeticOperationsZeroPageTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  ArithmeticOperationsZeroPageTest() : memory(Kilobytes(64)), cpu(memory) {}

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

TEST_P(ZeroPageAddressingTest, LoadsCorrectValueA) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, test_value);

  // Simulate the instruction fetch cycle for LDA ZeroPage
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // LDA
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for LDA ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(ZeroPageAddressingTest, LoadsCorrectValueX) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, test_value);

  // Simulate the instruction fetch cycle for LDX ZeroPage
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // LDX
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for LDX ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(ZeroPageAddressingTest, LoadsCorrectValueY) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, test_value);

  // Simulate the instruction fetch cycle for LDY ZeroPage
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // LDY
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for LDY ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(ZeroPageAddressingTest, StoresCorrectValueA) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);

  // Simulate the instruction fetch cycle for STA ZeroPage
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // STA
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STA ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address);

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageAddressingTest, StoresCorrectValueX) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, 0xEE);
  QNes::CPU_Testing::SetX(cpu, test_value);

  // Simulate the instruction fetch cycle for STX ZeroPage
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STX<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // STX
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STX ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address);

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // X register should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageAddressingTest, StoresCorrectValueY) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, 0xEE);
  QNes::CPU_Testing::SetY(cpu, test_value);

  // Simulate the instruction fetch cycle for STY ZeroPage
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STY<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // STY
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STY ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address);

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Y register should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(LoadFromMemory_ZeroPage,  // Instance name
                         ZeroPageAddressingTest,   // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);

TEST_P(LogicalOperationsZeroPageTest, LogicalAND) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // AND
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  // Act
  // Simulate the CPU cycles for AND ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(LogicalOperationsZeroPageTest, LogicalEOR) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // EOR
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  // Act
  // Simulate the CPU cycles for EOR ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(LogicalOperationsZeroPageTest, LogicalORA) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // ORA
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  // Act
  // Simulate the CPU cycles for ORA ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(LogicalOperationsZeroPageTest, LogicalBIT) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::BIT<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // BIT
                                                                // ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  // Act
  // Simulate the CPU cycles for BIT ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a, a);
  EXPECT_EQ(cpu_state.status.zero, (a & b) == 0);
  EXPECT_EQ(cpu_state.status.negative, (b & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.overflow, (b & 0x40) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    LogicalOperations_ZeroPage,     // Instance name
    LogicalOperationsZeroPageTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(ArithmeticOperationsZeroPageTest, ArithmeticADC_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // ADC ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  // Act
  // Simulate the CPU cycles for ADC ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(ArithmeticOperationsZeroPageTest, ArithmeticADC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // ADC ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  // Act
  // Simulate the CPU cycles for ADC ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(ArithmeticOperationsZeroPageTest, ArithmeticSBC_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // SBC ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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

TEST_P(ArithmeticOperationsZeroPageTest, ArithmeticSBC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<QNes::AddressingMode::ZeroPage>::OPCODE);  // Opcode for
                                                                // SBC ZeroPage
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address, b);  // value at zero page address

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC ZeroPage
  for (int cycle = 0; cycle < 3; ++cycle) {
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
    ArithmeticOperations_ZeroPage,     // Instance name
    ArithmeticOperationsZeroPageTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);
