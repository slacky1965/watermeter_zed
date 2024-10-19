#ifndef SRC_INCLUDE_APP_ENDPOINT_CFG_H_
#define SRC_INCLUDE_APP_ENDPOINT_CFG_H_

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

extern u8 SAMPLE_SWITCH_CB_CLUSTER_NUM;
extern const zcl_specClusterInfo_t g_sampleSwitchClusterList[];
extern const af_simple_descriptor_t sampleSwitch_simpleDesc;

/* Attributes */
extern zcl_basicAttr_t g_zcl_basicAttrs;
extern zcl_identifyAttr_t g_zcl_identifyAttrs;
extern zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs;

#define zcl_pollCtrlAttrGet()   &g_zcl_pollCtrlAttrs


#endif /* SRC_INCLUDE_APP_ENDPOINT_CFG_H_ */
