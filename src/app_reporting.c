#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define BUILD_U48(b0, b1, b2, b3, b4, b5)   ( (uint64_t)((((uint64_t)(b5) & 0x0000000000ff) << 40) + (((uint64_t)(b4) & 0x0000000000ff) << 32) + (((uint64_t)(b3) & 0x0000000000ff) << 24) + (((uint64_t)(b2) & 0x0000000000ff) << 16) + (((uint64_t)(b1) & 0x0000000000ff) << 8) + ((uint64_t)(b0) & 0x00000000FF)) )

app_reporting_t app_reporting[ZCL_REPORTING_TABLE_NUM];

extern void reportAttr(reportCfgInfo_t *pEntry);

/**********************************************************************
 * Custom reporting application
 */

static uint8_t app_reportableChangeValueChk(uint8_t dataType, uint8_t *curValue, uint8_t *prevValue, uint8_t *reportableChange) {
    uint8_t needReport = false;

    switch(dataType) {
        case ZCL_DATA_TYPE_UINT48: {
            uint64_t P = BUILD_U48(prevValue[0], prevValue[1], prevValue[2], prevValue[3], prevValue[4], prevValue[5]);
            uint64_t C = BUILD_U48(curValue[0], curValue[1], curValue[2], curValue[3], curValue[4], curValue[5]);
            uint64_t R = BUILD_U48(reportableChange[0], reportableChange[1], reportableChange[2], reportableChange[3], reportableChange[4], reportableChange[5]);
            if(P >= C){
                needReport = ((P - C) > R) ? true : false;
            }else{
                needReport = ((C - P) > R) ? true : false;
            }
            break;
        }
        default:
            needReport = reportableChangeValueChk(dataType, curValue, prevValue, reportableChange);
            break;
    }

    return needReport;
}

static int32_t app_reportMinAttrTimerCb(void *arg) {
    app_reporting_t *app_reporting = (app_reporting_t*)arg;
    reportCfgInfo_t *pEntry = app_reporting->pEntry;

    zclAttrInfo_t *pAttrEntry = zcl_findAttribute(pEntry->endPoint, pEntry->clusterID, pEntry->attrID);
    if(!pAttrEntry){
        //should not happen.
        ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_ZCL_ENTRY);
        app_reporting->timerReportMinEvt = NULL;
        return -1;
    }

    if (pEntry->minInterval == pEntry->maxInterval) {
        reportAttr(pEntry);
        app_reporting->time_posted = clock_time();
#if UART_PRINTF_MODE && DEBUG_REPORTING
        printf("Report Min_Interval has been sent. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, minInterval: %d, maxInterval: %d\r\n",
                pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
#endif
        return 0;
    }

    uint8_t len = zcl_getAttrSize(pAttrEntry->type, pAttrEntry->data);


    len = (len>8) ? (8):(len);

    if( (!zcl_analogDataType(pAttrEntry->type) && (memcmp(pEntry->prevData, pAttrEntry->data, len) != SUCCESS)) ||
            ((zcl_analogDataType(pAttrEntry->type) && app_reportableChangeValueChk(pAttrEntry->type,
            pAttrEntry->data, pEntry->prevData, pEntry->reportableChange)))) {

        reportAttr(pEntry);
        app_reporting->time_posted = clock_time();
#if UART_PRINTF_MODE && DEBUG_REPORTING
        printf("Report Min_Interval has been sent. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, minInterval: %d, maxInterval: %d\r\n",
                pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
#endif
    }

    return 0;
}

static int32_t app_reportMaxAttrTimerCb(void *arg) {
    app_reporting_t *app_reporting = (app_reporting_t*)arg;
    reportCfgInfo_t *pEntry = app_reporting->pEntry;

    if (clock_time_exceed(app_reporting->time_posted, pEntry->minInterval*1000*1000)) {
        if (app_reporting->timerReportMinEvt) {
            TL_ZB_TIMER_CANCEL(&app_reporting->timerReportMinEvt);
        }
#if UART_PRINTF_MODE && DEBUG_REPORTING
        printf("Report Max_Interval has been sent. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, minInterval: %d, maxInterval: %d\r\n",
                pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
#endif
        reportAttr(pEntry);
    }

    return 0;
}

static void app_reportAttrTimerStart() {
    if(zcl_reportingEntryActiveNumGet()) {
        for(uint8_t i = 0; i < ZCL_REPORTING_TABLE_NUM; i++) {
            reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
            app_reporting[i].pEntry = pEntry;
            if(pEntry->used && (pEntry->maxInterval != 0xFFFF) && (pEntry->minInterval || pEntry->maxInterval)){
                if(zb_bindingTblSearched(pEntry->clusterID, pEntry->endPoint)) {
                    if (!app_reporting[i].timerReportMinEvt) {
                        if (pEntry->minInterval && pEntry->maxInterval && pEntry->minInterval <= pEntry->maxInterval) {
#if UART_PRINTF_MODE && DEBUG_REPORTING
                            printf("Start minTimer. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, min: %d, max: %d\r\n", pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
#endif
                            app_reporting[i].timerReportMinEvt = TL_ZB_TIMER_SCHEDULE(app_reportMinAttrTimerCb, &app_reporting[i], pEntry->minInterval*1000);
                        }
                    }
                    if (!app_reporting[i].timerReportMaxEvt) {
                        if (pEntry->maxInterval) {
                            if (pEntry->minInterval < pEntry->maxInterval) {
                                if (pEntry->maxInterval != pEntry->minInterval && pEntry->maxInterval > pEntry->minInterval) {
#if UART_PRINTF_MODE && DEBUG_REPORTING
                                    printf("Start maxTimer. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, min: %d, max: %d\r\n", pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
#endif
                                    app_reporting[i].timerReportMaxEvt = TL_ZB_TIMER_SCHEDULE(app_reportMaxAttrTimerCb, &app_reporting[i], pEntry->maxInterval*1000);
                                }
                            }
                        } else {
                            app_reportMinAttrTimerCb(&app_reporting[i]);
                        }

                    }
                }
            }
        }
    }
}

static void app_reportNoMinLimit(void)
{
    if(zcl_reportingEntryActiveNumGet()){
        zclAttrInfo_t *pAttrEntry = NULL;
        uint16_t len = 0;

        for(uint8_t i = 0; i < ZCL_REPORTING_TABLE_NUM; i++){
            reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
            if(pEntry->used && (pEntry->maxInterval == 0 || ((pEntry->maxInterval != 0xFFFF) && (pEntry->minInterval == 0)))) {
                //there is no minimum limit
                pAttrEntry = zcl_findAttribute(pEntry->endPoint, pEntry->clusterID, pEntry->attrID);
                if(!pAttrEntry){
                    //should not happen.
                    ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_ZCL_ENTRY);
                    return;
                }

                len = zcl_getAttrSize(pAttrEntry->type, pAttrEntry->data);
                len = (len>8) ? (8):(len);

                if( (!zcl_analogDataType(pAttrEntry->type) && (memcmp(pEntry->prevData, pAttrEntry->data, len) != SUCCESS)) ||
                        ((zcl_analogDataType(pAttrEntry->type) && app_reportableChangeValueChk(pAttrEntry->type,
                        pAttrEntry->data, pEntry->prevData, pEntry->reportableChange)))) {

                    if(zb_bindingTblSearched(pEntry->clusterID, pEntry->endPoint)) {

                        reportAttr(pEntry);
                        app_reporting[i].time_posted = clock_time();
    #if UART_PRINTF_MODE && DEBUG_REPORTING
                        printf("Report No_Min_Limit has been sent. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, minInterval: %d, maxInterval: %d\r\n",
                                pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
    #endif
                        if (app_reporting[i].timerReportMaxEvt) {
                            TL_ZB_TIMER_CANCEL(&app_reporting[i].timerReportMaxEvt);
                        }
                        app_reporting[i].timerReportMaxEvt = TL_ZB_TIMER_SCHEDULE(app_reportMaxAttrTimerCb, &app_reporting[i], pEntry->maxInterval*1000);
    #if UART_PRINTF_MODE && DEBUG_REPORTING
                        printf("Start maxTimer. endPoint: %d, clusterID: 0x%x, attrID: 0x%x, min: %d, max: %d\r\n", pEntry->endPoint, pEntry->clusterID, pEntry->attrID, pEntry->minInterval, pEntry->maxInterval);
    #endif
                    }

                }
            }
        }
    }
}

void app_reporting_init() {

    TL_SETSTRUCTCONTENT(app_reporting, 0);

    for (uint8_t i = 0; i < ZCL_REPORTING_TABLE_NUM; i++) {
        reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
//        printf("(%d) used: %s\r\n", i, pEntry->used?"true":"false");
        if (pEntry->used) {
//            printf("(%d) endPoint: %d\r\n", i, pEntry->endPoint);
//            printf("(%d) clusterID: 0x%x\r\n", i, pEntry->clusterID);
//            printf("(%d) attrID: 0x%x\r\n", i, pEntry->attrID);
//            printf("(%d) dataType: 0x%x\r\n", i, pEntry->dataType);
//            printf("(%d) minInterval: %d\r\n", i, pEntry->minInterval);
//            printf("(%d) maxInterval: %d\r\n", i, pEntry->maxInterval);
            app_reporting[i].pEntry = pEntry;
        }
    }
}

void report_handler(void) {
    if(zb_isDeviceJoinedNwk()) {

        if (g_watermeterCtx.timerStopReportEvt) return;

        if(zcl_reportingEntryActiveNumGet()) {

            app_reportNoMinLimit();

            //start report timers
            app_reportAttrTimerStart();
        }
    }
}

int32_t stopReportCb(void *arg) {
    g_watermeterCtx.timerStopReportEvt = NULL;
    return -1;
}

int32_t forcedReportCb(void *arg) {

    if(zb_isDeviceJoinedNwk()){
        epInfo_t dstEpInfo;
        TL_SETSTRUCTCONTENT(dstEpInfo, 0);

        dstEpInfo.profileId = HA_PROFILE_ID;
#if FIND_AND_BIND_SUPPORT
        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#else
        dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
        dstEpInfo.dstEp = WATERMETER_ENDPOINT1;
        dstEpInfo.dstAddr.shortAddr = 0xfffc;
#endif
        zclAttrInfo_t *pAttrEntry;
        pAttrEntry = zcl_findAttribute(WATERMETER_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE);
        zcl_sendReportCmd(WATERMETER_ENDPOINT1, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                ZCL_CLUSTER_GEN_POWER_CFG, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);

        pAttrEntry = zcl_findAttribute(WATERMETER_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING);
        zcl_sendReportCmd(WATERMETER_ENDPOINT1, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                ZCL_CLUSTER_GEN_POWER_CFG, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);

        pAttrEntry = zcl_findAttribute(WATERMETER_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD);
        zcl_sendReportCmd(WATERMETER_ENDPOINT1, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                ZCL_CLUSTER_SE_METERING, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);

#if FIND_AND_BIND_SUPPORT
        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#else
        dstEpInfo.dstEp = WATERMETER_ENDPOINT2;
#endif
        pAttrEntry = zcl_findAttribute(WATERMETER_ENDPOINT2, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD);
        zcl_sendReportCmd(WATERMETER_ENDPOINT2, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                ZCL_CLUSTER_SE_METERING, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);

    }

    g_watermeterCtx.timerForcedReportEvt = NULL;

    return -1;
}

