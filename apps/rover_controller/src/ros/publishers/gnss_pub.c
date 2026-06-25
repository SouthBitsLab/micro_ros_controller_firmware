/**
 * @file gnss_pub.c
 * @brief /microros/fix publisher implementation.
 *
 * Publishes HAL-neutral GNSS fixes as sensor_msgs/NavSatFix.
 */

#include "common/time_utils.h"
#include "hal/gnss.h"
#include "hal/sensor_types.h"
#include "ros/node.h"
#include "ros/publishers/gnss_pub.h"

#include <sensor_msgs/msg/nav_sat_fix.h>
#include <sensor_msgs/msg/nav_sat_status.h>
#include <string.h>
#include <zephyr/drivers/gnss.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gnss_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static sensor_msgs__msg__NavSatFix msg;

/* Frame ID for the GNSS antenna/receiver location. */
static char gnss_frame_id[] = "gnss_antenna";

int gnss_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, NavSatFix);

    if (rclc_publisher_init_default(
            &publisher, ros_node_get_handle(), type_support,
            "/microros/fix") != RCL_RET_OK) {
        LOG_ERR("Failed to create GNSS publisher");
        return -EIO;
    }

    /* Pre-fill the frame_id so every publish only updates timestamp/data. */
    msg.header.frame_id.data = gnss_frame_id;
    msg.header.frame_id.size = strlen(gnss_frame_id);
    msg.header.frame_id.capacity = sizeof(gnss_frame_id);

    /* No covariance information available from the receiver. */
    msg.position_covariance[0] = -1.0;
    msg.position_covariance_type =
        sensor_msgs__msg__NavSatFix__COVARIANCE_TYPE_UNKNOWN;

    return 0;
}

void gnss_pub_publish(struct gnss_fix fix)
{
    msg.header.stamp.sec = fix.timestamp_ms / 1000;
    msg.header.stamp.nanosec = (fix.timestamp_ms % 1000) * 1000000;

    msg.latitude = fix.latitude;
    msg.longitude = fix.longitude;
    msg.altitude = fix.altitude;

    /* Map Zephyr fix quality to ROS NavSatStatus. */
    switch (fix.quality) {
    case GNSS_FIX_QUALITY_INVALID:
        msg.status.status = sensor_msgs__msg__NavSatStatus__STATUS_NO_FIX;
        break;
    case GNSS_FIX_QUALITY_DGNSS:
        msg.status.status = sensor_msgs__msg__NavSatStatus__STATUS_SBAS_FIX;
        break;
    case GNSS_FIX_QUALITY_RTK:
    case GNSS_FIX_QUALITY_FLOAT_RTK:
        msg.status.status = sensor_msgs__msg__NavSatStatus__STATUS_GBAS_FIX;
        break;
    case GNSS_FIX_QUALITY_GNSS_SPS:
    case GNSS_FIX_QUALITY_GNSS_PPS:
    case GNSS_FIX_QUALITY_ESTIMATED:
    default:
        msg.status.status = sensor_msgs__msg__NavSatStatus__STATUS_FIX;
        break;
    }

    /* Service bitmask: default to GPS when the actual constellation set is unknown. */
    msg.status.service = sensor_msgs__msg__NavSatStatus__SERVICE_GPS;

    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish GNSS fix");
    }
}
