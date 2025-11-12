#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"

class RegisterTransferTest : public ::testing::TestWithParam<int> {
 public:
  RegisterTransferTest() : memory(Kilobytes(64)), bus(&memory), cpu(&bus) {}

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

TEST_P(RegisterTransferTest, TAXInstructionTransfersAtoX) {
  // Arrange
  const u8 test_value = p_value;
  QNes::CPU_Testing::SetA(cpu, test_value);
  QNes::CPU_Testing::SetX(cpu, 0xFD);
  // Simulate the instruction fetch cycle for TAX
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TAX<QNes::AddressingMode::Implied>::OPCODE);  // Opcode for
                                                               // TAX
  QNes::CPU_Testing::SetPC(cpu, start_address);
  // Act
  // Simulate the CPU cycles for TAX
  for (int cycle = 0; cycle < 2; ++cycle) {
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
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(RegisterTransferTest, TAXInstructionTransfersAtoY) {
  // Arrange
  const u8 test_value = p_value;
  QNes::CPU_Testing::SetA(cpu, test_value);
  QNes::CPU_Testing::SetY(cpu, 0xFD);
  // Simulate the instruction fetch cycle for TAY
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TAY<QNes::AddressingMode::Implied>::OPCODE);  // Opcode for
                                                               // TAY
  QNes::CPU_Testing::SetPC(cpu, start_address);
  // Act
  // Simulate the CPU cycles for TAY
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.y,
            test_value);  // Y register should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(RegisterTransferTest, TXAInstructionTransfersXtoA) {
  // Arrange
  const u8 test_value = p_value;
  QNes::CPU_Testing::SetX(cpu, test_value);
  QNes::CPU_Testing::SetA(cpu, 0xFD);
  // Simulate the instruction fetch cycle for TXA
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TXA<QNes::AddressingMode::Implied>::OPCODE);  // Opcode for
                                                               // TXA
  QNes::CPU_Testing::SetPC(cpu, start_address);
  // Act
  // Simulate the CPU cycles for TXA
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // A register should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

TEST_P(RegisterTransferTest, TYAInstructionTransfersYtoA) {
  // Arrange
  const u8 test_value = p_value;
  QNes::CPU_Testing::SetY(cpu, test_value);
  QNes::CPU_Testing::SetA(cpu, 0xFD);
  // Simulate the instruction fetch cycle for TYA
  constexpr u16 start_address = 0x0000;  // Program Counter starts at 0x0000
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(
      start_address,
      QNes::ISA::TYA<QNes::AddressingMode::Implied>::OPCODE);  // Opcode for
                                                               // TYA
  QNes::CPU_Testing::SetPC(cpu, start_address);
  // Act
  // Simulate the CPU cycles for TYA
  for (int cycle = 0; cycle < 2; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.a,
            test_value);  // A register should hold the loaded value
  EXPECT_EQ(cpu_state.status.zero,
            test_value == 0);  // Zero flag should be set if value is 0
  EXPECT_EQ(cpu_state.status.negative,
            (test_value & 0x80) != 0);  // Negative flag set if bit 7 is set
  EXPECT_EQ(cpu_state.pc, start_address + 1);  // PC should advance by 1
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu),
            0);  // Cycle should reset to 0
}

INSTANTIATE_TEST_SUITE_P(RegisterTransfer,      // Instance name
                         RegisterTransferTest,  // The test fixture
                         ::testing::Values(1, 2, 0x42, 254, 255, 0xFF, 0, -1,
                                           -2, -10, -254,
                                           -255)  // The test data
);
