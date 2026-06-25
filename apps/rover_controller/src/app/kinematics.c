/**
 * @file kinematics.c
 * @brief Skid-steer kinematics implementation.
 */

#include "app/kinematics.h"
#include "common/robot_params.h"

static const float direction_signs[ROVER_NUM_MOTORS] = ROVER_MOTOR_DIRECTION_SIGNS;

int kinematics_compute_wheel_velocities(const struct robot_command *cmd,
                                        float wheel_velocities_rad_s[ROVER_NUM_MOTORS])
{
    if (cmd == NULL || wheel_velocities_rad_s == NULL) {
        return -EINVAL;
    }

    const float v = cmd->linear_x;
    const float omega = cmd->angular_z;

    const float v_left = v - (omega * ROVER_TRACK_WIDTH_M / 2.0f);
    const float v_right = v + (omega * ROVER_TRACK_WIDTH_M / 2.0f);

    /* Fill left-side motors. */
    wheel_velocities_rad_s[ROVER_MOTOR_FRONT_LEFT] =
        direction_signs[ROVER_MOTOR_FRONT_LEFT] * (v_left / ROVER_WHEEL_RADIUS_M);
    wheel_velocities_rad_s[ROVER_MOTOR_REAR_LEFT] =
        direction_signs[ROVER_MOTOR_REAR_LEFT] * (v_left / ROVER_WHEEL_RADIUS_M);

    /* Fill right-side motors. */
    wheel_velocities_rad_s[ROVER_MOTOR_FRONT_RIGHT] =
        direction_signs[ROVER_MOTOR_FRONT_RIGHT] * (v_right / ROVER_WHEEL_RADIUS_M);
    wheel_velocities_rad_s[ROVER_MOTOR_REAR_RIGHT] =
        direction_signs[ROVER_MOTOR_REAR_RIGHT] * (v_right / ROVER_WHEEL_RADIUS_M);

    /* Clamp to maximum wheel velocity. */
    for (uint8_t i = 0; i < ROVER_NUM_MOTORS; i++) {
        if (wheel_velocities_rad_s[i] > ROVER_MAX_WHEEL_VELOCITY_RAD_S) {
            wheel_velocities_rad_s[i] = ROVER_MAX_WHEEL_VELOCITY_RAD_S;
        } else if (wheel_velocities_rad_s[i] < -ROVER_MAX_WHEEL_VELOCITY_RAD_S) {
            wheel_velocities_rad_s[i] = -ROVER_MAX_WHEEL_VELOCITY_RAD_S;
        }
    }

    return 0;
}

int kinematics_compute_twist(const float wheel_velocities_rad_s[ROVER_NUM_MOTORS],
                             struct robot_twist *twist)
{
    if (wheel_velocities_rad_s == NULL || twist == NULL) {
        return -EINVAL;
    }

    const float w_fl = wheel_velocities_rad_s[ROVER_MOTOR_FRONT_LEFT] /
                       direction_signs[ROVER_MOTOR_FRONT_LEFT];
    const float w_fr = wheel_velocities_rad_s[ROVER_MOTOR_FRONT_RIGHT] /
                       direction_signs[ROVER_MOTOR_FRONT_RIGHT];

    const float v_left = w_fl * ROVER_WHEEL_RADIUS_M;
    const float v_right = w_fr * ROVER_WHEEL_RADIUS_M;

    twist->linear_x = (v_right + v_left) / 2.0f;
    twist->angular_z = (v_right - v_left) / ROVER_TRACK_WIDTH_M;

    return 0;
}
