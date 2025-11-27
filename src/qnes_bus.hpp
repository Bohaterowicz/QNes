#pragma once

#include "dma_controller.hpp"
#include "qnes_bits.hpp"
#include "qnes_c.hpp"
#include "qnes_cartridge.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {

/**
 * @brief Bus interface
 * @details The Bus interface is used to abstract the memory addressing logic
 * for the connected devices.
 */
class Bus {
 public:
  Bus() = default;
  Bus(const Bus &) = delete;
  Bus &operator=(const Bus &) = delete;
  Bus(Bus &&) = delete;
  Bus &operator=(Bus &&) = delete;
  virtual ~Bus() = default;

  void SetAddress(u8 high, u8 low) { addr = CombineToU16(high, low); }
  void SetAddress(u16 address) { addr = address; }

  [[nodiscard]] virtual u8 Read() = 0;
  virtual void Write(u8 value) = 0;

 protected:
  u8 adl = 0, adh = 0;  // Address Latch Low/High
  u8 op_latch = 0;      // Operand Latch
  u16 addr = 0;         // Effective Address

  friend struct ISA;
  friend struct ISA_detail;
};

using BusPtr = std::unique_ptr<Bus>;

/**
 * @brief RAM Bus
 * @details The RAM Bus is used to read and write to the RAM memory. It provides
 * read and write operations to the WHOLE RAM memory space (64KB) directly. It
 * expects that the provided Memory object has a size of at least 64KB.
 *
 * @note This bus is used mostly for testing purposes.
 */
class RAMBus : public Bus {
 public:
  RAMBus(Memory *memory) : memory(memory) {
    ASSERT(memory->GetSize() >= Kilobytes(64),
           "Memory size must be at least 64KB");
  }
  RAMBus(const RAMBus &) = delete;
  RAMBus &operator=(const RAMBus &) = delete;
  RAMBus(RAMBus &&) = delete;
  RAMBus &operator=(RAMBus &&) = delete;
  ~RAMBus() override = default;

  [[nodiscard]] u8 Read() override { return memory->Read(addr); }
  void Write(u8 value) override { memory->Write(addr, value); }

 private:
  Memory *memory = nullptr;  // RAM
};

/**
 * @brief NES Bus - CPU
 * @details The NES Bus is used by CPU to read and write to the NES memory
 * mapped devices. It performs correct mapping/mirroring of the memory spaces as
 * expected by the NES hardware.
 */
class NESBus : public Bus {
 public:
  NESBus(Memory *memory, DMAController *dma_controller, PPU *ppu,
         Cartridge *cartridge)
      : memory(memory),
        dma_controller(dma_controller),
        ppu(ppu),
        cartridge(cartridge) {};
  NESBus(const NESBus &) = delete;
  NESBus &operator=(const NESBus &) = delete;
  NESBus(NESBus &&) = delete;
  NESBus &operator=(NESBus &&) = delete;
  ~NESBus() override = default;

  [[nodiscard]] u8 Read() override;
  void Write(u8 value) override;

  void SetCartridge(Cartridge *cartridge) { this->cartridge = cartridge; }
  void SetCPU(CPU *cpu) { this->cpu = cpu; }

  void HaltCPU(bool halt) { cpu->Halt(halt); }

 private:
  CPU *cpu = nullptr;                       // CPU
  Memory *memory = nullptr;                 // RAM
  DMAController *dma_controller = nullptr;  // DMA Controller
  PPU *ppu = nullptr;                       // PPU
  Cartridge *cartridge = nullptr;           // Cartridge
};

using NESBusPtr = std::unique_ptr<NESBus>;

class VRAM_Only_PPUBus : public Bus {
 public:
  VRAM_Only_PPUBus(Memory *vram) : vram(vram) {};
  VRAM_Only_PPUBus(const VRAM_Only_PPUBus &) = delete;
  VRAM_Only_PPUBus &operator=(const VRAM_Only_PPUBus &) = delete;
  VRAM_Only_PPUBus(VRAM_Only_PPUBus &&) = delete;
  VRAM_Only_PPUBus &operator=(VRAM_Only_PPUBus &&) = delete;
  ~VRAM_Only_PPUBus() override = default;

  [[nodiscard]] u8 Read() override;
  void Write(u8 value) override;

 private:
  Memory *vram = nullptr;
};

/**
 * @brief PPU Bus - PPU
 * @details The NES Bus is used by PPU to read and write to the NES memory
 * mapped devices. It performs correct mapping/mirroring of the memory spaces as
 * expected by the NES hardware.
 */
class PPUBus : public Bus {
 public:
  PPUBus(Memory *vram, Cartridge *cartridge)
      : vram(vram), cartridge(cartridge) {};
  PPUBus(const PPUBus &) = delete;
  PPUBus &operator=(const PPUBus &) = delete;
  PPUBus(PPUBus &&) = delete;
  PPUBus &operator=(PPUBus &&) = delete;
  ~PPUBus() override = default;

  [[nodiscard]] u8 Read() override;
  void Write(u8 value) override;

  void SetCartridge(Cartridge *cartridge) { this->cartridge = cartridge; }

 private:
  Memory *vram = nullptr;
  Cartridge *cartridge = nullptr;  // Cartridge
};

}  // namespace QNes