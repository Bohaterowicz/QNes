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
        dma_controller(std::make_unique<DMAController>()),
        ppu_bus(std::make_unique<PPUBus>(vram.get(), nullptr)),
        ppu(std::make_unique<PPU>(ppu_bus.get(), nullptr)),
        bus(std::make_unique<NESBus>(memory.get(), dma_controller.get(),
                                     ppu.get(), nullptr)),
        cpu(std::make_unique<CPU>(bus.get())) {
    dma_controller->SetBus(bus.get());
  };
  Emulator(const Emulator &) = delete;
  Emulator &operator=(const Emulator &) = delete;
  Emulator(Emulator &&) = delete;
  Emulator &operator=(Emulator &&) = delete;
  ~Emulator() = default;

  void Run();

  void SetCartridge(Cartridge *cartridge) {
    this->cartridge = cartridge;
    auto *ppu_bus_ptr = static_cast<PPUBus *>(ppu_bus.get());
    auto *cpu_bus_ptr = static_cast<NESBus *>(bus.get());
    ppu_bus_ptr->SetCartridge(cartridge);
    cpu_bus_ptr->SetCartridge(cartridge);
  }

 private:
  Cartridge *cartridge = nullptr;
  MemoryPtr memory;
  MemoryPtr vram;
  DMAControllerPtr dma_controller;
  BusPtr ppu_bus;
  PPUPtr ppu;
  NESBusPtr bus;
  CPUPtr cpu;
};

}  // namespace QNes