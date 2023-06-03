/********************************************************************************************************
 * @file    app_cfg.h
 *
 * @brief   This is the header file for app_cfg
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

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define ON                      1
#define OFF                     0

#ifndef MCU_CORE_8258
#define MCU_CORE_8258   1
#endif

#define TIMEOUT_5SEC    5*1000*1000     /* timeout 5 sec    */
#define TIMEOUT_10SEC   10*1000*1000    /* timeout 10 sec   */
#define TIMEOUT_15SEC   15*1000*1000    /* timeout 15 sec   */


/**********************************************************************
 * Product Information
 */

#define ZCL_BASIC_MFG_NAME     {3,'D','I','Y'}
#define ZCL_BASIC_MODEL_ID     {19,'W','a','t','e','r','m','e','t','e','r','_','T','L','S','R','8','2','5','8'}

/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/* Debug mode config */
#define	UART_PRINTF_MODE				1
#define USB_PRINTF_MODE         		0

/* PM */
#define PM_ENABLE						1

/* PA */
#define PA_ENABLE						0

/* BDB */
#define TOUCHLINK_SUPPORT				1
#define FIND_AND_BIND_SUPPORT			0

///* Board ID */
//#define BOARD_826x_EVK					0
//#define BOARD_826x_DONGLE				1
//#define BOARD_826x_DONGLE_PA			2
//#define BOARD_8258_EVK					3
//#define BOARD_8258_EVK_V1P2				4//C1T139A30_V1.2
//#define BOARD_8258_DONGLE				5
//#define BOARD_8278_EVK					6
//#define BOARD_8278_DONGLE				7
//#define BOARD_B91_EVK					8
//#define BOARD_B91_DONGLE				9

#define CLOCK_SYS_CLOCK_HZ  		48000000


/* Voltage detect module */
/* If VOLTAGE_DETECT_ENABLE is set,
 * 1) if MCU_CORE_826x is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * and there is no need to configure the detection IO port;
 * 2) if MCU_CORE_8258 or MCU_CORE_8278 is defined, the DRV_ADC_VBAT_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be in a floating state.
 * 3) if MCU_CORE_B91 is defined, the DRV_ADC_BASE_MODE mode is used by default,
 * we need to configure the detection IO port, and the IO must be connected to the target under test,
 * such as VCC.
 */
#define VOLTAGE_DETECT_ENABLE						1

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE						0

/* UART module */
#define	MODULE_UART_ENABLE							0

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART)
	#define ZBHCI_EN								1
#endif

/************************* Configure KEY GPIO ***************************************/
#define BUTTON                  GPIO_PA7
#define PA7_INPUT_ENABLE        ON
#define PA7_DATA_OUT            OFF
#define PA7_OUTPUT_ENABLE       OFF
#define PA7_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PA7     PM_PIN_PULLUP_1M

/************************* Configure counters ***************************************/
#define LITERS_PER_PULSE        10              /* How many liters per one pulse */
#define COUNTERS_OVERFLOW       100000000       /* counters overflow             */

/************************* Configure HOT GPIO ***************************************/
#define HOT_GPIO                GPIO_PB7
#define PB7_INPUT_ENABLE        ON
#define PB7_DATA_OUT            OFF
#define PB7_OUTPUT_ENABLE       OFF
#define PB7_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB7     PM_PIN_PULLUP_1M

/************************* Configure COLD GPIO **************************************/
#define COLD_GPIO               GPIO_PB4
#define PB4_INPUT_ENABLE        ON
#define PB4_DATA_OUT            OFF
#define PB4_OUTPUT_ENABLE       OFF
#define PB4_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB4     PM_PIN_PULLUP_1M

/**************************** Configure UART ***************************************/
#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN       UART_TX_PB1//print
//#define BAUDRATE                115200
#endif /* UART_PRINTF_MODE */

/************************* Configure VBAT GPIO ***************************************/
#define VOLTAGE_DETECT_ADC_PIN  GPIO_PC4
//#define GPIO_VBAT               GPIO_PC4
#define PC4_INPUT_ENABLE        OFF
#define PC4_OUTPUT_ENABLE       ON
#define PC4_DATA_OUT            ON
#define PC4_FUNC                AS_GPIO
#define SHL_ADC_VBAT            9 //C4P

#define PM_WAKEUP_LEVEL         PM_WAKEUP_LEVEL_LOW // only for KEY

/************************* For 512K Flash only ***************************************/
/* Flash map:
  0x00000 Old Firmware bin
  0x34000 NV_1
  0x40000 OTA New bin storage Area
  0x76000 MAC address
  0x77000 C_Cfg_Info
  0x78000 U_Cfg_Info
  0x7A000 NV_2
  0x80000 End Flash
 */
#define USER_DATA_SIZE          0x34000
#define BEGIN_USER_DATA1        0x00000
#define END_USER_DATA1          (BEGIN_USER_DATA1 + USER_DATA_SIZE)
#define BEGIN_USER_DATA2        0x40000
#define END_USER_DATA2          (BEGIN_USER_DATA2 + USER_DATA_SIZE)
#define GEN_USER_CFG_DATA       END_USER_DATA2

/**********************************************************************
 * ZCL cluster support setting
 */
//#define ZCL_LEVEL_CTRL_SUPPORT			1
#define ZCL_GROUP_SUPPORT               1
#define ZCL_OTA_SUPPORT                 1
#if TOUCHLINK_SUPPORT
#define ZCL_ZLL_COMMISSIONING_SUPPORT   1
#endif
#define ZCL_WATERMETER_SUPPORT          1


/**********************************************************************
 * Stack configuration
 */
#include "stack_cfg.h"


/**********************************************************************
 * EV configuration
 */
typedef enum{
	EV_POLL_ED_DETECT,
	EV_POLL_HCI,
    EV_POLL_IDLE,
	EV_POLL_MAX,
}ev_poll_e;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
