/**
 * @file diagnostics.h
 * @brief Diagnostics aggregation interface.
 *
 * Collects health flags from the safety layer, HAL, and control loop, then
 * exposes them to the ROS diagnostics publisher.
 */

#ifndef ROVER_CONTROLLER_APP_DIAGNOSTICS_H
#define ROVER_CONTROLLER_APP_DIAGNOSTICS_H

#include "common/robot_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize diagnostics state.
 */
void diagnostics_init(void);

/**
 * @brief Update the diagnostics snapshot from all subsystems.
 *
 * This should be called at the diagnostics publication rate (1 Hz).
 */
void diagnostics_update(void);

/**
 * @brief Read the latest diagnostics snapshot.
 *
 * @param out Output diagnostics structure.
 * @return 0 on success, negative errno on failure.
 */
int diagnostics_get(struct diagnostic_flags *out);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_APP_DIAGNOSTICS_H */
