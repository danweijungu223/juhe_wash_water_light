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

static int LeakageDetection();

rt_thread_t equipment_tid = RT_NULL;  // 机器人设备控制模块 摄像头 机械手 水下灯
rt_thread_t* get_equipment_tid(){
    return &equipment_tid;
}

void EquipmentThreadEntry(void* parameter){
    DirectControlAdapterInit();

    rt_thread_delay(1000);
    for( ; ; ){
        int16_t equipment_output[6] = {1500, 1620, 1500, 1100, 1500, 1500};

        DirectControlProcess();

        // 直接控制模式
        if(DirectControlIsActive()) {
            #if defined(EQUIPMENT_CLOUD_PLATFORM) && EQUIPMENT_CLOUD_PLATFORM == 1
                equipment_output[0] = 1500;
                equipment_output[1] = 1620;
            #endif

            #if defined(EQUIPMENT_CAMERA) && EQUIPMENT_CAMERA == 1
                equipment_output[2] = DirectControlGetCamPitchPWM();
                LOG_D("[Equipment] Direct cam_pitch: %d", equipment_output[2]);
            #endif

            #if defined(EQUIPMENT_LED) && EQUIPMENT_LED == 1
                equipment_output[3] = DirectControlGetLightPWM();
                LOG_D("[Equipment] Direct light: %d", equipment_output[3]);
            #endif

            #if defined(EQUIPMENT_IRON_HAND) && EQUIPMENT_IRON_HAND == 1
                equipment_output[4] = DirectControlGetGripOpenPWM();    // 开合
                equipment_output[5] = DirectControlGetGripGraspPWM();   // 夹放
                LOG_D("[Equipment] Direct grip_open: %d, grip_grasp: %d",
                      equipment_output[4], equipment_output[5]);
            #endif
        } else {
            // 自动控制模式
            #if defined(EQUIPMENT_CLOUD_PLATFORM) && EQUIPMENT_CLOUD_PLATFORM == 1
                uint16_t cloud_platform_pitch = 1500 - get_control_msg()->rov_control.cloud_platform_pitch * 10.0f;    //云台俯仰角度
                uint16_t cloud_platform_revolve = 1620 + get_control_msg()->rov_control.cloud_platform_revolve * 3.2f;    //云台旋转角度
                equipment_output[0] = cloud_platform_pitch;
                equipment_output[1] = cloud_platform_revolve;
                LOG_D("cloud_platform_pitch: %d, cloud_platform_revolve: %d\r\n",cloud_platform_pitch, cloud_platform_revolve);
            #endif

            #if defined(EQUIPMENT_CAMERA) && EQUIPMENT_CAMERA == 1
                uint16_t camera_angle = 1500 + get_control_msg()->rov_control.camera_pitch * 6.0f;              //摄像头角度
                equipment_output[2] = camera_angle;
                LOG_D("camera_angle: %d\r\n",camera_angle);
            #endif

            #if defined(EQUIPMENT_LED) && EQUIPMENT_LED == 1
                uint16_t led_brightness = 1100 + get_control_msg()->rov_control.led_brightness * 6.0f;         //LED亮度控制
                equipment_output[3] = led_brightness;
                LOG_D("led_brightness: %d\r\n",led_brightness);
            #endif

            #if defined(EQUIPMENT_IRON_HAND) && EQUIPMENT_IRON_HAND == 1
                uint16_t iron_hand_angle = 1500 + get_control_msg()->rov_control.gripper_angle * 10.0f;                      //机械手开合
                equipment_output[4] = iron_hand_angle;
                LOG_D("iron_hand_angle: %d\r\n",iron_hand_angle);
                uint16_t iron_hand_speed = 1500 + get_control_msg()->rov_control.gripper_speed * 10.0f;                      //机械手夹放
                equipment_output[5] = iron_hand_speed;
                LOG_D("iron_hand_speed: %d\r\n",iron_hand_speed);
            #endif
        }


        // 声纳控制（独立于直接控制模式）
//        if(get_control_msg()->equip_switch.sonar) {
//            PD15_PWM_OUT_US(20001);
//        } else {
//            PD15_PWM_OUT_US(0);
//        }
        LOG_D("sonar : %d\r\n",get_control_msg()->equip_switch.sonar);

        // 输出到硬件
        #if defined(PUSHES_OUT_SWITCH) && PUSHES_OUT_SWITCH == 0
            // PWM输出模式
            PE5_PWM_OUT_US(equipment_output[0]);  // 云台俯仰
            PE6_PWM_OUT_US(equipment_output[1]);  // 云台旋转
            PB1_PWM_OUT_US(equipment_output[2]);  // 摄像头俯仰
            PB0_PWM_OUT_US(equipment_output[3]);  // LED亮度
            // equipment_output[4] 和 equipment_output[5] 需要根据实际硬件连接配置
        #elif defined(PUSHES_OUT_SWITCH) && PUSHES_OUT_SWITCH == 2
            // CAN总线输出模式
//            EquipmentTxDataConvertAndSend(0, equipment_output[0], equipment_output[1], equipment_output[3], 3);
            // 其他设备可能需要额外的CAN消息
        #endif

        PE9_PWM_OUT_US(20000);
        PE11_PWM_OUT_US(20000);
        PE13_PWM_OUT_US(20000);
        PE14_PWM_OUT_US(20000);

        PD12_PWM_OUT_US(20000);
        PD13_PWM_OUT_US(20000);
        PD14_PWM_OUT_US(20000);
        PD15_PWM_OUT_US(20000);

        rt_thread_delay(20);  // 与rov_move_thread保持相同的周期
    }
}

inline static int LeakageDetection() {
    static int cnt = 0;
    uint16_t adc = get_adc();
    if(adc < 4000) {
        if((++cnt) > 40) {
            cnt = 0;
            return 1;
        }
    } else {
        if(cnt > 0) cnt--;
    }
    return 0;
}

