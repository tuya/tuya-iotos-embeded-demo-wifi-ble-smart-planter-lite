# Tuya IoTOS Embeded Demo WiFi & BLE Smart Planter Lite

[English](./README.md) | [中文](./README_zh.md)

## 简介

本Demo通过涂鸦智能云平台、涂鸦智能APP、IoTOS Embeded WiFi &Ble SDK实现一款一个小型植物生长机。Demo详细阐述了整个实现过程、物料清单、结构图纸以及全部代码。基于涂鸦智能IOT平台和WB3S云模组实现以下功能：
* 手机 App 远程控制
* RGBW四路LED自由调配光照颜色及强度
* 自动浇水、定时补光

## 快速上手

### 编译与烧录

+ 下载[Tuya IoTOS Embeded WiFi & BLE sdk](https://github.com/tuya/tuya-iotos-embeded-sdk-wifi-ble-bk7231t) 

+ 下载Demo至SDK目录的apps目录下 

  ```bash
  $ cd apps
  $ git clone https://github.com/tuya/tuya-iotos-embeded-demo-wifi-ble-smart-planter-lite
  ```

+ 在SDK根目录下执行以下命令开始编译：

  ```bash
  sh build_app.sh apps/tuya-iotos-embeded-demo-wifi-ble-smart-planter-lite tuya-iotos-embeded-demo-wifi-ble-smart-planter-lite 1.0.0 
  ```

+ 固件烧录授权相关信息请参考：[Wi-Fi + BLE 系列模组烧录授权](https://developer.tuya.com/cn/docs/iot/device-development/burn-and-authorization/burn-and-authorize-wifi-ble-modules/burn-and-authorize-wb-series-modules?id=Ka78f4pttsytd) 

### 应用层文件介绍

```
├── src	
|    ├── plant_driver
|    |      └── plant_pwm.c         //驱动灯板相关，调用soc层的pwm接口进行再封装
|    ├── plant_soc               //tuya SDK soc层接口相关文件
|    ├── tuya_device.c           //应用层入口文件
|    ├── app_plant.c             //植物生长机主要应用层
|    └── plant_control.c         //植物生长机各个功能组件的控制逻辑相关
|
├── include				//头文件目录
|    ├── plant_driver
|    |      └── plant_pwm.h
|    ├── plant_soc
|    ├── tuya_device.h
|    ├── app_plant.h
|    └── plant_control.h
|
└── output              //编译产物
```

<br>

### 应用入口
入口文件：tuya_device.c

重要函数：device_init()

+ 调用 tuya_iot_wf_soc_dev_init_param() 接口进行SDK初始化，配置了工作模式、配网模式，同时注册了各种回调函数并存入了固件key和PID。
+ 调用 tuya_iot_reg_get_wf_nw_stat_cb() 接口注册设备网络状态回调函数。
+ 调用应用层初始化函数 app_plant_init()

<br>

### dp点相关

+ 下发dp点数据流：dev_obj_dp_cb() -> deal_dp_proc()
+ 上报dp点接口: dev_report_dp_json_async()

|函数名 | OPERATE_RET dev_report_dp_json_async(IN CONST CHAR_T *dev_id,IN CONST TY_OBJ_DP_S *dp_data,IN CONST UINT_T cnt)|
|	---|---|
|    devid | 设备id（若为网关、MCU、SOC类设备则devid = NULL;若为子设备，则devid = sub-device_id)|
|    dp_data | dp结构体数组名|
|    cnt |dp结构体数组的元素个数|
|    Return    |  OPRT_OK: 成功  Other: 失败 |

### I/O 列表

|补光灯板|浇水水泵|土壤湿度传感|
| --- | --- | --- |
|`R` P24|`Relay` P14|`ADC3` P23|
|`G` P7|||
|`B` P6|||
|`W` P8|||



## 相关文档

涂鸦Demo中心：https://developer.tuya.com/cn/demo/smart-planter



## 如何获得技术支持

您可以通过以下方法获得涂鸦的支持:

- 开发者中心：https://developer.tuya.com/cn/
- 帮助中心: https://support.tuya.com/en/help
- 技术支持工单中心: [https://service.console.tuya.com](https://service.console.tuya.com/)




