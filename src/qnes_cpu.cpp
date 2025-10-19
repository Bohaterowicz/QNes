#include "qnes_cpu.hpp"

#include "cpu_isa.hpp"

namespace QNes {

void CPU::Reset() {
  state.pc = 0;  // TODO: Fetch reset vector from memory
  state.sp = 0xFD;
  state.a = 0;
  state.x = 0;
  state.y = 0;
  state.status.status = 0;  // TODO: Set appropriate flags
  instruction_cycle = 0;
  ir = 0;
  page_crossed = false;
}

void CPU::Step() {
  if (instruction_cycle == 0) {
    // Fetch opcode
    mem_bus.SetAddress(0x00, state.pc);
    ir = mem_bus.Read();
    ++state.pc;
    ++instruction_cycle;
  } else {
    // Execute instruction
    Instructions[ir](*this);
  }
}

void CPU::WriteStackValue(u8 value) {
  mem_bus.SetAddress(0x01, state.sp);
  mem_bus.Write(value);
}

u8 CPU::ReadStackValue() {
  mem_bus.SetAddress(0x01, state.sp);
  return mem_bus.Read();
}

}  // namespace QNes