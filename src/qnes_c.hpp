#pragma once
#include <cstdint>
#include <print>

// Compiler detection
#if defined(_MSC_VER)
#define QNES_COMPILER_MSVC 1
#define QNES_COMPILER_GCC 0
#define QNES_COMPILER_CLANG 0
#elif defined(__GNUC__) && !defined(__clang__)
#define QNES_COMPILER_MSVC 0
#define QNES_COMPILER_GCC 1
#define QNES_COMPILER_CLANG 0
#elif defined(__clang__)
#define QNES_COMPILER_MSVC 0
#define QNES_COMPILER_GCC 0
#define QNES_COMPILER_CLANG 1
#else
#error Unsupported compiler (or unrecognized) compiler error...
#endif

// Force inline macro: Only force inline in non-debug builds
#if defined(NDEBUG)
#if QNES_COMPILER_MSVC
#define QNES_FORCE_INLINE __forceinline
#elif QNES_COMPILER_GCC || QNES_COMPILER_CLANG
#define QNES_FORCE_INLINE __attribute__((always_inline))
#else
#define QNES_FORCE_INLINE inline
#endif
#else
#define QNES_FORCE_INLINE inline
#endif

#ifdef NDEBUG
#define DBG_PRINT(msg) ((void)0)
#else
#define DBG_PRINT(msg) \
  std::println("[DEBUG] {}:{} in {}() - {}", __FILE__, __LINE__, __func__, msg)
#endif

// Debug assertion
#ifndef NDEBUG
#define ASSERT(Expression, msg)    \
  if (!(Expression)) {             \
    DBG_PRINT(msg);                \
    volatile int *crash = nullptr; \
    *crash = 0;                    \
  }
#else
#define ASSERT(Expression, msg)
#endif

// Type aliases
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using f32 = float;
using f64 = double;

// Size conversion functions
constexpr u64 Kilobytes(size_t size) { return size * 1024ULL; }
constexpr u64 Megabytes(size_t size) { return Kilobytes(size) * 1024ULL; }
constexpr u64 Gigabytes(size_t size) { return Megabytes(size) * 1024ULL; }
constexpr u64 Terabytes(size_t size) { return Gigabytes(size) * 1024ULL; }