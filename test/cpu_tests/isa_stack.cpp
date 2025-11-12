#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class CPUISAStackTest : public ::testing::Test {
 public:
  CPUISAStackTest() : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

 protected:
  void SetUp() override {
    memory.Clear();  // Clear memory before each test
    cpu.Reset();
    QNes::CPU_Testing::ExecuteReset(cpu);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::RAMBus bus;
  QNes::CPU cpu;
};

TEST_F(CPUISAStackTest, PHAInstructionPushesAtoStack) {
  // Arrange
  const u8 test_value = 0x42;
  QNes::CPU_Testing::SetA(cpu, test_value);
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Simulate the instruction fetch cycle for PHA
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::PHA<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // PHA

  // Act
  // Simulate the CPU cycles for PHA
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto state_after_push = cpu.GetState();
  u8 sp_after_push = state_after_push.sp;

  // Assert
  EXPECT_EQ(sp_after_push,
            initial_sp - 1);  // SP should decrement by 1 after push

  // Verify that the value was correctly pushed onto the stack in memory
  u8 value_on_stack = memory.Read(0x0100 + initial_sp);
  EXPECT_EQ(value_on_stack,
            test_value);  // Value on stack should match A register
}

TEST_F(CPUISAStackTest, PLAInstructionPullsAfromStackPositiveValue) {
  // Arrange
  const u8 test_value = 25;
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Preload the stack with a value to be pulled
  memory.Write(0x0100 + initial_sp, test_value);
  QNes::CPU_Testing::SetSP(cpu, initial_sp - 1);  // Adjust SP accordingly

  // Simulate the instruction fetch cycle for PLA
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::PLA<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // PLA

  // Act
  // Simulate the CPU cycles for PLA
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto state_after_pull = cpu.GetState();
  u8 sp_after_pull = state_after_pull.sp;
  u8 a_after_pull = state_after_pull.a;

  // Assert
  EXPECT_EQ(sp_after_pull,
            initial_sp);  // SP should increment by 1 after pull
  EXPECT_EQ(a_after_pull,
            test_value);  // A register should match the value pulled from stack

  // Verify that Zero and Negative flags are set correctly
  EXPECT_EQ(state_after_pull.status.zero, false);
  EXPECT_EQ(state_after_pull.status.negative, false);
}

TEST_F(CPUISAStackTest, PLAInstructionPullsAfromStackNegativeValue) {
  // Arrange
  const u8 test_value = -25;
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Preload the stack with a value to be pulled
  memory.Write(0x0100 + initial_sp, test_value);
  QNes::CPU_Testing::SetSP(cpu, initial_sp - 1);  // Adjust SP accordingly

  // Simulate the instruction fetch cycle for PLA
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::PLA<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // PLA

  // Act
  // Simulate the CPU cycles for PLA
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto state_after_pull = cpu.GetState();
  u8 sp_after_pull = state_after_pull.sp;
  u8 a_after_pull = state_after_pull.a;

  // Assert
  EXPECT_EQ(sp_after_pull,
            initial_sp);  // SP should increment by 1 after pull
  EXPECT_EQ(a_after_pull,
            test_value);  // A register should match the value pulled from stack

  // Verify that Zero and Negative flags are set correctly
  EXPECT_EQ(state_after_pull.status.zero, false);
  EXPECT_EQ(state_after_pull.status.negative, true);
}

TEST_F(CPUISAStackTest, PLAInstructionPullsAfromStackZeroValue) {
  // Arrange
  const u8 test_value = 0;
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Preload the stack with a value to be pulled
  memory.Write(0x0100 + initial_sp, test_value);
  QNes::CPU_Testing::SetSP(cpu, initial_sp - 1);  // Adjust SP accordingly
  QNes::CPU_Testing::SetA(cpu, 0xFF);  // Set A to non-zero to verify zero flag

  // Simulate the instruction fetch cycle for PLA
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::PLA<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // PLA

  // Act
  // Simulate the CPU cycles for PLA
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto state_after_pull = cpu.GetState();
  u8 sp_after_pull = state_after_pull.sp;
  u8 a_after_pull = state_after_pull.a;

  // Assert
  EXPECT_EQ(sp_after_pull,
            initial_sp);  // SP should increment by 1 after pull
  EXPECT_EQ(a_after_pull,
            test_value);  // A register should match the value pulled from stack

  // Verify that Zero and Negative flags are set correctly
  EXPECT_EQ(state_after_pull.status.zero, true);
  EXPECT_EQ(state_after_pull.status.negative, false);
}

TEST_F(CPUISAStackTest, PHPInstructionPushesPtoStack) {
  // Arrange
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Set some flags in the status register
  initial_state.status.carry = 1;
  initial_state.status.zero = 0;
  initial_state.status.interrupt_disable = 1;
  initial_state.status.decimal_mode = 0;
  initial_state.status.break_command = 0;
  initial_state.status.overflow = 0;
  initial_state.status.negative = 1;
  QNes::CPU_Testing::SetStatus(cpu, initial_state.status);

  // Simulate the instruction fetch cycle for PHP
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::PHP<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // PHP

  // Act
  // Simulate the CPU cycles for PHP
  for (int cycle = 0; cycle < 3; ++cycle) {
    cpu.Step();
  }

  auto state_after_push = cpu.GetState();
  u8 sp_after_push = state_after_push.sp;

  // Assert
  EXPECT_EQ(sp_after_push,
            initial_sp - 1);  // SP should decrement by 1 after push

  // Verify that the status register was correctly pushed onto the stack in
  // memory
  u8 value_on_stack = memory.Read(0x0100 + initial_sp);
  auto initial_state_2 = initial_state;  // Copy to modify for expected value
  initial_state_2.status.break_command = 1;  // Break flag is set when pushed
  EXPECT_EQ(value_on_stack,
            initial_state_2.status
                .status);  // Value on stack should match status register
}

TEST_F(CPUISAStackTest, PLPInstructionPullsPfromStack) {
  // Arrange
  auto initial_state = cpu.GetState();
  u8 initial_sp = initial_state.sp;

  // Preload the stack with a status register value to be pulled
  // Set some flags in the status register value
  QNes::CPU::StatusFlags status_to_push = {};
  status_to_push.carry = 1;              // Carry
  status_to_push.zero = 0;               // Zero
  status_to_push.interrupt_disable = 1;  // Interrupt Disable
  status_to_push.decimal_mode = 0;       // Decimal Mode
  status_to_push.break_command =
      1;                        // Break Command (will be ignored when pulled)
  status_to_push.unused = 0;    // Unused, always set to 1 in real hardware
  status_to_push.overflow = 1;  // Overflow
  status_to_push.negative = 1;  // Negative

  memory.Write(0x0100 + initial_sp, status_to_push.status);
  QNes::CPU_Testing::SetSP(cpu, initial_sp - 1);  // Adjust SP accordingly

  // Simulate the instruction fetch cycle for PLP
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::PLP<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // PLP

  // Act
  // Simulate the CPU cycles for PLP
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto state_after_pull = cpu.GetState();
  u8 sp_after_pull = state_after_pull.sp;
  auto status_after_pull = state_after_pull.status;

  // Assert
  EXPECT_EQ(sp_after_pull,
            initial_sp);  // SP should increment by 1 after pull

  // Verify that the status register matches the value pulled from stack
  EXPECT_EQ(status_after_pull.carry, true);
  EXPECT_EQ(status_after_pull.zero, false);
  EXPECT_EQ(status_after_pull.interrupt_disable, true);
  EXPECT_EQ(status_after_pull.decimal_mode, false);
  EXPECT_EQ(status_after_pull.break_command,
            0);  // Break flag is ignored when pulled
  EXPECT_EQ(status_after_pull.overflow, true);
  EXPECT_EQ(status_after_pull.negative, true);
}

TEST_F(CPUISAStackTest, TSXInstructionTransfersSPtoX) {
  // Arrange
  u8 initial_sp = 124;
  QNes::CPU_Testing::SetSP(cpu, initial_sp);
  QNes::CPU_Testing::SetX(cpu, 0xDE);

  // Simulate the instruction fetch cycle for TSX
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TSX<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // TSX

  // Act
  // Simulate the CPU cycles for TSX
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto state_after_transfer = cpu.GetState();
  u8 sp_after_transfer = state_after_transfer.sp;
  u8 x_after_transfer = state_after_transfer.x;

  // Assert
  EXPECT_EQ(sp_after_transfer, initial_sp);
  EXPECT_EQ(x_after_transfer, initial_sp);
  EXPECT_EQ(state_after_transfer.status.zero, false);
  EXPECT_EQ(state_after_transfer.status.negative, false);
}

TEST_F(CPUISAStackTest, TSXInstructionTransfersSPtoXZeroValue) {
  // Arrange
  QNes::CPU_Testing::SetSP(cpu, 0);
  QNes::CPU_Testing::SetX(cpu, 0xDE);

  // Simulate the instruction fetch cycle for TSX
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TSX<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // TSX

  // Act
  // Simulate the CPU cycles for TSX
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto state_after_transfer = cpu.GetState();
  u8 sp_after_transfer = state_after_transfer.sp;
  u8 x_after_transfer = state_after_transfer.x;

  // Assert
  EXPECT_EQ(sp_after_transfer,
            0);  // SP should be equal to 0 after transfer
  EXPECT_EQ(x_after_transfer,
            0);  // X should be equal to 0 after transfer
  EXPECT_EQ(state_after_transfer.status.zero, true);
  EXPECT_EQ(state_after_transfer.status.negative, false);
}

TEST_F(CPUISAStackTest, TSXInstructionTransfersSPtoXNegativeValue) {
  // Arrange
  QNes::CPU_Testing::SetSP(cpu, 0xFF);
  QNes::CPU_Testing::SetX(cpu, 0xDE);

  // Simulate the instruction fetch cycle for TSX
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TSX<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // TSX

  // Act
  // Simulate the CPU cycles for TSX
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto state_after_transfer = cpu.GetState();
  u8 sp_after_transfer = state_after_transfer.sp;
  u8 x_after_transfer = state_after_transfer.x;

  // Assert
  EXPECT_EQ(sp_after_transfer,
            0xFF);  // SP should be equal to 0xFF after transfer
  EXPECT_EQ(x_after_transfer,
            0xFF);  // X should be equal to 0xFF after transfer
  EXPECT_EQ(state_after_transfer.status.zero, false);
  EXPECT_EQ(state_after_transfer.status.negative, true);
}

TEST_F(CPUISAStackTest, TXSInstructionTransfersXtoSP) {
  // Arrange
  auto initial_state = cpu.GetState();
  u8 initial_x = initial_state.x;
  u8 initial_sp = initial_state.sp;
  QNes::CPU_Testing::SetX(cpu, initial_x);
  QNes::CPU_Testing::SetSP(cpu, initial_sp);
  // Simulate the instruction fetch cycle for TXS
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TSX<QNes::AddressingMode::Implied>::OPCODE);  // Opcode
                                                               // for
                                                               // TSX
}
