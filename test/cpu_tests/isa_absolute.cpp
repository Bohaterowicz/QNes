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

class ArithmeticOperationsAbsoluteTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  ArithmeticOperationsAbsoluteTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class CompareOperationsAbsoluteTest
    : public ::testing::TestWithParam<std::pair<int, int>> {
 public:
  CompareOperationsAbsoluteTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class IncrementDecrementAbsoluteTest : public ::testing::TestWithParam<int> {
 public:
  IncrementDecrementAbsoluteTest() : memory(Kilobytes(64)), cpu(memory) {}

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

class AbsoluteBasicTest : public ::testing::Test {
 public:
  AbsoluteBasicTest() : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::CPU cpu;
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

TEST_F(AbsoluteBasicTest, JMPAbsolute) {
  // Arrange
  constexpr u16 start_address = 0x0000;
  constexpr u16 target_address = 0xBEEF;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::JMP<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(target_address));
  memory.Write(start_address + 2, QNes::U16High(target_address));

  // Act
  // Simulate the CPU cycles for JMP Absolute (3 cycles)
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.pc, target_address);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(AbsoluteBasicTest, JSRAbsolute) {
  // Arrange
  constexpr u16 start_address = 0x0000;
  constexpr u16 target_address = 0xBEEF;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  auto initial_state = cpu.GetState();
  const u8 initial_sp = initial_state.sp;  // expected 0xFD
  memory.Write(start_address,
               QNes::ISA::JSR<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(target_address));
  memory.Write(start_address + 2, QNes::U16High(target_address));

  // Act
  // Simulate the CPU cycles for JSR Absolute (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // The 6502 pushes PC-1 (address of last operand byte) onto the stack.
  const u16 pushed_return = static_cast<u16>(start_address + 2);
  const u8 pushed_high = QNes::CPU_Testing::ReadStackValue(cpu, initial_sp);
  const u8 pushed_low = QNes::CPU_Testing::ReadStackValue(cpu, initial_sp - 1);

  // Assert
  EXPECT_EQ(cpu_state.pc, target_address);
  EXPECT_EQ(cpu_state.sp, static_cast<u8>(initial_sp - 2));
  EXPECT_EQ(pushed_high, QNes::U16High(pushed_return));
  EXPECT_EQ(pushed_low, QNes::U16Low(pushed_return));
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(AbsoluteBasicTest, RTSImpliedOnly) {
  // Arrange
  constexpr u16 start_address = 0x0000;
  constexpr u16 final_pc = 0xC123;  // expected PC after RTS completes
  // Load stack with return address minus 1 (as RTS will increment after pull)
  const u16 stacked_return = static_cast<u16>(final_pc - 1);
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  QNes::CPU_Testing::SetSP(cpu, 0xFB);
  const u8 sp_before = cpu.GetState().sp;
  // Place low and high bytes at SP+1 and SP+2 respectively
  memory.Write(static_cast<u16>(0x0100 + static_cast<u8>(sp_before + 1)),
               QNes::U16Low(stacked_return));
  memory.Write(static_cast<u16>(0x0100 + static_cast<u8>(sp_before + 2)),
               QNes::U16High(stacked_return));
  memory.Write(start_address,
               QNes::ISA::RTS<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Simulate the CPU cycles for RTS (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.pc, final_pc);
  EXPECT_EQ(cpu_state.sp, static_cast<u8>(sp_before + 2));
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(AbsoluteBasicTest, JSRThenRTS_ReturnsToCaller) {
  // Arrange
  constexpr u16 start_address = 0x0000;
  constexpr u16 subroutine_address = 0x00A0;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  const u8 initial_sp = cpu.GetState().sp;

  // Program: JSR subroutine; (at subroutine) RTS
  memory.Write(start_address,
               QNes::ISA::JSR<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(subroutine_address));
  memory.Write(start_address + 2, QNes::U16High(subroutine_address));
  memory.Write(subroutine_address,
               QNes::ISA::RTS<QNes::AddressingMode::Implied>::OPCODE);

  // Act
  // Execute JSR (6 cycles) then RTS (6 cycles)
  for (int cycle = 0; cycle < 12; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert: PC should now be at the instruction after JSR (start + 3)
  EXPECT_EQ(cpu_state.pc, static_cast<u16>(start_address + 3));
  // SP should be restored to its initial value after JSR pushed 2 and RTS
  // popped 2
  EXPECT_EQ(cpu_state.sp, initial_sp);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

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

TEST_P(ArithmeticOperationsAbsoluteTest, ArithmeticADC_NoCarry) {
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
      QNes::ISA::ADC<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // ADC Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for ADC Absolute
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

TEST_P(ArithmeticOperationsAbsoluteTest, ArithmeticADC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::ADC<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // ADC Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for ADC Absolute
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

TEST_P(ArithmeticOperationsAbsoluteTest, ArithmeticSBC_NoCarry) {
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
      QNes::ISA::SBC<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // SBC Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;

  // Act
  // Simulate the CPU cycles for SBC Absolute
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

TEST_P(ArithmeticOperationsAbsoluteTest, ArithmeticSBC_Carry) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetA(cpu, a);
  QNes::CPU_Testing::SetCarry(cpu, true);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::SBC<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // SBC Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  const u8 carry_value = QNes::CPU_Testing::GetCarry(cpu) ? 1 : 0;

  // Act
  // Simulate the CPU cycles for SBC Absolute
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

INSTANTIATE_TEST_SUITE_P(
    ArithmeticOperations_Absolute,     // Instance name
    ArithmeticOperationsAbsoluteTest,  // The test fixture
    ::testing::Values(std::make_pair(1, 2), std::make_pair(0x42, 254),
                      std::make_pair(0x50, 0x50), std::make_pair(0xD0, 0x90),
                      std::make_pair(255, 0xFF), std::make_pair(0, -1),
                      std::make_pair(-2, -10), std::make_pair(-254, -255),
                      std::make_pair(0x00, 0x00))  // The test data
);

TEST_P(CompareOperationsAbsoluteTest, CompareCMP) {
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
      QNes::ISA::CMP<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // CMP Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for CMP Absolute
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
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsAbsoluteTest, CompareCPX) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetX(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::CPX<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // CMP Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for CMP Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a - b;

  // Assert
  EXPECT_EQ(cpu_state.x, a);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, result >= 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(CompareOperationsAbsoluteTest, CompareCPY) {
  // Arrange
  const u8 a = a_value;
  const u8 b = b_value;
  const u16 absolute_address = 0xDEAD;
  QNes::CPU_Testing::SetY(cpu, a);
  QNes::CPU_Testing::SetPC(cpu, 0);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  memory.Write(
      start_address,
      QNes::ISA::CPY<QNes::AddressingMode::Absolute>::OPCODE);  // Opcode for
                                                                // CMP Absolute
  memory.Write(start_address + 1,
               QNes::U16Low(absolute_address));  // low byte of address
  memory.Write(start_address + 2,
               QNes::U16High(absolute_address));  // high byte of address
  memory.Write(absolute_address, b);              // value at absolute address

  // Act
  // Simulate the CPU cycles for CMP Absolute
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  u16 result = a - b;

  // Assert
  EXPECT_EQ(cpu_state.y, a);
  EXPECT_EQ(cpu_state.status.zero, result == 0);
  EXPECT_EQ(cpu_state.status.negative, (result & 0x80) != 0);
  EXPECT_EQ(cpu_state.status.carry, result >= 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(CompareOperations_Absolute,
                         CompareOperationsAbsoluteTest,
                         ::testing::Values(std::tuple<u8, u8>{0x50, 0x30},
                                           std::tuple<u8, u8>{0x50, 0x50},
                                           std::tuple<u8, u8>{0x50, 0x60},
                                           std::tuple<u8, u8>{0x10, 0x08},
                                           std::tuple<u8, u8>{0x10, 0x10},
                                           std::tuple<u8, u8>{0x10, 0x20},
                                           std::tuple<u8, u8>{0x80, 0x40},
                                           std::tuple<u8, u8>{0x80, 0x80},
                                           std::tuple<u8, u8>{0x80, 0xC0}));

TEST_P(IncrementDecrementAbsoluteTest, IncrementINC) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value + 1;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for INC Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::INC<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  // Simulate the CPU cycles for INC Absolute (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, DecrementDEC) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = start_value - 1;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for DEC Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::DEC<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  // Simulate the CPU cycles for DEC Absolute (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, ShiftLeft_ASL_Absolute) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = static_cast<u8>((start_value << 1) & 0xFF);
  const bool expected_carry = (start_value & 0x80) != 0;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for ASL Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ASL<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  // Simulate the CPU cycles for ASL Absolute (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);  // PC should advance by 3
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, ShiftRight_LSR_Absolute) {
  // Arrange
  const u8 start_value = initial_value;
  const u8 expected_result = static_cast<u8>((start_value >> 1) & 0xFF);
  const bool expected_carry = (start_value & 0x01) != 0;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for LSR Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::LSR<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  // Simulate the CPU cycles for LSR Absolute (6 cycles)
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, RotateLeft_ROL_Absolute_NoCarry) {
  // Arrange
  const u8 start_value = initial_value;
  QNes::CPU_Testing::SetCarry(cpu, false);
  const u8 expected_result = static_cast<u8>(((start_value << 1) | 0) & 0xFF);
  const bool expected_carry = (start_value & 0x80) != 0;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for ROL Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROL<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, RotateLeft_ROL_Absolute_Carry) {
  // Arrange
  const u8 start_value = initial_value;
  QNes::CPU_Testing::SetCarry(cpu, true);
  const u8 expected_result = static_cast<u8>(((start_value << 1) | 1) & 0xFF);
  const bool expected_carry = (start_value & 0x80) != 0;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for ROL Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROL<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, RotateRight_ROR_Absolute_NoCarry) {
  // Arrange
  const u8 start_value = initial_value;
  QNes::CPU_Testing::SetCarry(cpu, false);
  const u8 expected_result =
      static_cast<u8>(((start_value >> 1) | 0x00) & 0xFF);
  const bool expected_carry = (start_value & 0x01) != 0;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for ROR Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROR<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_P(IncrementDecrementAbsoluteTest, RotateRight_ROR_Absolute_Carry) {
  // Arrange
  const u8 start_value = initial_value;
  QNes::CPU_Testing::SetCarry(cpu, true);
  const u8 expected_result =
      static_cast<u8>(((start_value >> 1) | 0x80) & 0xFF);
  const bool expected_carry = (start_value & 0x01) != 0;
  const u16 test_address = 0x1234;
  memory.Write(test_address, start_value);

  // Simulate the instruction fetch cycle for ROR Absolute
  constexpr u16 start_address = 0x0000;
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::ROR<QNes::AddressingMode::Absolute>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(test_address));
  memory.Write(start_address + 2, QNes::U16High(test_address));

  // Act
  for (int cycle = 0; cycle < 6; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();
  auto mem_value = memory.Read(test_address);

  // Assert
  EXPECT_EQ(mem_value, expected_result);
  EXPECT_EQ(cpu_state.status.carry, expected_carry);
  EXPECT_EQ(cpu_state.status.zero, expected_result == 0);
  EXPECT_EQ(cpu_state.status.negative, (expected_result & 0x80) != 0);
  EXPECT_EQ(cpu_state.pc, start_address + 3);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

INSTANTIATE_TEST_SUITE_P(IncrementDecrementOperations_Absolute,
                         IncrementDecrementAbsoluteTest,
                         ::testing::Values(0, 1, 2, 0x42, 0x7F, 0x80, 254, 255,
                                           0xFF));