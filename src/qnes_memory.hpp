#pragma once
#include <algorithm>
#include <memory>

#include "qnes_c.hpp"

namespace QNes {

class Memory {
 public:
  Memory(size_t size) : size(size), data(std::make_unique<u8[]>(size)){};
  Memory(const Memory &) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(Memory &&) = delete;
  ~Memory() = default;

  [[nodiscard]] u8 Read(uint16_t address) const { return data[address]; }
  void Write(u16 address, u8 value) { data[address] = value; }

  void Clear() { std::ranges::fill(data.get(), data.get() + size, 0); }

 private:
  size_t size;
  std::unique_ptr<u8[]> data;
};

}  // namespace QNes