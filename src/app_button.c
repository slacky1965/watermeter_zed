#include "watermeter.h"

static void buttonKeepPressed(uint8_t btNum) {
    g_watermeterCtx.button[btNum-1].state = APP_FACTORY_NEW_DOING;
    g_watermeterCtx.button[btNum-1].ctn = 0;

    if (g_watermeterCtx.timerPollRateEvt) {
        TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
    }
    zb_setPollRate(g_watermeterCtx.short_poll);
    g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_1MIN);


    if(btNum == VK_SW1) {
#if UART_PRINTF_MODE && DEBUG_BUTTON
        printf("The button was keep pressed for 5 seconds. MCU reset!!!\r\n");
#endif

        light_blink_stop();
        light_blink_start(1, 1500, 1);
        TL_ZB_TIMER_SCHEDULE(delayedMcuResetCb, NULL, TIMEOUT_1SEC);
//        delayedFactoryResetCb(NULL);
    }
}

static void buttonCheckCommand(uint8_t btNum) {
    g_watermeterCtx.button[btNum-1].state = APP_STATE_NORMAL;

    if (g_watermeterCtx.timerPollRateEvt) {
        TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
    }
    zb_setPollRate(g_watermeterCtx.short_poll);
    g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_20SEC);

    if (g_watermeterCtx.button[btNum-1].ctn == 1) {
#if UART_PRINTF_MODE && DEBUG_BUTTON
        printf("Button push 1 time\r\n");
#endif
        batteryCb(NULL);
        if (!g_watermeterCtx.timerForcedReportEvt) {
            g_watermeterCtx.timerForcedReportEvt = TL_ZB_TIMER_SCHEDULE(forcedReportCb, NULL, TIMEOUT_1SEC);
        }
    } else if (g_watermeterCtx.button[btNum-1].ctn == 5) {
#if UART_PRINTF_MODE && DEBUG_BUTTON
        printf("Button push 5 times\r\n");
#endif
        TL_ZB_TIMER_SCHEDULE(delayedFactoryResetCb, NULL, TIMEOUT_1SEC);
//        delayedFactoryResetCb(NULL);
    }

    g_watermeterCtx.button[btNum-1].ctn = 0;
}


void keyScan_keyPressedCB(kb_data_t *kbEvt) {

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;
    uint8_t keyCode = kbEvt->keycode[0];

    if(keyCode != 0xff) {
        g_watermeterCtx.button[keyCode-1].pressed_time = clock_time();
        g_watermeterCtx.button[keyCode-1].state = APP_FACTORY_NEW_SET_CHECK;
        g_watermeterCtx.button[keyCode-1].ctn++;
        light_blink_start(1, 100, 1);

        if (keyCode == VK_SW1) {
            if(zb_isDeviceJoinedNwk()) {
                fillIASAddress(&dstEpInfo);

                zcl_getAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (uint8_t*)&statusChangeNotification.zoneStatus);
                zcl_getAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                statusChangeNotification.zoneStatus |= ZONE_STATUS_TEST;
                zcl_setAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (uint8_t*)&statusChangeNotification.zoneStatus);
                statusChangeNotification.extStatus = 0;
                statusChangeNotification.delay = 0;

                zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);

            }
        }
    }
}


void keyScan_keyReleasedCB(uint8_t keyCode) {

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;

    if (keyCode != 0xff) {
        g_watermeterCtx.button[keyCode-1].released_time = clock_time();
        g_watermeterCtx.button[keyCode-1].state = APP_STATE_RELEASE;

        if (keyCode == VK_SW1) {
            if(zb_isDeviceJoinedNwk()) {
                fillIASAddress(&dstEpInfo);

                zcl_getAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (uint8_t*)&statusChangeNotification.zoneStatus);
                zcl_getAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                statusChangeNotification.zoneStatus &= ~ZONE_STATUS_TEST;
                zcl_setAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (uint8_t*)&statusChangeNotification.zoneStatus);
                statusChangeNotification.extStatus = 0;
                statusChangeNotification.delay = 0;

                zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);

            }
        }

    }
}

void button_handler(void) {
    static uint8_t valid_keyCode = 0xff;

    for (uint8_t i = 0; i < MAX_BUTTON_NUM; i++) {
        if (g_watermeterCtx.button[i].state == APP_FACTORY_NEW_SET_CHECK) {
            if(clock_time_exceed(g_watermeterCtx.button[i].pressed_time, TIMEOUT_TICK_5SEC)) {
                buttonKeepPressed(i+1);
            }
        }

        if (g_watermeterCtx.button[i].state == APP_STATE_RELEASE) {
            if(clock_time_exceed(g_watermeterCtx.button[i].released_time, TIMEOUT_TICK_250MS)) {
                buttonCheckCommand(i+1);
            }

        }
    }

    if(kb_scan_key(0, 1)){
        if(kb_event.cnt){
            g_watermeterCtx.keyPressed = 1;
            keyScan_keyPressedCB(&kb_event);
            if(kb_event.cnt == 1){
                valid_keyCode = kb_event.keycode[0];
            }
        }else{
            keyScan_keyReleasedCB(valid_keyCode);
            valid_keyCode = 0xff;
            g_watermeterCtx.keyPressed = 0;
        }
    }
}

uint8_t button_idle() {

    if (g_watermeterCtx.keyPressed) {
        return true;
    }

    for (uint8_t i = 0; i < MAX_BUTTON_NUM; i++) {
        if (g_watermeterCtx.button[i].ctn) return true;
    }

    return false;
}


