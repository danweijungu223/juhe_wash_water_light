#include "ship_move_thread.h"

#define LOG_TAG     "s_m_t"
#define LOG_LVL     LOG_LVL_INFO
#include <ulog.h>

rt_thread_t ship_move_tid = RT_NULL; //水面无人艇线程控制块

inline rt_thread_t* get_ship_move_tid() {
    return &ship_move_tid;
}

void ShipMoveThreadEntry(void* parameter) {
    rt_thread_mdelay(1000);
    for (;;) {
        //TODO加入SHIP的电机速度计算
        LOG_I("Ship");
        rt_thread_mdelay(500);
    }
    return;
}
