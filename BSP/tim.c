#include "tim.h"
#include "led.h"
#include "oled.h"
#include "usart.h"
#include "delay.h"


TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

//本地全局时间  
uint32_t globel_time;

//小时，分钟，秒，用于oled显示
uint8_t sys_24_time[3] = {0};

#if MASTER
//广播就绪标志，将广播放到主函数执行，防止降低定时器精度
uint8_t Master_broad_STA;

//处理从机就绪标志
uint8_t handle_OK_STA;


#endif





/**
 * @description: TIM1 init function
 * @param {uint16_t} period
 * @param {uint8_t} prescaler
 * @return {*}
 */
void TIM1_Init(uint16_t period, uint8_t prescaler)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = prescaler;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = period;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_TIM_Base_Start_IT(&htim1);
}




void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	if(tim_baseHandle->Instance==TIM1)
	{
		/** Initializes the peripherals clocks
		*/
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1;
		PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLKSOURCE_PLL;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		/* TIM1 clock enable */
		__HAL_RCC_TIM1_CLK_ENABLE();

		/* TIM1 interrupt Init */
		HAL_NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 3, 3);
		HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

	}

}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim1);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#if MASTER
	//定时器溢出计时，用于主机广播
	static uint16_t count1 = 0;
	static uint16_t count2 = 0;
#endif

	if(htim == (&htim1))
	{
		globel_time++;//全局时间计数
#if MASTER
		count1++;
		count2++;
		if(count1 >= 2000)//每隔1秒处理一个从机
		{
			count1 = 0;
			handle_OK_STA = 1;//可以处理从机
		}
		if(count2 >= 2000 * 10)//1s * 60，同步一次所有从机
		{
			count2 = 0;
			Master_broad_STA = 1;//开始请求校时
		}
#endif

	}

}

/**
 * @description: 获取定时器的计数值
 * @return {uint16_t} 定时器的计数值
 */
uint16_t GetCount(void)
{
	return __HAL_TIM_GET_COUNTER(&htim1);
}


/**
 * @description: 设置定时器的计数值
 * @param {uint16_t} 定时器1的计数值
 * @param {uint32_t} globel_time全局时间的值
 * @return {*}
 */
void SetCount(uint16_t timer1, uint32_t timer2)
{
	HAL_TIM_Base_Stop_IT(&htim1);

	__HAL_TIM_SET_COUNTER(&htim1, timer1);
	globel_time = timer2;

	HAL_TIM_Base_Start_IT(&htim1);

}





/**
 * @description: 将系统时间转换为24小时制，用于屏幕显示
 * @return {*}
 */
void System_TIME_to_oled(void)
{
	int time2 = globel_time * 0.0001;//定时器2计数值转换为秒
	uint16_t time_count = (globel_time / 10) % 1000;//毫秒（ms）为单位显示

	sys_24_time[0] = time2 / 3600;
	time2 %= 3600;
	sys_24_time[1] = time2 / 60;
	sys_24_time[2] = time2 % 60;
	
	OLED_ShowNum(0, 6, sys_24_time[0], 2, 16);//时
	OLED_ShowNum(24, 6, sys_24_time[1], 2, 16);//分
	OLED_ShowNum(48, 6, sys_24_time[2], 2, 16);//秒
	OLED_ShowNum(80, 6, time_count, 3, 16);//毫秒
}

