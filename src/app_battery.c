#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define MAX_VBAT_MV         3100                        /* 3100 mV - > battery = 100%         */
#define MIN_VBAT_MV         BATTERY_SAFETY_THRESHOLD    /* 2200 mV - > battery = 0%           */

// 2200..3100 mv - 0..100%
static uint8_t get_battery_level(uint16_t battery_mv) {
    /* Zigbee 0% - 0x0, 50% - 0x64, 100% - 0xc8 */
    uint16_t battery_level = 0;
    if (battery_mv > MIN_VBAT_MV) {
        battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV - MIN_VBAT_MV) / 0xC8);
        if (battery_level > 0xC8)
            battery_level = 0xC8;
    }
    return battery_level;
}

int32_t batteryCb(void) {

    uint16_t voltage_raw = drv_get_adc_data();
    uint8_t voltage = (uint8_t)(voltage_raw/100);
    uint8_t level = get_battery_level(voltage_raw);

#if UART_PRINTF_MODE && DEBUG_BATTERY
    printf("Voltage_raw: %d\r\n", voltage_raw);
    printf("Voltage:     %d\r\n", voltage);
    printf("Level:       %d\r\n", level);
#endif

    zcl_setAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE, &voltage);
    zcl_setAttrVal(WATERMETER_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING, &level);

    return 0;
}

