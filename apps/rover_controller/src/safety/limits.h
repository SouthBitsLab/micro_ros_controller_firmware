/**
 * @file limits.h
 * @brief Safety thresholds and timing constants.
 *
 * Values are defaults; in the future they may be overridden by ROS parameters
 * or board Kconfig. They are isolated here so the safety monitor does not
 * depend on application logic.
 */

#ifndef ROVER_CONTROLLER_SAFETY_LIMITS_H
#define ROVER_CONTROLLER_SAFETY_LIMITS_H

#include "common/robot_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum age of a command before it is considered stale (ms). */
#define SAFETY_CMD_TIMEOUT_MS ROVER_CMD_TIMEOUT_MS

/** @brief Safety monitor thread period (ms). */
#define SAFETY_MONITOR_PERIOD_MS 20

/** @brief Undervoltage threshold below which motion is limited (V). */
#define SAFETY_UNDERVOLTAGE_V ROVER_UNDERVOLTAGE_THRESHOLD_V

/** @brief Per-motor overcurrent threshold (A). */
#define SAFETY_OVERCURRENT_A ROVER_OVERCURRENT_THRESHOLD_A

/** @brief Number of consecutive missed motor feedback samples before fault. */
#define SAFETY_MOTOR_FEEDBACK_MISS_COUNT 5

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_SAFETY_LIMITS_H */
