#ifndef SRC_INCLUDE_APP_MAIN_H_
#define SRC_INCLUDE_APP_MAIN_H_

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
    ev_timer_event_t *timerLedEvt;
    s32 Vbat;       //current voltage
    u32 keyPressedTime;

    u16 ledOnTime;
    u16 ledOffTime;
    u8  oriSta;     //original state before blink
    u8  sta;        //current state in blink
    u8  times;      //blink times
    u8  state;

    u8  keyPressed;

    app_linkKey_info_t tcLinkKey;
}app_ctx_t;

/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_ctx_t g_switchAppCtx;

extern bdb_appCb_t g_zbDemoBdbCb;

extern bdb_commissionSetting_t g_bdbCommissionSetting;

/**********************************************************************
 * FUNCTIONS
 */
void sampleSwitch_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t sampleSwitch_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleSwitch_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleSwitch_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleSwitch_sceneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t sampleSwitch_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);

void sampleSwitch_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void sampleSwitch_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void sampleSwitch_otaProcessMsgHandler(u8 evt, u8 status);


#endif /* SRC_INCLUDE_APP_MAIN_H_ */
