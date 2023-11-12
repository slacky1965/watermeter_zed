#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

void led_on(uint32_t pin)
{
    drv_gpio_write(pin, LED_ON);
}

void led_off(uint32_t pin)
{
    drv_gpio_write(pin, LED_OFF);
}

void light_on(void)
{
    led_on(LED1);
}

void light_off(void)
{
    led_off(LED1);
}

void light_init(void)
{
    led_off(LED1);

}

int32_t zclLightTimerCb(void *arg)
{
    uint32_t interval = 0;

    if(g_watermeterCtx.sta == g_watermeterCtx.oriSta){
        g_watermeterCtx.times--;
        if(g_watermeterCtx.times <= 0){
            g_watermeterCtx.timerLedEvt = NULL;
            return -1;
        }
    }

    g_watermeterCtx.sta = !g_watermeterCtx.sta;
    if(g_watermeterCtx.sta){
        light_on();
        interval = g_watermeterCtx.ledOnTime;
    }else{
        light_off();
        interval = g_watermeterCtx.ledOffTime;
    }

    return interval;
}

void light_blink_start(uint8_t times, uint16_t ledOnTime, uint16_t ledOffTime)
{
    uint32_t interval = 0;
    g_watermeterCtx.times = times;

    if(!g_watermeterCtx.timerLedEvt){
        if(g_watermeterCtx.oriSta){
            light_off();
            g_watermeterCtx.sta = 0;
            interval = ledOffTime;
        }else{
            light_on();
            g_watermeterCtx.sta = 1;
            interval = ledOnTime;
        }
        g_watermeterCtx.ledOnTime = ledOnTime;
        g_watermeterCtx.ledOffTime = ledOffTime;

        g_watermeterCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(zclLightTimerCb, NULL, interval);
    }
}

void light_blink_stop(void)
{
    if(g_watermeterCtx.timerLedEvt){
        TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerLedEvt);

        g_watermeterCtx.times = 0;
        if(g_watermeterCtx.oriSta){
            light_on();
        }else{
            light_off();
        }
    }
}
