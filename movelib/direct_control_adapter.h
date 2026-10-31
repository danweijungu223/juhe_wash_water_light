#ifndef __DIRECT_CONTROL_ADAPTER_H__
#define __DIRECT_CONTROL_ADAPTER_H__

#include <rtthread.h>
#include "userlib/robolink_def.h"

// 直接控制适配器功能：
// 1. 管理直接PWM控制状态
// 2. 提供状态查询和更新接口
// 3. 实现超时保护机制
// 4. 输出PWM数据给执行器

// 直接控制模式状态
typedef enum {
    DIRECT_CTRL_INACTIVE = 0,  // 未激活
    DIRECT_CTRL_ACTIVE = 1,    // 已激活
    DIRECT_CTRL_TIMEOUT = 2    // 超时
} direct_ctrl_status_t;

// 初始化直接控制适配器
void DirectControlAdapterInit(void);

// 更新直接PWM控制数据
void DirectControlUpdatePWM(const struct DirectPWMControl* data);

// 获取直接控制状态
direct_ctrl_status_t DirectControlGetStatus(void);

// 检查是否激活
uint8_t DirectControlIsActive(void);

// 获取推进器PWM值
uint16_t DirectControlGetMotorPWM(uint8_t motor_index);

// 获取灯光PWM值
uint16_t DirectControlGetLightPWM(void);

// 获取机械臂夹爪开合PWM值
uint16_t DirectControlGetGripOpenPWM(void);

// 获取机械臂夹爪夹放PWM值
uint16_t DirectControlGetGripGraspPWM(void);

// 获取摄像头俯仰PWM值
uint16_t DirectControlGetCamPitchPWM(void);

// 获取摄像头旋转PWM值
uint16_t DirectControlGetCamYawPWM(void);

// 设置直接控制激活状态
void DirectControlSetActive(uint8_t active);

// 主循环调用 - 检查超时
void DirectControlProcess(void);

// 获取PWM数据结构体
const struct DirectPWMControl* DirectControlGetPWMData(void);

// 内联函数 - 快速检查是否激活
static inline uint8_t IsDirectControlActiveInline(void)
{
    return DirectControlIsActive();
}

// 内联函数 - 获取推进器PWM（带默认值）
static inline uint16_t GetMotorPWMWithDefault(uint8_t index, uint16_t default_pwm)
{
    uint16_t pwm = DirectControlGetMotorPWM(index);
    return pwm ? pwm : default_pwm;
}

// 宏定义
#define DIRECT_CTRL_TIMEOUT_MS 1000
#define PWM_MIN_VALUE 1000
#define PWM_MAX_VALUE 2000
#define PWM_MID_VALUE 1500

// 设备PWM默认值宏定义
#define PWM_DEFAULT_LIGHT_OFF        1100  // 灯光关闭
#define PWM_DEFAULT_GRIP_OPEN        1500  // 夹爪开合中位
#define PWM_DEFAULT_GRIP_GRASP       1500  // 夹爪夹放中位
#define PWM_DEFAULT_CAM_PITCH        1500  // 摄像头俯仰水平
#define PWM_DEFAULT_CAM_YAW          1500  // 摄像头旋转中位

// 设备PWM范围宏定义
#define PWM_RANGE_LIGHT_MIN          1100  // 灯光最小值
#define PWM_RANGE_LIGHT_MAX          1900  // 灯光最大值
#define PWM_RANGE_GRIP_OPEN_MIN      1150  // 夹爪开合最小值（完全闭合）
#define PWM_RANGE_GRIP_OPEN_MAX      1850  // 夹爪开合最大值（完全张开）
#define PWM_RANGE_CAM_YAW_MIN        1350  // 摄像头旋转最小值（左转最大）
#define PWM_RANGE_CAM_YAW_MAX        1650  // 摄像头旋转最大值（右转最大）

#define IS_DIRECT_CTRL_TIMEOUT() (DirectControlGetStatus() == DIRECT_CTRL_TIMEOUT)
#define SAFELY_GET_MOTOR_PWM(idx) DirectControlGetMotorPWM(idx)
#define SAFELY_GET_LIGHT_PWM() DirectControlGetLightPWM()

#endif // __DIRECT_CONTROL_ADAPTER_H__