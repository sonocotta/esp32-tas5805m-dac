#ifndef TAS5805M_MATH_H
#define TAS5805M_MATH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

/**
 * @brief Swap the endianness of a 32-bit integer.
 *
 * @param val Input 32-bit integer.
 * @return 32-bit integer with swapped endianness.
 */
uint32_t tas5805m_swap_endian_32(uint32_t val);

/**
 * @brief Convert decibels (dB) to a linear gain (float).
 *
 * @param db Value in decibels (dB).
 * @return Linear gain as a float.
 */
float tas5805m_db_to_float(float db);

/**
 * @brief Convert a linear gain (float) to decibels (dB).
 *
 * @param linear Linear gain as a float.
 * @return Value in decibels (dB).
 */
float tas5805m_float_to_db(float linear);

/**
 * @brief Convert a Q9.23 fixed-point value to a float.
 *
 * @param raw Q9.23 value as a 32-bit unsigned integer.
 * @return float-precision floating point representation.
 */
float tas5805m_q9_23_to_float(uint32_t raw);

/**
 * @brief Convert a float to a Q9.23 fixed-point value.
 *
 * @param value float-precision floating point input.
 * @return Q9.23 fixed-point value as a 32-bit unsigned integer.
 */
uint32_t tas5805m_float_to_q9_23(float value);

/**
 * @brief Convert a Q2.30 fixed-point value to a float.
 *
 * @param raw Q2.30 value as a 32-bit unsigned integer.
 * @return float-precision floating point representation.
 */
float tas5805m_q2_30_to_float(uint32_t raw);

/**
 * @brief Convert a float to a Q2.30 fixed-point value.
 *
 * @param value float-precision floating point input.
 * @return Q2.30 fixed-point value as a 32-bit unsigned integer.
 */
uint32_t tas5805m_float_to_q2_30(float value);

#ifdef __cplusplus
}
#endif

#endif // TAS5805M_MATH_H