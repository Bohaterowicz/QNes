
#include <gtest/gtest.h>

#include <array>
#include <cstring>

#include "qnes_bits.hpp"
#include "qnes_bus.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {

class NESBusPPURegisterMirroringTest : public ::testing::Test {
 protected:
  NESBusPPURegisterMirroringTest()
      : ram_memory(Kilobytes(2)),
        vram_memory(Kilobytes(2)),
        ppu_bus(&vram_memory),
        ppu(&ppu_bus, nullptr),
        bus(&ram_memory, &dma_ctrl, &ppu, nullptr),
        cpu(&bus) {
    bus.SetCPU(&cpu);
    dma_ctrl.SetBus(&bus);
    ResetState();
  }

  void ResetState() {
    ram_memory.Clear();
    vram_memory.Clear();
    PPU_Testing::SetPPUStatus(ppu, 0);
    PPU_Testing::SetPPUControl(ppu, 0);
    PPU_Testing::SetPPUMask(ppu, 0);
    PPU_Testing::GetInternalRegisters(ppu) = {};
  }

  void SetAddress(u16 address) {
    bus.SetAddress(U16High(address), U16Low(address));
  }

  static constexpr std::array<u16, 4> kBaseMirrors = {0x2000, 0x2008, 0x2FF8,
                                                      0x3FF8};
  static constexpr std::array<u16, 3> kReadableOffsets = {0x0002, 0x0004,
                                                          0x0007};
  static constexpr std::array<u16, 7> kWritableOffsets = {
      0x0000, 0x0001, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007};

  Memory ram_memory;
  Memory vram_memory;
  DMAController dma_ctrl;
  VRAM_Only_PPUBus ppu_bus{&vram_memory};
  PPU ppu{&ppu_bus, nullptr};
  NESBus bus;
  CPU cpu;
};

TEST_F(NESBusPPURegisterMirroringTest,
       ReadsReturnValuesFromAnyMirroredAddress) {
  PPU_Testing::SetPPUStatus(ppu, 0xA1);
  PPU_Testing::SetOAMData(ppu, 0xB2);
  auto ppudata_value = 0xC3;

  for (u16 base : kBaseMirrors) {
    for (u16 offset : kReadableOffsets) {
      const u16 address = static_cast<u16>(base + offset);
      ASSERT_LT(address, 0x4000);

      SetAddress(address);

      const u8 expected = [offset, ppudata_value, this]() -> u8 {
        switch (offset & 0x0007) {
          case 0x0002:
            return PPU_Testing::GetPPUStatus(ppu);
          case 0x0004:
            return PPU_Testing::GetOAMData(ppu, 0);
          case 0x0007:
            return ppudata_value;
          default:
            return 0x00;
        }
      }();

      PPU_Testing::SetPPUDataBuffer(ppu, ppudata_value);
      PPU_Testing::SetVRAMAddress(ppu, 0x3711);
      EXPECT_EQ(bus.Read(), expected)
          << std::hex << "Read mismatch at 0x" << address << " (offset 0x"
          << (offset & 0x0007) << ")";
    }
  }
}

TEST_F(NESBusPPURegisterMirroringTest, WritesTargetSameRegisterAcrossMirrors) {
  u8 value = 0x10;

  auto& internal_registers = PPU_Testing::GetInternalRegisters(ppu);

  for (u16 offset : kWritableOffsets) {
    ResetState();

    const u16 canonical_address = static_cast<u16>(0x2000 + offset);
    SetAddress(canonical_address);
    PPU_Testing::SetVRAMAddress(ppu, 0x2AAA);
    bus.Write(value);

    const auto expected_ppu_status = PPU_Testing::GetPPUStatus(ppu);
    const auto expected_ppu_control = PPU_Testing::GetPPUControl(ppu);
    const auto expected_ppu_mask = PPU_Testing::GetPPUMask(ppu);
    const auto expected_internal = internal_registers;

    for (u16 base : kBaseMirrors) {
      ResetState();
      SetAddress(static_cast<u16>(base + offset));
      PPU_Testing::SetVRAMAddress(ppu, 0x2AAA);
      bus.Write(value);

      EXPECT_TRUE(PPU_Testing::GetPPUStatus(ppu) == expected_ppu_status &&
                  PPU_Testing::GetPPUControl(ppu) == expected_ppu_control &&
                  PPU_Testing::GetPPUMask(ppu) == expected_ppu_mask)
          << std::hex << "Register mismatch for offset 0x" << (offset & 0x0007)
          << " at mirror base 0x" << base;
      EXPECT_EQ(expected_internal.write_toggle, internal_registers.write_toggle)
          << std::hex << "Write toggle mismatch for offset 0x"
          << (offset & 0x0007) << " at mirror base 0x" << base;
    }

    ++value;
  }
}

}  // namespace QNes
