#include "qnes_emu.hpp"
#include <gtest/gtest.h>


// Demonstrate some basic assertions.
TEST(HelloQnesTest, Basic) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "qnes");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(QNesEmulatorBasicTest, Startup) {
  QNes::Emulator emulator;
  EXPECT_EQ(1, 1);
}
