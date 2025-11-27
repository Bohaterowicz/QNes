#include "dma_controller.hpp"

#include "qnes_bus.hpp"

namespace QNes {

void DMAController::RequestDMA(u8 base_address) {
  dma_base_address = base_address;
  state = DMAState::REQUESTED;
  bus->HaltCPU(true);
}

void DMAController::Step() {
  is_get_cycle = !is_get_cycle;
  switch (state) {
    case DMAState::REQUESTED: {
      if (is_get_cycle) {
        // we are in the get cycle during requested state - this means that the
        // next cycle will be a put cycle, we can't read on a put cycle, thus we
        // need an alignment cycle
        state = DMAState::ALIGNMENT;
      } else {
        // we are in the put cycle during requested state - this means that the
        // next cycle will be a get cycle, we can start transferring data on the
        // next cycle since the first operation needs to be a read
        state = DMAState::TRANSFER;
        transer_idx = 0;
      }
    } break;
    case DMAState::ALIGNMENT: {
      ASSERT(!is_get_cycle, "Alignment cycle must be on a put cycle");
      state = DMAState::TRANSFER;
      transer_idx = 0;
    } break;
    case DMAState::TRANSFER: {
      if (transer_idx == 256) {
        // DMA transfer is complete
        state = DMAState::IDLE;
        bus->HaltCPU(false);
        return;
      }
      if (is_get_cycle) {
        // Get cycle
        bus->SetAddress((static_cast<u16>(dma_base_address) << 8) +
                        transer_idx);
        dma_value = bus->Read();
      } else {
        // Put cycle
        bus->SetAddress(0x2004);
        bus->Write(dma_value);
        ++transer_idx;
      }
    }
    default: {
      // Idle state
      return;
    } break;
  }
}

}  // namespace QNes