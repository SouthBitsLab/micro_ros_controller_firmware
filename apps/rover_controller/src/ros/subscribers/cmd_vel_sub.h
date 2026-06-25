/**
 * @file cmd_vel_sub.h
 * @brief /cmd_vel subscriber interface.
 */

#ifndef ROVER_CONTROLLER_ROS_SUBSCRIBERS_CMD_VEL_SUB_H
#define ROVER_CONTROLLER_ROS_SUBSCRIBERS_CMD_VEL_SUB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the /cmd_vel subscriber.
 * @return 0 on success, negative errno on failure.
 */
int cmd_vel_sub_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_SUBSCRIBERS_CMD_VEL_SUB_H */
