#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <string>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

extern "C" {
    #include <kiss_fft.h>
    #include <kiss_fftr.h>
}

int main(int argc, char* argv[]) {
    const int N = 1024;

    std::vector<float> input(N, 0.0f);

    float sampleRate = 44100.0f;
    float frequency = 440.0f;

    if (argc > 1) {
        std::string filename = argv[1];

        unsigned int channels = 0;
        unsigned int wavSampleRate = 0;
        drwav_uint64 totalFrameCount = 0;

        float* samples = drwav_open_file_and_read_pcm_frames_f32(
            filename.c_str(),
            &channels,
            &wavSampleRate,
            &totalFrameCount,
            nullptr
        );

        if (!samples) {
            std::cerr << "Failed to open WAV file: " << filename << "\n";
            return 1;
        }

        sampleRate = static_cast<float>(wavSampleRate);

        //Picks the middle of the wav file for our snapshot
        drwav_uint64 startFrame = totalFrameCount / 2;

        if (startFrame + N > totalFrameCount) {
            startFrame = totalFrameCount > N ? totalFrameCount - N : 0;
        }

        for (int i = 0; i < N; ++i) {
            //Reads through left channel
            input[i] = samples[(startFrame + i) * channels]; 
        }

        drwav_free(samples, nullptr);

        std::cout << "Loaded WAV file: " << filename << "\n";
        std::cout << "Sample rate: " << sampleRate << " Hz\n";
        std::cout << "Analyzing middle slice\n\n";

    } else {
        // Fall back case where we use a genereated sine wave
        for (int i = 0; i < N; ++i) {
            input[i] = std::sin(2.0f * static_cast<float>(M_PI) * frequency * i / sampleRate);
        }

        std::cout << "Using generated 440 Hz sine wave.\n\n";
    }

    kiss_fftr_cfg cfg = kiss_fftr_alloc(N, 0, nullptr, nullptr);
    if (!cfg) {
        std::cerr << "Failed to allocate FFT configuration.\n";
        return 1;
    }

    std::vector<kiss_fft_cpx> output(N / 2 + 1);
    kiss_fftr(cfg, input.data(), output.data());


    //Printing to standard output
    for (int i = 0; i < N / 2; i += 8) {
        float magnitude = std::sqrt(output[i].r * output[i].r +
                                    output[i].i * output[i].i);

        float freq = i * sampleRate / N;
        float scaled = std::log10(1.0f + magnitude);

        int barLength = static_cast<int>(scaled * 8);
        barLength = std::clamp(barLength, 0, 40);

        std::cout << std::setw(8) << std::fixed << std::setprecision(1)
                  << freq << " Hz | ";

        for (int j = 0; j < barLength; ++j) {
            std::cout << "#";
        }

        std::cout << '\n';
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.get();

    free(cfg);
    return 0;
}