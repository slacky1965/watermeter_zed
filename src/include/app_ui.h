/********************************************************************************************************
 * @file    app_ui.h
 *
 * @brief   This is the header file for app_ui
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
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

#ifndef SRC_INCLUDE_APP_UI_H_
#define SRC_INCLUDE_APP_UI_H_

/**********************************************************************
 * CONSTANT
 */

/* for clock_time_exceed() */
#define TIMEOUT_TICK_1SEC   1000*1000       /* timeout 1 sec    */
#define TIMEOUT_TICK_5SEC   5*1000*1000     /* timeout 5 sec    */
#define TIMEOUT_TICK_10SEC  10*1000*1000    /* timeout 10 sec   */
#define TIMEOUT_TICK_15SEC  15*1000*1000    /* timeout 15 sec   */

/* for TL_ZB_TIMER_SCHEDULE() */
#define TIMEOUT_1SEC        1000            /* timeout 1 sec    */
#define TIMEOUT_3SEC        3*1000          /* timeout 3 sec    */
#define TIMEOUT_4SEC        4*1000          /* timeout 4 sec    */
#define TIMEOUT_5SEC        5*1000          /* timeout 5 sec    */
#define TIMEOUT_10SEC       10*1000         /* timeout 10 sec   */
#define TIMEOUT_15SEC       15*1000         /* timeout 15 sec   */
#define TIMEOUT_2MIN        120*1000        /* timeout 2 min    */
#define TIMEOUT_5MIN        300*1000        /* timeout 5 min    */
#define TIMEOUT_10MIN       600*1000        /* timeout 10 min   */

#define LED_ON                      1
#define LED_OFF                     0

/**********************************************************************
 * TYPEDEFS
 */

typedef struct _button_t {
    u8  released :1;
    u8  pressed  :1;
    u8  counter  :6;
    u16 bit;
    u32 pressed_time;
    u32 released_time;
} button_t;

typedef struct _water_conter_t {
    u8  pressed    :1;
    u8  release    :1;
    u8  counter    :6;
    u16 bit;
} water_counter_t;

/* must be no more than FLASH_PAGE_SIZE (256) bytes */
typedef struct __attribute__((packed)) _watermeter_config_t {
    u32 id;                 /* ID - ID_CONFIG                   */
    u8  new_ota;            /* new ota flag                     */
    u32 top;                /* 0x0 .. 0xFFFFFFFF                */
    u32 flash_addr_start;   /* flash page address start         */
    u32 flash_addr_end;     /* flash page address end           */
    u32 counter_hot_water;  /* Last number of liters hot water  */
    u32 counter_cold_water; /* Last number of litres cold water */
    u8  liters_per_pulse;   /* liters per pulse                 */
    u16 crc;
} watermeter_config_t;


/**********************************************************************
 * GLOBAL VARIABLES
 */

extern watermeter_config_t watermeter_config;

/**********************************************************************
 * FUNCTIONS
 */
void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime);
void light_blink_stop(void);

void light_init(void);
void light_on(void);
void light_off(void);

void led_on(u32 pin);
void led_off(u32 pin);

s32 poll_rateCb(void *arg);

s32 batteryCb();

void init_button();
void button_handler();
u8 button_idle();

void init_counters();
u8 counters_handler();
u32 check_counter_overflow(u32 check_count);
u8 counters_idle();

void init_config();
void write_config();
void write_config_after_ota();

#endif /* SRC_INCLUDE_APP_UI_H_ */
