/*
 * @Author: Li RF
 * @Date: 2024-02-27 14:28:06
 * @LastEditTime: 2024-03-11 17:09:14
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#ifndef __TIM_H__
#define __TIM_H__



#include "main.h"



#if MASTER
//广播就绪标志，将广播放到主函数执行，防止降低定时器精度
extern uint8_t Master_broad_STA;

//处理从机就绪标志
extern uint8_t handle_OK_STA;


#endif



extern TIM_HandleTypeDef htim1;
extern uint32_t globel_time;

void TIM1_Init(uint16_t period, uint8_t prescaler);



uint16_t GetCount(void);
void SetCount(uint16_t timer1, uint32_t timer2);

void System_TIME_to_oled(void);




#endif

