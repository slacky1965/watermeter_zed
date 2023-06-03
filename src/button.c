#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"

#include "watermeter.h"
#include "button.h"

button_t button;

void init_button() {

    memset(&button, 0, sizeof(button_t));

}

void button_handler() {

    if (!drv_gpio_read(BUTTON)) {
        if (!button.pressed) {
            button.pressed = true;
            button.pressed_time = clock_time();
        }
    } else {
        if (button.pressed) {
            button.released = true;
            button.released_time = clock_time();
        }
    }

    if (button.pressed && button.released) {
        button.pressed = button.released = false;
        /* long pressed >= 5 sec. */
        if (clock_time_exceed(button.pressed_time, TIMEOUT_5SEC)){
            zb_factoryReset();
            sleep_ms(100);
            SYSTEM_RESET();
        } else { /* short pressed < 5 sec. */
            printf("shortPressed\r\n");
        }
    }

}

u8 get_status_button() {
    if (!button.pressed && !button.released && clock_time_exceed(button.released_time, TIMEOUT_15SEC)) {
        return false;
    }
    return true;
}
