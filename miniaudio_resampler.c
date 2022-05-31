#include <retrovert/log.h>
#include <retrovert/resample.h>
#include <retrovert/settings.h>

#define PLUGIN_NAME "miniaudio resampler"
#define MA_NO_WASAPI
#define MA_NO_DSOUND
#define MA_NO_WINMM
#define MA_NO_ALSA
#define MA_NO_PULSEAUDIO
#define MA_NO_JACK
#define MA_NO_COREAUDIO
#define MA_NO_SNDIO
#define MA_NO_AUDIO4
#define MA_NO_OSS
#define MA_NO_AAUDIO
#define MA_NO_OPENSL
#define MA_NO_WEBAUDIO
#define MA_NO_NULL
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_DEVICE_IO
#define MA_NO_GENERATION
#define MA_API static
#define MINIAUDIO_IMPLEMENTATION
#include "external/miniaudio.h"

const RVLog* g_rv_log = NULL;
static int s_format_convert[10];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct MiniaudioResampler {
    RVConvertConfig converter_cfg;
    ma_data_converter_config cfg;
    ma_data_converter converter;
    bool is_init;

} MiniaudioResampler;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* miniaudio_resampler_create(const RVService* services) {
    MiniaudioResampler* data = calloc(1, sizeof(MiniaudioResampler));

    data->cfg = ma_data_converter_config_init_default();
    data->cfg.allowDynamicSampleRate = MA_TRUE;

    if (!data) {
        rvfl_error("Unable to allocate memory");
        return NULL;
    }

    return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int miniaudio_resampler_destroy(void* user_data) {
    MiniaudioResampler* data = (MiniaudioResampler*)user_data;
    if (data->is_init) {
        ma_data_converter_uninit(&data->converter, NULL);
    }

    free(data);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void miniaudio_resampler_set_config(void* user_data, const RVConvertConfig* format) {
    MiniaudioResampler* data = (MiniaudioResampler*)user_data;
    // check if the format is the same then we don't need to set anything up
    if (memcmp(&data->cfg, format, sizeof(RVConvertConfig)) == 0 && data->is_init) {
        return;
    }

    ma_result res;

    if (data->is_init) {
        ma_data_converter_uninit(&data->converter, NULL);
    }

    data->cfg.formatIn = s_format_convert[format->input.audio_format];
    data->cfg.channelsIn = format->input.channel_count;
    data->cfg.sampleRateIn = format->input.sample_rate;
    data->cfg.formatOut = s_format_convert[format->output.audio_format];
    data->cfg.channelsOut = format->output.channel_count;
    data->cfg.sampleRateOut = format->output.sample_rate;

    if ((res = ma_data_converter_init(&data->cfg, NULL, &data->converter)) != MA_SUCCESS) {
        rv_fatal("Unable to init Miniaudio data converter. error: %d", res);
        return;
    }

    data->is_init = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t miniaudio_resampler_convert(void* user_data, void* output_data, void* input_data, uint32_t input_frame_count) {
    MiniaudioResampler* data = (MiniaudioResampler*)user_data;
    ma_result res = 0;
    ma_uint64 frame_count_in = input_frame_count;
    ma_uint64 frame_count_out = 0;

    if ((res = ma_data_converter_process_pcm_frames(&data->converter, input_data, &frame_count_in, output_data,
                                                    &frame_count_out)) != MA_SUCCESS) {
        rv_fatal("Miniaudio resampler: Unable to convert data error: %d", res);
        return 0;
    }

    return (uint32_t)frame_count_out;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t miniaudio_resampler_get_expected_output_frame_count(void* user_data, uint32_t frame_count) {
    MiniaudioResampler* data = (MiniaudioResampler*)user_data;
    ma_result res = 0;
    ma_uint64 output_count = 0;

    if ((res = ma_data_converter_get_expected_output_frame_count(&data->converter, frame_count, &output_count)) !=
        MA_SUCCESS) {
        rv_fatal("Miniaudio resampler: Unable to get_exepected_output_frame_count error: %d", res);
        return 0;
    }

    return (uint32_t)output_count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void miniaudio_resampler_static_init(const RVService* service_api) {
    g_rv_log = RVService_get_log(service_api, RV_LOG_API_VERSION);

    // format remapping that only needs to be done once.
    s_format_convert[RVAudioStreamFormat_U8] = ma_format_u8;
    s_format_convert[RVAudioStreamFormat_S16] = ma_format_s16;
    s_format_convert[RVAudioStreamFormat_S24] = ma_format_s24;
    s_format_convert[RVAudioStreamFormat_S32] = ma_format_s32;
    s_format_convert[RVAudioStreamFormat_F32] = ma_format_f32;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static RVResamplePlugin s_miniaudio_resample_plugin = {
    RV_RESAMPLE_PLUGIN_API_VERSION,
    PLUGIN_NAME,
    "0.0.1",
    "0.11.9",
    miniaudio_resampler_create,
    miniaudio_resampler_destroy,
    miniaudio_resampler_set_config,
    miniaudio_resampler_convert,
    miniaudio_resampler_get_expected_output_frame_count,
    miniaudio_resampler_static_init,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern RV_EXPORT RVResamplePlugin* rv_resample_plugin() {
    return &s_miniaudio_resample_plugin;
}
