/**
 * @file kinematics.h
 * @brief Skid-steer kinematics interface.
 *
 * Converts between high-level robot velocity commands and per-wheel angular
 * velocities. This module is pure math and has no hardware dependencies.
 */

#ifndef ROVER_CONTROLLER_APP_KINEMATICS_H
#define ROVER_CONTROLLER_APP_KINEMATICS_H

#include "common/robot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute per-wheel angular velocities from a Twist command.
 *
 * Uses the standard skid-steer model:
 *   v_left  = v - omega * track_width / 2
 *   v_right = v + omega * track_width / 2
 *   wheel_angular_velocity = wheel_linear_velocity / wheel_radius
 *
 * The output array is indexed by @ref rover_motor_id. Direction signs from
 * @ref ROVER_MOTOR_DIRECTION_SIGNS are already applied.
 *
 * @param cmd Input velocity command.
 * @param wheel_velocities_rad_s Output array of ROVER_NUM_MOTORS floats.
 * @return 0 on success, negative errno on failure.
 */
int kinematics_compute_wheel_velocities(const struct robot_command *cmd,
                                        float wheel_velocities_rad_s[ROVER_NUM_MOTORS]);

/**
 * @brief Compute robot twist from per-wheel angular velocities.
 *
 * Inverse of the forward kinematics, used for odometry.
 *
 * @param wheel_velocities_rad_s Input array of ROVER_NUM_MOTORS floats.
 * @param twist Output twist.
 * @return 0 on success, negative errno on failure.
 */
int kinematics_compute_twist(const float wheel_velocities_rad_s[ROVER_NUM_MOTORS],
                             struct robot_twist *twist);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_APP_KINEMATICS_H */
