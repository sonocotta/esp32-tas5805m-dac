#include "tas5805m-math.h"
#include <math.h>
#include "esp_log.h"

static const char *TAG = "MATH";

uint32_t tas5805m_swap_endian_32(uint32_t val)
{
    return ((val & 0xFF) << 24) |
           ((val & 0xFF00) << 8) |
           ((val & 0xFF0000) >> 8) |
           ((val >> 24) & 0xFF);
}

// Convert dB to linear gain (double)
// Example: 0 dB -> 1.0, -6.0206 dB -> 0.5, +6.0206 dB -> 2.0
double tas5805m_db_to_double(double db)
{
    double result = powf(10.0, db / 20.0);
    ESP_LOGD(TAG, "%s: db=%f -> result=%f", __func__, db, result);
    return result;  
}

// Convert linear gain (double) to dB
// Example: 1.0 -> 0 dB, 0.5 -> -6.0206 dB, 2.0 -> +6.0206 dB
double tas5805m_double_to_db(double linear)
{
    double result = 20.0 * log10(linear);
    ESP_LOGD(TAG, "%s: linear=%f -> result=%f", __func__, linear, result);
    return result;
}

double tas5805m_q9_23_to_double(uint32_t raw)
{
    uint32_t val = tas5805m_swap_endian_32(raw);
    int32_t signed_val = (int32_t)val;
    // Divide by 2^23 = 8388608
    double result = (double)signed_val / 8388608.0;
    ESP_LOGD(TAG, "%s: raw=0x%08X, val=0x%08X, signed_val=%d, result=%f", __func__, 
        raw, val, signed_val, result);
    return result;
}

uint32_t tas5805m_double_to_q9_23(double value)
{
    if (value > 255.999999) value = 255.999999;
    if (value < -256.0) value = -256.0;

    int32_t fixed_val = (int32_t)(value * (1 << 23));
    uint32_t le_val = tas5805m_swap_endian_32((uint32_t)fixed_val);

    ESP_LOGD(TAG, "%s: value=%f -> fixed_val=%08X, le_val=0x%08X", __func__, 
        value, fixed_val, le_val);

    return le_val;
}

double tas5805m_q9_23_to_db(uint32_t raw) 
{
    // Convert raw Q9.23 value to linear doubleing-point
    double linear = tas5805m_q9_23_to_double(raw);
    // Avoid log of zero
    // if (linear <= 0.0) {
    //     return -INFINITY;  // represent as negative infinity dB
    // }
    // Convert to dB
    return 20.0 * log10(linear);
}

double tas5805m_q2_30_to_double(uint32_t raw)
{
    uint32_t val = tas5805m_swap_endian_32(raw);
    int32_t signed_val = (int32_t)val;
    double result = (double)signed_val / (double)(1 << 30);
    ESP_LOGD(TAG, "%s: raw=0x%08X, val=0x%08X, signed_val=%d, result=%f", __func__, 
        raw, val, signed_val, result);
    return result;
}

uint32_t tas5805m_double_to_q2_30(double value)
{
    // Clamp to valid representable range
    if (value > 3.999999) value = 3.999999;
    if (value < -4.0)     value = -4.0;

    int32_t fixed_val = (int32_t)(value * (1 << 30));

    // Swap to little-endian for TAS registers
    uint32_t le_val = ((fixed_val >> 24) & 0xFF) |
                      ((fixed_val >> 8)  & 0xFF00) |
                      ((fixed_val << 8)  & 0xFF0000) |
                      ((fixed_val << 24) & 0xFF000000);

    return le_val;
}

double tas5805m_q2_30_to_db(uint32_t raw)
{
    double linear = tas5805m_q2_30_to_double(raw);
    // if (linear <= 0.0) {
    //     return -INFINITY;  // avoid log(0) or negative
    // }
    return 20.0 * log10(linear);
}