/**
 * @file gnss.h
 * @brief HAL interface for the GNSS receiver.
 *
 * The GNSS adapter registers a driver callback that fires whenever a new fix is
 * reported. Each fix is converted to a HAL-neutral @ref gnss_fix structure and
 * placed on a kernel message queue. Higher-level consumers (the ROS publisher
 * thread) dequeue fixes and publish them.
 *
 * This design decouples the GNSS callback from micro-ROS publishing so slow or
 * transient agent disconnections do not block the driver.
 */

#ifndef ROVER_CONTROLLER_HAL_GNSS_H
#define ROVER_CONTROLLER_HAL_GNSS_H

#include <zephyr/kernel.h>

#include "hal/sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the GNSS receiver.
 *
 * Verifies the device is ready and logs its status. The fix callback is
 * registered statically by the driver macro.
 *
 * @return 0 on success, negative errno on failure.
 */
int gnss_init(void);

/**
 * @brief Return the kernel message queue that receives GNSS fixes.
 *
 * Consumers should call k_msgq_get() on the returned queue. The queue is valid
 * after gnss_init() but is statically allocated, so it may be referenced earlier
 * (e.g. by a K_THREAD_DEFINE thread that blocks until data arrives).
 */
struct k_msgq *gnss_get_msgq(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_HAL_GNSS_H */
