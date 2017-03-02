/**
  ******************************************************************************
  * @file    stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    13-November-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "header.h"
#include "CAN.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern uint8_t sd_ins_rem;

volatile uint8_t canconnect,canerr_clr,canerr_disp;
static volatile uint8_t count;



volatile uint8_t time_disp;
	/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
static __INLINE void LcdWriteReg(U16 Data) {
  // ... TBD by user
	LCD_REG_ADDRESS=Data;
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
static __INLINE  void LcdWriteData(U16 Data) {
  // ... TBD by user
	LCD_DATA_ADDRESS=Data;
}





/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*void SysTick_Handler(void)
{
	
}*/

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles RTC_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
		//static uint8_t count=0;
		uint32_t tmp, tmp1;																				// будем вычислять часы-минуты -секунды и обновлять соответ. ячейки памяти.
		tmp=(RTC->CNTH<<16)|RTC->CNTL;
		tmp %= 86400;		
	if (tmp==0)																			// Если настала полночь
		DateCalc();																	// то "сбегаем" обновим дату в соотв. ячейках памяти.
		
	// Вычисленное время запишем в соотв. ячейки памяти чтобы выйдя из прерывания отображать текущее время на экране 	
	  Time.hour =(uint8_t)(tmp/3600);
		tmp1 =tmp%3600; 
		Time.min =(uint8_t)(tmp1 /60);
		Time.sec =(uint8_t)(tmp1 %60);
		if(time_show)
		{
			time_disp=1;
			if(canconnect)
			{
				canerr_disp=1;
				count++;
				if(count>3)
				{
					canconnect=0;
					canerr_clr=1;
					canerr_disp=0;
				}
			}
			//GUI_SetFont(&GUI_FontArial16);	
	  }
		
		RTC->CRL &=~RTC_CRL_SECF;	
	
}
/**
  * @brief  This function handles EXTI0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler (void)
{
	uint32_t i;
	for(i=0;i<10000000;i++);
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)==0)
	{
		sd_error=SD_Init();
		if(sd_error==SD_OK)
			sd_error=SD_GetCardInfo(&sd_cardinfo);
		if(sd_error==SD_OK)
			sd_error=SD_SelectDeselect((u32)sd_cardinfo.RCA<<16);
		if(sd_error==SD_OK)
			sd_error=SD_EnableWideBusOperation(SDIO_BusWide_4b);
		if(sd_error==SD_OK)
			sd_error=SD_SetDeviceMode(SD_DMA_MODE);
		if(sd_error==SD_OK)
			f_mount(0,&fs);
	}	
	else	
	{
		SDIO_DeInit();
		f_mount(0,NULL);
	}
	sd_ins_rem=1;
	EXTI_ClearITPendingBit(EXTI_Line0);
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	
}
/**
  * @brief  This function handles EXTI0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler (void)
{
	uint32_t i;
	
	if(sleep_mode==0)
	{
		for(i=0;i<2000000;i++);
	 /*Выключаем PWM на подсветке */
	LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
	// 6 parameters to be set
	LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
	LcdWriteData(0x0000); 							// PWM duty cycle(0%)
	LcdWriteData(0x0001); 							// PWM controlled by host, PWM disable
	LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
	LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
	LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
		
	//LcdWriteReg(CMD_SET_DISPLAY_OFF);
	//LcdWriteReg(CMD_ENTER_SLEEP);
	backlight=BACKLIGHT_OFF;
	backlight_delay=0;
	sleep_mode=1;
	}
	else
		for(i=0;i<200000;i++);
	
	EXTI_ClearITPendingBit(EXTI_Line2);
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
	
}

/**
  * @brief  This function handles TIM2_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	TIM2->SR &=~TIM_SR_CC1IF;  //Сбрасываем флаг вызвавшего прерывание от захвата TIM2_CH1
	BrezErr+=BrezKoeff;
	if (BrezErr>.5f)
		{
			TIM2->CCER|=TIM_CCER_CC2E;  //Подключаем выход таймера к каналу сравнения
			BrezErr-=1;
		}
	else 
		TIM2->CCER &=~TIM_CCER_CC2E;	//Отключаем выход таймера от канала сравнения
	
}

/**
  * @brief  This function handles RTC_Alarm_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void RTCAlarm_IRQHandler(void){
		RTC->CRL&=~RTC_CRL_ALRF;
		switch(ALARMA_ACTION){
			case NONE:
			break;	
			case START_PWM:
				TIM2->CCER |=TIM_CCER_CC2E;												// Output Enable  CC2
				TIM2->CCER|=TIM_CCER_CC1E;												// Enable capture CC1	
				TimerONOFF=1;
			break;
			case STOP_PWM:
				TIM2->CCER &=~TIM_CCER_CC2E;												// Output Disable CC2
				TIM2->CCER&=~TIM_CCER_CC1E;												   //Disable capture CC1
				TimerONOFF=0;
				Brez_Count=0;
				BrezErr=0;
			break;
			case START_ADC:
			break;
			case STOP_ADC:
			break;
			default:
			break;		
		}
	
	ALARM_INT=1;
	EXTI_ClearITPendingBit(EXTI_Line17);
	NVIC_ClearPendingIRQ(RTCAlarm_IRQn);
}

/**
  * @brief  This function handles TIM6_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler (void){
	
	_CheckUpdateTouch();
	
		if(drawmode==0)
		{
			if(PROGBAR_MEM!=0)
			{
				usedbytes=GUI_ALLOC_GetNumUsedBytes();
				PROGBAR_SetValue(PROGBAR_MEM,usedbytes);
				WM_Paint(PROGBAR_MEM);
			}
		}
		TIM6->SR &= ~TIM_SR_UIF; 			//Сбрасываем флаг UIF
		NVIC_ClearPendingIRQ(TIM6_IRQn);
}
/**
  * @brief  This function handles TIM7_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler (void)
{
		backlight_delay++;
		if(backlight==BACKLIGHT_ON)
		{	
			LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
			// 6 parameters to be set
			LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
			LcdWriteData(0x0010); 							// PWM duty cycle(6%)
			LcdWriteData(0x0001); 							// PWM controlled by host, PWM disable
			LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
			LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
			LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
			backlight=BACKLIGHT_LOW;
		}		
	else if((backlight==BACKLIGHT_LOW)&&(backlight_delay==2))
		{
		/* Выключаем PWM на подсветке */
			LcdWriteReg(CMD_SET_PWM_CONF); 			//set PWM for Backlight. Manual p.53
			// 6 parameters to be set
			LcdWriteData(0x0004); 							// PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
			LcdWriteData(0x0000); 							// PWM duty cycle(0%)
			LcdWriteData(0x0001); 							// PWM controlled by host, PWM disable
			LcdWriteData(0x00f0); 							// brightness level 0x00 - 0xFF
			LcdWriteData(0x0000); 							// minimum brightness level =  0x00 - 0xFF
			LcdWriteData(0x0000);								// brightness prescalar 0x0 - 0xF
			
			//LcdWriteReg(CMD_SET_DISPLAY_OFF);
			//LcdWriteReg(CMD_ENTER_SLEEP);
			backlight=BACKLIGHT_OFF;
			//backlight_delay=0;
			sleep_mode=1;
		}
		//else if((backlight==1)&&(backlight_delay==1))
		//{
		//sleep_mode=1;
		//backlight_delay=0;	
		//}
		TIM7->SR &= ~TIM_SR_UIF; 			//Сбрасываем флаг UIF
		NVIC_ClearPendingIRQ(TIM7_IRQn);
		
}
/**
  * @brief  This function handles USB_LP_CAN1_RX0_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void){
	
	CAN_Receive_IRQHandler(0);
	CAN_RXProcess0();
	
}
void CAN1_RX1_IRQHandler(void){
	canconnect=1;//canerr_clr=0;
	count=0;
	CAN_Receive_IRQHandler(1);
	CAN_RXProcess1();
	
}
/**
  * @brief  This function handles SDIO_IRQHandler interrupt request.
  * @param  None
  * @retval None
  */
void SDIO_IRQHandler(void)
{
	SD_ProcessIRQSrc();
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
