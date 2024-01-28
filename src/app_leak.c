#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

static uint8_t leakage = false;

static void cmdOnOff() {
    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;
#if FIND_AND_BIND_SUPPORT
        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#else
    dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
    dstEpInfo.dstEp = WATERMETER_ENDPOINT4;
    dstEpInfo.dstAddr.shortAddr = 0xfffc;
#endif

//        zcl_onOff_toggleCmd(WATERMETER_ENDPOINT4, &dstEpInfo, FALSE);
    if (leakage) {
        zcl_onOff_onCmd(WATERMETER_ENDPOINT4, &dstEpInfo, FALSE);
    } else {
        zcl_onOff_offCmd(WATERMETER_ENDPOINT4, &dstEpInfo, FALSE);
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


void leak_handler() {

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;

    if (!drv_gpio_read(LEAK_GPIO)) {

        if (!leakage) {
            printf("Leakage on\r\n");
            leakage = true;
            if(zb_isDeviceJoinedNwk()) {
                cmdOnOff();
                fillIASAddress(&dstEpInfo);

                zoneStatusChangeNoti_t statusChangeNotification;

                zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                zcl_getAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_ALARM1;//ZONE_STATUS_TEST;
                zcl_setAttrVal(WATERMETER_ENDPOINT4, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                statusChangeNotification.extStatus = 0;
                //statusChangeNotification.zoneId = ZCL_ZONE_ID_INVALID;
                statusChangeNotification.delay = 0;

                zcl_iasZone_statusChangeNotificationCmd(WATERMETER_ENDPOINT4, &dstEpInfo, TRUE, &statusChangeNotification);
            }
        }
    } else {
        if (leakage) {
            printf("Leakage off\r\n");
            leakage = false;
            if(zb_isDeviceJoinedNwk()){
                cmdOnOff();
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
