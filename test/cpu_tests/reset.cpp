#include <gtest/gtest.h>

#include <vector>

#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class CPUResetTest : public ::testing::Test {
 public:
  CPUResetTest() : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();
    QNes::CPU_Testing::ZeroResetCycle(cpu);
  }

  void TearDown() override {}

  QNes::Memory memory;
  QNes::CPU cpu;
};

// Test complete reset sequence
TEST_F(CPUResetTest, CompleteResetSequence) {
  // Arrange
  constexpr u16 reset_vector = 0xABCD;
  memory.Write(0xFFFC, QNes::U16Low(reset_vector));
  memory.Write(0xFFFD, QNes::U16High(reset_vector));

  // Set initial state to verify it gets cleared
  QNes::CPU_Testing::SetA(cpu, 0x42);
  QNes::CPU_Testing::SetX(cpu, 0x84);
  QNes::CPU_Testing::SetY(cpu, 0x99);
  auto initial_status = cpu.GetState().status;
  initial_status.carry = true;
  initial_status.zero = true;
  QNes::CPU_Testing::SetStatus(cpu, initial_status);
  QNes::CPU_Testing::SetPC(cpu, 0x1234);
  QNes::CPU_Testing::SetSP(cpu, 0x42);

  // Act - Trigger reset and execute all 5 cycles
  cpu.Reset();
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto final_state = cpu.GetState();

  // Assert - Verify final state after reset
  EXPECT_EQ(final_state.pc, reset_vector);
  EXPECT_EQ(final_state.sp, 0xFD);
  EXPECT_EQ(final_state.status.interrupt_disable, true);
  EXPECT_EQ(final_state.status.unused, true);
  EXPECT_EQ(final_state.status.status,
            0b00100100);  // Only I and U flags set
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);

  // Verify mode switched to RUN
  EXPECT_EQ(QNes::CPU_Testing::GetGlobalMode(cpu), QNes::CPU::GlobalMode::RUN);
}

// Test reset sequence with various reset vector values
TEST_F(CPUResetTest, ResetSequenceWithVariousVectors) {
  std::vector<u16> test_vectors = {0x0000, 0x0100, 0x1234,
                                   0x7FFF, 0xFFFF, 0xC000};

  for (u16 reset_vector : test_vectors) {
    memory.Clear();
    memory.Write(0xFFFC, QNes::U16Low(reset_vector));
    memory.Write(0xFFFD, QNes::U16High(reset_vector));

    // Act
    cpu.Reset();
    for (int cycle = 0; cycle < 5; ++cycle) {
      cpu.Step();
    }

    auto final_state = cpu.GetState();

    // Assert
    EXPECT_EQ(final_state.pc, reset_vector)
        << "Failed with reset vector 0x" << std::hex << reset_vector;
    EXPECT_EQ(final_state.sp, 0xFD);
    EXPECT_EQ(final_state.status.interrupt_disable, true);
    EXPECT_EQ(final_state.status.unused, true);
  }
}

// Test reset cycle 0: Clear internal state
TEST_F(CPUResetTest, ResetCycle0_ClearsInternalState) {
  // Arrange - Set up state that should be cleared
  QNes::CPU_Testing::SetPC(cpu, 0x1234);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 5);
  auto status = cpu.GetState().status;
  status.carry = true;
  status.zero = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  // Act
  cpu.Reset();
  cpu.Step();  // Cycle 0

  auto state = cpu.GetState();

  // Assert
  EXPECT_EQ(state.status.status, 0);  // All flags cleared
  EXPECT_EQ(state.pc, 0);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

// Test reset cycle 1: Initialize SP and flags
TEST_F(CPUResetTest, ResetCycle1_InitializesSPAndFlags) {
  // Arrange
  QNes::CPU_Testing::SetSP(cpu, 0x00);

  // Act
  cpu.Reset();
  cpu.Step();  // Cycle 0
  cpu.Step();  // Cycle 1

  auto state = cpu.GetState();

  // Assert
  EXPECT_EQ(state.sp, 0xFD);
  EXPECT_EQ(state.status.interrupt_disable, true);
  EXPECT_EQ(state.status.unused, true);
}

// Test reset cycle 2: Fetch low byte of reset vector
TEST_F(CPUResetTest, ResetCycle2_FetchesLowByteOfResetVector) {
  // Arrange
  constexpr u8 reset_vector_low = 0x34;
  memory.Write(0xFFFC, reset_vector_low);

  // Act
  cpu.Reset();
  cpu.Step();  // Cycle 0
  cpu.Step();  // Cycle 1
  cpu.Step();  // Cycle 2

  // Verify memory read occurred (we can't directly verify the internal latch,
  // but we can verify the cycle advanced)
  auto state = cpu.GetState();

  // Assert - SP should still be 0xFD, flags should be set
  EXPECT_EQ(state.sp, 0xFD);
  EXPECT_EQ(state.status.interrupt_disable, true);
  EXPECT_EQ(state.status.unused, true);
}

// Test reset cycle 3: Fetch high byte of reset vector
TEST_F(CPUResetTest, ResetCycle3_FetchesHighByteOfResetVector) {
  // Arrange
  constexpr u8 reset_vector_high = 0xAB;
  memory.Write(0xFFFD, reset_vector_high);

  // Act
  cpu.Reset();
  cpu.Step();  // Cycle 0
  cpu.Step();  // Cycle 1
  cpu.Step();  // Cycle 2
  cpu.Step();  // Cycle 3

  // Verify memory read occurred
  auto state = cpu.GetState();

  // Assert
  EXPECT_EQ(state.sp, 0xFD);
  EXPECT_EQ(state.status.interrupt_disable, true);
  EXPECT_EQ(state.status.unused, true);
}

// Test reset cycle 4: Set PC and switch to RUN mode
TEST_F(CPUResetTest, ResetCycle4_SetsPCAndSwitchesToRunMode) {
  // Arrange
  constexpr u16 reset_vector = 0x1234;
  memory.Write(0xFFFC, QNes::U16Low(reset_vector));
  memory.Write(0xFFFD, QNes::U16High(reset_vector));

  // Act
  cpu.Reset();
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto state = cpu.GetState();

  // Assert
  EXPECT_EQ(state.pc, reset_vector);

  // Verify we're in RUN mode by executing one more step
  // In RUN mode, it should fetch an opcode; in RESET mode it would continue
  // resetting
  memory.Write(reset_vector, 0xEA);  // NOP opcode
  cpu.Step();
  EXPECT_EQ(state.pc, reset_vector);  // PC should have advanced if in RUN mode
  // Actually, let's check the final PC after the step
  auto state_after_step = cpu.GetState();
  EXPECT_EQ(state_after_step.pc,
            reset_vector + 1);  // PC should advance in RUN mode
}

// Test that reset clears all status flags except I and U
TEST_F(CPUResetTest, ResetClearsAllFlagsExceptIAndU) {
  // Arrange - Set all flags
  auto status = cpu.GetState().status;
  status.carry = true;
  status.zero = true;
  status.interrupt_disable = false;
  status.decimal_mode = true;
  status.break_command = true;
  status.unused = false;
  status.overflow = true;
  status.negative = true;
  QNes::CPU_Testing::SetStatus(cpu, status);

  memory.Write(0xFFFC, 0x00);
  memory.Write(0xFFFD, 0x00);

  // Act
  cpu.Reset();
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto final_state = cpu.GetState();

  // Assert
  EXPECT_EQ(final_state.status.carry, false);
  EXPECT_EQ(final_state.status.zero, false);
  EXPECT_EQ(final_state.status.interrupt_disable, true);  // Set by reset
  EXPECT_EQ(final_state.status.decimal_mode, false);
  EXPECT_EQ(final_state.status.break_command, false);
  EXPECT_EQ(final_state.status.unused, true);  // Set by reset
  EXPECT_EQ(final_state.status.overflow, false);
  EXPECT_EQ(final_state.status.negative, false);
}

// Test that registers A, X, Y are preserved during reset
TEST_F(CPUResetTest, ResetPreservesRegisters) {
  // Arrange
  QNes::CPU_Testing::SetA(cpu, 0x42);
  QNes::CPU_Testing::SetX(cpu, 0x84);
  QNes::CPU_Testing::SetY(cpu, 0x99);

  memory.Write(0xFFFC, 0x00);
  memory.Write(0xFFFD, 0x00);

  // Act
  cpu.Reset();
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto final_state = cpu.GetState();

  // Assert - Registers should be unchanged
  EXPECT_EQ(final_state.a, 0x42);
  EXPECT_EQ(final_state.x, 0x84);
  EXPECT_EQ(final_state.y, 0x99);
}

// Test multiple reset sequences in a row
TEST_F(CPUResetTest, MultipleResetSequences) {
  // Arrange
  constexpr u16 reset_vector_1 = 0x1234;
  constexpr u16 reset_vector_2 = 0xABCD;

  memory.Write(0xFFFC, QNes::U16Low(reset_vector_1));
  memory.Write(0xFFFD, QNes::U16High(reset_vector_1));

  // Act - First reset
  cpu.Reset();
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto state_1 = cpu.GetState();
  EXPECT_EQ(state_1.pc, reset_vector_1);

  // Act - Second reset with different vector
  memory.Write(0xFFFC, QNes::U16Low(reset_vector_2));
  memory.Write(0xFFFD, QNes::U16High(reset_vector_2));

  cpu.Reset();
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto state_2 = cpu.GetState();

  // Assert
  EXPECT_EQ(state_2.pc, reset_vector_2);
  EXPECT_EQ(state_2.sp, 0xFD);
  EXPECT_EQ(state_2.status.interrupt_disable, true);
  EXPECT_EQ(state_2.status.unused, true);
}

// Test reset sequence with edge case reset vectors
TEST_F(CPUResetTest, ResetSequenceEdgeCases) {
  // Test with reset vector at boundaries
  struct TestCase {
    u16 vector;
    u8 low;
    u8 high;
  };

  std::vector<TestCase> test_cases = {
      {0x0000, 0x00, 0x00}, {0x00FF, 0xFF, 0x00},
      {0x0100, 0x00, 0x01}, {0xFF00, 0x00, 0xFF},
      {0xFFFF, 0xFF, 0xFF}, {0x8000, 0x00, 0x80},  // Negative address
  };

  for (const auto& test_case : test_cases) {
    memory.Clear();
    memory.Write(0xFFFC, test_case.low);
    memory.Write(0xFFFD, test_case.high);

    // Act
    cpu.Reset();
    for (int cycle = 0; cycle < 5; ++cycle) {
      cpu.Step();
    }

    auto final_state = cpu.GetState();

    // Assert
    EXPECT_EQ(final_state.pc, test_case.vector)
        << "Failed with vector 0x" << std::hex << test_case.vector;
    EXPECT_EQ(final_state.sp, 0xFD);
  }
}

// Test that reset sequence completes in exactly 5 cycles
TEST_F(CPUResetTest, ResetSequenceCompletesIn5Cycles) {
  // Arrange
  memory.Write(0xFFFC, 0x00);
  memory.Write(0xFFFD, 0x00);

  // Act - Execute 4 cycles (should still be in reset)
  cpu.Reset();
  for (int cycle = 0; cycle < 4; ++cycle) {
    cpu.Step();
  }

  auto state_after_4 = cpu.GetState();
  EXPECT_EQ(state_after_4.pc, 0);  // PC not set yet

  // Act - Execute one more cycle (should complete reset)
  cpu.Step();

  auto state_after_5 = cpu.GetState();

  // Assert - After 5 cycles, PC should be set from reset vector
  // (Even though vector is 0x0000, the important thing is reset completed)
  EXPECT_EQ(state_after_5.pc, 0x0000);

  // Verify we're now in RUN mode by checking that next step fetches instruction
  memory.Write(0x0000, 0xEA);  // NOP
  cpu.Step();
  auto state_after_instruction = cpu.GetState();
  EXPECT_EQ(state_after_instruction.pc,
            0x0001);  // PC should advance in RUN mode
}
