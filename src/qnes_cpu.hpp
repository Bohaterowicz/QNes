#pragma once

#include "qnes_bits.hpp"
#include "qnes_c.hpp"
#include "qnes_memory.hpp"

namespace QNes {

struct ISA_detail;

class MemBus {
 public:
  MemBus(Memory &memory) : memory(memory) {};
  MemBus(const MemBus &) = delete;
  MemBus &operator=(const MemBus &) = delete;
  MemBus(MemBus &&) = delete;
  MemBus &operator=(MemBus &&) = delete;
  ~MemBus() = default;

  void SetAddress(u8 high, u8 low) { addr = CombineToU16(high, low); }

  u8 Read() { return memory.Read(addr); }
  void Write(u8 value) { memory.Write(addr, value); }

 private:
  u8 adl = 0, adh = 0;  // Address Latch Low/High
  u8 op_latch = 0;      // Operand Latch
  u16 addr = 0;         // Effective Address
  Memory &memory;

  friend struct ISA;
  friend struct ISA_detail;
};

class CPU {
 public:
  CPU(Memory &memory) : mem_bus(memory) {};
  CPU(const CPU &) = delete;
  CPU &operator=(const CPU &) = delete;
  CPU(CPU &&) = delete;
  CPU &operator=(CPU &&) = delete;
  ~CPU() = default;

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

 private:
  State state{};

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

  MemBus mem_bus;

  friend struct ISA;
  friend struct ISA_detail;
  friend struct CPU_Testing;
};

struct CPU_Testing {
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
  static u8 ReadStackValue(CPU &cpu, u8 sp) {
    cpu.mem_bus.SetAddress(0x01, sp);
    return cpu.mem_bus.Read();
  }
};

}  // namespace QNes
