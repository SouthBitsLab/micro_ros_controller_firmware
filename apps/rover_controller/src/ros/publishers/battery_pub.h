/**
 * @file battery_pub.h
 * @brief /battery_state publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_BATTERY_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_BATTERY_PUB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the battery state publisher.
 * @return 0 on success, negative errno on failure.
 */
int battery_pub_init(void);

/**
 * @brief Publish the current battery state.
 */
void battery_pub_publish(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_BATTERY_PUB_H */
