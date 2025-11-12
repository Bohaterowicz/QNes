#include "qnes_ppu.hpp"

#include "qnes_bus.hpp"

namespace QNes {

constexpr u16 PPU_16_BIT_MASK = 0x3FFF;

constexpr u8 PPU_STATUS_VBLANK_STARTED_MASK = 0x80;

constexpr u8 PPU_CTRL_VRAM_ADDRESS_INCREMENT_MASK = 0b00000100;

constexpr u16 TEMP_VRAM_COARSE_X_MASK = 0b000000000000011111;
constexpr u16 TEMP_VRAM_COARSE_Y_MASK = 0b000000001111100000;
constexpr u16 FINE_Y_SCROLL_MASK = 0b000111000000000000;
constexpr u8 FINE_X_SCROLL_MASK = 0b00000111;

constexpr u16 TEMP_VRAM_HIGH_ADDRESS_MASK = 0b0011111100000000;
constexpr u16 TEMP_VRAM_LOW_ADDRESS_MASK = 0b0000000011111111;

constexpr u16 TEMP_VRAM_NAME_TABLE_SELECTMASK = 0b00000110000000000;

constexpr u8 PPU_MASK_SHOW_BACKGROUND = 0b00001000;
constexpr u8 PPU_MASK_SHOW_SPRITES = 0b00010000;
constexpr u8 PPU_RENDERING_MASK =
    PPU_MASK_SHOW_BACKGROUND | PPU_MASK_SHOW_SPRITES;

void PPU::UpdateRenderingToggle() {
  if (rendering_toggle_scheduled) {
    if (rendering_toggle_cycles_to_wait == 0) {
      rendering_toggle_scheduled = false;
      registers.ppu_mask = (registers.ppu_mask & ~PPU_RENDERING_MASK) |
                           (new_rendering_flags & PPU_RENDERING_MASK);
      this->new_rendering_flags = 0;
    } else {
      --rendering_toggle_cycles_to_wait;
    }
  }
}

void PPU::Step() { UpdateRenderingToggle(); }

void PPU::ScheduleRenderingToggle(u8 new_rendering_flags, int cycles_to_wait) {
  rendering_toggle_scheduled = true;
  rendering_toggle_cycles_to_wait = cycles_to_wait;
  this->new_rendering_flags = new_rendering_flags;
}

u8 PPU::BusReadMappedRegister(u8 address) {
  switch (address) {
    case 2:
      return ReadPPUSTATUS();
    case 4:
      return registers.oam_data;
    case 7:
      return ReadPPUDATA();
    default:
      ASSERT(false, "Invalid PPU register address for read operation");
      return 0xFD;
  }
}

void PPU::BusWriteMappedRegister(u8 address, u8 value) {
  switch (address) {
    case 0:
      WritePPUCONTROL(value);
      break;
    case 1:
      WritePPUMASK(value);
      break;
    case 3:
      registers.oam_address = value;
      break;
    case 4:
      registers.oam_data = value;
      break;
    case 5:
      WritePPUSCROLL(value);
      break;
    case 6:
      WritePPUADDR(value);
      break;
    case 7:
      WritePPUDATA(value);
      break;
    default:
      ASSERT(false, "Invalid PPU register address for write operation");
      break;
  }
}

bool PPU::IsRenderingEnabled() const {
  return (registers.ppu_mask &
          (PPU_MASK_SHOW_BACKGROUND | PPU_MASK_SHOW_SPRITES)) != 0;
}

bool PPU::IsRenderingActive() const {
  return IsRenderingEnabled() && (scanline_idx < 240 || scanline_idx == 261);
}

u8 PPU::ReadPPUSTATUS() {
  // reading PPUSTATUS has the side effect of resetting the write toggle
  internal_registers.write_toggle = 0;
  auto value = registers.ppu_status;
  // another effect of reading PPUSTATUS is that it clears the vblank started
  // flag (bit 7) - but returns its value before clearing
  registers.ppu_status &= ~PPU_STATUS_VBLANK_STARTED_MASK;
  return value;
}

u8 PPU::ReadPPUDATA() {
  // NOTE: reading PPUDATA does not return current value of the VRAM address,
  // but rather the value of the previous read which was stored in the
  // ppu_data_buffer, and the current read fills ppu_data_buffer with value of
  // the current address read (buffered read)
  u16 address = internal_registers.current_vram_address & PPU_16_BIT_MASK;
  u8 result = 0;
  if (address > 0x3F00) {
    // name/pattern table read
    result = ppu_data_buffer;
  } else {
    // internal palette read (maybe this should go trough bus, but then the bus
    // would need to have pointer to ppu, while the ppu needs pointer to bus,
    // which makes it a bit akward)

    // bit-and with 0x1F to get the index into the palette RAM - mirroring
    result = palette_ram[address & 0x1F];
    // even if this was a palette read we still need to read from the bus
    address -= 0x1000;
  }

  ppu_bus->SetAddress(address);
  ppu_data_buffer = ppu_bus->Read();

  IncrementVRAMAddress();

  return result;
}

void PPU::WritePPUCONTROL(u8 value) {
  registers.ppu_control = value;
  internal_registers.temp_vram_address =
      (internal_registers.temp_vram_address &
       ~TEMP_VRAM_NAME_TABLE_SELECTMASK) |
      (value & TEMP_VRAM_NAME_TABLE_SELECTMASK);

  // this may need to happen 1 tick later...
  if (!IsRenderingEnabled()) {
    internal_registers.current_vram_address =
        internal_registers.temp_vram_address;
  }
}

void PPU::WritePPUMASK(u8 value) {
  bool previous_rendering_enabled = IsRenderingEnabled();
  u8 new_rendering_flags = value & PPU_RENDERING_MASK;
  bool new_rendering_enabled = new_rendering_flags != 0;
  if (previous_rendering_enabled != new_rendering_enabled) {
    // we are toggling rendering enabled state
    // from NESDEV (https://www.nesdev.org/wiki/PPU_registers#PPUMASK):
    // Toggling rendering takes effect approximately 3-4 dots after the write.
    // This delay is required by Battletoads to avoid a crash.
    // Thus we schedule a the rendering toggle to happen in 3 cycles
    ScheduleRenderingToggle(new_rendering_flags, 4);
    // remove rendering flags from the value - they will be set by the scheduled
    // rendering toggle
    value &= ~PPU_RENDERING_MASK;
    registers.ppu_mask = value;
  } else {
    // no rendering toggle needed - just set the new value
    registers.ppu_mask = value;
  }
}

void PPU::WritePPUSCROLL(u8 value) {
  if (internal_registers.write_toggle) {
    // second write
    registers.ppu_scroll[1] = value;
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~TEMP_VRAM_COARSE_Y_MASK) |
        (value & TEMP_VRAM_COARSE_Y_MASK);
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~FINE_Y_SCROLL_MASK) |
        (value & FINE_Y_SCROLL_MASK);
  } else {
    // first write
    registers.ppu_scroll[0] = value;
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~TEMP_VRAM_COARSE_X_MASK) |
        (value & TEMP_VRAM_COARSE_X_MASK);
    internal_registers.fine_x_scroll = (value & FINE_X_SCROLL_MASK);
  }
  internal_registers.write_toggle ^= 1;
}

void PPU::WritePPUADDR(u8 value) {
  if (internal_registers.write_toggle) {
    // second write
    registers.ppu_address[1] = value;
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~TEMP_VRAM_LOW_ADDRESS_MASK) |
        (value & TEMP_VRAM_LOW_ADDRESS_MASK);

    // this may need to happen 1 tick later...
    // https://www.nesdev.org/wiki/PPU_scrolling#$2006_(PPUADDR)_second_write_(w_is_1)
    if (!IsRenderingEnabled()) {
      internal_registers.current_vram_address =
          internal_registers.temp_vram_address;
    }
  } else {
    // first write
    registers.ppu_address[0] = value;
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~TEMP_VRAM_HIGH_ADDRESS_MASK) |
        (value & TEMP_VRAM_HIGH_ADDRESS_MASK);
    internal_registers.temp_vram_address =
        0 | (internal_registers.temp_vram_address & ~(1 << 15));
  }
  internal_registers.write_toggle ^= 1;
}

void PPU::WritePPUDATA(u8 value) {
  u16 address = internal_registers.current_vram_address & PPU_16_BIT_MASK;
  if (address > 0x3F00) {
    // internal palette write
    palette_ram[address & 0x1F] = value;
  } else {
    // name/pattern table write
    ppu_bus->SetAddress(address);
    ppu_bus->Write(value);
  }
  IncrementVRAMAddress();
}

void PPU::IncrementVRAMAddress() {
  if (IsRenderingActive()) {
    // mid-frame PPUDATA read - special increment bahaviour (rendering based
    // increment behaviour)
    VRAMIncrementCoarseX();
    VRAMIncrementFineY();
  } else {
    // rendering not active (no pixel generation is happening) - normal vram
    // increment
    internal_registers.current_vram_address +=
        ((registers.ppu_control & PPU_CTRL_VRAM_ADDRESS_INCREMENT_MASK) != 0)
            ? 32
            : 1;
  }
}

void PPU::VRAMIncrementCoarseX() {
  if ((internal_registers.current_vram_address & 0x001F) == 31) {
    internal_registers.current_vram_address &= ~0x001F;
    internal_registers.current_vram_address ^= 0x0400;
  } else {
    internal_registers.current_vram_address += 1;
  }
}

void PPU::VRAMIncrementFineY() {
  if ((internal_registers.current_vram_address & 0x7000) != 0x7000) {
    internal_registers.current_vram_address += 0x1000;
  } else {
    internal_registers.current_vram_address &= ~0x7000;
    auto y = (internal_registers.current_vram_address & 0x03E0) >> 5;
    if (y == 29) {
      y = 0;
      internal_registers.current_vram_address ^= 0x0800;
    } else if (y == 31) {
      y = 0;
    } else {
      y += 1;
    }
    internal_registers.current_vram_address =
        (internal_registers.current_vram_address & ~0x03E0) | (y << 5);
  }
}

}  // namespace QNes