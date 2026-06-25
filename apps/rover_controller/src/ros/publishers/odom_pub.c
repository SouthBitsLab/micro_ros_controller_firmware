/**
 * @file odom_pub.c
 * @brief /odom publisher implementation.
 */

#include "ros/publishers/odom_pub.h"
#include "ros/node.h"
#include "app/odometry.h"
#include <nav_msgs/msg/odometry.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(odom_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static nav_msgs__msg__Odometry msg;

int odom_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry);

    if (rclc_publisher_init_default(
            &publisher, ros_node_get_handle(), type_support,
            "/odom") != RCL_RET_OK) {
        LOG_ERR("Failed to create odometry publisher");
        return -EIO;
    }

    return 0;
}

void odom_pub_publish(void)
{
    struct odometry_state odom;

    if (odometry_get(&odom) != 0 || !odom.valid) {
        return;
    }

    msg.pose.pose.position.x = odom.pose.x;
    msg.pose.pose.position.y = odom.pose.y;
    msg.pose.pose.position.z = 0.0;

    msg.twist.twist.linear.x = odom.twist.linear_x;
    msg.twist.twist.angular.z = odom.twist.angular_z;

    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish odometry");
    }
}
