/*
 * @Author: Li RF
 * @Date: 2024-02-29 10:19:29
 * @LastEditTime: 2024-03-11 14:29:31
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#ifndef __LORA_H__
#define __LORA_H__



#include "main.h"
#include "usart.h"
extern uint8_t slave_num;

/******************** 接口函数定义 **********************/
#define LORA_Send_String        usart2_send_string
#define LORA_receive_buff       USART2_RX_BUF
#define LORA_receive_STA        USART2_RX_STA




/******************** M0/M1 引脚定义 **********************/
#define M0_Pin GPIO_PIN_6
#define M0_GPIO_Port GPIOA
#define M0_GPIO_CLK_ENABLE() \
    do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while(0)

#define M1_Pin GPIO_PIN_1
#define M1_GPIO_Port GPIOA
#define M1_GPIO_CLK_ENABLE() \
    do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while(0)

//读取电平
#define M0_VALUE                  HAL_GPIO_ReadPin(M0_GPIO_Port, M0_Pin)
#define M1_VALUE                  HAL_GPIO_ReadPin(M1_GPIO_Port, M1_Pin)

//设置电平
/*
#define M0_SET(x)   do{ x ? \
                      HAL_GPIO_WritePin(M0_GPIO_Port, M0_Pin, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(M0_GPIO_Port, M0_Pin, GPIO_PIN_RESET); \
                  }while(0)

#define M1_SET(x)   do{ x ? \
                      HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(M1_GPIO_Port, M1_Pin, GPIO_PIN_RESET); \
                  }while(0)
*/

/******************** 函数定义 **********************/
void LoRa_Init(void);	  
void mode_pin_init(void);



/************************* 主从分离代码 ****************************/
#if MASTER//主机部分
void Refresh_System_TIME_uart(void);

void Master_Send_sync_request(void);
void Master_Send_gap_time(void);
#else //从机部分
void Slave_Receive_period(void);

#endif




#endif

