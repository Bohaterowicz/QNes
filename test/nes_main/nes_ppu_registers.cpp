
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
  PPURegistersTest()
      : cpu_memory(Kilobytes(2)),
        vram_memory(Kilobytes(64)),
        ppu_bus(&vram_memory),  // VRAM only PPU bus
        ppu(&ppu_bus, nullptr),
        bus(&cpu_memory, &ppu, nullptr) {
    ResetState();
  }

  void SetUp() override { ResetState(); }

  void ResetState() {
    cpu_memory.Clear();
    vram_memory.Clear();
    auto &internal = PPU_Testing::GetInternalRegisters(ppu);
    PPU_Testing::SetPPUStatus(ppu, 0);
    PPU_Testing::SetPPUControl(ppu, 0);
    PPU_Testing::SetPPUMask(ppu, 0);
    std::memset(&internal, 0, sizeof(internal));
    PPU_Testing::SetPPUDataBuffer(ppu, 0);
    PPU_Testing::SetVRAMAddress(ppu, 0);
    PPU_Testing::SetPPUMask(ppu, 0);
    PPU_Testing::SetPPUControl(ppu, 0);
    PPU_Testing::SetScanline(ppu, 0);
    PPU_Testing::ClearRenderingToggle(ppu);
  }

  void SetAddress(u16 address) {
    bus.SetAddress(U16High(address), U16Low(address));
  }

  Memory cpu_memory;
  Memory vram_memory;
  VRAM_Only_PPUBus ppu_bus;
  PPU ppu;
  NESBus bus;
};

TEST_F(PPURegistersTest,
       PPUSTATUSReadReturnsLatchedValueThenClearsFlagsAndToggle) {
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  PPU_Testing::SetPPUStatus(ppu, 0b1110'0011);  // bits 7-5 set, low bits set
  internal.write_toggle = 1;

  SetAddress(0x2002);
  const u8 read_value = bus.Read();

  EXPECT_EQ(read_value, 0b1110'0011);
  EXPECT_EQ(PPU_Testing::GetPPUStatus(ppu), 0b0110'0011);  // vblank cleared
  EXPECT_EQ(internal.write_toggle, 0);                     // toggle cleared
}

TEST_F(PPURegistersTest,
       PPUSTATUSConsecutiveReadsReturnClearedValueAndKeepToggleCleared) {
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  PPU_Testing::SetPPUStatus(ppu, 0b1000'0000);  // only vblank set
  internal.write_toggle = 1;

  SetAddress(0x2002);
  const u8 first_read = bus.Read();

  EXPECT_EQ(first_read, 0b1000'0000);
  EXPECT_EQ(PPU_Testing::GetPPUStatus(ppu), 0);  // vblank bit cleared
  EXPECT_EQ(internal.write_toggle, 0);

  SetAddress(0x2002);
  const u8 second_read = bus.Read();

  EXPECT_EQ(second_read, 0);            // remains cleared
  EXPECT_EQ(internal.write_toggle, 0);  // stays cleared
}

TEST_F(PPURegistersTest, PPUSTATUSMirroredAddressesBehaveIdentically) {
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  PPU_Testing::SetPPUStatus(ppu, 0xFF);
  internal.write_toggle = 1;

  constexpr u16 mirrors[] = {0x2002, 0x200A, 0x2FFA, 0x3FFA};

  for (u16 addr : mirrors) {
    ResetState();
    PPU_Testing::SetPPUStatus(ppu, 0xFF);
    internal.write_toggle = 1;

    SetAddress(addr);
    const u8 read_value = bus.Read();

    EXPECT_EQ(read_value, 0xFF) << std::hex << "Mirror address 0x" << addr;
    EXPECT_EQ(PPU_Testing::GetPPUStatus(ppu), 0x7F)
        << std::hex << "Mirror address 0x" << addr;
    EXPECT_EQ(internal.write_toggle, 0)
        << std::hex << "Mirror address 0x" << addr;
  }
}

TEST_F(PPURegistersTest, PPUSCROLLRequiresTwoWrites) {
  ResetState();

  auto &internal_registers = PPU_Testing::GetInternalRegisters(ppu);

  SetAddress(0x2005);
  bus.Write(0x12);

  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2005);
  bus.Write(0x34);

  EXPECT_EQ(internal_registers.write_toggle, 0);

  SetAddress(0x2005);
  bus.Write(0x56);

  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2005);
  bus.Write(0x78);

  EXPECT_EQ(internal_registers.write_toggle, 0);
}

TEST_F(PPURegistersTest, PPUADDRRequiresTwoWrites) {
  ResetState();

  auto &internal_registers = PPU_Testing::GetInternalRegisters(ppu);

  SetAddress(0x2006);
  bus.Write(0xAB);

  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2006);
  bus.Write(0xCD);

  EXPECT_EQ(internal_registers.write_toggle, 0);

  SetAddress(0x2006);
  bus.Write(0x9E);

  EXPECT_EQ(internal_registers.write_toggle, 1);

  SetAddress(0x2006);
  bus.Write(0xF0);

  EXPECT_EQ(internal_registers.write_toggle, 0);
}

TEST_F(PPURegistersTest, ReadPPUDATANameTableBufferedReadAndIncrement) {
  ResetState();

  constexpr u16 kAddress = 0x2000;
  constexpr u8 kBufferedValue = 0xAA;
  constexpr u8 kFirstFetch = 0xBB;
  constexpr u8 kSecondFetch = 0xCC;

  PPU_Testing::SetVRAMAddress(ppu, kAddress);
  PPU_Testing::SetPPUDataBuffer(ppu, kBufferedValue);

  vram_memory.Write(kAddress, kFirstFetch);
  vram_memory.Write(kAddress + 1, kSecondFetch);

  const u8 first = PPU_Testing::ReadPPUDATA(ppu);
  EXPECT_EQ(first, kBufferedValue);
  EXPECT_EQ(PPU_Testing::GetPPUDataBuffer(ppu), kFirstFetch);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 1));

  const u8 second = PPU_Testing::ReadPPUDATA(ppu);
  EXPECT_EQ(second, kFirstFetch);
  EXPECT_EQ(PPU_Testing::GetPPUDataBuffer(ppu), kSecondFetch);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 2));
}

TEST_F(PPURegistersTest, ReadPPUDATAPaletteReadReturnsEntry) {
  constexpr u16 kAddress = 0x3F00;
  constexpr u8 kPaletteValue = 0x44;
  constexpr u8 kMemoryValue = 0x99;

  PPU_Testing::SetPaletteEntry(ppu, 0x00, kPaletteValue);
  PPU_Testing::SetPPUDataBuffer(ppu, 0xAA);
  PPU_Testing::SetVRAMAddress(ppu, kAddress);

  vram_memory.Write(static_cast<u16>(kAddress - 0x1000), kMemoryValue);

  const u8 result = PPU_Testing::ReadPPUDATA(ppu);

  EXPECT_EQ(result, kPaletteValue);
  EXPECT_EQ(PPU_Testing::GetPPUDataBuffer(ppu), kMemoryValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 1));
}

TEST_F(PPURegistersTest, ReadPPUDATAPaletteMirrorsIndex) {
  constexpr u16 kAddress = 0x3F20;
  constexpr u8 kPaletteValue = 0x11;
  constexpr u8 kMemoryValue = 0x77;

  PPU_Testing::SetPaletteEntry(ppu, 0x00, kPaletteValue);
  PPU_Testing::SetPPUDataBuffer(ppu, 0xAA);
  PPU_Testing::SetVRAMAddress(ppu, kAddress);

  vram_memory.Write(static_cast<u16>(kAddress - 0x1000), kMemoryValue);

  const u8 result = PPU_Testing::ReadPPUDATA(ppu);

  EXPECT_EQ(result, kPaletteValue);
  EXPECT_EQ(PPU_Testing::GetPPUDataBuffer(ppu), kMemoryValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 1));
}

TEST_F(PPURegistersTest, ReadPPUDATAIncrementsBy32WhenControlFlagSet) {
  constexpr u16 kAddress = 0x2000;
  constexpr u8 kBufferedValue = 0x10;
  constexpr u8 kFetchedValue = 0x20;

  PPU_Testing::SetVRAMAddress(ppu, kAddress);
  PPU_Testing::SetPPUDataBuffer(ppu, kBufferedValue);
  PPU_Testing::SetPPUControl(ppu, 0x04);

  vram_memory.Write(kAddress, kFetchedValue);

  const u8 result = PPU_Testing::ReadPPUDATA(ppu);

  EXPECT_EQ(result, kBufferedValue);
  EXPECT_EQ(PPU_Testing::GetPPUDataBuffer(ppu), kFetchedValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 32));
}

TEST_F(PPURegistersTest, ReadPPUDATARenderingActiveUsesScrollIncrement) {
  constexpr u16 kAddress = 0x001F;
  constexpr u8 kBufferedValue = 0x33;
  constexpr u8 kFetchedValue = 0x44;

  PPU_Testing::SetVRAMAddress(ppu, kAddress);
  PPU_Testing::SetPPUDataBuffer(ppu, kBufferedValue);
  PPU_Testing::SetPPUMask(ppu, 0x18);
  PPU_Testing::SetScanline(ppu, 0);

  vram_memory.Write(kAddress, kFetchedValue);

  const u8 result = PPU_Testing::ReadPPUDATA(ppu);

  EXPECT_EQ(result, kBufferedValue);
  EXPECT_EQ(PPU_Testing::GetPPUDataBuffer(ppu), kFetchedValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(0x1400));
}

TEST_F(PPURegistersTest, WritePPUDATANametableStoresValueAndIncrements) {
  constexpr u16 kAddress = 0x23AB;
  constexpr u8 kWriteValue = 0x5D;

  PPU_Testing::SetVRAMAddress(ppu, kAddress);
  PPU_Testing::WritePPUDATA(ppu, kWriteValue);

  EXPECT_EQ(vram_memory.Read(kAddress), kWriteValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 1));
}

TEST_F(PPURegistersTest, WritePPUDATAPaletteWriteUpdatesPaletteRamOnly) {
  constexpr u16 kAddress = 0x3F10;
  constexpr u8 kWriteValue = 0x8E;

  vram_memory.Write(kAddress, 0xAA);
  PPU_Testing::SetPaletteEntry(ppu, static_cast<u8>(kAddress & 0x1F), 0x00);
  PPU_Testing::SetVRAMAddress(ppu, kAddress);

  PPU_Testing::WritePPUDATA(ppu, kWriteValue);

  EXPECT_EQ(PPU_Testing::GetPaletteEntry(ppu, static_cast<u8>(kAddress & 0x1F)),
            kWriteValue);
  EXPECT_EQ(vram_memory.Read(kAddress), 0xAA);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 1));
}

TEST_F(PPURegistersTest, WritePPUDATAPaletteMirrorsIndex) {
  constexpr u16 kAddress = 0x3F30;
  constexpr u8 kWriteValue = 0x27;
  constexpr u8 kExpectedIndex = static_cast<u8>(kAddress & 0x1F);

  PPU_Testing::SetPaletteEntry(ppu, kExpectedIndex, 0x00);
  PPU_Testing::SetVRAMAddress(ppu, kAddress);

  PPU_Testing::WritePPUDATA(ppu, kWriteValue);

  EXPECT_EQ(PPU_Testing::GetPaletteEntry(ppu, kExpectedIndex), kWriteValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 1));
}

TEST_F(PPURegistersTest, WritePPUDATAIncrementsBy32WhenControlFlagSet) {
  constexpr u16 kAddress = 0x2000;
  constexpr u8 kWriteValue = 0x3C;

  PPU_Testing::SetVRAMAddress(ppu, kAddress);
  PPU_Testing::SetPPUControl(ppu, 0x04);

  PPU_Testing::WritePPUDATA(ppu, kWriteValue);

  EXPECT_EQ(vram_memory.Read(kAddress), kWriteValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(kAddress + 32));
}

TEST_F(PPURegistersTest, WritePPUDATARenderingActiveUsesScrollIncrement) {
  constexpr u16 kAddress = 0x001F;
  constexpr u8 kWriteValue = 0x6A;

  PPU_Testing::SetVRAMAddress(ppu, kAddress);
  PPU_Testing::SetPPUMask(ppu, 0x18);
  PPU_Testing::SetScanline(ppu, 0);

  PPU_Testing::WritePPUDATA(ppu, kWriteValue);

  EXPECT_EQ(vram_memory.Read(kAddress), kWriteValue);
  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), static_cast<u16>(0x1400));
}

TEST_F(PPURegistersTest, WritePPUCONTROLUpdatesTempNameTableBits) {
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  internal.temp_vram_address = 0x7FFF;

  constexpr u8 kValue = 0x03;
  constexpr u16 kMask = 0x0C00;

  PPU_Testing::WritePPUCONTROL(ppu, kValue);

  EXPECT_EQ(PPU_Testing::GetPPUControl(ppu), kValue);
  EXPECT_EQ(internal.temp_vram_address,
            static_cast<u16>((0x7FFF & ~kMask) | (kValue & kMask)));
}

TEST_F(PPURegistersTest, WritePPUCONTROLUpdatesCurrentAddressWhenRenderingOff) {
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  internal.temp_vram_address = 0x2468;
  PPU_Testing::SetVRAMAddress(ppu, 0x0000);
  PPU_Testing::SetPPUMask(ppu, 0x00);

  PPU_Testing::WritePPUCONTROL(ppu, 0x00);

  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), internal.temp_vram_address);
}

TEST_F(PPURegistersTest, WritePPUCONTROLPreservesCurrentWhenRenderingOn) {
  auto &internal = PPU_Testing::GetInternalRegisters(ppu);

  internal.temp_vram_address = 0x1357;
  PPU_Testing::SetVRAMAddress(ppu, 0x0ACE);
  PPU_Testing::SetPPUMask(ppu, 0x18);

  PPU_Testing::WritePPUCONTROL(ppu, 0x02);

  EXPECT_EQ(PPU_Testing::GetVRAMAddress(ppu), 0x0ACE);
  EXPECT_NE(internal.temp_vram_address, 0x0ACE);
}

TEST_F(PPURegistersTest, WritePPUMASKSchedulesToggleWhenEnablingRendering) {
  PPU_Testing::SetPPUMask(ppu, 0x00);
  PPU_Testing::WritePPUMASK(ppu, 0x1C);

  EXPECT_TRUE(PPU_Testing::IsRenderingToggleScheduled(ppu));
  EXPECT_EQ(PPU_Testing::RenderingToggleCyclesToWait(ppu), 3);
  EXPECT_EQ(PPU_Testing::PendingRenderingFlags(ppu), 0x18);
  EXPECT_EQ(PPU_Testing::GetPPUMask(ppu), static_cast<u8>(0x1C & ~0x18));

  for (int i = 0; i < 4; ++i) {
    ASSERT_TRUE(PPU_Testing::IsRenderingToggleScheduled(ppu));
    PPU_Testing::UpdateRenderingToggle(ppu);
  }

  EXPECT_FALSE(PPU_Testing::IsRenderingToggleScheduled(ppu));
  EXPECT_EQ(PPU_Testing::GetPPUMask(ppu) & 0x18, 0x18);
  EXPECT_EQ(PPU_Testing::PendingRenderingFlags(ppu), 0);
}

TEST_F(PPURegistersTest, WritePPUMASKSchedulesToggleWhenDisablingRendering) {
  PPU_Testing::SetPPUMask(ppu, 0x18);
  PPU_Testing::WritePPUMASK(ppu, 0x00);

  EXPECT_TRUE(PPU_Testing::IsRenderingToggleScheduled(ppu));
  EXPECT_EQ(PPU_Testing::RenderingToggleCyclesToWait(ppu), 3);
  EXPECT_EQ(PPU_Testing::PendingRenderingFlags(ppu), 0x00);
  EXPECT_EQ(PPU_Testing::GetPPUMask(ppu), 0x00);

  for (int i = 0; i < 4; ++i) {
    ASSERT_TRUE(PPU_Testing::IsRenderingToggleScheduled(ppu));
    PPU_Testing::UpdateRenderingToggle(ppu);
  }

  EXPECT_FALSE(PPU_Testing::IsRenderingToggleScheduled(ppu));
  EXPECT_EQ(PPU_Testing::GetPPUMask(ppu) & 0x18, 0x00);
}

TEST_F(PPURegistersTest, WritePPUMASKWithoutToggleSetsValueImmediately) {
  PPU_Testing::SetPPUMask(ppu, 0x04);
  PPU_Testing::WritePPUMASK(ppu, 0x04);

  EXPECT_FALSE(PPU_Testing::IsRenderingToggleScheduled(ppu));
  EXPECT_EQ(PPU_Testing::GetPPUMask(ppu), 0x04);
}

}  // namespace
}  // namespace QNes
