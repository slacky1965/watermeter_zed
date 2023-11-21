#ifndef SRC_INCLUDE_APP_BATTERY_H_
#define SRC_INCLUDE_APP_BATTERY_H_

#define MAX_VBAT_MV             3100                        /* 3100 mV - > battery = 100%         */
#define MIN_VBAT_MV             BATTERY_SAFETY_THRESHOLD    /* 2200 mV - > battery = 0%           */
#define BATTERY_TIMER_INTERVAL  TIMEOUT_15MIN

int32_t batteryCb(void *arg);

#endif /* SRC_INCLUDE_APP_BATTERY_H_ */
