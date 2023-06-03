#ifndef SRC_INCLUDE_WATERMETER_ZCL_H_
#define SRC_INCLUDE_WATERMETER_ZCL_H_

//#if (ZCL_WATERMETER_SUPPORT)

#define ZCL_CLUSTER_WATERMETER                  0xFC01

//Basic Information Attribute Set
#define ZCL_ATTRID_HOT_WATER_VALUE              0x0000
#define ZCL_ATTRID_COLD_WATER_VALUE             0x0001
#define ZCL_ATTRID_WATER_STEP                   0x0002
#define ZCL_ATTRID_VOLTAGE_VALUE                0x0003
#define ZCL_ATTRID_BATTERY_LEVEL_VALUE          0x0004

#define ZCL_CMD_SET_HOT_WATER_VALUE             0x00
#define ZCL_CMD_SET_COLD_WATER_VALUE            0x01
#define ZCL_CMD_SET_WATER_STEP                  0x02

//#endif

//extern const u8 zcl_watermeter_attrNum;
//extern const zclAttrInfo_t zcl_watermeter_attrTbl[];

status_t zcl_watermeter_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb);

#endif /* SRC_INCLUDE_WATERMETER_ZCL_H_ */
