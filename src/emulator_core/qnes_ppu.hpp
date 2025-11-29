#pragma once

#include <algorithm>

#include "qnes_c.hpp"
#include "qnes_framebuffer.hpp"

namespace QNes {

class Bus;
class NESBus;

class PPU {
 public:
  PPU(Bus *ppu_bus, FrameBuffer *external_framebuffer)
      : ppu_bus(ppu_bus), external_framebuffer(external_framebuffer) {};
  PPU(const PPU &) = delete;
  PPU &operator=(const PPU &) = delete;
  PPU(PPU &&) = delete;
  PPU &operator=(PPU &&) = delete;
  ~PPU() = default;

  void Step();

  struct InternalRegisters {
    u16 current_vram_address : 15;
    u16 temp_vram_address : 15;
    u8 fine_x_scroll : 3;
    u8 write_toggle : 1;
  };

  struct OAMEntry {
    u8 y;
    u8 tile_index;
    u8 attributes;
    u8 x;
  };

 private:
  u8 ppu_data_buffer = 0;

  u8 palette_ram[32]{};

  u8 oam_address = 0;
  u8 oam_data[64 * sizeof(OAMEntry)]{};
  u8 oam_secondary_data[8 * sizeof(OAMEntry)]{};

  InternalRegisters internal_registers{};

  u8 ppu_control = 0;
  u8 ppu_mask = 0;
  u8 ppu_status = 0;

  u16 scanline_idx = 0;
  u16 scanline_cycle = 0;

  u8 nametable_byte_latch = 0;

  bool rendering_toggle_scheduled = false;
  int rendering_toggle_cycles_to_wait = 0;
  u8 new_rendering_flags = 0;
  void ScheduleRenderingToggle(u8 new_rendering_flags, int cycles_to_wait);
  void UpdateRenderingToggle();

  // Method for reading PPU registers trough the external NES bus
  [[nodiscard]] u8 Read(u8 address);
  // Method for writing PPU registers trough the external NES bus
  void Write(u8 address, u8 value);

  [[nodiscard]] bool IsRenderingEnabled() const;
  [[nodiscard]] bool IsRenderingActive() const;

  void IncrementVRAMAddress();

  [[nodiscard]] u8 ReadPPUSTATUS();
  [[nodiscard]] u8 ReadPPUDATA();
  [[nodiscard]] u8 ReadOAMDATA();

  void WritePPUCONTROL(u8 value);
  void WritePPUMASK(u8 value);
  void WriteOAMADDRESS(u8 value);
  void WriteOAMDATA(u8 value);
  void WritePPUSCROLL(u8 value);
  void WritePPUADDR(u8 value);
  void WritePPUDATA(u8 value);

  void VRAMIncrementCoarseX();
  void VRAMIncrementFineY();

  void ProcessScanlineCycle();

  Bus *ppu_bus = nullptr;
  FrameBuffer *external_framebuffer = nullptr;

  friend class NESBus;
  friend struct PPU_Testing;
};

using PPUPtr = std::unique_ptr<PPU>;

struct PPU_Testing {
  static PPU::InternalRegisters &GetInternalRegisters(PPU &ppu) {
    return ppu.internal_registers;
  }
  static void SetPPUDataBuffer(PPU &ppu, u8 value) {
    ppu.ppu_data_buffer = value;
  }
  static u8 GetPPUDataBuffer(const PPU &ppu) { return ppu.ppu_data_buffer; }
  static void SetVRAMAddress(PPU &ppu, u16 value) {
    ppu.internal_registers.current_vram_address = value;
  }
  static u16 GetVRAMAddress(const PPU &ppu) {
    return ppu.internal_registers.current_vram_address;
  }
  static void SetPPUMask(PPU &ppu, u8 value) { ppu.ppu_mask = value; }
  static void SetPPUControl(PPU &ppu, u8 value) { ppu.ppu_control = value; }
  static void SetPPUStatus(PPU &ppu, u8 value) { ppu.ppu_status = value; }
  static u8 GetPPUStatus(const PPU &ppu) { return ppu.ppu_status; }
  static u8 GetPPUMask(const PPU &ppu) { return ppu.ppu_mask; }
  static u8 GetPPUControl(const PPU &ppu) { return ppu.ppu_control; }
  static void SetOAMData(PPU &ppu, u8 value) {
    // fill oam data with value
    std::ranges::fill(ppu.oam_data, value);
  }
  static u8 GetOAMData(const PPU &ppu, u8 index) {
    return ppu.oam_data[index & 0x3F];
  }
  static void SetScanline(PPU &ppu, u16 value) { ppu.scanline_idx = value; }
  static void SetPaletteEntry(PPU &ppu, u8 index, u8 value) {
    ppu.palette_ram[index & 0x1F] = value;
  }
  static u8 GetPaletteEntry(const PPU &ppu, u8 index) {
    return ppu.palette_ram[index & 0x1F];
  }
  static u8 ReadPPUDATA(PPU &ppu) { return ppu.ReadPPUDATA(); }
  static void WritePPUDATA(PPU &ppu, u8 value) { ppu.WritePPUDATA(value); }
  static void WritePPUCONTROL(PPU &ppu, u8 value) {
    ppu.WritePPUCONTROL(value);
  }
  static void WritePPUMASK(PPU &ppu, u8 value) { ppu.WritePPUMASK(value); }
  static bool IsRenderingToggleScheduled(const PPU &ppu) {
    return ppu.rendering_toggle_scheduled;
  }
  static int RenderingToggleCyclesToWait(const PPU &ppu) {
    return ppu.rendering_toggle_cycles_to_wait;
  }
  static u8 PendingRenderingFlags(const PPU &ppu) {
    return ppu.new_rendering_flags;
  }
  static void UpdateRenderingToggle(PPU &ppu) { ppu.UpdateRenderingToggle(); }
  static void ClearRenderingToggle(PPU &ppu) {
    ppu.rendering_toggle_scheduled = false;
    ppu.rendering_toggle_cycles_to_wait = 0;
    ppu.new_rendering_flags = 0;
  }
};

}  // namespace QNes