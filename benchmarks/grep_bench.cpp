#include "grep_cpp/algorithms.hpp"

#include <benchmark/benchmark.h>

#include <string>

namespace {
// Builds a buffer of `lines` lines where every 10th line contains "needle".
std::string makeBuffer(long lines) {
  std::string buffer;
  for (long i = 0; i < lines; ++i) {
    buffer += (i % 10 == 0) ? "some text with a needle in it\n"
                            : "just an ordinary line of filler text\n";
  }
  return buffer;
}
} // namespace

static void BM_BasicGrep(benchmark::State &state) {
  const std::string buffer = makeBuffer(state.range(0));
  const std::string pattern = "needle";

  for (auto _ : state) {
    auto results = basicGrep(buffer, pattern);
    benchmark::DoNotOptimize(results);
  }
  state.SetBytesProcessed(state.iterations() *
                          static_cast<int64_t>(buffer.size()));
}
BENCHMARK(BM_BasicGrep)->RangeMultiplier(10)->Range(100, 1'000'000);
