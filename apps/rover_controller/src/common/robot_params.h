/**
 * @file robot_params.h
 * @brief Robot geometry, limits, and runtime-tunable constants.
 *
 * This file contains constants that describe the physical robot configuration.
 * Default values are defined here; in the future these may be overridden via
 * ROS parameters or board-specific Kconfig options.
 */

#ifndef ROVER_CONTROLLER_COMMON_ROBOT_PARAMS_H
#define ROVER_CONTROLLER_COMMON_ROBOT_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/robot_types.h"

/** @brief Wheel radius in meters. */
#define ROVER_WHEEL_RADIUS_M 0.05f

/** @brief Track width (distance between left and right wheels) in meters. */
#define ROVER_TRACK_WIDTH_M 0.30f

/** @brief Wheelbase (distance between front and rear axles) in meters. */
#define ROVER_WHEELBASE_M 0.30f

/**
 * @brief Per-motor direction correction factors.
 *
 * Each entry is either +1.0f or -1.0f and is applied to the commanded wheel
 * velocity to account for physical wiring orientation. The values below must
 * be verified against the final chassis layout.
 */
#define ROVER_MOTOR_DIRECTION_SIGNS { \
    [ROVER_MOTOR_FRONT_LEFT]  =  1.0f, \
    [ROVER_MOTOR_FRONT_RIGHT] = -1.0f, \
    [ROVER_MOTOR_REAR_LEFT]   =  1.0f, \
    [ROVER_MOTOR_REAR_RIGHT]  = -1.0f, \
}

/** @brief Maximum allowed linear velocity (m/s). */
#define ROVER_MAX_LINEAR_VELOCITY_M_S 1.0f

/** @brief Maximum allowed angular velocity (rad/s). */
#define ROVER_MAX_ANGULAR_VELOCITY_RAD_S 2.0f

/** @brief Maximum allowed wheel angular velocity (rad/s). */
#define ROVER_MAX_WHEEL_VELOCITY_RAD_S 20.0f

/** @brief Command timeout after which safety monitor zeros velocity (ms). */
#define ROVER_CMD_TIMEOUT_MS 500

/** @brief Undervoltage threshold below which motion is limited (V). */
#define ROVER_UNDERVOLTAGE_THRESHOLD_V 10.5f

/** @brief Overcurrent threshold per motor (A). */
#define ROVER_OVERCURRENT_THRESHOLD_A 5.0f

/* Heartbeat publication period (ms). */
#define HEARTBEAT_PERIOD_MS 1000

/* Odometry/wheel states publication period (ms). */
#define TELEMETRY_ODOM_PERIOD_MS 20

/* Diagnostics publication period (ms). */
#define TELEMETRY_DIAG_PERIOD_MS 1000

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_COMMON_ROBOT_PARAMS_H */
