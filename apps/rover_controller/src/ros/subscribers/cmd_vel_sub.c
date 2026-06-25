/**
 * @file cmd_vel_sub.c
 * @brief /cmd_vel subscriber implementation.
 *
 * Receives geometry_msgs/Twist, clamps it to safety limits, and writes it to
 * the shared robot_command target. It never commands motors directly.
 */

#include "ros/subscribers/cmd_vel_sub.h"
#include "ros/node.h"
#include "app/robot_command.h"
#include "app/state_machine.h"
#include "safety/safety_monitor.h"
#include <geometry_msgs/msg/twist.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(cmd_vel_sub, LOG_LEVEL_DBG);

static rcl_subscription_t subscription;
static geometry_msgs__msg__Twist msg;

/**
 * @brief Callback invoked by the executor when a /cmd_vel message arrives.
 */
static void cmd_vel_callback(const void *msg_in)
{
    const geometry_msgs__msg__Twist *twist = (const geometry_msgs__msg__Twist *)msg_in;
    struct robot_command cmd;

    cmd.linear_x = twist->linear.x;
    cmd.angular_z = twist->angular.z;
    cmd.timestamp_ms = 0; /* robot_command_set fills this. */
    cmd.valid = true;

    (void)robot_command_set(&cmd);

    /* Transition from IDLE to ACTIVE when the first safe command arrives. */
    if (state_machine_get() == ROBOT_STATE_IDLE &&
        safety_monitor_motion_permitted()) {
        (void)state_machine_transition(ROBOT_STATE_ACTIVE);
    }
}

int cmd_vel_sub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist);

    if (rclc_subscription_init_default(
            &subscription, ros_node_get_handle(), type_support,
            "/cmd_vel") != RCL_RET_OK) {
        LOG_ERR("Failed to create cmd_vel subscriber");
        return -EIO;
    }

    if (rclc_executor_add_subscription(
            ros_node_get_executor(), &subscription, &msg,
            cmd_vel_callback, ON_NEW_DATA) != RCL_RET_OK) {
        LOG_ERR("Failed to add cmd_vel subscription to executor");
        return -EIO;
    }

    return 0;
}
