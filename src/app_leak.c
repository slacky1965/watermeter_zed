#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define DEBOUNCE_COUNTER    32                          /* number of polls for debounce       */

static uint8_t waterleak_debounce1 = 1;
static uint8_t waterleak_debounce2 = 1;

static void cmdOnOff(uint8_t endpoint, uint8_t command) {
    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;

//#if FIND_AND_BIND_SUPPORT
//    dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
//#else
//    dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
//    dstEpInfo.dstEp = endpoint;
//    dstEpInfo.dstAddr.shortAddr = 0xfffc;
//#endif

    dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;

    /* command 0x00 - off, 0x01 - on, 0x02 - toggle */

    switch(command) {
        case ZCL_CMD_ONOFF_OFF:
            zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
            break;
        case ZCL_CMD_ONOFF_ON:
            zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
            break;
        case ZCL_CMD_ONOFF_TOGGLE:
            zcl_onOff_toggleCmd(endpoint, &dstEpInfo, FALSE);
            break;
        default:
            break;
    }
}


void fillIASAddress(epInfo_t* pdstEpInfo) {
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
    zcl_onOffSwitchCfg_4_5_Attr_t *onoffCfgAttrs = zcl_onOffSwitchCfgAttrGet();

    if (!drv_gpio_read(WLEAK1_GPIO)) {
        if (waterleak_debounce1 != DEBOUNCE_COUNTER) {
            waterleak_debounce1++;
            if (waterleak_debounce1 == DEBOUNCE_COUNTER) {
#if UART_PRINTF_MODE && DEBUG_WATERLEAK
                printf("Waterleak first channel leaking\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {

                    switch(onoffCfgAttrs->ep4_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

                    switch(onoffCfgAttrs->ep5_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

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
#if UART_PRINTF_MODE && DEBUG_WATERLEAK
                printf("Waterleak first channel clear\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()){
#if UART_PRINTF_MODE && DEBUG_WATERLEAK
                    switch(onoffCfgAttrs->ep4_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

                    switch(onoffCfgAttrs->ep5_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

#endif /* UART_PRINTF_MODE */
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
#if UART_PRINTF_MODE && DEBUG_WATERLEAK
                printf("Waterleak second channel leaking\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {

                    switch(onoffCfgAttrs->ep4_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

                    switch(onoffCfgAttrs->ep5_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

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
#if UART_PRINTF_MODE && DEBUG_WATERLEAK
                printf("Waterleak second channel clear\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()){
#if UART_PRINTF_MODE && DEBUG_WATERLEAK
                    switch(onoffCfgAttrs->ep4_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT4, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }

                    switch(onoffCfgAttrs->ep5_attrs.switchActions) {
                        case ZCL_SWITCH_ACTION_ON_OFF:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_ON);
                            break;
                        case ZCL_SWITCH_ACTION_OFF_ON:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_OFF);
                            break;
                        case ZCL_SWITCH_ACTION_TOGGLE:
                            cmdOnOff(WATERMETER_ENDPOINT5, ZCL_CMD_ONOFF_TOGGLE);
                            break;
                        default:
                            break;
                    }
#endif /* UART_PRINTF_MODE */
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

