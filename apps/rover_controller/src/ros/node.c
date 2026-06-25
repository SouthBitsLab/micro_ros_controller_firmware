/**
 * @file node.c
 * @brief Single micro-ROS node and executor implementation.
 *
 * Uses UDP transport by default, matching the existing h723_rover_controller
 * configuration. The executor thread is event-driven with a bounded spin
 * timeout.
 */

#include "ros/node.h"
#include <stdio.h>
#include <string.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <microros_transports.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ros_node, LOG_LEVEL_DBG);

#define ROS_NODE_STACK_SIZE 8192
#define ROS_NODE_PRIORITY   K_PRIO_PREEMPT(5)
#define ROS_SPIN_TIMEOUT_MS 10

static void ros_node_thread_entry(void *p1, void *p2, void *p3);

K_THREAD_DEFINE(ros_node_tid, ROS_NODE_STACK_SIZE,
                ros_node_thread_entry, NULL, NULL, NULL,
                ROS_NODE_PRIORITY, 0, 0);

static rcl_allocator_t allocator;
static rclc_support_t support;
static rcl_node_t node;
static rclc_executor_t executor;
static bool agent_connected;

int ros_node_init(void)
{
    /* Configure UDP transport to match the host agent. */
    snprintf((char *)default_params.ip, sizeof(default_params.ip), "10.42.0.1");
    snprintf((char *)default_params.port, sizeof(default_params.port), "8888");

    rmw_uros_set_custom_transport(
        MICRO_ROS_FRAMING_REQUIRED,
        (void *)&default_params,
        zephyr_transport_open,
        zephyr_transport_close,
        zephyr_transport_write,
        zephyr_transport_read);

    allocator = rcl_get_default_allocator();

    if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK) {
        LOG_ERR("Failed to initialize rclc support");
        return -EIO;
    }

    if (rclc_node_init_default(&node, "rover_controller", "", &support) != RCL_RET_OK) {
        LOG_ERR("Failed to initialize node");
        return -EIO;
    }

    const size_t executor_handles = 8; /* Timers + subscriptions. */
    if (rclc_executor_init(&executor, &support.context,
                           executor_handles, &allocator) != RCL_RET_OK) {
        LOG_ERR("Failed to initialize executor");
        return -EIO;
    }

    agent_connected = false;
    LOG_INF("micro-ROS node initialized");

    return 0;
}

int ros_node_start(void)
{
    /* The thread is defined with K_THREAD_DEFINE; nothing to do here. */
    LOG_INF("micro-ROS executor thread started");
    return 0;
}

rclc_executor_t *ros_node_get_executor(void)
{
    return &executor;
}

rcl_node_t *ros_node_get_handle(void)
{
    return &node;
}

rcl_allocator_t *ros_node_get_allocator(void)
{
    return &allocator;
}

bool ros_node_agent_connected(void)
{
    return agent_connected;
}

static void ros_node_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("micro-ROS executor thread running");

    while (true) {
        rcl_ret_t rc = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(ROS_SPIN_TIMEOUT_MS));
        agent_connected = (rc == RCL_RET_OK);
        k_sleep(K_MSEC(ROS_SPIN_TIMEOUT_MS));
    }
}
