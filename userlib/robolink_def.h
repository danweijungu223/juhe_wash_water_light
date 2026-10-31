#ifndef __ROBOLINK_DEF_H_
#define __ROBOLINK_DEF_H_

#include "bytetransformers.h"
#include <string.h>

#define XBOX_CTRL_A  (1u)
#define XBOX_CTRL_B  (2u)
#define XBOX_CTRL_X  (4u)
#define XBOX_CTRL_Y  (8u)
#define XBOX_CTRL_LB (16u)
#define XBOX_CTRL_RB (32u)
#define XBOX_VIEW    (64u)
#define XBOX_MENU    (128u)
#define XBOX_LS_CLICK (256u)
#define XBOX_RS_CLICK (512u)
#define XBOX_CTRL_UP     (0u)
#define XBOX_CTRL_DOWN   (18000u)
#define XBOX_CTRL_LEFT   (27000u)
#define XBOX_CTRL_RIGHT  (9000u)
#define XBOX_IS_PRESSED_BTN(last, now, btn) ((!(((last)->dwButtons & btn) == btn)) && (((now)->dwButtons & btn) == btn))
#define XBOX_IS_PRESS_POV(now, direction)   ((now)->dwPOV == direction)

struct ControllerGamepad
{
    uint32_t dwXpos;// 左右平移
    uint32_t dwYpos;// 前后平移
    uint32_t dwZpos;
    uint32_t dwRpos;
    uint32_t dwUpos;
    uint32_t dwVpos;
    uint32_t dwButtons;
    uint32_t dwButtonNumber;
    uint32_t dwPOV;
};

struct ControllerBox2023 {
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t z_pos;
    uint16_t r_pos;
    uint16_t u_pos;
    uint16_t v_pos;
    uint16_t knob1[5];
    uint16_t knob2[3];
    uint16_t on_off[3];
};

struct ControllerGamepadWeb {
    uint16_t axes[4];
    uint16_t buttons;
    uint16_t button6;
    uint16_t button7;
};

static inline void robolink_process_raw_controller_gamepad(uint8_t* raw_data, struct ControllerGamepad* data)
{
    if(raw_data == NULL || data == NULL) return;
    data->dwXpos = get_u32(raw_data + 0, LSB);
    data->dwYpos = get_u32(raw_data + 4, LSB);
    data->dwZpos = get_u32(raw_data + 8, LSB);
    data->dwRpos = get_u32(raw_data + 12, LSB);
    data->dwUpos = get_u32(raw_data + 16, LSB);
    data->dwVpos = get_u32(raw_data + 20, LSB);
    data->dwButtons = get_u32(raw_data + 24, LSB);
    data->dwButtonNumber = get_u32(raw_data + 28, LSB);
    data->dwPOV = get_u32(raw_data + 32, LSB);
}

static inline void robolink_process_raw_controller_box_2023(uint8_t* raw_data, struct ControllerBox2023* data)
{
    if(raw_data == NULL || data == NULL) return;
    data->x_pos = get_u16(raw_data + 0, LSB);
    data->y_pos = get_u16(raw_data + 2, LSB);
    data->z_pos = get_u16(raw_data + 8, LSB);
    data->r_pos = get_u16(raw_data + 6, LSB);
    data->u_pos = get_u16(raw_data + 4, LSB);
    data->v_pos = get_u16(raw_data + 10, LSB);
    data->knob1[0] = get_u16(raw_data + 12, LSB);
    data->knob1[1] = get_u16(raw_data + 14, LSB);
    data->knob1[2] = get_u16(raw_data + 16, LSB);
    data->knob1[3] = get_u16(raw_data + 18, LSB);
    data->knob1[4] = get_u16(raw_data + 20, LSB);
    data->knob2[0] = get_u16(raw_data + 22, LSB);
    data->knob2[1] = get_u16(raw_data + 24, LSB);
    data->knob2[2] = get_u16(raw_data + 26, LSB);
    data->on_off[0] = get_u16(raw_data + 28, LSB);
    data->on_off[1] = get_u16(raw_data + 30, LSB);
    data->on_off[2] = get_u16(raw_data + 32, LSB);
}

static inline void robolink_process_raw_controller_gamepad_web(uint8_t* raw_data, struct ControllerGamepadWeb* data)
{
    if(raw_data == NULL || data == NULL) return;
    data->axes[0] = get_u16(raw_data + 0, LSB);
    data->axes[1] = get_u16(raw_data + 2, LSB);
    data->axes[2] = get_u16(raw_data + 4, LSB);
    data->axes[3] = get_u16(raw_data + 6, LSB);
    data->buttons = get_u16(raw_data + 8, LSB);
    data->button6 = get_u16(raw_data + 10, LSB);
    data->button7 = get_u16(raw_data + 12, LSB);
}

static inline void reset_controller_gamepad(struct ControllerGamepad* data)
{
    data->dwXpos = 32768;
    data->dwYpos = 32768;
    data->dwZpos = 32768;
    data->dwRpos = 32768;
    data->dwUpos = 0;
    data->dwVpos = 0;
    data->dwButtons = 0;
    data->dwButtonNumber = 0;
    data->dwPOV = 65535;
}

static inline void reset_controller_box_2023(struct ControllerBox2023* data)
{
    // TODO: 完善控制器数重置函数
}

static inline void reset_controller_gamepad_web(struct ControllerGamepadWeb* data)
{
    data->axes[0] = 32768;
    data->axes[1] = 32768;
    data->axes[2] = 32768;
    data->axes[3] = 32768;
    data->buttons = 0;
    data->button6 = 0;
    data->button7 = 0;
}



/**
 * @brief 直接PWM控制数据结构（36字节载荷）
 * @note  用于直接控制推进器、灯光、机械臂和摄像头，绕过运动学解算
 *        所有多字节数据均为小端序（低位在前）
 */
#pragma pack(1)
struct DirectPWMControl {
    uint16_t motor_pwm[8];     /**< 推进器PWM值，范围1000~2000 (8*2=16字节) */
    uint16_t light_pwm;        /**< 灯光PWM值，范围1100~1900 (1*2=2字节) */
    uint16_t grip_open_pwm;    /**< 机械臂夹爪开合PWM值，范围1150~1850 (1*2=2字节) */
    uint16_t grip_grasp_pwm;   /**< 机械臂夹爪夹放PWM值，范围1000~2000 (1*2=2字节) */
    uint16_t cam_pitch_pwm;    /**< 摄像头俯仰PWM值，范围1000~2000 (1*2=2字节) */
    uint16_t cam_yaw_pwm;      /**< 摄像头旋转PWM值，范围1350~1650 (1*2=2字节) */
    uint16_t reserved;         /**< 保留字段，必须为0 (1*2=2字节) */
}; // 总计36字节
#pragma pack()

/**
 * @brief 直接PWM控制完整数据结构
 */
struct DirectPWMControlMsg {
    struct DirectPWMControl data;  /**< PWM控制数据 */
    uint8_t packet_seq;           /**< 包序号 */
    uint8_t system_id;            /**< 系统ID */
    uint8_t valid;                /**< 数据有效标志 */
};

/**
 * @brief 直接PWM控制状态
 */
struct DirectPWMControlState {
    uint8_t active;                       /**< 激活标志 */
    uint32_t last_recv_timestamp;         /**< 最后接收时间戳 */
    struct DirectPWMControl pwm_data;     /**< PWM数据缓存 */
};

/**
 * @brief 直接PWM控制解析函数
 */
static inline void robolink_process_raw_direct_pwm_control(uint8_t* raw_data, struct DirectPWMControlMsg* data)
{
    if(raw_data == NULL || data == NULL) return;

    // 包序号和系统ID
    data->packet_seq = raw_data[1];
    data->system_id = raw_data[3];

    // 解析36字节的载荷数据
    struct DirectPWMControl* payload = (struct DirectPWMControl*)(raw_data + 6);

    // 推进器PWM数据 - 16字节
    for(int i = 0; i < 8; i++) {
        data->data.motor_pwm[i] = get_u16((uint8_t*)&payload->motor_pwm[i], LSB);
        // PWM值范围限制
        if(data->data.motor_pwm[i] < 1000) data->data.motor_pwm[i] = 1000;
        if(data->data.motor_pwm[i] > 2000) data->data.motor_pwm[i] = 2000;
    }

    // 灯光PWM数据 - 2字节
    data->data.light_pwm = get_u16((uint8_t*)&payload->light_pwm, LSB);
    if(data->data.light_pwm < 1100) data->data.light_pwm = 1100;
    if(data->data.light_pwm > 1900) data->data.light_pwm = 1900;

    // 机械臂夹爪开合PWM数据 - 2字节
    data->data.grip_open_pwm = get_u16((uint8_t*)&payload->grip_open_pwm, LSB);
    if(data->data.grip_open_pwm < 1150) data->data.grip_open_pwm = 1150;
    if(data->data.grip_open_pwm > 1850) data->data.grip_open_pwm = 1850;

    // 机械臂夹爪夹放PWM数据 - 2字节
    data->data.grip_grasp_pwm = get_u16((uint8_t*)&payload->grip_grasp_pwm, LSB);
    if(data->data.grip_grasp_pwm < 1000) data->data.grip_grasp_pwm = 1000;
    if(data->data.grip_grasp_pwm > 2000) data->data.grip_grasp_pwm = 2000;

    // 摄像头俯仰PWM数据 - 2字节
    data->data.cam_pitch_pwm = get_u16((uint8_t*)&payload->cam_pitch_pwm, LSB);
    if(data->data.cam_pitch_pwm < 1000) data->data.cam_pitch_pwm = 1000;
    if(data->data.cam_pitch_pwm > 2000) data->data.cam_pitch_pwm = 2000;

    // 摄像头旋转PWM数据 - 2字节
    data->data.cam_yaw_pwm = get_u16((uint8_t*)&payload->cam_yaw_pwm, LSB);
    if(data->data.cam_yaw_pwm < 1350) data->data.cam_yaw_pwm = 1350;
    if(data->data.cam_yaw_pwm > 1650) data->data.cam_yaw_pwm = 1650;

    data->data.reserved = get_u16((uint8_t*)&payload->reserved, LSB);
    data->valid = 1;
}

/**
 * @brief 重置直接PWM控制数据
 */
static inline void reset_direct_pwm_control(struct DirectPWMControlMsg* data)
{
    if(data == NULL) return;
    memset(data, 0, sizeof(struct DirectPWMControlMsg));
    for(int i = 0; i < 8; i++) {
        data->data.motor_pwm[i] = 1500;    // 推进器停止
    }
    data->data.light_pwm = 1100;           // 灯光关闭
    data->data.grip_open_pwm = 1500;       // 夹爪开合中位
    data->data.grip_grasp_pwm = 1500;      // 夹爪夹放中位
    data->data.cam_pitch_pwm = 1500;       // 摄像头俯仰水平
    data->data.cam_yaw_pwm = 1500;         // 摄像头旋转中位
    data->data.reserved = 0;               // 保留字段清零
}

struct RobolinkGetMsg
{
    struct ControllerGamepad controller_gamepad;
    struct ControllerBox2023 controller_box_2023;
    struct ControllerGamepadWeb controller_gamepad_web;
    struct DirectPWMControlMsg direct_pwm_control;         // 直接PWM控制
};

#endif  // __ROBOLINK_DEF_H_
