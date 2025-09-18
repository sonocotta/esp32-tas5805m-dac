#include "tas5805m-math.h"
#include <math.h>
#include "esp_log.h"

static const char *TAG = "MATH";

#define TAG "TAS5805M"

// Utility: swap endian for 32-bit values
uint32_t tas5805m_swap_endian_32(uint32_t val)
{
    return ((val & 0xFF) << 24) |
           ((val & 0xFF00) << 8) |
           ((val & 0xFF0000) >> 8) |
           ((val >> 24) & 0xFF);
}

/* -------------------------
   Fixed-point dB helpers
   ------------------------- */

// Convert dB (in tenths) to linear float
// Example: 0 -> 1.0, -60 -> 0.501, +60 -> 2.0
float tas5805m_db10_to_float(int32_t db10)
{
    float db = (float)db10 / 10.0f;
    float result = powf(10.0f, db / 20.0f);
    ESP_LOGD(TAG, "%s: db10=%d (%.1f dB) -> result=%f", __func__, db10, db, result);
    return result;
}

// Convert linear float to dB in tenths
int32_t tas5805m_float_to_db10(float linear)
{
    float db = 20.0f * log10f(linear);
    int32_t db10 = (int32_t)lroundf(db * 10.0f);
    ESP_LOGD(TAG, "%s: linear=%f -> db10=%d (%.1f dB)", __func__, linear, db10, (float)db10/10.0f);
    return db10;
}

/* -------------------------
   Q9.23 conversions
   ------------------------- */

float tas5805m_q9_23_to_float(uint32_t raw)
{
    uint32_t val = tas5805m_swap_endian_32(raw);
    int32_t signed_val = (int32_t)val;
    float result = (float)signed_val / 8388608.0f; // 2^23
    ESP_LOGD(TAG, "%s: raw=0x%08X, signed_val=%d -> result=%f",
             __func__, raw, signed_val, result);
    return result;
}

uint32_t tas5805m_float_to_q9_23(float value)
{
    if (value > 255.999999f) value = 255.999999f;
    if (value < -256.0f)     value = -256.0f;

    int32_t fixed_val = (int32_t)(value * (1 << 23));
    uint32_t le_val = tas5805m_swap_endian_32((uint32_t)fixed_val);

    ESP_LOGD(TAG, "%s: value=%f -> fixed_val=%d, le_val=0x%08X",
             __func__, value, fixed_val, le_val);

    return le_val;
}

// Convert raw Q9.23 register to dB in tenths
// int32_t tas5805m_q9_23_to_db10(uint32_t raw)
// {
//     float linear = tas5805m_q9_23_to_float(raw);
//     return tas5805m_float_to_db10(linear);
// }

/* -------------------------
   Q2.30 conversions
   ------------------------- */

float tas5805m_q2_30_to_float(uint32_t raw)
{
    uint32_t val = tas5805m_swap_endian_32(raw);
    int32_t signed_val = (int32_t)val;
    float result = (float)signed_val / 1073741824.0f; // 2^30
    ESP_LOGD(TAG, "%s: raw=0x%08X, signed_val=%d -> result=%f",
             __func__, raw, signed_val, result);
    return result;
}

uint32_t tas5805m_float_to_q2_30(float value)
{
    if (value > 3.999999f) value = 3.999999f;
    if (value < -4.0f)     value = -4.0f;

    int32_t fixed_val = (int32_t)(value * (1 << 30));
    uint32_t le_val = tas5805m_swap_endian_32((uint32_t)fixed_val);

    ESP_LOGD(TAG, "%s: value=%f -> fixed_val=%d, le_val=0x%08X",
             __func__, value, fixed_val, le_val);

    return le_val;
}

// Convert raw Q2.30 register to dB in tenths
// int32_t tas5805m_q2_30_to_db10(uint32_t raw)
// {
//     float linear = tas5805m_q2_30_to_float(raw);
//     return tas5805m_float_to_db10(linear);
// }

float tas5805m_q1_31_to_float(uint32_t raw)
{
    uint32_t val = tas5805m_swap_endian_32(raw);
    int32_t signed_val = (int32_t)val;
    float result = (float)signed_val / 2147483648.0f; // 2^31
    ESP_LOGD(TAG, "%s: raw=0x%08X, signed_val=%d -> result=%f",
             __func__, raw, signed_val, result);
    return result;
}