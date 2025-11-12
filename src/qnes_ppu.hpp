#pragma once

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

  struct Registers {
    u8 ppu_control;
    u8 ppu_mask;
    u8 ppu_status;
    u8 oam_address;
    u8 oam_data;
    u8 ppu_scroll[2];
    u8 ppu_address[2];
    u8 ppu_data;
  };

 private:
  u8 ppu_data_buffer = 0;

  u8 palette_ram[32];

  InternalRegisters internal_registers;
  Registers registers;
  u16 scanline_idx = 0;
  u16 scanline_cycle = 0;

  bool rendering_toggle_scheduled = false;
  int rendering_toggle_cycles_to_wait = 0;
  u8 new_rendering_flags = 0;
  void ScheduleRenderingToggle(u8 new_rendering_flags, int cycles_to_wait);
  void UpdateRenderingToggle();

  // Method for reading PPU registers trough the external NES bus
  [[nodiscard]] u8 BusReadMappedRegister(u8 address);
  // Method for writing PPU registers trough the external NES bus
  void BusWriteMappedRegister(u8 address, u8 value);

  [[nodiscard]] bool IsRenderingEnabled() const;
  [[nodiscard]] bool IsRenderingActive() const;

  void IncrementVRAMAddress();

  [[nodiscard]] u8 ReadPPUSTATUS();
  [[nodiscard]] u8 ReadPPUDATA();

  void WritePPUCONTROL(u8 value);
  void WritePPUMASK(u8 value);
  void WritePPUSCROLL(u8 value);
  void WritePPUADDR(u8 value);
  void WritePPUDATA(u8 value);

  void VRAMIncrementCoarseX();
  void VRAMIncrementFineY();

  Bus *ppu_bus = nullptr;
  FrameBuffer *external_framebuffer = nullptr;

  friend class NESBus;
  friend struct PPU_Testing;
};

using PPUPtr = std::unique_ptr<PPU>;

struct PPU_Testing {
  static PPU::Registers &GetRegisters(PPU &ppu) { return ppu.registers; }
  static PPU::InternalRegisters &GetInternalRegisters(PPU &ppu) {
    return ppu.internal_registers;
  }
  static void SetPPUDataBuffer(PPU &ppu, u8 value) {
    ppu.ppu_data_buffer = value;
  }
  static void SetVRAMAddress(PPU &ppu, u16 value) {
    ppu.internal_registers.current_vram_address = value;
  }
};

}  // namespace QNes