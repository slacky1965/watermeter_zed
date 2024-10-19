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

#include "app_types.h"

#define ON                              1
#define OFF                             0

#ifndef MCU_CORE_8258
#define MCU_CORE_8258                   1
#endif

/* for reporting */
#define REPORTING_MIN                   300             /* 5 min            */
#define REPORTING_MAX                   3600            /* 60 min           */

/* for polling */
#define LONG_POLL                       REPORTING_MIN
#define TIMEOUT_NET                     TIMEOUT_30MIN


/**********************************************************************
 * Product Information
 */
#define ZCL_BASIC_MFG_NAME              {10,'S','l','a','c','k','y','-','D','I','Y'}
#define ZCL_BASIC_MODEL_ID              {19,'W','a','t','e','r','m','e','t','e','r','_','T','L','S','R','8','2','5','8'}
//#define ZCL_BASIC_MODEL_ID              {15,'W','a','t','e','r','m','e','t','e','r','_','T','E','S','T'}

/**********************************************************************
 * Version configuration
 */
#include "version_cfg.h"

/* Debug mode config */
#define UART_PRINTF_MODE                ON
#define USB_PRINTF_MODE                 OFF

#define DEBUG_CONFIG                    OFF
#define DEBUG_WATERLEAK                 OFF
#define DEBUG_REPORTING                 OFF
#define DEBUG_BUTTON                    ON
#define DEBUG_COUNTER                   ON
#define DEBUG_BATTERY                   OFF
#define DEBUG_PM                        OFF
#define DEBUG_OTA                       OFF
#define DEBUG_STA_STATUS                OFF

/* PM */
#define PM_ENABLE                       ON

/* PA */
#define PA_ENABLE                       OFF

/* 32K RC or External 32K crystal */
#define CLOCK_32K_EXT_CRYSTAL           OFF

/* BDB */
#define TOUCHLINK_SUPPORT               ON
#define FIND_AND_BIND_SUPPORT           OFF

/* Board ID */
#define BOARD_826x_EVK                  0
#define BOARD_826x_DONGLE               1
#define BOARD_826x_DONGLE_PA            2
#define BOARD_8258_EVK                  3
#define BOARD_8258_EVK_V1P2             4//C1T139A30_V1.2
#define BOARD_8258_DONGLE               5
#define BOARD_8278_EVK                  6
#define BOARD_8278_DONGLE               7
#define BOARD_B91_EVK                   8
#define BOARD_B91_DONGLE                9
#define BOARD_B92_EVK                   10
#define BOARD_B92_DONGLE                11
#define BOARD_TL721X_EVK                12
#define BOARD_TL721X_DONGLE             13
#define BOARD_TL321X_EVK                14
#define BOARD_TL321X_DONGLE             15
#define BOARD_8258_DIY                  16
#define BOARD_8258_DIY_ZI               17


/* Board define */
#if defined(MCU_CORE_826x)
#if !PA_ENABLE
	#define BOARD                       BOARD_826x_DONGLE
#else
	#define BOARD                       BOARD_826x_DONGLE_PA
#endif
	#define CLOCK_SYS_CLOCK_HZ          32000000
#elif defined(MCU_CORE_8258)
#if (CHIP_TYPE == TLSR_8258_1M)
	#define FLASH_CAP_SIZE_1M           1
#endif
	#define BOARD                       BOARD_8258_DIY //BOARD_8258_DIY_ZI //BOARD_8258_DONGLE //
	#define CLOCK_SYS_CLOCK_HZ          48000000
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
    #define USER_DATA_SIZE              FLASH_OTA_IMAGE_MAX_SIZE            //0x34000
    #define BEGIN_USER_DATA1            APP_IMAGE_ADDR                      //0x0
    #define END_USER_DATA1              (BEGIN_USER_DATA1 + USER_DATA_SIZE)
    #define BEGIN_USER_DATA2            FLASH_ADDR_OF_OTA_IMAGE             //0x40000
    #define END_USER_DATA2              (BEGIN_USER_DATA2 + USER_DATA_SIZE)
    #define NV_ITEM_APP_USER_CFG        (NV_ITEM_APP_GP_TRANS_TABLE + 1)    // see sdk/proj/drivers/drv_nv.h
#elif defined(MCU_CORE_8278)
	#define FLASH_CAP_SIZE_1M           1
	#define BOARD                       BOARD_8278_DONGLE//BOARD_8278_EVK
	#define CLOCK_SYS_CLOCK_HZ          48000000
#elif defined(MCU_CORE_B91)
	#define FLASH_CAP_SIZE_1M           1
	#define BOARD                       BOARD_B91_DONGLE//BOARD_B91_EVK
	#define CLOCK_SYS_CLOCK_HZ          48000000
#elif defined(MCU_CORE_B92)
	#define FLASH_CAP_SIZE_1M           1
	#define BOARD                       BOARD_B92_DONGLE//BOARD_B92_EVK
	#define CLOCK_SYS_CLOCK_HZ          48000000
#elif defined(MCU_CORE_TL721X)
	#define FLASH_CAP_SIZE_1M           1
	#define BOARD                       BOARD_TL721X_DONGLE//BOARD_TL721X_EVK
	#define CLOCK_SYS_CLOCK_HZ          120000000
#elif defined(MCU_CORE_TL321X)
	#define FLASH_CAP_SIZE_1M           1
	#define BOARD                       BOARD_TL321X_DONGLE//BOARD_TL321X_EVK
	#define CLOCK_SYS_CLOCK_HZ          96000000
#else
	#error "MCU is undefined!"
#endif

/* Board include */
#if (BOARD == BOARD_826x_EVK)
	#include "board_826x_evk.h"
#elif (BOARD == BOARD_826x_DONGLE)
	#include "board_826x_dongle.h"
#elif (BOARD == BOARD_826x_DONGLE_PA)
	#include "board_826x_dongle_pa.h"
#elif (BOARD == BOARD_8258_DONGLE)
	#include "board_8258_dongle.h"
#elif (BOARD == BOARD_8258_EVK)
	#include "board_8258_evk.h"
#elif (BOARD == BOARD_8258_EVK_V1P2)
	#include "board_8258_evk_v1p2.h"
#elif (BOARD == BOARD_8278_EVK)
	#include "board_8278_evk.h"
#elif (BOARD == BOARD_8278_DONGLE)
	#include "board_8278_dongle.h"
#elif (BOARD == BOARD_B91_EVK)
	#include "board_b91_evk.h"
#elif (BOARD == BOARD_B91_DONGLE)
	#include "board_b91_dongle.h"
#elif (BOARD == BOARD_B92_EVK)
	#include "board_b92_evk.h"
#elif (BOARD == BOARD_B92_DONGLE)
	#include "board_b92_dongle.h"
#elif (BOARD == BOARD_TL721X_EVK)
	#include "board_tl721x_evk.h"
#elif (BOARD == BOARD_TL721X_DONGLE)
	#include "board_tl721x_dongle.h"
#elif (BOARD == BOARD_TL321X_EVK)
	#include "board_tl321x_evk.h"
#elif (BOARD == BOARD_TL321X_DONGLE)
	#include "board_tl321x_dongle.h"
#elif (BOARD == BOARD_8258_DIY)
    #include "board_8258_diy.h"
#elif (BOARD == BOARD_8258_DIY_ZI)
    #include "board_8258_diy_zi.h"
#endif


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
#define VOLTAGE_DETECT_ENABLE			OFF
#define VOLTAGE_DETECT_ADC_PIN			VOLTAGE_DETECT_PIN

/* Flash protect module */
/* Only the firmware area will be locked, the NV data area will not be locked.
 * For details, please refer to drv_flash.c file.
 */
#define FLASH_PROTECT_ENABLE            ON

/* Watch dog module */
#define MODULE_WATCHDOG_ENABLE          OFF

/* UART module */
#define	MODULE_UART_ENABLE              OFF

#if (ZBHCI_USB_PRINT || ZBHCI_USB_CDC || ZBHCI_USB_HID || ZBHCI_UART)
	#define ZBHCI_EN                    ON
#endif


/**********************************************************************
 * ZCL cluster support setting
 */
#define ZCL_IAS_ZONE_SUPPORT            ON
#define ZCL_ON_OFF_SUPPORT              ON
#define ZCL_ON_OFF_SWITCH_CFG_SUPPORT   ON
#define ZCL_GROUP_SUPPORT               ON
#define ZCL_POWER_CFG_SUPPORT           ON
#define ZCL_METERING_SUPPORT            ON
#define ZCL_OTA_SUPPORT                 ON
#if TOUCHLINK_SUPPORT
#define ZCL_ZLL_COMMISSIONING_SUPPORT   ON
#endif


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
