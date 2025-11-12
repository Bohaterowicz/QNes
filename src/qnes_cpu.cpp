#include "qnes_cpu.hpp"

#include "cpu_isa.hpp"
#include "qnes_bus.hpp"

namespace QNes {

void CPU::WriteStackValue(u8 value) {
  bus->SetAddress(0x01, state.sp);
  bus->Write(value);
}

u8 CPU::ReadStackValue() {
  bus->SetAddress(0x01, state.sp);
  return bus->Read();
}

void CPU::Reset() { glabal_mode = GlobalMode::RESET; }

void CPU::Step() {
  switch (glabal_mode) {
    case GlobalMode::RESET: {
      HandleReset();
    } break;
    case GlobalMode::NMI: {
      HandleNMI();
    } break;
    case GlobalMode::IRQ: {
      HandleIRQ();
    } break;
    case GlobalMode::RUN: {
      // Check for interrupts when instruction completes
      if (instruction_cycle == 0) {
        // NMI has priority and cannot be disabled
        if (nmi_pending) {
          nmi_pending = false;
          glabal_mode = GlobalMode::NMI;
          interrupt_cycle = 0;
          HandleNMI();
          return;
        }

        // IRQ can be disabled by I flag
        if (irq_pending && !state.status.interrupt_disable) {
          irq_pending = false;
          glabal_mode = GlobalMode::IRQ;
          interrupt_cycle = 0;
          HandleIRQ();
          return;
        }

        // No interrupt, fetch next opcode
        bus->SetAddress(U16High(state.pc), U16Low(state.pc));
        ir = bus->Read();
        ++state.pc;
        ++instruction_cycle;
      } else {
        // Execute instruction
        Instructions[ir](*this);
      }
    } break;
  }
}

void CPU::HandleReset() {
  thread_local u8 pc_adl = 0;
  thread_local u8 pc_adh = 0;
  switch (interrupt_cycle) {
    case 0: {
      // clear internal state
      state.status.status = 0;
      state.pc = 0;
      instruction_cycle = 0;
      ir = 0;
      page_crossed = false;
      nmi_pending = false;
      irq_pending = false;
      interrupt_cycle = 0;
      ++interrupt_cycle;
    } break;
    case 1: {
      // SP and flags initialization
      state.sp = 0xFD;
      state.status.interrupt_disable = 1;
      state.status.unused = 1;
      ++interrupt_cycle;
    } break;
    case 2: {
      // fetch low byte of reset vector
      bus->SetAddress(0xFF, 0xFC);
      pc_adl = bus->Read();
      ++interrupt_cycle;
    } break;
    case 3: {
      // fetch high byte of reset vector
      bus->SetAddress(0xFF, 0xFD);
      pc_adh = bus->Read();
      ++interrupt_cycle;
    } break;
    case 4: {
      // set PC
      state.pc = CombineToU16(pc_adh, pc_adl);
      glabal_mode = GlobalMode::RUN;
      interrupt_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid reset cycle");
  }
}

void CPU::HandleNMI() {
  switch (interrupt_cycle) {
    case 0: {
      // Dummy read
      bus->SetAddress(U16High(state.pc), U16Low(state.pc));
      (void)bus->Read();  // Dummy read for cycle accuracy
      ++interrupt_cycle;
    } break;
    case 1: {
      // Push PCH to stack
      PushStack(U16High(state.pc));
      ++interrupt_cycle;
    } break;
    case 2: {
      // Push PCL to stack
      PushStack(U16Low(state.pc));
      ++interrupt_cycle;
    } break;
    case 3: {
      // Push status to stack - set unused flag, but NOT break flag
      state.status.break_command = 0;
      state.status.unused = 1;
      PushStack(state.status.status);
      ++interrupt_cycle;
    } break;
    case 4: {
      state.status.interrupt_disable = 1;
      ++interrupt_cycle;
    } break;
    case 5: {
      // Fetch low byte of NMI vector from 0xFFFA
      bus->SetAddress(0xFF, 0xFA);
      u8 low_byte = bus->Read();
      state.pc = CombineToU16(U16High(state.pc), low_byte);
      ++interrupt_cycle;
    } break;
    case 6: {
      // Fetch high byte of NMI vector from 0xFFFB
      bus->SetAddress(0xFF, 0xFB);
      u8 high_byte = bus->Read();
      state.pc = CombineToU16(high_byte, U16Low(state.pc));
      interrupt_cycle = 0;
      glabal_mode = GlobalMode::RUN;
    } break;
    default:
      ASSERT(false, "Invalid NMI cycle");
  }
}

void CPU::HandleIRQ() {
  switch (interrupt_cycle) {
    case 0: {
      // Dummy read
      bus->SetAddress(U16High(state.pc), U16Low(state.pc));
      (void)bus->Read();  // Dummy read for cycle accuracy
      ++interrupt_cycle;
    } break;
    case 1: {
      // Push PCH to stack
      PushStack(U16High(state.pc));
      ++interrupt_cycle;
    } break;
    case 2: {
      // Push PCL to stack
      PushStack(U16Low(state.pc));
      ++interrupt_cycle;
    } break;
    case 3: {
      // Push status to stack - set unused flag, but NOT break flag
      state.status.unused = 1;
      state.status.break_command = 0;
      PushStack(state.status.status);
      ++interrupt_cycle;
    } break;
    case 4: {
      state.status.interrupt_disable = 1;
      ++interrupt_cycle;
    } break;
    case 5: {
      // Fetch low byte of IRQ vector from 0xFFFE
      bus->SetAddress(0xFF, 0xFE);
      u8 low_byte = bus->Read();
      state.pc = CombineToU16(U16High(state.pc), low_byte);
      ++interrupt_cycle;
    } break;
    case 6: {
      // Fetch high byte of IRQ vector from 0xFFFF
      bus->SetAddress(0xFF, 0xFF);
      u8 high_byte = bus->Read();
      state.pc = CombineToU16(high_byte, U16Low(state.pc));
      glabal_mode = GlobalMode::RUN;
      interrupt_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid IRQ cycle");
  }
}

u8 CPU_Testing::ReadStackValue(CPU &cpu, u8 sp) {
  cpu.bus->SetAddress(0x01, sp);
  return cpu.bus->Read();
}

}  // namespace QNes