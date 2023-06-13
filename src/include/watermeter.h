#ifndef SRC_INCLUDE_WATERMETER_H_
#define SRC_INCLUDE_WATERMETER_H_

/**********************************************************************
 * CONSTANT
 */
#define WATERMETER_ENDPOINT  0x01

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
    ev_timer_event_t *timerBatteryEvt;
    ev_timer_event_t *timerPollRateEvt;
    ev_timer_event_t *timerLedEvt;

    u32 short_poll;
    u32 long_poll;

    u16 battery_mv;         /* 2200 ... 3100 mv                 */
    u8  battery_level;      /* 0 ... 100 %                      */

    button_t button;

    u16 ledOnTime;
    u16 ledOffTime;
    u8  oriSta;     //original state before blink
    u8  sta;        //current state in blink
    u8  times;      //blink times
    u8  state;

    app_linkKey_info_t tcLinkKey;
}app_ctx_t;

/**
 *  @brief Defined for basic cluster attributes
 */
typedef struct{
    u8  zclVersion;
    u8  appVersion;
    u8  stackVersion;
    u8  hwVersion;
    u8  manuName[ZCL_BASIC_MAX_LENGTH];
    u8  modelId[ZCL_BASIC_MAX_LENGTH];
    u8  powerSource;
    u8  deviceEnable;
}zcl_basicAttr_t;

/**
 *  @brief Defined for identify cluster attributes
 */
typedef struct{
    u16 identifyTime;
}zcl_identifyAttr_t;

/**
 *  @brief  Defined for poll control cluster attributes
 */
typedef struct{
    u32 chkInInterval;
    u32 longPollInterval;
    u32 chkInIntervalMin;
    u32 longPollIntervalMin;
    u16 shortPollInterval;
    u16 fastPollTimeout;
    u16 fastPollTimeoutMax;
}zcl_pollCtrlAttr_t;

/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_ctx_t g_watermeterCtx;

extern bdb_appCb_t g_zbDemoBdbCb;

extern bdb_commissionSetting_t g_bdbCommissionSetting;

extern u8 SAMPLE_SWITCH_CB_CLUSTER_NUM;
extern const zcl_specClusterInfo_t g_watermeterClusterList[];
extern const af_simple_descriptor_t watermeter_simpleDesc;

/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs;

#define zcl_pollCtrlAttrGet()   &g_zcl_pollCtrlAttrs
/**********************************************************************
 * FUNCTIONS
 */
void watermeter_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t watermeter_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_sceneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);

void watermeter_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void watermeter_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void watermeter_otaProcessMsgHandler(u8 evt, u8 status);


#endif /* SRC_INCLUDE_WATERMETER_H_ */
