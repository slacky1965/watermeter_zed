#ifndef SRC_INCLUDE_BOARD_8258_DIY_H_
#define SRC_INCLUDE_BOARD_8258_DIY_H_

/************************* Configure KEY GPIO ***************************************/

#define MAX_BUTTON_NUM  2

#define BUTTON                      GPIO_PC1
#define PC1_FUNC                    AS_GPIO
#define PC1_OUTPUT_ENABLE           0
#define PC1_INPUT_ENABLE            1
#define PULL_WAKEUP_SRC_PC1         PM_PIN_PULLUP_1M //PM_PIN_PULLUP_10K //

#define BUTTON1  BUTTON

#define BUTTON2                     GPIO_PC4
#define PC4_FUNC                    AS_GPIO
#define PC4_OUTPUT_ENABLE           0
#define PC4_INPUT_ENABLE            1
#define PULL_WAKEUP_SRC_PC4         PM_PIN_PULLUP_1M //PM_PIN_PULLUP_10K //

#define PM_WAKEUP_LEVEL             PM_WAKEUP_LEVEL_LOW

enum{
    VK_SW1 = 0x01,
    VK_SW2 = 0x02
};

#define KB_MAP_NORMAL   {\
        {VK_SW1,}, \
        {VK_SW2,}, }

#define KB_MAP_NUM      KB_MAP_NORMAL
#define KB_MAP_FN       KB_MAP_NORMAL

#define KB_DRIVE_PINS  {NULL }
#define KB_SCAN_PINS   {BUTTON1,  BUTTON2}


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
#define DEBUG_INFO_TX_PIN       UART_TX_PD3//print
#define BAUDRATE                9600
#endif /* UART_PRINTF_MODE */

// UART
#if ZBHCI_UART
    #define UART_TX_PIN             UART_TX_PD7
    #define UART_RX_PIN             UART_RX_PA0

    #define UART_PIN_CFG()          uart_gpio_set(UART_TX_PIN, UART_RX_PIN);// uart tx/rx pin set
#endif



/**************************** Configure LED ******************************************/

#define LED3                        GPIO_PB5
#define PB5_FUNC                    AS_GPIO
#define PB5_OUTPUT_ENABLE           ON
#define PB5_INPUT_ENABLE            OFF

#define LED2                        GPIO_PB6
#define PB6_FUNC                    AS_GPIO
#define PB6_OUTPUT_ENABLE           ON
#define PB6_INPUT_ENABLE            OFF

#define LED1                        GPIO_PB7
#define PB7_FUNC                    AS_GPIO
#define PB7_OUTPUT_ENABLE           ON
#define PB7_INPUT_ENABLE            OFF

/**************************************************************************************/

#endif /* SRC_INCLUDE_BOARD_8258_DIY_H_ */
