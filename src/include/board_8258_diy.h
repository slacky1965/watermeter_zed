#ifndef SRC_INCLUDE_BOARD_8258_DIY_H_
#define SRC_INCLUDE_BOARD_8258_DIY_H_

/************************* Configure KEY GPIO ***************************************/
#define BUTTON                  GPIO_PB6
#define PB6_INPUT_ENABLE        ON
#define PB6_DATA_OUT            OFF
#define PB6_OUTPUT_ENABLE       OFF
#define PB6_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB6     PM_PIN_PULLUP_1M

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

/**************************** Configure UART ***************************************
*    UART_TX_PA2 = GPIO_PA2,
*    UART_TX_PB1 = GPIO_PB1,
*    UART_TX_PC2 = GPIO_PC2,
*    UART_TX_PD0 = GPIO_PD0,
*    UART_TX_PD3 = GPIO_PD3,
*    UART_TX_PD7 = GPIO_PD7,
*    UART_RX_PA0 = GPIO_PA0,
*    UART_RX_PB0 = GPIO_PB0,
*    UART_RX_PB7 = GPIO_PB7,
*    UART_RX_PC3 = GPIO_PC3,
*    UART_RX_PC5 = GPIO_PC5,
*    UART_RX_PD6 = GPIO_PD6,
*/

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN       UART_TX_PD7//print
#define BAUDRATE                115200
#endif /* UART_PRINTF_MODE */

/**************************** Configure LED ******************************************/

#define LED1                        GPIO_PB1
#define PB1_FUNC                    AS_GPIO
#define PB1_OUTPUT_ENABLE           ON
#define PB1_INPUT_ENABLE            OFF


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
