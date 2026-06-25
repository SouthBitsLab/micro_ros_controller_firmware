/**
 * @file imu.c
 * @brief HAL adapter for the ICG-20660L 6-axis IMU.
 *
 * A data-ready trigger handler runs in the sensor driver's own thread and
 * copies each new sample into a Zephyr message queue. The queue is drained by
 * a separate telemetry thread in main.c, which keeps the interrupt path short
 * and isolates sensor timing from micro-ROS publish latency.
 */

#include "hal/imu.h"
#include "common/time_utils.h"

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hal_imu, LOG_LEVEL_DBG);

/* Number of IMU samples the queue can hold before the producer starts dropping. */
#define IMU_MSGQ_COUNT 32U

/* Message queue for timestamped IMU samples. */
K_MSGQ_DEFINE(imu_msgq, sizeof(struct imu_sample), IMU_MSGQ_COUNT, 4);

/* Static device reference resolved from the Devicetree alias. */
static const struct device *const imu_dev = DEVICE_DT_GET(DT_ALIAS(imu0));

/* Data-ready trigger descriptor used to register the handler. */
static struct sensor_trigger imu_trigger = {
    .type = SENSOR_TRIG_DATA_READY,
    .chan = SENSOR_CHAN_ACCEL_XYZ,
};

/**
 * @brief Trigger handler called by the sensor driver when new data is ready.
 *
 * Fetches the latest accelerometer and gyroscope values, converts them to the
 * HAL-neutral @ref imu_sample format, and enqueues the sample with no wait.
 */
static void imu_trigger_handler(const struct device *dev,
                                const struct sensor_trigger *trigger)
{
    ARG_UNUSED(trigger);

    struct imu_sample sample = {0};
    struct sensor_value accel[3];
    struct sensor_value gyro[3];

    /* Fetch all channels to clear the data-ready interrupt. */
    if (sensor_sample_fetch(dev) != 0) {
        LOG_WRN("IMU sample fetch failed");
        return;
    }

    if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel) != 0 ||
        sensor_channel_get(dev, SENSOR_CHAN_GYRO_XYZ, gyro) != 0) {
        LOG_WRN("IMU channel get failed");
        return;
    }

    for (int i = 0; i < 3; i++) {
        sample.accel[i] = sensor_value_to_float(&accel[i]);
        sample.gyro[i] = sensor_value_to_float(&gyro[i]);
    }

    sample.timestamp_ms = time_now_ms();

    int rc = k_msgq_put(&imu_msgq, &sample, K_NO_WAIT);
    if (rc != 0) {
        LOG_WRN("IMU sample dropped (queue full)");
    }
}

int imu_init(void)
{
    if (!device_is_ready(imu_dev)) {
        LOG_ERR("IMU device is not ready");
        return -ENODEV;
    }

    if (sensor_trigger_set(imu_dev, &imu_trigger, imu_trigger_handler) != 0) {
        LOG_ERR("Failed to set IMU data-ready trigger");
        return -EIO;
    }

    LOG_INF("IMU initialized: %s", imu_dev->name);
    return 0;
}

struct k_msgq *imu_get_msgq(void)
{
    return &imu_msgq;
}
