#ifndef TAS5805M_MATH_H
#define TAS5805M_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert a Q9.23 fixed-point value to a floating point value.
 *
 * @param raw Q9.23 value as a 32-bit signed integer.
 * @return Floating point representation.
 */
float tas5805m_q9_23_to_float(uint32_t raw);

/**
 * @brief Convert a floating point value to a Q9.23 fixed-point value.
 *
 * @param value Floating point input.
 * @return Q9.23 fixed-point value as a 32-bit signed integer.
 */
uint32_t tas5805m_float_to_q9_23(float value);

#ifdef __cplusplus
}
#endif

#endif // TAS5805M_MATH_H