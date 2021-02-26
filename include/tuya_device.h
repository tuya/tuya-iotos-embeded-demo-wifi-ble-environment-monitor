/*
 * @Author: yj 
 * @email: shiliu.yang@tuya.com
 * @LastEditors: yj 
 * @file name: tuya_device.c
 * @Description: air-box demo for SDK WiFi & BLE for BK7231T
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-01-23 16:33:00
 * @LastEditTime: 2021-01-27 17:00:00
 */

#ifndef __TUYA_DEVICE_H__
#define __TUYA_DEVICE_H__
    
#include "tuya_cloud_error_code.h"
    
#ifdef __cplusplus
    extern "C" {
#endif
    
#ifdef _TUYA_DEVICE_GLOBAL
    #define _TUYA_DEVICE_EXT 
#else
    #define _TUYA_DEVICE_EXT extern
#endif /* _TUYA_DEVICE_GLOBAL */ 
    
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/              
/* Exported macro ------------------------------------------------------------*/
// device information define
#define DEV_SW_VERSION USER_SW_VER
#define PRODECT_ID "bpg14yai6ert4fcr"

/* Exported functions ------------------------------------------------------- */

/**
 * @Function: device_init
 * @Description: 设备初始化处理
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
_TUYA_DEVICE_EXT \
OPERATE_RET device_init(VOID);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* tuya_device.h */
