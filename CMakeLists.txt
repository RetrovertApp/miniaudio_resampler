cmake_minimum_required(VERSION 3.10)
project(miniaudio_resampler VERSION 0.0.2 DESCRIPTION "miniaudio resampler library for Retrovert")

include_directories("retrovert_api/c")

add_library(miniaudio_resampler SHARED "miniaudio_resampler.c") 
