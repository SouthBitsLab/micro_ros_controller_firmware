/**
 * @file odometry.h
 * @brief Wheel odometry integration interface.
 *
 * Maintains a 2-D pose estimate from wheel feedback. This module is pure math
 * and has no hardware dependencies.
 */

#ifndef ROVER_CONTROLLER_APP_ODOMETRY_H
#define ROVER_CONTROLLER_APP_ODOMETRY_H

#include "common/robot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize odometry state to zeros.
 */
void odometry_init(void);

/**
 * @brief Reset odometry to the given pose.
 *
 * @param x New X position (m).
 * @param y New Y position (m).
 * @param theta New heading (rad).
 */
void odometry_reset(float x, float y, float theta);

/**
 * @brief Update odometry from the latest wheel state.
 *
 * Uses the wheel angular velocities and positions to integrate pose forward.
 *
 * @param wheel_state Latest wheel state.
 * @return 0 on success, negative errno on failure.
 */
int odometry_update(const struct wheel_state *wheel_state);

/**
 * @brief Return the current odometry snapshot.
 *
 * @param out Output odometry structure.
 * @return 0 on success, negative errno on failure.
 */
int odometry_get(struct odometry_state *out);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_APP_ODOMETRY_H */
