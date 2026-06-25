/**
 * @file imu_pub.c
 * @brief /microros/imu publisher implementation.
 *
 * Reads the latest IMU sample through the HAL sensor interface and publishes it
 * as sensor_msgs/Imu.
 */


#include "algo/MadgwickAHRS.h"
#include "common/time_utils.h"
#include "hal/sensor_types.h"
#include "ros/publishers/imu_pub.h"
#include "ros/node.h"

#include <sensor_msgs/msg/imu.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(imu_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static sensor_msgs__msg__Imu msg;

int imu_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu);

    if (rclc_publisher_init_default(
            &publisher, ros_node_get_handle(), type_support,
            "/microros/imu") != RCL_RET_OK) {
        LOG_ERR("Failed to create IMU publisher");
        return -EIO;
    }

    return 0;
}

void imu_pub_publish(struct imu_sample sample)
{
    msg.header.stamp.sec = sample.timestamp_ms / 1000;
    msg.header.stamp.nanosec = (sample.timestamp_ms % 1000) * 1000000;

    msg.linear_acceleration.x = sample.accel[0];
    msg.linear_acceleration.y = sample.accel[1];
    msg.linear_acceleration.z = sample.accel[2];
    msg.angular_velocity.x = sample.gyro[0];
    msg.angular_velocity.y = sample.gyro[1];
    msg.angular_velocity.z = sample.gyro[2];

    if (sample.mag[0] != 0 || sample.mag[1] != 0 || sample.mag[2] != 0) {
        // TODO: Compute orientation using Madgwick filter if magnetometer data is available
        // For now, we will just set the orientation to a default value (no rotation)
        msg.orientation.x = 0.0;
        msg.orientation.y = 0.0;
        msg.orientation.z = 0.0;
        msg.orientation.w = 1.0;
    }else {
        // Indicate that Orientation is not yet calculated (-1 in element 0)
        msg.orientation_covariance[0] = -1.0;
    }

    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish IMU");
    }
}
