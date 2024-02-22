#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define DEBOUNCE_COUNTER    32                          /* number of polls for debounce       */

static uint8_t waterleak_debounce1 = 1;
static uint8_t waterleak_debounce2 = 1;

static void cmdOnOff(uint8_t endpoint, uint8_t waterleak) {
    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;
#if FIND_AND_BIND_SUPPORT
        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#else
    dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
    dstEpInfo.dstEp = endpoint;
    dstEpInfo.dstAddr.shortAddr = 0xfffc;
#endif

    if (waterleak) {
        zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
    } else {
        zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
    }
}


static void fillIASAddress(epInfo_t* pdstEpInfo) {
    u16 len;
    u8 zoneState;

    memset((u8 *)pdstEpInfo, 0, sizeof(epInfo_t));

    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATE, &len, &zoneState);

    pdstEpInfo->dstEp = WATERMETER_ENDPOINT4;
    pdstEpInfo->profileId = HA_PROFILE_ID;

    if (zoneState&ZONE_STATE_ENROLLED) { //device enrolled
        pdstEpInfo->dstAddrMode = APS_LONG_DSTADDR_WITHEP;
        zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_IAS_CIE_ADDR, &len, (u8*)&pdstEpInfo->dstAddr.extAddr);
    }
    else {
        pdstEpInfo->dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
        pdstEpInfo->dstAddr.shortAddr = 0x0000;
    }
}


void waterleak_handler() {

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;

    if (!drv_gpio_read(WLEAK1_GPIO)) {
        if (waterleak_debounce1 != DEBOUNCE_COUNTER) {
            waterleak_debounce1++;
            if (waterleak_debounce1 == DEBOUNCE_COUNTER) {
#if UART_PRINTF_MODE && DEBUG_COUNTER
                printf("Waterleak first channel leaking\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {
                    cmdOnOff(WATERMETER_ENDPOINT4, true);
                    cmdOnOff(WATERMETER_ENDPOINT5, true);
                    fillIASAddress(&dstEpInfo);

                    zoneStatusChangeNoti_t statusChangeNotification;

                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_ALARM1;
                    zcl_setAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT4, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    } else {
        if (waterleak_debounce1 != 1) {
            waterleak_debounce1--;
            if (waterleak_debounce1 == 1) {
#if UART_PRINTF_MODE && DEBUG_COUNTER
                printf("Waterleak first channel clear\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()){
//                    cmdOnOff(WATERMETER_ENDPOINT4, false);
//                    cmdOnOff(WATERMETER_ENDPOINT5, false);
                    fillIASAddress(&dstEpInfo);

                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus &= ~ZONE_STATUS_BIT_ALARM1;
                    zcl_setAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT4, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    }

    if (!drv_gpio_read(WLEAK2_GPIO)) {
        if (waterleak_debounce2 != DEBOUNCE_COUNTER) {
            waterleak_debounce2++;
            if (waterleak_debounce2 == DEBOUNCE_COUNTER) {
#if UART_PRINTF_MODE && DEBUG_COUNTER
                printf("Waterleak second channel leaking\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {
                    cmdOnOff(WATERMETER_ENDPOINT4, true);
                    cmdOnOff(WATERMETER_ENDPOINT5, true);
                    fillIASAddress(&dstEpInfo);

                    zoneStatusChangeNoti_t statusChangeNotification;

                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_ALARM1;
                    zcl_setAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT4, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    } else {
        if (waterleak_debounce2 != 1) {
            waterleak_debounce2--;
            if (waterleak_debounce2 == 1) {
#if UART_PRINTF_MODE && DEBUG_COUNTER
                printf("Waterleak second channel clear\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()){
//                    cmdOnOff(WATERMETER_ENDPOINT4, false);
//                    cmdOnOff(WATERMETER_ENDPOINT5, false);
                    fillIASAddress(&dstEpInfo);

                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus &= ~ZONE_STATUS_BIT_ALARM1;
                    zcl_setAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT4, &dstEpInfo, TRUE, &statusChangeNotification);
                }

            }
        }
    }

    if (waterleak_idle()) {
        sleep_ms(1);
    }
}

uint8_t waterleak_idle() {
    if ((waterleak_debounce1 != 1 && waterleak_debounce1 != DEBOUNCE_COUNTER) ||
        (waterleak_debounce2 != 1 && waterleak_debounce2 != DEBOUNCE_COUNTER)) {
        return true;
    }
    return false;
}

