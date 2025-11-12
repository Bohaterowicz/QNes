
#include <gtest/gtest.h>

#include <array>
#include <cstring>

#include "qnes_bits.hpp"
#include "qnes_bus.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {
namespace {

class NESBusPPURegisterMirroringTest : public ::testing::Test {
 protected:
  NESBusPPURegisterMirroringTest()
      : ram_memory(Kilobytes(2)),
        vram_memory(Kilobytes(2)),
        bus(&ram_memory, &ppu) {
    ResetState();
  }

  void ResetState() {
    ram_memory.Clear();
    vram_memory.Clear();
    PPU_Testing::GetRegisters(ppu) = {};
    PPU_Testing::GetInternalRegisters(ppu) = {};
  }

  void SetAddress(u16 address) {
    bus.SetAddress(U16High(address), U16Low(address));
  }

  static bool RegistersEqual(const PPU::Registers& lhs,
                             const PPU::Registers& rhs) {
    return std::memcmp(&lhs, &rhs, sizeof(PPU::Registers)) == 0;
  }

  static constexpr std::array<u16, 4> kBaseMirrors = {0x2000, 0x2008, 0x2FF8,
                                                      0x3FF8};
  static constexpr std::array<u16, 3> kReadableOffsets = {0x0002, 0x0004,
                                                          0x0007};
  static constexpr std::array<u16, 7> kWritableOffsets = {
      0x0000, 0x0001, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007};

  Memory ram_memory;
  Memory vram_memory;
  PPUBus ppu_bus{&vram_memory};
  PPU ppu{&ppu_bus, nullptr};
  NESBus bus;
};

TEST_F(NESBusPPURegisterMirroringTest,
       ReadsReturnValuesFromAnyMirroredAddress) {
  auto& registers = PPU_Testing::GetRegisters(ppu);
  registers.ppu_status = 0xA1;
  registers.oam_data = 0xB2;
  auto ppudata_value = 0xC3;

  for (u16 base : kBaseMirrors) {
    for (u16 offset : kReadableOffsets) {
      const u16 address = static_cast<u16>(base + offset);
      ASSERT_LT(address, 0x4000);

      SetAddress(address);

      const u8 expected = [offset, ppudata_value, &registers]() -> u8 {
        switch (offset & 0x0007) {
          case 0x0002:
            return registers.ppu_status;
          case 0x0004:
            return registers.oam_data;
          case 0x0007:
            return ppudata_value;
          default:
            return 0x00;
        }
      }();

      PPU_Testing::SetPPUDataBuffer(ppu, ppudata_value);
      PPU_Testing::SetVRAMAddress(ppu, 0x3F11);
      EXPECT_EQ(bus.Read(), expected)
          << std::hex << "Read mismatch at 0x" << address << " (offset 0x"
          << (offset & 0x0007) << ")";
    }
  }
}

TEST_F(NESBusPPURegisterMirroringTest, WritesTargetSameRegisterAcrossMirrors) {
  u8 value = 0x10;

  auto& registers = PPU_Testing::GetRegisters(ppu);
  auto& internal_registers = PPU_Testing::GetInternalRegisters(ppu);

  for (u16 offset : kWritableOffsets) {
    ResetState();

    const u16 canonical_address = static_cast<u16>(0x2000 + offset);
    SetAddress(canonical_address);
    bus.Write(value);

    const auto expected_registers = registers;
    const auto expected_internal = internal_registers;

    for (u16 base : kBaseMirrors) {
      ResetState();
      SetAddress(static_cast<u16>(base + offset));
      bus.Write(value);

      EXPECT_TRUE(RegistersEqual(expected_registers, registers))
          << std::hex << "Register mismatch for offset 0x" << (offset & 0x0007)
          << " at mirror base 0x" << base;
      EXPECT_EQ(expected_internal.write_toggle, internal_registers.write_toggle)
          << std::hex << "Write toggle mismatch for offset 0x"
          << (offset & 0x0007) << " at mirror base 0x" << base;
    }

    ++value;
  }
}

}  // namespace
}  // namespace QNes
