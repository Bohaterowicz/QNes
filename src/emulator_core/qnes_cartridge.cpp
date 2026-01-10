#include "qnes_cartridge.hpp"

#include <vector>

#include "qnes_texture.hpp"

namespace QNes {

Cartridge::Cartridge(DataBuffer data, INESHeader header) noexcept
    : header(header), format(NESFormat::INES), raw_data(std::move(data)) {
  std::span<const u8> data_span =
      std::span<const u8>(raw_data.data.get(), raw_data.size);
  if (((header.flags_7 >> 2) & 0b11) == 0b10) {
    format = NESFormat::NES20;
  }

  // skip header
  size_t offset = sizeof(INESHeader);
  if ((header.flags_6 & INES_TRAINER_FLAG) == INES_TRAINER_FLAG) {
    offset += 512;
  }
  size_t prg_rom_size = header.prg_rom_bank_count * INES_PRG_ROM_BANK_SIZE;
  prg_rom_data = data_span.subspan(offset, prg_rom_size);
  offset += prg_rom_size;
  size_t chr_rom_size = header.chr_rom_bank_count * INES_CHR_ROM_BANK_SIZE;
  // If CHR-ROM size is 0, it means the cartridge has no CHR-ROM data - use
  // CHR-RAM instead
  if (chr_rom_size > 0) {
    chr_rom_data = data_span.subspan(offset, chr_rom_size);
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

std::optional<Cartridge> LoadCartridge(DataBuffer data) {
  // Verify header signature (iNES)
  INESHeader header{};
  std::memcpy(&header, data.data.get(), sizeof(INESHeader));
  if (std::memcmp(header.signature, INES_HEADER_SIGNATURE,
                  sizeof(INES_HEADER_SIGNATURE)) != 0) {
    return std::nullopt;
  }

  Cartridge cartridge{std::move(data), header};
  return cartridge;
}

void FlipTextureVertical(u8 *image_ptr, u32 width, u32 height,
                         u32 bytes_per_pixel) {
  if (image_ptr == nullptr || height <= 1 || width <= 0 ||
      bytes_per_pixel <= 0) {
    return;  // Nothing to flip
  }

  // Calculate the size of a single row in bytes
  size_t row_size_bytes = (size_t)width * bytes_per_pixel;

  // Use a vector as a temporary buffer to store one row during the swap
  std::vector<u8> temp_row(row_size_bytes);

  // Only iterate through the first half of the rows
  for (int i = 0; i < height / 2; ++i) {
    // 1. Calculate pointers to the current top and bottom rows
    // Current row (top half)
    unsigned char *row_top = image_ptr + i * row_size_bytes;

    // Corresponding row from the bottom half
    unsigned char *row_bottom = image_ptr + (height - 1 - i) * row_size_bytes;

    // 2. Copy the top row data into the temporary buffer
    std::copy(row_top, row_top + row_size_bytes, temp_row.begin());

    // 3. Copy the bottom row data into the top row's position
    std::copy(row_bottom, row_bottom + row_size_bytes, row_top);

    // 4. Copy the temporary buffer (original top row data) into the bottom
    // row's position
    std::copy(temp_row.begin(), temp_row.end(), row_bottom);
  }
}

void DecodePatternTable(const Cartridge &cartridge, NESTexture &texture,
                        size_t pattern_table_index) {
  const auto *chr_ptr = cartridge.GetCHRROMDataPtr();

  static constexpr u32 colors[4] = {
      0x000000FF,  // Black (index 0)
      0x555555FF,  // Gray (index 1)
      0xAAAAAAFF,  // Light Gray (index 2)
      0xFFFFFFFF   // White (index 3)
  };
  constexpr size_t pattern_table_size = 0x1000;
  constexpr size_t tile_count = 256;

  const size_t pattern_table_offset = pattern_table_size * pattern_table_index;

  auto [width, height] = texture.GetDimensions();
  ASSERT(width == 128 && height == 128,
         "Pattern table texture must be 128x128");

  for (size_t i = 0; i < tile_count; i++) {
    const u8 *tile_plane_base = chr_ptr + (pattern_table_offset + i * 16);
    size_t tile_grid_x = i % 16;
    size_t tile_grid_y = i / 16;
    for (size_t y = 0; y < 8; y++) {
      u8 plane_low_byte = tile_plane_base[y];
      u8 plane_high_byte = tile_plane_base[y + 8];
      for (size_t x = 0; x < 8; x++) {
        const size_t bit_idx = 7 - x;
        auto bit_low = (plane_low_byte >> bit_idx) & 0x01;
        auto bit_high = (plane_high_byte >> bit_idx) & 0x01;
        auto color_index = (bit_high << 1) | bit_low;

        auto pixel_x = (tile_grid_x * 8) + x;
        auto pixel_y = (tile_grid_y * 8) + y;
        size_t buffer_index_start = (pixel_y * width + pixel_x) * 4;

        auto color = colors[color_index];

        texture.GetData()[buffer_index_start + 0] = (color >> 24) & 0xFF;
        texture.GetData()[buffer_index_start + 1] = (color >> 16) & 0xFF;
        texture.GetData()[buffer_index_start + 2] = (color >> 8) & 0xFF;
        texture.GetData()[buffer_index_start + 3] = (color >> 0) & 0xFF;
      }
    }
  }
  FlipTextureVertical(texture.GetData(), width, height, 4);
}

}  // namespace QNes
