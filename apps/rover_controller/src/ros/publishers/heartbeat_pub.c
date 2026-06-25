/**
 * @file heartbeat_pub.c
 * @brief /microros/heartbeat publisher implementation.
 */

#include "ros/publishers/heartbeat_pub.h"
#include "ros/node.h"
#include "common/time_utils.h"
#include <std_msgs/msg/u_int32.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(heartbeat_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static std_msgs__msg__UInt32 msg;

int heartbeat_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt32);

    if (rclc_publisher_init_default(&publisher, ros_node_get_handle(),
                                    type_support, "/microros/heartbeat") != RCL_RET_OK) {
        LOG_ERR("Failed to create heartbeat publisher");
        return -EIO;
    }

    msg.data = time_now_ms();
    return 0;
}

void heartbeat_pub_publish(void)
{
    msg.data = time_now_ms();
    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish heartbeat");
    }
}
