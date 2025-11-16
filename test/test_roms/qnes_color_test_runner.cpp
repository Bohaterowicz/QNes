#include <fstream>
#include <iostream>
#include <vector>

#include "qnes_cartridge.hpp"
#include "qnes_emu.hpp"

static std::string FindBinaryFile() {
  std::vector<std::string> possible_paths = {
      "test_roms/color_test.nes",
  };
  return possible_paths[0];
}

int main() {
  std::string binary_path = FindBinaryFile();

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

  auto cartridge = QNes::LoadCartridge(binary_data);
  if (!cartridge) {
    std::cerr << "Failed to load cartridge\n";
    return 1;
  }

  QNes::Emulator emulator;
  emulator.SetCartridge(&cartridge.value());
  emulator.Run();

  return 0;
}