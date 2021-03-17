# Tuya Environment Monitor Demo Through IoTOS Embedded Wi-Fi and Bluetooth Low Energy SDK

  [English](./README.md) | [中文](./README_zh.md) 

## Overview

This demo describes how to develop an environment monitor by using the Tuya IoT Platform, Tuya Smart app, IoTOS Embeded Wi-Fi and Bluetooth Low Energy SDK. It uses Tuya Wi-Fi modules and Wi-Fi + Blueooth Low Energy modules to provide smart functions such as remote data monitoring and remote automatic control.

The following features have been implemented:

  + Flame detection 

  + Smoke detection

  + Combustible gas detection

  + PM2.5 detection

  + Formaldehyde detection

  + Auto power off 

  

  ## Get started 

  ### Build and flash

  + Download [Tuya IoTOS Embedded Wi-Fi & Bluetooth Low Energy SDK](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t).` 

  + Download the demo to the apps directory of the SDK directory. 

    ```bash
    $ cd apps
    $ git clone https://github.com/tuya/tuya-iotos-embeded-demo-wifi-ble-environment-monitor
    ```

  + Execute the following command in the SDK root directory to start building.

    ```bash
    sh build_app.sh ./apps/tuya-iotos-embeded-demo-wifi-ble-environment-monitor tuya-iotos-embeded-demo-wifi-ble-environment-monitor 1.0.0 
    ```

  + For more information about firmware flashing and authorization, see [Wi-Fi + BLE Modules](https://developer.tuya.com/en/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules?categoryId=864069). 


  ### Files

  ```
  ├── include
  │ ├── common
  │ │ ├── device_config_load.h
  │ │ ├── light_printf.h
  │ │ ├── light_types.h
  │ ├── soc
  │ │ ├── soc_adc.h
  │ │ ├── soc_pwm.h
  │ │ ├── soc_timer.h
  │ ├── tuya_environment_monitor.h
  │ ├── tuya_device.h
  │ └── tuya_dp_process.h
  └── src
      ├── soc 					// Tuya SDK SoC layer API file
      │ ├── soc_adc.c
      │ ├── soc_pwm.c
      │ └── soc_timer.c
      ├── tuya_environment_monitor.c	// Main function implementation of the environment monitor
      ├── tuya_device.c 		// Application entry file
      └── tuya_dp_process.c 	// Data point (DP) processing file
  ```

  
### Demo entry

Entry file: `tuya_device.c`

Important functions: `device_init()`

+ Call the `tuya_iot_wf_soc_dev_init_param()` interface to initialize the SDK, configure the operating mode and pairing mode, register various callback functions, and store the firmware key and PID.

+ Call the `tuya_iot_reg_get_wf_nw_stat_cb()` interface to register the callback function of the device network status.

+ Call the application layer initialization function `air_box_device_init()`.

  

### Data point related interface

+ Report the DP through `dev_report_dp_json_async()`.

| Function name | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------------- | ------------------------------------------------------------ |
| devid         | The device ID (if the device is a gateway, MCU, SOC device, then devid = NULL. If it is a sub-device, then devid = sub-device_id) |
| dp_data       | The DP struct array name                                      |
| cnt           | The number of elements of the DP struct array      |
| return        | OPRT_OK: success, other: failure                              |

  

  ### I/O List 

| RS2255 |      Smoke sensor      | Combustible gas sensor | Flame sensor |  PM2.5 sensor   | Formaldehyde sensor | Power off |
| :----: | :--------------------: | :--------------------: | :----------: | :-------------: | :-----------------: | :-----------: |
|  A P6  |           A1           |           A2           |      P8      |      UART1      |        UART2        |      P24      |
|  B P7  | RS2255_A:0, RS2255_B:1 | RS2255_A:1, RS2255_B:0 |              | Baud Rate: 9600 |   Baud Rate: 9600   |               |

  

  ## Documentation

  Tuya Demo Center: https://developer.tuya.com/cn/demo/
  

  ## Technical Support

  You can get support from Tuya with the following methods:

  - Developer Center: https://developer.tuya.com/cn/
  - Help Center: https://support.tuya.com/en/help
  - Technical Support Work Order Center: [https://service.console.tuya.com](https://service.console.tuya.com/) 
