#pragma once

#include <optional>
#include <span>

#include "qnes_c.hpp"
#include "qnes_mapper.hpp"

namespace QNes {

static constexpr u8 INES_HEADER_SIGNATURE[] = {0x4E, 0x45, 0x53, 0x1A};
static constexpr u8 INES_CHR_RAM_FLAG = 0b00000010;
static constexpr u8 INES_TRAINER_FLAG = 0b00000100;

static constexpr size_t INES_PRG_ROM_BANK_SIZE = Kilobytes(16);
static constexpr size_t INES_CHR_ROM_BANK_SIZE = Kilobytes(8);
static constexpr size_t INES_CHR_RAM_BANK_SIZE = Kilobytes(8);

struct INESHeader {
  u8 signature[4];
  u8 prg_rom_bank_count;
  u8 chr_rom_bank_count;
  u8 flags_6;
  u8 flags_7;
  u8 flags_8;
  u8 flags_9;
  u8 flags_10;
  u8 unused[5];
};
static_assert(sizeof(INESHeader) == 16);

class Cartridge {
 public:
  enum class NESFormat : u8 {
    INES,
    NES20,
  };

  enum class MirroringMode : u8 {
    Horizontal,
    Vertical,
    FourScreen,
    SingleScreen,
  };

  Cartridge(INESHeader header, std::span<const u8> data) noexcept;

  Cartridge(const Cartridge &) noexcept = delete;
  Cartridge &operator=(const Cartridge &) noexcept = delete;
  Cartridge(Cartridge &&other) noexcept
      : mapper(std::move(other.mapper)),
        header(other.header),
        format(other.format),
        mapper_number(other.mapper_number),
        submapper_number(other.submapper_number),
        prg_rom_data(other.prg_rom_data),
        chr_rom_data(other.chr_rom_data),
        chr_ram_data(std::move(other.chr_ram_data)) {
    mapper->cartridge = this;
    other.mapper = nullptr;
    other.header = {};
    other.format = NESFormat::INES;
    other.mapper_number = 0;
    other.submapper_number = 0;
    other.prg_rom_data = {};
    other.chr_rom_data = std::nullopt;
    other.chr_ram_data = std::nullopt;
  }
  Cartridge &operator=(Cartridge &&other) noexcept {
    if (this != &other) {
      mapper = std::move(other.mapper);
      mapper->cartridge = this;
      header = other.header;
      format = other.format;
      mapper_number = other.mapper_number;
      submapper_number = other.submapper_number;
      prg_rom_data = other.prg_rom_data;
      chr_rom_data = other.chr_rom_data;
      chr_ram_data = std::move(other.chr_ram_data);
      other.mapper = nullptr;
      other.header = {};
      other.format = NESFormat::INES;
      other.mapper_number = 0;
      other.submapper_number = 0;
      other.prg_rom_data = {};
      other.chr_rom_data = std::nullopt;
      other.chr_ram_data = std::nullopt;
    }
    return *this;
  }

  ~Cartridge() noexcept = default;

  [[nodiscard]] MirroringMode GetMirroringMode() const {
    return MirroringMode::Horizontal;
  }

  [[nodiscard]] u8 CPURead(u16 address) const {
    return mapper->CPURead(address);
  }
  void CPUWrite(u16 address, u8 value) { mapper->CPUWrite(address, value); }

  [[nodiscard]] u8 PPURead(u16 address) const {
    return mapper->PPURead(address);
  }
  void PPUWrite(u16 address, u8 value) { mapper->PPUWrite(address, value); }

 private:
  std::unique_ptr<Mapper> mapper = nullptr;
  INESHeader header{};
  NESFormat format;
  u16 mapper_number = 0;
  u8 submapper_number = 0;
  std::span<const u8> prg_rom_data;
  std::optional<std::span<const u8>> chr_rom_data;
  std::optional<std::unique_ptr<u8[]>> chr_ram_data;

  friend class Mapper;
};

[[nodiscard]] std::optional<Cartridge> LoadCartridge(std::span<const u8> data);

}  // namespace QNes