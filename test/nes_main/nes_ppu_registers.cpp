
#include <gtest/gtest.h>

#include <cstring>

#include "qnes_bits.hpp"
#include "qnes_bus.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {
namespace {

class PPURegistersTest : public ::testing::Test {
 protected:
  PPURegistersTest() : memory(Kilobytes(2)), bus(&memory, &ppu) {
    ResetState();
  }

  void ResetState() {
    memory.Clear();
    auto &regs = PPU_Testing::GetRegisters(ppu);
    auto &internal = PPU_Testing::GetInternalRegisters(ppu);
    std::memset(&regs, 0, sizeof(regs));
    std::memset(&internal, 0, sizeof(internal));
  }

  void SetAddress(u16 address) {
    bus.SetAddress(U16High(address), U16Low(address));
  }

  Memory memory;
  PPU ppu{nullptr, nullptr};
  NESBus bus;
};

TEST_F(PPURegistersTest,
       PPUSTATUSReadReturnsLatchedValueThenClearsFlagsAndToggle) {
  auto &regs = PPU_Testing::GetRegisters(ppu);
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  regs.ppu_status = 0b1110'0011;  // bits 7-5 set, low bits set
  internal.write_toggle = 1;

  SetAddress(0x2002);
  const u8 read_value = bus.Read();

  EXPECT_EQ(read_value, 0b1110'0011);
  EXPECT_EQ(regs.ppu_status, 0b0110'0011);  // vblank cleared
  EXPECT_EQ(internal.write_toggle, 0);      // toggle cleared
}

TEST_F(PPURegistersTest,
       PPUSTATUSConsecutiveReadsReturnClearedValueAndKeepToggleCleared) {
  auto &regs = PPU_Testing::GetRegisters(ppu);
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  regs.ppu_status = 0b1000'0000;  // only vblank set
  internal.write_toggle = 1;

  SetAddress(0x2002);
  const u8 first_read = bus.Read();

  EXPECT_EQ(first_read, 0b1000'0000);
  EXPECT_EQ(regs.ppu_status, 0);  // vblank bit cleared
  EXPECT_EQ(internal.write_toggle, 0);

  SetAddress(0x2002);
  const u8 second_read = bus.Read();

  EXPECT_EQ(second_read, 0);            // remains cleared
  EXPECT_EQ(internal.write_toggle, 0);  // stays cleared
}

TEST_F(PPURegistersTest, PPUSTATUSMirroredAddressesBehaveIdentically) {
  auto &regs = PPU_Testing::GetRegisters(ppu);
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  regs.ppu_status = 0xFF;
  internal.write_toggle = 1;

  constexpr u16 mirrors[] = {0x2002, 0x200A, 0x2FFA, 0x3FFA};

  for (u16 addr : mirrors) {
    ResetState();
    regs.ppu_status = 0xFF;
    internal.write_toggle = 1;

    SetAddress(addr);
    const u8 read_value = bus.Read();

    EXPECT_EQ(read_value, 0xFF) << std::hex << "Mirror address 0x" << addr;
    EXPECT_EQ(regs.ppu_status, 0x7F) << std::hex << "Mirror address 0x" << addr;
    EXPECT_EQ(internal.write_toggle, 0)
        << std::hex << "Mirror address 0x" << addr;
  }
}

TEST_F(PPURegistersTest, PPUSCROLLRequiresTwoWrites) {
  ResetState();

  auto &registers = PPU_Testing::GetRegisters(ppu);
  auto &internal_registers = PPU_Testing::GetInternalRegisters(ppu);

  SetAddress(0x2005);
  bus.Write(0x12);

  EXPECT_EQ(registers.ppu_scroll[0], 0x12);
  EXPECT_EQ(registers.ppu_scroll[1], 0x00);
  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2005);
  bus.Write(0x34);

  EXPECT_EQ(registers.ppu_scroll[0], 0x12);
  EXPECT_EQ(registers.ppu_scroll[1], 0x34);
  EXPECT_EQ(internal_registers.write_toggle, 0);

  SetAddress(0x2005);
  bus.Write(0x56);

  EXPECT_EQ(registers.ppu_scroll[0], 0x56);
  EXPECT_EQ(registers.ppu_scroll[1], 0x34);
  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2005);
  bus.Write(0x78);

  EXPECT_EQ(registers.ppu_scroll[0], 0x56);
  EXPECT_EQ(registers.ppu_scroll[1], 0x78);
  EXPECT_EQ(internal_registers.write_toggle, 0);
}

TEST_F(PPURegistersTest, PPUADDRRequiresTwoWrites) {
  ResetState();

  auto &registers = PPU_Testing::GetRegisters(ppu);
  auto &internal_registers = PPU_Testing::GetInternalRegisters(ppu);

  SetAddress(0x2006);
  bus.Write(0xAB);

  EXPECT_EQ(registers.ppu_address[0], 0xAB);
  EXPECT_EQ(registers.ppu_address[1], 0x00);
  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2006);
  bus.Write(0xCD);

  EXPECT_EQ(registers.ppu_address[0], 0xAB);
  EXPECT_EQ(registers.ppu_address[1], 0xCD);
  EXPECT_EQ(internal_registers.write_toggle, 0);

  SetAddress(0x2006);
  bus.Write(0x9E);

  EXPECT_EQ(registers.ppu_address[0], 0x9E);
  EXPECT_EQ(registers.ppu_address[1], 0xCD);
  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2006);
  bus.Write(0xF0);

  EXPECT_EQ(registers.ppu_address[0], 0x9E);
  EXPECT_EQ(registers.ppu_address[1], 0xF0);
  EXPECT_EQ(internal_registers.write_toggle, 0);
}

}  // namespace
}  // namespace QNes
