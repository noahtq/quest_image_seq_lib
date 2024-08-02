//
// Created by Noah Turnquist on 7/29/24.
//

#include <fstream>
#include <benchmark/benchmark.h>
#include "../quest_seq_lib.h"

// Image sequences used in benchmarks
// 1. A 33 second long, 30 fps, 1280x720 image sequence of some waves
const std::filesystem::path wave_path =
    "../../media/test_media/videos/image_sequences/waves_001/waves_001_%04d.png";
const std::filesystem::path wave_output_path =
    "../../media/test_media/videos/image_sequences/waves_001_render/waves_render_001_%04d.png";
Quest::ImageSeq wave_seq;

// 2. A 187 frame long, 1080x1920 image sequence of a dog

static void DoSetup(const benchmark::State& state) {
    wave_seq.open(wave_path);
}

static void DoTeardown(const benchmark::State& state) {
    Quest::SeqPath teardown_output_seq(wave_output_path);
    for (int i = 1; i < 991; i++) {
        if (std::ifstream(teardown_output_seq.outputPath())) {
            std::filesystem::remove(teardown_output_seq.outputPath());
        }
        teardown_output_seq.increment();
    }
}

// Benchmark opening a 33 second long image sequence that is 1280x720
static void BM_OpeningLongHDImageSequence(benchmark::State& state) {
    Quest::ImageSeq seq;
    for (auto _ : state) {
        seq.open(wave_path);
    }
}
BENCHMARK(BM_OpeningLongHDImageSequence)->Threads(4)->Unit(benchmark::kSecond);

// Benchmark rendering a 33 second long image sequence that is 1280x720
static void BM_WritingLongHDImageSequence(benchmark::State& state) {
    for (auto _ : state) {
        wave_seq.render(wave_output_path);
    }
}
BENCHMARK(BM_WritingLongHDImageSequence)->Threads(4)->Setup(DoSetup)->Teardown(DoTeardown)->Unit(benchmark::kSecond);

BENCHMARK_MAIN();