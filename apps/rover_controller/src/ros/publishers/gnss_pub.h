/**
 * @file gnss_pub.h
 * @brief /fix publisher interface.
 */

#ifndef ROVER_CONTROLLER_ROS_PUBLISHERS_GNSS_PUB_H
#define ROVER_CONTROLLER_ROS_PUBLISHERS_GNSS_PUB_H

#include "hal/sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create and initialize the GNSS fix publisher.
 * @return 0 on success, negative errno on failure.
 */
int gnss_pub_init(void);

/**
 * @brief Publish the supplied GNSS fix as sensor_msgs/NavSatFix.
 *
 * @param fix Timestamped GNSS fix to publish.
 */
void gnss_pub_publish(struct gnss_fix fix);

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_ROS_PUBLISHERS_GNSS_PUB_H */
