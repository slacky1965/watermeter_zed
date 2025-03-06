/********************************************************************************************************
 * @file    version_cfg.h
 *
 * @brief   This is the header file for version_cfg
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

#include "../common/comm_cfg.h"

#if defined(MCU_CORE_826x)
	#if (CHIP_8269)
		#define CHIP_TYPE					TLSR_8269
	#else
		#define CHIP_TYPE					TLSR_8267
	#endif
#elif defined(MCU_CORE_8258)
		#define CHIP_TYPE					TLSR_8258_512K//TLSR_8258_1M
#elif defined(MCU_CORE_8278)
		#define CHIP_TYPE					TLSR_8278
#elif defined(MCU_CORE_B91)
		#define CHIP_TYPE					TLSR_B91
#endif

#define APP_RELEASE                         0x20        //app release 1.3
#define APP_BUILD                           0x04        //app build 01, full version - v1.3.01
#define STACK_RELEASE						0x30        //stack release 3.0
#define STACK_BUILD							0x01        //stack build 01
#define HW_VERSION                          0x01

#ifndef ZCL_BASIC_MFG_NAME
    #define ZCL_BASIC_MFG_NAME     {6,'T','E','L','I','N','K'}
#endif
#ifndef ZCL_BASIC_MODEL_ID
    #define ZCL_BASIC_MODEL_ID     {8,'T','L','S','R','8','2','6','x'}
#endif

#ifndef ZCL_BASIC_DATE_CODE
#ifdef BUILD_DATE
    #define ZCL_BASIC_DATE_CODE    BUILD_DATE
#else
    #define ZCL_BASIC_DATE_CODE    {8,'2','0','2','3','1','1','1','7'}
#endif
#endif

#ifndef ZCL_BASIC_LOC_DESC
    #define ZCL_BASIC_LOC_DESC     {7,'U','N','K','N','O','W','N'}
#endif
#ifndef ZCL_BASIC_BUILD_ID
    #define ZCL_BASIC_BUILD_ID     {10,'0','1','2','2','0','5','2','0','1','7'}
#endif
#ifndef ZCL_BASIC_SW_BUILD_ID //max 16 chars v1.3.02
    #define ZCL_BASIC_SW_BUILD_ID       {7,'v',(APP_RELEASE>>4)+0x30,'.',(APP_RELEASE&0xf)+0x30,'.',(APP_BUILD>>4)+0x30,(APP_BUILD&0xf)+0x30}
#endif

#ifndef BUILD_DATE
#define BUILD_DATE "20231118"
#endif

#define IMAGE_TYPE_WATERMETER        (0x04 | (IMAGE_TYPE_BOOT_FLAG << 7))

/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */
//#define MANUFACTURER_CODE_TELINK    0x1141//Telink ID
#define MANUFACTURER_CODE_TELINK    0x6565//Custom ID
#define	IMAGE_TYPE					((CHIP_TYPE << 8) | IMAGE_TYPE_WATERMETER)
#define	FILE_VERSION				((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)

/* Pre-compiled link configuration. */
#define IS_BOOT_LOADER_IMAGE				0
#define RESV_FOR_APP_RAM_CODE_SIZE			0
#define IMAGE_OFFSET						APP_IMAGE_ADDR
