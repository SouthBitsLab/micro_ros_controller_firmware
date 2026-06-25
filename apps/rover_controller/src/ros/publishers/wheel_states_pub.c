/**
 * @file wheel_states_pub.c
 * @brief /wheel_states publisher implementation.
 */

#include "ros/publishers/wheel_states_pub.h"
#include "ros/node.h"
#include "app/control_loop.h"
#include <sensor_msgs/msg/joint_state.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(wheel_states_pub, LOG_LEVEL_DBG);

static rcl_publisher_t publisher;
static sensor_msgs__msg__JointState msg;

/* Fixed joint names for the four wheel hubs. */
static const char *joint_names[4] = {
    "front_left_wheel",
    "front_right_wheel",
    "rear_left_wheel",
    "rear_right_wheel",
};

int wheel_states_pub_init(void)
{
    const rosidl_message_type_support_t *type_support =
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState);

    if (rclc_publisher_init_default(
            &publisher, ros_node_get_handle(), type_support,
            "/wheel_states") != RCL_RET_OK) {
        LOG_ERR("Failed to create wheel states publisher");
        return -EIO;
    }

    /* Point message name array at constant strings. */
    msg.name.data = (char **)joint_names;
    msg.name.size = 4;
    msg.name.capacity = 4;

    /* Position and velocity arrays are left empty in the skeleton. */
    return 0;
}

void wheel_states_pub_publish(void)
{
    struct wheel_state wheels;

    if (control_loop_get_wheel_state(&wheels) != 0 || !wheels.valid) {
        return;
    }

    /* TODO: populate msg.position and msg.velocity arrays from wheels. */
    (void)wheels;

    rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
    if (rc != RCL_RET_OK) {
        LOG_WRN("Failed to publish wheel states");
    }
}
