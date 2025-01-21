/*
 * @Author: Li RF
 * @Date: 2024-02-27 16:23:57
 * @LastEditTime: 2024-03-06 12:32:10
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#include "led.h"



void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    LED1_GPIO_CLK_ENABLE();
    LED2_GPIO_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = LED1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LED2_Pin;
    HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

    LED1_OFF();
    LED2_OFF();
}

