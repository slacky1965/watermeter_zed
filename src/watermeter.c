#include "tl_common.h"
#include "app_cfg.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#if ZBHCI_EN
#include "zbhci.h"
#endif

#include "app_ui.h"
#include "watermeter.h"
//#include "zcl_watermeter.h"

app_reporting_t app_reporting[ZCL_REPORTING_TABLE_NUM];

app_ctx_t g_watermeterCtx = {
        .bdbFBTimerEvt = NULL,
        .timerPollRateEvt = NULL,
        .timerReportEvt = NULL,
        .short_poll = POLL_RATE * 3,
        .long_poll = POLL_RATE * LONG_POLL,
        .oriSta = false,
};

//u32 count_restart = 0;

#ifdef ZCL_OTA
extern ota_callBack_t watermeter_otaCb;

//running code firmware information
ota_preamble_t watermeter_otaInfo = {
    .fileVer            = FILE_VERSION,
    .imageType          = IMAGE_TYPE,
    .manufacturerCode   = MANUFACTURER_CODE_TELINK,
};
#endif


//Must declare the application call back function which used by ZDO layer
const zdo_appIndCb_t appCbLst = {
    bdb_zdoStartDevCnf,//start device cnf cb
    NULL,//reset cnf cb
    NULL,//device announce indication cb
    watermeter_leaveIndHandler,//leave ind cb
    watermeter_leaveCnfHandler,//leave cnf cb
    NULL,//nwk update ind cb
    NULL,//permit join ind cb
    NULL,//nlme sync cnf cb
    NULL,//tc join ind cb
    NULL,//tc detects that the frame counter is near limit
};


///**
// *  @brief Definition for BDB finding and binding cluster
// */
//u16 bdb_findBindClusterList[] =
//{
//    ZCL_CLUSTER_GEN_ON_OFF,
//};
//
///**
// *  @brief Definition for BDB finding and binding cluster number
// */
//#define FIND_AND_BIND_CLUSTER_NUM       (sizeof(bdb_findBindClusterList)/sizeof(bdb_findBindClusterList[0]))

/**
 *  @brief Definition for bdb commissioning setting
 */
bdb_commissionSetting_t g_bdbCommissionSetting = {
    .linkKey.tcLinkKey.keyType = SS_GLOBAL_LINK_KEY,
    .linkKey.tcLinkKey.key = (u8 *)tcLinkKeyCentralDefault,             //can use unique link key stored in NV

    .linkKey.distributeLinkKey.keyType = MASTER_KEY,
    .linkKey.distributeLinkKey.key = (u8 *)linkKeyDistributedMaster,    //use linkKeyDistributedCertification before testing

    .linkKey.touchLinkKey.keyType = MASTER_KEY,
    .linkKey.touchLinkKey.key = (u8 *)touchLinkKeyMaster,               //use touchLinkKeyCertification before testing

#if TOUCHLINK_SUPPORT
    .touchlinkEnable = 1,                                               /* enable touch-link */
#else
    .touchlinkEnable = 0,                                               /* disable touch-link */
#endif
    .touchlinkChannel = DEFAULT_CHANNEL,                                /* touch-link default operation channel for target */
    .touchlinkLqiThreshold = 0xA0,                                      /* threshold for touch-link scan req/resp command */
};

#if PM_ENABLE
/**
 *  @brief Definition for wakeup source and level for PM
 */

drv_pm_pinCfg_t pin_PmCfg[] = {
    {
        HOT_GPIO,
        PM_WAKEUP_LEVEL
    },
    {
        COLD_GPIO,
        PM_WAKEUP_LEVEL
    },
    {
        BUTTON,
        PM_WAKEUP_LEVEL
    },
};

static void app_lowPowerEnter() {

    for(u32 i = 0; i < 2; i++) {
        drv_pm_wakeupPinLevelChange(&pin_PmCfg[i], 2);
    }

    drv_pm_lowPowerEnter();
}

#endif
/**********************************************************************
 * LOCAL VARIABLES
 */


/**********************************************************************
 * FUNCTIONS
 */

extern void reportAttr(reportCfgInfo_t *pEntry);

/**********************************************************************
 * Custom reporting application
 */

static void app_reporting_init() {
    TL_SETSTRUCTCONTENT(app_reporting, 0);

    for (u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++) {
        reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
        if (pEntry->used) {
            app_reporting[i].pEntry = pEntry;
        }
    }
}

static u8 app_reportableChangeValueChk(u8 dataType, u8 *curValue, u8 *prevValue, u8 *reportableChange) {
    u8 needReport = false;

    switch(dataType) {
        case ZCL_DATA_TYPE_UINT48:
        {
            u64 P = BUILD_U48(prevValue[0], prevValue[1], prevValue[2], prevValue[3], prevValue[4], prevValue[5]);
            u64 C = BUILD_U48(curValue[0], curValue[1], curValue[2], curValue[3], curValue[4], curValue[5]);
            u64 R = BUILD_U48(reportableChange[0], reportableChange[1], reportableChange[2], reportableChange[3], reportableChange[4], reportableChange[5]);
            if(P >= C){
                needReport = ((P - C) > R) ? true : false;
            }else{
                needReport = ((C - P) > R) ? true : false;
            }
            break;
        }
        default:
            break;
    }

    return needReport;
}

static s32 app_reportMinAttrTimerCb(void *arg) {
    app_reporting_t *app_reporting = (app_reporting_t*)arg;
    reportCfgInfo_t *pEntry = app_reporting->pEntry;

    app_reporting->timerReportMinEvt = NULL;

    if (pEntry->minInterval == pEntry->maxInterval) {
#if UART_PRINTF_MODE
        printf("Report has been sent. attrID: 0x%x. minInterval: %d\r\n", pEntry->attrID, pEntry->minInterval);
#endif
        reportAttr(pEntry);
        app_reporting->time_posted = clock_time();
        return -1;
    }

    zclAttrInfo_t *pAttrEntry = zcl_findAttribute(pEntry->endPoint, pEntry->clusterID, pEntry->attrID);
    if(!pAttrEntry){
        //should not happen.
        ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_ZCL_ENTRY);
        return -1;
    }

    u8 len = zcl_getAttrSize(pAttrEntry->type, pAttrEntry->data);

    len = (len>8) ? (8):(len);

    if( (!zcl_analogDataType(pAttrEntry->type) && (memcmp(pEntry->prevData, pAttrEntry->data, len) != SUCCESS)) ||
                    ((zcl_analogDataType(pAttrEntry->type) && reportableChangeValueChk(pAttrEntry->type,
                    pAttrEntry->data, pEntry->prevData, pEntry->reportableChange))) ||
                    ((zcl_analogDataType(pAttrEntry->type) && pAttrEntry->type == ZCL_DATA_TYPE_UINT48 &&
                    app_reportableChangeValueChk(pAttrEntry->type, pAttrEntry->data, pEntry->prevData, pEntry->reportableChange)))) {
#if UART_PRINTF_MODE
        printf("Report has been sent. clusterID: %x, attrID: 0x%x. minInterval: %d\r\n", pEntry->clusterID, pEntry->attrID, pEntry->minInterval);
#endif
        reportAttr(pEntry);
        app_reporting->time_posted = clock_time();
    }

    return -1;
}

static s32 app_reportMaxAttrTimerCb(void *arg) {
    app_reporting_t *app_reporting = (app_reporting_t*)arg;
    reportCfgInfo_t *pEntry = app_reporting->pEntry;

    if (clock_time_exceed(app_reporting->time_posted, pEntry->minInterval*1000*1000)) {
        if (app_reporting->timerReportMinEvt) {
            TL_ZB_TIMER_CANCEL(&app_reporting->timerReportMinEvt);
        }
#if UART_PRINTF_MODE
        printf("Report has been sent. clusterID: %x, attrID: 0x%x. maxInterval: %d\r\n", pEntry->clusterID, pEntry->attrID, pEntry->maxInterval);
#endif
        reportAttr(pEntry);
    }

    app_reporting->timerReportMaxEvt = NULL;

    return -1;
}

static void app_reportAttrTimerStart() {
    if(zcl_reportingEntryActiveNumGet()) {
        for(u8 i = 0; i < ZCL_REPORTING_TABLE_NUM; i++) {
            reportCfgInfo_t *pEntry = &reportingTab.reportCfgInfo[i];
            app_reporting[i].pEntry = pEntry;
            if(pEntry->used && (pEntry->maxInterval != 0xFFFF) && (pEntry->minInterval || pEntry->maxInterval)){
                if(zb_bindingTblSearched(pEntry->clusterID, pEntry->endPoint)) {
//                    printf("pEntry->clusterID: %x, min: %d, max: %d\r\n", pEntry->clusterID, pEntry->minInterval, pEntry->maxInterval);
                    if (!app_reporting[i].timerReportMinEvt) {
                        if (pEntry->minInterval && pEntry->maxInterval) {
                            //printf("%d. pEntry->minInterval: %d\r\n", i, pEntry->minInterval);
                            app_reporting[i].timerReportMinEvt = TL_ZB_TIMER_SCHEDULE(app_reportMinAttrTimerCb, &app_reporting[i], pEntry->minInterval*1000);
                        }
                    }
                    if (!pEntry->maxInterval) {
                        //printf("pEntry->clusterID: %x, min: %d, max: %d\r\n", pEntry->clusterID, pEntry->minInterval, pEntry->maxInterval);
                        if (app_reporting[i].timerReportMinEvt) {
                            TL_ZB_TIMER_CANCEL(&app_reporting[i].timerReportMinEvt);
                        }
                        if (app_reporting[i].timerReportMaxEvt) {
                            TL_ZB_TIMER_CANCEL(&app_reporting[i].timerReportMaxEvt);
                        }
                        app_reportMinAttrTimerCb(&app_reporting[i]);
                    }
                    if (!app_reporting[i].timerReportMaxEvt) {
                        if (pEntry->maxInterval) {
                            if (pEntry->maxInterval != pEntry->minInterval && pEntry->maxInterval > pEntry->minInterval) {
                                //printf("%d. pEntry->maxInterval: %d\r\n", i, pEntry->maxInterval);
                                app_reporting[i].timerReportMaxEvt = TL_ZB_TIMER_SCHEDULE(app_reportMaxAttrTimerCb, &app_reporting[i], pEntry->maxInterval*1000);
                            }
                        }

                    }
                }
            }
        }
    }
}

/*********************************************************************
 * @fn      stack_init
 *
 * @brief   This function initialize the ZigBee stack and related profile. If HA/ZLL profile is
 *          enabled in this application, related cluster should be registered here.
 *
 * @param   None
 *
 * @return  None
 */
void stack_init(void)
{
    /* Initialize ZB stack */
    zb_init();

    /* Register stack CB */
    zb_zdoCbRegister((zdo_appIndCb_t *)&appCbLst);
}

/*********************************************************************
 * @fn      user_app_init
 *
 * @brief   This function initialize the application(Endpoint) information for this node.
 *
 * @param   None
 *
 * @return  None
 */
void user_app_init(void)
{

#if ZCL_POLL_CTRL_SUPPORT
    af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_PERIODICALLY);
#else
    af_powerDescPowerModeUpdate(POWER_MODE_RECEIVER_COMES_WHEN_STIMULATED);
#endif

    /* Initialize ZCL layer */
    /* Register Incoming ZCL Foundation command/response messages */
    zcl_init(watermeter_zclProcessIncomingMsg);

    /* register endPoint */
    af_endpointRegister(WATERMETER_ENDPOINT1, (af_simple_descriptor_t *)&watermeter_ep1Desc, zcl_rx_handler, NULL);
    af_endpointRegister(WATERMETER_ENDPOINT2, (af_simple_descriptor_t *)&watermeter_ep2Desc, zcl_rx_handler, NULL);

    zcl_reportingTabInit();

    /* Register ZCL specific cluster information */
    zcl_register(WATERMETER_ENDPOINT1, WATERMETER_EP1_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_watermeterEp1ClusterList);
    zcl_register(WATERMETER_ENDPOINT2, WATERMETER_EP2_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_watermeterEp2ClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&watermeter_ep1Desc, &watermeter_otaInfo, &watermeter_otaCb);
#endif

    init_counters();
    init_config();
    init_button();

    batteryCb();
    TL_ZB_TIMER_SCHEDULE(batteryCb, NULL, TIMEOUT_15MIN);
}

void led_init(void)
{
    light_init();
}

void report_handler(void) {
    if(zb_isDeviceJoinedNwk()){
        if(zcl_reportingEntryActiveNumGet()) {

            reportNoMinLimit();

            //start report timers
            app_reportAttrTimerStart();
        }
    }
}


void app_task(void) {

    button_handler();
    counters_handler();

    if(bdb_isIdle()){
        report_handler();
#if PM_ENABLE
        if(!button_idle() && !counters_idle()) {
            app_lowPowerEnter();
        }
#endif
    }
}

static void watermeterSysException(void)
{
#if 1
    SYSTEM_RESET();
#else
    light_on();
    while(1);
#endif
}

/*********************************************************************
 * @fn      user_init
 *
 * @brief   User level initialization code.
 *
 * @param   isRetention - if it is waking up with ram retention.
 *
 * @return  None
 */
void user_init(bool isRetention)
{
#if UART_PRINTF_MODE
//    printf("[%d] isRetention: %s\r\n", count_restart++, isRetention?"true":"false");
#endif /* UART_PRINTF_MODE */

    /* Initialize LEDs*/
    led_init();

#if PA_ENABLE
    rf_paInit(PA_TX, PA_RX);
#endif

#if ZBHCI_EN
    zbhciInit();
#endif

#if PM_ENABLE
    drv_pm_wakeupPinConfig(pin_PmCfg, sizeof(pin_PmCfg)/sizeof(drv_pm_pinCfg_t));
#endif

    if(!isRetention){
        /* Initialize Stack */
        stack_init();

        /* Initialize user application */
        user_app_init();

        /* Register except handler for test */
        sys_exceptHandlerRegister(watermeterSysException);

        /* User's Task */
#if ZBHCI_EN
        ev_on_poll(EV_POLL_HCI, zbhciTask);
#endif
        ev_on_poll(EV_POLL_IDLE, app_task);

        /* Load the pre-install code from flash */
        if(bdb_preInstallCodeLoad(&g_watermeterCtx.tcLinkKey.keyType, g_watermeterCtx.tcLinkKey.key) == RET_OK){
            g_bdbCommissionSetting.linkKey.tcLinkKey.keyType = g_watermeterCtx.tcLinkKey.keyType;
            g_bdbCommissionSetting.linkKey.tcLinkKey.key = g_watermeterCtx.tcLinkKey.key;
        }

//        bdb_findBindMatchClusterSet(FIND_AND_BIND_CLUSTER_NUM, bdb_findBindClusterList);

        /* Set default reporting configuration */
        u8 reportableChange = 0x00;
        bdb_defaultReportingCfg(WATERMETER_ENDPOINT1, HA_PROFILE_ID, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE,
                REPORTING_MIN, REPORTING_MAX, (u8 *)&reportableChange);
        bdb_defaultReportingCfg(WATERMETER_ENDPOINT1, HA_PROFILE_ID, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
                REPORTING_MIN, REPORTING_MAX, (u8 *)&reportableChange);
        bdb_defaultReportingCfg(WATERMETER_ENDPOINT1, HA_PROFILE_ID, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD,
                REPORTING_MIN, 0, (u8 *)&reportableChange);

        /* custom reporting application (non SDK) */
        app_reporting_init();

        /* Initialize BDB */
        u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
        bdb_init((af_simple_descriptor_t *)&watermeter_ep1Desc, &g_bdbCommissionSetting, &g_zbBdbCb, repower);

    }else{
        /* Re-config phy when system recovery from deep sleep with retention */
        mac_phyReconfig();
    }
}

