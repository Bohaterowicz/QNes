#include "qnes_bus.hpp"

namespace QNes {

u8 NESBus::Read() {
  if (addr < 0x2000) {
    // Internal RAM (2 KB, mirrored)
    // mask the address to 0x07FF effectively truncating the address to 11 bits
    return memory->Read(addr & 0x07FF);
  } else if (addr < 0x4000) {
    ASSERT(ppu != nullptr, "PPU is not initialized");
    // PPU registers (8 bytes mirrored)
    // mask the address to 0x0007 effectively truncating the address to 3 bits
    // NOTE: Only PPUSTATUS, OAMDATA, and PPUDATA registers are readable by the
    // external bus
    const u8 masked_addr = addr & 0x0007;
    ASSERT(masked_addr == 2 || masked_addr == 4 || masked_addr == 7,
           "Invalid PPU register read address");
    return ppu->BusReadMappedRegister(masked_addr);
  }
  ASSERT(false, "Invalid address");
  return 0;
}

void NESBus::Write(u8 value) {
  if (addr < 0x2000) {
    // Internal RAM (2 KB, mirrored)
    // mask the address to 0x07FF effectively truncating the address to 11 bits
    memory->Write(addr & 0x07FF, value);
  } else if (addr < 0x4000) {
    ASSERT(ppu != nullptr, "PPU is not initialized");
    // PPU registers (8 bytes mirrored)
    // mask the address to 0x0007 effectively truncating the address to 3 bits
    // NOTE: Only PPUCONTROL, PPUMASK, OAMADDR, OAMDATA, PPUSCROLL, PPUADDRESS,
    // PPUDATA registers are writable by the external bus
    const u8 masked_addr = addr & 0x0007;
    ASSERT(masked_addr == 0 || masked_addr == 1 || masked_addr == 3 ||
               masked_addr == 4 || masked_addr == 5 || masked_addr == 6 ||
               masked_addr == 7,
           "Invalid PPU register write address");
    ppu->BusWriteMappedRegister(masked_addr, value);
  } else if (addr == 0x4014) {
    // DO DMA transfer
    ASSERT(false, "Invalid address");
  } else {
    ASSERT(false, "Invalid address");
  }
  return;
}

u8 PPUBus::Read() { return vram->Read(addr); }

void PPUBus::Write(u8 value) { vram->Write(addr, value); }

}  // namespace QNes