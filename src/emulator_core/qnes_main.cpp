#include "qnes_c.hpp"
#include "qnes_emu.hpp"

int main() {
  QNes::Emulator emulator;

  emulator.Run();

  ASSERT(false, "lol");

  return 0;
}