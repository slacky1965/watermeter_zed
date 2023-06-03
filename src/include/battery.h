#ifndef SRC_INCLUDE_BATTERY_H_
#define SRC_INCLUDE_BATTERY_H_

#define MAX_VBAT_MV     3100 // 3100 mV - > battery = 100%
#define MIN_VBAT_MV     2200 // 2200 mV - > battery = 0%

extern u16 battery_mv;
extern u8  battery_level;


u8 get_battery_level(u16 battery_mv);
u16 get_battery_mv(void);


#endif /* SRC_INCLUDE_BATTERY_H_ */
