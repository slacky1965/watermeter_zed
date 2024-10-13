#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#if PM_ENABLE
/**
 *  @brief Definition for wakeup source and level for PM
 */

static drv_pm_pinCfg_t pin_PmCfg[] = {
    {
        HOT_GPIO,
        PM_WAKEUP_LEVEL
    },
    {
        COLD_GPIO,
        PM_WAKEUP_LEVEL
    },
    {
        WLEAK1_GPIO,
        PM_WAKEUP_LEVEL
    },
    {
        WLEAK2_GPIO,
        PM_WAKEUP_LEVEL
    },
    {
        BUTTON1,
        PM_WAKEUP_LEVEL
    },
};

void app_wakeupPinConfig() {
    drv_pm_wakeupPinConfig(pin_PmCfg, sizeof(pin_PmCfg)/sizeof(drv_pm_pinCfg_t));
}


void app_wakeupPinLevelChange() {
    drv_pm_wakeupPinLevelChange(pin_PmCfg, 4);
}

void app_lowPowerEnter() {

    app_wakeupPinLevelChange();
    drv_pm_lowPowerEnter();
}

int32_t no_joinedCb(void *arg) {

    if (!zb_isDeviceJoinedNwk()) {

        if (tl_stackBusy() || !zb_isTaskDone()) {

//            printf("tl_stackBusy: %s,  zb_isTaskDone: %d\r\n", tl_stackBusy()?"true":"false", zb_isTaskDone());
            return TIMEOUT_1MIN;
        }

#if UART_PRINTF_MODE && DEBUG_PM
        printf("Without network more then 30 minutes! Deep sleep ...\r\n");
#endif

        app_wakeupPinLevelChange();

        apsCleanToStopSecondClock();

        drv_disable_irq();
        rf_paShutDown();
        drv_pm_deepSleep_frameCnt_set(ss_outgoingFrameCntGet());

        set_regDeepSleep();

        drv_pm_longSleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_PAD, 1);
    }

    g_watermeterCtx.timerNoJoinedEvt = NULL;
    return -1;
}


#endif

void set_regDeepSleep() {

    uint8_t reg_deep_sleep = 0;

    /* 0x01: input level on HOT_PIN before deep sleep                   */
    /* 0x02: input level on COLD_PIN before deep sleep                  */
    /* 0x04: 1 - deep sleep, watchdog or soft_reset,  0 - new start MCU */

    reg_deep_sleep = 0x04;
    if (drv_gpio_read(HOT_GPIO))  reg_deep_sleep |= 0x01;
    if (drv_gpio_read(COLD_GPIO)) reg_deep_sleep |= 0x02;

    analog_write(CHK_DEEP_SLEEP, reg_deep_sleep);

}

