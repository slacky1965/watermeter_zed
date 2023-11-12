#ifndef SRC_INCLUDE_APP_BUTTON_H_
#define SRC_INCLUDE_APP_BUTTON_H_

typedef struct _button_t {
    uint8_t  released :1;
    uint8_t  pressed  :1;
    uint8_t  counter  :6;
    uint8_t  debounce;
    uint32_t pressed_time;
    uint32_t released_time;
} button_t;

void init_button();
void button_handler();
uint8_t button_idle();

#endif /* SRC_INCLUDE_APP_BUTTON_H_ */
