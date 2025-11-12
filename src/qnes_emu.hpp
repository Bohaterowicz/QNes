#pragma once

#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"

namespace QNes {

class Emulator {
 public:
  Emulator()
      : memory(std::make_unique<Memory>(Kilobytes(2))),
        vram(std::make_unique<Memory>(Kilobytes(2))),
        ppu_bus(std::make_unique<PPUBus>(vram.get())),
        ppu(std::make_unique<PPU>(ppu_bus.get(), nullptr)),
        bus(std::make_unique<NESBus>(memory.get(), ppu.get())),
        cpu(std::make_unique<CPU>(bus.get())) {};
  Emulator(const Emulator &) = delete;
  Emulator &operator=(const Emulator &) = delete;
  Emulator(Emulator &&) = delete;
  Emulator &operator=(Emulator &&) = delete;
  ~Emulator() = default;

  void Run();

 private:
  MemoryPtr memory;
  MemoryPtr vram;
  BusPtr ppu_bus;
  PPUPtr ppu;
  BusPtr bus;
  CPUPtr cpu;
};

}  // namespace QNes