# grep_cpp

Implementation of the `grep` command in C++.

Prints every line in the given files that contains `PATTERN`. Matches are
highlighted in red when output goes to a terminal, and printed plain when piped
or redirected.

## Requirements

- CMake 3.20 or newer
- A C++20 compiler (GCC or Clang)
- Internet access on the first configure (GoogleTest and Google Benchmark are
  downloaded automatically)

## Building

```bash
cmake -S . -B build
cmake --build build
```

This builds a Debug binary. For an optimized build, which you want before
benchmarking:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To skip the tests or benchmarks, configure with `-DGREP_BUILD_TESTS=OFF` or
`-DGREP_BUILD_BENCHMARKS=OFF`.

## Usage

```bash
./build/grep_cpp PATTERN FILE...
```

Example:

```console
$ ./build/grep_cpp hello test.txt
hello world
hello
say hello to everyone
othello is a play
helloworld
the word hello appears twice: hello
    indented hello with spaces
```

The search is case-sensitive and matches plain text, not regular expressions.

## Running the tests

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

Or run the test binary directly to see each test:

```bash
./build/grep_tests
./build/grep_tests --gtest_filter='BasicGrep.*'   # only the basicGrep tests
./build/grep_tests --gtest_list_tests             # list tests without running them
```

New test files go in `tests/` and must be added to `add_executable(grep_tests ...)`
in `CMakeLists.txt`.

## Running the benchmarks

```bash
./build/grep_bench
```

Use a Release build for meaningful numbers.

## Project layout

```text
include/grep_cpp/   public headers
src/                implementation and main.cpp
tests/              GoogleTest unit tests
benchmarks/         Google Benchmark benchmarks
```
