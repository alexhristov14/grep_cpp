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

## Search algorithms

All three return the same lines. A line with several matches is returned once.

| Function | How it searches | Time | 1M-line benchmark |
|---|---|---|---|
| `basicGrep` | Splits the buffer into lines, then runs `std::string_view::find` on each line | O(n · m) worst case | 19.2 ms (1.8 GiB/s) |
| `onePassGrep` | Runs `std::string::find` over the whole buffer, then expands each match to its line | O(n · m) worst case | 17.7 ms (1.9 GiB/s) |
| `boyerMooreGrep` | Boyer-Moore-Horspool with a `memchr` prefilter on the pattern's rarest byte (sampled from the first 4 KiB of the buffer) | Sublinear on average, O(n · m) worst case | 9.9 ms (3.5 GiB/s) |

The numbers come from `./build/grep_bench` on a Release build. The pattern is
`needle`, and one line in ten contains it. They will vary by machine.

`std::string::find` uses `memchr` on the pattern's first byte. Its speed
therefore depends on how common that byte is in the text. `boyerMooreGrep`
searches for the rarest byte instead. It is fastest when the pattern starts
with a common letter, and about equal to `onePassGrep` when the first byte is
already rare.

## Project layout

```text
include/grep_cpp/   public headers
src/                implementation and main.cpp
tests/              GoogleTest unit tests
benchmarks/         Google Benchmark benchmarks
```
