
#include <gtest/gtest.h>

#include <array>

#include "qnes_bits.hpp"
#include "qnes_bus.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {
namespace {

class NESBusMirroringTest : public ::testing::Test {
 protected:
  // NOTE: PPU is not used in this test
  NESBusMirroringTest() : memory(Kilobytes(2)), bus(&memory, nullptr) {
    memory.Clear();
  }

  void SetAddress(u16 address) {
    bus.SetAddress(U16High(address), U16Low(address));
  }

  static constexpr std::array<u16, 4> kMirrors = {0x0000, 0x0800, 0x1000,
                                                  0x1800};

  Memory memory;
  NESBus bus;
};

TEST_F(NESBusMirroringTest, WriteMirrorsEvery2KB) {
  constexpr std::array<u16, 3> offsets = {0x0000, 0x0010, 0x07FF};

  u8 write_value = 0x10;

  for (u16 offset : offsets) {
    memory.Write(offset, 0x00);

    for (u16 base : kMirrors) {
      const u16 mirrored_address = static_cast<u16>(base + offset);
      ASSERT_LT(mirrored_address, 0x2000);

      SetAddress(mirrored_address);
      bus.Write(write_value);

      EXPECT_EQ(memory.Read(offset), write_value)
          << std::hex << "Expected mirrored write at base 0x" << offset
          << " from address 0x" << mirrored_address;

      // Modify the write value so consecutive mirrors don't accidentally pass
      ++write_value;
    }
  }
}

TEST_F(NESBusMirroringTest, ReadMirrorsAcrossInternalRam) {
  constexpr std::array<u16, 3> offsets = {0x0001, 0x0200, 0x07FE};
  u8 value = 0x80;

  for (u16 offset : offsets) {
    memory.Write(offset, value);

    for (u16 base : kMirrors) {
      const u16 mirrored_address = static_cast<u16>(base + offset);
      ASSERT_LT(mirrored_address, 0x2000);

      SetAddress(mirrored_address);
      EXPECT_EQ(bus.Read(), value)
          << std::hex << "Expected mirrored read at base 0x" << offset
          << " from address 0x" << mirrored_address;
    }

    ++value;
  }
}

}  // namespace
}  // namespace QNes
