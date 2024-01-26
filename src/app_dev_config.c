#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"

#define ID_CONFIG_0         0x0FED1410  /* Before 1.3.06 version (0 version) */
#define ID_CONFIG_1         0x00000000  /* For the future (1 version)        */
#define ID_CONFIG_LAST      0x0FED1411  /* From version 1.3.07 (last version)*/
#define TOP_MASK            0xFFFFFFFF

typedef enum {
    CONFIG_0 = 0,
    CONFIG_LAST
} config_version;

watermeter_config_t watermeter_config;

static uint8_t  default_config = false;
static uint32_t config_addr_start = 0;
static uint32_t config_addr_end = 0;

uint8_t mcuBootAddrGet(void);

static uint16_t checksum(const uint8_t *src_buffer, uint8_t len) {

    const uint16_t generator = 0xa010;

    uint16_t crc = 0xffff;

    len -= 2;

    for (const uint8_t *ptr = src_buffer; ptr < src_buffer + len; ptr++) {
        crc ^= *ptr;

        for (uint8_t bit = 8; bit > 0; bit--) {
            if (crc & 1)
                crc = (crc >> 1) ^ generator;
            else
                crc >>= 1;
        }
    }

    return crc;
}

static void get_user_data_addr(uint8_t print) {
#ifdef ZCL_OTA
    if (mcuBootAddrGet()) {
        config_addr_start = BEGIN_USER_DATA1;
        config_addr_end = END_USER_DATA1;
#if UART_PRINTF_MODE
        if (print) printf("OTA mode enabled. MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA2);
#endif /* UART_PRINTF_MODE */
    } else {
        config_addr_start = BEGIN_USER_DATA2;
        config_addr_end = END_USER_DATA2;
#if UART_PRINTF_MODE
        if (print) printf("OTA mode enabled. MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA1);
#endif /* UART_PRINTF_MODE */
    }
#else
    config_addr_start = BEGIN_USER_DATA2;
    config_addr_end = END_USER_DATA2;

#if UART_PRINTF_MODE
    if (print) printf("OTA mode desabled. MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA1);
#endif /* UART_PRINTF_MODE */

#endif
}

static void clear_user_data(uint32_t flash_addr) {

    uint32_t flash_data_size = flash_addr + USER_DATA_SIZE;

    while(flash_addr < flash_data_size) {
        flash_erase_sector(flash_addr);
        flash_addr += FLASH_SECTOR_SIZE;
    }
}

static void init_default_config() {
    memset(&watermeter_config, 0, sizeof(watermeter_config_t));
    watermeter_config.id = ID_CONFIG_LAST;
    watermeter_config.top = 0;
    watermeter_config.new_ota = 0;
    watermeter_config.counter_hot_water = 0;
    watermeter_config.counter_cold_water = 0;
    watermeter_config.liters_per_pulse = LITERS_PER_PULSE;
    watermeter_config.flash_addr_start = config_addr_start;
    watermeter_config.flash_addr_end = config_addr_end;
    default_config = true;
    write_config();
}

static void write_restore_config() {
    watermeter_config.crc = checksum((uint8_t*)&(watermeter_config), sizeof(watermeter_config_t));
    nv_flashWriteNew(1, NV_MODULE_APP,  NV_ITEM_APP_USER_CFG, sizeof(watermeter_config_t), (uint8_t*)&watermeter_config);

#if UART_PRINTF_MODE && DEBUG_CONFIG
    printf("Save restored config to nv_ram in module NV_MODULE_APP (%d) item NV_ITEM_APP_USER_CFG (%d)\r\n",
            NV_MODULE_APP,  NV_ITEM_APP_USER_CFG);
#endif /* UART_PRINTF_MODE */

}

void init_config(uint8_t print) {
    watermeter_config_t config_curr, config_next, config_restore;
    uint8_t find_config = false;
    nv_sts_t st = NV_SUCC;

    get_user_data_addr(print);

#if !NV_ENABLE
#error "NV_ENABLE must be enable in "stack_cfg.h" file!"
#endif

    st = nv_flashReadNew(1, NV_MODULE_APP,  NV_ITEM_APP_USER_CFG, sizeof(watermeter_config_t), (uint8_t*)&config_restore);
    //flash_read(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (uint8_t*)&config_restore);

    uint16_t crc = checksum((uint8_t*)&config_restore, sizeof(watermeter_config_t));

    if (st != NV_SUCC || config_restore.id != ID_CONFIG || crc != config_restore.crc) {
#if UART_PRINTF_MODE && DEBUG_CONFIG
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
        return;

    }

    if (config_restore.new_ota) {
        config_restore.new_ota = false;
        config_restore.flash_addr_start = config_addr_start;
        config_restore.flash_addr_end = config_addr_end;
        memcpy(&watermeter_config, &config_restore, sizeof(watermeter_config_t));
        default_config = true;
        write_config();
        return;
    }

    uint32_t flash_addr = config_addr_start;

    flash_read_page(flash_addr, sizeof(watermeter_config_t), (uint8_t*)&config_curr);

    if (config_curr.id != ID_CONFIG) {
#if UART_PRINTF_MODE && DEBUG_CONFIG
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
        return;
    }

    flash_addr += FLASH_PAGE_SIZE;

    while(flash_addr < config_addr_end) {
        flash_read_page(flash_addr, sizeof(watermeter_config_t), (uint8_t*)&config_next);
        crc = checksum((uint8_t*)&config_next, sizeof(watermeter_config_t));
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
#if UART_PRINTF_MODE && DEBUG_CONFIG
        printf("Read config from flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
    } else {
#if UART_PRINTF_MODE && DEBUG_CONFIG
        printf("No active saved config! Reinit.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
    }
}

void write_config() {
    if (default_config) {
        write_restore_config();
        flash_erase(watermeter_config.flash_addr_start);
        flash_write(watermeter_config.flash_addr_start, sizeof(watermeter_config_t), (uint8_t*)&(watermeter_config));
        default_config = false;
#if UART_PRINTF_MODE && DEBUG_CONFIG
        printf("Save config to flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
    } else {
        if (!watermeter_config.new_ota) {
            watermeter_config.flash_addr_start += FLASH_PAGE_SIZE;
            if (watermeter_config.flash_addr_start == config_addr_end) {
                watermeter_config.flash_addr_start = config_addr_start;
            }
            if (watermeter_config.flash_addr_start % FLASH_SECTOR_SIZE == 0) {
                flash_erase(watermeter_config.flash_addr_start);
            }
            watermeter_config.top++;
            watermeter_config.top &= TOP_MASK;
            watermeter_config.crc = checksum((uint8_t*)&(watermeter_config), sizeof(watermeter_config_t));
            flash_write(watermeter_config.flash_addr_start, sizeof(watermeter_config_t), (uint8_t*)&(watermeter_config));
#if UART_PRINTF_MODE && DEBUG_CONFIG
            printf("Save config to flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
        } else {
            write_restore_config();
        }
    }

}

