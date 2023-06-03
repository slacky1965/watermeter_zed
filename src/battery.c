#include "tl_common.h"

#include "battery.h"

u16 battery_mv;         /* 2200 ... 3100 mv                 */
u8  battery_level;      /* 0 ... 100 %                      */

// 2200..3100 mv - 0..100%
u8 get_battery_level(u16 battery_mv) {
    u8 battery_level = 0;
    if (battery_mv > MIN_VBAT_MV) {
        battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV
                - MIN_VBAT_MV) / 100);
        if (battery_level > 100)
            battery_level = 100;
    }
    return battery_level;
}

u16 get_battery_mv(void) {

    battery_mv = drv_get_adc_data();
    battery_level = get_battery_level(battery_mv);

    return battery_mv;
}

