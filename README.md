# Tuya IoTOS Embeded Demo WiFi & BLE Environment-Monitor

  [English](./README.md) | [中文](./README_zh.md) 

  ## Introduction 

This demo implements a smart WiFi environment monitor through Tuya Smart Cloud Platform, Tuya Smart APP, IoTOS Embeded WiFi &Ble SDK, using Tuya WiFi/WiFi+BLE series modules to achieve remote data monitoring, remote automatic control and other intelligent functions.

  Implemented features include.

  + Flame detection 

  + Smoke detection

  + Combustible gas detection

  + PM2.5 detection

  + Formaldehyde detection

  + Auto power off 

  

  ## Quick start 

  ### Compile & Burn

  + Download [Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t) 

  + Download the demo to the apps directory of the SDK directory 

    ```bash
    $ cd apps
    $ git clone https://github.com/tuya/tuya-iotos-embeded-demo-wifi-ble-environment-monitor
    ```

  + Execute the following command in the SDK root directory to start compilation.

    ```bash
    sh build_app.sh ./apps/tuya-iotos-embeded-demo-wifi-ble-environment-monitor tuya-iotos-embeded-demo-wifi-ble-environment-monitor 1.0.0 
    ```

  + Firmware burn-in license information please refer to: [Wi-Fi + BLE series module burn-in license](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 

  

  ### File introduction 

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
      ├── soc 					//tuya SDK soc layer interface related files
      │ ├── soc_adc.c
      │ ├── soc_pwm.c
      │ └── soc_timer.c
      ├── tuya_environment_monitor.c	// Main function implementation of environment monitor
      ├── tuya_device.c 		// Application entry file
      └── tuya_dp_process.c 	//dp point processing file
  ```

  

  ### Demo entry

  Entry file: tuya_device.c

  Important functions: device_init()

  + Call tuya_iot_wf_soc_dev_init_param() interface to initialize the SDK, configure the operating mode, the mating mode, and register various callback functions and store the firmware key and PID.

  + Calling the tuya_iot_reg_get_wf_nw_stat_cb() interface to register the device network status callback functions.

  + Call the application layer initialization function air_box_device_init()

  

  ### DataPoint related

  + Report DataPoint interface: dev_report_dp_json_async()

| function name | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------------- | ------------------------------------------------------------ |
| devid         | device id (if gateway, MCU, SOC class device then devid = NULL; if sub-device, then devid = sub-device_id) |
| dp_data       | dp structure array name                                      |
| cnt           | Number of elements of the DP(datapoint) structure array      |
| return        | OPRT_OK: Success Other: Failure                              |

  

  ### I/O List 

| RS2255 |      Smoke Sensor      | Combustible Gas Sensor | Flame Sensor |  PM2.5 Sensor   | Formaldehyde Sensor | Power Failure |
| :----: | :--------------------: | :--------------------: | :----------: | :-------------: | :-----------------: | :-----------: |
|  A P6  |           A1           |           A2           |      P8      |      UART1      |        UART2        |      P24      |
|  B P7  | RS2255_A:0, RS2255_B:1 | RS2255_A:1, RS2255_B:0 |              | Baud Rate: 9600 |   Baud Rate: 9600   |               |

  

  ## Related Documents

  Tuya Demo Center: https://developer.tuya.com/cn/demo/

  

  ## Technical Support

  You can get support for Tuya by using the following methods:

  - Developer Center: https://developer.tuya.com/cn/
  - Help Center: https://support.tuya.com/en/help
  - Technical Support Work Order Center: [https://service.console.tuya.com](https://service.console.tuya.com/) 
