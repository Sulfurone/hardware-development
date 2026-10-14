#include "measurement.h"
#include "ads1256.h"

#define MEASUREMENT_WINDOW_SAMPLES  3000UL /* 100 ms at 30 kSPS. */
#define ADC_POSITIVE_FULL_SCALE     8388607UL
#define CLIP_LEVEL                  8300000L

static uint32_t s_sample_rate;
static uint8_t s_pga;
static uint32_t s_scale_q16 = 65536UL;
static int32_t s_hysteresis_codes = 4000L;
static uint32_t s_count;
static int64_t s_sum;
static uint64_t s_sum_sq;
static int32_t s_low_pass;
static int32_t s_carrier_baseline;
static int8_t s_low_state;
static int8_t s_carrier_state;
static uint32_t s_low_first, s_low_last, s_low_edges;
static uint32_t s_car_first, s_car_last, s_car_edges;
static uint8_t s_clipped;
static MeasurementResult s_result;

static uint32_t isqrt64(uint64_t value)
{
    uint64_t bit = (uint64_t)1U << 62;
    uint64_t root = 0U;
    while (bit > value) {
        bit >>= 2;
    }
    while (bit != 0U) {
        if (value >= root + bit) {
            value -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root >>= 1;
        }
        bit >>= 2;
    }
    return (uint32_t)root;
}

static void track_edge(int32_t value, int8_t *state, uint32_t index,
                       uint32_t *first, uint32_t *last, uint32_t *edges)
{
    if (value > s_hysteresis_codes) {
        if (*state < 0) {
            if (*edges == 0U) {
                *first = index;
            }
            *last = index;
            (*edges)++;
        }
        *state = 1;
    } else if (value < -s_hysteresis_codes) {
        *state = -1;
    }
}

static uint32_t edges_to_millihertz(uint32_t edges, uint32_t first, uint32_t last)
{
    uint32_t periods;
    uint32_t samples;
    if (edges < 2U) {
        return 0U;
    }
    periods = edges - 1U;
    samples = last - first;
    if (samples == 0U) {
        return 0U;
    }
    return (uint32_t)(((uint64_t)periods * s_sample_rate * 1000U) / samples);
}

void Measurement_Init(uint32_t sample_rate, uint8_t pga)
{
    s_sample_rate = sample_rate;
    s_pga = pga;
    s_count = 0U;
    s_sum = 0;
    s_sum_sq = 0U;
    s_low_pass = 0;
    s_carrier_baseline = 0;
    s_low_state = 0;
    s_carrier_state = 0;
    s_low_edges = 0U;
    s_car_edges = 0U;
    s_clipped = 0U;
    s_result.valid = 0U;
}

void Measurement_SetScaleQ16(uint32_t scale_q16)
{
    if (scale_q16 != 0U) {
        s_scale_q16 = scale_q16;
    }
}

void Measurement_SetHysteresisUv(uint32_t hysteresis_uV)
{
    uint64_t codes = (uint64_t)hysteresis_uV * s_pga * ADC_POSITIVE_FULL_SCALE;
    codes /= (2U * ADS1256_REFERENCE_UV);
    if (codes < 16U) {
        codes = 16U;
    }
    if (codes > 1000000U) {
        codes = 1000000U;
    }
    s_hysteresis_codes = (int32_t)codes;
}

uint8_t Measurement_Push(int32_t sample)
{
    int32_t high_pass;
    int64_t mean;
    uint64_t mean_square;
    uint64_t variance;
    uint32_t rms_codes;
    uint64_t rms_uV;

    s_sum += sample;
    s_sum_sq += (uint64_t)((int64_t)sample * sample);
    if ((sample >= CLIP_LEVEL) || (sample <= -CLIP_LEVEL)) {
        s_clipped = 1U;
    }

    /* At 30 kSPS: low-pass corner ~150 Hz; carrier high-pass corner ~300 Hz. */
    s_low_pass += (sample - s_low_pass) >> 5;
    s_carrier_baseline += (sample - s_carrier_baseline) >> 4;
    high_pass = sample - s_carrier_baseline;

    track_edge(s_low_pass, &s_low_state, s_count, &s_low_first, &s_low_last, &s_low_edges);
    track_edge(high_pass, &s_carrier_state, s_count, &s_car_first, &s_car_last, &s_car_edges);
    s_count++;

    if (s_count < MEASUREMENT_WINDOW_SAMPLES) {
        return 0U;
    }

    mean = s_sum / (int64_t)s_count;
    mean_square = s_sum_sq / s_count;
    variance = (mean_square > (uint64_t)(mean * mean)) ?
               mean_square - (uint64_t)(mean * mean) : 0U;
    rms_codes = isqrt64(variance);
    rms_uV = (uint64_t)rms_codes * (2U * ADS1256_REFERENCE_UV);
    rms_uV /= ((uint64_t)s_pga * ADC_POSITIVE_FULL_SCALE);
    rms_uV = (rms_uV * s_scale_q16) >> 16;

    s_result.rms_uV = (rms_uV > 0xFFFFFFFFULL) ? 0xFFFFFFFFUL : (uint32_t)rms_uV;
    s_result.low_frequency_mHz = edges_to_millihertz(s_low_edges, s_low_first, s_low_last);
    s_result.carrier_frequency_mHz = edges_to_millihertz(s_car_edges, s_car_first, s_car_last);
    s_result.clipped = s_clipped;
    s_result.valid = 1U;

    s_count = 0U;
    s_sum = 0;
    s_sum_sq = 0U;
    s_low_edges = 0U;
    s_car_edges = 0U;
    s_clipped = 0U;
    return 1U;
}

const MeasurementResult *Measurement_Get(void)
{
    return &s_result;
}
