#include <gtest/gtest.h>

#include "qnes_bits.hpp"
#include "qnes_bus.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {
namespace {

class OAMDMATest : public ::testing::Test {
 protected:
  OAMDMATest()
      : cpu_memory(Kilobytes(2)),
        vram_memory(Kilobytes(64)),
        ppu_bus(&vram_memory),
        ppu(&ppu_bus, nullptr),
        bus(&cpu_memory, &dma_ctrl, &ppu, nullptr),
        cpu(&bus) {
    bus.SetCPU(&cpu);
    dma_ctrl.SetBus(&bus);
    ResetState();
  }

  void SetUp() override { ResetState(); }

  void ResetState() {
    cpu_memory.Clear();
    vram_memory.Clear();
    // Ensure OAMADDR starts from zero before each transfer, as on hardware.
    SetOAMAddress(0x00);
  }

  void WriteRegister(u16 address, u8 value) {
    bus.SetAddress(address);
    bus.Write(value);
  }

  u8 ReadRegister(u16 address) {
    bus.SetAddress(U16High(address), U16Low(address));
    return bus.Read();
  }

  void SetOAMAddress(u8 value) { WriteRegister(0x2003, value); }

  u8 ReadOAMByte(u8 index) {
    SetOAMAddress(index);
    return ReadRegister(0x2004);
  }

  void WriteSourcePage(u8 page, u8 seed) {
    const u16 base = static_cast<u16>(page) << 8;
    for (u16 offset = 0; offset < 256; ++offset) {
      cpu_memory.Write(static_cast<u16>(base + offset),
                       static_cast<u8>(seed + offset));
    }
  }

  void ExpectOAMMatchesSourcePage(u8 page) {
    const u16 base = static_cast<u16>(page) << 8;
    for (u16 offset = 0; offset < 256; ++offset) {
      const u8 expected = cpu_memory.Read(static_cast<u16>(base + offset));
      EXPECT_EQ(ReadOAMByte(static_cast<u8>(offset)), expected)
          << "Mismatch at OAM byte " << offset;
    }
  }

  void RequestOAMDMA(u8 page) { WriteRegister(0x4014, page); }

  int RunDMAUntilComplete() {
    int cycles = 0;
    while (dma_ctrl.IsDMAActive()) {
      dma_ctrl.Step();
      ++cycles;
    }
    // subtract 1 cycle because the last step means the DMA finsihed on previous cycle
    return --cycles;
  }

  void PrimeDMAForAlignedStart() { dma_ctrl.Step(); }

  Memory cpu_memory;
  Memory vram_memory;
  DMAController dma_ctrl;
  VRAM_Only_PPUBus ppu_bus;
  PPU ppu;
  NESBus bus;
  CPU cpu;
};

TEST_F(OAMDMATest, DMATransferCompletesIn513CyclesWhenAlreadyAligned) {
  constexpr u8 kPage = 0x02;
  constexpr u8 kSeed = 0x10;

  WriteSourcePage(kPage, kSeed);
  SetOAMAddress(0x00);

  // Prime DMA so the next cycle begins on a put cycle, eliminating alignment.
  PrimeDMAForAlignedStart();
  RequestOAMDMA(kPage);

  const int cycles = RunDMAUntilComplete();

  ExpectOAMMatchesSourcePage(kPage);
  EXPECT_EQ(cycles, 513);
}

TEST_F(OAMDMATest, DMATransferCompletesIn514CyclesWhenAlignmentNeeded) {
  constexpr u8 kPage = 0x03;
  constexpr u8 kSeed = 0x80;

  WriteSourcePage(kPage, kSeed);
  SetOAMAddress(0x00);

  RequestOAMDMA(kPage);

  const int cycles = RunDMAUntilComplete();

  ExpectOAMMatchesSourcePage(kPage);
  EXPECT_EQ(cycles, 514);
}

}  // namespace
}  // namespace QNes
