/**
 * @file safety_monitor.h
 * @brief High-priority safety monitor thread interface.
 *
 * The safety monitor is a cooperative, high-priority thread that runs
 * independently of ROS and the control loop. It validates command age, battery
 * voltage, E-stop, and motor feedback, and posts fault flags that the control
 * loop must honor before issuing motor commands.
 */

#ifndef ROVER_CONTROLLER_SAFETY_SAFETY_MONITOR_H
#define ROVER_CONTROLLER_SAFETY_SAFETY_MONITOR_H

#include <stdint.h>
#include <stdbool.h>
#include "common/robot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the safety monitor and its resources.
 *
 * Must be called before safety_monitor_start().
 *
 * @return 0 on success, negative errno on failure.
 */
int safety_monitor_init(void);

/**
 * @brief Start the safety monitor thread.
 *
 * The monitor begins periodic execution after this call.
 *
 * @return 0 on success, negative errno on failure.
 */
int safety_monitor_start(void);

/**
 * @brief Notify the safety monitor that the micro-ROS agent is connected.
 *
 * @param connected True if the agent is currently reachable.
 */
void safety_monitor_set_agent_connected(bool connected);

/**
 * @brief Notify the safety monitor of the latest battery reading.
 *
 * @param voltage_v Battery terminal voltage (V).
 */
void safety_monitor_set_battery_voltage(float voltage_v);

/**
 * @brief Notify the safety monitor of the latest motor feedback.
 *
 * @param feedback Array of per-motor feedback. Must have ROVER_NUM_MOTORS
 *                 elements.
 */
void safety_monitor_set_motor_feedback(const struct motor_feedback *feedback);

/**
 * @brief Return true if motion is currently permitted by the safety monitor.
 *
 * The control loop checks this before applying kinematics outputs.
 *
 * @return True if it is safe to command non-zero velocity.
 */
bool safety_monitor_motion_permitted(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_SAFETY_SAFETY_MONITOR_H */
