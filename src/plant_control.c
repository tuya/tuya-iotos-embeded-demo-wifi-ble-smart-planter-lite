/*
 * @Author: zgw
 * @email: liang.zhang@tuya.com
 * @LastEditors: zgw
 * @file name: sht21.h
 * @Description: SHT21 IIC drive src file
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2020-12-16 18:51:29
 * @LastEditTime: 20120-12-16 18:51:29
 */

#include "plant_control.h"
#include "soc_adc.h"
#include "tuya_gpio.h"
#include "plant_pwm.h"
#include "sys_timer.h"
/***********************************************************
*************************types define***********************
***********************************************************/
typedef enum
{
    LOW = 0,
    HIGH,
}default_level;



USER_PWM_DUTY_T user_pwm_duty = {0};

PLANT_CTRL_DATA_T plant_ctrl_data = {FALSE, FALSE, FALSE, 0, 0, 0, 0, 0, 0};
PLANT_REPORT_DATA_T plant_report_data = {0};
SOIL_MOISTURE_THRESHOLD_T soil_moisture_threshold = {3000, 2300, 1800};

STATIC TIMER_ID light_timer;

STATIC USHORT_T light_flag_min = 0;

STATIC USHORT_T soil_moisture_value = 0;

STATIC UINT8 ADD_WATER_COUNT = 0;
STATIC UINT8 ADD_WATER_READY = 1;
/***********************************************************
*************************IO control device define***********
***********************************************************/

#define WATER_PUMP_PORT                    (14)
#define WATER_PUMP_LEVEL                   HIGH

/***********************************************************
*************************about adc init*********************
***********************************************************/


#define TEMP_ADC_DATA_LEN           (4)

tuya_adc_dev_t tuya_adc;

/***********************************************************
*************************about iic init*********************
***********************************************************/


/***********************************************************
*************************function***************************
***********************************************************/

STATIC VOID __ctrl_gpio_init(CONST TY_GPIO_PORT_E port, CONST BOOL_T high)
{   
    //Set IO port as output mode
    tuya_gpio_inout_set(port, FALSE);
    //Set IO port level
    tuya_gpio_write(port, high);
}

STATIC VOID light_timer_cb(UINT_T timerID, PVOID_T pTimerArg)
{
    light_flag_min--;
    plant_report_data.Countdown_left = light_flag_min;
    if(!light_flag_min) {
        plant_ctrl_data.Countdown_set = cancel;
        plant_pwm_off();
    }
}

VOID plant_device_init(VOID)
{
    INT_T op_ret = 0;
    
    op_ret = sys_add_timer(light_timer_cb, NULL, &light_timer);
    if(op_ret != OPRT_OK) {
        PR_ERR("add light timer failed");
    }

    plant_pwm_init();

    //gpio init
    __ctrl_gpio_init(WATER_PUMP_PORT, WATER_PUMP_LEVEL);

    //adc driver init
    tuya_adc.priv.pData = Malloc(TEMP_ADC_DATA_LEN * sizeof(USHORT_T));
    memset(tuya_adc.priv.pData, 0, TEMP_ADC_DATA_LEN*sizeof(USHORT_T));
    tuya_adc.priv.data_buff_size = TEMP_ADC_DATA_LEN;

}

VOID plant_get_adc_sensor_data(VOID)
{
    tuya_hal_adc_init(&tuya_adc);

    tuya_hal_adc_value_get(TEMP_ADC_DATA_LEN, &soil_moisture_value);

    PR_NOTICE("water_tank_value = %d",soil_moisture_value);

    tuya_hal_adc_finalize(&tuya_adc);
}

STATIC VOID __set_pwm_duty(VOID)
{
    user_pwm_duty.duty_red = (USHORT_T)(((float)plant_ctrl_data.Red_value/255.0)*1000);
    PR_NOTICE("duty_red = %d",user_pwm_duty.duty_red);
    user_pwm_duty.duty_green = (USHORT_T)(((float)plant_ctrl_data.Green_value/255.0)*1000);
    user_pwm_duty.duty_blue = (USHORT_T)(((float)plant_ctrl_data.Blue_value/255.0)*1000);
    user_pwm_duty.duty_warm = (USHORT_T)(((float)plant_ctrl_data.Warm_value/255.0)*1000);
}

STATIC VOID __passive_ctrl_module_pump(VOID)
{   
    if(!ADD_WATER_READY) {
        tuya_gpio_write(WATER_PUMP_PORT, WATER_PUMP_LEVEL);
        ADD_WATER_COUNT++;
        if(ADD_WATER_COUNT >15) {
            ADD_WATER_READY = 1;
            ADD_WATER_COUNT = 0;
        }
    }

    switch (plant_ctrl_data.soil_moisture_level) {
    
    case manual_control: 
        ADD_WATER_COUNT == 0;
        ADD_WATER_READY == 1;
        break;
    case low:
        if(soil_moisture_value > soil_moisture_threshold.Low) {
            if(ADD_WATER_READY) {
                tuya_gpio_write(WATER_PUMP_PORT, !WATER_PUMP_LEVEL);
                ADD_WATER_COUNT++;
                if(ADD_WATER_COUNT > 2) {
                    ADD_WATER_READY = 0;
                }
            }
        }else {
            tuya_gpio_write(WATER_PUMP_PORT, WATER_PUMP_LEVEL);
        }
        break;
    case medium:
        if(soil_moisture_value > soil_moisture_threshold.Medium) {
            if(ADD_WATER_READY) {
                tuya_gpio_write(WATER_PUMP_PORT, !WATER_PUMP_LEVEL);
                ADD_WATER_COUNT++;
                if(ADD_WATER_COUNT > 2) {
                    ADD_WATER_READY = 0;
                }
            }
        }else {
            tuya_gpio_write(WATER_PUMP_PORT, WATER_PUMP_LEVEL);
        }
        break;
    case high:
        if(soil_moisture_value > soil_moisture_threshold.High) {
            if(ADD_WATER_READY) {
                tuya_gpio_write(WATER_PUMP_PORT, !WATER_PUMP_LEVEL);
                ADD_WATER_COUNT++;
                if(ADD_WATER_COUNT > 2) {
                    ADD_WATER_READY = 0;
                }
            }
        }else {
            tuya_gpio_write(WATER_PUMP_PORT, WATER_PUMP_LEVEL);
        }
        break;
    default:
        break;
    }
    
}

STATIC VOID __initiative_ctrl_module_pump(VOID)
{   
    if(plant_ctrl_data.soil_moisture_level != manual_control) {
        return;        
    }
    
    if(TRUE == plant_ctrl_data.Pump) {
        tuya_gpio_write(WATER_PUMP_PORT, !WATER_PUMP_LEVEL);
    }else {
        tuya_gpio_write(WATER_PUMP_PORT, WATER_PUMP_LEVEL);
    }
    
}

STATIC VOID __initiative_ctrl_module_light(VOID)
{   
    if (plant_ctrl_data.Countdown_set != cancel)
    {
        if(IsThisSysTimerRun(light_timer) == FALSE) {
            light_flag_min = (USHORT_T)plant_ctrl_data.Countdown_set * 60;
            plant_pwm_set(&user_pwm_duty);
            sys_start_timer(light_timer,1000*60,TIMER_CYCLE); 
        }else {
            plant_pwm_set(&user_pwm_duty);
        }
    }else {
        light_flag_min = 0;
        if(TRUE == plant_ctrl_data.Light_switch) {
            plant_pwm_set(&user_pwm_duty);
        }else {
            plant_pwm_off();
        }
    }
    plant_report_data.Countdown_left = light_flag_min;                                  
}

VOID plant_ctrl_handle(VOID)
{   
    PR_DEBUG("enter ctrl handle");
    
    __set_pwm_duty();

    //automatic water pump control
    __passive_ctrl_module_pump();

    //active water pump control
    __initiative_ctrl_module_pump();

    //active control of light board
    __initiative_ctrl_module_light();
}

VOID plant_ctrl_all_off(VOID)
{   
    tuya_gpio_write(WATER_PUMP_PORT, WATER_PUMP_LEVEL);

    plant_pwm_off();
}

 