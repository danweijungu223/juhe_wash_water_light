#include "rov_move_thread.h"
#include "controller_switch_thread.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "hardware/motor_control_center.h"
#include "movelib/rov_move.h"
#include "movelib/direct_control_adapter.h"

#define LOG_TAG     "e_t"
#define LOG_LVL     LOG_LVL_INFO
#include <ulog.h>

rt_thread_t equipment_tid = RT_NULL;  // 机器人设备控制模块 摄像头 机械手 水下灯
rt_thread_t* get_equipment_tid(){
    return &equipment_tid;
}

void EquipmentThreadEntry(void* parameter){
    DirectControlAdapterInit();

    rt_thread_delay(1000);
    for( ; ; ){
            uint16_t water_light = get_water_light();
            uint16_t led_brightness = 1100 + (water_light * 300);

            PE9_PWM_OUT_US(led_brightness);
            PE11_PWM_OUT_US(led_brightness);
            PE13_PWM_OUT_US(led_brightness);
            PE14_PWM_OUT_US(led_brightness);

            PD12_PWM_OUT_US(led_brightness);
            PD13_PWM_OUT_US(led_brightness);
            PD14_PWM_OUT_US(led_brightness);
            PD15_PWM_OUT_US(led_brightness);

        rt_thread_delay(20);  // 与rov_move_thread保持相同的周期
    }
}


