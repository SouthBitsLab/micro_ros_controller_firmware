/**
 * @file state_machine.c
 * @brief Robot lifecycle state machine implementation.
 */

#include "app/state_machine.h"
#include "safety/fault.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(state_machine, LOG_LEVEL_DBG);

static struct k_spinlock state_lock;
static enum robot_state current_state;

void state_machine_init(void)
{
    k_spinlock_key_t key = k_spin_lock(&state_lock);
    current_state = ROBOT_STATE_BOOT;
    k_spin_unlock(&state_lock, key);
}

int state_machine_transition(enum robot_state new_state)
{
    int ret = 0;
    k_spinlock_key_t key = k_spin_lock(&state_lock);
    enum robot_state old_state = current_state;

    /* ESTOP overrides any state. */
    if (new_state == ROBOT_STATE_ESTOP) {
        current_state = ROBOT_STATE_ESTOP;
        goto unlock;
    }

    switch (old_state) {
    case ROBOT_STATE_BOOT:
        if (new_state == ROBOT_STATE_WAIT_AGENT) {
            current_state = new_state;
        } else {
            ret = -EPERM;
        }
        break;

    case ROBOT_STATE_WAIT_AGENT:
        if (new_state == ROBOT_STATE_IDLE || new_state == ROBOT_STATE_FAULT) {
            current_state = new_state;
        } else {
            ret = -EPERM;
        }
        break;

    case ROBOT_STATE_IDLE:
        if (new_state == ROBOT_STATE_ACTIVE || new_state == ROBOT_STATE_FAULT ||
            new_state == ROBOT_STATE_WAIT_AGENT) {
            current_state = new_state;
        } else {
            ret = -EPERM;
        }
        break;

    case ROBOT_STATE_ACTIVE:
        if (new_state == ROBOT_STATE_IDLE || new_state == ROBOT_STATE_FAULT) {
            current_state = new_state;
        } else {
            ret = -EPERM;
        }
        break;

    case ROBOT_STATE_FAULT:
        /* Fault requires explicit reset to BOOT. */
        if (new_state == ROBOT_STATE_BOOT) {
            current_state = new_state;
            fault_init();
        } else {
            ret = -EPERM;
        }
        break;

    case ROBOT_STATE_ESTOP:
        /* E-stop requires reset to BOOT. */
        if (new_state == ROBOT_STATE_BOOT) {
            current_state = new_state;
            fault_init();
        } else {
            ret = -EPERM;
        }
        break;

    default:
        ret = -EINVAL;
        break;
    }

unlock:
    k_spin_unlock(&state_lock, key);

    if (ret == 0) {
        LOG_INF("State: %d -> %d", old_state, current_state);
    }

    return ret;
}

enum robot_state state_machine_get(void)
{
    k_spinlock_key_t key = k_spin_lock(&state_lock);
    enum robot_state state = current_state;
    k_spin_unlock(&state_lock, key);

    return state;
}

bool state_machine_motion_allowed(void)
{
    return state_machine_get() == ROBOT_STATE_ACTIVE;
}
