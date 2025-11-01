#include "qnes_cpu.hpp"

#include "cpu_isa.hpp"

namespace QNes {

void CPU::Reset() { glabal_mode = GlobalMode::RESET; }

void CPU::Step() {
  switch (glabal_mode) {
    case GlobalMode::RESET: {
      ResetInternal();
    } break;
    case GlobalMode::RUN: {
      if (instruction_cycle == 0) {
        // Fetch opcode
        mem_bus.SetAddress(U16High(state.pc), U16Low(state.pc));
        ir = mem_bus.Read();
        ++state.pc;
        ++instruction_cycle;
      } else {
        // Execute instruction
        Instructions[ir](*this);
      }
    } break;
  }
}

void CPU::ResetInternal() {
  thread_local u8 pc_adl = 0;
  thread_local u8 pc_adh = 0;
  switch (reset_cycle) {
    case 0: {
      // clear internal state
      state.status.status = 0;
      state.pc = 0;
      instruction_cycle = 0;
      ir = 0;
      page_crossed = false;
      ++reset_cycle;
    } break;
    case 1: {
      // SP and flags initialization
      state.sp = 0xFD;
      state.status.interrupt_disable = 1;
      state.status.unused = 1;
      ++reset_cycle;
    } break;
    case 2: {
      // fetch low byte of reset vector
      mem_bus.SetAddress(0xFF, 0xFC);
      pc_adl = mem_bus.Read();
      ++reset_cycle;
    } break;
    case 3: {
      // fetch high byte of reset vector
      mem_bus.SetAddress(0xFF, 0xFD);
      pc_adh = mem_bus.Read();
      ++reset_cycle;
    } break;
    case 4: {
      // set PC
      state.pc = CombineToU16(pc_adh, pc_adl);
      glabal_mode = GlobalMode::RUN;
      reset_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid reset cycle");
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