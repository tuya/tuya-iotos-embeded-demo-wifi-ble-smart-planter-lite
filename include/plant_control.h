#ifndef __PLANT_CONTROL_H__
#define __PLANT_CONTROL_H__

#include "uni_log.h"
#include "tuya_cloud_error_code.h"

typedef enum {
    cancel = 0,
    set_1h,
    set_2h,
    set_3h,
    set_4h,
    set_5h,
    set_6h,
    set_7h,
    set_8h,
    set_9h,
    set_10h,
    set_11h,
    set_12h
}COUNTDOWN_SET;

typedef enum {
    manual_control,
    low,
    medium,
    high
}MOISTURE_LV;

typedef struct {
    BOOL_T Switch;
    BOOL_T Pump;
    BOOL_T Light_switch;
    COUNTDOWN_SET Countdown_set;
    USHORT_T Red_value;
    USHORT_T Green_value;
    USHORT_T Blue_value;
    USHORT_T Warm_value;
    MOISTURE_LV soil_moisture_level;
}PLANT_CTRL_DATA_T;

typedef struct {
    USHORT_T Countdown_left;
    UCHAR_T fault;
}PLANT_REPORT_DATA_T;

typedef struct {
    USHORT_T Low;
    USHORT_T Medium;
    USHORT_T High;
}SOIL_MOISTURE_THRESHOLD_T;
/********************************************************************************
 * FUNCTION:       plant_device_init
 * DESCRIPTION:    device initialization
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         none
 * HISTORY:        2021-01-12
 *******************************************************************************/
VOID plant_device_init(VOID);

/********************************************************************************
 * FUNCTION:       plant_get_adc_sensor_data
 * DESCRIPTION:    Get ADC sensor data
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         none
 * HISTORY:        2021-01-12
 *******************************************************************************/
VOID plant_get_adc_sensor_data(VOID);

/********************************************************************************
 * FUNCTION:       plant_ctrl_handle
 * DESCRIPTION:    sensor data deal handle
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         none
 * HISTORY:        2021-01-12
 *******************************************************************************/
VOID plant_ctrl_handle(VOID);

/********************************************************************************
 * FUNCTION:       plant_ctrl_all_off
 * DESCRIPTION:    Close all control components
 * INPUT:          none
 * OUTPUT:         none
 * RETURN:         none
 * OTHERS:         none
 * HISTORY:        2021-01-12
 *******************************************************************************/
VOID plant_ctrl_all_off(VOID);

#endif