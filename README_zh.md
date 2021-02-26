# Tuya IoTOS Embeded Demo WiFi & BLE Environment-Monitor

[English](./README.md) | [中文](./README_zh.md)

## 简介 

本Demo通过涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded WiFi &Ble SDK实现一款环境安全卫士，使用涂鸦WiFi/WiFi+BLE系列模组，实现远程数据监测、远程自动控制等智能化功能。

已实现功能包括：

+ 火焰检测 

+ 烟雾检测

+ 可燃气体检测

+ PM2.5检测

+ 甲醛检测

+ 自动断电 



## 快速上手 

### 编译与烧录
+ 下载[Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t) 

+ 下载Demo至SDK目录的apps目录下 

  ```bash
  $ cd apps
  $ git clone https://github.com/tuya/tuya-iotos-embeded-demo-wifi-ble-environment-monitor
  ```
  
+ 在SDK根目录下执行以下命令开始编译：

  ```bash
  sh build_app.sh ./apps/tuya-iotos-embeded-demo-wifi-ble-environment-monitor tuya-iotos-embeded-demo-wifi-ble-environment-monitor 1.0.0 
  ```

+ 固件烧录授权相关信息请参考：[Wi-Fi + BLE 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 



### 文件介绍 

```
├── include
│   ├── common
│   │   ├── device_config_load.h
│   │   ├── light_printf.h
│   │   └── light_types.h
│   ├── soc
│   │   ├── soc_adc.h
│   │   ├── soc_pwm.h
│   │   └── soc_timer.h
│   ├── tuya_environment_monitor.h
│   ├── tuya_device.h
│   └── tuya_dp_process.h
└── src
    ├── soc					//tuya SDK soc层接口相关文件
    │   ├── soc_adc.c
    │   ├── soc_pwm.c
    │   └── soc_timer.c
    ├── tuya_environment_monitor.c		//环境安全卫士主要功能实现
    ├── tuya_device.c		//应用层入口文件
    └── tuya_dp_process.c	//dp点处理文件
```



### Demo入口

入口文件：tuya_device.c

重要函数：device_init()

+ 调用 tuya_iot_wf_soc_dev_init_param() 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。

+ 调用 tuya_iot_reg_get_wf_nw_stat_cb() 接口注册设备网络状态回调函数。

+ 调用应用层初始化函数 air_box_device_init()



### DP点相关

+ 上报dp点接口: dev_report_dp_json_async()

| 函数名  | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt) |
| ------- | ------------------------------------------------------------ |
| devid   | 设备id（若为网关、MCU、SOC类设备则devid = NULL;若为子设备，则devid = sub-device_id) |
| dp_data | dp结构体数组名                                               |
| cnt     | dp结构体数组的元素个数                                       |
| Return  | OPRT_OK: 成功  Other: 失败                                   |



### I/O 列表 

| RS2255 |       烟雾传感器       |     可燃气体传感器     | 火焰传感器 | PM2.5传感器  |  甲醛传感器  | 断电 |
| :----: | :--------------------: | :--------------------: | :--------: | :----------: | :----------: | :--: |
|  A P6  |           A1           |           A2           |     P8     |    UART1     |    UART2     | P24  |
|  B P7  | RS2255_A:0, RS2255_B:1 | RS2255_A:1, RS2255_B:0 |            | 波特率：9600 | 波特率：9600 |      |



## 相关文档

涂鸦Demo中心：https://developer.tuya.com/cn/demo/



## 技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：https://developer.tuya.com/cn/
- 帮助中心: https://support.tuya.com/en/help
- 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/) 
