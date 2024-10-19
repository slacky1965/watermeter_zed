#ifndef SRC_INCLUDE_APP_ENDPOINT_CFG_H_
#define SRC_INCLUDE_APP_ENDPOINT_CFG_H_

#define APP_ENDPOINT1 0x01
#define APP_ENDPOINT2 0x02
#define APP_ENDPOINT3 0x03
#define APP_ENDPOINT4 0x04
#define APP_ENDPOINT5 0x05

/**
 *  @brief Defined for basic cluster attributes
 */
typedef struct{
    uint8_t  zclVersion;
    uint8_t  appVersion;
    uint8_t  stackVersion;
    uint8_t  hwVersion;
    uint8_t  manuName[ZCL_BASIC_MAX_LENGTH];
    uint8_t  modelId[ZCL_BASIC_MAX_LENGTH];
    uint8_t  dateCode[ZCL_BASIC_MAX_LENGTH];
    uint8_t  powerSource;
    uint8_t  deviceEnable;
    uint8_t  swBuildId[ZCL_BASIC_MAX_LENGTH];    //attr 4000
}zcl_basicAttr_t;

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
    uint8_t  batteryVoltage;        //0x20
    uint8_t  batteryPercentage;     //0x21
}zcl_powerAttr_t;

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

typedef struct {
    uint64_t hot_water_counter;
    uint64_t cold_water_counter;
} zcl_watermeterAttr_t;

typedef struct {
    uint32_t hot_water_preset;
    uint32_t cold_water_preset;
    uint16_t water_step_preset;
} zcl_watermeterCfgAttr_t;

typedef struct {
    uint8_t     status;
    uint8_t     unit;                   // 0x07 - Litres
    uint8_t     summationFormatting;    // 0b0100000 - 7bit - 0, 3-6bit - 8, 0-2bit - 0 = 0x40
    uint8_t     deviceType;             // 2 - Water Metering
} zcl_se_meteringAttr_t;

/**
 *  @brief Defined for ias zone cluster attributes
 */
typedef struct {
    uint8_t     zoneState;
    uint16_t    zoneType;
    uint16_t    zoneStatus;
    extAddr_t   iasCieAddr;
    uint8_t     zoneId;
} zcl_iasZoneAttr_t;


/**
 *  @brief Defined for onoffconfig cluster attributes
 */
typedef struct {
    u8 switchType;
    u8 switchActions;
} zcl_onOffSwitchCfgAttr_t;

typedef struct {
    zcl_onOffSwitchCfgAttr_t ep4_attrs;
    zcl_onOffSwitchCfgAttr_t ep5_attrs;
} zcl_onOffSwitchCfg_4_5_Attr_t;


extern uint8_t APP_EP1_CB_CLUSTER_NUM;
extern uint8_t APP_EP2_CB_CLUSTER_NUM;
extern uint8_t APP_EP3_CB_CLUSTER_NUM;
extern uint8_t APP_EP4_CB_CLUSTER_NUM;
extern uint8_t APP_EP5_CB_CLUSTER_NUM;

extern const zcl_specClusterInfo_t g_appEp1ClusterList[];
extern const zcl_specClusterInfo_t g_appEp2ClusterList[];
extern const zcl_specClusterInfo_t g_appEp3ClusterList[];
extern const zcl_specClusterInfo_t g_appEp4ClusterList[];
extern const zcl_specClusterInfo_t g_appEp5ClusterList[];

extern const af_simple_descriptor_t app_ep1Desc;
extern const af_simple_descriptor_t app_ep2Desc;
extern const af_simple_descriptor_t app_ep3Desc;
extern const af_simple_descriptor_t app_ep4Desc;
extern const af_simple_descriptor_t app_ep5Desc;


/* Attributes */
extern zcl_basicAttr_t                  g_zcl_basicAttrs;
extern zcl_identifyAttr_t               g_zcl_identifyAttrs;
extern zcl_pollCtrlAttr_t               g_zcl_pollCtrlAttrs;
extern zcl_watermeterAttr_t             g_zcl_watermeterAttrs;
extern zcl_watermeterCfgAttr_t          g_zcl_watermeterCfgAttrs;
extern zcl_iasZoneAttr_t                g_zcl_iasZoneAttrs;
extern zcl_onOffSwitchCfg_4_5_Attr_t    g_zcl_onOffSwitchCfgAttrs;

#define zcl_iasZoneAttrGet()        &g_zcl_iasZoneAttrs
#define zcl_pollCtrlAttrGet()       &g_zcl_pollCtrlAttrs
#define zcl_onOffSwitchCfgAttrGet() &g_zcl_onOffSwitchCfgAttrs


#endif /* SRC_INCLUDE_APP_ENDPOINT_CFG_H_ */
