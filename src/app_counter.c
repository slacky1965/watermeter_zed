#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define DEBOUNCE_COUNTER    56                          /* number of polls for debounce       */

static water_counter_t hot_counter;
static water_counter_t cold_counter;

uint32_t check_counter_overflow(uint32_t check_count) {
    uint32_t count;

    if (check_count >= COUNTERS_OVERFLOW) {
        count = check_count - COUNTERS_OVERFLOW;
#if UART_PRINTF_MODE && DEBUG_COUNTER
        printf("Counter overflow. Old value: %d, new value: %d\r\n", check_count, count);
#endif /* UART_PRINTF_MODE */
    } else {
        count = check_count;
    }

    return count;
}

/* reg_deep_sleep is                                                */
/* 0x01: input level on HOT_PIN before deep sleep                   */
/* 0x02: input level on COLD_PIN before deep sleep                  */
/* 0x04: 1 - deep sleep, watchdog or soft_reset,  0 - new start MCU */

void init_counters() {

    uint8_t reg_deep_sleep = analog_read(CHK_DEEP_SLEEP);

    hot_counter.counter = 0;
    cold_counter.counter = 0;

    if (reg_deep_sleep & 0x04) {
        /* wakeup from deep sleep */

        if (!drv_gpio_read(HOT_GPIO)) {
            hot_counter.debounce = DEBOUNCE_COUNTER;
            if (reg_deep_sleep & 0x01) {
                hot_counter.counter++;
            }
        } else {
            hot_counter.debounce = 1;
        }

        if (!drv_gpio_read(COLD_GPIO)) {
            cold_counter.debounce = DEBOUNCE_COUNTER;
            if (reg_deep_sleep & 0x02) {
                cold_counter.counter++;
            }
        } else {
            cold_counter.debounce = 1;
        }

    } else {
        /* new start MCU */
        if (!drv_gpio_read(HOT_GPIO)) {
            hot_counter.debounce = DEBOUNCE_COUNTER;
        } else {
            hot_counter.debounce = 1;
        }

        if (!drv_gpio_read(COLD_GPIO)) {
            cold_counter.debounce = DEBOUNCE_COUNTER;
        } else {
            cold_counter.debounce = 1;
        }
    }

    reg_deep_sleep = 0;
    analog_write(CHK_DEEP_SLEEP, reg_deep_sleep);
}

uint8_t counters_handler() {

    uint8_t save_config = false;
    uint64_t water_counter = 0;

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_10K);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_10K);
#endif

    if (!drv_gpio_read(HOT_GPIO)) {
        if (hot_counter.debounce != DEBOUNCE_COUNTER) {
            hot_counter.debounce++;
            if (hot_counter.debounce == DEBOUNCE_COUNTER) {
                hot_counter.counter++;
            }
        }
    } else {
        if (hot_counter.debounce != 1) {
            hot_counter.debounce--;
        }
    }

    if (!drv_gpio_read(COLD_GPIO)) {
        if (cold_counter.debounce != DEBOUNCE_COUNTER) {
            cold_counter.debounce++;
            if (cold_counter.debounce == DEBOUNCE_COUNTER) {
                cold_counter.counter++;
            }
        }
    } else {
        if (cold_counter.debounce != 1) {
            cold_counter.debounce--;
        }
    }

    if (hot_counter.counter) {
        save_config = true;
        /* detect hot counter overflow */
        watermeter_config.counter_hot_water =
                check_counter_overflow(watermeter_config.counter_hot_water +
                (watermeter_config.liters_per_pulse * hot_counter.counter));
        hot_counter.counter = 0;
#if UART_PRINTF_MODE && DEBUG_COUNTER
        printf("hot counter - %d\r\n", watermeter_config.counter_hot_water);
#endif /* UART_PRINTF_MODE */
        water_counter = watermeter_config.counter_hot_water & 0xffffffffffff;
        zcl_setAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD, (uint8_t*)&water_counter);
    }

    if (cold_counter.counter) {
        save_config = true;
        /* detect cold counter overflow */
        watermeter_config.counter_cold_water =
                check_counter_overflow(watermeter_config.counter_cold_water +
                (watermeter_config.liters_per_pulse * cold_counter.counter));
        cold_counter.counter = 0;
#if UART_PRINTF_MODE && DEBUG_COUNTER
        printf("cold counter - %d\r\n", watermeter_config.counter_cold_water);
#endif /* UART_PRINTF_MODE */
        water_counter = watermeter_config.counter_cold_water & 0xffffffffffff;
        zcl_setAttrVal(WATERMETER_ENDPOINT2, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD, (uint8_t*)&water_counter);
    }

    if (save_config) {
        write_config();
        zb_setPollRate(g_watermeterCtx.short_poll);
        if (g_watermeterCtx.timerPollRateEvt) {
            TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
        }
        g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_30SEC);
    }

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_1M);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_1M);
#endif

    if (counters_idle()) {
        sleep_ms(1);
    }

    return save_config;
}

uint8_t counters_idle() {
    if ((hot_counter.debounce != 1 && hot_counter.debounce != DEBOUNCE_COUNTER) ||
        (cold_counter.debounce != 1 && cold_counter.debounce != DEBOUNCE_COUNTER)) {
        return true;
    }
    return false;
}
