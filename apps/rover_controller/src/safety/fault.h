/**
 * @file fault.h
 * @brief Fault flag definitions and latching helpers.
 *
 * The safety monitor sets fault flags; the control loop and diagnostics read
 * them to decide whether motion is allowed and what to report.
 */

#ifndef ROVER_CONTROLLER_SAFETY_FAULT_H
#define ROVER_CONTROLLER_SAFETY_FAULT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Individual fault bits used by the safety layer. */
enum fault_bit {
    FAULT_BIT_NONE          = 0,
    FAULT_BIT_CMD_TIMEOUT   = (1U << 0),
    FAULT_BIT_AGENT_LOST    = (1U << 1),
    FAULT_BIT_UNDERVOLTAGE  = (1U << 2),
    FAULT_BIT_OVERCURRENT   = (1U << 3),
    FAULT_BIT_MOTOR_FEEDBACK = (1U << 4),
    FAULT_BIT_ESTOP         = (1U << 5),
    FAULT_BIT_WATCHDOG      = (1U << 6),
};

/**
 * @brief Initialize the fault word.
 *
 * Clears latched faults and resets the critical-fault history.
 */
void fault_init(void);

/**
 * @brief Set one or more fault bits.
 *
 * Setting is atomic and latched until explicitly cleared by fault_clear().
 *
 * @param bits Bitmask of @ref fault_bit values.
 */
void fault_set(uint32_t bits);

/**
 * @brief Clear one or more fault bits.
 *
 * @param bits Bitmask of @ref fault_bit values to clear.
 */
void fault_clear(uint32_t bits);

/**
 * @brief Return the current fault bitmask.
 * @return Bitmask of active faults.
 */
uint32_t fault_get(void);

/**
 * @brief Test whether any of the given fault bits are active.
 *
 * @param bits Bitmask to test.
 * @return True if any requested bit is set.
 */
bool fault_is_set(uint32_t bits);

/**
 * @brief Test whether a motion-inhibiting fault is active.
 *
 * Convenience wrapper that checks the set of faults that should stop the robot.
 *
 * @return True if motion should be inhibited.
 */
bool fault_motion_inhibited(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_SAFETY_FAULT_H */
