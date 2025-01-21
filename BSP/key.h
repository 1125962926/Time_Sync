/*
 * @Author: Li RF
 * @Date: 2024-02-27 16:50:48
 * @LastEditTime: 2024-03-11 14:53:36
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#ifndef __KEY_H__
#define __KEY_H__



#include "main.h"


#define S1_KEY_Pin            	GPIO_PIN_0
#define S1_KEY_GPIO_Port      	GPIOA
#define S1_KEY_GPIO_CLK_ENABLE() \
    do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while(0)


#define S1_KEY                  HAL_GPIO_ReadPin(S1_KEY_GPIO_Port, S1_KEY_Pin)


void KEY_Init(void);
uint8_t KEY_Scan(void);

#endif

