#include "tl_common.h"

#include "zcl_include.h"

#include "app_ui.h"
#include "watermeter_zcl.h"


_CODE_ZCL_ status_t zcl_watermeter_register(u8 endpoint, u16 manuCode, u8 attrNum, const zclAttrInfo_t attrTbl[], cluster_forAppCb_t cb)
{
    return zcl_registerCluster(endpoint, ZCL_CLUSTER_WATERMETER, manuCode, attrNum, attrTbl, NULL /*zcl_electricalMeasure_cmdHandler*/ , cb);
}


