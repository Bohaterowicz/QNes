#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class AbsoluteIndexedAddressingTest : public ::testing::TestWithParam<int> {
 public:
  AbsoluteIndexedAddressingTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class LogicalOperationsAbsoluteIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  LogicalOperationsAbsoluteIndexedTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class ArithmeticOperationsAbsoluteIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  ArithmeticOperationsAbsoluteIndexedTest()
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

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueA_X_ZERO_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + 0,
               test_value);  // Value at address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDA AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueA_Y_ZERO_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA AbsoluteIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, 0);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + 0,
               test_value);  // Value at address + Y (zero) offset

  // Act
  // Simulate the CPU cycles for LDA AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueA_X_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0x11;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + X offset

  // Act
  // Simulate the CPU cycles for LDA AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueA_Y_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0x11;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA AbsoluteIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + Y offset

  // Act
  // Simulate the CPU cycles for LDA AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueA_X_OFFSET_PAGE_CROSS) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               (test_address >> 8) & 0x00FF);  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + X offset

  // Act
  // Simulate the CPU cycles for LDA AbsoluteIndexed + 1 cycle for page cross
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueA_Y_OFFSET_PAGE_CROSS) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDA AbsoluteIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);       // Opcode for
                                                           // LDA
                                                           // AbsoluteIndexed
  memory.Write(start_address + 1, test_address & 0x00FF);  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + Y offset

  // Act
  // Simulate the CPU cycles for LDA AbsoluteIndexed + 1 cycle for page cross
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueX_Y_ZERO_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDX AbsoluteIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, 0);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // LDX
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + 0,
               test_value);  // Value at address + Y (zero) offset

  // Act
  // Simulate the CPU cycles for LDX AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueY_X_ZERO_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDY AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // LDY
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + 0,
               test_value);  // Value at address + X (zero) offset

  // Act
  // Simulate the CPU cycles for LDY AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueY_X_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0x11;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDY AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // LDY
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + X offset

  // Act
  // Simulate the CPU cycles for LDY AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueX_Y_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0x11;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDX AbsoluteIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // LDX
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + Y offset

  // Act
  // Simulate the CPU cycles for LDX AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueY_X_OFFSET_PAGE_CROSS) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDY AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDY<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // LDY
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + X offset

  // Act
  // Simulate the CPU cycles for LDY AbsoluteIndexed + 1 cycle for page cross
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, LoadsCorrectValueX_Y_OFFSET_PAGE_CROSS) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u8 test_value = p_value;

  // Simulate the instruction fetch cycle for LDX AbsoluteIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDX<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // LDX
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(test_address));  // test address low
  memory.Write(start_address + 2,
               QNes::U16High(test_address));  // test address high
  memory.Write(test_address + offset,
               test_value);  // Value at address + Y offset

  // Act
  // Simulate the CPU cycles for LDX AbsoluteIndexed + 1 cycle for page cross
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, StoresCorrectValueA_X_ZERO_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 test_value = p_value;
  memory.Write(test_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // STA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1, QNes::U16Low(test_address));   // test address
  memory.Write(start_address + 2, QNes::U16High(test_address));  // test address
  // Act
  // Simulate the CPU cycles for STA AbsoluteIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
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

TEST_P(AbsoluteIndexedAddressingTest, StoresCorrectValueA_X_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0x11;
  const u8 test_value = p_value;
  memory.Write(test_address + offset, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // STA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1, QNes::U16Low(test_address));   // test address
  memory.Write(start_address + 2, QNes::U16High(test_address));  // test address
  // Act
  // Simulate the CPU cycles for STA AbsoluteIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address + offset);
  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, StoresCorrectValueA_X_OFFSET_WRAP) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u8 test_value = p_value;

  const u16 effective_address = test_address + offset;

  memory.Write(effective_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // STA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1, QNes::U16Low(test_address));   // test address
  memory.Write(start_address + 2, QNes::U16High(test_address));  // test address
  // Act
  // Simulate the CPU cycles for STA AbsoluteIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }
  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);
  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, StoresCorrectValueA_Y_ZERO_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 test_value = p_value;
  memory.Write(test_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, 0);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // STA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1, QNes::U16Low(test_address));   // test address
  memory.Write(start_address + 2, QNes::U16High(test_address));  // test address
  // Act
  // Simulate the CPU cycles for STA AbsoluteIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
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

TEST_P(AbsoluteIndexedAddressingTest, StoresCorrectValueA_Y_OFFSET) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0x11;
  const u8 test_value = p_value;
  memory.Write(test_address + offset, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // STA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1, QNes::U16Low(test_address));   // test address
  memory.Write(start_address + 2, QNes::U16High(test_address));  // test address
  // Act
  // Simulate the CPU cycles for STA AbsoluteIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address + offset);
  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(AbsoluteIndexedAddressingTest, StoresCorrectValueA_Y_OFFSET_WRAP) {
  // Arrange
  const u16 test_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u8 test_value = p_value;

  const u16 effective_address = test_address + offset;

  memory.Write(effective_address, 0xEE);
  QNes::CPU_Testing::SetA(cpu, test_value);
  // Simulate the instruction fetch cycle for STA AbsoluteIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::STA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // STA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1, QNes::U16Low(test_address));   // test address
  memory.Write(start_address + 2, QNes::U16High(test_address));  // test address
  // Act
  // Simulate the CPU cycles for STA AbsoluteIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }
  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(effective_address);
  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // Accumulator should hold the loaded value
  EXPECT_EQ(mem_value,
            test_value);  // Memory should hold the stored value
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(LoadFromMemory_AbsoluteIndexed,  // Instance name
                         AbsoluteIndexedAddressingTest,   // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalAND_X_ZERO_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // AND
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + X offset

  // Act
  // Simulate the CPU cycles for AND AbsoluteIndexed X
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalAND_X_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0x11;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // AND
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + X offset

  // Act
  // Simulate the CPU cycles for AND AbsoluteIndexed X
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalAND_X_OFFSET_WRAP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u16 effective_address = absolute_address + offset;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // AND
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(effective_address,
               b);  // value at effective address

  // Act
  // Simulate the CPU cycles for AND AbsoluteIndexed X + 1 cycle for page
  // crossing
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalAND_Y_ZERO_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // AND
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + Y offset

  // Act
  // Simulate the CPU cycles for AND AbsoluteIndexed Y
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalAND_Y_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0x11;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // AND
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + Y offset

  // Act
  // Simulate the CPU cycles for AND AbsoluteIndexed Y
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalAND_Y_OFFSET_WRAP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u16 effective_address = absolute_address + offset;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // AND
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(effective_address,
               b);  // value at effective address

  // Act
  // Simulate the CPU cycles for AND AbsoluteIndexed Y + 1 cycle for page
  // crossing
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalEOR_X_ZERO_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + X offset

  // Act
  // Simulate the CPU cycles for EOR AbsoluteIndexed X
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalEOR_X_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0x11;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + X offset

  // Act
  // Simulate the CPU cycles for EOR AbsoluteIndexed X
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalEOR_X_OFFSET_WRAP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u16 effective_address = absolute_address + offset;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(effective_address,
               b);  // value at effective address

  // Act
  // Simulate the CPU cycles for EOR AbsoluteIndexed X + 1 cycle for page
  // crossing
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalEOR_Y_ZERO_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + Y offset

  // Act
  // Simulate the CPU cycles for EOR AbsoluteIndexed Y
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalEOR_Y_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0x11;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + Y offset

  // Act
  // Simulate the CPU cycles for EOR AbsoluteIndexed Y
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalEOR_Y_OFFSET_WRAP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u16 effective_address = absolute_address + offset;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(effective_address,
               b);  // value at effective address

  // Act
  // Simulate the CPU cycles for EOR AbsoluteIndexed Y + 1 cycle for page
  // crossing
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalORA_X_ZERO_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + X offset

  // Act
  // Simulate the CPU cycles for ORA AbsoluteIndexed X
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalORA_X_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0x11;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + X offset

  // Act
  // Simulate the CPU cycles for ORA AbsoluteIndexed X
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalORA_X_OFFSET_WRAP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u16 effective_address = absolute_address + offset;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(effective_address,
               b);  // value at effective address

  // Act
  // Simulate the CPU cycles for ORA AbsoluteIndexed X + 1 cycle for page
  // crossing
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalORA_Y_ZERO_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + Y offset

  // Act
  // Simulate the CPU cycles for ORA AbsoluteIndexed Y
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalORA_Y_OFFSET) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0x11;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address + offset,
               b);  // value at absolute address + Y offset

  // Act
  // Simulate the CPU cycles for ORA AbsoluteIndexed Y
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

TEST_P(LogicalOperationsAbsoluteIndexedTest, LogicalORA_Y_OFFSET_WRAP) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  const u8 offset = 0xDD;
  const u16 effective_address = absolute_address + offset;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(effective_address,
               b);  // value at effective address

  // Act
  // Simulate the CPU cycles for ORA AbsoluteIndexed Y + 1 cycle for page
  // crossing
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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

INSTANTIATE_TEST_SUITE_P(
    LogicalOperations_AbsoluteIndexed,     // Instance name
    LogicalOperationsAbsoluteIndexedTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(ArithmeticOperationsAbsoluteIndexedTest,
       ArithmeticADC_X_NO_WRAP_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDE11;
  const u8 offset = 0x22;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticADC_X_NO_WRAP_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDE11;
  const u8 offset = 0x22;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticADC_X_WRAP_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEDD;
  const u8 offset = 0xDD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticADC_X_WRAP_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEDD;
  const u8 offset = 0xDD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetX(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteX>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest,
       ArithmeticADC_Y_NO_WRAP_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDE11;
  const u8 offset = 0x22;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticADC_Y_NO_WRAP_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDE11;
  const u8 offset = 0x22;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticADC_Y_WRAP_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEDD;
  const u8 offset = 0xDD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticADC_Y_WRAP_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEDD;
  const u8 offset = 0xDD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // ADC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  // Act
  // Simulate the CPU cycles for ADC AbsoluteIndexed
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest,
       ArithmeticSBC_Y_NO_WRAP_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDE11;
  const u8 offset = 0x22;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // SBC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticSBC_Y_NO_WRAP_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDE11;
  const u8 offset = 0x22;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // SBC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC AbsoluteIndexed
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticSBC_Y_WRAP_NoCarry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEDD;
  const u8 offset = 0xDD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // SBC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC AbsoluteIndexed
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsAbsoluteIndexedTest, ArithmeticSBC_Y_WRAP_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEDD;
  const u8 offset = 0xDD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetY(cpu, offset);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u16 effective_address =
      (absolute_address +
       static_cast<u16>(offset));        // Wrap around absolute address
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::AbsoluteY>::OPCODE);  // Opcode for
                                                      // SBC AbsoluteIndexed
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // absolute address low
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // absolute address high
  memory.Write(effective_address, b);  // value at absolute address + offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC AbsoluteIndexed
  for (int cycle = 0; cycle < 4 + 1; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    ArithmeticOperations_AbsoluteIndexed,     // Instance name
    ArithmeticOperationsAbsoluteIndexedTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);
