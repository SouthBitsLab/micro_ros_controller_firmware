/**
 * @file battery_pub.c
 * @brief /battery_state publisher implementation.
 */

#include "ros/publishers/battery_pub.h"
#include "ros/node.h"
#include "hal/power_monitor.h"
#include <sensor_msgs/msg/battery_state.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(battery_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static sensor_msgs__msg__BatteryState msg;

int battery_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState);

    if (rclc_publisher_init_default(
            &publisher, ros_node_get_handle(), type_support,
            "/battery_state") != RCL_RET_OK) {
        LOG_ERR("Failed to create battery publisher");
        return -EIO;
    }

    return 0;
}

void battery_pub_publish(void)
{
    struct battery_state bat;

    if (power_monitor_read(&bat) != 0 || !bat.valid) {
        return;
    }

    msg.voltage = bat.voltage_v;
    msg.current = bat.current_a;
    msg.power_supply_status = SENSOR_MSGS__MSG__BATTERY_STATE__POWER_SUPPLY_STATUS_UNKNOWN;

    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish battery state");
    }
}
