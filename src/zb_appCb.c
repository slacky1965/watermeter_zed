/********************************************************************************************************
 * @file    zb_appCb.c
 *
 * @brief   This is the source file for zb_appCb
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
#include "zcl_include.h"
#include "ota.h"

#include "watermeter.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
void zb_bdbInitCb(uint8_t status, uint8_t joinedNetwork);
void zb_bdbCommissioningCb(uint8_t status, void *arg);
void zb_bdbIdentifyCb(uint8_t endpoint, uint16_t srcAddr, uint16_t identifyTime);
void zb_bdbFindBindSuccessCb(findBindDst_t *pDstInfo);


/**********************************************************************
 * LOCAL VARIABLES
 */
bdb_appCb_t g_zbBdbCb = {
    zb_bdbInitCb,
    zb_bdbCommissioningCb,
    zb_bdbIdentifyCb,
    zb_bdbFindBindSuccessCb
};

#ifdef ZCL_OTA
ota_callBack_t app_otaCb = {
    app_otaProcessMsgHandler,
};
#endif


/**********************************************************************
 * FUNCTIONS
 */
int32_t app_bdbNetworkSteerStart(void *arg) {
    bdb_networkSteerStart();

    return -1;
}

#if FIND_AND_BIND_SUPPORT
int32_t app_bdbFindAndBindStart(void *arg) {
    BDB_ATTR_GROUP_ID_SET(0x1234);//only for initiator
    bdb_findAndBindStart(BDB_COMMISSIONING_ROLE_INITIATOR);

    g_watermeterCtx.bdbFBTimerEvt = NULL;
    return -1;
}
#endif

ev_timer_event_t *appRejoinBackoffTimerEvt = NULL;
int32_t app_rejoinBacckoff(void *arg) {
    if (zb_isDeviceFactoryNew()) {
        appRejoinBackoffTimerEvt = NULL;
        return -1;
    }

    zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    return 0;
}

/*********************************************************************
 * @fn      zb_bdbInitCb
 *
 * @brief   application callback for bdb initiation
 *
 * @param   status - the status of bdb init BDB_INIT_STATUS_SUCCESS or BDB_INIT_STATUS_FAILURE
 *
 * @param   joinedNetwork  - 1: node is on a network, 0: node isn't on a network
 *
 * @return  None
 */
void zb_bdbInitCb(uint8_t status, uint8_t joinedNetwork) {
    //printf("bdbInitCb: sta = %x, joined = %x\n", status, joinedNetwork);

    if (status == BDB_INIT_STATUS_SUCCESS) {
        /*
         * for non-factory-new device:
         * 		load zcl data from NV, start poll rate, start ota query, bdb_networkSteerStart
         *
         * for factory-new device:
         * 		steer a network
         *
         */
        if (joinedNetwork) {
            zb_setPollRate(g_watermeterCtx.short_poll);
            if (g_watermeterCtx.timerPollRateEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
            }
            g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_1MIN);

            if (g_watermeterCtx.timerStopReportEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerStopReportEvt);
            }
            g_watermeterCtx.timerStopReportEvt = TL_ZB_TIMER_SCHEDULE(stopReportCb, NULL, TIMEOUT_15SEC);

#ifdef ZCL_OTA
            ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
#endif

#ifdef ZCL_POLL_CTRL
			app_zclCheckInStart();
#endif
        } else {
            uint16_t jitter = 0;
            do {
                jitter = zb_random() % 0x0fff;
            } while (jitter == 0);
            TL_ZB_TIMER_SCHEDULE(app_bdbNetworkSteerStart, NULL, jitter);
        }
    } else {
        if (joinedNetwork) {
//			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
            if (!appRejoinBackoffTimerEvt) {
                appRejoinBackoffTimerEvt = TL_ZB_TIMER_SCHEDULE(app_rejoinBacckoff, NULL, 60 * 1000);
            }
        }
    }
}

/*
BDB_COMMISSION_STA_SUCCESS = 0,         //<! The commissioning sub-procedure was successful.
BDB_COMMISSION_STA_IN_PROGRESS,         //<! One of the commissioning sub-procedures has started but is not yet complete
BDB_COMMISSION_STA_NOT_AA_CAPABLE,      //<! The initiator is not address assignment capable during touchlink.
BDB_COMMISSION_STA_NO_NETWORK,          //<! A network has not been found during network steering or touchlink
BDB_COMMISSION_STA_TARGET_FAILURE,      //<! A node has not joined a network when requested during touchlink
BDB_COMMISSION_STA_FORMATION_FAILURE,   //<! A network could not be formed during network formation.
BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE,//<! No response to an identify query command has been received during finding & binding
BDB_COMMISSION_STA_BINDING_TABLE_FULL,  //<! A binding table entry could not be created due to insufficient space in the binding table during finding & binding
BDB_COMMISSION_STA_NO_SCAN_RESPONSE,    //<! No response to a scan request inter-PAN command has been received during touchlink
BDB_COMMISSION_STA_NOT_PERMITTED,       //<! A touchlink (steal) attempt was made when a node is already connected to a centralized security network
BDB_COMMISSION_STA_TCLK_EX_FAILURE,     //<! The Trust Center link key exchange procedure has failed attempting to join a centralized security network.

BDB_COMMISSION_STA_PARENT_LOST,
BDB_COMMISSION_STA_REJOIN_FAILURE,
BDB_COMMISSION_STA_FORMATION_DONE,
*/

#if UART_PRINTF_MODE && DEBUG_STA_STATUS
const static uint8_t bdb_commission_sta_status[][64] = {
        "BDB_COMMISSION_STA_SUCCESS",
        "BDB_COMMISSION_STA_IN_PROGRESS",
        "BDB_COMMISSION_STA_NOT_AA_CAPABLE",
        "BDB_COMMISSION_STA_NO_NETWORK",
        "BDB_COMMISSION_STA_TARGET_FAILURE",
        "BDB_COMMISSION_STA_FORMATION_FAILURE",
        "BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE",
        "BDB_COMMISSION_STA_BINDING_TABLE_FULL",
        "BDB_COMMISSION_STA_NO_SCAN_RESPONSE",
        "BDB_COMMISSION_STA_NOT_PERMITTED",
        "BDB_COMMISSION_STA_TCLK_EX_FAILURE",
        "BDB_COMMISSION_STA_PARENT_LOST",
        "BDB_COMMISSION_STA_REJOIN_FAILURE",
        "BDB_COMMISSION_STA_FORMATION_DONE"
};
#endif /* UART_PRINTF_MODE */

/*********************************************************************
 * @fn      zb_bdbCommissioningCb
 *
 * @brief   application callback for bdb commissioning
 *
 * @param   status - the status of bdb commissioning
 *
 * @param   arg
 *
 * @return  None
 */
void zb_bdbCommissioningCb(uint8_t status, void *arg) {
    //printf("zb_bdbCommissioningCb: sta = %x\r\n", status);

    switch (status) {
        case BDB_COMMISSION_STA_SUCCESS:
            light_blink_start(2, 200, 200);

            zb_setPollRate(g_watermeterCtx.short_poll);
            if (g_watermeterCtx.timerPollRateEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
            }
            g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_1MIN);

            if (g_watermeterCtx.timerNoJoinedEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerNoJoinedEvt);
            }

            if (g_watermeterCtx.timerStopReportEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerStopReportEvt);
            }
            g_watermeterCtx.timerStopReportEvt = TL_ZB_TIMER_SCHEDULE(stopReportCb, NULL, TIMEOUT_15SEC);

#ifdef ZCL_POLL_CTRL
			app_zclCheckInStart();
#endif
#ifdef ZCL_OTA
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
#endif
#if FIND_AND_BIND_SUPPORT
			//start Finding & Binding
			if(!g_watermeterCtx.bdbFBTimerEvt){
			    g_watermeterCtx.bdbFBTimerEvt = TL_ZB_TIMER_SCHEDULE(app_bdbFindAndBindStart, NULL, 50);
			}
#endif
			if (appRejoinBackoffTimerEvt) {
			    TL_ZB_TIMER_CANCEL(&appRejoinBackoffTimerEvt);
			}
			break;
        case BDB_COMMISSION_STA_IN_PROGRESS:
            break;
        case BDB_COMMISSION_STA_NOT_AA_CAPABLE:
            break;
        case BDB_COMMISSION_STA_NO_NETWORK:
        case BDB_COMMISSION_STA_TCLK_EX_FAILURE:
        case BDB_COMMISSION_STA_TARGET_FAILURE:
            light_blink_stop();
            light_blink_start(3, 30, 250);
            uint16_t jitter = 0;
            do {
                jitter = zb_random() % 0x0fff;
            } while (jitter == 0);
            TL_ZB_TIMER_SCHEDULE(app_bdbNetworkSteerStart, NULL, jitter);

            if (!g_watermeterCtx.timerNoJoinedEvt) {
                g_watermeterCtx.timerNoJoinedEvt = TL_ZB_TIMER_SCHEDULE(no_joinedCb, NULL, TIMEOUT_NET);
#if UART_PRINTF_MODE && DEBUG_STA_STATUS
                printf("Not joined, status: %s (%d)\r\n", bdb_commission_sta_status[status], status);
#endif /* UART_PRINTF_MODE */
            }
            break;
        case BDB_COMMISSION_STA_FORMATION_FAILURE:
            break;
        case BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE:
            break;
        case BDB_COMMISSION_STA_BINDING_TABLE_FULL:
            break;
        case BDB_COMMISSION_STA_NO_SCAN_RESPONSE:
            break;
        case BDB_COMMISSION_STA_NOT_PERMITTED:
            break;
        case BDB_COMMISSION_STA_PARENT_LOST:
            //zb_rejoinSecModeSet(REJOIN_INSECURITY);
            zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
//			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
            break;
        case BDB_COMMISSION_STA_REJOIN_FAILURE:
            light_blink_stop();
            light_blink_start(3, 30, 250);
            if (!appRejoinBackoffTimerEvt) {
                appRejoinBackoffTimerEvt = TL_ZB_TIMER_SCHEDULE(app_rejoinBacckoff, NULL, 60 * 1000);
            }
            if (!g_watermeterCtx.timerNoJoinedEvt) {
                g_watermeterCtx.timerNoJoinedEvt = TL_ZB_TIMER_SCHEDULE(no_joinedCb, NULL, TIMEOUT_NET);
#if UART_PRINTF_MODE && DEBUG_STA_STATUS
                printf("Not joined, status: %s (%d)\r\n", bdb_commission_sta_status[status], status);
#endif /* UART_PRINTF_MODE */
            }
            break;
        default:
            break;
    }
}


extern void app_zclIdentifyCmdHandler(uint8_t endpoint, uint16_t srcAddr, uint16_t identifyTime);
void zb_bdbIdentifyCb(uint8_t endpoint, uint16_t srcAddr, uint16_t identifyTime){
#if FIND_AND_BIND_SUPPORT
	app_zclIdentifyCmdHandler(endpoint, srcAddr, identifyTime);
#endif
}

/*********************************************************************
 * @fn      zb_bdbFindBindSuccessCb
 *
 * @brief   application callback for finding & binding
 *
 * @param   pDstInfo
 *
 * @return  None
 */
void zb_bdbFindBindSuccessCb(findBindDst_t *pDstInfo) {
#if FIND_AND_BIND_SUPPORT
	epInfo_t dstEpInfo;
	TL_SETSTRUCTCONTENT(dstEpInfo, 0);

	dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
	dstEpInfo.dstAddr.shortAddr = pDstInfo->addr;
	dstEpInfo.dstEp = pDstInfo->endpoint;
	dstEpInfo.profileId = HA_PROFILE_ID;

	zcl_identify_identifyCmd(WATERMETER_ENDPOINT1, &dstEpInfo, FALSE, 0, 0);
#endif
}



#ifdef ZCL_OTA

extern ota_clientInfo_t otaClientInfo;
void ota_upgradeComplete(u8 status);

static void app_ota_abort() {

    /* reset update OTA */
    nv_resetModule(NV_MODULE_OTA);

    memset((uint8_t*) &otaClientInfo, 0, sizeof(otaClientInfo));
    otaClientInfo.clientOtaFlg = OTA_FLAG_INIT_DONE;
    otaClientInfo.crcValue = 0xffffffff;

    zcl_attr_imageTypeID = 0xffff;
    zcl_attr_fileOffset = 0xffffffff;
    zcl_attr_downloadFileVer = 0xffffffff;
}

void app_otaProcessMsgHandler(uint8_t evt, uint8_t status) {
    //printf("app_otaProcessMsgHandler: status = %x\r\n", status);
    if (evt == OTA_EVT_START) {

#if (VOLTAGE_DETECT_ENABLE)

    if(drv_get_adc_data() < ((MAX_VBAT_MV - 100 - MIN_VBAT_MV) / 2 + MIN_VBAT_MV)) {

#if UART_PRINTF_MODE && DEBUG_OTA
        printf("Battery charge less than 50%%, OTA update abort.\r\n");
#endif /* UART_PRINTF_MODE */

        app_ota_abort();
        ota_upgradeComplete(ZCL_STA_ABORT);
        return;
    }
#endif

        if (status == ZCL_STA_SUCCESS) {

#if UART_PRINTF_MODE && DEBUG_OTA
            printf("OTA update start.\r\n");
#endif /* UART_PRINTF_MODE */

            watermeter_config.new_ota = true;
            write_config();

            if (g_watermeterCtx.timerPollRateEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
            }

            zb_setPollRate(QUEUE_POLL_RATE);

        } else {

        }
    } else if (evt == OTA_EVT_COMPLETE) {

//        zb_setPollRate(POLL_RATE * 3);
        zb_setPollRate(g_watermeterCtx.short_poll);
        if (g_watermeterCtx.timerPollRateEvt) {
            TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
        }
        g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateAppCb, NULL, TIMEOUT_2MIN);

        if (status == ZCL_STA_SUCCESS) {

#if UART_PRINTF_MODE && DEBUG_OTA
            printf("OTA update successful.\r\n");
#endif /* UART_PRINTF_MODE */

            set_regDeepSleep();
            ota_mcuReboot();

        } else {

#if UART_PRINTF_MODE && DEBUG_OTA
            printf("OTA update failure. Try again.\r\n");
#endif /* UART_PRINTF_MODE */

            app_ota_abort();

            /* restore config */
            if (watermeter_config.new_ota) init_config(false);

            ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
        }
    }
}
#endif

/*********************************************************************
 * @fn      app_leaveCnfHandler
 *
 * @brief   Handler for ZDO Leave Confirm message.
 *
 * @param   pRsp - parameter of leave confirm
 *
 * @return  None
 */
void app_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf) {
    if (pLeaveCnf->status == SUCCESS) {
        //SYSTEM_RESET();

        if (appRejoinBackoffTimerEvt) {
            TL_ZB_TIMER_CANCEL(&appRejoinBackoffTimerEvt);
        }
    }
}

/*********************************************************************
 * @fn      app_leaveIndHandler
 *
 * @brief   Handler for ZDO leave indication message.
 *
 * @param   pInd - parameter of leave indication
 *
 * @return  None
 */
void app_leaveIndHandler(nlme_leave_ind_t *pLeaveInd)
{
    //printf("app_leaveIndHandler, rejoin = %d\n", pLeaveInd->rejoin);
    //printfArray(pLeaveInd->device_address, 8);
}


