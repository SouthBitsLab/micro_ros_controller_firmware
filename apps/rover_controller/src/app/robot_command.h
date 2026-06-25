/**
 * @file robot_command.h
 * @brief Thread-safe command target storage.
 *
 * The ROS subscriber writes the high-level velocity target into this object;
 * the safety monitor and control loop read it. Access is protected by a mutex.
 */

#ifndef ROVER_CONTROLLER_APP_ROBOT_COMMAND_H
#define ROVER_CONTROLLER_APP_ROBOT_COMMAND_H

#include "common/robot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the command target storage.
 * @return 0 on success, negative errno on failure.
 */
int robot_command_init(void);

/**
 * @brief Atomically update the command target.
 *
 * Validates the incoming command against safety limits before storing.
 *
 * @param cmd New command value.
 * @return 0 on success, negative errno on failure.
 */
int robot_command_set(const struct robot_command *cmd);

/**
 * @brief Atomically read the current command target.
 *
 * @param out Pointer to command structure to fill.
 * @return 0 on success, negative errno on failure.
 */
int robot_command_get(struct robot_command *out);

/**
 * @brief Zero the command target (used by safety / fault recovery).
 */
void robot_command_zero(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_APP_ROBOT_COMMAND_H */
