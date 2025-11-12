#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "qnes_bus.hpp"
#include "qnes_c.hpp"
#include "qnes_cpu.hpp"
#include "qnes_memory.hpp"


// Helper function to find the binary file in common locations
std::string FindBinaryFile() {
  std::vector<std::string> possible_paths = {
      // Build directory (where CMake copies it)
      "bin/test_roms/6502_functional_tests.bin",
      "test_roms/6502_functional_tests.bin",
      // Source directory locations
      "test/test_roms/6502_functional_tests.bin",
      "../test/test_roms/6502_functional_tests.bin",
      "../../test/test_roms/6502_functional_tests.bin",
      "../../../test/test_roms/6502_functional_tests.bin",
      // Working directory
      "6502_functional_tests.bin",
  };

  for (const auto &path : possible_paths) {
    std::ifstream file(path, std::ios::binary);
    if (file.is_open()) {
      file.close();
      return path;
    }
  }

  return "";
}

int main() {
  std::cout << "Klaus 6502 Functional Test Runner\n";
  std::cout << "==================================\n\n";

  // Find and load the binary file
  std::string binary_path = FindBinaryFile();
  if (binary_path.empty()) {
    std::cerr << "ERROR: Could not find 6502_functional_tests.bin\n";
    std::cerr << "Please ensure the binary file is in the working directory "
                 "or a test_roms subdirectory.\n";
    return 1;
  }

  std::cout << "Loading binary: " << binary_path << "\n";

  std::ifstream file(binary_path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "ERROR: Failed to open binary file\n";
    return 1;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<u8> binary_data(size);
  if (!file.read(reinterpret_cast<char *>(binary_data.data()), size)) {
    std::cerr << "ERROR: Failed to read binary data\n";
    return 1;
  }

  std::cout << "Binary size: " << size << " bytes\n\n";

  // Create CPU and Memory
  QNes::Memory memory(Kilobytes(64));
  memory.Clear();

  // Load binary data
  if (binary_data.size() > Kilobytes(64)) {
    std::cerr << "ERROR: Binary file too large to fit in memory\n";
    return 1;
  }
  memory.Initialize(binary_data);

  // Set up reset vector to point to test start (0x0400)
  constexpr u16 test_start = 0x0400;
  memory.Write(0xFFFC, QNes::U16Low(test_start));
  memory.Write(0xFFFD, QNes::U16High(test_start));

  QNes::RAMBus bus(&memory);
  QNes::CPU cpu(&bus);

  std::cout << "Initializing CPU...\n";

  // Reset the CPU
  QNes::CPU_Testing::ZeroInterruptCycle(cpu);
  cpu.Reset();

  // Execute reset sequence (5 cycles)
  for (int i = 0; i < 5; ++i) {
    cpu.Step();
  }

  auto initial_state = cpu.GetState();
  std::cout << "CPU reset complete. PC: 0x" << std::hex << initial_state.pc
            << std::dec << "\n\n";

  if (initial_state.pc != test_start) {
    std::cerr << "ERROR: CPU did not reset to test start address (0x"
              << std::hex << test_start << "). Got: 0x" << initial_state.pc
              << std::dec << "\n";
    return 1;
  }

  std::cout << "Running functional test...\n";
  std::cout
      << "This may take a while. The test will loop when it completes.\n\n";

  // Run until we detect a loop (repeating pattern of 2-3 addresses)
  // jmp * instruction cycles through 2-3 addresses before jumping back
  constexpr size_t repeat_count = 10;
  constexpr size_t sequence_size = 3;
  u16 sequence[sequence_size] = {};
  constexpr size_t history_size =
      repeat_count * sequence_size;  // Track enough to detect patterns
  u16 pc_history[history_size] = {};
  size_t history_index = 0;  // Current write position in circular buffer
  size_t history_count = 0;  // Number of entries filled so far
  constexpr size_t max_cycles = 100000000;  // Safety limit (100M cycles)

  bool test_completed = false;
  u16 success_pcs[3] = {0x336d, 0x336e, 0x336f};
  u16 final_pc = 0;
  size_t cycle_count = 0;

  while (cycle_count < max_cycles) {
    cpu.Step();
    ++cycle_count;

    auto state = cpu.GetState();
    u16 current_pc = state.pc;

    pc_history[history_index++] = current_pc;
    history_index %= history_size;
    if (history_count >= history_size) {
      // find the first elements of sequence in pc_history
      for (size_t i = 0; i < sequence_size; ++i) {
        sequence[i] = pc_history[i];
      }

      // check if the next elements of pc_history are the same as sequence
      bool pattern_found = true;
      for (size_t i = 1; i < repeat_count; ++i) {
        for (size_t j = 0; j < sequence_size; ++j) {
          if (pc_history[(i * sequence_size) + j] != sequence[j]) {
            pattern_found = false;
            break;
          }
        }
        if (!pattern_found) {
          break;
        }
      }

      if (pattern_found) {
        test_completed = true;
        final_pc = current_pc;
        break;
      }
    } else {
      ++history_count;
    }

    // Progress reporting every 1M cycles
    if (cycle_count % 1000000 == 0) {
      std::cout << "Cycles: " << cycle_count / 1000000 << "M, PC: 0x"
                << std::hex << current_pc << std::dec << "\n";
    }
  }

  if (!test_completed) {
    std::cerr << "\nERROR: Test did not complete within " << max_cycles
              << " cycles\n";
    auto final_state = cpu.GetState();
    std::cerr << "Final PC: 0x" << std::hex << final_state.pc << std::dec
              << "\n";
    return 1;
  }

  // Determine test result based on final PC
  std::cout << "\nTest completed!\n";
  std::cout << "Total cycles: " << cycle_count << "\n";
  std::cout << "Final PC: 0x" << std::hex << final_pc << std::dec << "\n";

  auto final_state = cpu.GetState();
  std::cout << "\nFinal CPU State:\n";
  std::cout << "  PC: 0x" << std::hex << final_state.pc << std::dec << "\n";
  std::cout << "  SP: 0x" << std::hex << static_cast<int>(final_state.sp)
            << std::dec << "\n";
  std::cout << "  A:  0x" << std::hex << static_cast<int>(final_state.a)
            << std::dec << "\n";
  std::cout << "  X:  0x" << std::hex << static_cast<int>(final_state.x)
            << std::dec << "\n";
  std::cout << "  Y:  0x" << std::hex << static_cast<int>(final_state.y)
            << std::dec << "\n";
  std::cout << "  Status: 0x" << std::hex
            << static_cast<int>(final_state.status.status) << std::dec << "\n";

  if (std::ranges::find(success_pcs, final_pc) != std::end(success_pcs)) {
    std::cout << "\n*** TEST PASSED ***\n";
    std::cout << "PC is at the success location (0x" << std::hex << final_pc
              << std::dec << ")\n";
    return 0;
  } else {
    // Any other address with a loop is likely a failure trap
    std::cerr << "\n*** TEST FAILED ***\n";
    std::cerr << "PC indicates a test failure (PC = 0x" << std::hex << final_pc
              << std::dec << ")\n";
    std::cerr << "Expected success at 0x" << std::hex << success_pcs[0]
              << std::dec << "\n";
    std::cerr << "Check the assembly listing for the trap at PC 0x" << std::hex
              << final_pc << std::dec << "\n";
    return 1;
  }
}
