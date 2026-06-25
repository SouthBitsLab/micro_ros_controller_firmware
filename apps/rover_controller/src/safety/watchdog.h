/**
 * @file watchdog.h
 * @brief Watchdog feed policy.
 *
 * The watchdog is fed only when both the safety monitor and the control loop
 * are running on schedule. If either stalls, the system resets into a safe boot
 * state.
 */

#ifndef ROVER_CONTROLLER_SAFETY_WATCHDOG_H
#define ROVER_CONTROLLER_SAFETY_WATCHDOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the hardware watchdog with the desired timeout.
 * @return 0 on success, negative errno on failure.
 */
int watchdog_init(void);

/**
 * @brief Feed the watchdog.
 *
 * Should only be called from the safety monitor after it has verified that the
 * control loop is alive.
 */
void watchdog_feed(void);

/**
 * @brief Mark a subsystem as alive.
 *
 * The safety monitor calls this on behalf of itself and checks whether the
 * control loop has also checked in before feeding the watchdog.
 *
 * @param subsystem_id Identifier of the subsystem checking in.
 */
void watchdog_check_in(uint8_t subsystem_id);

/** @brief Subsystem identifiers used with watchdog_check_in(). */
enum watchdog_subsystem {
    WATCHDOG_SUBSYSTEM_SAFETY = 0,
    WATCHDOG_SUBSYSTEM_CONTROL,
    WATCHDOG_SUBSYSTEM_COUNT,
};

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_SAFETY_WATCHDOG_H */
