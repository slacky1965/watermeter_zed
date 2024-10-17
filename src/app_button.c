#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define DEBOUNCE_BUTTON     16                          /* number of polls for debounce       */
#define COUNT_FACTORY_RESET 5                           /* number of clicks for factory reset */

void init_button() {

    memset(&g_watermeterCtx.button, 0, sizeof(button_t));

    g_watermeterCtx.button.debounce = 1;
    g_watermeterCtx.button.released_time = clock_time();

}

void button_handler() {

    if (!drv_gpio_read(BUTTON1)) {
        if (g_watermeterCtx.button.debounce != DEBOUNCE_BUTTON) {
            g_watermeterCtx.button.debounce++;
            if (g_watermeterCtx.button.debounce == DEBOUNCE_BUTTON) {
                g_watermeterCtx.button.pressed = true;
                g_watermeterCtx.button.pressed_time = clock_time();
                if (!clock_time_exceed(g_watermeterCtx.button.released_time, TIMEOUT_TICK_1SEC)) {
                    g_watermeterCtx.button.counter++;
                } else {
                    g_watermeterCtx.button.counter = 1;
                }
            }
        }
    } else {
        if (g_watermeterCtx.button.debounce != 1) {
            g_watermeterCtx.button.debounce--;
            if (g_watermeterCtx.button.debounce == 1 && g_watermeterCtx.button.pressed) {
                g_watermeterCtx.button.released = true;
                g_watermeterCtx.button.released_time = clock_time();
            }
        }
    }

    if (g_watermeterCtx.button.pressed && g_watermeterCtx.button.released) {
        g_watermeterCtx.button.pressed = g_watermeterCtx.button.released = false;
        if (clock_time_exceed(g_watermeterCtx.button.pressed_time, TIMEOUT_TICK_10SEC)) {
            /* long pressed > 10 sec. */
            /* TODO: full clean (factory reset and clean config) */
            light_blink_start(3, 30, 250);
#if UART_PRINTF_MODE
            printf("Full reset of the device!\r\n");
#endif
            TL_ZB_TIMER_SCHEDULE(delayedFullResetCb, NULL, TIMEOUT_1SEC);
        } else if (clock_time_exceed(g_watermeterCtx.button.pressed_time, TIMEOUT_TICK_5SEC)) {
            /* long pressed > 5 sec. */
            light_blink_start(3, 30, 250);
#if UART_PRINTF_MODE
            printf("MCU reset!\r\n");
#endif
            TL_ZB_TIMER_SCHEDULE(delayedMcuResetCb, NULL, TIMEOUT_1SEC);
        } else { /* short pressed < 5 sec. */
            light_blink_start(1, 30, 30);
            if (g_watermeterCtx.timerPollRateEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
            }
            if (!watermeter_config.new_ota) {
                zb_setPollRate(g_watermeterCtx.short_poll);
                g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_2MIN);
            }

            if (!g_watermeterCtx.timerForcedReportEvt) {
                g_watermeterCtx.timerForcedReportEvt = TL_ZB_TIMER_SCHEDULE(forcedReportCb, NULL, TIMEOUT_5SEC);
            }
        }
    } else if (!g_watermeterCtx.button.pressed) {
        if (clock_time_exceed(g_watermeterCtx.button.released_time, TIMEOUT_TICK_1SEC)) {
            if (g_watermeterCtx.button.counter == COUNT_FACTORY_RESET) {
                light_blink_start(3, 30, 250);
                g_watermeterCtx.button.counter = 0;
#if UART_PRINTF_MODE
                printf("Factory reset!\r\n");
#endif
                TL_ZB_TIMER_SCHEDULE(delayedFactoryResetCb, NULL, TIMEOUT_1SEC);
            } else {
                g_watermeterCtx.button.counter = 0;
            }
        }

    }
}

u8 button_idle() {
    if ((g_watermeterCtx.button.debounce != 1 && g_watermeterCtx.button.debounce != DEBOUNCE_BUTTON)
            || g_watermeterCtx.button.pressed
            || g_watermeterCtx.button.counter) {
        return true;
    }
    return false;
}
