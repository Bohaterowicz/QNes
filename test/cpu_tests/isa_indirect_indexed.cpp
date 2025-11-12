#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"

class IndirectIndexedAddressingTest : public ::testing::TestWithParam<int> {
 public:
  IndirectIndexedAddressingTest()
      : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    p_value = GetParam();
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;

  int p_value = 0;
};

class LogicalOperationsIndirectIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  LogicalOperationsIndirectIndexedTest()
      : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    auto [a_value, b_value] = GetParam();
    this->a_value = a_value;
    this->b_value = b_value;
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;

  int a_value = 0;
  int b_value = 0;
};

class ArithmeticOperationsIndirectIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  ArithmeticOperationsIndirectIndexedTest()
      : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    auto [a_value, b_value] = GetParam();
    this->a_value = a_value;
    this->b_value = b_value;
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;

  int a_value = 0;
  int b_value = 0;
};

class CompareOperationsIndirectIndexedTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  CompareOperationsIndirectIndexedTest()
      : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
    auto [a_value, b_value] = GetParam();
    this->a_value = a_value;
    this->b_value = b_value;
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;

  int a_value = 0;
  int b_value = 0;
};

TEST_P(IndirectIndexedAddressingTest, LoadsCorrectValueA_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 test_value = p_value;
  const u8 offset = 0;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address + X
  memory.Write(zeropage_address + offset, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + offset + 1),
               QNes::U16High(indirect_address));
  memory.Write(indirect_address, test_value);  // Value at indirect address

  // Act
  // Simulate the CPU cycles for LDA IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
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

TEST_P(IndirectIndexedAddressingTest, LoadsCorrectValueA_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 test_value = p_value;
  const u8 offset = 0x22;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address + X
  memory.Write(zeropage_address + offset, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + offset + 1),
               QNes::U16High(indirect_address));
  memory.Write(indirect_address, test_value);  // Value at indirect address

  // Act
  // Simulate the CPU cycles for LDA IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
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

TEST_P(IndirectIndexedAddressingTest, LoadsCorrectValueA_X_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 test_value = p_value;
  const u8 offset = 0xAA;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed X
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address + X
  memory.Write((zeropage_address + offset) & 0x00FF, indirect_address & 0x00FF);
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + offset + 1) & 0x00FF,
               (indirect_address >> 8) & 0x00FF);
  memory.Write(indirect_address, test_value);  // Value at indirect address

  // Act
  // Simulate the CPU cycles for LDA IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
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

TEST_P(IndirectIndexedAddressingTest, LoadsCorrectValueA_Y_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 test_value = p_value;
  const u8 offset = 0;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               test_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for LDA IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
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

TEST_P(IndirectIndexedAddressingTest, LoadsCorrectValueA_Y_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 test_value = p_value;
  const u8 offset = 0x22;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               test_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for LDA IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
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

TEST_P(IndirectIndexedAddressingTest, LoadsCorrectValueA_Y_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 test_value = p_value;
  const u8 offset = 0xAA;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::LDA<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // LDA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               test_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for LDA IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
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

INSTANTIATE_TEST_SUITE_P(LoadFromMemory_IndirectIndexed,  // Instance name
                         IndirectIndexedAddressingTest,   // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalAND_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0;
  const u16 effective_address = zeropage_address + offset;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // AND
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for AND IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value & b_value;

  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalAND_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // AND
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for AND IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value & b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalAND_X_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // AND
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for AND IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value & b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalAND_Y_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // AND
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for AND IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value & b_value;

  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalAND_Y_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // AND
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for AND IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value & b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalAND_Y_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  // Simulate the instruction fetch cycle for LDA IndirectIndexed Y
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::AND<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // AND
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for AND IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value & b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalEOR_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for EOR IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value ^ b_value;

  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalEOR_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for EOR IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value ^ b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalEOR_X_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for EOR IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value ^ b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalEOR_Y_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for EOR IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value ^ b_value;

  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalEOR_Y_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for EOR IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value ^ b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalEOR_Y_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::EOR<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // EOR
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for EOR IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value ^ b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalORA_X_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ORA IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value | b_value;

  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalORA_X_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ORA IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value | b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalORA_X_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ORA IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value | b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalORA_Y_ZERO_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ORA IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value | b_value;

  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalORA_Y_OFFSET) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ORA IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value | b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalORA_Y_OFFSET_PAGE_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ORA<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ORA
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ORA IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 result = a_value | b_value;
  // Assert
  EXPECT_EQ(cpu_state.a,
            result);  // Accumulator should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            result == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (result & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    LogicalOperations_IndirectIndexed,     // Instance name
    LogicalOperationsIndirectIndexedTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(ArithmeticOperationsIndirectIndexedTest,
       ArithmeticADC_X_OFFSET_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest, ArithmeticADC_X_OFFSET_Carry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value + 1;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest,
       ArithmeticADC_X_OFFSET_PAGE_WRAP_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest,
       ArithmeticADC_X_OFFSET_PAGE_WRAP_Carry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value + 1;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalADC_Y_OFFSET_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalADC_Y_OFFSET_Carry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value + 1;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest,
       LogicalADC_Y_OFFSET_PAGE_WRAP_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest,
       LogicalADC_Y_OFFSET_PAGE_WRAP_Carry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::ADC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // ADC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for ADC IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u16 result = a_value + b_value + 1;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ b_value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest,
       ArithmeticSBC_X_OFFSET_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  const u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest, ArithmeticSBC_X_OFFSET_Carry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest,
       ArithmeticSBC_X_OFFSET_PAGE_WRAP_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(ArithmeticOperationsIndirectIndexedTest,
       ArithmeticSBC_X_OFFSET_PAGE_WRAP_Carry) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalSBC_Y_OFFSET_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest, LogicalSBC_Y_OFFSET_Carry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest,
       LogicalSBC_Y_OFFSET_PAGE_WRAP_NoCarry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(LogicalOperationsIndirectIndexedTest,
       LogicalSBC_Y_OFFSET_PAGE_WRAP_Carry) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::SBC<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // SBC
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;
  // Act
  // Simulate the CPU cycles for SBC IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  const u8 value = ~b_value;
  u16 result = a_value + value + carry_value;
  // Assert
  EXPECT_EQ(cpu_state.a, QNes::U16Low(result));
  EXPECT_EQ(cpu_state.status.zero, QNes::U16Low(result) == 0);
  EXPECT_EQ(cpu_state.status.negative, (QNes::U16Low(result) & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (result & 0x100) != 0);
  EXPECT_EQ(
      cpu_state.status.overflow,
      (~(a_value ^ value) & (a_value ^ QNes::U16Low(result)) & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(
    ArithmeticOperations_IndirectIndexed,     // Instance name
    ArithmeticOperationsIndirectIndexedTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(CompareOperationsIndirectIndexedTest, CompareCMP_X_NO_WRAP) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::CMP<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // CMP
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for CMP IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a_value - b_value;

  // Assert
  EXPECT_EQ(cpu_state.a, a_value);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (a_value >= b_value) ? 1 : 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsIndirectIndexedTest, CompareCMP_X_WRAP) {
  // Arrange
  const u16 zeropage_address = 0x11;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;
  const u16 effective_address = zeropage_address + offset;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetX(cpu, offset);  // X register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::CMP<
          QNes::AddressingMode::XIndirect>::OPCODE);  // Opcode for
                                                      // CMP
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(effective_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((effective_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address,
               b_value);  // Value at indirect address + X offset

  // Act
  // Simulate the CPU cycles for CMP IndirectIndexed
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a_value - b_value;

  // Assert
  EXPECT_EQ(cpu_state.a, a_value);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (a_value >= b_value) ? 1 : 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsIndirectIndexedTest, CompareCMP_Y_NO_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBE11;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0x22;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::CMP<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // CMP
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for CMP IndirectIndexed
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a_value - b_value;

  // Assert
  EXPECT_EQ(cpu_state.a, a_value);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (a_value >= b_value) ? 1 : 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsIndirectIndexedTest, CompareCMP_Y_WRAP) {
  // Arrange
  const u16 zeropage_address = 0xDE;
  const u16 indirect_address = 0xBEEF;
  const u8 a_value = this->a_value;
  const u8 b_value = this->b_value;
  const u8 offset = 0xAA;

  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetA(cpu, a_value);
  QNes::CPU_Testing::SetY(cpu, offset);  // Y register set to 0 for no offset
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::CMP<
          QNes::AddressingMode::IndirectY>::OPCODE);  // Opcode for
                                                      // CMP
                                                      // IndirectIndexed
  memory.Write(start_address + 1, zeropage_address);  // zero page address
  // Indirect address low byte at zero page address
  memory.Write(zeropage_address, QNes::U16Low(indirect_address));
  // Indirect address high byte at next byte
  memory.Write((zeropage_address + 1), QNes::U16High(indirect_address));
  memory.Write(indirect_address + offset,
               b_value);  // Value at indirect address + Y offset

  // Act
  // Simulate the CPU cycles for CMP IndirectIndexed + 1 cycle for page wrap
  for (int cycle = 0; cycle < 5 + 1; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a_value - b_value;

  // Assert
  EXPECT_EQ(cpu_state.a, a_value);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, (a_value >= b_value) ? 1 : 0);
  EXPECT_EQ(cpu_state.pc, start_address + 2);  // PC should advance by 2
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(CompareOperations_IndirectIndexed,
                         CompareOperationsIndirectIndexedTest,
                         ::testing::Values(std::tuple<u8, u8>{0x50, 0x30},
                                           std::tuple<u8, u8>{0x50, 0x50},
                                           std::tuple<u8, u8>{0x50, 0x60},
                                           std::tuple<u8, u8>{0x10, 0x08},
                                           std::tuple<u8, u8>{0x10, 0x10},
                                           std::tuple<u8, u8>{0x10, 0x20},
                                           std::tuple<u8, u8>{0x80, 0x40},
                                           std::tuple<u8, u8>{0x80, 0x80},
                                           std::tuple<u8, u8>{0x80, 0xC0}));