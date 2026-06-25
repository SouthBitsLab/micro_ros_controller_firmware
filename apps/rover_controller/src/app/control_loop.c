/**
 * @file control_loop.c
 * @brief Motor control loop thread implementation.
 *
 * Runs at a fixed high priority and fixed period. On each iteration:
 *   1. Checks safety monitor permission and state machine state.
 *   2. Reads the latest robot_command; zeros it if motion is not permitted.
 *   3. Computes wheel velocities via kinematics.
 *   4. Sends velocities to the motor drive HAL.
 *   5. Reads motor feedback and updates odometry / wheel_state.
 *   6. Checks into the watchdog.
 */

#include "app/control_loop.h"
#include "app/robot_command.h"
#include "app/kinematics.h"
#include "app/odometry.h"
#include "app/state_machine.h"
#include "safety/safety_monitor.h"
#include "safety/fault.h"
#include "safety/watchdog.h"
#include "hal/motor_drive.h"
#include "hal/board.h"
#include "common/time_utils.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(control_loop, LOG_LEVEL_DBG);

#define CONTROL_LOOP_STACK_SIZE 4096
#define CONTROL_LOOP_PRIORITY   K_PRIO_PREEMPT(3)
#define CONTROL_LOOP_PERIOD_MS  20

static void control_loop_thread_entry(void *p1, void *p2, void *p3);

K_THREAD_DEFINE(control_loop_tid, CONTROL_LOOP_STACK_SIZE,
                control_loop_thread_entry, NULL, NULL, NULL,
                CONTROL_LOOP_PRIORITY, 0, 0);

static struct k_mutex wheel_state_mutex;
static struct wheel_state latest_wheel_state;
static const struct device *motor_dev;

int control_loop_init(void)
{
    k_mutex_init(&wheel_state_mutex);
    odometry_init();

    motor_dev = board_get_motor_drive_device();
    if (motor_dev == NULL) {
        LOG_WRN("No motor drive device configured");
    }

    return 0;
}

int control_loop_start(void)
{
    LOG_INF("Control loop started");
    return 0;
}

int control_loop_get_wheel_state(struct wheel_state *out)
{
    if (out == NULL) {
        return -EINVAL;
    }

    k_mutex_lock(&wheel_state_mutex, K_FOREVER);
    *out = latest_wheel_state;
    k_mutex_unlock(&wheel_state_mutex);

    return 0;
}

static void control_loop_run_once(void)
{
    struct robot_command cmd;
    float wheel_velocities[ROVER_NUM_MOTORS];
    struct motor_feedback feedback[ROVER_NUM_MOTORS];

    /* Default to zero motion unless explicitly permitted. */
    robot_command_get(&cmd);
    if (!state_machine_motion_allowed() || !safety_monitor_motion_permitted()) {
        robot_command_zero();
        cmd.linear_x = 0.0f;
        cmd.angular_z = 0.0f;
    }

    /* Compute wheel velocities. */
    (void)kinematics_compute_wheel_velocities(&cmd, wheel_velocities);

    /* Send to motor drive HAL. */
    if (motor_dev != NULL) {
        for (uint8_t i = 0; i < ROVER_NUM_MOTORS; i++) {
            (void)motor_drive_set_velocity(motor_dev, i, wheel_velocities[i]);
        }

        /* Read feedback. */
        for (uint8_t i = 0; i < ROVER_NUM_MOTORS; i++) {
            (void)motor_drive_get_feedback(motor_dev, i, &feedback[i]);
        }

        /* Notify safety monitor of latest feedback. */
        safety_monitor_set_motor_feedback(feedback);
    }

    /* Update shared wheel state. */
    k_mutex_lock(&wheel_state_mutex, K_FOREVER);
    for (uint8_t i = 0; i < ROVER_NUM_MOTORS; i++) {
        latest_wheel_state.velocity_rad_s[i] = wheel_velocities[i];
        if (motor_dev != NULL && feedback[i].valid) {
            latest_wheel_state.position_rad[i] = feedback[i].position_rad;
        }
    }
    latest_wheel_state.timestamp_ms = time_now_ms();
    latest_wheel_state.valid = true;
    k_mutex_unlock(&wheel_state_mutex);

    /* Integrate odometry. */
    (void)odometry_update(&latest_wheel_state);

    /* Check into the watchdog. */
    watchdog_check_in(WATCHDOG_SUBSYSTEM_CONTROL);
}

static void control_loop_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("Control loop thread running");

    while (true) {
        control_loop_run_once();
        k_sleep(K_MSEC(CONTROL_LOOP_PERIOD_MS));
    }
}
