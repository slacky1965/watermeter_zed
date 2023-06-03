#include <stdint.h>
#include "tl_common.h"

#include "pulse.h"
#include "cfg.h"
//#include "app.h"
//#include "log.h"
//#include "ble.h"

#define BIT_COUNT   32768             /* number of polls for debounce */

#define ST_CLOSE    1
#define ST_OPEN     0
#define ST_PRESSED  1
#define ST_RELEASE  0

static water_counter_t hot_counter;
static water_counter_t cold_counter;

u32 check_counter_overflow(u32 check_count) {
    u32 count;

    if (check_count >= COUNTERS_OVERFLOW) {
        count = check_count - COUNTERS_OVERFLOW;
#if UART_PRINTF_MODE
        printf("Counter overflow\r\n");
#endif /* UART_PRINTF_MODE */
    } else {
        count = check_count;
    }

    return count;
}

void init_pulse() {

    hot_counter.status = ST_OPEN;
    hot_counter.counter = 0;
    if (!drv_gpio_read(HOT_GPIO)) {
        hot_counter.bit = BIT_COUNT;
        hot_counter.pressed = ST_PRESSED;
    } else {
        hot_counter.bit = 1;
        hot_counter.pressed = ST_RELEASE;
    }

    cold_counter.status = ST_OPEN;
    cold_counter.counter = 0;
    if (!drv_gpio_read(COLD_GPIO)) {
        cold_counter.bit = BIT_COUNT;
        cold_counter.pressed = ST_PRESSED;
    } else {
        cold_counter.bit = 1;
        cold_counter.pressed = ST_RELEASE;
    }

}

static void water_counters() {

    if (!drv_gpio_read(HOT_GPIO)) {
        if (hot_counter.bit == BIT_COUNT) {
            hot_counter.status = ST_CLOSE;
        } else {
            hot_counter.bit <<= 1;
        }
    } else {
        if (hot_counter.bit == 1) {
            hot_counter.status = ST_OPEN;
        } else {
            hot_counter.bit >>= 1;
        }
    }
    if (hot_counter.status) {

        if (!hot_counter.pressed) {
            hot_counter.pressed = ST_PRESSED;
            hot_counter.counter++;
        }
    } else {
        hot_counter.pressed = ST_RELEASE;
    }

    if (!drv_gpio_read(COLD_GPIO)) {
        if (cold_counter.bit == BIT_COUNT) {
            cold_counter.status = ST_CLOSE;
        } else {
            cold_counter.bit <<= 1;
        }
    } else {
        if (cold_counter.bit == 1) {
            cold_counter.status = ST_OPEN;
        } else {
            cold_counter.bit >>= 1;
        }
    }
    if (cold_counter.status) {

        if (!cold_counter.pressed) {
            cold_counter.pressed = ST_PRESSED;
            cold_counter.counter++;
        }
    } else {
        cold_counter.pressed = ST_RELEASE;
    }


}

u8 counters_handler() {

    u8 save_config = false;

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_10K);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_10K);
#endif

    do {
        water_counters();
        sleep_ms(1);
    } while ((hot_counter.bit != BIT_COUNT && hot_counter.bit != 1) ||
             (hot_counter.bit == BIT_COUNT && hot_counter.status != ST_CLOSE) ||
             (hot_counter.bit == 1 && hot_counter.status != ST_OPEN) ||
             (cold_counter.bit != BIT_COUNT && cold_counter.bit != 1) ||
             (cold_counter.bit == BIT_COUNT && cold_counter.status != ST_CLOSE) ||
             (cold_counter.bit == 1 && cold_counter.status != ST_OPEN));

    if (hot_counter.counter) {
        save_config = true;
        /* detect hot counter overflow */
        watermeter_config.counter_hot_water =
                check_counter_overflow(watermeter_config.counter_hot_water +
                (watermeter_config.liters_per_pulse * hot_counter.counter));
        hot_counter.counter = 0;
#if UART_PRINTF_MODE
        printf("hot counter - %d\r\n", watermeter_config.counter_hot_water);
#endif /* UART_PRINTF_MODE */
    }

    if (cold_counter.counter) {
        save_config = true;
        /* detect cold counter overflow */
        watermeter_config.counter_cold_water =
                check_counter_overflow(watermeter_config.counter_cold_water +
                (watermeter_config.liters_per_pulse * cold_counter.counter));
        cold_counter.counter = 0;
#if UART_PRINTF_MODE
        printf("cold counter - %d\r\n", watermeter_config.counter_cold_water);
#endif /* UART_PRINTF_MODE */
    }

    if (save_config) {
        write_config();
    }

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_1M);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_1M);
#endif

    return save_config;
}

