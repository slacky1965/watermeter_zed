#ifndef SRC_INCLUDE_PULSE_H_
#define SRC_INCLUDE_PULSE_H_

typedef struct _water_conter_t {
    u8  status     :1;
    u8  pressed    :1;
    u8  counter    :6;
    u16 bit;
} water_counter_t;

void init_pulse();
u8 counters_handler();
u32 check_counter_overflow(u32 check_count);

#endif /* SRC_INCLUDE_PULSE_H_ */
