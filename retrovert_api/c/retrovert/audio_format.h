///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file is auto-generated by api_generator. DO NOT EDIT!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "rv_types.h"

#ifdef _cplusplus
extern "C" {
#endif

typedef enum RVAudioStreamFormat {
    RVAudioStreamFormat_U8 = 1,
    RVAudioStreamFormat_S16 = 2,
    RVAudioStreamFormat_S24 = 3,
    RVAudioStreamFormat_S32 = 4,
    RVAudioStreamFormat_F32 = 5,
} RVAudioStreamFormat;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is usually used as output type for a soundcard, wavfile, etc, but is also used as input/output
// for data converting between different formats
typedef struct RVAudioFormat {
    RVAudioStreamFormat audio_format;
    uint32_t channel_count;
    uint32_t sample_rate;
} RVAudioFormat;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _cplusplus
}
#endif