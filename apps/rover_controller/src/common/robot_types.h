/**
 * @file robot_types.h
 * @brief Common data structures shared across all firmware layers.
 *
 * This header contains robot-level data types that are intentionally independent
 * of any hardware driver or ROS message representation. Keeping these types in
 * one place guarantees that the control layer, safety layer, HAL, and ROS layer
 * all speak the same language without leaking driver-specific details upward.
 */

#ifndef ROVER_CONTROLLER_COMMON_ROBOT_TYPES_H
#define ROVER_CONTROLLER_COMMON_ROBOT_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Number of drive motors on the four-wheel mobile base. */
#define ROVER_NUM_MOTORS 4U

/** @brief Motor identifiers for the four wheel hubs. */
enum rover_motor_id {
    ROVER_MOTOR_FRONT_LEFT = 0,
    ROVER_MOTOR_FRONT_RIGHT,
    ROVER_MOTOR_REAR_LEFT,
    ROVER_MOTOR_REAR_RIGHT,
};

/**
 * @brief High-level velocity command received from the host.
 *
 * Expressed in the robot base frame. Positive linear velocity is forward;
 * positive angular velocity is counter-clockwise when viewed from above.
 */
struct robot_command {
    /** Linear velocity along the robot X axis (m/s). */
    float linear_x;

    /** Angular velocity about the robot Z axis (rad/s). */
    float angular_z;

    /** Timestamp when the command was last updated (kernel uptime, ms). */
    int64_t timestamp_ms;

    /** True if a command has been received since boot/reset. */
    bool valid;
};

/**
 * @brief Per-motor feedback returned by the motor drive adapter.
 *
 * This structure is populated by the HAL motor drive implementation and is
 * consumed by odometry, diagnostics, and safety modules.
 */
struct motor_feedback {
    /** Motor electrical angle or wheel position (rad). */
    float position_rad;

    /** Motor/wheel angular velocity (rad/s). */
    float velocity_rad_s;

    /** Motor current (A), if available. */
    float current_a;

    /** Bitmask of adapter-specific fault flags. */
    uint32_t fault_flags;

    /** True if the feedback value is fresh since the last read. */
    bool valid;
};

/**
 * @brief Per-wheel state used for odometry and /wheel_states publishing.
 */
struct wheel_state {
    /** Angular position of each wheel (rad). */
    float position_rad[ROVER_NUM_MOTORS];

    /** Angular velocity of each wheel (rad/s). */
    float velocity_rad_s[ROVER_NUM_MOTORS];

    /** Timestamp in kernel uptime milliseconds. */
    int64_t timestamp_ms;

    /** True when the state has been updated. */
    bool valid;
};

/**
 * @brief 2-D odometry pose estimate.
 *
 * Maintained by the odometry module from wheel feedback.
 */
struct robot_pose {
    /** X position in the odometry frame (m). */
    float x;

    /** Y position in the odometry frame (m). */
    float y;

    /** Heading angle in the odometry frame (rad). */
    float theta;
};

/**
 * @brief Robot velocity estimate in the odometry frame.
 */
struct robot_twist {
    /** Linear velocity along the robot X axis (m/s). */
    float linear_x;

    /** Angular velocity about the robot Z axis (rad/s). */
    float angular_z;
};

/**
 * @brief Compact odometry snapshot published by the control loop.
 */
struct odometry_state {
    struct robot_pose pose;
    struct robot_twist twist;
    int64_t timestamp_ms;
    bool valid;
};

/**
 * @brief Diagnostic health flags reported in /diagnostics.
 */
struct diagnostic_flags {
    bool imu_ok;
    bool gnss_ok;
    bool battery_ok;
    bool motors_ok;
    bool agent_connected;
    bool estop_active;
    bool cmd_timeout;
    bool undervoltage;
    bool overcurrent;
};

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_COMMON_ROBOT_TYPES_H */
