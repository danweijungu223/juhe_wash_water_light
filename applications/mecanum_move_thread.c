#include "mecanum_move_thread.h"
#include <movelib/mecanum_move.h>
#include "hardware/motor_control_center.h"

#define LOG_TAG     "m_m_t"
#define LOG_LVL     LOG_LVL_INFO
#include <ulog.h>

rt_thread_t mecanum_move_tid = RT_NULL; //麦克纳姆车辆线程控制块

inline rt_thread_t* get_mecanum_move_tid() {
    return &mecanum_move_tid;
}

void MecanumMoveThreadEntry(void* parameter) {
    rt_thread_mdelay(1000);
    LOG_I("Mecanum");
    for (;;) {
        rt_thread_mdelay(20);
	    struct MotionSpeed motion_speed = { 0.0, 0.0, 0.0, 0.0 };
		float motor_speed[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		int motor_speed_for_int[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//		int16_t motor_speed_pwm_output[6] = { 0, 0, 0, 0 };

		MoveAutoAndManualForMecanum(&motion_speed, get_control_msg(), 20.0f/1000);

		// x y z ... -> 电机速度
		CalcMecanumWheelWheelsSpeed(motor_speed, motion_speed.x, motion_speed.y, motion_speed.yaw_z, 1.0);

		ScaleDownDataF(motor_speed+0, 1000, 4);
		WheelsSpeedInvert(motor_speed);

		#if WHEELS_OUT_SWITCH == 1
			for(int i=0; i<4; i++) {
				motor_speed[i] *= (5000 / 1000.0);
//				motor_speed[i] *= (10 / 1000.0);
				motor_speed_for_int[i] = (int)motor_speed[i];
			}
			WheelTxDataConvertAndSend(motor_speed_for_int);
//			LOG_D("%d %d %d %d ", motor_speed_for_int[0], motor_speed_for_int[1], motor_speed_for_int[2], motor_speed_for_int[3]);
		#elif WHEELS_OUT_SWITCH == 2
			for(int i=0; i<4; i++) {
			    motor_speed[i] *= (100000 / 1000.0);
			}
//			MotorsSpeedConvertToTargetArray(motor_speed_pwm_output, motor_speed, 4 + PUSHES_SWITCH * 2, 0, -100000, 100000);
			MotorVescDutyCanControlSendEightMotorsF(motor_speed);
		#else
			WheelsSpeedConvertToTargetArray(motor_speed_pwm_output, motor_speed, 4, 1500, 1000, 2000);
			WheelrPwmOut_4wheels(motor_speed_pwm_output);
		#endif
    }
    return;
}
