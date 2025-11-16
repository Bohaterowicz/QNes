#include "qnes_emu.hpp"

namespace QNes {

void Emulator::Run() {
  // Emulator main loop would go here
  while (true) {
    ppu->Step();
    ppu->Step();
    ppu->Step();
    cpu->Step();
  }
}

}  // namespace QNes
