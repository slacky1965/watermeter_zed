#include "tl_common.h"
#include "zcl_include.h"

#include "app_main.h"

#define DEBOUNCE_BUTTON     40                          /* usec for debounce                  */
#define COUNT_FACTORY_RESET 5                           /* number of clicks for factory reset */

void init_button() {

    memset(&g_appCtx.button, 0, sizeof(button_t));

    g_appCtx.button.debounce = 1;
    g_appCtx.button.released_time = clock_time();

}

void button_handler() {

    if (!drv_gpio_read(BUTTON1)) {
        sleep_us(DEBOUNCE_BUTTON);
        if (!drv_gpio_read(BUTTON1) && !g_appCtx.button.pressed) {
            //printf("button pressed\r\n");
            g_appCtx.button.pressed = true;
            g_appCtx.button.pressed_time = clock_time();
            if (!clock_time_exceed(g_appCtx.button.released_time, TIMEOUT_TICK_1SEC)) {
                g_appCtx.button.counter++;
            } else {
                g_appCtx.button.counter = 1;
            }
        }
    } else {
        sleep_us(DEBOUNCE_BUTTON);
        if (drv_gpio_read(BUTTON1) && !g_appCtx.button.released) {
            if (g_appCtx.button.pressed) {
                //printf("button released\r\n");
                g_appCtx.button.released = true;
                g_appCtx.button.released_time = clock_time();
            }
        }
    }

    if (g_appCtx.button.pressed && g_appCtx.button.released) {
        g_appCtx.button.pressed = g_appCtx.button.released = false;
        if (clock_time_exceed(g_appCtx.button.pressed_time, TIMEOUT_TICK_10SEC)) {
            /* long pressed > 10 sec. */
            /* TODO: full clean (factory reset and clean config) */
            light_blink_start(3, 30, 250);
#if UART_PRINTF_MODE
            printf("Full reset of the device!\r\n");
#endif
            TL_ZB_TIMER_SCHEDULE(delayedFullResetCb, NULL, TIMEOUT_1SEC);
        } else if (clock_time_exceed(g_appCtx.button.pressed_time, TIMEOUT_TICK_5SEC)) {
            /* long pressed > 5 sec. */
            light_blink_start(3, 30, 250);
#if UART_PRINTF_MODE
            printf("MCU reset!\r\n");
#endif
            TL_ZB_TIMER_SCHEDULE(delayedMcuResetCb, NULL, TIMEOUT_1SEC);
        } else { /* short pressed < 5 sec. */
            light_blink_start(1, 30, 30);
            if (g_appCtx.timerPollRateEvt) {
                TL_ZB_TIMER_CANCEL(&g_appCtx.timerPollRateEvt);
            }
//            if (!watermeter_config.new_ota) {
//                zb_setPollRate(g_appCtx.short_poll);
//                g_appCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_2MIN);
//            }
//
//            if (!g_appCtx.timerForcedReportEvt) {
//                g_appCtx.timerForcedReportEvt = TL_ZB_TIMER_SCHEDULE(forcedReportCb, NULL, TIMEOUT_5SEC);
//            }
        }
    } else if (!g_appCtx.button.pressed) {
        if (clock_time_exceed(g_appCtx.button.released_time, TIMEOUT_TICK_1SEC)) {
            if (g_appCtx.button.counter == COUNT_FACTORY_RESET) {
                light_blink_start(3, 30, 250);
                g_appCtx.button.counter = 0;
#if UART_PRINTF_MODE
                printf("Factory reset!\r\n");
#endif
                TL_ZB_TIMER_SCHEDULE(delayedFactoryResetCb, NULL, TIMEOUT_1SEC);
            } else {
                g_appCtx.button.counter = 0;
            }
        }

    }
}

u8 button_idle() {
    if (g_appCtx.button.pressed || g_appCtx.button.counter) {
        return true;
    }
    return false;
}
