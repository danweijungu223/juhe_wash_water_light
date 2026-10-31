#ifndef __MECANUM_MOVE_THREAD_H_
#define __MECANUM_MOVE_THREAD_H_

#include "stm32f4xx.h"
#include "rthw.h"
#include "rtthread.h"

rt_thread_t* get_mecanum_move_tid();

void MecanumMoveThreadEntry(void* parameter);

#endif
