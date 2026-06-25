/**
 * @file odometry.c
 * @brief Wheel odometry integration implementation (stub).
 *
 * In the real implementation this will integrate wheel velocities using the
 * standard differential-drive equations. The stub simply stores the latest
 * wheel state without integration.
 */

#include "app/odometry.h"
#include "common/robot_params.h"
#include <zephyr/kernel.h>

static struct k_spinlock odom_lock;
static struct odometry_state odom_state;

void odometry_init(void)
{
    k_spinlock_key_t key = k_spin_lock(&odom_lock);
    odom_state.pose.x = 0.0f;
    odom_state.pose.y = 0.0f;
    odom_state.pose.theta = 0.0f;
    odom_state.twist.linear_x = 0.0f;
    odom_state.twist.angular_z = 0.0f;
    odom_state.timestamp_ms = 0;
    odom_state.valid = false;
    k_spin_unlock(&odom_lock, key);
}

void odometry_reset(float x, float y, float theta)
{
    k_spinlock_key_t key = k_spin_lock(&odom_lock);
    odom_state.pose.x = x;
    odom_state.pose.y = y;
    odom_state.pose.theta = theta;
    k_spin_unlock(&odom_lock, key);
}

int odometry_update(const struct wheel_state *wheel_state)
{
    if (wheel_state == NULL) {
        return -EINVAL;
    }

    /* TODO: integrate pose from wheel velocities. */
    k_spinlock_key_t key = k_spin_lock(&odom_lock);
    odom_state.twist.linear_x = 0.0f;
    odom_state.twist.angular_z = 0.0f;
    odom_state.timestamp_ms = wheel_state->timestamp_ms;
    odom_state.valid = wheel_state->valid;
    k_spin_unlock(&odom_lock, key);

    return 0;
}

int odometry_get(struct odometry_state *out)
{
    if (out == NULL) {
        return -EINVAL;
    }

    k_spinlock_key_t key = k_spin_lock(&odom_lock);
    *out = odom_state;
    k_spin_unlock(&odom_lock, key);

    return 0;
}
