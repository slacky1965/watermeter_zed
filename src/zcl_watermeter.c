#include "tl_common.h"

#include "zcl_include.h"

#include "app_ui.h"
#include "zcl_watermeter.h"


_CODE_ZCL_ status_t zcl_custom_watermeter_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
    return zcl_registerCluster(endpoint, ZCL_CLUSTER_CUSTOM_WATERMETER, manuCode, attrNum, attrTbl, NULL /*zcl_electricalMeasure_cmdHandler*/ , cb);
}

