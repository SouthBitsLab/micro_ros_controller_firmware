/**
 * @file diagnostics_pub.h
 * @brief /diagnostics publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_DIAGNOSTICS_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_DIAGNOSTICS_PUB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the diagnostics publisher.
 * @return 0 on success, negative errno on failure.
 */
int diagnostics_pub_init(void);

/**
 * @brief Publish the current diagnostics array.
 */
void diagnostics_pub_publish(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_DIAGNOSTICS_PUB_H */
