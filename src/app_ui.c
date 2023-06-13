/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for app_ui
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"

#include "app_ui.h"
#include "watermeter.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */

#define ID_CONFIG   0x0FED1410
#define TOP_MASK    0xFFFFFFFF

#define MAX_VBAT_MV 3100                        /* 3100 mV - > battery = 100%   */
#define MIN_VBAT_MV BATTERY_SAFETY_THRESHOLD    /* 2200 mV - > battery = 0%     */

#define BIT_COUNT   32768                       /* number of polls for debounce */

/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * GLOBAL VARIABLES
 */

watermeter_config_t watermeter_config;


/**********************************************************************
 * LOCAL VARIABLES
 */

static water_counter_t hot_counter;
static water_counter_t cold_counter;

static u8 default_config = false;
static u32 config_addr_start = 0;
static u32 config_addr_end = 0;

/**********************************************************************
 * FUNCTIONS
 */

u8 mcuBootAddrGet(void);

/**********************************************************************
 *  Timers callback
 */

s32 poll_rateCb(void *arg) {

    zb_setPollRate(g_watermeterCtx.long_poll);
    g_watermeterCtx.timerPollRateEvt = NULL;

    return -1;
}

static s32 delayedMcuResetCb(void *arg) {

//    zb_resetDevice();
    printf("mcu reset\r\n");
    return -1;
}

static s32 delayedFactoryResetCb(void *arg) {

    zb_factoryReset();
    printf("factory reset\r\n");
    return -1;
}

static s32 delayedFullResetCb(void *arg) {

    printf("full reset\r\n");
    return -1;
}

/**********************************************************************
 *  For LED
 */

void led_on(u32 pin)
{
    drv_gpio_write(pin, LED_ON);
}

void led_off(u32 pin)
{
    drv_gpio_write(pin, LED_OFF);
}

void light_on(void)
{
    led_on(LED1);
}

void light_off(void)
{
    led_off(LED1);
}

void light_init(void)
{
    led_off(LED1);

}

s32 zclLightTimerCb(void *arg)
{
    u32 interval = 0;

    if(g_watermeterCtx.sta == g_watermeterCtx.oriSta){
        g_watermeterCtx.times--;
        if(g_watermeterCtx.times <= 0){
            g_watermeterCtx.timerLedEvt = NULL;
            return -1;
        }
    }

    g_watermeterCtx.sta = !g_watermeterCtx.sta;
    if(g_watermeterCtx.sta){
        light_on();
        interval = g_watermeterCtx.ledOnTime;
    }else{
        light_off();
        interval = g_watermeterCtx.ledOffTime;
    }

    return interval;
}

void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime)
{
    u32 interval = 0;
    g_watermeterCtx.times = times;
//    g_watermeterCtx.oriSta = drv_gpio_read(LED1);

    if(!g_watermeterCtx.timerLedEvt){
        if(g_watermeterCtx.oriSta){
            light_off();
            g_watermeterCtx.sta = 0;
            interval = ledOffTime;
        }else{
            light_on();
            g_watermeterCtx.sta = 1;
            interval = ledOnTime;
        }
        g_watermeterCtx.ledOnTime = ledOnTime;
        g_watermeterCtx.ledOffTime = ledOffTime;

        g_watermeterCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(zclLightTimerCb, NULL, interval);
    }
}

void light_blink_stop(void)
{
    if(g_watermeterCtx.timerLedEvt){
        TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerLedEvt);

        g_watermeterCtx.times = 0;
        if(g_watermeterCtx.oriSta){
            light_on();
        }else{
            light_off();
        }
    }
}


/**********************************************************************
 *  For check battery
 */

// 2200..3100 mv - 0..100%
static u8 get_battery_level(u16 battery_mv) {
    u8 battery_level = 0;
    if (battery_mv > MIN_VBAT_MV) {
        battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV - MIN_VBAT_MV) / 100);
        if (battery_level > 100)
            battery_level = 100;
    }
    return battery_level;
}

static u16 get_battery_mv(void) {

    g_watermeterCtx.battery_mv = drv_get_adc_data();
    g_watermeterCtx.battery_level = get_battery_level(g_watermeterCtx.battery_mv);

    return g_watermeterCtx.battery_mv;
}

s32 batteryCb(void) {

#if UART_PRINTF_MODE
    printf("Timer check battery 10 min.\r\n");
#endif

    get_battery_mv();

    return 0;
}


/**********************************************************************
 *  For button
 */

void init_button() {

    memset(&g_watermeterCtx.button, 0, sizeof(button_t));

    g_watermeterCtx.button.bit = 1;
    g_watermeterCtx.button.released_time = clock_time();

}

void button_handler() {

    if (!drv_gpio_read(BUTTON)) {
        if (g_watermeterCtx.button.bit != BIT_COUNT) {
            g_watermeterCtx.button.bit <<= 1;
            if (g_watermeterCtx.button.bit == BIT_COUNT) {
                g_watermeterCtx.button.pressed = true;
                g_watermeterCtx.button.pressed_time = clock_time();
//                light_on();
                if (!clock_time_exceed(g_watermeterCtx.button.released_time, TIMEOUT_TICK_1SEC)) {
                    g_watermeterCtx.button.counter++;
                } else {
                    g_watermeterCtx.button.counter = 1;
                }
            }
        }
    } else {
        if (g_watermeterCtx.button.bit != 1) {
            g_watermeterCtx.button.bit >>= 1;
            if (g_watermeterCtx.button.bit == 1 && g_watermeterCtx.button.pressed) {
//                light_off();
                g_watermeterCtx.button.released = true;
                g_watermeterCtx.button.released_time = clock_time();
            }
        }
    }

    if (g_watermeterCtx.button.pressed && g_watermeterCtx.button.released) {
        g_watermeterCtx.button.pressed = g_watermeterCtx.button.released = false;
        if (clock_time_exceed(g_watermeterCtx.button.pressed_time, TIMEOUT_TICK_10SEC)) {
            /* long pressed > 10 sec. */
            /* TODO: full clean (factory reset and clean config) */
            light_blink_start(3, 30, 250);
#if UART_PRINTF_MODE
            printf("Full reset of the device!\r\n");
#endif
            TL_ZB_TIMER_SCHEDULE(delayedFullResetCb, NULL, TIMEOUT_1SEC);
        } else if (clock_time_exceed(g_watermeterCtx.button.pressed_time, TIMEOUT_TICK_5SEC)) {
            /* long pressed > 5 sec. */
            light_blink_start(3, 30, 250);
#if UART_PRINTF_MODE
            printf("MCU reset!\r\n");
#endif
//            zb_resetDevice();
            TL_ZB_TIMER_SCHEDULE(delayedMcuResetCb, NULL, TIMEOUT_1SEC);
        } else { /* short pressed < 5 sec. */
            light_blink_start(1, 30, 30);
            if (g_watermeterCtx.timerPollRateEvt) {
                TL_ZB_TIMER_CANCEL(&g_watermeterCtx.timerPollRateEvt);
            }
            g_watermeterCtx.timerPollRateEvt = TL_ZB_TIMER_SCHEDULE(poll_rateCb, NULL, TIMEOUT_2MIN);
            zb_setPollRate(g_watermeterCtx.short_poll);

        }
    } else if (!g_watermeterCtx.button.pressed) {
        if (clock_time_exceed(g_watermeterCtx.button.released_time, TIMEOUT_TICK_1SEC)) {
            if (g_watermeterCtx.button.counter == 3) {
                light_blink_start(3, 30, 250);
                g_watermeterCtx.button.counter = 0;
#if UART_PRINTF_MODE
                printf("Factory reset!\r\n");
#endif
//                zb_factoryReset();
//                sleep_ms(100);
//                zb_resetDevice();
                TL_ZB_TIMER_SCHEDULE(delayedFactoryResetCb, NULL, TIMEOUT_1SEC);
            } else {
                g_watermeterCtx.button.counter = 0;
            }
        }

    }
}

u8 button_idle() {
    if ((g_watermeterCtx.button.bit != 1 && g_watermeterCtx.button.bit != BIT_COUNT)
            || g_watermeterCtx.button.pressed
            || g_watermeterCtx.button.counter) {
        return true;
    }
    return false;
}


/**********************************************************************
 *  For counter hot and cold waters
 */

u32 check_counter_overflow(u32 check_count) {
    u32 count;

    if (check_count >= COUNTERS_OVERFLOW) {
        count = check_count - COUNTERS_OVERFLOW;
#if UART_PRINTF_MODE
        printf("Counter overflow\r\n");
#endif /* UART_PRINTF_MODE */
    } else {
        count = check_count;
    }

    return count;
}

void init_counters() {

    hot_counter.counter = 0;
    if (!drv_gpio_read(HOT_GPIO)) {
        hot_counter.bit = BIT_COUNT;
        hot_counter.pressed = true;
    } else {
        hot_counter.bit = 1;
        hot_counter.pressed = false;
    }

    cold_counter.counter = 0;
    if (!drv_gpio_read(COLD_GPIO)) {
        cold_counter.bit = BIT_COUNT;
        cold_counter.pressed = true;
    } else {
        cold_counter.bit = 1;
        cold_counter.pressed = false;
    }

}


u8 counters_handler() {

    u8 save_config = false;

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_10K);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_10K);
#endif

    if (!drv_gpio_read(HOT_GPIO)) {
        if (hot_counter.bit != BIT_COUNT) {
            hot_counter.bit <<= 1;
            if (hot_counter.bit == BIT_COUNT) {
                hot_counter.pressed = true;
                hot_counter.counter++;
            }
        }
    } else {
        if (hot_counter.bit != 1) {
            hot_counter.bit >>= 1;
//            if (hot_counter.bit == 1 && hot_counter.pressed) {
//                hot_counter.release = true;
//            }
        }
    }

    if (!drv_gpio_read(COLD_GPIO)) {
        if (cold_counter.bit != BIT_COUNT) {
            cold_counter.bit <<= 1;
            if (cold_counter.bit == BIT_COUNT) {
                cold_counter.pressed = true;
                cold_counter.counter++;
            }
        }
    } else {
        if (cold_counter.bit != 1) {
            cold_counter.bit >>= 1;
//            if (cold_counter.bit == 1 && cold_counter.pressed) {
//                cold_counter.release = true;
//            }
        }
    }

    if (hot_counter.counter) {
        hot_counter.pressed = hot_counter.release = false;
        save_config = true;
        /* detect hot counter overflow */
        watermeter_config.counter_hot_water =
                check_counter_overflow(watermeter_config.counter_hot_water +
                (watermeter_config.liters_per_pulse * hot_counter.counter));
        hot_counter.counter = 0;
#if UART_PRINTF_MODE
        printf("hot counter - %d\r\n", watermeter_config.counter_hot_water);
#endif /* UART_PRINTF_MODE */
    }

    if (cold_counter.counter) {
        cold_counter.pressed = cold_counter.release = false;
        save_config = true;
        /* detect cold counter overflow */
        watermeter_config.counter_cold_water =
                check_counter_overflow(watermeter_config.counter_cold_water +
                (watermeter_config.liters_per_pulse * cold_counter.counter));
        cold_counter.counter = 0;
#if UART_PRINTF_MODE
        printf("cold counter - %d\r\n", watermeter_config.counter_cold_water);
#endif /* UART_PRINTF_MODE */
    }

    if (save_config) {
        write_config();
    }

#if 0
    gpio_setup_up_down_resistor(HOT_GPIO, PM_PIN_PULLUP_1M);
    gpio_setup_up_down_resistor(COLD_GPIO, PM_PIN_PULLUP_1M);
#endif

    return save_config;
}

u8 counters_idle() {
    if (((hot_counter.bit != 1 && hot_counter.bit != BIT_COUNT) || hot_counter.pressed || hot_counter.counter) ||
        ((cold_counter.bit != 1 && cold_counter.bit != BIT_COUNT) || cold_counter.pressed || cold_counter.counter)  ) {
        return true;
    }
    return false;
}

/**********************************************************************
 *  For config watermeter
 */

static u16 checksum(const u8 *src_buffer, u8 len) {

    const u16 generator = 0xa010;

    u16 crc = 0xffff;

    len -= 2;

    for (const u8 *ptr = src_buffer; ptr < src_buffer + len; ptr++) {
        crc ^= *ptr;

        for (u8 bit = 8; bit > 0; bit--) {
            if (crc & 1)
                crc = (crc >> 1) ^ generator;
            else
                crc >>= 1;
        }
    }

    return crc;
}

static void get_user_data_addr() {
#ifdef ZCL_OTA
    if (mcuBootAddrGet()) {
        config_addr_start = BEGIN_USER_DATA1;
        config_addr_end = END_USER_DATA1;
#if UART_PRINTF_MODE
        printf("OTA mode enabled. MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA2);
#endif /* UART_PRINTF_MODE */
    } else {
        config_addr_start = BEGIN_USER_DATA2;
        config_addr_end = END_USER_DATA2;
#if UART_PRINTF_MODE
        printf("MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA1);
#endif /* UART_PRINTF_MODE */
    }
#else
    config_addr_start = BEGIN_USER_DATA2;
    config_addr_end = END_USER_DATA2;

#if UART_PRINTF_MODE
    printf("OTA mode desabled. MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA1);
#endif /* UART_PRINTF_MODE */

#endif
}

static void clear_user_data(u32 flash_addr) {

    u32 flash_data_size = flash_addr + USER_DATA_SIZE;

    while(flash_addr < flash_data_size) {
        flash_erase_sector(flash_addr);
        flash_addr += FLASH_SECTOR_SIZE;
    }
}

static void init_default_config() {
    memset(&watermeter_config, 0, sizeof(watermeter_config_t));
    watermeter_config.id = ID_CONFIG;
    watermeter_config.top = 0;
    watermeter_config.new_ota = 0;
    watermeter_config.liters_per_pulse = LITERS_PER_PULSE;
    watermeter_config.counter_hot_water = 0;
    watermeter_config.counter_cold_water = 0;
    watermeter_config.flash_addr_start = config_addr_start;
    watermeter_config.flash_addr_end = config_addr_end;
    default_config = true;
    write_config();
}

void init_config() {
    watermeter_config_t config_curr, config_next, config_restory;
    u8 find_config = false;

    get_user_data_addr();

    flash_read(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (u8*)&config_restory);

    u16 crc = checksum((u8*)&config_restory, sizeof(watermeter_config_t));

    if (config_restory.id != ID_CONFIG || crc != config_restory.crc) {
#if UART_PRINTF_MODE
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
        return;

    }

    if (config_restory.new_ota) {
        config_restory.new_ota = false;
        config_restory.flash_addr_start = config_addr_start;
        config_restory.flash_addr_end = config_addr_end;
        memcpy(&watermeter_config, &config_restory, sizeof(watermeter_config_t));
        default_config = true;
        write_config();
        return;
    }

    u32 flash_addr = config_addr_start;

    flash_read_page(flash_addr, sizeof(watermeter_config_t), (u8*)&config_curr);

    if (config_curr.id != ID_CONFIG) {
#if UART_PRINTF_MODE
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
        return;
    }

    flash_addr += FLASH_PAGE_SIZE;

    while(flash_addr < config_addr_end) {
        flash_read_page(flash_addr, sizeof(watermeter_config_t), (u8*)&config_next);
        crc = checksum((u8*)&config_next, sizeof(watermeter_config_t));
        if (config_next.id == ID_CONFIG && crc == config_next.crc) {
            if ((config_curr.top + 1) == config_next.top || (config_curr.top == TOP_MASK && config_next.top == 0)) {
                memcpy(&config_curr, &config_next, sizeof(watermeter_config_t));
                flash_addr += FLASH_PAGE_SIZE;
                continue;
            }
            find_config = true;
            break;
        }
        find_config = true;
        break;
    }

    if (find_config) {
        memcpy(&watermeter_config, &config_curr, sizeof(watermeter_config_t));
        watermeter_config.flash_addr_start = flash_addr-FLASH_PAGE_SIZE;
#if UART_PRINTF_MODE
        printf("Read config from flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
    } else {
#if UART_PRINTF_MODE
        printf("No active saved config! Reinit.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
    }
}

void write_config() {
    if (default_config) {
        watermeter_config.crc = checksum((u8*)&(watermeter_config), sizeof(watermeter_config_t));
        flash_erase(GEN_USER_CFG_DATA);
        flash_write(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
        flash_erase(watermeter_config.flash_addr_start);
        flash_write(watermeter_config.flash_addr_start, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
        default_config = false;
    } else {
        watermeter_config.flash_addr_start += FLASH_PAGE_SIZE;
        if (watermeter_config.flash_addr_start == config_addr_end) {
            watermeter_config.flash_addr_start = config_addr_start;
        }
        if (watermeter_config.flash_addr_start % FLASH_SECTOR_SIZE == 0) {
            flash_erase(watermeter_config.flash_addr_start);
        }
        watermeter_config.top++;
        watermeter_config.top &= TOP_MASK;
        watermeter_config.crc = checksum((u8*)&(watermeter_config), sizeof(watermeter_config_t));
        flash_write(watermeter_config.flash_addr_start, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
    }
#if UART_PRINTF_MODE
    printf("Save config to flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
}

void write_config_after_ota() {
    watermeter_config.new_ota = true;
    watermeter_config.crc = checksum((u8*)&(watermeter_config), sizeof(watermeter_config_t));
    flash_erase(GEN_USER_CFG_DATA);
    flash_write(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
}
