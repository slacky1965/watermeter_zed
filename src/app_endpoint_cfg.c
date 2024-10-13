#include "tl_common.h"
#include "zcl_include.h"

#include "se_custom_attr.h"
#include "watermeter.h"

#ifndef ZCL_BASIC_MFG_NAME
#define ZCL_BASIC_MFG_NAME          {6,'T','E','L','I','N','K'}
#endif
#ifndef ZCL_BASIC_MODEL_ID
#define ZCL_BASIC_MODEL_ID          {8,'T','L','S','R','8','2','x','x'}
#endif
#ifndef ZCL_BASIC_SW_BUILD_ID
#define ZCL_BASIC_SW_BUILD_ID       {10,'0','1','2','2','0','5','2','0','1','7'}
#endif


#define R               ACCESS_CONTROL_READ
#define RW              ACCESS_CONTROL_READ | ACCESS_CONTROL_WRITE
#define RR              ACCESS_CONTROL_READ | ACCESS_CONTROL_REPORTABLE

#define ZCL_UINT8       ZCL_DATA_TYPE_UINT8
#define ZCL_UINT16      ZCL_DATA_TYPE_UINT16
#define ZCL_UINT32      ZCL_DATA_TYPE_UINT32
#define ZCL_UINT48      ZCL_DATA_TYPE_UINT48
#define ZCL_ENUM8       ZCL_DATA_TYPE_ENUM8
#define ZCL_ENUM16      ZCL_DATA_TYPE_ENUM16
#define ZCL_BITMAP16    ZCL_DATA_TYPE_BITMAP16
#define ZCL_BOOLEAN     ZCL_DATA_TYPE_BOOLEAN
#define ZCL_CHAR_STR    ZCL_DATA_TYPE_CHAR_STR
#define ZCL_IEEE_ADDR   ZCL_DATA_TYPE_IEEE_ADDR

/**
 *  @brief Definition for Incoming cluster / Sever Cluster
 */
const uint16_t watermeter_ep1_inClusterList[] =
{
    ZCL_CLUSTER_GEN_BASIC,
    ZCL_CLUSTER_GEN_IDENTIFY,
#ifdef ZCL_POLL_CTRL
    ZCL_CLUSTER_GEN_POLL_CONTROL,
#endif
    ZCL_CLUSTER_GEN_POWER_CFG,
    ZCL_CLUSTER_SE_METERING,
};

/**
 *  @brief Definition for Outgoing cluster / Client Cluster
 */
const uint16_t watermeter_ep1_outClusterList[] =
{
#ifdef ZCL_GROUP
    ZCL_CLUSTER_GEN_GROUPS,
#endif
#ifdef ZCL_SCENE
    ZCL_CLUSTER_GEN_SCENES,
#endif
#ifdef ZCL_LEVEL_CTRL
    ZCL_CLUSTER_GEN_LEVEL_CONTROL,
#endif
#ifdef ZCL_OTA
    ZCL_CLUSTER_OTA,
#endif
#ifdef ZCL_ZLL_COMMISSIONING
    ZCL_CLUSTER_TOUCHLINK_COMMISSIONING,
#endif
};

/**
 *  @brief Definition for Server cluster number and Client cluster number
 */
#define WATERMETER_EP1_IN_CLUSTER_NUM   (sizeof(watermeter_ep1_inClusterList)/sizeof(watermeter_ep1_inClusterList[0]))
#define WATERMETER_EP1_OUT_CLUSTER_NUM  (sizeof(watermeter_ep1_outClusterList)/sizeof(watermeter_ep1_outClusterList[0]))

const uint16_t watermeter_ep2_inClusterList[] = {
    ZCL_CLUSTER_SE_METERING,
};

const uint16_t watermeter_ep2_outClusterList[] = {
    //ZCL_CLUSTER_SE_METERING,
};

#define WATERMETER_EP2_IN_CLUSTER_NUM   (sizeof(watermeter_ep2_inClusterList)/sizeof(watermeter_ep2_inClusterList[0]))
#define WATERMETER_EP2_OUT_CLUSTER_NUM  (sizeof(watermeter_ep2_outClusterList)/sizeof(watermeter_ep2_outClusterList[0]))

const uint16_t watermeter_ep3_inClusterList[] = {
    ZCL_CLUSTER_SE_METERING,
};

const uint16_t watermeter_ep3_outClusterList[] = {
    ZCL_CLUSTER_SE_METERING,
};

#define WATERMETER_EP3_IN_CLUSTER_NUM   (sizeof(watermeter_ep3_inClusterList)/sizeof(watermeter_ep3_inClusterList[0]))
#define WATERMETER_EP3_OUT_CLUSTER_NUM  (sizeof(watermeter_ep3_outClusterList)/sizeof(watermeter_ep3_outClusterList[0]))


const uint16_t watermeter_ep4_inClusterList[] = {
#ifdef ZCL_IAS_ZONE
    ZCL_CLUSTER_SS_IAS_ZONE,
#endif
#ifdef ZCL_ON_OFF_SWITCH_CFG
    ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG,
#endif
};

const uint16_t watermeter_ep4_outClusterList[] = {
#ifdef ZCL_ON_OFF
    ZCL_CLUSTER_GEN_ON_OFF,
#endif
};

#define WATERMETER_EP4_IN_CLUSTER_NUM   (sizeof(watermeter_ep4_inClusterList)/sizeof(watermeter_ep4_inClusterList[0]))
#define WATERMETER_EP4_OUT_CLUSTER_NUM  (sizeof(watermeter_ep4_outClusterList)/sizeof(watermeter_ep4_outClusterList[0]))

const uint16_t watermeter_ep5_inClusterList[] = {
//#ifdef ZCL_IAS_ZONE
//    ZCL_CLUSTER_SS_IAS_ZONE,
//#endif
#ifdef ZCL_ON_OFF_SWITCH_CFG
    ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG,
#endif
};

const uint16_t watermeter_ep5_outClusterList[] = {
#ifdef ZCL_ON_OFF
    ZCL_CLUSTER_GEN_ON_OFF,
#endif
};

#define WATERMETER_EP5_IN_CLUSTER_NUM   (sizeof(watermeter_ep5_inClusterList)/sizeof(watermeter_ep5_inClusterList[0]))
#define WATERMETER_EP5_OUT_CLUSTER_NUM  (sizeof(watermeter_ep5_outClusterList)/sizeof(watermeter_ep5_outClusterList[0]))


/**
 *  @brief Definition for simple description for HA profile
 */
const af_simple_descriptor_t watermeter_ep1Desc = {
    HA_PROFILE_ID,                          /* Application profile identifier */
    HA_DEV_METER_INTERFACE,                 /* Application device identifier */
    WATERMETER_ENDPOINT1,                   /* Endpoint */
    2,                                      /* Application device version */
    0,                                      /* Reserved */
    WATERMETER_EP1_IN_CLUSTER_NUM,          /* Application input cluster count */
    WATERMETER_EP1_OUT_CLUSTER_NUM,         /* Application output cluster count */
    (uint16_t *)watermeter_ep1_inClusterList,    /* Application input cluster list */
    (uint16_t *)watermeter_ep1_outClusterList,   /* Application output cluster list */
};

const af_simple_descriptor_t watermeter_ep2Desc =
{
    HA_PROFILE_ID,                          /* Application profile identifier */
    HA_DEV_METER_INTERFACE,                 /* Application device identifier */
    WATERMETER_ENDPOINT2,                   /* Endpoint */
    2,                                      /* Application device version */
    0,                                      /* Reserved */
    WATERMETER_EP2_IN_CLUSTER_NUM,          /* Application input cluster count */
    WATERMETER_EP2_OUT_CLUSTER_NUM,         /* Application output cluster count */
    (uint16_t *)watermeter_ep2_inClusterList,    /* Application input cluster list */
    (uint16_t *)watermeter_ep2_outClusterList,   /* Application output cluster list */
};

const af_simple_descriptor_t watermeter_ep3Desc =
{
    HA_PROFILE_ID,                          /* Application profile identifier */
    HA_DEV_METER_INTERFACE,                 /* Application device identifier */
    WATERMETER_ENDPOINT3,                   /* Endpoint */
    2,                                      /* Application device version */
    0,                                      /* Reserved */
    WATERMETER_EP3_IN_CLUSTER_NUM,          /* Application input cluster count */
    WATERMETER_EP3_OUT_CLUSTER_NUM,         /* Application output cluster count */
    (uint16_t *)watermeter_ep3_inClusterList,    /* Application input cluster list */
    (uint16_t *)watermeter_ep3_outClusterList,   /* Application output cluster list */
};


const af_simple_descriptor_t watermeter_ep4Desc =
{
    HA_PROFILE_ID,                          /* Application profile identifier */
    HA_DEV_IAS_ZONE,                        /* Application device identifier */
    WATERMETER_ENDPOINT4,                   /* Endpoint */
    2,                                      /* Application device version */
    0,                                      /* Reserved */
    WATERMETER_EP4_IN_CLUSTER_NUM,          /* Application input cluster count */
    WATERMETER_EP4_OUT_CLUSTER_NUM,         /* Application output cluster count */
    (uint16_t *)watermeter_ep4_inClusterList,    /* Application input cluster list */
    (uint16_t *)watermeter_ep4_outClusterList,   /* Application output cluster list */
};

const af_simple_descriptor_t watermeter_ep5Desc =
{
    HA_PROFILE_ID,                          /* Application profile identifier */
    HA_DEV_ONOFF_SENSOR,                    /* Application device identifier */
    WATERMETER_ENDPOINT5,                   /* Endpoint */
    2,                                      /* Application device version */
    0,                                      /* Reserved */
    WATERMETER_EP5_IN_CLUSTER_NUM,          /* Application input cluster count */
    WATERMETER_EP5_OUT_CLUSTER_NUM,         /* Application output cluster count */
    (uint16_t *)watermeter_ep5_inClusterList,    /* Application input cluster list */
    (uint16_t *)watermeter_ep5_outClusterList,   /* Application output cluster list */
};

/* Basic */
zcl_basicAttr_t g_zcl_basicAttrs =
{
    .zclVersion     = 0x03,
    .appVersion     = APP_RELEASE,
    .stackVersion   = (STACK_RELEASE|STACK_BUILD),
    .hwVersion      = HW_VERSION,
    .manuName       = ZCL_BASIC_MFG_NAME,
    .modelId        = ZCL_BASIC_MODEL_ID,
    .dateCode       = ZCL_BASIC_DATE_CODE,
    .powerSource    = POWER_SOURCE_BATTERY,
    .swBuildId      = ZCL_BASIC_SW_BUILD_ID,
    .deviceEnable   = TRUE,
};

const zclAttrInfo_t basic_attrTbl[] =
{
    { ZCL_ATTRID_BASIC_ZCL_VER,             ZCL_UINT8,      R,  (uint8_t*)&g_zcl_basicAttrs.zclVersion},
    { ZCL_ATTRID_BASIC_APP_VER,             ZCL_UINT8,      R,  (uint8_t*)&g_zcl_basicAttrs.appVersion},
    { ZCL_ATTRID_BASIC_STACK_VER,           ZCL_UINT8,      R,  (uint8_t*)&g_zcl_basicAttrs.stackVersion},
    { ZCL_ATTRID_BASIC_HW_VER,              ZCL_UINT8,      R,  (uint8_t*)&g_zcl_basicAttrs.hwVersion},
    { ZCL_ATTRID_BASIC_MFR_NAME,            ZCL_CHAR_STR,   R,  (uint8_t*)g_zcl_basicAttrs.manuName},
    { ZCL_ATTRID_BASIC_MODEL_ID,            ZCL_CHAR_STR,   R,  (uint8_t*)g_zcl_basicAttrs.modelId},
    { ZCL_ATTRID_BASIC_DATE_CODE,           ZCL_CHAR_STR,   R,  (uint8_t*)g_zcl_basicAttrs.dateCode},
    { ZCL_ATTRID_BASIC_POWER_SOURCE,        ZCL_ENUM8,      R,  (uint8_t*)&g_zcl_basicAttrs.powerSource},
    { ZCL_ATTRID_BASIC_DEV_ENABLED,         ZCL_BOOLEAN,    RW, (uint8_t*)&g_zcl_basicAttrs.deviceEnable},
    { ZCL_ATTRID_BASIC_SW_BUILD_ID,         ZCL_CHAR_STR,   R,  (uint8_t*)&g_zcl_basicAttrs.swBuildId},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16,     R,  (uint8_t*)&zcl_attr_global_clusterRevision},

};

#define ZCL_BASIC_ATTR_NUM       sizeof(basic_attrTbl) / sizeof(zclAttrInfo_t)


/* Identify */
zcl_identifyAttr_t g_zcl_identifyAttrs =
{
    .identifyTime   = 0x0000,
};

const zclAttrInfo_t identify_attrTbl[] =
{
    { ZCL_ATTRID_IDENTIFY_TIME,             ZCL_UINT16,     RW, (uint8_t*)&g_zcl_identifyAttrs.identifyTime },
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16,     R,  (uint8_t*)&zcl_attr_global_clusterRevision},
};

#define ZCL_IDENTIFY_ATTR_NUM           sizeof(identify_attrTbl) / sizeof(zclAttrInfo_t)

/* power */
zcl_powerAttr_t g_zcl_powerAttrs =
{
    .batteryVoltage    = 30,   //in 100 mV units, 0xff - unknown
    .batteryPercentage = 0x64, //in 0,5% units, 0xff - unknown
};

const zclAttrInfo_t powerCfg_attrTbl[] =
{
    { ZCL_ATTRID_BATTERY_VOLTAGE,               ZCL_UINT8,  RR, (uint8_t*)&g_zcl_powerAttrs.batteryVoltage},
    { ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING,  ZCL_UINT8,  RR, (uint8_t*)&g_zcl_powerAttrs.batteryPercentage},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,       ZCL_UINT16, R,  (uint8_t*)&zcl_attr_global_clusterRevision},
};

#define ZCL_POWER_CFG_ATTR_NUM       sizeof(powerCfg_attrTbl) / sizeof(zclAttrInfo_t)



#ifdef ZCL_POLL_CTRL
/* Poll Control */
zcl_pollCtrlAttr_t g_zcl_pollCtrlAttrs =
{
    .chkInInterval          = 0x3840,
    .longPollInterval       = 0x14,
    .shortPollInterval      = 0x02,
    .fastPollTimeout        = 0x28,
    .chkInIntervalMin       = 0x00,
    .longPollIntervalMin    = 0x00,
    .fastPollTimeoutMax     = 0x00,
};

const zclAttrInfo_t pollCtrl_attrTbl[] =
{
    { ZCL_ATTRID_CHK_IN_INTERVAL,           ZCL_UINT32, RW, (uint8_t*)&g_zcl_pollCtrlAttrs.chkInInterval },
    { ZCL_ATTRID_LONG_POLL_INTERVAL,        ZCL_UINT32, R,  (uint8_t*)&g_zcl_pollCtrlAttrs.longPollInterval },
    { ZCL_ATTRID_SHORT_POLL_INTERVAL,       ZCL_UINT16, R,  (uint8_t*)&g_zcl_pollCtrlAttrs.shortPollInterval },
    { ZCL_ATTRID_FAST_POLL_TIMEOUT,         ZCL_UINT16, RW, (uint8_t*)&g_zcl_pollCtrlAttrs.fastPollTimeout },
    { ZCL_ATTRID_CHK_IN_INTERVAL_MIN,       ZCL_UINT32, R,  (uint8_t*)&g_zcl_pollCtrlAttrs.chkInIntervalMin},
    { ZCL_ATTRID_LONG_POLL_INTERVAL_MIN,    ZCL_UINT32, R,  (uint8_t*)&g_zcl_pollCtrlAttrs.longPollIntervalMin },
    { ZCL_ATTRID_FAST_POLL_TIMEOUT_MAX,     ZCL_UINT16, R,  (uint8_t*)&g_zcl_pollCtrlAttrs.fastPollTimeoutMax},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16, R,  (uint8_t*)&zcl_attr_global_clusterRevision},
};

#define ZCL_POLLCTRL_ATTR_NUM           sizeof(pollCtrl_attrTbl) / sizeof(zclAttrInfo_t)
#endif

#ifdef ZCL_IAS_ZONE
/* IAS Zone */
zcl_iasZoneAttr_t g_zcl_iasZoneAttrs =
{
    .zoneState      = ZONE_STATE_NOT_ENROLLED,
    .zoneType       = ZONE_TYPE_WATER_SENSOR,
    .zoneStatus     = ZONE_STATUS_BIT_RESTORE_NOTIFY,
    .iasCieAddr     = {0x00},
    .zoneId         = ZCL_ZONE_ID_INVALID,
};

const zclAttrInfo_t iasZone_attrTbl[] =
{
    { ZCL_ATTRID_ZONE_STATE,                ZCL_ENUM8,      R,  (u8*)&g_zcl_iasZoneAttrs.zoneState },
    { ZCL_ATTRID_ZONE_TYPE,                 ZCL_ENUM16,     R,  (u8*)&g_zcl_iasZoneAttrs.zoneType },
    { ZCL_ATTRID_ZONE_STATUS,               ZCL_BITMAP16,   R,  (u8*)&g_zcl_iasZoneAttrs.zoneStatus },
    { ZCL_ATTRID_IAS_CIE_ADDR,              ZCL_IEEE_ADDR,  RW, (u8*)g_zcl_iasZoneAttrs.iasCieAddr },
    { ZCL_ATTRID_ZONE_ID,                   ZCL_UINT8,      R,  (u8*)&g_zcl_iasZoneAttrs.zoneId},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16,     R,  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_IASZONE_ATTR_NUM         sizeof(iasZone_attrTbl) / sizeof(zclAttrInfo_t)

#endif

#ifdef ZCL_ON_OFF_SWITCH_CFG
/* On/Off Config */

zcl_onOffSwitchCfg_4_5_Attr_t g_zcl_onOffSwitchCfgAttrs = {
    .ep4_attrs.switchType       = ZCL_SWITCH_TYPE_TOGGLE,
    .ep4_attrs.switchActions    = ZCL_SWITCH_ACTION_ON_OFF,
    .ep5_attrs.switchType       = ZCL_SWITCH_TYPE_TOGGLE,
    .ep5_attrs.switchActions    = ZCL_SWITCH_ACTION_ON_OFF,
};

//zcl_onOffSwitchCfgAttr_t g_zcl_onOffSwitchCfg_1Attrs =
//{
//    .switchType         = ZCL_SWITCH_TYPE_TOGGLE, //Toggle
//    .switchActions      = ZCL_SWITCH_ACTION_ON_OFF,
//};

const zclAttrInfo_t on_off_switch_config_1attrTbl[] =
{
    { ZCL_ATTRID_SWITCH_TYPE,               ZCL_ENUM8,  R,  (u8*)&g_zcl_onOffSwitchCfgAttrs.ep4_attrs.switchType },
    { ZCL_ATTRID_SWITCH_ACTION,             ZCL_ENUM8,  RW, (u8*)&g_zcl_onOffSwitchCfgAttrs.ep4_attrs.switchActions },

    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16, R,  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_ON_OFF_SWITCH_CFG_1ATTR_NUM       sizeof(on_off_switch_config_1attrTbl) / sizeof(zclAttrInfo_t)

//zcl_onOffSwitchCfgAttr_t g_zcl_onOffSwitchCfg_2Attrs =
//{
//    .switchType         = ZCL_SWITCH_TYPE_TOGGLE, //Toggle
//    .switchActions      = ZCL_SWITCH_ACTION_ON_OFF,
//};

const zclAttrInfo_t on_off_switch_config_2attrTbl[] =
{
    { ZCL_ATTRID_SWITCH_TYPE,               ZCL_ENUM8,  R,  (u8*)&g_zcl_onOffSwitchCfgAttrs.ep5_attrs.switchType },
    { ZCL_ATTRID_SWITCH_ACTION,             ZCL_ENUM8,  RW, (u8*)&g_zcl_onOffSwitchCfgAttrs.ep5_attrs.switchActions },

    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16, R,  (u8*)&zcl_attr_global_clusterRevision},
};

#define ZCL_ON_OFF_SWITCH_CFG_2ATTR_NUM       sizeof(on_off_switch_config_2attrTbl) / sizeof(zclAttrInfo_t)



#endif //ZCL_ON_OFF_SWITCH_CFG


zcl_watermeterAttr_t g_zcl_watermeterAttrs = {
    .hot_water_counter = 1,
    .cold_water_counter = 1,
};

zcl_watermeterCfgAttr_t g_zcl_watermeterCfgAttrs = {
    .hot_water_preset = 0,
    .cold_water_preset = 0,
    .water_step_preset = LITERS_PER_PULSE,
};

/* Attribute record list */
const zclAttrInfo_t zcl_hotWater_attrTbl[] = {
    { ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD,    ZCL_UINT48, RR, (uint8_t*)&g_zcl_watermeterAttrs.hot_water_counter},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,       ZCL_UINT16, R,  (uint8_t*)&zcl_attr_global_clusterRevision},
};

#define ZCL_HOT_WATERMETER_ATTR_NUM         sizeof(zcl_hotWater_attrTbl) / sizeof(zclAttrInfo_t)

const zclAttrInfo_t zcl_coldWater_attrTbl[] = {
    { ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD,    ZCL_UINT48, RR, (uint8_t*)&g_zcl_watermeterAttrs.cold_water_counter},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,       ZCL_UINT16, R,  (uint8_t*)&zcl_attr_global_clusterRevision},
};

#define ZCL_COLD_WATERMETER_ATTR_NUM         sizeof(zcl_coldWater_attrTbl) / sizeof(zclAttrInfo_t)

const zclAttrInfo_t zcl_watermeterCfg_attrTbl[] = {
    { ZCL_ATTRID_CUSTOM_HOT_WATER_PRESET,   ZCL_UINT32, RW, (uint8_t*)&g_zcl_watermeterCfgAttrs.hot_water_preset},
    { ZCL_ATTRID_CUSTOM_COLD_WATER_PRESET,  ZCL_UINT32, RW, (uint8_t*)&g_zcl_watermeterCfgAttrs.cold_water_preset},
    { ZCL_ATTRID_CUSTOM_WATER_STEP_PRESET,  ZCL_UINT16, RW, (uint8_t*)&g_zcl_watermeterCfgAttrs.water_step_preset},
    { ZCL_ATTRID_GLOBAL_CLUSTER_REVISION,   ZCL_UINT16, R,  (uint8_t*)&zcl_attr_global_clusterRevision},
};

#define ZCL_WATERMETER_CFG_ATTR_NUM         sizeof(zcl_watermeterCfg_attrTbl) / sizeof(zclAttrInfo_t)

/**
 *  @brief Definition for simple switch ZCL specific cluster
 */
const zcl_specClusterInfo_t g_watermeterEp1ClusterList[] =
{
    {ZCL_CLUSTER_GEN_BASIC,         MANUFACTURER_CODE_NONE, ZCL_BASIC_ATTR_NUM,     basic_attrTbl,      zcl_basic_register,     app_basicCb},
    {ZCL_CLUSTER_GEN_IDENTIFY,      MANUFACTURER_CODE_NONE, ZCL_IDENTIFY_ATTR_NUM,  identify_attrTbl,   zcl_identify_register,  app_identifyCb},
    {ZCL_CLUSTER_GEN_POWER_CFG,     MANUFACTURER_CODE_NONE, ZCL_POWER_CFG_ATTR_NUM, powerCfg_attrTbl,   zcl_powerCfg_register,  app_powerCfgCb},
#ifdef ZCL_GROUP
    {ZCL_CLUSTER_GEN_GROUPS,        MANUFACTURER_CODE_NONE, 0,                      NULL,               zcl_group_register,     app_groupCb},
#endif
#ifdef ZCL_SCENE
    {ZCL_CLUSTER_GEN_SCENES,        MANUFACTURER_CODE_NONE, 0,                      NULL,               zcl_scene_register,     app_sceneCb},
#endif
#ifdef ZCL_POLL_CTRL
    {ZCL_CLUSTER_GEN_POLL_CONTROL,  MANUFACTURER_CODE_NONE, ZCL_POLLCTRL_ATTR_NUM,  pollCtrl_attrTbl,   zcl_pollCtrl_register,  app_pollCtrlCb},
#endif
    {ZCL_CLUSTER_SE_METERING,  MANUFACTURER_CODE_NONE, ZCL_HOT_WATERMETER_ATTR_NUM,  zcl_hotWater_attrTbl,   zcl_metering_register,  app_meteringCb},
};

uint8_t WATERMETER_EP1_CB_CLUSTER_NUM = (sizeof(g_watermeterEp1ClusterList)/sizeof(g_watermeterEp1ClusterList[0]));

const zcl_specClusterInfo_t g_watermeterEp2ClusterList[] =
{
    {ZCL_CLUSTER_SE_METERING,  MANUFACTURER_CODE_NONE, ZCL_COLD_WATERMETER_ATTR_NUM,  zcl_coldWater_attrTbl,   zcl_metering_register,  app_meteringCb},
};

uint8_t WATERMETER_EP2_CB_CLUSTER_NUM = (sizeof(g_watermeterEp2ClusterList)/sizeof(g_watermeterEp2ClusterList[0]));

const zcl_specClusterInfo_t g_watermeterEp3ClusterList[] =
{
    {ZCL_CLUSTER_SE_METERING,  MANUFACTURER_CODE_NONE, ZCL_WATERMETER_CFG_ATTR_NUM,  zcl_watermeterCfg_attrTbl,   zcl_metering_register,  app_meteringCb},
};

uint8_t WATERMETER_EP3_CB_CLUSTER_NUM = (sizeof(g_watermeterEp3ClusterList)/sizeof(g_watermeterEp3ClusterList[0]));


const zcl_specClusterInfo_t g_watermeterEp4ClusterList[] =
{
#ifdef ZCL_IAS_ZONE
    {ZCL_CLUSTER_SS_IAS_ZONE,   MANUFACTURER_CODE_NONE, ZCL_IASZONE_ATTR_NUM,   iasZone_attrTbl,    zcl_iasZone_register,   leak_iasZoneCb},
#endif
#ifdef ZCL_ON_OFF_SWITCH_CFG
    {ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG, MANUFACTURER_CODE_NONE, ZCL_ON_OFF_SWITCH_CFG_2ATTR_NUM, on_off_switch_config_1attrTbl, zcl_onOffSwitchCfg_register, NULL},
#endif
};

uint8_t WATERMETER_EP4_CB_CLUSTER_NUM = (sizeof(g_watermeterEp4ClusterList)/sizeof(g_watermeterEp4ClusterList[0]));


const zcl_specClusterInfo_t g_watermeterEp5ClusterList[] =
{
#ifdef ZCL_ON_OFF_SWITCH_CFG
    {ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG, MANUFACTURER_CODE_NONE, ZCL_ON_OFF_SWITCH_CFG_2ATTR_NUM, on_off_switch_config_2attrTbl, zcl_onOffSwitchCfg_register, NULL},
#endif
};

uint8_t WATERMETER_EP5_CB_CLUSTER_NUM = (sizeof(g_watermeterEp5ClusterList)/sizeof(g_watermeterEp5ClusterList[0]));


