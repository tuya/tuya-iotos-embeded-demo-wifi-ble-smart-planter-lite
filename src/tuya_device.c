/*
 * @Author: www.tuya.com
 * @email: 
 * @LastEditors:   
 * @file name: tuya_device.c
 * @Description: plant growth demo
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-01-28 15:03:00
 * @LastEditTime: 2021-01-28 15:03:00
 */
#define _TUYA_DEVICE_GLOBAL

#include "tuya_device.h"
#include "tuya_iot_wifi_api.h"
#include "tuya_hal_system.h"
#include "tuya_iot_com_api.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_gpio.h"
#include "tuya_key.h"
#include "tuya_led.h"
#include "gw_intf.h"
#include "mem_pool.h"
#include "uni_log.h"
#include "gpio_test.h"
#include "app_plant.h"

/* Private define ------------------------------------------------------------*/
#define WIFI_LED_PIN                TY_GPIOA_26 //LED pin 
#define WIFI_LED_LOW_LEVEL_ENABLE   TRUE        //TRUE：Low level lights up led; FALSE：High level lights up led
#define WIFI_LED_FAST_FLASH_MS      300         //Fast flash time 300ms 
#define WIFI_LED_LOW_FLASH_MS       500         //Slow flash time 500ms

/* Private variables ---------------------------------------------------------*/
LED_HANDLE wifi_led_handle;

/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
/**
 * @Function: wifi_state_led_reminder
 * @Description: WiFi led indicator, according to the current WiFi status, make different prompts 
 * @Input: cur_stat：current WiFi status 
 * @Output: none
 * @Return: none
 * @Others: 
 */
STATIC VOID wifi_state_led_reminder(IN CONST GW_WIFI_NW_STAT_E cur_stat)
{
    switch (cur_stat)
    {
        case STAT_LOW_POWER: //wifi connection timed out, enter low power mode
            tuya_set_led_light_type(wifi_led_handle, OL_HIGH, 0, 0); //turn off led
        break;

        case STAT_UNPROVISION: //ez network distribution mode, waiting for connection
            tuya_set_led_light_type(wifi_led_handle, OL_FLASH_LOW, WIFI_LED_FAST_FLASH_MS, 0xffff); //led flash fastly
        break;

        case STAT_AP_STA_UNCFG: //ap network distribution mode, waiting for connection
            tuya_set_led_light_type(wifi_led_handle, OL_FLASH_LOW, WIFI_LED_LOW_FLASH_MS, 0xffff); //led flash slowly
        break;

        case STAT_AP_STA_DISC:
        case STAT_STA_DISC: //ez,ap connecting
            tuya_set_led_light_type(wifi_led_handle, OL_HIGH, 0, 0); //turn off led
        break;

        case STAT_CLOUD_CONN:
        case STAT_AP_CLOUD_CONN: //connect to Tuya Cloud
            tuya_set_led_light_type(wifi_led_handle, OL_LOW, 0, 0xffff); //led constant
        break;

        default:
        break;
    }
}

/**
 * @Function: wifi_config_init
 * @Description: Initialize WiFi related devices, buttons, led indicator
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
STATIC VOID wifi_config_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;

    /* LED related initialization */ 
    op_ret = tuya_create_led_handle(WIFI_LED_PIN, TRUE, &wifi_led_handle);
    if (op_ret != OPRT_OK) {
        PR_ERR("key_init err:%d", op_ret);
        return;
    }
    tuya_set_led_light_type(wifi_led_handle, OL_HIGH, 0, 0); //turn off LED
    
    #if 0
    /* key related initialization */
    KEY_USER_DEF_S key_def;

    op_ret = key_init(NULL, 0, WIFI_KEY_TIMER_MS);
    if (op_ret != OPRT_OK) {
        PR_ERR("key_init err:%d", op_ret);
        return;
    }

    /* Initialize key related parameters */
    memset(&key_def, 0, SIZEOF(key_def));
    key_def.port = WIFI_KEY_PIN;    //key pin
    key_def.long_key_time = WIFI_KEY_LONG_PRESS_MS;
    key_def.low_level_detect = WIFI_KEY_LOW_LEVEL_ENABLE;
    key_def.lp_tp = LP_ONCE_TRIG;
    key_def.call_back = wifi_key_process;
    key_def.seq_key_detect_time = WIFI_KEY_SEQ_PRESS_MS;

    /* register key */
    op_ret = reg_proc_key(&key_def);
    if (op_ret != OPRT_OK) {
        PR_ERR("reg_proc_key err:%d", op_ret);
    }
    #endif
    return;
}

/**
 * @Function: gpio_test
 * @Description: gpio test
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
BOOL_T gpio_test(IN CONST CHAR_T *in, OUT CHAR_T *out)
{
    return gpio_test_all(in, out);
}

/**
 * @Function: mf_user_callback
 * @Description: production test authorization callback function
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID mf_user_callback(VOID)
{
    hw_reset_flash_data();
    return;
}

/**
 * @Function: prod_test
 * @Description: scan to the production test hot spot, enter the callback function
 * @Input: flag:authorization ID；rssi:signal strength
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID prod_test(BOOL_T flag,CHAR_T rssi)
{
    if (flag == FALSE || rssi < -60) 
    {
        PR_ERR("Prod test failed... flag:%d, rssi:%d", flag, rssi);
        return;
    }
    PR_NOTICE("flag:%d rssi:%d", flag, rssi);

}

/**
 * @Function: app_init
 * @Description: device initialization, set working mode
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 无
 */
VOID app_init(VOID)
{   
    
    //app_cfg_set(GWCM_OLD,prod_test);
    PR_DEBUG("wifi_mode:%d",GWCM_OLD);

    /* WiFi led initialization */
    wifi_config_init();
    
}

/**
 * @Function: pre_device_init
 * @Description: none
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID pre_device_init(VOID)
{
    PR_DEBUG("%s",tuya_iot_get_sdk_info());
    PR_DEBUG("%s:%s",APP_BIN_NAME,DEV_SW_VERSION);
    PR_NOTICE("firmware compiled at %s %s", __DATE__, __TIME__);
    SetLogManageAttr(TY_LOG_LEVEL_DEBUG);
}

/**
 * @Function: status_changed_cb
 * @Description: gw status changed callback
 * @Input: status: current status
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID status_changed_cb(IN CONST GW_STATUS_E status)
{
    PR_NOTICE("status_changed_cb is status:%d",status);

    if(GW_NORMAL == status) {
        app_report_all_dp_status();
    }else if(GW_RESET == status) {
        PR_NOTICE("status is GW_RESET");
    }
}

/**
 * @Function: upgrade_notify_cb
 * @Description: firmware download finish result callback
 * @Input: fw: firmware info
 * @Input: download_result: 0 means download succes. other means fail
 * @Input: pri_data: private data
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID upgrade_notify_cb(IN CONST FW_UG_S *fw, IN CONST INT_T download_result, IN PVOID_T pri_data)
{
    PR_DEBUG("download  Finish");
    PR_DEBUG("download_result:%d", download_result);
}

/**
 * @Function: get_file_data_cb
 * @Description: firmware download content process callback
 * @Input: fw: firmware info
 * @Input: total_len: firmware total size
 * @Input: offset: offset of this download package
 * @Input: data && len: this download package
 * @Input: pri_data: private data
 * @Output: remain_len: the size left to process in next cb
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
OPERATE_RET get_file_data_cb(IN CONST FW_UG_S *fw, IN CONST UINT_T total_len, IN CONST UINT_T offset,
                                     IN CONST BYTE_T *data, IN CONST UINT_T len, OUT UINT_T *remain_len, IN PVOID_T pri_data)
{
    PR_DEBUG("Rev File Data");
    PR_DEBUG("Total_len:%d ", total_len);
    PR_DEBUG("Offset:%d Len:%d", offset, len);

    return OPRT_OK;
}

/**
 * @Function: gw_ug_inform_cb
 * @Description: gateway ota firmware available nofity callback
 * @Input: fw: firmware info
 * @Output: none
 * @Return: int:
 * @Others: 
 */
VOID gw_ug_inform_cb(IN CONST FW_UG_S *fw)
{
    PR_DEBUG("Rev GW Upgrade Info");
    PR_DEBUG("fw->fw_url:%s", fw->fw_url);
    PR_DEBUG("fw->sw_ver:%s", fw->sw_ver);
    PR_DEBUG("fw->file_size:%d", fw->file_size);

    tuya_iot_upgrade_gw(fw, get_file_data_cb, upgrade_notify_cb, NULL);
}

/**
 * @Function: hw_reset_flash_data
 * @Description: hardware reset, erase user data from flash
 * @Input: none
 * @Output: none
 * @Return: none
 * @Others: 
 */
VOID hw_reset_flash_data(VOID)
{
    return;
}

/**
 * @Function: gw_reset_cb
 * @Description: gateway restart callback
 * @Input: type:gateway reset type
 * @Output: none
 * @Return: none
 * @Others: reset factory clear flash data
 */
VOID gw_reset_cb(IN CONST GW_RESET_TYPE_E type)
{
    PR_DEBUG("gw_reset_cb type:%d",type);
    if(GW_REMOTE_RESET_FACTORY != type) {
        PR_DEBUG("type is GW_REMOTE_RESET_FACTORY");
        return;
    }

    return;
}

/**
 * @Function: dev_obj_dp_cb
 * @Description: obj dp info cmd callback
 * @Input: dp:obj dp info
 * @Output: none
 * @Return: none
 * @Others: app send data by dpid  control device stat
 */
VOID dev_obj_dp_cb(IN CONST TY_RECV_OBJ_DP_S *dp)
{
    PR_DEBUG("dp->cid:%s dp->dps_cnt:%d",dp->cid,dp->dps_cnt);
    UCHAR_T i = 0;

    for(i = 0;i < dp->dps_cnt;i++) {
        deal_dp_proc(&(dp->dps[i]));
    }
}

/**
 * @Function: dev_raw_dp_cb
 * @Description: raw dp info cmd callback
 * @Input: dp: raw dp info
 * @Output: none
 * @Return: none
 * @Others: none
 */
VOID dev_raw_dp_cb(IN CONST TY_RECV_RAW_DP_S *dp)
{
    PR_DEBUG("raw data dpid:%d",dp->dpid);

    PR_DEBUG("recv len:%d",dp->len);
#if 0
    INT_T i = 0;
    
    for(i = 0;i < dp->len;i++) {
        PR_DEBUG_RAW("%02X ",dp->data[i]);
    }
#endif
    PR_DEBUG_RAW("\n");
    PR_DEBUG("end");
}

/**
 * @Function: dev_dp_query_cb
 * @Description: dp info query callback
 * @Input: dp_qry: query info
 * @Output: none
 * @Return: none
 * @Others: none
 */
STATIC VOID dev_dp_query_cb(IN CONST TY_DP_QUERY_S *dp_qry)
{
    PR_NOTICE("Recv DP Query Cmd");
    app_report_all_dp_status();
}

/**
 * @Function: wf_nw_status_cb
 * @Description: tuya-sdk network state check callback
 * @Input: stat: curr network status
 * @Output: none
 * @Return: none
 * @Others: none
 */
STATIC VOID wf_nw_status_cb(IN CONST GW_WIFI_NW_STAT_E stat)
{
    PR_NOTICE("wf_nw_status_cb,wifi_status:%d",stat);
    wifi_state_led_reminder(stat);
    if(stat == STAT_AP_STA_CONN || stat >= STAT_STA_CONN) {
        app_report_all_dp_status();
    }
}

/**
 * @Function: device_init
 * @Description: device initialization processing
 * @Input: none
 * @Output: none
 * @Return: OPRT_OK: success  Other: fail
 * @Others: none
 */
OPERATE_RET device_init(VOID)
{
    OPERATE_RET op_ret = OPRT_OK;
    
    UCHAR_T connect_mode = 0;

    PR_NOTICE("goto device_init!!!");

    TY_IOT_CBS_S wf_cbs = {
        status_changed_cb,\
        gw_ug_inform_cb,\
        gw_reset_cb,\
        dev_obj_dp_cb,\
        dev_raw_dp_cb,\
        dev_dp_query_cb,\
        NULL,
    };

    connect_mode = GWCM_OLD;

    op_ret = tuya_iot_wf_soc_dev_init_param(connect_mode,WF_START_SMART_FIRST,&wf_cbs,NULL,PRODECT_KEY,DEV_SW_VERSION);
    
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_wf_soc_dev_init_param error,err_num:%d",op_ret);
        return op_ret;
    }

    op_ret = tuya_iot_reg_get_wf_nw_stat_cb(wf_nw_status_cb);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_reg_get_wf_nw_stat_cb is error,err_num:%d",op_ret);
        return op_ret;
    }
    
    op_ret = app_plant_init(APP_PLANT_NORMAL);
    if(OPRT_OK != op_ret) {
        PR_ERR("plant init err!");
        return op_ret;
    }

    return op_ret;
}
