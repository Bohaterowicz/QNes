#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class ZeroPageIndexedAddressingTest : public ::testing::TestWithParam<int> {
 public:
  ZeroPageIndexedAddressingTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class LogicalOperationsZeroPageIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  LogicalOperationsZeroPageIndexedTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class ArithmeticOperationsZeroPageIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  ArithmeticOperationsZeroPageIndexedTest()
      : memory(Kilobytes(64)), cpu(memory) {}

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

class CompareOperationsZeroPageIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  CompareOperationsZeroPageIndexedTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class IncrementDecrementZeroPageIndexedTest
    : public ::testing::TestWithParam<int> {
 public:
  IncrementDecrementZeroPageIndexedTest()
      : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    QNes::CPU_Testing::SetX(cpu, 0);
    initial_value = GetParam();
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::CPU cpu;

  int initial_value = 0;
};

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueA_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address + 0,
               test_value);  // Value at zero page address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueA_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x33;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address + offset,
               test_value);  // Value at zero page address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueA_X_OFFSET_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDD;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  const u8 effective_address = static_cast<u8>(
      (zeropage_address + offset) & 0x00FF);  // Wrap around zero page

  // Simulate the instruction fetch cycle for LDA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address,
               test_value);  // Value at zero page address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueY_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address + 0,
               test_value);  // Value at zero page address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueY_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x33;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address + offset,
               test_value);  // Value at zero page address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueY_X_OFFSET_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDD;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  const u8 effective_address = static_cast<u8>(
      (zeropage_address + offset) & 0x00FF);  // Wrap around zero page

  // Simulate the instruction fetch cycle for LDA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address,
               test_value);  // Value at zero page address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueX_Y_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, 0);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<
          QNes::AddressingMode::ZeroPageY>::OPCODE);  // Opcode for
                                                      // LDX
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address + 0,
               test_value);  // Value at zero page address + Y (zero) offset

  // Act
  // Simulate the CPU cycles for LDX ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // X register should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueX_Y_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x33;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<
          QNes::AddressingMode::ZeroPageY>::OPCODE);  // Opcode for
                                                      // LDX
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(zeropage_address + offset,
               test_value);  // Value at zero page address + Y offset

  // Act
  // Simulate the CPU cycles for LDA ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // X register should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, LoadsCorrectValueX_Y_OFFSET_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDD;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  const u8 effective_address = static_cast<u8>(
      (zeropage_address + offset) & 0x00FF);  // Wrap around zero page

  // Simulate the instruction fetch cycle for LDX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<
          QNes::AddressingMode::ZeroPageY>::OPCODE);  // Opcode for
                                                      // LDX
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address,
               test_value);  // Value at zero page address + Y offset

  // Act
  // Simulate the CPU cycles for LDX ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // X register should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueA_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // STA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STA ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueA_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x33;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;
  memory.Write(zeropage_address + offset, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // STA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STA ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address + offset);

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueA_X_OFFSET_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xBB;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page

  memory.Write(effective_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // STA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STA ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueX_Y_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, 0xEE);
  QNes::CPU_Testing::SetX(cpu, test_value);
  // Simulate the instruction fetch cycle for STX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, 0);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STX<
          QNes::AddressingMode::ZeroPageY>::OPCODE);  // Opcode for
                                                      // STX
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STX ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address);

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueX_Y_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x33;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;
  memory.Write(zeropage_address + offset, 0xEE);
  QNes::CPU_Testing::SetX(cpu, test_value);
  // Simulate the instruction fetch cycle for STX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STX<
          QNes::AddressingMode::ZeroPageY>::OPCODE);  // Opcode for
                                                      // STX
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STX ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address + offset);

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueX_Y_OFFSET_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xBB;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page

  memory.Write(effective_address, 0xEE);
  QNes::CPU_Testing::SetX(cpu, test_value);
  // Simulate the instruction fetch cycle for STX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STX<
          QNes::AddressingMode::ZeroPageY>::OPCODE);  // Opcode for
                                                      // STX
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STX ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);

  // Assert
  EXPECT_EQ(cpu_state.x,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueY_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u8 test_value = p_value;
  memory.Write(zeropage_address, 0xEE);
  QNes::CPU_Testing::SetY(cpu, test_value);
  // Simulate the instruction fetch cycle for STY ZeroPageIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STY<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // STY
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STY ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address);

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueY_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x33;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;
  memory.Write(zeropage_address + offset, 0xEE);
  QNes::CPU_Testing::SetY(cpu, test_value);
  // Simulate the instruction fetch cycle for STX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STY<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // STY
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STY ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(zeropage_address + offset);

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ZeroPageIndexedAddressingTest, StoresCorrectValueY_X_OFFSET_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xBB;
  const u8 offset = 0xAA;
  const u8 test_value = p_value;

  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page

  memory.Write(effective_address, 0xEE);
  QNes::CPU_Testing::SetY(cpu, test_value);
  // Simulate the instruction fetch cycle for STX ZeroPageIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STY<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // STY
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address

  // Act
  // Simulate the CPU cycles for STY ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(LoadFromMemory_ZeroPageIndexed,  // Instance name
                         ZeroPageIndexedAddressingTest,   // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);

TEST_P(LogicalOperationsZeroPageIndexedTest, LogicalAND) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // AND
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address,
               b);  // value at zero page address + offset

  // Act
  // Simulate the CPU cycles for AND ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

TEST_P(LogicalOperationsZeroPageIndexedTest, LogicalEOR) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address,
               b);  // value at zero page address + offset

  // Act
  // Simulate the CPU cycles for EOR ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

TEST_P(LogicalOperationsZeroPageIndexedTest, LogicalORA) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address,
               b);  // value at zero page address + offset

  // Act
  // Simulate the CPU cycles for ORA ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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
    LogicalOperations_ZeroPageIndexed,     // Instance name
    LogicalOperationsZeroPageIndexedTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(ArithmeticOperationsZeroPageIndexedTest, ArithmeticADC_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // ADC ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address, b);  // value at zero page address + offset

  // Act
  // Simulate the CPU cycles for ADC ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

TEST_P(ArithmeticOperationsZeroPageIndexedTest, ArithmeticADC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // ADC ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address, b);  // value at zero page address + offset

  // Act
  // Simulate the CPU cycles for ADC ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

TEST_P(ArithmeticOperationsZeroPageIndexedTest, ArithmeticSBC_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // SBC ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address, b);  // value at zero page address + offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;

  // Act
  // Simulate the CPU cycles for SBC ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

TEST_P(ArithmeticOperationsZeroPageIndexedTest, ArithmeticSBC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // SBC ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address, b);  // value at zero page address + offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;

  // Act
  // Simulate the CPU cycles for SBC ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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
    ArithmeticOperations_ZeroPageIndexed,     // Instance name
    ArithmeticOperationsZeroPageIndexedTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(0x50, 0x50), std::make_pair(0xD0, 0x90),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(CompareOperationsZeroPageIndexedTest, CompareCMP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 zeropage_address = 0xDE;
  const u8 offset = 0xBB;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u8 effective_address = QNes::U16Low(
      (zeropage_address + static_cast<u16>(offset)));  // Wrap around zero page
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::CMP<
          QNes::AddressingMode::ZeroPageX>::OPCODE);  // Opcode for
                                                      // CMP ZeroPageIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  memory.Write(effective_address, b);  // value at zero page address + offset

  // Act
  // Simulate the CPU cycles for CMP ZeroPageIndexed
  for (int cycle = 0; cycle < 4; ++cycle) {
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

INSTANTIATE_TEST_SUITE_P(CompareOperations_ZeroPageIndexed,
                         CompareOperationsZeroPageIndexedTest,
                         ::testing::Values(std::tuple<u8, u8>{0x50, 0x30},
                                           std::tuple<u8, u8>{0x50, 0x50},
                                           std::tuple<u8, u8>{0x50, 0x60},
                                           std::tuple<u8, u8>{0x10, 0x08},
                                           std::tuple<u8, u8>{0x10, 0x10},
                                           std::tuple<u8, u8>{0x10, 0x20},
                                           std::tuple<u8, u8>{0x80, 0x40},
                                           std::tuple<u8, u8>{0x80, 0x80},
                                           std::tuple<u8, u8>{0x80, 0xC0}));

TEST_P(IncrementDecrementZeroPageIndexedTest, IncrementINC_X) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value + 1;
  const u16 zeropage_address = 0xDE;
  const u8 x_offset = 0x05;
  const u16 effective_address = zeropage_address + x_offset;

  QNes::CPU_Testing::SetX(cpu, x_offset);
  memory.Write(effective_address, start_value);

  // Simulate the instruction fetch cycle for INC ZeroPageX
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::INC<QNes::AddressingMode::ZeroPageX>::OPCODE);
  memory.Write(start_address + 1, zeropage_address);

  // Act
  // Simulate the CPU cycles for INC ZeroPageX (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementZeroPageIndexedTest, DecrementDEC_X) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value - 1;
  const u16 zeropage_address = 0xDE;
  const u8 x_offset = 0x05;
  const u16 effective_address = zeropage_address + x_offset;

  QNes::CPU_Testing::SetX(cpu, x_offset);
  memory.Write(effective_address, start_value);

  // Simulate the instruction fetch cycle for DEC ZeroPageX
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::DEC<QNes::AddressingMode::ZeroPageX>::OPCODE);
  memory.Write(start_address + 1, zeropage_address);

  // Act
  // Simulate the CPU cycles for DEC ZeroPageX (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

INSTANTIATE_TEST_SUITE_P(IncrementDecrementOperations_ZeroPageIndexed,
                         IncrementDecrementZeroPageIndexedTest,
                         ::testing::Values(0, 1, 2, 0x42, 0x7F, 0x80, 254, 255,
                                           0xFF));