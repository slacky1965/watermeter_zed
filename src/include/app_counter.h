#ifndef SRC_INCLUDE_APP_COUNTER_H_
#define SRC_INCLUDE_APP_COUNTER_H_

typedef struct _water_conter_t {
    uint8_t  counter;
    uint8_t  debounce;
} water_counter_t;

void init_counters();
uint8_t counters_handler();
uint32_t check_counter_overflow(uint32_t check_count);
uint8_t counters_idle();

#endif /* SRC_INCLUDE_APP_COUNTER_H_ */
