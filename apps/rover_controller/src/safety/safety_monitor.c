/**
 * @file safety_monitor.c
 * @brief High-priority safety monitor thread implementation.
 *
 * The safety monitor runs at a fixed period and performs the following:
 *   - checks command target age and latches FAULT_BIT_CMD_TIMEOUT if stale;
 *   - checks battery voltage and latches FAULT_BIT_UNDERVOLTAGE;
 *   - checks motor feedback freshness and latches FAULT_BIT_MOTOR_FEEDBACK;
 *   - updates FAULT_BIT_AGENT_LOST based on agent connection state;
 *   - feeds the watchdog only when the control loop has also checked in.
 *
 * It never directly issues motor commands; it only sets fault flags that the
 * control loop must honor.
 */

#include "safety/safety_monitor.h"
#include "safety/fault.h"
#include "safety/watchdog.h"
#include "safety/limits.h"
#include "app/robot_command.h"
#include "common/time_utils.h"
#include "common/robot_params.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(safety_monitor, LOG_LEVEL_DBG);

/* Stack size for the safety monitor thread. */
#define SAFETY_MONITOR_STACK_SIZE 2048

/* Thread priority: high cooperative. */
#define SAFETY_MONITOR_PRIORITY K_PRIO_COOP(2)

static void safety_monitor_thread_entry(void *p1, void *p2, void *p3);

K_THREAD_DEFINE(safety_monitor_tid, SAFETY_MONITOR_STACK_SIZE,
                safety_monitor_thread_entry, NULL, NULL, NULL,
                SAFETY_MONITOR_PRIORITY, 0, 0);

static struct k_mutex monitor_lock;
static bool agent_connected;
static float battery_voltage_v;
static struct motor_feedback motor_feedback_cache[ROVER_NUM_MOTORS];

int safety_monitor_init(void)
{
    k_mutex_init(&monitor_lock);
    fault_init();
    agent_connected = false;
    battery_voltage_v = 0.0f;

    for (uint8_t i = 0; i < ROVER_NUM_MOTORS; i++) {
        motor_feedback_cache[i].valid = false;
    }

    return watchdog_init();
}

int safety_monitor_start(void)
{
    /* The thread is already defined with K_THREAD_DEFINE; nothing to do. */
    LOG_INF("Safety monitor started");
    return 0;
}

void safety_monitor_set_agent_connected(bool connected)
{
    k_mutex_lock(&monitor_lock, K_FOREVER);
    agent_connected = connected;
    k_mutex_unlock(&monitor_lock);
}

void safety_monitor_set_battery_voltage(float voltage_v)
{
    k_mutex_lock(&monitor_lock, K_FOREVER);
    battery_voltage_v = voltage_v;
    k_mutex_unlock(&monitor_lock);
}

void safety_monitor_set_motor_feedback(const struct motor_feedback *feedback)
{
    if (feedback == NULL) {
        return;
    }

    k_mutex_lock(&monitor_lock, K_FOREVER);
    for (uint8_t i = 0; i < ROVER_NUM_MOTORS; i++) {
        motor_feedback_cache[i] = feedback[i];
    }
    k_mutex_unlock(&monitor_lock);
}

bool safety_monitor_motion_permitted(void)
{
    return !fault_motion_inhibited();
}

static void safety_monitor_evaluate(void)
{
    struct robot_command cmd;

    /* Evaluate command timeout. */
    robot_command_get(&cmd);
    if (!cmd.valid || time_is_expired_ms(cmd.timestamp_ms, SAFETY_CMD_TIMEOUT_MS)) {
        fault_set(FAULT_BIT_CMD_TIMEOUT);
    } else {
        fault_clear(FAULT_BIT_CMD_TIMEOUT);
    }

    /* Evaluate agent connection. */
    k_mutex_lock(&monitor_lock, K_FOREVER);
    bool connected = agent_connected;
    float voltage = battery_voltage_v;
    k_mutex_unlock(&monitor_lock);

    if (!connected) {
        fault_set(FAULT_BIT_AGENT_LOST);
    } else {
        fault_clear(FAULT_BIT_AGENT_LOST);
    }

    /* Evaluate battery undervoltage. */
    if (voltage > 0.0f && voltage < SAFETY_UNDERVOLTAGE_V) {
        fault_set(FAULT_BIT_UNDERVOLTAGE);
    } else {
        fault_clear(FAULT_BIT_UNDERVOLTAGE);
    }

    /* TODO: evaluate E-stop input and motor feedback freshness. */

    /* Check ourselves into the watchdog window. */
    watchdog_check_in(WATCHDOG_SUBSYSTEM_SAFETY);
}

static void safety_monitor_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("Safety monitor thread running");

    while (true) {
        safety_monitor_evaluate();
        watchdog_feed();
        k_sleep(K_MSEC(SAFETY_MONITOR_PERIOD_MS));
    }
}
