#include "qnes_cartridge.hpp"

namespace QNes {

Cartridge::Cartridge(const INESHeader header, std::span<const u8> data) noexcept
    : header(header), format(NESFormat::INES) {
  if (((header.flags_7 >> 2) & 0b11) == 0b10) {
    format = NESFormat::NES20;
  }

  u8 offset = 0;
  if ((header.flags_6 & INES_TRAINER_FLAG) == INES_TRAINER_FLAG) {
    offset += 512;
  }
  size_t prg_rom_size = header.prg_rom_bank_count * INES_PRG_ROM_BANK_SIZE;
  prg_rom_data = data.subspan(offset, prg_rom_size);
  offset += prg_rom_size;
  size_t chr_rom_size = header.chr_rom_bank_count * INES_CHR_ROM_BANK_SIZE;
  // If CHR-ROM size is 0, it means the cartridge has no CHR-ROM data - use
  // CHR-RAM instead
  if (chr_rom_size > 0) {
    chr_rom_data = data.subspan(offset, chr_rom_size);
  } else {
    chr_rom_data = std::nullopt;
  }

  u8 mapper_number_f6 = header.flags_6 >> 4;
  u8 mapper_number_f7 = header.flags_7 >> 4;
  if (format == NESFormat::INES) {
    mapper_number = mapper_number_f7 << 4 | mapper_number_f6;
    submapper_number = 0;
  } else {
    u8 mapper_number_f8 = header.flags_8 & 0x0F;
    mapper_number =
        mapper_number_f8 << 8 | mapper_number_f7 << 4 | mapper_number_f6;
    submapper_number = mapper_number_f8 >> 4;
  }

  if (format == NESFormat::INES) {
    if ((header.flags_6 & INES_CHR_RAM_FLAG) == INES_CHR_RAM_FLAG) {
      // if flags_8 is 0, use 1 bank of CHR-RAM - common fallback
      size_t chr_ram_bank_size = header.flags_8 != 0 ? header.flags_8 : 1;
      chr_ram_data =
          std::make_unique<u8[]>(chr_ram_bank_size * INES_CHR_RAM_BANK_SIZE);
    } else {
      chr_ram_data = std::nullopt;
    }
  } else {
    // TODO: Implement NES20 CHR-RAM handling
  }

  mapper = std::make_unique<Mapper>(this);
}

std::optional<Cartridge> LoadCartridge(std::span<const u8> data) {
  // Verify header signature (iNES)
  INESHeader header{};
  std::memcpy(&header, data.data(), sizeof(INESHeader));
  if (std::memcmp(header.signature, INES_HEADER_SIGNATURE,
                  sizeof(INES_HEADER_SIGNATURE)) != 0) {
    return std::nullopt;
  }

  Cartridge cartridge{header, data.subspan(sizeof(INESHeader))};
  return cartridge;
}

}  // namespace QNes