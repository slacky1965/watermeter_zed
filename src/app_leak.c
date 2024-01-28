#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

static uint8_t leakage1 = false;
static uint8_t leakage2 = false;

static void cmdOnOff(uint8_t endpoint, uint8_t leakage) {
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

//        zcl_onOff_toggleCmd(endpoint, &dstEpInfo, FALSE);
    if (leakage) {
        zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
    } else {
        zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
    }
}




static void fillIASAddress(epInfo_t* pdstEpInfo, uint8_t endpoint) {
    u16 len;
    u8 zoneState;

    memset((u8 *)pdstEpInfo, 0, sizeof(epInfo_t));

    zcl_getAttrVal(endpoint, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATE, &len, &zoneState);

    pdstEpInfo->dstEp = endpoint;
    pdstEpInfo->profileId = HA_PROFILE_ID;

    if (zoneState&ZONE_STATE_ENROLLED) { //device enrolled
        pdstEpInfo->dstAddrMode = APS_LONG_DSTADDR_WITHEP;
        zcl_getAttrVal(endpoint, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_IAS_CIE_ADDR, &len, (u8*)&pdstEpInfo->dstAddr.extAddr);
    }
    else {
        pdstEpInfo->dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
        pdstEpInfo->dstAddr.shortAddr = 0x0000;
    }
}


void leak_handler() {

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;

    if (!drv_gpio_read(LEAK1_GPIO)) {

        if (!leakage1) {
            printf("Leakage on\r\n");
            leakage1 = true;
            if(zb_isDeviceJoinedNwk()) {
                cmdOnOff(WATERMETER_ENDPOINT4, leakage1);
                fillIASAddress(&dstEpInfo, WATERMETER_ENDPOINT4);

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
    } else {
        if (leakage1) {
            printf("Leakage off\r\n");
            leakage1 = false;
            if(zb_isDeviceJoinedNwk()){
                cmdOnOff(WATERMETER_ENDPOINT4, leakage1);
                fillIASAddress(&dstEpInfo, WATERMETER_ENDPOINT4);

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

    if (!drv_gpio_read(LEAK2_GPIO)) {

        if (!leakage2) {
            printf("Leakage2 on\r\n");
            leakage2 = true;
            if(zb_isDeviceJoinedNwk()) {
                cmdOnOff(WATERMETER_ENDPOINT5, leakage2);
                fillIASAddress(&dstEpInfo, WATERMETER_ENDPOINT5);

                zoneStatusChangeNoti_t statusChangeNotification;

                zcl_getAttrVal(WATERMETER_ENDPOINT5, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                zcl_getAttrVal(WATERMETER_ENDPOINT5, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_ALARM1;
                zcl_setAttrVal(WATERMETER_ENDPOINT5, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                statusChangeNotification.extStatus = 0;
                statusChangeNotification.delay = 0;

                zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT5, &dstEpInfo, TRUE, &statusChangeNotification);
            }
        }
    } else {
        if (leakage2) {
            printf("Leakage2 off\r\n");
            leakage2 = false;
            if(zb_isDeviceJoinedNwk()){
                cmdOnOff(WATERMETER_ENDPOINT5, leakage2);
                fillIASAddress(&dstEpInfo, WATERMETER_ENDPOINT5);

                zcl_getAttrVal(WATERMETER_ENDPOINT5, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                zcl_getAttrVal(WATERMETER_ENDPOINT5, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                statusChangeNotification.zoneStatus &= ~ZONE_STATUS_BIT_ALARM1;
                zcl_setAttrVal(WATERMETER_ENDPOINT5, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                statusChangeNotification.extStatus = 0;
                statusChangeNotification.delay = 0;

                zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT5, &dstEpInfo, TRUE, &statusChangeNotification);
            }
        }
    }
}
