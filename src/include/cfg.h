#ifndef SRC_INCLUDE_CFG_H_
#define SRC_INCLUDE_CFG_H_

#include "stdint.h"

#define ID_CONFIG       0x0FED1410
#define TOP_MASK        0xFFFFFFFF

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

extern watermeter_config_t watermeter_config;

void init_config();
void write_config();
void write_config_after_ota();

#endif /* SRC_INCLUDE_CFG_H_ */
