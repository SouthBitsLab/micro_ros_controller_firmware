/**
 * @file fault.c
 * @brief Fault flag implementation.
 */

#include "safety/fault.h"
#include <zephyr/kernel.h>

static uint32_t fault_word;
static struct k_spinlock fault_lock;

void fault_init(void)
{
    k_spinlock_key_t key = k_spin_lock(&fault_lock);
    fault_word = 0;
    k_spin_unlock(&fault_lock, key);
}

void fault_set(uint32_t bits)
{
    k_spinlock_key_t key = k_spin_lock(&fault_lock);
    fault_word |= bits;
    k_spin_unlock(&fault_lock, key);
}

void fault_clear(uint32_t bits)
{
    k_spinlock_key_t key = k_spin_lock(&fault_lock);
    fault_word &= ~bits;
    k_spin_unlock(&fault_lock, key);
}

uint32_t fault_get(void)
{
    k_spinlock_key_t key = k_spin_lock(&fault_lock);
    uint32_t value = fault_word;
    k_spin_unlock(&fault_lock, key);

    return value;
}

bool fault_is_set(uint32_t bits)
{
    return (fault_get() & bits) != 0;
}

bool fault_motion_inhibited(void)
{
    const uint32_t motion_faults =
        FAULT_BIT_CMD_TIMEOUT |
        FAULT_BIT_AGENT_LOST |
        FAULT_BIT_UNDERVOLTAGE |
        FAULT_BIT_ESTOP |
        FAULT_BIT_WATCHDOG;

    return fault_is_set(motion_faults);
}
