/**
 * @file heartbeat_pub.h
 * @brief /microros/heartbeat publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_HEARTBEAT_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_HEARTBEAT_PUB_H

#include <rcl/publisher.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the heartbeat publisher.
 * @return 0 on success, negative errno on failure.
 */
int heartbeat_pub_init(void);

/**
 * @brief Publish the current heartbeat counter.
 */
void heartbeat_pub_publish(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_HEARTBEAT_PUB_H */
