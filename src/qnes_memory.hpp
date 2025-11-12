#pragma once
#include <algorithm>
#include <memory>
#include <span>

#include "qnes_c.hpp"

namespace QNes {

class Memory {
 public:
  Memory(size_t size) : size(size), data(std::make_unique<u8[]>(size)) {};
  Memory(const Memory &) = delete;
  Memory &operator=(const Memory &) = delete;
  Memory(Memory &&) = delete;
  Memory &operator=(Memory &&) = delete;
  ~Memory() = default;

  [[nodiscard]] u8 Read(uint16_t address) const { return data[address]; }
  void Write(u16 address, u8 value) { data[address] = value; }

  void Clear() { std::ranges::fill(data.get(), data.get() + size, 0); }

  void Initialize(std::span<const u8> data) {
    std::ranges::copy(data, this->data.get());
  }

  void InitializeFrom(size_t offset, std::span<const u8> data) {
    ASSERT(offset + data.size() <= size,
           "Offset and data size exceed memory size");
    std::ranges::copy(data, this->data.get() + offset);
  }

  [[nodiscard]] size_t GetSize() const { return size; }

 private:
  size_t size;
  std::unique_ptr<u8[]> data;
};

using MemoryPtr = std::unique_ptr<Memory>;
}  // namespace QNes