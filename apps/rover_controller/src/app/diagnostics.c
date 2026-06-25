/
 * @file diagnostics.c
 * @brief Diagnostics aggregation implementation.
 */

#include "app/diagnostics.h"
#include "safety/fault.h"
#include "app/state_machine.h"
#include <zephyr/kernel.h>

static struct k_spinlock diag_lock;
static struct diagnostic_flags diag_flags;

void diagnostics_init(void)
{
    k_spinlock_key_t key = k_spin_lock(&diag_lock);
    diag_flags.imu_ok = false;
    diag_flags.gnss_ok = false;
    diag_flags.battery_ok = false;
    diag_flags.motors_ok = false;
    diag_flags.agent_connected = false;
    diag_flags.estop_active = false;
    diag_flags.cmd_timeout = false;
    diag_flags.undervoltage = false;
    diag_flags.overcurrent = false;
    k_spin_unlock(&diag_lock, key);
}

void diagnostics_update(void)
{
    uint32_t faults = fault_get();

    k_spinlock_key_t key = k_spin_lock(&diag_lock);
    diag_flags.cmd_timeout = fault_is_set(FAULT_BIT_CMD_TIMEOUT);
    diag_flags.agent_connected = !fault_is_set(FAULT_BIT_AGENT_LOST);
    diag_flags.undervoltage = fault_is_set(FAULT_BIT_UNDERVOLTAGE);
    diag_flags.overcurrent = fault_is_set(FAULT_BIT_OVERCURRENT);
    diag_flags.motors_ok = !fault_is_set(FAULT_BIT_MOTOR_FEEDBACK);
    diag_flags.estop_active = fault_is_set(FAULT_BIT_ESTOP);

    /* TODO: set imu_ok, gnss_ok, battery_ok from HAL health checks. */
    (void)faults;
    k_spin_unlock(&diag_lock, key);
}

int diagnostics_get(struct diagnostic_flags *out)
{
    if (out == NULL) {
        return -EINVAL;
    }

    k_spinlock_key_t key = k_spin_lock(&diag_lock);
    *out = diag_flags;
    k_spin_unlock(&diag_lock, key);

    return 0;
}
