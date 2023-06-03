#include "tl_common.h"
#include "app_cfg.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#if ZBHCI_EN
#include "zbhci.h"
#endif

#include "watermeter.h"
#include "button.h"
#include "cfg.h"
#include "pulse.h"
#include "battery.h"

//app_ctx_t g_switchAppCtx;

u32 count_restart = 0;

#ifdef ZCL_OTA
extern ota_callBack_t sampleSwitch_otaCb;

//running code firmware information
ota_preamble_t sampleSwitch_otaInfo = {
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
    sampleSwitch_leaveIndHandler,//leave ind cb
    sampleSwitch_leaveCnfHandler,//leave cnf cb
    NULL,//nwk update ind cb
    NULL,//permit join ind cb
    NULL,//nlme sync cnf cb
    NULL,//tc join ind cb
    NULL,//tc detects that the frame counter is near limit
};


/**
 *  @brief Definition for BDB finding and binding cluster
 */
u16 bdb_findBindClusterList[] =
{
    ZCL_CLUSTER_GEN_ON_OFF,
};

/**
 *  @brief Definition for BDB finding and binding cluster number
 */
#define FIND_AND_BIND_CLUSTER_NUM       (sizeof(bdb_findBindClusterList)/sizeof(bdb_findBindClusterList[0]))

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
    }
};

static void app_lowPowerEnter() {

//    u32 sleepTime = 0;
//    bool longSleep = 0;

    for(u32 i = 0; i < 2; i++) {
        drv_pm_wakeupPinLevelChange(&pin_PmCfg[i], 2);
    }

    drv_pm_lowPowerEnter();

//    drv_pm_wakeup_src_e wakeupSrc = PM_WAKEUP_SRC_PAD;
//
//#if !defined(__PROJECT_TL_BOOT_LOADER__) && !defined(__PROJECT_TL_SNIFFER__)
//    if(tl_stackBusy() || !zb_isTaskDone()){
//        return;
//    }
//
//    apsCleanToStopSecondClock();
//#endif
//
//    u32 r = drv_disable_irq();
//    ev_timer_event_t *timerEvt = NULL;
//
//    if (zb_isDeviceJoinedNwk()) {
//        longSleep = true;
//        sleepTime = 0x07FFFFFF;
//    } else {
//        timerEvt = ev_timer_nearestGet();
//        if(timerEvt){
//            wakeupSrc |= PM_WAKEUP_SRC_TIMER;
//            sleepTime = timerEvt->timeout;
//        }
//
//        if(sleepTime){
//            if(sleepTime > PM_NORMAL_SLEEP_MAX){
//                longSleep = 1;
//            }else{
//                longSleep = 0;
//            }
//        }
//    }
//
//
//#if defined(MCU_CORE_826x)
//    drv_pm_sleep_mode_e sleepMode = (wakeupSrc & PM_WAKEUP_SRC_TIMER) ? PM_SLEEP_MODE_SUSPEND : PM_SLEEP_MODE_DEEPSLEEP;
//#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91)
//    drv_pm_sleep_mode_e sleepMode = PM_SLEEP_MODE_DEEP_WITH_RETENTION; //(wakeupSrc & PM_WAKEUP_SRC_TIMER) ? PM_SLEEP_MODE_DEEP_WITH_RETENTION : PM_SLEEP_MODE_DEEPSLEEP;
//#endif
//
//
//
//#if !defined(__PROJECT_TL_BOOT_LOADER__) && !defined(__PROJECT_TL_SNIFFER__)
//    rf_paShutDown();
//    if(sleepMode == PM_SLEEP_MODE_DEEPSLEEP){
//        drv_pm_deepSleep_frameCnt_set(ss_outgoingFrameCntGet());
//    }
//#endif
//
//    if(!longSleep){
//        drv_pm_sleep(sleepMode, wakeupSrc, sleepTime);
//    }else{
//        drv_pm_longSleep(sleepMode, wakeupSrc, sleepTime);
//    }
//
//    drv_restore_irq(r);

}

#endif
/**********************************************************************
 * LOCAL VARIABLES
 */


/**********************************************************************
 * FUNCTIONS
 */

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
    zcl_init(sampleSwitch_zclProcessIncomingMsg);

    /* register endPoint */
    af_endpointRegister(WATERMETER_ENDPOINT, (af_simple_descriptor_t *)&sampleSwitch_simpleDesc, zcl_rx_handler, NULL);

    /* Register ZCL specific cluster information */
    zcl_register(WATERMETER_ENDPOINT, SAMPLE_SWITCH_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_sampleSwitchClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&sampleSwitch_simpleDesc, &sampleSwitch_otaInfo, &sampleSwitch_otaCb);
#endif

    init_pulse();
    init_config();
    init_button();
}


void app_task(void) {

    button_handler();
    counters_handler();
    get_battery_mv();

    if(bdb_isIdle()){
#if PM_ENABLE
        if(!get_status_button()) {
//            printf("zb_isDeviceJoinedNwk: %s\r\n", zb_isDeviceJoinedNwk()?"true":"false");
            app_lowPowerEnter();
        }
#endif
    }

//    sleep_ms(1000);
}

static void sampleSwitchSysException(void)
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
    printf("[%d] isRetention: %s\r\n", count_restart++, isRetention?"true":"false");
#endif /* UART_PRINTF_MODE */

//    /* Initialize LEDs*/
//    led_init();

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
        sys_exceptHandlerRegister(sampleSwitchSysException);

        /* User's Task */
#if ZBHCI_EN
        ev_on_poll(EV_POLL_HCI, zbhciTask);
#endif
        ev_on_poll(EV_POLL_IDLE, app_task);

        /* Load the pre-install code from flash */
//        if(bdb_preInstallCodeLoad(&g_switchAppCtx.tcLinkKey.keyType, g_switchAppCtx.tcLinkKey.key) == RET_OK){
//            g_bdbCommissionSetting.linkKey.tcLinkKey.keyType = g_switchAppCtx.tcLinkKey.keyType;
//            g_bdbCommissionSetting.linkKey.tcLinkKey.key = g_switchAppCtx.tcLinkKey.key;
//        }

        bdb_findBindMatchClusterSet(FIND_AND_BIND_CLUSTER_NUM, bdb_findBindClusterList);

        /* Initialize BDB */
        u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
        bdb_init((af_simple_descriptor_t *)&sampleSwitch_simpleDesc, &g_bdbCommissionSetting, &g_zbDemoBdbCb, repower);

    }else{
        /* Re-config phy when system recovery from deep sleep with retention */
        mac_phyReconfig();
    }
}

