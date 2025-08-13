#include "tas5805m-math.h"

float tas5805m_q9_23_to_float(uint32_t raw)
{
    uint32_t val = ((raw & 0xFF) << 24) |
                   ((raw & 0xFF00) << 8) |
                   ((raw & 0xFF0000) >> 8) |
                   ((raw >> 24) & 0xFF);

    int32_t signed_val = (int32_t)val;
    return (float)signed_val / (float)(1 << 23);
}

uint32_t tas5805m_float_to_q9_23(float value)
{
    if (value > 255.999999) value = 255.999999;
    if (value < -256.0) value = -256.0;

    int32_t fixed_val = (int32_t)(value * (1 << 23));
    uint32_t le_val = ((fixed_val >> 24) & 0xFF) |
                      ((fixed_val >> 8) & 0xFF00) |
                      ((fixed_val << 8) & 0xFF0000) |
                      ((fixed_val << 24) & 0xFF000000);

    return le_val;
}