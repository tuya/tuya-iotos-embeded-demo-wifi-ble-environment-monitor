/**
 * @File: tuya_dp_process.h 
 * @Author: shiliu.yang@tuya.com 
 * @Last Modified time: 2021-01-18 
 * @Description: 
 */
#ifndef __TUYA_DP_PROCESS_H_
#define __TUYA_DP_PROCESS_H_

#include "tuya_cloud_com_defs.h"
#include "tuya_cloud_types.h"

/* dp 点枚举 */
typedef UCHAR_T AIR_BOX_DP_T;
#define DP_PREHEAT              20  //预热 
#define DP_PM25_SENSOR_VALUE    33  //pm2.5 
#define DP_CH2O_SENSOR_VALUE    39  //甲醛传感器 
#define DP_SMOKE_SENSOR_STATE   13  //烟雾传感器 
#define DP_GAS_SENSOR_STATE     25  //燃气传感器 
#define DP_FLAME_SENSOR_STATE   101 //火焰传感器

//设备状态枚举值
typedef UCHAR_T SENSOR_STATE_T;
#define ALARM   0   //警报 
#define NORMAL  1   //正常

//设备状态结构体
typedef struct {
    /* 设备 dp 点 */
    AIR_BOX_DP_T    dp_preheat;     //预热dp
    AIR_BOX_DP_T    dp_pm25_value;  //pm2.5 dp 点
    AIR_BOX_DP_T    dp_ch2o_value;  //甲醛 dp 点
    AIR_BOX_DP_T    dp_smoke_state; //烟雾 dp 点
    AIR_BOX_DP_T    dp_gas_state;   //燃气 dp 点
    AIR_BOX_DP_T    dp_flame_state; //火焰 dp 点

    /* 采集传感器数值 */
    BOOL_T          preheat_state;
    INT_T           pm25_value;
    INT_T           ch2o_value;
    SENSOR_STATE_T  smoke_state;
    SENSOR_STATE_T  gas_state;
    SENSOR_STATE_T  flame_state;
}AIR_BOX_DEVICE_S;


extern AIR_BOX_DEVICE_S gs_environment_monitor;

VOID updata_dp_all(VOID);
VOID dp_process(IN CONST TY_OBJ_DP_S *root);
VOID updata_dp_single(BYTE_T dp_id, DP_PROP_TP_E dp_type, INT_T dp_value);


#endif

