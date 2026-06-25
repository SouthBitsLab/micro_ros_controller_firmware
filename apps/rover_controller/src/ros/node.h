/**
 * @file node.h
 * @brief Single micro-ROS node and executor management.
 *
 * All ROS publishers and subscribers share one node handle and one executor.
 * This module owns initialization, spin, teardown, and transport setup.
 */

#ifndef ROVER_CONTROLLER_ROS_NODE_H
#define ROVER_CONTROLLER_ROS_NODE_H

#include <stdbool.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the micro-ROS node, transport, and executor.
 *
 * @return 0 on success, negative errno on failure.
 */
int ros_node_init(void);

/**
 * @brief Start the ROS executor in a dedicated thread.
 *
 * The thread runs rclc_executor_spin_some() at a fixed rate and handles
 * subscriptions and timers.
 *
 * @return 0 on success, negative errno on failure.
 */
int ros_node_start(void);

/**
 * @brief Return the shared executor for publisher/subscriber registration.
 * @return Pointer to the executor, or NULL before initialization.
 */
rclc_executor_t *ros_node_get_executor(void);

/**
 * @brief Return the shared node handle.
 * @return Pointer to the node, or NULL before initialization.
 */
rcl_node_t *ros_node_get_handle(void);

/**
 * @brief Return the allocator used by the node.
 * @return Pointer to the allocator, or NULL before initialization.
 */
rcl_allocator_t *ros_node_get_allocator(void);

/**
 * @brief Return true if the micro-ROS agent is currently connected.
 * @return True if the last ping/spin succeeded.
 */
bool ros_node_agent_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_NODE_H */
