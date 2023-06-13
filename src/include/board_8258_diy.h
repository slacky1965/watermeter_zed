#ifndef SRC_INCLUDE_BOARD_8258_DIY_H_
#define SRC_INCLUDE_BOARD_8258_DIY_H_

/************************* Configure KEY GPIO ***************************************/
#define BUTTON                  GPIO_PA7
#define PA7_INPUT_ENABLE        ON
#define PA7_DATA_OUT            OFF
#define PA7_OUTPUT_ENABLE       OFF
#define PA7_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PA7     PM_PIN_PULLUP_1M

#define PM_WAKEUP_LEVEL         PM_WAKEUP_LEVEL_LOW // only for KEY

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
#define BAUDRATE                115200
#endif /* UART_PRINTF_MODE */

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



#endif /* SRC_INCLUDE_BOARD_8258_DIY_H_ */
