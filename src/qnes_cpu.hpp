#pragma once

#include "qnes_c.hpp"

namespace QNes {

struct ISA_detail;
class Bus;

class CPU {
 public:
  CPU(Bus *bus) : bus(bus) {};
  CPU(const CPU &) = delete;
  CPU &operator=(const CPU &) = delete;
  CPU(CPU &&) = delete;
  CPU &operator=(CPU &&) = delete;
  ~CPU() = default;

  enum class GlobalMode : u8 {
    RESET = 0,
    RUN,
    NMI,
    IRQ,
  };

  union StatusFlags {
    u8 status = 0;
    struct {
      u8 carry : 1;
      u8 zero : 1;
      u8 interrupt_disable : 1;
      u8 decimal_mode : 1;
      u8 break_command : 1;
      u8 unused : 1;
      u8 overflow : 1;
      u8 negative : 1;
    };
  };

  struct State {
    u16 pc = 0;            // Program Counter
    u8 sp = 0;             // Stack Pointer
    u8 a = 0;              // Accumulator
    u8 x = 0;              // X Register
    u8 y = 0;              // Y Register
    StatusFlags status{};  // Status Register
  };

  [[nodiscard]] State GetState() const { return state; }

  void Reset();
  void Step();

  void SignalNMI() { nmi_pending = true; }
  void SignalIRQ() { irq_pending = true; }

 private:
  GlobalMode glabal_mode = GlobalMode::RESET;
  State state{};

  void HandleReset();
  void HandleNMI();
  void HandleIRQ();

  u8 interrupt_cycle = 0;

  void WriteStackValue(u8 value);
  u8 ReadStackValue();
  void IncrementSP() { state.sp = static_cast<u8>((state.sp + 1) & 0xFF); }
  void DecrementSP() { state.sp = static_cast<u8>((state.sp - 1) & 0xFF); }
  void PushStack(u8 value) {
    WriteStackValue(value);
    DecrementSP();
  }
  u8 PopStack() {
    IncrementSP();
    return ReadStackValue();
  }

  u8 ir = 0;  // Instruction Register (Opcode)
  bool page_crossed = false;
  u8 instruction_cycle = 0;

  bool nmi_pending = false;
  bool irq_pending = false;

  Bus *bus = nullptr;

  friend struct ISA;
  friend struct ISA_detail;
  friend struct CPU_Testing;
};

using CPUPtr = std::unique_ptr<CPU>;

struct CPU_Testing {
  static CPU::GlobalMode GetGlobalMode(const CPU &cpu) {
    return cpu.glabal_mode;
  }
  static void SetGlobalMode(CPU &cpu, CPU::GlobalMode mode) {
    cpu.glabal_mode = mode;
  }
  static void ExecuteReset(CPU &cpu) {
    cpu.Reset();
    for (int i = 0; i < 5; ++i) {
      cpu.Step();
    }
  }

  static void ZeroInterruptCycle(CPU &cpu) { cpu.interrupt_cycle = 0; }
  static void SetPC(CPU &cpu, u16 pc) { cpu.state.pc = pc; }
  static void SetCarry(CPU &cpu, bool carry) { cpu.state.status.carry = carry; }
  static bool GetCarry(const CPU &cpu) { return cpu.state.status.carry; }
  static void SetA(CPU &cpu, u8 a) { cpu.state.a = a; }
  static void SetX(CPU &cpu, u8 x) { cpu.state.x = x; }
  static void SetY(CPU &cpu, u8 y) { cpu.state.y = y; }
  static void SetStatus(CPU &cpu, CPU::StatusFlags status) {
    cpu.state.status = status;
  }
  static void SetSP(CPU &cpu, u8 sp) { cpu.state.sp = sp; }
  static void IncrementSP(CPU &cpu) { cpu.IncrementSP(); }
  static void DecrementSP(CPU &cpu) { cpu.DecrementSP(); }
  static void SetInstructionCycle(CPU &cpu, u8 cycle) {
    cpu.instruction_cycle = cycle;
  }
  static u8 GetInstructionCycle(const CPU &cpu) {
    return cpu.instruction_cycle;
  }

  static void PushStack(CPU &cpu, u8 value) { cpu.PushStack(value); }
  static u8 PopStack(CPU &cpu) { return cpu.PopStack(); }
  static u8 ReadStackValue(CPU &cpu, u8 sp);
};

}  // namespace QNes
