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
    return ppu->Read(masked_addr);
  } else {
    ASSERT(addr >= 0x8000, "Invalid address");
    // Cartrige access
    return cartridge->CPURead(addr);
  }
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
    ppu->Write(masked_addr, value);
  } else if (addr == 0x4014) {
    dma_controller->RequestDMA(value);
  } else {
    ASSERT(addr >= 0x8000, "Invalid address");
    // Cartrige access
    cartridge->CPUWrite(addr, value);
  }
}

u8 VRAM_Only_PPUBus::Read() {
  auto masked_addr = addr & 0x3FFF;
  return vram->Read(masked_addr);
}

void VRAM_Only_PPUBus::Write(u8 value) {
  auto masked_addr = addr & 0x3FFF;
  vram->Write(masked_addr, value);
}

u8 PPUBus::Read() {
  auto masked_addr = addr & 0x3FFF;
  if (addr < 0x2000) {
    return cartridge->PPURead(masked_addr);
  } else if (addr < 0x3F00) {
    return vram->Read(masked_addr - 0x2000);
  } else {
    // palette access - should be handled in ppu before bus read
    ASSERT(false, "Palette access should be handled in ppu before bus read");
    return 0;
  }
}

void PPUBus::Write(u8 value) {
  auto masked_addr = addr & 0x3FFF;
  if (addr < 0x2000) {
    cartridge->PPUWrite(masked_addr, value);
  } else if (addr < 0x3F00) {
    vram->Write(masked_addr - 0x2000, value);
  } else {
    ASSERT(false, "Palette access should be handled in ppu before bus write");
  }
}

}  // namespace QNes