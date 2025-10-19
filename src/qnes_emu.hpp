#pragma once

#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"

namespace QNes {

class Emulator {
 public:
  Emulator() : memory(Kilobytes(64)), cpu(memory){};
  Emulator(const Emulator &) = delete;
  Emulator &operator=(const Emulator &) = delete;
  Emulator(Emulator &&) = delete;
  Emulator &operator=(Emulator &&) = delete;
  Emulator(size_t memory_size) : memory(memory_size), cpu(memory){};
  ~Emulator() = default;

  void Run();

 private:
  Memory memory;  // 64KB of memory
  CPU cpu;
};

}  // namespace QNes