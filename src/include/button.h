#ifndef SRC_INCLUDE_BUTTON_H_
#define SRC_INCLUDE_BUTTON_H_

typedef struct _button_t {
    u8  released :1;
    u8  pressed  :1;
    u8  reserve  :6;
    u32 pressed_time;
    u32 released_time;
} button_t;

void init_button();
void button_handler();
u8 get_status_button();


#endif /* SRC_INCLUDE_BUTTON_H_ */
