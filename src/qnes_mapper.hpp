#pragma once

#include "qnes_c.hpp"

namespace QNes {

class Cartridge;

class Mapper {
 public:
  Mapper(Cartridge *cartridge) noexcept : cartridge(cartridge) {};

  Mapper(const Mapper &) = delete;
  Mapper &operator=(const Mapper &) = delete;
  Mapper(Mapper &&other) noexcept = default;
  Mapper &operator=(Mapper &&other) noexcept = default;
  ~Mapper() noexcept = default;

  [[nodiscard]] u8 CPURead(u16 address) const;
  void CPUWrite(u16 address, u8 value);

  [[nodiscard]] u8 PPURead(u16 address) const;
  void PPUWrite(u16 address, u8 value);

 private:
  Cartridge *cartridge = nullptr;

  friend class Cartridge;
};

}  // namespace QNes