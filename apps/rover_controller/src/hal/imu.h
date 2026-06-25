/**
 * @file imu.h
 * @brief HAL interface for the ICG-20660L IMU.
 *
 * The IMU adapter configures a data-ready trigger and, for every interrupt,
 * fetches an accelerometer + gyroscope sample and places it on a kernel message
 * queue. Higher-level consumers (the ROS publisher thread) dequeue samples and
 * publish them.
 *
 * This design decouples the sensor interrupt path from micro-ROS publishing so
 * that slow or transient agent disconnections do not block the driver.
 */

#ifndef ROVER_CONTROLLER_HAL_IMU_H
#define ROVER_CONTROLLER_HAL_IMU_H

#include <zephyr/kernel.h>

#include "hal/sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the IMU hardware and data-ready trigger.
 *
 * @return 0 on success, negative errno on failure.
 */
int imu_init(void);

/**
 * @brief Return the kernel message queue that receives IMU samples.
 *
 * Consumers should call k_msgq_get() on the returned queue. The queue is valid
 * after imu_init() but is statically allocated, so it may be referenced earlier
 * (e.g. by a K_THREAD_DEFINE thread that blocks until data arrives).
 */
struct k_msgq *imu_get_msgq(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_HAL_IMU_H */
