#ifndef SRC_INCLUDE_APP_BUTTON_H_
#define SRC_INCLUDE_APP_BUTTON_H_

#define MAX_BUTTON_MULTI_PRESSED    5

enum {
    APP_STATE_NORMAL,
    APP_STATE_RELEASE,
    APP_FACTORY_NEW_SET_CHECK,
    APP_FACTORY_NEW_DOING
};

typedef struct {
    uint8_t     ctn;
    uint32_t    pressed_time;
    uint32_t    released_time;
    uint8_t     state;
} button_t;

void button_handler(void);
u8 button_idle();

#endif /* SRC_INCLUDE_APP_BUTTON_H_ */
