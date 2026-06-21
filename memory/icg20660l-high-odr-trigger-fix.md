---
name: icg20660l-high-odr-trigger-fix
description: Removed GPIO interrupt disable/re-enable in trigger path and increased thread stack to support ODR > 200 Hz.
metadata:
  type: project
---

The ICG20660L driver was observed to stop producing interrupts after a few seconds when `odr` was set above 200 Hz. At 200 Hz it ran stable overnight.

Likely cause: the original trigger path disabled the GPIO interrupt in the ISR and re-enabled it after the user handler. At high ODR this creates a window where edges can be lost, and repeated `gpio_pin_interrupt_configure_dt` calls may fail or leave the line unarmed.

Changes in `zephyr/drivers/sensor/tdk/icg20660l/icg20660l_trigger.c`:
- Removed `gpio_pin_interrupt_configure_dt(..., GPIO_INT_DISABLE)` from `icg20660l_gpio_callback`.
- Removed `gpio_pin_interrupt_configure_dt(..., GPIO_INT_EDGE_TO_ACTIVE)` from `icg20660l_handle_data_ready`.
- The GPIO edge interrupt now stays armed; the driver's semaphore absorbs back-to-back edges.
- Kept edge-triggered configuration in `icg20660l_trigger_set()` (enable/disable only when registering/unregistering the handler).

Changes in `zephyr/drivers/sensor/tdk/icg20660l/Kconfig`:
- Default `ICG20660L_THREAD_STACK_SIZE` raised from 1024 to 2048 bytes to handle high-rate context switching and I2C stack usage.

**Why:** Disabling and re-arming the GPIO interrupt on every sample adds latency and can drop edges when the sensor outpaces the handler. Leaving it armed lets the semaphore queue absorb bursts.

**How to apply:** Rebuild and test with `odr = <500>` or higher after a power cycle of the board/ST-Link.

Related: [[icg20660l-dt-odr-support]]
