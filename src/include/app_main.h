#ifndef SRC_INCLUDE_APP_MAIN_H_
#define SRC_INCLUDE_APP_MAIN_H_

#include "app_utility.h"
#include "app_button.h"
#include "app_led.h"
#include "app_utility.h"

/**********************************************************************
 * CONSTANT
 */
#define SAMPLE_SWITCH_ENDPOINT  0x01

/**********************************************************************
 * TYPEDEFS
 */
typedef struct{
    u8 keyType; /* ERTIFICATION_KEY or MASTER_KEY key for touch-link or distribute network
                 SS_UNIQUE_LINK_KEY or SS_GLOBAL_LINK_KEY for distribute network */
    u8 key[16]; /* the key used */
}app_linkKey_info_t;

typedef struct{
    ev_timer_event_t *bdbFBTimerEvt;
    ev_timer_event_t *timerForcedReportEvt;
    ev_timer_event_t *timerStopReportEvt;
    ev_timer_event_t *timerPollRateEvt;
    ev_timer_event_t *timerBatteryEvt;
    ev_timer_event_t *timerLedEvt;
    ev_timer_event_t *timerNoJoinedEvt;

    uint32_t short_poll;
    uint32_t long_poll;
    uint32_t current_poll;

    button_t button;
    u8  keyPressed;

    uint16_t ledOnTime;
    uint16_t ledOffTime;
    uint8_t  oriSta;     //original state before blink
    uint8_t  sta;        //current state in blink
    uint8_t  times;      //blink times
    uint8_t  state;

    uint32_t time_without_joined;

    app_linkKey_info_t tcLinkKey;
}app_ctx_t;

/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_ctx_t g_appCtx;

extern bdb_appCb_t g_zbBdbCb;

extern bdb_commissionSetting_t g_bdbCommissionSetting;

/**********************************************************************
 * FUNCTIONS
 */
void app_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t app_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_sceneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t app_meteringCb(zclIncomingAddrInfo_t *pAddrInfo, uint8_t cmdId, void *cmdPayload);
status_t app_powerCfgCb(zclIncomingAddrInfo_t *pAddrInfo, uint8_t cmdId, void *cmdPayload);
status_t leak_iasZoneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);

void app_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void app_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void app_otaProcessMsgHandler(u8 evt, u8 status);


#endif /* SRC_INCLUDE_APP_MAIN_H_ */
