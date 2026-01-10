#pragma once

#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"
#include "qnes_ppu.hpp"
#include "qnes_texture.hpp"

namespace QNes {

class Emulator {
 public:
  static Emulator &Get() {
    auto &emulator = GetInternal();
    if (!emulator.initialized) {
      throw std::runtime_error("Emulator not initialized");
    }
    return emulator;
  }

  static void Initialize(NESTexture *framebuffer) {
    auto &emu = GetInternal();
    emu.framebuffer = framebuffer;
    emu.initialized = true;
  }

  Emulator(const Emulator &) noexcept = delete;
  Emulator &operator=(const Emulator &) noexcept = delete;
  Emulator(Emulator &&) noexcept = delete;
  Emulator &operator=(Emulator &&) noexcept = delete;

  void Run();

  void SetCartridge(Cartridge &&cartridge) {
    this->cartridge = std::move(cartridge);
    auto *ppu_bus_ptr = static_cast<PPUBus *>(ppu_bus.get());
    auto *cpu_bus_ptr = static_cast<NESBus *>(bus.get());
    ppu_bus_ptr->SetCartridge(&this->cartridge);
    cpu_bus_ptr->SetCartridge(&this->cartridge);
  }

  [[nodiscard]] const Cartridge &GetCartridge() const { return cartridge; }

  void Pause(bool paused) { is_paused = paused; }
  [[nodiscard]] bool IsPaused() const { return is_paused; }

  void Shutdown() { is_shutdown_requested = true; }
  [[nodiscard]] bool IsShutdownRequested() const {
    return is_shutdown_requested;
  }

 private:
  bool initialized = false;
  Emulator() noexcept
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
  ~Emulator() noexcept = default;

  static Emulator &GetInternal() {
    static Emulator emulator;
    return emulator;
  }

  Cartridge cartridge;
  MemoryPtr memory;
  MemoryPtr vram;
  DMAControllerPtr dma_controller;
  BusPtr ppu_bus;
  PPUPtr ppu;
  NESBusPtr bus;
  CPUPtr cpu;
  NESTexture *framebuffer = nullptr;

  bool is_paused = false;
  bool is_shutdown_requested = false;
};

}  // namespace QNes