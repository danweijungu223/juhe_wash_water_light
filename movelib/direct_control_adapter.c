#include "direct_control_adapter.h"

// PWM值范围限制
#ifndef PWM_CLAMP
#define PWM_CLAMP(value, min_val, max_val) \
    ((value) < (min_val) ? (min_val) : ((value) > (max_val) ? (max_val) : (value)))
#endif

#define LOG_TAG "direct_ctrl"
#define LOG_LVL LOG_LVL_INFO
#include <ulog.h>

// 直接控制状态管理
static struct {
    struct DirectPWMControlState state;
    uint32_t last_process_time;
    uint8_t initialized;
} g_direct_ctrl = {0};

// 初始化直接控制适配器
void DirectControlAdapterInit(void)
{
    if(g_direct_ctrl.initialized) {
        LOG_W("Direct Control Adapter already initialized");
        return;
    }

    // 初始化状态
    g_direct_ctrl.state.active = 0;
    g_direct_ctrl.state.last_recv_timestamp = 0;

    // 初始化PWM值为安全默认值
    for(int i = 0; i < 8; i++) {
        g_direct_ctrl.state.pwm_data.motor_pwm[i] = PWM_MID_VALUE;
    }
    g_direct_ctrl.state.pwm_data.light_pwm = PWM_DEFAULT_LIGHT_OFF;     // 灯光关闭
    g_direct_ctrl.state.pwm_data.grip_open_pwm = PWM_DEFAULT_GRIP_OPEN;  // 夹爪开合中位
    g_direct_ctrl.state.pwm_data.grip_grasp_pwm = PWM_DEFAULT_GRIP_GRASP; // 夹爪夹放中位
    g_direct_ctrl.state.pwm_data.cam_pitch_pwm = PWM_DEFAULT_CAM_PITCH;  // 摄像头俯仰水平
    g_direct_ctrl.state.pwm_data.cam_yaw_pwm = PWM_DEFAULT_CAM_YAW;      // 摄像头旋转中位
    g_direct_ctrl.state.pwm_data.reserved = 0;

    g_direct_ctrl.last_process_time = rt_tick_get();
    g_direct_ctrl.initialized = 1;
}

// 更新直接PWM控制数据
void DirectControlUpdatePWM(const struct DirectPWMControl* data)
{
    if(!g_direct_ctrl.initialized) {
        LOG_W("Direct Control Adapter not initialized");
        return;
    }

    if(data == NULL) {
        LOG_E("NULL data pointer");
        return;
    }

    // 更新数据
    g_direct_ctrl.state.pwm_data = *data;
    g_direct_ctrl.state.last_recv_timestamp = rt_tick_get();
    g_direct_ctrl.state.active = 1;

    // 验证PWM值范围
    for(int i = 0; i < 8; i++) {
        if(g_direct_ctrl.state.pwm_data.motor_pwm[i] < PWM_MIN_VALUE ||
           g_direct_ctrl.state.pwm_data.motor_pwm[i] > PWM_MAX_VALUE) {
            LOG_W("Motor %d PWM value out of range: %d", i,
                  g_direct_ctrl.state.pwm_data.motor_pwm[i]);
            g_direct_ctrl.state.pwm_data.motor_pwm[i] = PWM_CLAMP(
                g_direct_ctrl.state.pwm_data.motor_pwm[i],
                PWM_MIN_VALUE, PWM_MAX_VALUE);
        }
    }

    // 验证灯光PWM值
    if(g_direct_ctrl.state.pwm_data.light_pwm < PWM_RANGE_LIGHT_MIN ||
       g_direct_ctrl.state.pwm_data.light_pwm > PWM_RANGE_LIGHT_MAX) {
        LOG_W("Light PWM value out of range: %d",
              g_direct_ctrl.state.pwm_data.light_pwm);
        g_direct_ctrl.state.pwm_data.light_pwm = PWM_CLAMP(
            g_direct_ctrl.state.pwm_data.light_pwm,
            PWM_RANGE_LIGHT_MIN, PWM_RANGE_LIGHT_MAX);
    }

    // 验证夹爪开合PWM值
    if(g_direct_ctrl.state.pwm_data.grip_open_pwm < PWM_RANGE_GRIP_OPEN_MIN ||
       g_direct_ctrl.state.pwm_data.grip_open_pwm > PWM_RANGE_GRIP_OPEN_MAX) {
        LOG_W("Grip open PWM value out of range: %d",
              g_direct_ctrl.state.pwm_data.grip_open_pwm);
        g_direct_ctrl.state.pwm_data.grip_open_pwm = PWM_CLAMP(
            g_direct_ctrl.state.pwm_data.grip_open_pwm,
            PWM_RANGE_GRIP_OPEN_MIN, PWM_RANGE_GRIP_OPEN_MAX);
    }

    // 验证夹爪夹放PWM值
    if(g_direct_ctrl.state.pwm_data.grip_grasp_pwm < PWM_MIN_VALUE ||
       g_direct_ctrl.state.pwm_data.grip_grasp_pwm > PWM_MAX_VALUE) {
        LOG_W("Grip grasp PWM value out of range: %d",
              g_direct_ctrl.state.pwm_data.grip_grasp_pwm);
        g_direct_ctrl.state.pwm_data.grip_grasp_pwm = PWM_CLAMP(
            g_direct_ctrl.state.pwm_data.grip_grasp_pwm,
            PWM_MIN_VALUE, PWM_MAX_VALUE);
    }

    // 验证摄像头俯仰PWM值
    if(g_direct_ctrl.state.pwm_data.cam_pitch_pwm < PWM_MIN_VALUE ||
       g_direct_ctrl.state.pwm_data.cam_pitch_pwm > PWM_MAX_VALUE) {
        LOG_W("Camera pitch PWM value out of range: %d",
              g_direct_ctrl.state.pwm_data.cam_pitch_pwm);
        g_direct_ctrl.state.pwm_data.cam_pitch_pwm = PWM_CLAMP(
            g_direct_ctrl.state.pwm_data.cam_pitch_pwm,
            PWM_MIN_VALUE, PWM_MAX_VALUE);
    }

    // 验证摄像头旋转PWM值
    if(g_direct_ctrl.state.pwm_data.cam_yaw_pwm < PWM_RANGE_CAM_YAW_MIN ||
       g_direct_ctrl.state.pwm_data.cam_yaw_pwm > PWM_RANGE_CAM_YAW_MAX) {
        LOG_W("Camera yaw PWM value out of range: %d",
              g_direct_ctrl.state.pwm_data.cam_yaw_pwm);
        g_direct_ctrl.state.pwm_data.cam_yaw_pwm = PWM_CLAMP(
            g_direct_ctrl.state.pwm_data.cam_yaw_pwm,
            PWM_RANGE_CAM_YAW_MIN, PWM_RANGE_CAM_YAW_MAX);
    }

    LOG_D("Direct PWM data updated");
}

// 获取直接控制状态
direct_ctrl_status_t DirectControlGetStatus(void)
{
    if(!g_direct_ctrl.initialized) {
        return DIRECT_CTRL_INACTIVE;
    }

    if(!g_direct_ctrl.state.active) {
        return DIRECT_CTRL_INACTIVE;
    }

    // 检查超时
    if((rt_tick_get() - g_direct_ctrl.state.last_recv_timestamp) > DIRECT_CTRL_TIMEOUT_MS) {
        return DIRECT_CTRL_TIMEOUT;
    }

    return DIRECT_CTRL_ACTIVE;
}

// 检查是否激活
uint8_t DirectControlIsActive(void)
{
    direct_ctrl_status_t status = DirectControlGetStatus();
    return (status == DIRECT_CTRL_ACTIVE);
}

// 获取推进器PWM值
uint16_t DirectControlGetMotorPWM(uint8_t motor_index)
{
    if(!g_direct_ctrl.initialized || motor_index >= 8) {
        return PWM_MID_VALUE;
    }

    return g_direct_ctrl.state.pwm_data.motor_pwm[motor_index];
}

// 获取灯光PWM值
uint16_t DirectControlGetLightPWM(void)
{
    if(!g_direct_ctrl.initialized) {
        return PWM_DEFAULT_LIGHT_OFF;
    }

    return g_direct_ctrl.state.pwm_data.light_pwm;
}

// 获取机械臂夹爪开合PWM值
uint16_t DirectControlGetGripOpenPWM(void)
{
    if(!g_direct_ctrl.initialized) {
        return PWM_DEFAULT_GRIP_OPEN;
    }

    return g_direct_ctrl.state.pwm_data.grip_open_pwm;
}

// 获取机械臂夹爪夹放PWM值
uint16_t DirectControlGetGripGraspPWM(void)
{
    if(!g_direct_ctrl.initialized) {
        return PWM_DEFAULT_GRIP_GRASP;
    }

    return g_direct_ctrl.state.pwm_data.grip_grasp_pwm;
}

// 获取摄像头俯仰PWM值
uint16_t DirectControlGetCamPitchPWM(void)
{
    if(!g_direct_ctrl.initialized) {
        return PWM_DEFAULT_CAM_PITCH;
    }

    return g_direct_ctrl.state.pwm_data.cam_pitch_pwm;
}

// 获取摄像头旋转PWM值
uint16_t DirectControlGetCamYawPWM(void)
{
    if(!g_direct_ctrl.initialized) {
        return PWM_DEFAULT_CAM_YAW;
    }

    return g_direct_ctrl.state.pwm_data.cam_yaw_pwm;
}

// 设置直接控制激活状态
void DirectControlSetActive(uint8_t active)
{
    if(!g_direct_ctrl.initialized) {
        LOG_W("Direct Control Adapter not initialized");
        return;
    }

    if(g_direct_ctrl.state.active != active) {
        g_direct_ctrl.state.active = active;
        if(active) {
            g_direct_ctrl.state.last_recv_timestamp = rt_tick_get();
            LOG_I("Direct control activated");
        } else {
            LOG_I("Direct control deactivated");
        }
    }
}

void DirectControlProcess(void)
{
    if(!g_direct_ctrl.initialized) {
        return;
    }

    direct_ctrl_status_t status = DirectControlGetStatus();

    if(status == DIRECT_CTRL_TIMEOUT) {
        LOG_W("Direct control timeout, deactivating");
        DirectControlSetActive(0);

        // 重置PWM值为安全值
        for(int i = 0; i < 8; i++) {
            g_direct_ctrl.state.pwm_data.motor_pwm[i] = PWM_MID_VALUE;
        }
        g_direct_ctrl.state.pwm_data.light_pwm = PWM_DEFAULT_LIGHT_OFF;     // 灯光关闭
        g_direct_ctrl.state.pwm_data.grip_open_pwm = PWM_DEFAULT_GRIP_OPEN;  // 夹爪开合中位
        g_direct_ctrl.state.pwm_data.grip_grasp_pwm = PWM_DEFAULT_GRIP_GRASP; // 夹爪夹放中位
        g_direct_ctrl.state.pwm_data.cam_pitch_pwm = PWM_DEFAULT_CAM_PITCH;  // 摄像头俯仰水平
        g_direct_ctrl.state.pwm_data.cam_yaw_pwm = PWM_DEFAULT_CAM_YAW;      // 摄像头旋转中位
    }

    g_direct_ctrl.last_process_time = rt_tick_get();
}

// 获取PWM数据结构体
const struct DirectPWMControl* DirectControlGetPWMData(void)
{
    if(!g_direct_ctrl.initialized) {
        return NULL;
    }

    return &g_direct_ctrl.state.pwm_data;
}

// 兼容性函数 - 与原有接口保持一致
const struct DirectPWMControlState* get_direct_pwm_state(void)
{
    if(!g_direct_ctrl.initialized) {
        return NULL;
    }

    return &g_direct_ctrl.state;
}

void set_direct_pwm_active(uint8_t active)
{
    DirectControlSetActive(active);
}

void update_direct_pwm_data(const struct DirectPWMControl* data)
{
    DirectControlUpdatePWM(data);
}