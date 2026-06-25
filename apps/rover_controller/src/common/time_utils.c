/**
 * @file time_utils.c
 * @brief Implementation of kernel time helpers.
 */

#include "common/time_utils.h"
#include <zephyr/kernel.h>

int64_t time_now_ms(void)
{
    return k_uptime_get();
}

bool time_is_expired_ms(int64_t timestamp_ms, int32_t timeout_ms)
{
    if (timestamp_ms <= 0 || timeout_ms <= 0) {
        return true;
    }

    return (time_now_ms() - timestamp_ms) >= timeout_ms;
}

int64_t time_elapsed_ms(int64_t timestamp_ms)
{
    int64_t delta = time_now_ms() - timestamp_ms;

    return (delta > 0) ? delta : 0;
}
