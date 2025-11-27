#pragma once

#include <memory>

#include "qnes_c.hpp"

namespace QNes {

class NESBus;

class DMAController {
 public:
  enum class DMAState : u8 {
    IDLE = 0,   // DMA is not active
    REQUESTED,  // DMA is requested, the CPU is halted (DMA has control over the
                // bus)
    ALIGNMENT,  // DMA is aligning the cycle (DMA must start on get cycle)
    TRANSFER,   // DMA is transferring the data (DMA has control over the bus)
  };

  DMAController() = default;
  DMAController(const DMAController &) = delete;
  DMAController &operator=(const DMAController &) = delete;
  DMAController(DMAController &&) = delete;
  DMAController &operator=(DMAController &&) = delete;
  ~DMAController() = default;

  void SetBus(NESBus *bus) { this->bus = bus; }
  [[nodiscard]] bool IsDMAActive() const { return state != DMAState::IDLE; }

  void RequestDMA(u8 base_address);

  void Step();

 private:
  u8 dma_base_address = 0;

  u16 transer_idx = 0;
  u8 dma_value = 0;

  DMAState state = DMAState::IDLE;

  // DMA controller alternates between get and put cycles
  bool is_get_cycle = false;

  NESBus *bus = nullptr;
};

using DMAControllerPtr = std::unique_ptr<DMAController>;

}  // namespace QNes