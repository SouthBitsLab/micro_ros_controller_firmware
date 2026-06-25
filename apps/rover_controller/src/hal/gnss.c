/**
 * @file gnss.c
 * @brief HAL adapter for the GNSS receiver.
 *
 * A driver-level fix callback converts each new GNSS report into a
 * HAL-neutral @ref gnss_fix structure and enqueues it on a Zephyr message
 * queue. The queue is drained by a separate telemetry thread in main.c, which
 * keeps the callback path short and isolates sensor timing from micro-ROS
 * publish latency.
 */

#include "hal/gnss.h"
#include "common/time_utils.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gnss.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(hal_gnss, LOG_LEVEL_DBG);

/* Number of GNSS fixes the queue can hold before the producer starts dropping. */
#define GNSS_MSGQ_COUNT 8U

/* Message queue for parsed GNSS fixes. */
K_MSGQ_DEFINE(gnss_msgq, sizeof(struct gnss_fix), GNSS_MSGQ_COUNT, 4);

/* Static device reference resolved from the Devicetree alias. */
static const struct device *const gnss_dev = DEVICE_DT_GET(DT_ALIAS(gnss0));

/**
 * @brief Callback invoked by the GNSS driver when a new fix is available.
 *
 * Parses the fix data, converts it to the HAL-neutral @ref gnss_fix format,
 * and enqueues the fix with no wait.
 */
static void gnss_data_cb(const struct device *dev, const struct gnss_data *data)
{
    struct gnss_fix fix = {0};

    fix.timestamp_ms = time_now_ms();
    fix.num_satellites = (uint8_t)data->info.satellites_cnt;
    fix.quality = (uint8_t)data->info.fix_quality;

    if (data->info.fix_status != GNSS_FIX_STATUS_NO_FIX) {
        fix.latitude = (double)data->nav_data.latitude / 1e9;
        fix.longitude = (double)data->nav_data.longitude / 1e9;
        fix.altitude = (double)data->nav_data.altitude / 1000.0;

        // LOG_INF("date: %02d:%02d:%02d:%03d %02d-%02d-%04d",
        //         data->utc.hour, data->utc.minute,
        //         data->utc.millisecond / 1000, data->utc.millisecond % 1000,
        //         data->utc.month_day, data->utc.month,
        //         data->utc.century_year + 2000);
        // LOG_INF("%s location (%.7f, %.7f) with %d tracked satellites",
        //         dev->name, fix.latitude, fix.longitude, fix.num_satellites);

        int rc = k_msgq_put(&gnss_msgq, &fix, K_NO_WAIT);
        if (rc != 0) {
            LOG_WRN("GNSS fix dropped (queue full)");
        }
    } else {
        LOG_WRN("no fix. Searching satellites...");
    }

}

GNSS_DATA_CALLBACK_DEFINE(gnss_dev, gnss_data_cb);

int gnss_init(void)
{
    if (!device_is_ready(gnss_dev)) {
        LOG_ERR("GNSS device is not ready");
        return -ENODEV;
    }

    LOG_INF("GNSS initialized: %s", gnss_dev->name);
    return 0;
}

struct k_msgq *gnss_get_msgq(void)
{
    return &gnss_msgq;
}
