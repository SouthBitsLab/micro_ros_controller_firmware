/**
 * @file robot_command.c
 * @brief Thread-safe command target implementation.
 */

#include "app/robot_command.h"
#include "common/robot_params.h"
#include "common/time_utils.h"
#include <zephyr/kernel.h>

static struct k_mutex cmd_mutex;
static struct robot_command cmd_target;

int robot_command_init(void)
{
    k_mutex_init(&cmd_mutex);
    cmd_target.linear_x = 0.0f;
    cmd_target.angular_z = 0.0f;
    cmd_target.timestamp_ms = 0;
    cmd_target.valid = false;

    return 0;
}

int robot_command_set(const struct robot_command *cmd)
{
    if (cmd == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&cmd_mutex, K_FOREVER);

    cmd_target.linear_x = cmd->linear_x;
    if (cmd_target.linear_x > ROVER_MAX_LINEAR_VELOCITY_M_S) {
        cmd_target.linear_x = ROVER_MAX_LINEAR_VELOCITY_M_S;
    } else if (cmd_target.linear_x < -ROVER_MAX_LINEAR_VELOCITY_M_S) {
        cmd_target.linear_x = -ROVER_MAX_LINEAR_VELOCITY_M_S;
    }

    cmd_target.angular_z = cmd->angular_z;
    if (cmd_target.angular_z > ROVER_MAX_ANGULAR_VELOCITY_RAD_S) {
        cmd_target.angular_z = ROVER_MAX_ANGULAR_VELOCITY_RAD_S;
    } else if (cmd_target.angular_z < -ROVER_MAX_ANGULAR_VELOCITY_RAD_S) {
        cmd_target.angular_z = -ROVER_MAX_ANGULAR_VELOCITY_RAD_S;
    }

    cmd_target.timestamp_ms = time_now_ms();
    cmd_target.valid = true;

    k_mutex_unlock(&cmd_mutex);

    return 0;
}

int robot_command_get(struct robot_command *out)
{
    if (out == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&cmd_mutex, K_FOREVER);
    *out = cmd_target;
    k_mutex_unlock(&cmd_mutex);

    return 0;
}

void robot_command_zero(void)
{
    k_mutex_lock(&cmd_mutex, K_FOREVER);
    cmd_target.linear_x = 0.0f;
    cmd_target.angular_z = 0.0f;
    cmd_target.timestamp_ms = time_now_ms();
    cmd_target.valid = false;
    k_mutex_unlock(&cmd_mutex);
}
