/**
 * @file control_loop.h
 * @brief Motor control loop thread interface.
 *
 * The control loop is the only thread that commands the motor drive HAL. It
 * reads the latest robot_command, checks safety fault flags, computes wheel
 * velocities through kinematics, sends them to the HAL, reads feedback, and
 * updates odometry.
 */

#ifndef ROVER_CONTROLLER_APP_CONTROL_LOOP_H
#define ROVER_CONTROLLER_APP_CONTROL_LOOP_H

#include "common/robot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the control loop resources (mutexes, semaphores).
 * @return 0 on success, negative errno on failure.
 */
int control_loop_init(void);

/**
 * @brief Start the control loop thread.
 * @return 0 on success, negative errno on failure.
 */
int control_loop_start(void);

/**
 * @brief Get the most recent wheel state computed by the control loop.
 *
 * @param out Output wheel state structure.
 * @return 0 on success, negative errno on failure.
 */
int control_loop_get_wheel_state(struct wheel_state *out);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_APP_CONTROL_LOOP_H */
