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
      ppu_mask = (ppu_mask & ~PPU_RENDERING_MASK) |
                 (new_rendering_flags & PPU_RENDERING_MASK);
      this->new_rendering_flags = 0;
    } else {
      --rendering_toggle_cycles_to_wait;
    }
  }
}

void PPU::ProcessScanlineCycle() {
  if (scanline_cycle == 0) {
    // first cycle of the scanline is a idle cycle
    return;
  }

  if (scanline_cycle < 257) {
    // 256 cycles

    // first fetch tile data - this process is spread across 8 cycle - 4 reads,
    // each read is 2 cycles.
    // 1. Read nametable byte
    // 2. Read attribute table byte
    // 3. Read pattern table low (plane 0)
    // 4. Read pattern table high (plane 1)
    // Again remember - each read is 2 cycles thus fetching those four values
    // takes 8 cycles.

    auto read_cycle = (scanline_cycle - 1) % 8;
    if (read_cycle < 2) {
      // first two reads are of nametable byte
      if (read_cycle == 0) {
        // prepare address for nametable byte read
        ppu_bus->SetAddress(internal_registers.current_vram_address);
      } else {
        // perform nametable byte read
        nametable_byte_latch = ppu_bus->Read();
      }
    }
  }
}

void PPU::Step() {
  UpdateRenderingToggle();

  ProcessScanlineCycle();

  ++scanline_cycle;
  if (scanline_cycle > 340) {
    ++scanline_idx;
    scanline_cycle = 0;
    if (scanline_idx > 261) {
      scanline_idx = 0;
    }
  }
}

void PPU::ScheduleRenderingToggle(u8 new_rendering_flags, int cycles_to_wait) {
  rendering_toggle_scheduled = true;
  rendering_toggle_cycles_to_wait = cycles_to_wait;
  this->new_rendering_flags = new_rendering_flags;
}

u8 PPU::Read(u8 address) {
  switch (address) {
    case 2:
      return ReadPPUSTATUS();
    case 4:
      return ReadOAMDATA();
    case 7:
      return ReadPPUDATA();
    default:
      ASSERT(false, "Invalid PPU register address for read operation");
      return 0xFD;
  }
}

void PPU::Write(u8 address, u8 value) {
  switch (address) {
    case 0:
      WritePPUCONTROL(value);
      break;
    case 1:
      WritePPUMASK(value);
      break;
    case 3:
      WriteOAMADDRESS(value);
      break;
    case 4:
      WriteOAMDATA(value);
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
  return (ppu_mask & (PPU_MASK_SHOW_BACKGROUND | PPU_MASK_SHOW_SPRITES)) != 0;
}

bool PPU::IsRenderingActive() const {
  return IsRenderingEnabled() && (scanline_idx < 240 || scanline_idx == 261);
}

u8 PPU::ReadPPUSTATUS() {
  // reading PPUSTATUS has the side effect of resetting the write toggle
  internal_registers.write_toggle = 0;
  auto value = ppu_status;
  // another effect of reading PPUSTATUS is that it clears the vblank started
  // flag (bit 7) - but returns its value before clearing
  ppu_status &= ~PPU_STATUS_VBLANK_STARTED_MASK;
  return value;
}

u8 PPU::ReadPPUDATA() {
  // NOTE: reading PPUDATA does not return current value of the VRAM address,
  // but rather the value of the previous read which was stored in the
  // ppu_data_buffer, and the current read fills ppu_data_buffer with value of
  // the current address read (buffered read)
  u16 address = internal_registers.current_vram_address & PPU_16_BIT_MASK;
  u8 result = 0;
  if (address >= 0x3F00) {
    // internal palette read (maybe this should go trough bus, but then the bus
    // would need to have pointer to ppu, while the ppu needs pointer to bus,
    // which makes it a bit akward)

    // bit-and with 0x1F to get the index into the palette RAM - mirroring
    result = palette_ram[address & 0x1F];
    // even if this was a palette read we still need to read from the bus
    address -= 0x1000;

  } else {
    // name/pattern table read
    result = ppu_data_buffer;
  }

  ppu_bus->SetAddress(address);
  ppu_data_buffer = ppu_bus->Read();

  IncrementVRAMAddress();

  return result;
}

u8 PPU::ReadOAMDATA() { return oam_data[oam_address]; }

void PPU::WritePPUCONTROL(u8 value) {
  ppu_control = value;
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
    ScheduleRenderingToggle(new_rendering_flags, 3);
    // remove rendering flags from the value - they will be set by the scheduled
    // rendering toggle
    value &= ~PPU_RENDERING_MASK;
  }
  ppu_mask = value;
}

void PPU::WritePPUSCROLL(u8 value) {
  if (internal_registers.write_toggle) {
    // second write
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~TEMP_VRAM_COARSE_Y_MASK) |
        (value & TEMP_VRAM_COARSE_Y_MASK);
    internal_registers.temp_vram_address =
        (internal_registers.temp_vram_address & ~FINE_Y_SCROLL_MASK) |
        (value & FINE_Y_SCROLL_MASK);
  } else {
    // first write
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
  if (address >= 0x3F00) {
    // internal palette write
    palette_ram[address & 0x1F] = value;
  } else {
    // name/pattern table write
    ppu_bus->SetAddress(address);
    ppu_bus->Write(value);
  }
  IncrementVRAMAddress();
}

void PPU::WriteOAMADDRESS(u8 value) { oam_address = value; }

void PPU::WriteOAMDATA(u8 value) {
  // According to NESDEV (https://www.nesdev.org/wiki/PPU_registers#OAMDATA)
  // writes during rendering do not affect OAM data, and also perform a glitchy
  // increment - for the sake of emulation we will ignore writes to oam data
  // during rendering for now
  if (!IsRenderingActive()) {
    oam_data[oam_address] = value;
    ++oam_address;
  }
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
        ((ppu_control & PPU_CTRL_VRAM_ADDRESS_INCREMENT_MASK) != 0) ? 32 : 1;
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