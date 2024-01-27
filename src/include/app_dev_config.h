#ifndef SRC_INCLUDE_APP_DEV_CONFIG_H_
#define SRC_INCLUDE_APP_DEV_CONFIG_H_

/* must be no more than FLASH_PAGE_SIZE (256) bytes */
typedef struct __attribute__((packed)) _watermeter_config_t {
    uint32_t id;                     /* ID - ID_CONFIG                   */
    uint8_t  new_ota;                /* new ota flag                     */
    uint32_t top;                    /* 0x0 .. 0xFFFFFFFF                */
    uint32_t flash_addr_start;       /* flash page address start         */
    uint32_t flash_addr_end;         /* flash page address end           */
    uint32_t counter_hot_water;      /* Last number of liters hot water  */
    uint32_t counter_cold_water;     /* Last number of litres cold water */
    uint16_t  liters_per_pulse;      /* liters per pulse                 */
    uint16_t crc;
} watermeter_config_t;

typedef struct __attribute__((packed)) _watermeter_config1306_t {
    uint32_t id;                     /* ID - ID_CONFIG                   */
    uint8_t  new_ota;                /* new ota flag                     */
    uint32_t top;                    /* 0x0 .. 0xFFFFFFFF                */
    uint32_t flash_addr_start;       /* flash page address start         */
    uint32_t flash_addr_end;         /* flash page address end           */
    uint32_t counter_hot_water;      /* Last number of liters hot water  */
    uint32_t counter_cold_water;     /* Last number of litres cold water */
    uint8_t  liters_per_pulse;       /* liters per pulse                 */
    uint16_t crc;
} watermeter_config1306_t;

typedef struct __attribute__((packed)) _config_restore_t {
    uint32_t id;                     /* ID - ID_CONFIG                   */
    uint8_t  new_ota;                /* new ota flag                     */
    uint32_t counter_hot_water;      /* Last number of liters hot water  */
    uint32_t counter_cold_water;     /* Last number of litres cold water */
    uint16_t  liters_per_pulse;       /* liters per pulse                 */
    uint16_t crc;
} config_restore_t;

extern watermeter_config_t watermeter_config;

void init_config(uint8_t print);
void write_config();
int32_t write_config_testingCb(void *arg);

#endif /* SRC_INCLUDE_APP_DEV_CONFIG_H_ */
