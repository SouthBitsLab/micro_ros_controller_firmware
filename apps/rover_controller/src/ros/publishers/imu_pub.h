/**
 * @file imu_pub.h
 * @brief /microros/imu publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_IMU_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_IMU_PUB_H

#include "hal/sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the IMU publisher.
 * @return 0 on success, negative errno on failure.
 */
int imu_pub_init(void);

/**
 * @brief Publish the supplied IMU sample as sensor_msgs/Imu.
 *
 * @param sample Timestamped accelerometer + gyroscope sample to publish.
 */
void imu_pub_publish(struct imu_sample sample);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_IMU_PUB_H */
