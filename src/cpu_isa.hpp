#pragma once

#include <array>

#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

namespace QNes {

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

  return table;
}();

}  // namespace QNes
