#include <gtest/gtest.h>

#include "cpu_isa.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

class RelativeBranchTest : public ::testing::Test {
 public:
  RelativeBranchTest() : memory(Kilobytes(64)), cpu(memory) {}

 protected:
  void SetUp() override {
    memory.Clear();
    QNes::CPU_Testing::SetGlobalMode(cpu, QNes::CPU::GlobalMode::RUN);
    QNes::CPU_Testing::SetPC(cpu, 0);
    QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  }

  QNes::Memory memory;
  QNes::CPU cpu;
};

// Helper to run a single branch instruction
static void WriteAndRunBranch(QNes::CPU &cpu, QNes::Memory &memory, u16 start,
                              u8 opcode, u8 rel_offset, int cycles) {
  QNes::CPU_Testing::SetPC(cpu, start);
  QNes::CPU_Testing::SetInstructionCycle(cpu, 0);
  memory.Write(start, opcode);
  memory.Write(start + 1, rel_offset);
  for (int i = 0; i < cycles; ++i) {
    cpu.Step();
  }
}

// Not taken: PC should advance by 2, cycles = 2
TEST_F(RelativeBranchTest, BCC_NotTaken) {
  QNes::CPU_Testing::SetCarry(cpu, true);
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BCC<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
  EXPECT_EQ(QNes::CPU_Testing::GetInstructionCycle(cpu), 0);
}

TEST_F(RelativeBranchTest, BCS_NotTaken) {
  QNes::CPU_Testing::SetCarry(cpu, false);
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BCS<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

TEST_F(RelativeBranchTest, BEQ_NotTaken) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.zero = 0;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BEQ<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

TEST_F(RelativeBranchTest, BMI_NotTaken) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.negative = 0;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BMI<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

TEST_F(RelativeBranchTest, BNE_NotTaken) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.zero = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BNE<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

TEST_F(RelativeBranchTest, BPL_NotTaken) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.negative = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BPL<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

TEST_F(RelativeBranchTest, BVC_NotTaken) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.overflow = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BVC<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

TEST_F(RelativeBranchTest, BVS_NotTaken) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.overflow = 0;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BVS<QNes::AddressingMode::Relative>::OPCODE,
                    0x10, 2);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2);
}

// Taken, no page cross, positive offset: cycles = 3
TEST_F(RelativeBranchTest, BNE_Taken_Positive_NoCross) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.zero = 0;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0000;
  constexpr u8 offset = 0x05;  // +5
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BNE<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 3);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2 + static_cast<i8>(offset));
}

// Taken, no page cross, negative offset: cycles = 3
TEST_F(RelativeBranchTest, BEQ_Taken_Negative_NoCross) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.zero = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0040;
  constexpr u8 offset = 0xFB;  // -5
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BEQ<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 3);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2 + static_cast<i8>(offset));
}

// Taken with page cross upward: cycles = 4
TEST_F(RelativeBranchTest, BCC_Taken_PageCross_Up) {
  QNes::CPU_Testing::SetCarry(cpu, false);
  constexpr u16 start = 0x00EE;  // start+2 = 0x00F0
  constexpr u8 offset = 0x7F;    // +127 -> 0x00F0 + 127 = 0x017F
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BCC<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 4);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc,
            static_cast<u16>(start + 2 + static_cast<i8>(offset)));
}

// Taken with page cross downward: cycles = 4
TEST_F(RelativeBranchTest, BCS_Taken_PageCross_Down) {
  QNes::CPU_Testing::SetCarry(cpu, true);
  constexpr u16 start = 0x00FE;  // start+2 = 0x0100
  constexpr u8 offset = 0xFF;    // -1 -> 0x00FF
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BCS<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 4);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc,
            static_cast<u16>(start + 2 + static_cast<i8>(offset)));
}

// Smoke: ensure each branch opcode can take a simple forward branch
TEST_F(RelativeBranchTest, BMI_Taken_Positive_NoCross) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.negative = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0020;
  constexpr u8 offset = 0x10;  // +16
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BMI<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 3);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2 + static_cast<i8>(offset));
}

TEST_F(RelativeBranchTest, BPL_Taken_Negative_NoCross) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.negative = 0;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0080;
  constexpr u8 offset = 0xF6;  // -10
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BPL<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 3);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2 + static_cast<i8>(offset));
}

TEST_F(RelativeBranchTest, BVC_Taken_Positive_NoCross) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.overflow = 0;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0100;
  constexpr u8 offset = 0x20;  // +32
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BVC<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 3);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc, start + 2 + static_cast<i8>(offset));
}

TEST_F(RelativeBranchTest, BVS_Taken_Negative_PageCross_Down) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.overflow = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0200;  // base = 0x0202
  constexpr u8 offset = 0xF0;    // -16 => target 0x01F2 (page cross)
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BVS<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 4);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc,
            static_cast<u16>(start + 2 + static_cast<i8>(offset)));
}

TEST_F(RelativeBranchTest, BVS_Taken_Negative_NoCross) {
  {
    QNes::CPU::StatusFlags flags{};
    flags.overflow = 1;
    QNes::CPU_Testing::SetStatus(cpu, flags);
  }
  constexpr u16 start = 0x0210;  // base = 0x0212
  constexpr u8 offset = 0xF0;    // -16 => target 0x0202 (no cross)
  WriteAndRunBranch(cpu, memory, start,
                    QNes::ISA::BVS<QNes::AddressingMode::Relative>::OPCODE,
                    offset, 3);
  auto state_after = cpu.GetState();
  EXPECT_EQ(state_after.pc,
            static_cast<u16>(start + 2 + static_cast<i8>(offset)));
}
