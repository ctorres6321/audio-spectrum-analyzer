cmake_minimum_required(VERSION 3.10)

project(AudioSpectrumAnalyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(audio_spectrum_analyzer
    src/main.cpp
    kissfft/kiss_fft.c
    kissfft/kiss_fftr.c
)

target_include_directories(audio_spectrum_analyzer PRIVATE
    ${PROJECT_SOURCE_DIR}/kissfft
    ${PROJECT_SOURCE_DIR}/external
)