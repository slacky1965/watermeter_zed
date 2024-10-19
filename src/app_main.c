/********************************************************************************************************
 * @file    app_main.c
 *
 * @brief   This is the source file for app_main
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "app_endpoint_cfg.h"
#include "app_main.h"
#include "app_battery.h"
//#include "app_ui.h"
#if ZBHCI_EN
#include "zbhci.h"
#endif


/**********************************************************************
 * LOCAL CONSTANTS
 */
static uint32_t last_light = 0;


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */

app_ctx_t g_appCtx = {
        .bdbFBTimerEvt = NULL,
        .timerPollRateEvt = NULL,
        .timerForcedReportEvt = NULL,
        .timerStopReportEvt = NULL,
        .timerNoJoinedEvt = NULL,
        .short_poll = POLL_RATE * 3,
        .long_poll = POLL_RATE * LONG_POLL,
        .oriSta = false,
        .time_without_joined = 0,
};

//uint32_t count_restart = 0;


#ifdef ZCL_OTA
extern ota_callBack_t app_otaCb;

//running code firmware information
ota_preamble_t app_otaInfo = {
	.fileVer 			= FILE_VERSION,
	.imageType 			= IMAGE_TYPE,
	.manufacturerCode 	= MANUFACTURER_CODE_TELINK,
};
#endif


//Must declare the application call back function which used by ZDO layer
const zdo_appIndCb_t appCbLst = {
	bdb_zdoStartDevCnf,//start device cnf cb
	NULL,//reset cnf cb
	NULL,//device announce indication cb
	app_leaveIndHandler,//leave ind cb
	app_leaveCnfHandler,//leave cnf cb
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
#define FIND_AND_BIND_CLUSTER_NUM		(sizeof(bdb_findBindClusterList)/sizeof(bdb_findBindClusterList[0]))

/**
 *  @brief Definition for bdb commissioning setting
 */
bdb_commissionSetting_t g_bdbCommissionSetting = {
	.linkKey.tcLinkKey.keyType = SS_GLOBAL_LINK_KEY,
	.linkKey.tcLinkKey.key = (u8 *)tcLinkKeyCentralDefault,       		//can use unique link key stored in NV

	.linkKey.distributeLinkKey.keyType = MASTER_KEY,
	.linkKey.distributeLinkKey.key = (u8 *)linkKeyDistributedMaster,  	//use linkKeyDistributedCertification before testing

	.linkKey.touchLinkKey.keyType = MASTER_KEY,
	.linkKey.touchLinkKey.key = (u8 *)touchLinkKeyMaster,   			//use touchLinkKeyCertification before testing

#if TOUCHLINK_SUPPORT
	.touchlinkEnable = 1,												/* enable touch-link */
#else
	.touchlinkEnable = 0,												/* disable touch-link */
#endif
	.touchlinkChannel = DEFAULT_CHANNEL, 								/* touch-link default operation channel for target */
	.touchlinkLqiThreshold = 0xA0,			   							/* threshold for touch-link scan req/resp command */
};

#if PM_ENABLE
/**
 *  @brief Definition for wakeup source and level for PM
 */
drv_pm_pinCfg_t g_switchPmCfg[] = {
	{
		BUTTON1,
		PM_WAKEUP_LEVEL
	},
};
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
	zcl_init(app_zclProcessIncomingMsg);

    af_endpointRegister(APP_ENDPOINT1, (af_simple_descriptor_t *)&app_ep1Desc, zcl_rx_handler, NULL);
    af_endpointRegister(APP_ENDPOINT2, (af_simple_descriptor_t *)&app_ep2Desc, zcl_rx_handler, NULL);
    af_endpointRegister(APP_ENDPOINT3, (af_simple_descriptor_t *)&app_ep3Desc, zcl_rx_handler, NULL);
    af_endpointRegister(APP_ENDPOINT4, (af_simple_descriptor_t *)&app_ep4Desc, zcl_rx_handler, NULL);
    af_endpointRegister(APP_ENDPOINT5, (af_simple_descriptor_t *)&app_ep5Desc, zcl_rx_handler, NULL);

//    zcl_onOffCfgAttr_restore();
    zcl_reportingTabInit();

    /* Register ZCL specific cluster information */
    zcl_register(APP_ENDPOINT1, APP_EP1_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_appEp1ClusterList);
    zcl_register(APP_ENDPOINT2, APP_EP2_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_appEp2ClusterList);
    zcl_register(APP_ENDPOINT3, APP_EP3_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_appEp3ClusterList);
    zcl_register(APP_ENDPOINT4, APP_EP4_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_appEp4ClusterList);
    zcl_register(APP_ENDPOINT5, APP_EP5_CB_CLUSTER_NUM, (zcl_specClusterInfo_t *)g_appEp5ClusterList);

#if ZCL_OTA_SUPPORT
    ota_init(OTA_TYPE_CLIENT, (af_simple_descriptor_t *)&app_ep1Desc, &app_otaInfo, &app_otaCb);
#endif

//    init_config(true);
//    init_counters();
    init_button();

    batteryCb(NULL);
    g_appCtx.timerBatteryEvt = TL_ZB_TIMER_SCHEDULE(batteryCb, NULL, BATTERY_TIMER_INTERVAL);

//    uint64_t water_counter = watermeter_config.counter_hot_water & 0xffffffffffff;
//    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD, (uint8_t*)&water_counter);
//    water_counter = watermeter_config.counter_cold_water & 0xffffffffffff;
//    zcl_setAttrVal(APP_ENDPOINT2, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD, (uint8_t*)&water_counter);

//#if UART_PRINTF_MODE
//    printf("IMAGE_TYPE: 0x%x\r\n", IMAGE_TYPE);
//    printf("FILE_VERSION: 0x%x\r\n", FILE_VERSION);
//#endif

}



void led_init(void)
{
	light_init();
}

void app_task(void)
{
    button_handler();

	if(bdb_isIdle()){

        if (clock_time_exceed(last_light, TIMEOUT_TICK_5SEC)) {
            if (zb_isDeviceJoinedNwk()) {
                light_blink_stop();
//                if (watermeter_config.new_ota) {
//                    light_blink_start(2, 30, 250);
//                } else {
                    light_blink_start(1, 30, 30);
//                }
            }
            last_light = clock_time();
        }
//        report_handler();


#if PM_ENABLE
	    button_handler();
		
        if(!button_idle() /*&& !counters_idle() && !waterleak_idle()*/) {
            drv_pm_lowPowerEnter();
//            app_lowPowerEnter();
        }

//		if(!g_appCtx.keyPressed){
//			drv_pm_lowPowerEnter();
//		}
#endif
	}
}

static void appSysException(void)
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
	/* Initialize LEDs*/
	led_init();

#if PA_ENABLE
	rf_paInit(PA_TX, PA_RX);
#endif

#if ZBHCI_EN
	zbhciInit();
#endif

#if PM_ENABLE
	drv_pm_wakeupPinConfig(g_switchPmCfg, sizeof(g_switchPmCfg)/sizeof(drv_pm_pinCfg_t));
#endif

	if(!isRetention){
		/* Initialize Stack */
		stack_init();

		/* Initialize user application */
		user_app_init();

		/* Register except handler for test */
		sys_exceptHandlerRegister(appSysException);

		/* User's Task */
#if ZBHCI_EN
		ev_on_poll(EV_POLL_HCI, zbhciTask);
#endif
		ev_on_poll(EV_POLL_IDLE, app_task);

		/* Load the pre-install code from flash */
		if(bdb_preInstallCodeLoad(&g_appCtx.tcLinkKey.keyType, g_appCtx.tcLinkKey.key) == RET_OK){
			g_bdbCommissionSetting.linkKey.tcLinkKey.keyType = g_appCtx.tcLinkKey.keyType;
			g_bdbCommissionSetting.linkKey.tcLinkKey.key = g_appCtx.tcLinkKey.key;
		}

		bdb_findBindMatchClusterSet(FIND_AND_BIND_CLUSTER_NUM, bdb_findBindClusterList);

        /* Set default reporting configuration */
        uint8_t reportableChange = 0x00;
        bdb_defaultReportingCfg(APP_ENDPOINT1, HA_PROFILE_ID, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE,
                REPORTING_MIN, REPORTING_MAX, (uint8_t *)&reportableChange);
        bdb_defaultReportingCfg(APP_ENDPOINT1, HA_PROFILE_ID, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,
                REPORTING_MIN, REPORTING_MAX, (uint8_t *)&reportableChange);
        bdb_defaultReportingCfg(APP_ENDPOINT1, HA_PROFILE_ID, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD,
                0, REPORTING_MIN, (uint8_t *)&reportableChange);
        bdb_defaultReportingCfg(APP_ENDPOINT2, HA_PROFILE_ID, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD,
                0, REPORTING_MIN, (uint8_t *)&reportableChange);

        /* custom reporting application (non SDK) */
//        app_reporting_init();

		/* Initialize BDB */
		u8 repower = drv_pm_deepSleep_flag_get() ? 0 : 1;
		bdb_init((af_simple_descriptor_t *)&app_ep1Desc, &g_bdbCommissionSetting, &g_zbBdbCb, repower);
	}else{
		/* Re-config phy when system recovery from deep sleep with retention */
		mac_phyReconfig();
	}
}

