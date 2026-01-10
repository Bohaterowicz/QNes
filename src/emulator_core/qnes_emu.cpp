#include "qnes_emu.hpp"

namespace QNes {

void Emulator::Run() {
  ppu->Step();
  ppu->Step();
  ppu->Step();
  cpu->Step();
}

}  // namespace QNes
