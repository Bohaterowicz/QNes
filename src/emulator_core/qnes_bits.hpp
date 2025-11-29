#pragma once

#include "qnes_c.hpp"

namespace QNes {

inline u8 U16Low(u16 value) { return static_cast<u8>(value & 0xFF); }
inline u8 U16High(u16 value) { return static_cast<u8>(value >> 8); }

inline u16 CombineToU16(u8 high, u8 low) {
  return (static_cast<u16>(high) << 8) | static_cast<u16>(low);
}

}  // namespace QNes
