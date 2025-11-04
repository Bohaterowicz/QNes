# 6502 Assembler (as65)

This directory contains the **as65.exe** 6502 CPU assembler, a command-line tool used to assemble 6502 assembly code into binary machine code.

## Purpose

The assembler is primarily used to assemble the [Klaus Dormann 6502 Functional Test Suite](https://github.com/Klaus2m5/6502_65C02_functional_tests), which provides comprehensive test coverage for 6502 CPU emulators. The test suite validates that the CPU implementation correctly handles all instructions, addressing modes, and edge cases.

## Assembler Details

- **Tool**: as65.exe (6502 assembler)
- **Author**: Klaus Dormann
- **GitHub Repository**: https://github.com/Klaus2m5/6502_65C02_functional_tests
- **License**: See the original repository for licensing information

## Usage

### Basic Command

```bash
.\as65.exe -l -m -w -h0 test\test_roms\6502_functional_tests.a65
```

### Command-Line Options

- `-l` - Generate listing file (.lst)
- `-m` - Generate map file
- `-w` - Enable warnings
- `-h0` - Set hex output format

### Output Files

The assembler produces the following files:

- **`6502_functional_tests.bin`** - Binary machine code (64 KB)
  - Fits exactly into the 6502's 64 KB address space
  - Can be directly loaded into emulator memory
  - Execution starts at address `0x0400` (code segment)

- **`6502_functional_tests.lst`** - Human-readable assembly listing
  - Shows assembled instructions with addresses
  - Useful for debugging and understanding test flow
  - Includes labels, macros, and line numbers

## Test Suite Integration

### Source File

The functional test source is located at:
```
test/test_roms/6502_functional_tests.a65
```

### Test Runner

The test logic is implemented in:
```
test/test_roms/cpu_functional_test.cpp
```

This file produces a standalone executable (`qnes_functional_test`) that:
- Loads the assembled binary into memory
- Initializes the CPU state
- Executes the test suite
- Detects completion via looping patterns
- Reports pass/fail status based on final PC value

### Building and Running

When the project is built:
1. The assembler is used to create the binary from the source
2. The binary is automatically copied to the build directory (via CMake)
3. The test runner executable can be run independently:
   ```bash
   ./build/bin/qnes_functional_test
   ```

## Test Execution Flow

1. **Load**: Binary is loaded into memory starting at `0x0400`
2. **Initialize**: CPU is reset and PC is set to test start address
3. **Execute**: CPU cycles until a looping pattern is detected
4. **Verify**: Final PC is checked against expected success address (`0x336d`)
5. **Report**: Test result (pass/fail) is displayed

## Notes

- The assembled binary is exactly 64 KB, matching the 6502's address space
- The test suite covers all 6502 instructions and addressing modes
- Success is indicated by PC looping at address `0x336d` (success macro location)
- Failure traps loop at various addresses (all marked as "failed anyway" in the listing)
