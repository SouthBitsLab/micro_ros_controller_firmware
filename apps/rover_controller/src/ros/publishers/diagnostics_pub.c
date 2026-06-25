/**
 * @file diagnostics_pub.c
 * @brief /diagnostics publisher implementation.
 */

#include "ros/publishers/diagnostics_pub.h"
#include "ros/node.h"
#include "app/diagnostics.h"
#include <diagnostic_msgs/msg/diagnostic_array.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(diagnostics_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static diagnostic_msgs__msg__DiagnosticArray msg;

int diagnostics_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(diagnostic_msgs, msg, DiagnosticArray);

    if (rclc_publisher_init_default(
            &publisher, ros_node_get_handle(), type_support,
            "/diagnostics") != RCL_RET_OK) {
        LOG_ERR("Failed to create diagnostics publisher");
        return -EIO;
    }

    return 0;
}

void diagnostics_pub_publish(void)
{
    struct diagnostic_flags flags;

    diagnostics_update();
    if (diagnostics_get(&flags) != 0) {
        return;
    }

    /* TODO: convert flags into DiagnosticStatus messages. */
    (void)flags;

    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish diagnostics");
    }
}
