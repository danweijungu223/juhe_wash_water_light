#ifndef __SHIP_MOVE_THREAD_H_
#define __SHIP_MOVE_THREAD_H_

#include "stm32f4xx.h"
#include "rthw.h"
#include "rtthread.h"

rt_thread_t* get_ship_move_tid();

void ShipMoveThreadEntry(void* parameter);

#endif
