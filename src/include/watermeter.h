#ifndef SRC_INCLUDE_WATERMETER_H_
#define SRC_INCLUDE_WATERMETER_H_

#include "app_utility.h"

/**********************************************************************
 * CONSTANT
 */
#define WATERMETER_ENDPOINT1 0x01
#define WATERMETER_ENDPOINT2 0x02
#define WATERMETER_ENDPOINT3 0x03
#define SE_PROFILE_ID        0x0109

#define CHK_DEEP_SLEEP       DEEP_ANA_REG1

/**********************************************************************
 * TYPEDEFS
 */
typedef struct{
    u8 keyType; /* CERTIFICATION_KEY or MASTER_KEY key for touch-link or distribute network
                   SS_UNIQUE_LINK_KEY or SS_GLOBAL_LINK_KEY for distribute network */
    u8 key[16]; /* the key used */
}app_linkKey_info_t;

typedef struct {
    ev_timer_event_t *timerReportMinEvt;
    ev_timer_event_t *timerReportMaxEvt;
    reportCfgInfo_t  *pEntry;
    u32               time_posted;
} app_reporting_t;

typedef struct{
    ev_timer_event_t *bdbFBTimerEvt;
    ev_timer_event_t *timerForcedReportEvt;
    ev_timer_event_t *timerStopReportEvt;
    ev_timer_event_t *timerPollRateEvt;
    ev_timer_event_t *timerBatteryEvt;
    ev_timer_event_t *timerLedEvt;
    ev_timer_event_t *timerNoJoinedEvt;

    u32 short_poll;
    u32 long_poll;
    u32 current_poll;

    button_t button;

    u16 ledOnTime;
    u16 ledOffTime;
    u8  oriSta;     //original state before blink
    u8  sta;        //current state in blink
    u8  times;      //blink times
    u8  state;

    u32 time_without_joined;

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
    u8  dateCode[ZCL_BASIC_MAX_LENGTH];
    u8  powerSource;
    u8  genDevClass;                        //attr 8
    u8  genDevType;                         //attr 9
    u8  deviceEnable;
    u8  swBuildId[ZCL_BASIC_MAX_LENGTH];    //attr 4000
} zcl_basicAttr_t;

/**
 *  @brief Defined for identify cluster attributes
 */
typedef struct{
    u16 identifyTime;
}zcl_identifyAttr_t;

/**
 *  @brief Defined for power configuration cluster attributes
 */
typedef struct{
#ifdef POWER_MAINS
    u16 mainsVoltage;
    u8  mainsFrequency;
#endif
    u8  batteryVoltage;      //0x20
    u8  batteryPercentage;   //0x21
}zcl_powerAttr_t;

typedef struct {
    u64 hot_water_counter;
    u64 cold_water_counter;
} zcl_watermeterAttr_t;

typedef struct {
    u32 hot_water_preset;
    u32 cold_water_preset;
    u16 water_step_preset;
} zcl_watermeterCfgAttr_t;


/**
 *  @brief  Defined for poll control cluster attributes
 */
typedef struct {
    u32 chkInInterval;
    u32 longPollInterval;
    u32 chkInIntervalMin;
    u32 longPollIntervalMin;
    u16 shortPollInterval;
    u16 fastPollTimeout;
    u16 fastPollTimeoutMax;
} zcl_pollCtrlAttr_t;


/**********************************************************************
 * GLOBAL VARIABLES
 */
extern app_reporting_t app_reporting[ZCL_REPORTING_TABLE_NUM];

extern app_ctx_t g_watermeterCtx;

extern bdb_appCb_t g_zbBdbCb;

extern bdb_commissionSetting_t g_bdbCommissionSetting;

extern u8 WATERMETER_EP1_CB_CLUSTER_NUM;
extern u8 WATERMETER_EP2_CB_CLUSTER_NUM;
extern u8 WATERMETER_EP3_CB_CLUSTER_NUM;
extern const zcl_specClusterInfo_t g_watermeterEp1ClusterList[];
extern const zcl_specClusterInfo_t g_watermeterEp2ClusterList[];
extern const zcl_specClusterInfo_t g_watermeterEp3ClusterList[];
extern const af_simple_descriptor_t watermeter_ep1Desc;
extern const af_simple_descriptor_t watermeter_ep2Desc;
extern const af_simple_descriptor_t watermeter_ep3Desc;

/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs;
extern zcl_watermeterAttr_t g_zcl_watermeterAttrs;
extern zcl_watermeterCfgAttr_t g_zcl_watermeterCfgAttrs;

#define zcl_pollCtrlAttrGet()   &g_zcl_pollCtrlAttrs
/**********************************************************************
 * FUNCTIONS
 */
void watermeter_zclProcessIncomingMsg(zclIncoming_t *pInHdlrMsg);

status_t watermeter_basicCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_identifyCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_powerCfgCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_groupCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_sceneCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_pollCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_meteringCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);
status_t watermeter_cfgCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload);

void watermeter_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf);
void watermeter_leaveIndHandler(nlme_leave_ind_t *pLeaveInd);
void watermeter_otaProcessMsgHandler(u8 evt, u8 status);
void app_wakeupPinLevelChange();


#endif /* SRC_INCLUDE_WATERMETER_H_ */
