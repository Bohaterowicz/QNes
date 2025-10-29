#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class IndirectBasicTest : public ::testing::Test {
 public:
  IndirectBasicTest() : memory(Kilobytes(64)), cpu(memory) {}

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

TEST_F(IndirectBasicTest, JMPIndirect_Simple) {
  // Arrange
  constexpr u16 start_address = 0x0000;
  constexpr u16 vector_address = 0x0200;
  constexpr u16 target_address = 0xBEEF;

  // Encode JMP ($0200)
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::JMP<QNes::AddressingMode::Indirect>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(vector_address));
  memory.Write(start_address + 2, QNes::U16High(vector_address));

  // Vector contents -> actual target address
  memory.Write(vector_address, QNes::U16Low(target_address));
  memory.Write(vector_address + 1, QNes::U16High(target_address));

  // Act (3 cycles per ISA)
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // Assert
  EXPECT_EQ(cpu_state.pc, target_address);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(IndirectBasicTest, JMPIndirect_PageBoundaryWrapBug) {
  // Arrange
  constexpr u16 start_address = 0x0000;
  constexpr u16 vector_address = 0x02FF;  // page boundary

  // Intentionally set different values at 0x0300 and 0x0200 to detect wrap
  // According to 6502 bug, MSB is read from 0x0200 instead of 0x0300
  const u8 target_low = 0x34;
  const u8 msb_at_0200 = 0x12;  // expected high byte due to bug
  const u8 msb_at_0300 = 0xAB;  // would be the correct high byte without bug

  // Encode JMP ($02FF)
  QNes::CPU_Testing::SetPC(cpu, start_address);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start_address,
               QNes::ISA::JMP<QNes::AddressingMode::Indirect>::OPCODE);
  memory.Write(start_address + 1, QNes::U16Low(vector_address));
  memory.Write(start_address + 2, QNes::U16High(vector_address));

  // Place vector low at 0x02FF and two different highs at 0x0200 and 0x0300
  memory.Write(vector_address, target_low);  // 0x02FF
  memory.Write(0x0200, msb_at_0200);         // wrap-around high byte
  memory.Write(0x0300, msb_at_0300);         // non-bug high byte

  // Act (3 cycles per ISA)
  for (int cycle = 0; cycle < 5; ++cycle) {
    cpu.Step();
  }

  auto cpu_state = cpu.GetState();

  // With the 6502 bug, target = 0x12<<8 | 0x34 = 0x1234
  const u16 expected_bug_target = 0x1234;
  EXPECT_EQ(cpu_state.pc, expected_bug_target);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}
