/**
 * @File: tuya_dp_process.c 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-18  
 * @Description: 
 */

#include "tuya_dp_process.h"
#include "tuya_iot_com_api.h"
#include "tuya_cloud_wifi_defs.h"
#include "uni_log.h"

AIR_BOX_DEVICE_S gs_environment_monitor = {
    .dp_preheat = DP_PREHEAT,     //预热dp
    .dp_pm25_value = DP_PM25_SENSOR_VALUE,  //pm2.5 dp 点
    .dp_ch2o_value = DP_CH2O_SENSOR_VALUE,  //甲醛 dp 点
    .dp_smoke_state = DP_SMOKE_SENSOR_STATE, //烟雾 dp 点
    .dp_gas_state = DP_GAS_SENSOR_STATE,   //燃气 dp 点
    .dp_flame_state = DP_FLAME_SENSOR_STATE, //火焰 dp 点

    .preheat_state = 1,
    .pm25_value = 0,
    .ch2o_value = 0,
    .smoke_state = NORMAL,
    .gas_state = NORMAL,
    .flame_state = NORMAL
};

 /***********************************************************
 *   Function:  dp_process
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    处理 dp 数据 
 ***********************************************************/
VOID dp_process(IN CONST TY_OBJ_DP_S *root)
{
    UCHAR_T dpid;

    dpid = root->dpid;
    PR_DEBUG("dpid:%d",dpid);

    return;
}

 /***********************************************************
 *   Function:  updata_dp_all
 *   Input:     none
 *   Output:    none
 *   Return:    none
 *   Notice:    上报所有 dp 
 ***********************************************************/
VOID updata_dp_all(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    INT_T dp_cnt = 0;
    dp_cnt = 6;

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    /* 预热状态 */
    dp_arr[0].dpid = gs_environment_monitor.dp_preheat;
    dp_arr[0].type = PROP_BOOL;
    dp_arr[0].time_stamp = 0;
    dp_arr[0].value.dp_bool = gs_environment_monitor.preheat_state;

    /* pm2.5 */ 
    dp_arr[1].dpid = gs_environment_monitor.dp_pm25_value;
    dp_arr[1].type = PROP_VALUE;
    dp_arr[1].time_stamp = 0;
    dp_arr[1].value.dp_value = gs_environment_monitor.pm25_value;

    /* 甲醛 */ 
    dp_arr[2].dpid = gs_environment_monitor.dp_ch2o_value;
    dp_arr[2].type = PROP_VALUE;
    dp_arr[2].time_stamp = 0;
    dp_arr[2].value.dp_value = gs_environment_monitor.ch2o_value;

    /* 烟雾 */ 
    dp_arr[3].dpid = gs_environment_monitor.dp_smoke_state;
    dp_arr[3].type = PROP_ENUM;
    dp_arr[3].time_stamp = 0;
    dp_arr[3].value.dp_enum = gs_environment_monitor.smoke_state;
    /* 火焰 */
    dp_arr[4].dpid = gs_environment_monitor.dp_flame_state;
    dp_arr[4].type = PROP_ENUM;
    dp_arr[4].time_stamp = 0;
    dp_arr[4].value.dp_enum = gs_environment_monitor.flame_state;
    /* 燃气 */
    dp_arr[5].dpid = gs_environment_monitor.dp_gas_state;
    dp_arr[5].type = PROP_ENUM;
    dp_arr[5].time_stamp = 0;
    dp_arr[5].value.dp_enum = gs_environment_monitor.gas_state;

    op_ret = dev_report_dp_json_async(NULL , dp_arr, dp_cnt);
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    PR_DEBUG("dp_query report_all_dp_data");
    
    return;
}

 /***********************************************************
 *   Function:  updata_dp_single
 *   Input:     dp_id   :   dp 点值
 *              dp_type :   PROP_BOOL, PROP_BITMAP, PROP_ENUM, PROP_STR(该数据类型是一个指针，dp_value 应指向数据地址), PROP_VALUE
 *              dp_value:   根据 dp_type 输入对应类型的数据
 *   Output:    none
 *   Return:    none
 *   Notice:    上报单个 dp 
 ***********************************************************/
VOID updata_dp_single(BYTE_T dp_id, DP_PROP_TP_E dp_type, INT_T dp_value)
{
    OPERATE_RET op_ret = OPRT_OK;
    GW_WIFI_NW_STAT_E wifi_state = 0xFF;

    INT_T dp_cnt = 1;

    /* 没有连接到路由器，退出 */
    op_ret = get_wf_gw_nw_status(&wifi_state);
    if (OPRT_OK != op_ret) {
        PR_ERR("get wifi state err");
        return;
    }
    if (wifi_state == STAT_LOW_POWER) {
        return;
    }

    TY_OBJ_DP_S *dp_arr = (TY_OBJ_DP_S *)Malloc(dp_cnt*SIZEOF(TY_OBJ_DP_S));
    if(NULL == dp_arr) {
        PR_ERR("malloc failed");
        return;
    }

    memset(dp_arr, 0, dp_cnt*SIZEOF(TY_OBJ_DP_S));

    dp_arr[0].dpid = dp_id;
    dp_arr[0].type = dp_type;
    dp_arr[0].time_stamp = 0;
    if (dp_type = PROP_BOOL) {
        dp_arr[0].value.dp_bool = (BOOL_T)dp_value;
    } else if (dp_type = PROP_BITMAP) {
        dp_arr[0].value.dp_bitmap = (UINT_T)dp_value;
    } else if (dp_type = PROP_ENUM) {
        dp_arr[0].value.dp_enum = (UINT_T)dp_value;
    } else if (dp_type = PROP_STR) {
        dp_arr[0].value.dp_str = (CHAR_T*)dp_value;
    } else if (dp_type = PROP_VALUE) {
        dp_arr[0].value.dp_value = (INT_T)dp_value;
    } else {
        ;
    }
    
    op_ret = dev_report_dp_json_async(NULL , dp_arr, dp_cnt);
    Free(dp_arr);
    dp_arr = NULL;
    if(OPRT_OK != op_ret) {
        PR_ERR("dev_report_dp_json_async relay_config data error,err_num",op_ret);
    }

    return;
}

