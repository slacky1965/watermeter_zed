#include "tl_common.h"
#include "zcl_include.h"

#include "watermeter.h"


int32_t poll_rateAppCb(void *arg) {

    uint32_t poll_rate = zb_getPollRate();

    if (poll_rate == g_watermeterCtx.long_poll) {
        zb_setPollRate(g_watermeterCtx.short_poll);
        return TIMEOUT_30SEC;
    }

    zb_setPollRate(g_watermeterCtx.long_poll);

    return g_watermeterCtx.long_poll;
}

int32_t delayedMcuResetCb(void *arg) {

    //printf("mcu reset\r\n");
    set_regDeepSleep();
    zb_resetDevice();
    return -1;
}

int32_t delayedFactoryResetCb(void *arg) {

    //printf("factory reset\r\n");
    set_regDeepSleep();
    zb_resetDevice2FN();
//    TL_ZB_TIMER_SCHEDULE(delayedMcuResetCb, NULL, TIMEOUT_3SEC);
//    zb_factoryReset();
//    //sleep_ms(500);
//    zb_resetDevice();
    return -1;
}

int32_t delayedFullResetCb(void *arg) {

    //printf("full reset\r\n");
    return -1;
}
