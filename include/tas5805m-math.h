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
 * @brief Convert decibels (dB) to a linear gain (double).
 *
 * @param db Value in decibels (dB).
 * @return Linear gain as a double.
 */
double tas5805m_db_to_double(double db);

/**
 * @brief Convert a linear gain (double) to decibels (dB).
 *
 * @param linear Linear gain as a double.
 * @return Value in decibels (dB).
 */
double tas5805m_double_to_db(double linear);

/**
 * @brief Convert a Q9.23 fixed-point value to a double.
 *
 * @param raw Q9.23 value as a 32-bit unsigned integer.
 * @return Double-precision floating point representation.
 */
double tas5805m_q9_23_to_double(uint32_t raw);

/**
 * @brief Convert a double to a Q9.23 fixed-point value.
 *
 * @param value Double-precision floating point input.
 * @return Q9.23 fixed-point value as a 32-bit unsigned integer.
 */
uint32_t tas5805m_double_to_q9_23(double value);

/**
 * @brief Convert a Q9.23 fixed-point value to decibels (dB).
 *
 * @param raw Q9.23 value as a 32-bit unsigned integer.
 * @return Value in decibels (dB).
 */
double tas5805m_q9_23_to_db(uint32_t raw);

/**
 * @brief Convert a Q2.30 fixed-point value to a double.
 *
 * @param raw Q2.30 value as a 32-bit unsigned integer.
 * @return Double-precision floating point representation.
 */
double tas5805m_q2_30_to_double(uint32_t raw);

/**
 * @brief Convert a double to a Q2.30 fixed-point value.
 *
 * @param value Double-precision floating point input.
 * @return Q2.30 fixed-point value as a 32-bit unsigned integer.
 */
uint32_t tas5805m_double_to_q2_30(double value);

/**
 * @brief Convert a Q2.30 fixed-point value to decibels (dB).
 *
 * @param raw Q2.30 value as a 32-bit unsigned integer.
 * @return Value in decibels (dB).
 */
double tas5805m_q2_30_to_db(uint32_t raw);

#ifdef __cplusplus
}
#endif

#endif // TAS5805M_MATH_H