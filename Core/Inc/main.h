/*
 * @Author: Li RF
 * @Date: 2024-02-27 14:28:06
 * @LastEditTime: 2024-03-12 09:32:36
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#ifndef __MAIN_H
#define __MAIN_H



/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "stm32g0xx.h"
#include <stdio.h>
#include <string.h>

/**
 * @description: 主从切换开关
 * 	MASTER 为1时，代码用于Lora校时主机
 * 	MASTER 为0时，代码用于Lora从机
 */
#define MASTER 0

#if !MASTER
/**
 * @description: 本次烧录从机地址，由于发送采用递增遍历，所有从机地址需连续
 */
#define SLAVE_ADDR  3

#else
/**
 * @description: 从机数量
 */
#define SLAVE_NUMBER  3

/**
 * @description: 从机最小地址，用于递增遍历
 */
#define SLAVE_START_ADDR  3

#endif




void Error_Handler(void);
void SystemClock_Config(void);




#endif /* __MAIN_H */
