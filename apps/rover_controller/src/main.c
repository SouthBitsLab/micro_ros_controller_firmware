/**
 * @file main.c
 * @brief Application entry point and initialization orchestration.
 *
 * main() performs a bottom-up initialization of the firmware layers:
 *   1. Board HAL.
 *   2. Common resources (command target, time utilities).
 *   3. Hardware adapters (sensors, power, motor drive HAL).
 *   4. Safety layer (faults, watchdog, safety monitor).
 *   5. Application layer (state machine, control loop).
 *   6. ROS interface (node, publishers, subscriber).
 *
 * After initialization it enters a low-priority telemetry loop that publishes
 * slow topics and monitors the micro-ROS agent connection.
 */

#include <stdio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "common/time_utils.h"
#include "hal/gnss.h"
#include "hal/imu.h"

// #include "safety/safety_monitor.h"
// #include "safety/watchdog.h"

// #include "app/state_machine.h"
// #include "app/robot_command.h"
// #include "app/control_loop.h"
// #include "app/diagnostics.h"

#include "ros/node.h"
#include "ros/publishers/heartbeat_pub.h"
#include "ros/publishers/gnss_pub.h"
#include "ros/publishers/imu_pub.h"
// #include "ros/publishers/odom_pub.h"
// #include "ros/publishers/wheel_states_pub.h"
// #include "ros/publishers/battery_pub.h"
// #include "ros/publishers/gnss_pub.h"
// #include "ros/publishers/diagnostics_pub.h"
// #include "ros/subscribers/cmd_vel_sub.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

static int initialize_layers(void)
{
    int rc;

    // /* Layer 0: board HAL. */
    // rc = board_init();
    // if (rc != 0) {
    //     LOG_ERR("board_init failed: %d", rc);
    //     return rc;
    // }

    // /* Layer 1: common resources. */
    // time_now_ms(); /* warm up uptime reference */
    // rc = robot_command_init();
    // if (rc != 0) {
    //     LOG_ERR("robot_command_init failed: %d", rc);
    //     return rc;
    // }

    rc = gnss_init();
    if (rc != 0) {
        LOG_ERR("gnss_init failed: %d", rc);
        return rc;
    }

    rc = imu_init();
    if (rc != 0) {
        LOG_ERR("imu_init failed: %d", rc);
        return rc;
    }

    // (void)sensor_gnss_init();
    // (void)power_monitor_init();

    // const struct device *motor_dev = board_get_motor_drive_device();
    // if (motor_dev != NULL) {
    //     (void)motor_drive_init(motor_dev);
    // }

    // /* Layer 3: safety. */
    // rc = safety_monitor_init();
    // if (rc != 0) {
    //     LOG_ERR("safety_monitor_init failed: %d", rc);
    //     return rc;
    // }

    // /* Layer 4: application / control. */
    // state_machine_init();
    // rc = control_loop_init();
    // if (rc != 0) {
    //     LOG_ERR("control_loop_init failed: %d", rc);
    //     return rc;
    // }
    // diagnostics_init();

    /* Layer 5: ROS interface. */
    rc = ros_node_init();
    if (rc != 0) {
        LOG_ERR("ros_node_init failed: %d", rc);
        return rc;
    }

    // rc = heartbeat_pub_init();
    // if (rc != 0) {
    //     LOG_ERR("heartbeat_pub_init failed: %d", rc);
    //     return rc;
    // }

    rc = imu_pub_init();
    if (rc != 0) {
        LOG_ERR("imu_pub_init failed: %d", rc);
        return rc;
    }

    // rc = odom_pub_init();
    // if (rc != 0) {
    //     LOG_ERR("odom_pub_init failed: %d", rc);
    //     return rc;
    // }

    // rc = wheel_states_pub_init();
    // if (rc != 0) {
    //     LOG_ERR("wheel_states_pub_init failed: %d", rc);
    //     return rc;
    // }

    // rc = battery_pub_init();
    // if (rc != 0) {
    //     LOG_ERR("battery_pub_init failed: %d", rc);
    //     return rc;
    // }

    rc = gnss_pub_init();
    if (rc != 0) {
        LOG_ERR("gnss_pub_init failed: %d", rc);
        return rc;
    }

    // rc = diagnostics_pub_init();
    // if (rc != 0) {
    //     LOG_ERR("diagnostics_pub_init failed: %d", rc);
    //     return rc;
    // }

    // rc = cmd_vel_sub_init();
    // if (rc != 0) {
    //     LOG_ERR("cmd_vel_sub_init failed: %d", rc);
    //     return rc;
    // }

    return 0;
}

/* Thread that drains sensor message queues and publishes each sample to micro-ROS. */
#define MICROROS_PUBLISH_STACK_SIZE 4096
#define MICROROS_PUBLISH_PRIORITY   K_PRIO_PREEMPT(2)

static void microros_publish_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    /* Additional sensor queues can be added here as new K_POLL_EVENT
     * initializers; the dispatch switch below selects the right publisher.
     */
    struct k_poll_event events[] = {
        K_POLL_EVENT_STATIC_INITIALIZER(
            K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
            K_POLL_MODE_NOTIFY_ONLY,
            imu_get_msgq(),
            0),
        K_POLL_EVENT_STATIC_INITIALIZER(
            K_POLL_TYPE_MSGQ_DATA_AVAILABLE,
            K_POLL_MODE_NOTIFY_ONLY,
            gnss_get_msgq(),
            0),
    };

    while (true) {

        int rc = k_poll(events, ARRAY_SIZE(events), K_FOREVER);
        if (rc != 0) {
            continue;
        }

        if (events[0].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
            struct imu_sample sample;

            /* Drain the queue so bursts do not backlog indefinitely. */
            while (k_msgq_get(imu_get_msgq(), &sample, K_NO_WAIT) == 0) {
                imu_pub_publish(sample);
            }
            events[0].state = K_POLL_STATE_NOT_READY;
        }

        if (events[1].state == K_POLL_STATE_MSGQ_DATA_AVAILABLE) {
            struct gnss_fix fix;

            while (k_msgq_get(gnss_get_msgq(), &fix, K_NO_WAIT) == 0) {
                gnss_pub_publish(fix);
            }
            events[1].state = K_POLL_STATE_NOT_READY;
        }
    }
}

/* Stack and thread object for the micro-ROS publisher thread.
 * The thread is created in start_threads() after the IMU driver and ROS node
 * are initialized, so it never publishes on an inactive publisher.
 */
static K_THREAD_STACK_DEFINE(microros_publish_stack, MICROROS_PUBLISH_STACK_SIZE);
static struct k_thread microros_publish_thread;

static void start_threads(void)
{
    /* Safety monitor and control loop are statically defined with
     * K_THREAD_DEFINE; calling start() logs confirmation. */
    // (void)safety_monitor_start();
    // (void)control_loop_start();
    (void)ros_node_start();

    k_thread_create(
        &microros_publish_thread,
        microros_publish_stack,
        K_THREAD_STACK_SIZEOF(microros_publish_stack),
        microros_publish_thread_entry,
        NULL, NULL, NULL,
        MICROROS_PUBLISH_PRIORITY, 0, K_NO_WAIT);
}

int main(void)
{
    LOG_INF("Rover controller firmware starting");

    if (initialize_layers() != 0) {
        LOG_ERR("Initialization failed; halting");
        return -1;
    }

    //state_machine_transition(ROBOT_STATE_WAIT_AGENT);
    start_threads();

    /* Wait briefly to allow the executor to connect before leaving BOOT. */
    k_sleep(K_MSEC(100));
    //state_machine_transition(ROBOT_STATE_IDLE);

    while (true) {
        k_sleep(K_MSEC(1000));
    }

    return 0;
}