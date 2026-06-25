/**
 * @file watchdog.c
 * @brief Watchdog implementation stub.
 *
 * When a hardware watchdog device is available in Devicetree, this module will
 * configure it. For now it logs feed events and tracks subsystem check-ins
 * without triggering a reset.
 */

#include "safety/watchdog.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(safety_watchdog, LOG_LEVEL_DBG);

static bool checkins[WATCHDOG_SUBSYSTEM_COUNT];

int watchdog_init(void)
{
    for (int i = 0; i < WATCHDOG_SUBSYSTEM_COUNT; i++) {
        checkins[i] = false;
    }

    /* TODO: configure hardware watchdog via DT if available. */
    LOG_INF("Watchdog initialized (stub)");
    return 0;
}

void watchdog_feed(void)
{
    bool all_ok = true;

    for (int i = 0; i < WATCHDOG_SUBSYSTEM_COUNT; i++) {
        if (!checkins[i]) {
            all_ok = false;
            break;
        }
    }

    if (all_ok) {
        /* TODO: call wdt_feed(). */
        LOG_DBG("Watchdog fed");
    } else {
        LOG_WRN("Watchdog not fed: missing subsystem check-in");
    }

    /* Reset check-ins for the next window. */
    for (int i = 0; i < WATCHDOG_SUBSYSTEM_COUNT; i++) {
        checkins[i] = false;
    }
}

void watchdog_check_in(uint8_t subsystem_id)
{
    if (subsystem_id < WATCHDOG_SUBSYSTEM_COUNT) {
        checkins[subsystem_id] = true;
    }
}
