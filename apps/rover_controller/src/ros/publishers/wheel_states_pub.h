/**
 * @file wheel_states_pub.h
 * @brief /wheel_states publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_WHEEL_STATES_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_WHEEL_STATES_PUB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the wheel states publisher.
 * @return 0 on success, negative errno on failure.
 */
int wheel_states_pub_init(void);

/**
 * @brief Publish the current wheel positions and velocities.
 */
void wheel_states_pub_publish(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_WHEEL_STATES_PUB_H */
