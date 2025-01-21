#ifndef __LED_H__
#define __LED_H__



#include "main.h"


#define LED1_Pin            GPIO_PIN_15
#define LED1_GPIO_Port      GPIOC
#define LED1_GPIO_CLK_ENABLE() \
    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while(0)

#define LED2_Pin            GPIO_PIN_14
#define LED2_GPIO_Port      GPIOC
#define LED2_GPIO_CLK_ENABLE() \
    do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while(0)



#define LED1_ON() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
#define LED1_OFF() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#define LED1_TOGGLE()   do{ HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); }while(0)

#define LED2_ON() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
#define LED2_OFF() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
#define LED2_TOGGLE()   do{ HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); }while(0)


void LED_Init(void);

#endif
