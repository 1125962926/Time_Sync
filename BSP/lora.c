/*
 * @Author: Li RF
 * @Date: 2024-02-29 10:18:28
 * @LastEditTime: 2024-03-12 09:23:05
 * @Description: 
 * Email: 1125962926@qq.com
 * Copyright (c) 2024 by Li RF, All Rights Reserved. 
 */
#include "lora.h"
#include "delay.h"
#include "oled.h"
#include "key.h"
#include "tim.h"
#include "led.h"

/********************** 常用指令预设 ****************************/
//const char Cmd_Get_Reg[3]={0xC1, 0x00, 0x09};	//查询lora模块寄存器指令

#if MASTER
//主lora模块地址固定为0x01，其余保持默认值。C0+起始地址+长度+参数
//网络地址Cmd_Set_board[5]和信道控制Cmd_Set_board[8]可自行指定
const uint8_t Cmd_Set_master_board[12]={0xC0, 0x00, 0x09, 0xff, 0xff, 0x17, 0xe5, 0x00, 0x50, 0x00, 0x00, 0x00};	

//当前通信的从机地址
uint8_t slave_num = SLAVE_START_ADDR - 1;

//记录第一次发送的时间节点
static uint16_t tim1_old;
static uint32_t tim2_old;

#else
//每个从机需要不同地址，烧录前更改LoRa_Init的传参
uint8_t Cmd_Set_slave_board[12]={0xC0, 0x00, 0x09, 0x00, 0x00, 0x17, 0xe5, 0x00, 0x50, 0x00, 0x00, 0x00};	

const char Cmd_echo[3]={SLAVE_ADDR, 0xff, 2};//回应主机报文，回应主机收到同步请求

#endif






/**
 * @description: 初始化lora模块
 * @param {uint16_t} slave_addr：从机烧录地址，在main.h中修改
 * @return {*}
 */
void LoRa_Init(void)
{
	mode_pin_init();//初始化lora模块模式引脚
	delay_ms(1000);//等待硬件自检

	OLED_ShowString(0, 2, (uint8_t *)"Addr:");
	OLED_ShowString(78, 2, (uint8_t *)"Chl:");

	if(!M0_VALUE && M1_VALUE)//进入配置模式
	{
		do
		{
#if MASTER
			LORA_Send_String((char *)Cmd_Set_master_board, 12);//配置lora
#else			
			Cmd_Set_slave_board[3] = (SLAVE_ADDR & 0xFF00) >> 8;
			Cmd_Set_slave_board[4] = SLAVE_ADDR & 0xFF;	//设置从机地址

			LORA_Send_String((char *)Cmd_Set_slave_board, 12);//配置lora
#endif

			while(!(LORA_receive_STA & 0x8000));
			LORA_receive_STA = 0;
			delay_ms(100);
		} while (LORA_receive_buff[0] != 0xC1);//检查模块回应

		OLED_ShowNum(40, 2, LORA_receive_buff[5], 2, 16);//addr
		OLED_ShowNum(110, 2, LORA_receive_buff[8], 2, 16);//channel
#if !MASTER
		OLED_ShowNum(90, 0, LORA_receive_buff[3] << 8 | LORA_receive_buff[4], 2, 16);//lora模块地址
#endif

		//在开关没有变化期间，可以通过串口配置，也可以将模块取出后配置
		while(1)
		{
			delay_ms(10);
			if(KEY_Scan())
				break;

			if(USART_RX_STA & 0x8000)//单片机发给lora
			{
				usart2_send_string((char *)USART_RX_BUF, (USART_RX_STA & 0X3FFF));
				USART_RX_STA = 0;
			}

			if(LORA_receive_STA & 0x8000)//lora发给单片机
			{
				usart_send_string((char *)LORA_receive_buff, (LORA_receive_STA & 0X3FFF));
				LORA_receive_STA = 0;
			}
		}
	}
	else//传输模式，显示预设地址
	{
#if MASTER
		OLED_ShowNum(40, 2, Cmd_Set_master_board[5], 2, 16);//addr
		OLED_ShowNum(110, 2, Cmd_Set_master_board[8], 2, 16);//channel
#else
		OLED_ShowNum(40, 2, Cmd_Set_slave_board[5], 2, 16);//addr
		OLED_ShowNum(110, 2, Cmd_Set_slave_board[8], 2, 16);//channel
		OLED_ShowNum(90, 0, SLAVE_ADDR, 2, 16);//从机模块地址
#endif
		LORA_receive_STA = 0;//不清空会使定时器无法启动（主函数判断该标志位）
	}
	
}

/**
 * @description: 初始化模式选择引脚M0，M1
 * 				M0，M1内部弱上拉，靠近M0，M1为单片机控制，数字一端为接地
 * @return {*}
 */
void mode_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	M0_GPIO_CLK_ENABLE();
	M1_GPIO_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = M0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(M0_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = M1_Pin;
	HAL_GPIO_Init(M1_GPIO_Port, &GPIO_InitStruct);

	//M0_SET(1);
	//M1_SET(1);
}







/************************* 主从分离代码 ****************************/

#if MASTER//主机独有部分
/**
 * @description: 根据手机时间刷新本地时间轴，只有主机需要24小时制时间转换，用于和手机交换信息
 * 				调用后会将 tim1 的装载值清零，计算 tim2 对应的装载值
 * @return {*}
 */




/**
 * @description: 更新本地时间，串口方式。格式：小时:分钟:秒，例： 12:30:00
 * 					调用后会将 tim1 的装载值清零，计算 tim2 对应的装载值
 * 				12:23:30
 * 				31 32 3A 32 33 3A 33 30 
 * @return {*}
 */
void Refresh_System_TIME_uart(void)
{
	uint32_t temp_period;
	uint8_t temp_24_time[3];

	temp_24_time[0] = (USART_RX_BUF[0] - '0') * 10 + USART_RX_BUF[1] - '0';
	temp_24_time[1] = (USART_RX_BUF[3] - '0') * 10 + USART_RX_BUF[4] - '0';
	temp_24_time[2] = (USART_RX_BUF[6] - '0') * 10 + USART_RX_BUF[7] - '0';

	temp_period = temp_24_time[0] * 3600 + temp_24_time[1] * 60 + temp_24_time[2];

	SetCount(0, temp_period * 10000 + 1);
}



/**
 * @description: 主机发送同步请求，并记录发送时间
 * @return {*}
 */
void Master_Send_sync_request(void)
{
	tim1_old = GetCount();
	tim2_old = globel_time;
	char temp_period[8] = {0xff};//同步请求报文，从机地址 + 0xff + 1

	slave_num++;//无论是否处理完，进入下一个
	if(slave_num == SLAVE_NUMBER + SLAVE_START_ADDR)//处理完最后一个从机会再处理一次第一个
	{
		//Master_broad_STA = 0;
		slave_num = SLAVE_START_ADDR;
	}

	temp_period[0] = slave_num;
	temp_period[1] = 0xff;
	temp_period[2] = 1;//交互的第一阶段，首次发送

	LORA_Send_String(temp_period, 8);//发送同步请求
}



/**
 * @description: 主机发送主从时间间隔差值
 * @return {*}
 */
void Master_Send_gap_time(void)
{
	if((LORA_receive_buff[0] == slave_num) && (LORA_receive_buff[1] == 0xff) && (LORA_receive_buff[2] == 2))
	{
		char temp_period[8];//发送时间报文
		uint16_t tim1_new = GetCount();
		uint32_t tim2_new = globel_time;
		uint16_t temp_tim1_period;
		uint32_t temp_tim2_period;

		//计算传输误差
		if(tim1_new < tim1_old)//定时器溢出但是没有超过计数点
		{
			temp_tim1_period = tim1_new + (12800 - tim1_old);
		}
		else
		{
			temp_tim1_period = tim1_new - tim1_old;			
		}
		temp_tim1_period /= 2;//收发两个方向，除2
		temp_tim2_period = (tim2_new - tim2_old) / 2;//得到主从传输耗时


		//矫正时间
		if(temp_tim1_period + tim1_new > 12800)//超过满装载值
		{
			temp_tim1_period = tim1_old + tim1_new - 12800;
			temp_tim2_period++;
		}
		else
		{
			temp_tim1_period = tim1_old + tim1_new;
		}	
		temp_tim2_period += tim2_new;

		//从机地址（1字节）  tim1计数（2字节） 标志位（1-2）  tim2计数（4字节）
		//标志位分别代表该数据属于双向校时的哪一环节
		temp_period[0] = slave_num;
		temp_period[1] = (temp_tim1_period & 0xff00) >> 8;
		temp_period[2] = (temp_tim1_period & 0xff) >> 0;
		temp_period[3] = 3;//交互的第三阶段，主机第二次发送
		temp_period[4] = (temp_tim2_period & 0xff000000) >> 24;
		temp_period[5] = (temp_tim2_period & 0xff0000) >> 16;
		temp_period[6] = (temp_tim2_period & 0xff00) >> 8;
		temp_period[7] = (temp_tim2_period & 0xff) >> 0;

		LORA_Send_String(temp_period, 8);//发送偏移后的时间
	}
}


#else //从机独有部分


void Slave_Receive_period(void)
{
	if((LORA_receive_buff[1] == 0xff) && (LORA_receive_buff[2] == 1))//交互的第二阶段，第一次接收
	{
		LORA_Send_String(Cmd_echo, 3);//回应主机报文
	}
	else if(LORA_receive_buff[3] == 3)//第二次接收，矫正传输耗时
	{
		uint16_t tim1_new;
		uint32_t tim2_new;

		tim1_new = ((LORA_receive_buff[1] << 8) | LORA_receive_buff[2]);
		tim2_new = ((LORA_receive_buff[4] << 24) | (LORA_receive_buff[5] << 16) |
					(LORA_receive_buff[6] << 8) | LORA_receive_buff[7]);

		SetCount(tim1_new, tim2_new);
	}
}




#endif

