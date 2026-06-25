/**
 * @file odom_pub.h
 * @brief /odom publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_ODOM_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_ODOM_PUB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the odometry publisher.
 * @return 0 on success, negative errno on failure.
 */
int odom_pub_init(void);

/**
 * @brief Publish the current odometry estimate.
 */
void odom_pub_publish(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_ODOM_PUB_H */
