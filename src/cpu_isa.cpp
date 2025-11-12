#include "cpu_isa.hpp"

#include "qnes_bits.hpp"
#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"

namespace QNes {

struct ISA_detail {
  enum class Operation : u8 {
    AND,
    EOR,
    ORA,
    BIT,
    ADC,
    SBC,
    CMP,
    INC,
    DEC,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    ROTATE_LEFT,
    ROTATE_RIGHT,
  };

  enum class BranchCondition : u8 {
    BCC,
    BCS,
    BEQ,
    BMI,
    BNE,
    BPL,
    BVC,
    BVS,
  };

  static QNES_FORCE_INLINE void ReadValueFromMem(Bus *mem_bus, u8 high_addr,
                                                 u8 low_addr, u8 &reg) {
    mem_bus->SetAddress(high_addr, low_addr);
    reg = mem_bus->Read();
  }

  static QNES_FORCE_INLINE void WriteValueToMem(Bus *mem_bus, u8 high_addr,
                                                u8 low_addr, u8 value) {
    mem_bus->SetAddress(high_addr, low_addr);
    mem_bus->Write(value);
  }

  static QNES_FORCE_INLINE void SetZNFlags(CPU &cpu, u8 value) {
    cpu.state.status.zero = (value == 0);
    cpu.state.status.negative = (value & 0x80) != 0;
  }

  static QNES_FORCE_INLINE void SetArithmeticOverflowFlag(CPU &cpu, u8 &value,
                                                          u8 operand,
                                                          u8 result) {
    cpu.state.status.overflow =
        (~(value ^ operand) & (value ^ result) & 0x80) != 0;
  }

  template <BranchCondition CONDITION>
  static QNES_FORCE_INLINE bool ExecuteCondition(CPU &cpu) {
    if constexpr (CONDITION == BranchCondition::BCC) {
      return !cpu.state.status.carry;
    } else if constexpr (CONDITION == BranchCondition::BCS) {
      return cpu.state.status.carry;
    } else if constexpr (CONDITION == BranchCondition::BEQ) {
      return cpu.state.status.zero;
    } else if constexpr (CONDITION == BranchCondition::BMI) {
      return cpu.state.status.negative;
    } else if constexpr (CONDITION == BranchCondition::BNE) {
      return !cpu.state.status.zero;
    } else if constexpr (CONDITION == BranchCondition::BPL) {
      return !cpu.state.status.negative;
    } else if constexpr (CONDITION == BranchCondition::BVC) {
      return !cpu.state.status.overflow;
    } else if constexpr (CONDITION == BranchCondition::BVS) {
      return cpu.state.status.overflow;
    } else {
      ASSERT(false, "Invalid branch condition");
      return false;
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void ExecuteOperation(CPU &cpu, u8 &reg,
                                                 u8 operand [[maybe_unused]]) {
    if constexpr (OP == Operation::AND) {
      reg = reg & operand;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::EOR) {
      reg = reg ^ operand;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::ORA) {
      reg = reg | operand;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::BIT) {
      const u8 result = reg & operand;
      cpu.state.status.zero = (result == 0);
      cpu.state.status.negative = (operand & 0x80) != 0;
      cpu.state.status.overflow = (operand & 0x40) != 0;
    } else if constexpr (OP == Operation::ADC) {
      u8 reg_begore = reg;
      u16 result = reg + operand + (cpu.state.status.carry ? 1 : 0);
      cpu.state.status.carry = (result & 0x100) != 0;
      reg = U16Low(result);
      ISA_detail::SetZNFlags(cpu, reg);
      SetArithmeticOverflowFlag(cpu, reg_begore, operand, result);
    } else if constexpr (OP == Operation::SBC) {
      // NOTE: A ← A - M - (1 - C) == A ← A + (~M) + C
      // so we can use the same code for both ADC and SBC
      u8 reg_begore = reg;
      u8 tmp_value = ~operand;
      u16 result = reg + tmp_value + (cpu.state.status.carry ? 1 : 0);
      cpu.state.status.carry = (result & 0x100) != 0;
      reg = U16Low(result);
      ISA_detail::SetZNFlags(cpu, reg);
      SetArithmeticOverflowFlag(cpu, reg_begore, tmp_value, result);
    } else if constexpr (OP == Operation::CMP) {
      cpu.state.status.carry = (reg >= operand) ? 1 : 0;
      u16 result = reg - operand;
      SetZNFlags(cpu, U16Low(result));
    } else if constexpr (OP == Operation::INC) {
      ++reg;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::DEC) {
      --reg;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::SHIFT_LEFT) {
      cpu.state.status.carry = (reg & 0x80) != 0;
      reg <<= 1;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::SHIFT_RIGHT) {
      cpu.state.status.carry = (reg & 0x01) != 0;
      reg >>= 1;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::ROTATE_LEFT) {
      const bool carry = cpu.state.status.carry;
      cpu.state.status.carry = (reg & 0x80) != 0;
      reg = (reg << 1) | carry;
      ISA_detail::SetZNFlags(cpu, reg);
    } else if constexpr (OP == Operation::ROTATE_RIGHT) {
      const bool carry = cpu.state.status.carry;
      cpu.state.status.carry = (reg & 0x01) != 0;
      reg = (reg >> 1) | (carry << 7);
      ISA_detail::SetZNFlags(cpu, reg);
    } else {
      ASSERT(false, "Invalid operation");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterAbsolute(CPU &cpu,
                                                             u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch low byte of address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch high byte of address and form full address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adh);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read value from the effective address into the accumulator
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterImmediate(CPU &cpu,
                                                              u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch immediate value
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         reg);
        ++cpu.state.pc;
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterZeroPage(CPU &cpu,
                                                             u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch zero page address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Read value from the zero page address into the register
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterZeroPageIndexed(
      CPU &cpu, u8 &reg, u8 idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch zero page address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Perform dummy read and add indexed register to address (high byte
        // stays zero)
        u8 dummy = 0;
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl, dummy);
        cpu.bus->adl = static_cast<u8>(
            (static_cast<u16>(cpu.bus->adl) + idx_reg) & 0x00FF);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read value from the zero page address into the register
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterAbsoluteIndexed(
      CPU &cpu, u8 &reg, u8 idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch low byte of address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch high byte of address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adh);
        // Add index register to low byte to, if needed, trigger page crossing
        // in the next cycle
        u16 tmp = static_cast<u16>(cpu.bus->adl) + static_cast<u16>(idx_reg);
        cpu.bus->adl = static_cast<u8>(tmp & 0x00FF);
        cpu.page_crossed = (tmp & 0xFF00) != 0;
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Perform read, if page was crossed this is a dummy read
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        // Increment high byte if page crossed else finish instruction
        if (cpu.page_crossed) {
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          ++cpu.instruction_cycle;
        } else {
          cpu.instruction_cycle = 0;
        }
      } break;
      case 4: {
        ASSERT(cpu.page_crossed,
               "Unexpected cycle for LDA Absolute Indexed (page was not "
               "corossed)");
        cpu.page_crossed = false;
        // Read value from the effective address into the register
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterXIndirect(CPU &cpu, u8 &reg,
                                                              u8 idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Read pointer address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->op_latch);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Perform dummy read and add X to pointer (zero page wraparound)
        u8 dummy = 0;
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, dummy);
        // page bound wraparound is not handled
        cpu.bus->op_latch = static_cast<u8>(
            (static_cast<u16>(cpu.bus->op_latch) + idx_reg) & 0x00FF);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read effective address low byte
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, cpu.bus->adl);
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Read effective address high byte
        ReadValueFromMem(
            cpu.bus, 0x00,
            static_cast<u8>((static_cast<u16>(cpu.bus->op_latch) + 1) & 0x00FF),
            cpu.bus->adh);
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // Read value from the effective address into the register
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void LoadMemoryToRegisterIndirectY(CPU &cpu,
                                                              u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Read pointer address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->op_latch);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Read effective address low byte
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, cpu.bus->adl);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read effective address high byte
        ReadValueFromMem(
            cpu.bus, 0x00,
            static_cast<u8>((static_cast<u16>(cpu.bus->op_latch) + 1) & 0x00FF),
            cpu.bus->adh);
        // Add Y to low byte to, if needed, trigger page crossing
        u16 tmp =
            static_cast<u16>(cpu.bus->adl) + static_cast<u16>(cpu.state.y);
        cpu.bus->adl = static_cast<u8>(tmp & 0x00FF);
        cpu.page_crossed = (tmp & 0xFF00) != 0;
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Perform read, if page was crossed this is a dummy read
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        // Increment high byte if page crossed else finish instruction
        if (cpu.page_crossed) {
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          ++cpu.instruction_cycle;
        } else {
          cpu.instruction_cycle = 0;
        }
      } break;
      case 5: {
        ASSERT(cpu.page_crossed,
               "Unexpected cycle for LDA Indirect Y (page was not corossed)");
        cpu.page_crossed = false;
        // Read value from the effective address into the register
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        // Set Zero and Negative flags based on the value loaded
        SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void StoreRegisterAbsolute(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch low byte of address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch high byte of address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adh);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Write value to the effective address
        WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void StoreRegisterZeroPage(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch zero page address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Write value to the effective address
        WriteValueToMem(cpu.bus, 0x00, cpu.bus->adl, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void StoreRegisterZeroPageIndexed(CPU &cpu, u8 &reg,
                                                             u8 idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch zero page address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Perform dummy read and add register to pointer (zero page wraparound)
        u8 dummy = 0;
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl, dummy);
        cpu.bus->adl =
            U16Low(static_cast<u16>(cpu.bus->adl) + static_cast<u16>(idx_reg));
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Write value to the zero page address
        WriteValueToMem(cpu.bus, 0x00, cpu.bus->adl, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void StoreRegisterAbsoluteIndexed(CPU &cpu, u8 &reg,
                                                             u8 idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch absolute address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch high byte of address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->adh);
        // Add index register to low byte to, if needed, trigger page crossing
        // in the next cycle
        u16 tmp = static_cast<u16>(cpu.bus->adl) + static_cast<u16>(idx_reg);
        cpu.bus->adl = U16Low(tmp);
        cpu.page_crossed = (tmp & 0xFF00) != 0;
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Dummy read since page may have been crossed and the processor cannot
        // undo writes it always reads from the address first
        u8 dummy = 0;
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, dummy);
        // fix high byte if page crossed
        if (cpu.page_crossed) {
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          cpu.page_crossed = false;
        }
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Write value to the effective address
        WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void StoreRegisterXIndirect(CPU &cpu, u8 &reg,
                                                       u8 idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Read pointer address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->op_latch);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Perform dummy read and add X to pointer (zero page wraparound)
        u8 dummy = 0;
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, dummy);
        cpu.bus->op_latch =
            U16Low(static_cast<u16>(cpu.bus->op_latch) + idx_reg);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read effective address low byte
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, cpu.bus->adl);
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Read effective address high byte
        ReadValueFromMem(cpu.bus, 0x00,
                         U16Low((static_cast<u16>(cpu.bus->op_latch) + 1)),
                         cpu.bus->adh);
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // Write value to the effective address
        WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void StoreRegisterIndirectY(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Read pointer address
        ReadValueFromMem(cpu.bus, U16High(cpu.state.pc), U16Low(cpu.state.pc),
                         cpu.bus->op_latch);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Read effective address low byte
        ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, cpu.bus->adl);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read effective address high byte
        ReadValueFromMem(cpu.bus, 0x00,
                         U16Low((static_cast<u16>(cpu.bus->op_latch) + 1)),
                         cpu.bus->adh);
        // Add Y to effective address low byte, if needed, trigger page crossing
        // in the next cycle
        u16 tmp =
            static_cast<u16>(cpu.bus->adl) + static_cast<u16>(cpu.state.y);
        cpu.bus->adl = U16Low(tmp);
        cpu.page_crossed = (tmp & 0xFF00) != 0;
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Dummy read since page may have been crossed and the processor cannot
        // undo writes it always reads from the address first
        u8 dummy = 0;
        ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, dummy);
        // fix high byte if page crossed
        if (cpu.page_crossed) {
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          cpu.page_crossed = false;
        }
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // Write value to the effective address
        WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  static QNES_FORCE_INLINE void TransferRegister(CPU &cpu, u8 &reg, u8 value) {
    switch (cpu.instruction_cycle) {
      case 1: {
        reg = value;
        ISA_detail::SetZNFlags(cpu, reg);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle")
    };
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationImmediate(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        ++cpu.state.pc;
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationZeroPage(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationZeroPage_ReadModifyWrite(CPU &cpu) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // dummy write
        ISA_detail::WriteValueToMem(cpu.bus, 0x00, cpu.bus->adl,
                                    cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, cpu.bus->op_latch,
                                         cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // final write
        ISA_detail::WriteValueToMem(cpu.bus, 0x00, cpu.bus->adl,
                                    cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationZeroPageIndexed(CPU &cpu, u8 &reg,
                                                         u8 &idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        u8 dummy = 0;
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl, dummy);
        cpu.bus->adl = U16Low((static_cast<u16>(cpu.bus->adl) + idx_reg));
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationZeroPageIndexed_ReadModifyWrite(
      CPU &cpu, u8 &idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        u8 dummy = 0;
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl, dummy);
        cpu.bus->adl = U16Low((static_cast<u16>(cpu.bus->adl) + idx_reg));
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // dummy write
        ISA_detail::WriteValueToMem(cpu.bus, 0x00, cpu.bus->adl,
                                    cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, cpu.bus->op_latch,
                                         cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // final write
        ISA_detail::WriteValueToMem(cpu.bus, 0x00, cpu.bus->adl,
                                    cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationAbsolute(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adh);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationAbsolute_ReadModifyWrite(CPU &cpu) {
    switch (cpu.instruction_cycle) {
      case 1: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adh);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // dummy write
        ISA_detail::WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                    cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, cpu.bus->op_latch,
                                         cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // final write
        ISA_detail::WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                    cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationAbsoluteIndexed(CPU &cpu, u8 &reg,
                                                         u8 &idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch low byte of address
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch high byte of address
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adh);
        // Add index register to low byte to, if needed, trigger page crossing
        auto tmp = static_cast<u16>(cpu.bus->adl) + static_cast<u16>(idx_reg);
        cpu.bus->adl = U16Low(tmp);
        cpu.page_crossed = U16High(tmp) != 0;
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read value from the effective address into the register, if page was
        // crossed this is a dummy read
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        if (cpu.page_crossed) {
          // page was crossed, increment high byte and set op_latch to 0
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          cpu.bus->op_latch = 0;
          ++cpu.instruction_cycle;
        } else {
          // page was not crossed, execute logical operation and set flags
          ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
          cpu.instruction_cycle = 0;
        }
      } break;
      case 4: {
        ASSERT(cpu.page_crossed,
               "Unexpected cycle for Logical Operation Absolute Indexed (page "
               "was not corossed)");
        cpu.page_crossed = false;
        // Read value from the effective address into the register
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationAbsoluteIndexed_ReadModifyWrite(
      CPU &cpu, u8 &idx_reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch low byte of address
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adl);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch high byte of address
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->adh);
        // Add index register to low byte to, if needed, trigger page crossing
        auto tmp = static_cast<u16>(cpu.bus->adl) + static_cast<u16>(idx_reg);
        cpu.bus->adl = U16Low(tmp);
        cpu.page_crossed = U16High(tmp) != 0;
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read value from effective address (always happens regardless of page
        // crossing)
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        if (cpu.page_crossed) {
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          cpu.page_crossed = false;
        }
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // reread value from effective address (now its correct)
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // dummy write
        ISA_detail::WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                    cpu.bus->op_latch);
        // Execute the operation (INC or DEC)
        ISA_detail::ExecuteOperation<OP>(cpu, cpu.bus->op_latch,
                                         cpu.bus->op_latch);
        ++cpu.instruction_cycle;
      } break;
      case 6: {
        // Final write
        ISA_detail::WriteValueToMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                    cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationIndirectY(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch pointer address
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->op_latch);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Fetch low byte of address
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch,
                                     cpu.bus->adl);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Fetch high byte of address
        const u8 high_byte = U16Low(static_cast<u16>(cpu.bus->op_latch) + 1);
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, high_byte, cpu.bus->adh);
        // Add Y to low byte to, if needed, trigger page crossing
        u16 tmp =
            static_cast<u16>(cpu.bus->adl) + static_cast<u16>(cpu.state.y);
        cpu.bus->adl = U16Low(tmp);
        cpu.page_crossed = U16High(tmp) != 0;
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Read value from the effective address into the register, if page was
        // crossed this is a dummy read
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        if (cpu.page_crossed) {
          // page was crossed, increment high byte and set op_latch to 0
          cpu.bus->adh = static_cast<u8>(cpu.bus->adh + 1);
          cpu.bus->op_latch = 0;
          ++cpu.instruction_cycle;
        } else {
          // page was not crossed, execute logical operation and set flags
          ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
          cpu.instruction_cycle = 0;
        }
      } break;
      case 5: {
        ASSERT(cpu.page_crossed,
               "Unexpected cycle for Logical Operation Indirect Y (page was "
               "not corossed)");
        cpu.page_crossed = false;
        // Read value from the effective address into the register
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationXIndirect(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch low byte of address
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->op_latch);
        ++cpu.state.pc;
        ++cpu.instruction_cycle;
      } break;
      case 2: {
        // Perform dummy read and add X to pointer (zero page wraparound)
        u8 dummy = 0;
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch, dummy);
        cpu.bus->op_latch =
            U16Low(static_cast<u16>(cpu.bus->op_latch) + cpu.state.x);
        ++cpu.instruction_cycle;
      } break;
      case 3: {
        // Read low byte of effective address
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.bus->op_latch,
                                     cpu.bus->adl);
        ++cpu.instruction_cycle;
      } break;
      case 4: {
        // Read high byte of effective address
        const u8 high_byte = U16Low(static_cast<u16>(cpu.bus->op_latch) + 1);
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, high_byte, cpu.bus->adh);
        ++cpu.instruction_cycle;
      } break;
      case 5: {
        // Read value from the effective address into the register
        ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                     cpu.bus->op_latch);
        ISA_detail::ExecuteOperation<OP>(cpu, reg, cpu.bus->op_latch);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <Operation OP>
  static QNES_FORCE_INLINE void OperationImplied(CPU &cpu, u8 &reg) {
    switch (cpu.instruction_cycle) {
      case 1: {
        u8 void_operand = 0;
        ISA_detail::ExecuteOperation<OP>(cpu, reg, void_operand);
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }

  template <BranchCondition CONDITION>
  static QNES_FORCE_INLINE void RelativeBranch(CPU &cpu) {
    switch (cpu.instruction_cycle) {
      case 1: {
        // Fetch operand
        ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                     U16Low(cpu.state.pc), cpu.bus->op_latch);
        // Evaluate branch condition
        if (!ISA_detail::ExecuteCondition<CONDITION>(cpu)) {
          // branch not taken - end instruction - next cycle will fetch the next
          // instruction
          cpu.instruction_cycle = 0;
        } else {
          ++cpu.instruction_cycle;
        }
        ++cpu.state.pc;
      } break;
      case 2: {
        // Fetch OPCODE for next instruction - dummy read
        u8 dummy = 0;
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.state.pc, dummy);
        // Add operand to PCL and check for page crossing
        auto offset = static_cast<int8_t>(cpu.bus->op_latch);
        u16 new_pc = cpu.state.pc + static_cast<int16_t>(offset);
        cpu.page_crossed = U16High(new_pc) != U16High(cpu.state.pc);
        cpu.state.pc = CombineToU16(U16High(cpu.state.pc), U16Low(new_pc));
        if (cpu.page_crossed) {
          ++cpu.instruction_cycle;
          cpu.bus->op_latch = U16High(new_pc);
        } else {
          cpu.instruction_cycle = 0;
        }
      } break;
      case 3: {
        ASSERT(cpu.page_crossed,
               "Unexpected cycle for Relative Branch (page was not crossed)");
        cpu.page_crossed = false;
        // Fetch OPCODE for next instruction - dummy read
        u8 dummy = 0;
        ISA_detail::ReadValueFromMem(cpu.bus, 0x00, cpu.state.pc, dummy);
        // Fix high byte of PC
        cpu.state.pc = CombineToU16(cpu.bus->op_latch, U16Low(cpu.state.pc));
        cpu.instruction_cycle = 0;
      } break;
      default:
        ASSERT(false, "Invalid cycle");
    }
  }
};

void ISA::PHA<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Dummy read of the next instruction byte
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // Push accumulator onto stack
      cpu.PushStack(cpu.state.a);
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::PLA<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Dummy read of the next instruction byte
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      cpu.IncrementSP();  // Increment stack pointer
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // Pull value into accumulator
      cpu.state.a = cpu.ReadStackValue();
      // Set Zero and Negative flags based on the value loaded
      ISA_detail::SetZNFlags(cpu, cpu.state.a);
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::PHP<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Dummy read of the next instruction byte
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // Push processor status onto stack
      auto status_to_push = cpu.state.status;
      // The break flag is set to 1 when pushed to the stack
      status_to_push.break_command = 1;
      status_to_push.unused = 1;
      cpu.PushStack(status_to_push.status);
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::PLP<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Dummy read of the next instruction byte
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      cpu.IncrementSP();  // Increment stack pointer
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // Pull value into processor status
      u8 status_value = cpu.ReadStackValue();
      cpu.state.status.status = status_value;
      // The break flag is ignored when pulled from the stack
      cpu.state.status.break_command = 0;
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::TSX<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      cpu.state.x = cpu.state.sp;
      ISA_detail::SetZNFlags(cpu, cpu.state.x);
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::TXS<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      cpu.state.sp = cpu.state.x;
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::LDA<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsolute(cpu, cpu.state.a);
}
void ISA::LDA<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterImmediate(cpu, cpu.state.a);
}
void ISA::LDA<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterZeroPage(cpu, cpu.state.a);
}
void ISA::LDA<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterZeroPageIndexed(cpu, cpu.state.a,
                                                  cpu.state.x);
}
void ISA::LDA<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsoluteIndexed(cpu, cpu.state.a,
                                                  cpu.state.x);
}
void ISA::LDA<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsoluteIndexed(cpu, cpu.state.a,
                                                  cpu.state.y);
}
void ISA::LDA<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterXIndirect(cpu, cpu.state.a, cpu.state.x);
}
void ISA::LDA<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterIndirectY(cpu, cpu.state.a);
}

void ISA::LDX<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsolute(cpu, cpu.state.x);
}
void ISA::LDX<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterImmediate(cpu, cpu.state.x);
}
void ISA::LDX<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterZeroPage(cpu, cpu.state.x);
}
void ISA::LDX<AddressingMode::ZeroPageY>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterZeroPageIndexed(cpu, cpu.state.x,
                                                  cpu.state.y);
}
void ISA::LDX<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsoluteIndexed(cpu, cpu.state.x,
                                                  cpu.state.y);
}

void ISA::LDY<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsolute(cpu, cpu.state.y);
}
void ISA::LDY<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterImmediate(cpu, cpu.state.y);
}
void ISA::LDY<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterZeroPage(cpu, cpu.state.y);
}
void ISA::LDY<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterZeroPageIndexed(cpu, cpu.state.y,
                                                  cpu.state.x);
}
void ISA::LDY<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::LoadMemoryToRegisterAbsoluteIndexed(cpu, cpu.state.y,
                                                  cpu.state.x);
}

void ISA::STA<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterAbsolute(cpu, cpu.state.a);
}
void ISA::STA<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterZeroPage(cpu, cpu.state.a);
}
void ISA::STA<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterZeroPageIndexed(cpu, cpu.state.a, cpu.state.x);
}
void ISA::STA<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterAbsoluteIndexed(cpu, cpu.state.a, cpu.state.x);
}
void ISA::STA<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterAbsoluteIndexed(cpu, cpu.state.a, cpu.state.y);
}
void ISA::STA<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterXIndirect(cpu, cpu.state.a, cpu.state.x);
}
void ISA::STA<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterIndirectY(cpu, cpu.state.a);
}

void ISA::STX<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterAbsolute(cpu, cpu.state.x);
}
void ISA::STX<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterZeroPage(cpu, cpu.state.x);
}
void ISA::STX<AddressingMode::ZeroPageY>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterZeroPageIndexed(cpu, cpu.state.x, cpu.state.y);
}

void ISA::STY<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterAbsolute(cpu, cpu.state.y);
}
void ISA::STY<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterZeroPage(cpu, cpu.state.y);
}
void ISA::STY<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::StoreRegisterZeroPageIndexed(cpu, cpu.state.y, cpu.state.x);
}

void ISA::TAX<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::TransferRegister(cpu, cpu.state.x, cpu.state.a);
}
void ISA::TAY<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::TransferRegister(cpu, cpu.state.y, cpu.state.a);
}
void ISA::TXA<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::TransferRegister(cpu, cpu.state.a, cpu.state.x);
}
void ISA::TYA<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::TransferRegister(cpu, cpu.state.a, cpu.state.y);
}

void ISA::AND<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::AND>(cpu, cpu.state.a);
}

void ISA::AND<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::AND>(cpu, cpu.state.a);
}

void ISA::AND<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed<ISA_detail::Operation::AND>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::AND<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::AND>(cpu, cpu.state.a);
}

void ISA::AND<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::AND>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::AND<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::AND>(
      cpu, cpu.state.a, cpu.state.y);
}

void ISA::AND<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::OperationXIndirect<ISA_detail::Operation::AND>(cpu, cpu.state.a);
}

void ISA::AND<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::OperationIndirectY<ISA_detail::Operation::AND>(cpu, cpu.state.a);
}

void ISA::EOR<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::EOR>(cpu, cpu.state.a);
}

void ISA::EOR<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::EOR>(cpu, cpu.state.a);
}

void ISA::EOR<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed<ISA_detail::Operation::EOR>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::EOR<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::EOR>(cpu, cpu.state.a);
}

void ISA::EOR<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::EOR>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::EOR<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::EOR>(
      cpu, cpu.state.a, cpu.state.y);
}

void ISA::EOR<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::OperationXIndirect<ISA_detail::Operation::EOR>(cpu, cpu.state.a);
}

void ISA::EOR<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::OperationIndirectY<ISA_detail::Operation::EOR>(cpu, cpu.state.a);
}

void ISA::ORA<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::ORA>(cpu, cpu.state.a);
}

void ISA::ORA<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::ORA>(cpu, cpu.state.a);
}

void ISA::ORA<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed<ISA_detail::Operation::ORA>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::ORA<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::ORA>(cpu, cpu.state.a);
}

void ISA::ORA<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::ORA>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::ORA<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::ORA>(
      cpu, cpu.state.a, cpu.state.y);
}

void ISA::ORA<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::OperationXIndirect<ISA_detail::Operation::ORA>(cpu, cpu.state.a);
}

void ISA::ORA<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::OperationIndirectY<ISA_detail::Operation::ORA>(cpu, cpu.state.a);
}

void ISA::BIT<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::BIT>(cpu, cpu.state.a);
}

void ISA::BIT<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::BIT>(cpu, cpu.state.a);
}

void ISA::ADC<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::ADC>(cpu, cpu.state.a);
}

void ISA::ADC<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::ADC>(cpu, cpu.state.a);
}

void ISA::ADC<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed<ISA_detail::Operation::ADC>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::ADC<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::ADC>(cpu, cpu.state.a);
}

void ISA::ADC<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::ADC>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::ADC<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::ADC>(
      cpu, cpu.state.a, cpu.state.y);
}

void ISA::ADC<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::OperationXIndirect<ISA_detail::Operation::ADC>(cpu, cpu.state.a);
}

void ISA::ADC<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::OperationIndirectY<ISA_detail::Operation::ADC>(cpu, cpu.state.a);
}

void ISA::SBC<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::SBC>(cpu, cpu.state.a);
}

void ISA::SBC<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::SBC>(cpu, cpu.state.a);
}

void ISA::SBC<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed<ISA_detail::Operation::SBC>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::SBC<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::SBC>(cpu, cpu.state.a);
}

void ISA::SBC<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::SBC>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::SBC<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::SBC>(
      cpu, cpu.state.a, cpu.state.y);
}

void ISA::SBC<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::OperationXIndirect<ISA_detail::Operation::SBC>(cpu, cpu.state.a);
}

void ISA::SBC<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::OperationIndirectY<ISA_detail::Operation::SBC>(cpu, cpu.state.a);
}

void ISA::CMP<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::CMP>(cpu, cpu.state.a);
}

void ISA::CMP<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::CMP>(cpu, cpu.state.a);
}

void ISA::CMP<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed<ISA_detail::Operation::CMP>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::CMP<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::CMP>(cpu, cpu.state.a);
}

void ISA::CMP<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::CMP>(
      cpu, cpu.state.a, cpu.state.x);
}

void ISA::CMP<AddressingMode::AbsoluteY>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed<ISA_detail::Operation::CMP>(
      cpu, cpu.state.a, cpu.state.y);
}

void ISA::CMP<AddressingMode::XIndirect>::Execute(CPU &cpu) {
  ISA_detail::OperationXIndirect<ISA_detail::Operation::CMP>(cpu, cpu.state.a);
}

void ISA::CMP<AddressingMode::IndirectY>::Execute(CPU &cpu) {
  ISA_detail::OperationIndirectY<ISA_detail::Operation::CMP>(cpu, cpu.state.a);
}

void ISA::CPX<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::CMP>(cpu, cpu.state.x);
}

void ISA::CPX<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::CMP>(cpu, cpu.state.x);
}

void ISA::CPX<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::CMP>(cpu, cpu.state.x);
}

void ISA::CPY<AddressingMode::Immediate>::Execute(CPU &cpu) {
  ISA_detail::OperationImmediate<ISA_detail::Operation::CMP>(cpu, cpu.state.y);
}

void ISA::CPY<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage<ISA_detail::Operation::CMP>(cpu, cpu.state.y);
}

void ISA::CPY<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute<ISA_detail::Operation::CMP>(cpu, cpu.state.y);
}

void ISA::INC<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage_ReadModifyWrite<ISA_detail::Operation::INC>(
      cpu);
}

void ISA::INC<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed_ReadModifyWrite<
      ISA_detail::Operation::INC>(cpu, cpu.state.x);
}

void ISA::INC<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute_ReadModifyWrite<ISA_detail::Operation::INC>(
      cpu);
}

void ISA::INC<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed_ReadModifyWrite<
      ISA_detail::Operation::INC>(cpu, cpu.state.x);
}

void ISA::INX<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::INC>(cpu, cpu.state.x);
}

void ISA::INY<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::INC>(cpu, cpu.state.y);
}

void ISA::DEC<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage_ReadModifyWrite<ISA_detail::Operation::DEC>(
      cpu);
}

void ISA::DEC<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed_ReadModifyWrite<
      ISA_detail::Operation::DEC>(cpu, cpu.state.x);
}

void ISA::DEC<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute_ReadModifyWrite<ISA_detail::Operation::DEC>(
      cpu);
}

void ISA::DEC<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed_ReadModifyWrite<
      ISA_detail::Operation::DEC>(cpu, cpu.state.x);
}

void ISA::DEX<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::DEC>(cpu, cpu.state.x);
}

void ISA::DEY<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::DEC>(cpu, cpu.state.y);
}

void ISA::ASL<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::SHIFT_LEFT>(cpu,
                                                                  cpu.state.a);
}

void ISA::ASL<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_LEFT>(cpu);
}

void ISA::ASL<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_LEFT>(cpu, cpu.state.x);
}

void ISA::ASL<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_LEFT>(cpu);
}

void ISA::ASL<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_LEFT>(cpu, cpu.state.x);
}

void ISA::LSR<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::SHIFT_RIGHT>(cpu,
                                                                   cpu.state.a);
}

void ISA::LSR<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_RIGHT>(cpu);
}

void ISA::LSR<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_RIGHT>(cpu, cpu.state.x);
}

void ISA::LSR<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_RIGHT>(cpu);
}

void ISA::LSR<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed_ReadModifyWrite<
      ISA_detail::Operation::SHIFT_RIGHT>(cpu, cpu.state.x);
}

void ISA::ROL<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::ROTATE_LEFT>(cpu,
                                                                   cpu.state.a);
}

void ISA::ROL<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_LEFT>(cpu);
}

void ISA::ROL<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_LEFT>(cpu, cpu.state.x);
}

void ISA::ROL<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_LEFT>(cpu);
}

void ISA::ROL<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_LEFT>(cpu, cpu.state.x);
}

void ISA::ROR<AddressingMode::Implied>::Execute(CPU &cpu) {
  ISA_detail::OperationImplied<ISA_detail::Operation::ROTATE_RIGHT>(
      cpu, cpu.state.a);
}

void ISA::ROR<AddressingMode::ZeroPage>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPage_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_RIGHT>(cpu);
}

void ISA::ROR<AddressingMode::ZeroPageX>::Execute(CPU &cpu) {
  ISA_detail::OperationZeroPageIndexed_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_RIGHT>(cpu, cpu.state.x);
}

void ISA::ROR<AddressingMode::Absolute>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsolute_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_RIGHT>(cpu);
}

void ISA::ROR<AddressingMode::AbsoluteX>::Execute(CPU &cpu) {
  ISA_detail::OperationAbsoluteIndexed_ReadModifyWrite<
      ISA_detail::Operation::ROTATE_RIGHT>(cpu, cpu.state.x);
}

void ISA::JMP<AddressingMode::Absolute>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Fetch low byte of address
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), cpu.bus->op_latch);
      ++cpu.state.pc;
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // Fetch high byte of address
      u16 address = cpu.bus->op_latch;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), cpu.bus->op_latch);
      address = (cpu.bus->op_latch << 8) | address;
      cpu.state.pc = address;
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::JMP<AddressingMode::Indirect>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Fetch low byte of pointer
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), cpu.bus->adl);
      ++cpu.state.pc;
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // Fetch high byte of pointer
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), cpu.bus->adh);
      ++cpu.state.pc;
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // Fetch low byte of address
      ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl,
                                   cpu.bus->op_latch);
      ++cpu.instruction_cycle;
    } break;
    case 4: {
      // Fetch high byte of address and set PC
      u16 address = cpu.bus->op_latch;
      ISA_detail::ReadValueFromMem(cpu.bus, cpu.bus->adh, cpu.bus->adl + 1,
                                   cpu.bus->op_latch);
      address = CombineToU16(cpu.bus->op_latch, address);
      cpu.state.pc = address;
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::JSR<AddressingMode::Absolute>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Fetch low byte of address
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), cpu.bus->adl);
      ++cpu.state.pc;
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // internal operation
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // push PCH on stack
      cpu.PushStack(cpu.state.pc >> 8);
      ++cpu.instruction_cycle;
    } break;
    case 4: {
      // push PCL on stack
      cpu.PushStack(cpu.state.pc & 0xFF);
      ++cpu.instruction_cycle;
    } break;
    case 5: {
      // Fetch high byte of address and set PC
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), cpu.bus->adh);
      u16 address = CombineToU16(cpu.bus->adh, cpu.bus->adl);
      cpu.state.pc = address;
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::RTS<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // Read next PC byte and throw it away
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // increment SP
      cpu.IncrementSP();
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // pull PCL from stack and increment SP
      cpu.state.pc = CombineToU16(0x00, cpu.ReadStackValue());
      cpu.IncrementSP();
      ++cpu.instruction_cycle;
    } break;
    case 4: {
      // pull PCH from stack - dont increment SP
      cpu.state.pc = CombineToU16(cpu.ReadStackValue(), cpu.state.pc & 0xFF);
      ++cpu.instruction_cycle;
    } break;
    case 5: {
      // increment PC
      ++cpu.state.pc;
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::BCC<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BCC>(cpu);
}

void ISA::BCS<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BCS>(cpu);
}

void ISA::BEQ<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BEQ>(cpu);
}

void ISA::BMI<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BMI>(cpu);
}

void ISA::BNE<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BNE>(cpu);
}

void ISA::BPL<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BPL>(cpu);
}

void ISA::BVC<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BVC>(cpu);
}

void ISA::BVS<AddressingMode::Relative>::Execute(CPU &cpu) {
  ISA_detail::RelativeBranch<ISA_detail::BranchCondition::BVS>(cpu);
}

void ISA::CLC<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.carry = false;
  cpu.instruction_cycle = 0;
}

void ISA::CLD<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.decimal_mode = false;
  cpu.instruction_cycle = 0;
}

void ISA::CLI<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.interrupt_disable = false;
  cpu.instruction_cycle = 0;
}

void ISA::CLV<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.overflow = false;
  cpu.instruction_cycle = 0;
}

void ISA::SEC<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.carry = true;
  cpu.instruction_cycle = 0;
}

void ISA::SED<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.decimal_mode = true;
  cpu.instruction_cycle = 0;
}

void ISA::SEI<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.state.status.interrupt_disable = true;
  cpu.instruction_cycle = 0;
}

void ISA::NOP<AddressingMode::Implied>::Execute(CPU &cpu) {
  ASSERT(cpu.instruction_cycle == 1, "Invalid cycle");
  cpu.instruction_cycle = 0;
}

void ISA::RTI<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // dummy read
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.state.pc;
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // increment SP
      cpu.IncrementSP();
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // pull status from stack, increment SP
      cpu.state.status.status = cpu.ReadStackValue();
      cpu.IncrementSP();
      ++cpu.instruction_cycle;
    } break;
    case 4: {
      // pull PCL from stack, increment SP
      cpu.state.pc = CombineToU16(cpu.state.pc, cpu.ReadStackValue());
      cpu.IncrementSP();
      ++cpu.instruction_cycle;
    } break;
    case 5: {
      // pull PCH from stack
      cpu.state.pc = CombineToU16(cpu.ReadStackValue(), U16Low(cpu.state.pc));
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

void ISA::BRK<AddressingMode::Implied>::Execute(CPU &cpu) {
  switch (cpu.instruction_cycle) {
    case 1: {
      // dummy read
      u8 dummy = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, U16High(cpu.state.pc),
                                   U16Low(cpu.state.pc), dummy);
      ++cpu.state.pc;
      ++cpu.instruction_cycle;
    } break;
    case 2: {
      // Push  PCH to stack
      cpu.PushStack(U16High(cpu.state.pc));
      ++cpu.instruction_cycle;
    } break;
    case 3: {
      // Push PCL to stack
      cpu.PushStack(U16Low(cpu.state.pc));
      ++cpu.instruction_cycle;
    } break;
    case 4: {
      // Push status to stack - set B flag
      cpu.state.status.break_command = 1;
      cpu.state.status.unused = 1;
      cpu.PushStack(cpu.state.status.status);
      cpu.state.status.interrupt_disable = 1;
      ++cpu.instruction_cycle;
    } break;
    case 5: {
      // Fetch low byte of interrupt vector
      u8 low_byte = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, 0xFF, 0xFE, low_byte);
      cpu.state.pc = CombineToU16(U16High(cpu.state.pc), low_byte);
      ++cpu.instruction_cycle;
    } break;
    case 6: {
      // Fetch high byte of interrupt vector
      u8 high_byte = 0;
      ISA_detail::ReadValueFromMem(cpu.bus, 0xFF, 0xFF, high_byte);
      cpu.state.pc = CombineToU16(high_byte, U16Low(cpu.state.pc));
      cpu.instruction_cycle = 0;
    } break;
    default:
      ASSERT(false, "Invalid cycle");
  }
}

}  // namespace QNes
