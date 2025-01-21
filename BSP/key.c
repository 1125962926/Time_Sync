/*
 * @Author: Li RF
 * @Date: 2024-02-27 16:50:36
 * @LastEditTime: 2024-03-11 14:53:26
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#include "key.h"
#include "delay.h"


/**
 * @description: 按键初始化
 * @return {*}
 */
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;

    S1_KEY_GPIO_CLK_ENABLE();
    
    GPIO_Initure.Pin = S1_KEY_Pin;	
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLUP;      
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;   
    HAL_GPIO_Init(S1_KEY_GPIO_Port, &GPIO_Initure);
}


/**
 * @description: 按键扫描函数
 * @return {uint8_t} 返回按键值: 0:没有按下   1: KEY按下
 */
uint8_t KEY_Scan(void)
{
    if(!S1_KEY)
    {
        delay_ms(10);
        return 1;
    }

    return 0;
}
