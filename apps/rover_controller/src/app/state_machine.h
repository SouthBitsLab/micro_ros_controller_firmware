/**
 * @file state_machine.h
 * @brief Robot lifecycle state machine.
 *
 * Defines the high-level operational states and transitions. The micro-ROS
 * thread and safety monitor query the state to decide what telemetry to publish
 * and whether motion is permitted.
 */

#ifndef ROVER_CONTROLLER_APP_STATE_MACHINE_H
#define ROVER_CONTROLLER_APP_STATE_MACHINE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Robot lifecycle states. */
enum robot_state {
    ROBOT_STATE_BOOT = 0,
    ROBOT_STATE_WAIT_AGENT,
    ROBOT_STATE_IDLE,
    ROBOT_STATE_ACTIVE,
    ROBOT_STATE_FAULT,
    ROBOT_STATE_ESTOP,
};

/**
 * @brief Initialize the state machine in BOOT state.
 */
void state_machine_init(void);

/**
 * @brief Request a state transition.
 *
 * The request is validated against allowed transitions. ESTOP always succeeds.
 *
 * @param new_state Desired target state.
 * @return 0 on success, -EPERM if the transition is not allowed.
 */
int state_machine_transition(enum robot_state new_state);

/**
 * @brief Return the current robot state.
 * @return Current state.
 */
enum robot_state state_machine_get(void);

/**
 * @brief Return true if the robot is in a state that permits motion.
 * @return True for ACTIVE, false otherwise.
 */
bool state_machine_motion_allowed(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_APP_STATE_MACHINE_H */
