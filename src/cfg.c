#include "tl_common.h"
//#include "zb_af.h"
//#include "ota.h"
#include "app_cfg.h"

#include "cfg.h"
//#include "log.h"
//#include "bthome.h"


watermeter_config_t watermeter_config;
static u8 default_config = false;
static u32 config_addr_start = 0;
static u32 config_addr_end = 0;

extern u8 mcuBootAddrGet(void);

static u16 checksum(const u8 *src_buffer, u8 len) {

    const u16 generator = 0xa010;

    u16 crc = 0xffff;

    len -= 2;

    for (const u8 *ptr = src_buffer; ptr < src_buffer + len; ptr++) {
        crc ^= *ptr;

        for (u8 bit = 8; bit > 0; bit--) {
            if (crc & 1)
                crc = (crc >> 1) ^ generator;
            else
                crc >>= 1;
        }
    }

    return crc;
}

static void get_user_data_addr() {
    if (mcuBootAddrGet()) {
        config_addr_start = BEGIN_USER_DATA1;
        config_addr_end = END_USER_DATA1;
#if UART_PRINTF_MODE
        printf("MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA2);
#endif /* UART_PRINTF_MODE */
    } else {
        config_addr_start = BEGIN_USER_DATA2;
        config_addr_end = END_USER_DATA2;
#if UART_PRINTF_MODE
        printf("MCU boot from address: 0x%x\r\n", BEGIN_USER_DATA1);
#endif /* UART_PRINTF_MODE */
    }
}

static void clear_user_data(u32 flash_addr) {

    u32 flash_data_size = flash_addr + USER_DATA_SIZE;

    while(flash_addr < flash_data_size) {
        flash_erase_sector(flash_addr);
        flash_addr += FLASH_SECTOR_SIZE;
    }
}

static void init_default_config() {
    memset(&watermeter_config, 0, sizeof(watermeter_config_t));
    watermeter_config.id = ID_CONFIG;
    watermeter_config.top = 0;
    watermeter_config.new_ota = 0;
    watermeter_config.liters_per_pulse = LITERS_PER_PULSE;
    watermeter_config.counter_hot_water = 0;
    watermeter_config.counter_cold_water = 0;
    watermeter_config.flash_addr_start = config_addr_start;
    watermeter_config.flash_addr_end = config_addr_end;
    default_config = true;
    write_config();
}

void init_config() {
    watermeter_config_t config_curr, config_next, config_restory;
    u8 find_config = false;

    get_user_data_addr();

    flash_read(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (u8*)&config_restory);

    u16 crc = checksum((u8*)&config_restory, sizeof(watermeter_config_t));

    if (config_restory.id != ID_CONFIG || crc != config_restory.crc) {
#if UART_PRINTF_MODE
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
        return;

    }

    if (config_restory.new_ota) {
        config_restory.new_ota = false;
        config_restory.flash_addr_start = config_addr_start;
        config_restory.flash_addr_end = config_addr_end;
        memcpy(&watermeter_config, &config_restory, sizeof(watermeter_config_t));
        default_config = true;
        write_config();
        return;
    }

    u32 flash_addr = config_addr_start;

    flash_read_page(flash_addr, sizeof(watermeter_config_t), (u8*)&config_curr);

    if (config_curr.id != ID_CONFIG) {
#if UART_PRINTF_MODE
        printf("No saved config! Init.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
        return;
    }

    flash_addr += FLASH_PAGE_SIZE;

    while(flash_addr < config_addr_end) {
        flash_read_page(flash_addr, sizeof(watermeter_config_t), (u8*)&config_next);
        crc = checksum((u8*)&config_next, sizeof(watermeter_config_t));
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
#if UART_PRINTF_MODE
        printf("Read config from flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
    } else {
#if UART_PRINTF_MODE
        printf("No active saved config! Reinit.\r\n");
#endif /* UART_PRINTF_MODE */
        clear_user_data(config_addr_start);
        init_default_config();
    }
}

void write_config() {
    if (default_config) {
        watermeter_config.crc = checksum((u8*)&(watermeter_config), sizeof(watermeter_config_t));
        flash_erase(GEN_USER_CFG_DATA);
        flash_write(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
        flash_erase(watermeter_config.flash_addr_start);
        flash_write(watermeter_config.flash_addr_start, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
        default_config = false;
    } else {
        watermeter_config.flash_addr_start += FLASH_PAGE_SIZE;
        if (watermeter_config.flash_addr_start == config_addr_end) {
            watermeter_config.flash_addr_start = config_addr_start;
        }
        if (watermeter_config.flash_addr_start % FLASH_SECTOR_SIZE == 0) {
            flash_erase(watermeter_config.flash_addr_start);
        }
        watermeter_config.top++;
        watermeter_config.top &= TOP_MASK;
        watermeter_config.crc = checksum((u8*)&(watermeter_config), sizeof(watermeter_config_t));
        flash_write(watermeter_config.flash_addr_start, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
    }
#if UART_PRINTF_MODE
    printf("Save config to flash address - 0x%x\r\n", watermeter_config.flash_addr_start);
#endif /* UART_PRINTF_MODE */
}

void write_config_after_ota() {
    watermeter_config.new_ota = true;
    watermeter_config.crc = checksum((u8*)&(watermeter_config), sizeof(watermeter_config_t));
    flash_erase(GEN_USER_CFG_DATA);
    flash_write(GEN_USER_CFG_DATA, sizeof(watermeter_config_t), (u8*)&(watermeter_config));
}
