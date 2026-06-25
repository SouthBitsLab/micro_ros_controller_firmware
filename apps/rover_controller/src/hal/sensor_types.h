/**
 * @file sensor_types.h
 * @brief Common data structures for HAL sensor interfaces.
 *
 * This header contains data types that abstract away hardware-specific details of sensors and 
 * provide a common language for the HAL, control, and ROS layers to interact with sensor data. 
 * Each sensor type has a corresponding adapter implementation in the HAL that populates these 
 * structures, and the ROS publishers convert them to the appropriate ROS message types.
 */

#ifndef ROVER_CONTROLLER_HAL_SENSOR_TYPES_H
#define ROVER_CONTROLLER_HAL_SENSOR_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timestamped 9-axis IMU sample.
 *
 * Units match the ROS sensor_msgs/Imu convention so the ROS publisher only
 * performs a copy, but this type is driver-agnostic.
 */
struct imu_sample {
    /** Acceleration in m/s^2. */
    float accel[3];

    /** Angular velocity in rad/s. */
    float gyro[3];

    /** Magnetic field strength in Tesla. */
    float mag[3];

    /** Sample timestamp in kernel uptime milliseconds. */
    int64_t timestamp_ms;
};

/**
 * @brief Parsed GNSS fix.
 *
 * Hides UART vs. I2C transport details. The GNSS adapter fills this structure;
 * the ROS publisher converts it to sensor_msgs/NavSatFix.
 */
struct gnss_fix {
    /** Latitude in decimal degrees. */
    double latitude;

    /** Longitude in decimal degrees. */
    double longitude;

    /** Altitude above ellipsoid in meters. */
    double altitude;

    /** GNSS fix quality indicator (0 = invalid, 1 = fix, 2 = DGPS, etc.). */
    uint8_t quality;

    /** Number of satellites used in the solution. */
    uint8_t num_satellites;

    /** Sample timestamp in kernel uptime milliseconds. */
    int64_t timestamp_ms;
};

/**
 * @brief Aggregated power/battery state.
 *
 * Populated by the power monitor adapter and consumed by diagnostics and the
 * battery ROS publisher.
 */
struct battery_state {
    /** Battery terminal voltage (V). */
    float voltage_v;

    /** Load current (A). Positive when discharging. */
    float current_a;

    /** Instantaneous power (W). */
    float power_w;

    /** State-of-charge estimate, 0.0–1.0 if available. */
    float soc;

    /** Adapter-specific status flags. */
    uint32_t status_flags;

    /** Sample timestamp in kernel uptime milliseconds. */
    int64_t timestamp_ms;
};

#ifdef __cplusplus
}
#endif

#endif /* ROVER_CONTROLLER_HAL_SENSOR_TYPES_H */
