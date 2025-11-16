#include "qnes_mapper.hpp"

#include "qnes_cartridge.hpp"

namespace QNes {

u8 Mapper::CPURead(u16 address) const {
  return cartridge->prg_rom_data[address - 0x8000];
}

void Mapper::CPUWrite(u16 address, u8 value) {
  ASSERT(false, "Invalid address");
}

u8 Mapper::PPURead(u16 address) const { return 0; }

void Mapper::PPUWrite(u16 address, u8 value) { return; }

}  // namespace QNes