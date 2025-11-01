#pragma once

#include <array>

#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

namespace QNes {

enum class AddressingMode : u8 {
  Implied,
  Immediate,
  ZeroPage,
  ZeroPageX,
  ZeroPageY,
  Absolute,
  AbsoluteX,
  AbsoluteY,
  Indirect,
  XIndirect,
  IndirectY,
  Relative
};

struct ISA {
  using InstructionFunc = void (*)(CPU &);

  template <AddressingMode MODE>
  struct PHA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid PHA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct PLA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid PLA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct PHP {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid PHP Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct PLP {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid PLP Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct TSX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid TSX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct TXS {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid TXS Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct LDA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid LDA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct LDX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid LDX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct LDY {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid LDY Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct STA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid STA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct STX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid STX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct STY {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid STY Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct TAX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid TAX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct TAY {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid TAY Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct TXA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid TXA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct TYA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid TYA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct AND {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid AND Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct EOR {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid EOR Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct ORA {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid ORA Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BIT {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BIT Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct ADC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid ADC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct SBC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid SBC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CMP {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CMP Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CPX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CPX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CPY {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CPY Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct INC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid INC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct INX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid INX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct INY {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid INY Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct DEC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid DEC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct DEX {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid DEX Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct DEY {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid DEY Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct ASL {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid ASL Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct LSR {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid LSR Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct ROL {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid ROL Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct ROR {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid ROR Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct JMP {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid JMP Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct JSR {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid JSR Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct RTS {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid RTS Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BCC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BCC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BCS {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BCS Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BEQ {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BEQ Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BMI {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BMI Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BNE {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BNE Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BPL {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BPL Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BVC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BVC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BVS {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BVS Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CLC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CLC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CLD {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CLD Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CLI {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CLI Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct CLV {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid CLV Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct SEC {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid SEC Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct SED {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid SED Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct SEI {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid SEI Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct NOP {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid NOP Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct RTI {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid RTI Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };

  template <AddressingMode MODE>
  struct BRK {
    static void Execute(CPU &cpu) { ASSERT(false, "Invalid BRK Instruction"); }
    static constexpr u8 OPCODE = 0xFF;
    static constexpr u8 CYCLES = 0;
  };
};

template <>
struct ISA::PHA<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x48;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::PLA<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x68;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::PHP<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x08;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::PLP<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x28;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::TSX<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xBA;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::TXS<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x9A;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::LDA<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xAD;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::LDA<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA9;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::LDA<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA5;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::LDA<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB5;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::LDA<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xBD;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::LDA<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB9;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::LDA<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA1;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::LDA<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB1;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::LDX<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xAE;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::LDX<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA2;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::LDX<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA6;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::LDX<AddressingMode::ZeroPageY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB6;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::LDX<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xBE;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::LDY<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xAC;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::LDY<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA0;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::LDY<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA4;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::LDY<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB4;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::LDY<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xBC;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::STA<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x8D;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::STA<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x85;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::STA<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x95;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::STA<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x9D;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::STA<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x99;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::STA<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x81;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::STA<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x91;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::STX<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x8E;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::STX<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x86;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::STX<AddressingMode::ZeroPageY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x96;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::STY<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x8C;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::STY<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x84;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::STY<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x94;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::TAX<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xAA;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::TAY<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xA8;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::TXA<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x8A;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::TYA<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x98;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::AND<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x29;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::AND<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x25;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::AND<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x35;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::AND<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x2D;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::AND<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x3D;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::AND<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x39;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::AND<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x21;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::AND<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x31;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::EOR<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x49;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::EOR<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x45;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::EOR<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x55;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::EOR<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x4D;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::EOR<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x5D;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::EOR<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x59;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::EOR<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x41;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::EOR<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x51;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::ORA<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x09;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::ORA<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x05;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::ORA<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x15;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::ORA<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x0D;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::ORA<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x1D;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::ORA<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x19;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::ORA<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x01;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ORA<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x11;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::BIT<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x24;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::BIT<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x2C;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::ADC<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x69;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::ADC<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x65;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::ADC<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x75;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::ADC<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x6D;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::ADC<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x7D;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::ADC<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x79;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::ADC<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x61;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ADC<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x71;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::SBC<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE9;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::SBC<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE5;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::SBC<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xF5;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::SBC<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xED;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::SBC<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xFD;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::SBC<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xF9;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::SBC<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE1;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::SBC<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xF1;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::CMP<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC9;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::CMP<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC5;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::CMP<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xD5;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::CMP<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xCD;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::CMP<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xDD;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::CMP<AddressingMode::AbsoluteY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xD9;
  static constexpr u8 CYCLES = 4;  // +1 if page crossed
};

template <>
struct ISA::CMP<AddressingMode::XIndirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC1;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::CMP<AddressingMode::IndirectY> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xD1;
  static constexpr u8 CYCLES = 5;  // +1 if page crossed
};

template <>
struct ISA::CPX<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE0;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::CPX<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE4;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::CPX<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xEC;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::CPY<AddressingMode::Immediate> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC0;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::CPY<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC4;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::CPY<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xCC;
  static constexpr u8 CYCLES = 4;
};

template <>
struct ISA::INC<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE6;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::INC<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xF6;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::INC<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xEE;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::INC<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xFE;
  static constexpr u8 CYCLES = 7;
};

template <>
struct ISA::INX<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xE8;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::INY<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC8;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::DEC<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xC6;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::DEC<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xD6;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::DEC<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xCE;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::DEC<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xDE;
  static constexpr u8 CYCLES = 7;
};

template <>
struct ISA::DEX<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xCA;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::DEY<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x88;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::ASL<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x0A;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::ASL<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x06;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::ASL<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x16;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ASL<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x0E;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ASL<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x1E;
  static constexpr u8 CYCLES = 7;
};

template <>
struct ISA::LSR<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x4A;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::LSR<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x46;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::LSR<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x56;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::LSR<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x4E;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::LSR<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x5E;
  static constexpr u8 CYCLES = 7;
};

template <>
struct ISA::ROL<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x2A;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::ROL<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x26;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::ROL<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x36;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ROL<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x2E;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ROL<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x3E;
  static constexpr u8 CYCLES = 7;
};

template <>
struct ISA::ROR<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x6A;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::ROR<AddressingMode::ZeroPage> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x66;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::ROR<AddressingMode::ZeroPageX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x76;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ROR<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x6E;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::ROR<AddressingMode::AbsoluteX> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x7E;
  static constexpr u8 CYCLES = 7;
};

template <>
struct ISA::JMP<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x4C;
  static constexpr u8 CYCLES = 3;
};

template <>
struct ISA::JMP<AddressingMode::Indirect> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x6C;
  static constexpr u8 CYCLES = 5;
};

template <>
struct ISA::JSR<AddressingMode::Absolute> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x20;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::RTS<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x60;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::BCC<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x90;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BCS<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB0;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BEQ<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xF0;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BMI<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x30;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BNE<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xD0;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BPL<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x10;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BVC<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x50;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::BVS<AddressingMode::Relative> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x70;
  static constexpr u8 CYCLES = 2;  // +1 if branch taken and +1 to a new page
};

template <>
struct ISA::CLC<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x18;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::CLD<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xD8;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::CLI<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x58;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::CLV<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xB8;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::SEC<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x38;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::SED<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xF8;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::SEI<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x78;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::NOP<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0xEA;
  static constexpr u8 CYCLES = 2;
};

template <>
struct ISA::RTI<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x40;
  static constexpr u8 CYCLES = 6;
};

template <>
struct ISA::BRK<AddressingMode::Implied> {
  static void Execute(CPU &cpu);
  static constexpr u8 OPCODE = 0x00;
  static constexpr u8 CYCLES = 7;
};

inline constexpr auto Instructions = [] constexpr {
  std::array<ISA::InstructionFunc, 256> table{};
  table.fill(nullptr);

  // PHA - Push Accumulator
  table[ISA::PHA<AddressingMode::Implied>::OPCODE] =
      ISA::PHA<AddressingMode::Implied>::Execute;
  // PLA - Pull Accumulator
  table[ISA::PLA<AddressingMode::Implied>::OPCODE] =
      ISA::PLA<AddressingMode::Implied>::Execute;
  // PHP - Push Processor Status
  table[ISA::PHP<AddressingMode::Implied>::OPCODE] =
      ISA::PHP<AddressingMode::Implied>::Execute;
  // PLP - Pull Processor Status
  table[ISA::PLP<AddressingMode::Implied>::OPCODE] =
      ISA::PLP<AddressingMode::Implied>::Execute;
  // TSX - Transfer Stack Pointer to X
  table[ISA::TSX<AddressingMode::Implied>::OPCODE] =
      ISA::TSX<AddressingMode::Implied>::Execute;
  // TXS - Transfer X to Stack Pointer
  table[ISA::TXS<AddressingMode::Implied>::OPCODE] =
      ISA::TXS<AddressingMode::Implied>::Execute;

  // LDA - Load Accumulator
  table[ISA::LDA<AddressingMode::Absolute>::OPCODE] =
      ISA::LDA<AddressingMode::Absolute>::Execute;
  table[ISA::LDA<AddressingMode::Immediate>::OPCODE] =
      ISA::LDA<AddressingMode::Immediate>::Execute;
  table[ISA::LDA<AddressingMode::ZeroPage>::OPCODE] =
      ISA::LDA<AddressingMode::ZeroPage>::Execute;
  table[ISA::LDA<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::LDA<AddressingMode::ZeroPageX>::Execute;
  table[ISA::LDA<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::LDA<AddressingMode::AbsoluteX>::Execute;
  table[ISA::LDA<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::LDA<AddressingMode::AbsoluteY>::Execute;
  table[ISA::LDA<AddressingMode::XIndirect>::OPCODE] =
      ISA::LDA<AddressingMode::XIndirect>::Execute;
  table[ISA::LDA<AddressingMode::IndirectY>::OPCODE] =
      ISA::LDA<AddressingMode::IndirectY>::Execute;

  // LDX - Load X Register
  table[ISA::LDX<AddressingMode::Absolute>::OPCODE] =
      ISA::LDX<AddressingMode::Absolute>::Execute;
  table[ISA::LDX<AddressingMode::Immediate>::OPCODE] =
      ISA::LDX<AddressingMode::Immediate>::Execute;
  table[ISA::LDX<AddressingMode::ZeroPage>::OPCODE] =
      ISA::LDX<AddressingMode::ZeroPage>::Execute;
  table[ISA::LDX<AddressingMode::ZeroPageY>::OPCODE] =
      ISA::LDX<AddressingMode::ZeroPageY>::Execute;
  table[ISA::LDX<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::LDX<AddressingMode::AbsoluteY>::Execute;

  // LDY - Load Y Register
  table[ISA::LDY<AddressingMode::Absolute>::OPCODE] =
      ISA::LDY<AddressingMode::Absolute>::Execute;
  table[ISA::LDY<AddressingMode::Immediate>::OPCODE] =
      ISA::LDY<AddressingMode::Immediate>::Execute;
  table[ISA::LDY<AddressingMode::ZeroPage>::OPCODE] =
      ISA::LDY<AddressingMode::ZeroPage>::Execute;
  table[ISA::LDY<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::LDY<AddressingMode::ZeroPageX>::Execute;
  table[ISA::LDY<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::LDY<AddressingMode::AbsoluteX>::Execute;

  // STA - Store Accumulator
  table[ISA::STA<AddressingMode::Absolute>::OPCODE] =
      ISA::STA<AddressingMode::Absolute>::Execute;
  table[ISA::STA<AddressingMode::ZeroPage>::OPCODE] =
      ISA::STA<AddressingMode::ZeroPage>::Execute;
  table[ISA::STA<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::STA<AddressingMode::ZeroPageX>::Execute;
  table[ISA::STA<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::STA<AddressingMode::AbsoluteX>::Execute;
  table[ISA::STA<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::STA<AddressingMode::AbsoluteY>::Execute;
  table[ISA::STA<AddressingMode::XIndirect>::OPCODE] =
      ISA::STA<AddressingMode::XIndirect>::Execute;
  table[ISA::STA<AddressingMode::IndirectY>::OPCODE] =
      ISA::STA<AddressingMode::IndirectY>::Execute;

  // STX - Store X Register
  table[ISA::STX<AddressingMode::Absolute>::OPCODE] =
      ISA::STX<AddressingMode::Absolute>::Execute;
  table[ISA::STX<AddressingMode::ZeroPage>::OPCODE] =
      ISA::STX<AddressingMode::ZeroPage>::Execute;
  table[ISA::STX<AddressingMode::ZeroPageY>::OPCODE] =
      ISA::STX<AddressingMode::ZeroPageY>::Execute;

  // STY - Store Y Register
  table[ISA::STY<AddressingMode::Absolute>::OPCODE] =
      ISA::STY<AddressingMode::Absolute>::Execute;
  table[ISA::STY<AddressingMode::ZeroPage>::OPCODE] =
      ISA::STY<AddressingMode::ZeroPage>::Execute;
  table[ISA::STY<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::STY<AddressingMode::ZeroPageX>::Execute;

  // TAX - Transfer Accumulator to X
  table[ISA::TAX<AddressingMode::Implied>::OPCODE] =
      ISA::TAX<AddressingMode::Implied>::Execute;
  // TAY - Transfer Accumulator to Y
  table[ISA::TAY<AddressingMode::Implied>::OPCODE] =
      ISA::TAY<AddressingMode::Implied>::Execute;
  // TXA - Transfer X to Accumulator
  table[ISA::TXA<AddressingMode::Implied>::OPCODE] =
      ISA::TXA<AddressingMode::Implied>::Execute;
  // TYA - Transfer Y to Accumulator
  table[ISA::TYA<AddressingMode::Implied>::OPCODE] =
      ISA::TYA<AddressingMode::Implied>::Execute;

  // AND - Logical AND
  table[ISA::AND<AddressingMode::Immediate>::OPCODE] =
      ISA::AND<AddressingMode::Immediate>::Execute;
  table[ISA::AND<AddressingMode::ZeroPage>::OPCODE] =
      ISA::AND<AddressingMode::ZeroPage>::Execute;
  table[ISA::AND<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::AND<AddressingMode::ZeroPageX>::Execute;
  table[ISA::AND<AddressingMode::Absolute>::OPCODE] =
      ISA::AND<AddressingMode::Absolute>::Execute;
  table[ISA::AND<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::AND<AddressingMode::AbsoluteX>::Execute;
  table[ISA::AND<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::AND<AddressingMode::AbsoluteY>::Execute;
  table[ISA::AND<AddressingMode::XIndirect>::OPCODE] =
      ISA::AND<AddressingMode::XIndirect>::Execute;
  table[ISA::AND<AddressingMode::IndirectY>::OPCODE] =
      ISA::AND<AddressingMode::IndirectY>::Execute;

  // EOR - Logical Exclusive OR
  table[ISA::EOR<AddressingMode::Immediate>::OPCODE] =
      ISA::EOR<AddressingMode::Immediate>::Execute;
  table[ISA::EOR<AddressingMode::ZeroPage>::OPCODE] =
      ISA::EOR<AddressingMode::ZeroPage>::Execute;
  table[ISA::EOR<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::EOR<AddressingMode::ZeroPageX>::Execute;
  table[ISA::EOR<AddressingMode::Absolute>::OPCODE] =
      ISA::EOR<AddressingMode::Absolute>::Execute;
  table[ISA::EOR<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::EOR<AddressingMode::AbsoluteX>::Execute;
  table[ISA::EOR<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::EOR<AddressingMode::AbsoluteY>::Execute;
  table[ISA::EOR<AddressingMode::XIndirect>::OPCODE] =
      ISA::EOR<AddressingMode::XIndirect>::Execute;
  table[ISA::EOR<AddressingMode::IndirectY>::OPCODE] =
      ISA::EOR<AddressingMode::IndirectY>::Execute;

  // ORA - Logical Inclusive OR
  table[ISA::ORA<AddressingMode::Immediate>::OPCODE] =
      ISA::ORA<AddressingMode::Immediate>::Execute;
  table[ISA::ORA<AddressingMode::ZeroPage>::OPCODE] =
      ISA::ORA<AddressingMode::ZeroPage>::Execute;
  table[ISA::ORA<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::ORA<AddressingMode::ZeroPageX>::Execute;
  table[ISA::ORA<AddressingMode::Absolute>::OPCODE] =
      ISA::ORA<AddressingMode::Absolute>::Execute;
  table[ISA::ORA<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::ORA<AddressingMode::AbsoluteX>::Execute;
  table[ISA::ORA<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::ORA<AddressingMode::AbsoluteY>::Execute;
  table[ISA::ORA<AddressingMode::XIndirect>::OPCODE] =
      ISA::ORA<AddressingMode::XIndirect>::Execute;
  table[ISA::ORA<AddressingMode::IndirectY>::OPCODE] =
      ISA::ORA<AddressingMode::IndirectY>::Execute;

  // BIT - Test Bits in Memory
  table[ISA::BIT<AddressingMode::ZeroPage>::OPCODE] =
      ISA::BIT<AddressingMode::ZeroPage>::Execute;
  table[ISA::BIT<AddressingMode::Absolute>::OPCODE] =
      ISA::BIT<AddressingMode::Absolute>::Execute;
  table[ISA::BIT<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::BIT<AddressingMode::AbsoluteX>::Execute;

  // ADC - Add with Carry
  table[ISA::ADC<AddressingMode::Immediate>::OPCODE] =
      ISA::ADC<AddressingMode::Immediate>::Execute;
  table[ISA::ADC<AddressingMode::ZeroPage>::OPCODE] =
      ISA::ADC<AddressingMode::ZeroPage>::Execute;
  table[ISA::ADC<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::ADC<AddressingMode::ZeroPageX>::Execute;
  table[ISA::ADC<AddressingMode::Absolute>::OPCODE] =
      ISA::ADC<AddressingMode::Absolute>::Execute;
  table[ISA::ADC<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::ADC<AddressingMode::AbsoluteX>::Execute;
  table[ISA::ADC<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::ADC<AddressingMode::AbsoluteY>::Execute;
  table[ISA::ADC<AddressingMode::XIndirect>::OPCODE] =
      ISA::ADC<AddressingMode::XIndirect>::Execute;
  table[ISA::ADC<AddressingMode::IndirectY>::OPCODE] =
      ISA::ADC<AddressingMode::IndirectY>::Execute;

  // SBC - Subtract with Carry
  table[ISA::SBC<AddressingMode::Immediate>::OPCODE] =
      ISA::SBC<AddressingMode::Immediate>::Execute;
  table[ISA::SBC<AddressingMode::ZeroPage>::OPCODE] =
      ISA::SBC<AddressingMode::ZeroPage>::Execute;
  table[ISA::SBC<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::SBC<AddressingMode::ZeroPageX>::Execute;
  table[ISA::SBC<AddressingMode::Absolute>::OPCODE] =
      ISA::SBC<AddressingMode::Absolute>::Execute;
  table[ISA::SBC<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::SBC<AddressingMode::AbsoluteX>::Execute;
  table[ISA::SBC<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::SBC<AddressingMode::AbsoluteY>::Execute;
  table[ISA::SBC<AddressingMode::XIndirect>::OPCODE] =
      ISA::SBC<AddressingMode::XIndirect>::Execute;
  table[ISA::SBC<AddressingMode::IndirectY>::OPCODE] =
      ISA::SBC<AddressingMode::IndirectY>::Execute;

  // CMP - Compare Accumulator
  table[ISA::CMP<AddressingMode::Immediate>::OPCODE] =
      ISA::CMP<AddressingMode::Immediate>::Execute;
  table[ISA::CMP<AddressingMode::ZeroPage>::OPCODE] =
      ISA::CMP<AddressingMode::ZeroPage>::Execute;
  table[ISA::CMP<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::CMP<AddressingMode::ZeroPageX>::Execute;
  table[ISA::CMP<AddressingMode::Absolute>::OPCODE] =
      ISA::CMP<AddressingMode::Absolute>::Execute;
  table[ISA::CMP<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::CMP<AddressingMode::AbsoluteX>::Execute;
  table[ISA::CMP<AddressingMode::AbsoluteY>::OPCODE] =
      ISA::CMP<AddressingMode::AbsoluteY>::Execute;
  table[ISA::CMP<AddressingMode::XIndirect>::OPCODE] =
      ISA::CMP<AddressingMode::XIndirect>::Execute;
  table[ISA::CMP<AddressingMode::IndirectY>::OPCODE] =
      ISA::CMP<AddressingMode::IndirectY>::Execute;

  // CPX - Compare X Register
  table[ISA::CPX<AddressingMode::Immediate>::OPCODE] =
      ISA::CPX<AddressingMode::Immediate>::Execute;
  table[ISA::CPX<AddressingMode::ZeroPage>::OPCODE] =
      ISA::CPX<AddressingMode::ZeroPage>::Execute;
  table[ISA::CPX<AddressingMode::Absolute>::OPCODE] =
      ISA::CPX<AddressingMode::Absolute>::Execute;

  // CPY - Compare Y Register
  table[ISA::CPY<AddressingMode::Immediate>::OPCODE] =
      ISA::CPY<AddressingMode::Immediate>::Execute;
  table[ISA::CPY<AddressingMode::ZeroPage>::OPCODE] =
      ISA::CPY<AddressingMode::ZeroPage>::Execute;
  table[ISA::CPY<AddressingMode::Absolute>::OPCODE] =
      ISA::CPY<AddressingMode::Absolute>::Execute;

  // INC - Increment Memory
  table[ISA::INC<AddressingMode::ZeroPage>::OPCODE] =
      ISA::INC<AddressingMode::ZeroPage>::Execute;
  table[ISA::INC<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::INC<AddressingMode::ZeroPageX>::Execute;
  table[ISA::INC<AddressingMode::Absolute>::OPCODE] =
      ISA::INC<AddressingMode::Absolute>::Execute;
  table[ISA::INC<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::INC<AddressingMode::AbsoluteX>::Execute;

  // INX - Increment X Register
  table[ISA::INX<AddressingMode::Implied>::OPCODE] =
      ISA::INX<AddressingMode::Implied>::Execute;

  // INY - Increment Y Register
  table[ISA::INY<AddressingMode::Implied>::OPCODE] =
      ISA::INY<AddressingMode::Implied>::Execute;

  // DEC - Decrement Memory
  table[ISA::DEC<AddressingMode::ZeroPage>::OPCODE] =
      ISA::DEC<AddressingMode::ZeroPage>::Execute;
  table[ISA::DEC<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::DEC<AddressingMode::ZeroPageX>::Execute;
  table[ISA::DEC<AddressingMode::Absolute>::OPCODE] =
      ISA::DEC<AddressingMode::Absolute>::Execute;
  table[ISA::DEC<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::DEC<AddressingMode::AbsoluteX>::Execute;

  // DEX - Decrement X Register
  table[ISA::DEX<AddressingMode::Implied>::OPCODE] =
      ISA::DEX<AddressingMode::Implied>::Execute;

  // DEY - Decrement Y Register
  table[ISA::DEY<AddressingMode::Implied>::OPCODE] =
      ISA::DEY<AddressingMode::Implied>::Execute;

  // ASL - Arithmetic Shift Left
  table[ISA::ASL<AddressingMode::Implied>::OPCODE] =  // implied = accumulator
      ISA::ASL<AddressingMode::Implied>::Execute;
  table[ISA::ASL<AddressingMode::ZeroPage>::OPCODE] =
      ISA::ASL<AddressingMode::ZeroPage>::Execute;
  table[ISA::ASL<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::ASL<AddressingMode::ZeroPageX>::Execute;
  table[ISA::ASL<AddressingMode::Absolute>::OPCODE] =
      ISA::ASL<AddressingMode::Absolute>::Execute;
  table[ISA::ASL<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::ASL<AddressingMode::AbsoluteX>::Execute;

  // LSR - Logical Shift Right
  table[ISA::LSR<AddressingMode::Implied>::OPCODE] =  // implied = accumulator
      ISA::LSR<AddressingMode::Implied>::Execute;
  table[ISA::LSR<AddressingMode::ZeroPage>::OPCODE] =
      ISA::LSR<AddressingMode::ZeroPage>::Execute;
  table[ISA::LSR<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::LSR<AddressingMode::ZeroPageX>::Execute;
  table[ISA::LSR<AddressingMode::Absolute>::OPCODE] =
      ISA::LSR<AddressingMode::Absolute>::Execute;
  table[ISA::LSR<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::LSR<AddressingMode::AbsoluteX>::Execute;

  // ROL - Rotate Left
  table[ISA::ROL<AddressingMode::Implied>::OPCODE] =  // implied = accumulator
      ISA::ROL<AddressingMode::Implied>::Execute;
  table[ISA::ROL<AddressingMode::ZeroPage>::OPCODE] =
      ISA::ROL<AddressingMode::ZeroPage>::Execute;
  table[ISA::ROL<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::ROL<AddressingMode::ZeroPageX>::Execute;
  table[ISA::ROL<AddressingMode::Absolute>::OPCODE] =
      ISA::ROL<AddressingMode::Absolute>::Execute;
  table[ISA::ROL<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::ROL<AddressingMode::AbsoluteX>::Execute;

  // ROR - Rotate Right
  table[ISA::ROR<AddressingMode::Implied>::OPCODE] =  // implied = accumulator
      ISA::ROR<AddressingMode::Implied>::Execute;
  table[ISA::ROR<AddressingMode::ZeroPage>::OPCODE] =
      ISA::ROR<AddressingMode::ZeroPage>::Execute;
  table[ISA::ROR<AddressingMode::ZeroPageX>::OPCODE] =
      ISA::ROR<AddressingMode::ZeroPageX>::Execute;
  table[ISA::ROR<AddressingMode::Absolute>::OPCODE] =
      ISA::ROR<AddressingMode::Absolute>::Execute;
  table[ISA::ROR<AddressingMode::AbsoluteX>::OPCODE] =
      ISA::ROR<AddressingMode::AbsoluteX>::Execute;

  // JMP - Jump to Subroutine
  table[ISA::JMP<AddressingMode::Absolute>::OPCODE] =
      ISA::JMP<AddressingMode::Absolute>::Execute;
  table[ISA::JMP<AddressingMode::Indirect>::OPCODE] =
      ISA::JMP<AddressingMode::Indirect>::Execute;

  // JSR - Jump to Subroutine
  table[ISA::JSR<AddressingMode::Absolute>::OPCODE] =
      ISA::JSR<AddressingMode::Absolute>::Execute;

  // RTS - Return from Subroutine
  table[ISA::RTS<AddressingMode::Implied>::OPCODE] =
      ISA::RTS<AddressingMode::Implied>::Execute;

  // BCC - Branch if Carry Clear
  table[ISA::BCC<AddressingMode::Relative>::OPCODE] =
      ISA::BCC<AddressingMode::Relative>::Execute;

  // BCS - Branch if Carry Set
  table[ISA::BCS<AddressingMode::Relative>::OPCODE] =
      ISA::BCS<AddressingMode::Relative>::Execute;

  // BEQ - Branch if Equal
  table[ISA::BEQ<AddressingMode::Relative>::OPCODE] =
      ISA::BEQ<AddressingMode::Relative>::Execute;

  // BMI - Branch if Minus
  table[ISA::BMI<AddressingMode::Relative>::OPCODE] =
      ISA::BMI<AddressingMode::Relative>::Execute;

  // BNE - Branch if Not Equal
  table[ISA::BNE<AddressingMode::Relative>::OPCODE] =
      ISA::BNE<AddressingMode::Relative>::Execute;

  // BPL - Branch if Plus
  table[ISA::BPL<AddressingMode::Relative>::OPCODE] =
      ISA::BPL<AddressingMode::Relative>::Execute;

  // BVC - Branch if Overflow Clear
  table[ISA::BVC<AddressingMode::Relative>::OPCODE] =
      ISA::BVC<AddressingMode::Relative>::Execute;

  // BVS - Branch if Overflow Set
  table[ISA::BVS<AddressingMode::Relative>::OPCODE] =
      ISA::BVS<AddressingMode::Relative>::Execute;

  // CLC - Clear Carry Flag
  table[ISA::CLC<AddressingMode::Implied>::OPCODE] =
      ISA::CLC<AddressingMode::Implied>::Execute;

  // CLD - Clear Decimal Mode Flag
  table[ISA::CLD<AddressingMode::Implied>::OPCODE] =
      ISA::CLD<AddressingMode::Implied>::Execute;

  // CLI - Clear Interrupt Disable Flag
  table[ISA::CLI<AddressingMode::Implied>::OPCODE] =
      ISA::CLI<AddressingMode::Implied>::Execute;

  // CLV - Clear Overflow Flag
  table[ISA::CLV<AddressingMode::Implied>::OPCODE] =
      ISA::CLV<AddressingMode::Implied>::Execute;

  // SEC - Set Carry Flag
  table[ISA::SEC<AddressingMode::Implied>::OPCODE] =
      ISA::SEC<AddressingMode::Implied>::Execute;

  // SED - Set Decimal Mode Flag
  table[ISA::SED<AddressingMode::Implied>::OPCODE] =
      ISA::SED<AddressingMode::Implied>::Execute;

  // SEI - Set Interrupt Disable Flag
  table[ISA::SEI<AddressingMode::Implied>::OPCODE] =
      ISA::SEI<AddressingMode::Implied>::Execute;

  // NOP - No Operation
  table[ISA::NOP<AddressingMode::Implied>::OPCODE] =
      ISA::NOP<AddressingMode::Implied>::Execute;

  // RTI - Return from Interrupt
  table[ISA::RTI<AddressingMode::Implied>::OPCODE] =
      ISA::RTI<AddressingMode::Implied>::Execute;

  // BRK - Break
  table[ISA::BRK<AddressingMode::Implied>::OPCODE] =
      ISA::BRK<AddressingMode::Implied>::Execute;

  return table;
}();

}  // namespace QNes
