/**
 * @file time_utils.h
 * @brief Kernel time helpers.
 *
 * Centralizes timestamping so that all modules use the same monotonic clock
 * (Zephyr kernel uptime) for timeouts, sensor stamps, and command age checks.
 */

#ifndef ROVER_CONTROLLER_COMMON_TIME_UTILS_H
#define ROVER_CONTROLLER_COMMON_TIME_UTILS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the current kernel uptime in milliseconds.
 * @return Monotonic timestamp in milliseconds.
 */
int64_t time_now_ms(void);

/**
 * @brief Check whether @p timestamp_ms is older than @p timeout_ms.
 *
 * @param timestamp_ms The timestamp to test.
 * @param timeout_ms The timeout interval.
 * @return True if (now - timestamp_ms) >= timeout_ms, or if timestamp_ms is
 *         zero/invalid. False otherwise.
 */
bool time_is_expired_ms(int64_t timestamp_ms, int32_t timeout_ms);

/**
 * @brief Compute elapsed milliseconds since @p timestamp_ms.
 *
 * @param timestamp_ms The reference timestamp.
 * @return Elapsed milliseconds, clamped to zero if timestamp is in the future.
 */
int64_t time_elapsed_ms(int64_t timestamp_ms);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_COMMON_TIME_UTILS_H */
