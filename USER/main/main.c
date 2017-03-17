#include "stm32f10x.h"
#include <stdlib.h>
#include <header.h>
#include "DIALOG.h"
#include "CAN.h"
void assert_failed(uint8_t* file, uint32_t line)
{
	while(1){};
}

#define LEDPIN							GPIO_Pin_6
#define LEDPORT						  GPIOC

#define ID_BUTTON_YES (GUI_ID_USER + 0x01)
#define ID_BUTTON_NO (GUI_ID_USER + 0x20)

uint8_t yes=1,no=1;
extern GUI_CONST_STORAGE GUI_BITMAP bmphoto;
WM_HWIN hButton_YES,hButton_NO;

//uint32_t array[]={exitt,photo,screen,paint,next,prev,date,pwm,sd,alarm,AlarmA,AlarmB,Alarm_d,add_folder,del_folder};
//uint32_t farray[]={fexit,fphoto,fscreen,fpaint,fnext,fprev,fdate,fpwm,fsd,falarm,fAlarmA,fAlarmB,fAlarm_d,fadd_folder,fdel_folder};
/*GUI_CONST_STORAGE GUI_BITMAP *p[]={&bmexit,&bmphoto,&bmscreen,&bmpaint,&bmnext,&bmprev,&bmdate,&bmpwm,&bmsd,&bmalarm,
																		&bmAlarmA,&bmAlarmB,&bmAlarm_D,&bmadd_folder,&bmdel_folder};*/

const char *CardType[]={"STD_CAPACITY_SD_CARD_V1_x",
												"STD_CAPACITY_SD_CARD_V2_0",
												"HIGH_CAPACITY_SD_CARD",
												"MULTIMEDIA_CARD",
												"SECURE_DIGITAL_IO_CARD",
												"HIGH_SPEED_MULTIMEDIA_CARD",
												"SECURE_DIGITAL_IO_COMBO_CARD",
												"HIGH_CAPACITY_MMC_CARD"};

											
EXTI_InitTypeDef							EXTI_InitStruct;							
SPI_InitTypeDef 							SPI_InitStruct;
GPIO_InitTypeDef 							GPIO_InitStruct;
DMA_InitTypeDef 							DMA_InitStruct;
FSMC_NORSRAMTimingInitTypeDef  FMC_NORSRAMTiming;
FSMC_NORSRAMInitTypeDef        FMC_NORSRAM;
												
												
/****************************************************************/
/* 															Инициализация RTC								*/																					
/****************************************************************/
void RTC_Init(void){
		RCC->APB1ENR |= RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN;								// Включим тактирование BKP и PWR интерфейса  
		PWR_BackupAccessCmd(ENABLE);																					// Разрешим доступ к бэкап домену установкой бита DBP
		if ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY)  							// Проверим если часы запущены то выход из инициализации
		// Если часы не включены проинициализируем их и загрузим значения делителя и начальное значение счетчика
			{	
				RCC->BDCR |= RCC_BDCR_BDRST;																	// Установим сброс на BKP  домен 
				RCC->BDCR &=~ RCC_BDCR_BDRST;																	// Снимим сброс с BKP  домена 
				RCC->BDCR |= RCC_BDCR_LSEON;																	// Включим 	усилитель кварца 32768		
																																			// Дождемся установки бита готовности LSERDY
				while((RCC->BDCR&RCC_BDCR_LSERDY)!=RCC_BDCR_LSERDY){}	/*Ждем установки LSERDY или примерно 1.5 сек*/
				RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;															/*Если флаг установился настраиваем RTC*/	
				RCC->BDCR|=RCC_BDCR_RTCEN;
				
				while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
				RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
				RTC->PRLL = 0x7fff;																						// Загрузим в предделитель число 0x7fff 
				RTC->CRL &=~ RTC_CRL_CNF;																		
				while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
				
				RTC->CRL &=~RTC_CRL_RSF;																			// После запуска часов сбросим флаг RSF	
				while ((RTC->CRL &RTC_CRL_RSF)!=RTC_CRL_RSF){}								// И дождемся его аппаратной установки
				//while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
				//RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
				//RTC->ALRH =BKP->DR1;
				//RTC->ALRL =BKP->DR2;
				//RTC->CRL &=~ RTC_CRL_CNF;																		
				//while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}					// Дождемся готовности часов для записи
			}	
			RTC->CRH |= RTC_CRH_SECIE;				// Разрешим прерывание от секундных импульсов в модуле RTC SECIE
}
/****************************************************************/
/*		    Настройка конфигурации  периферийных модулей		      */
/****************************************************************/
void Periph_Init(void){
		
	RCC_AHBPeriphClockCmd(	RCC_AHBPeriph_DMA2|
													RCC_AHBPeriph_CRC	|	
													RCC_AHBPeriph_FSMC|
													RCC_AHBPeriph_SDIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|
													 RCC_APB2Periph_GPIOA|
													 RCC_APB2Periph_GPIOB|
													 RCC_APB2Periph_GPIOC|
													 RCC_APB2Periph_GPIOD|
													 RCC_APB2Periph_GPIOE|
													 RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|
													 RCC_APB1Periph_SPI2|
													 RCC_APB1Periph_PWR|
													 RCC_APB1Periph_TIM6|
													 RCC_APB1Periph_TIM7|
											     RCC_APB1Periph_TIM2,ENABLE);
			
/********************************************************************/
	/*	 		Настройка PB5 управление ключом питания LCD модуля																																*/
/********************************************************************/	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIOB->BSRR=GPIO_BSRR_BR5;		//открываем нижний MOSFET  подаем питание на LCD
/*******************************************************************/	
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line2;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	AFIO->EXTICR[0]=AFIO_EXTICR1_EXTI2_PA;		// PA2 подключим к EXTI2   
	
		
/***********************************************/
/*				  	Настройка будильника	       		 */			
/***********************************************/
	EXTI_InitStruct.EXTI_Line=EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
/********************************************************************/
/*					SDIO card insert  PE0  																*/
/********************************************************************/	

		//GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
		//GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;			// config PE0 in SD_LowLevel_Init();
		//GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
		//GPIO_Init(GPIOE,&GPIO_InitStruct);
		
		AFIO->EXTICR[0]=AFIO_EXTICR1_EXTI0_PE		;//SYSCFG->EXTICR[0]=SYSCFG_EXTICR1_EXTI1_PB;
		
		EXTI_InitStruct.EXTI_Line=EXTI_Line0;
		EXTI_InitStruct.EXTI_LineCmd=ENABLE;
		EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
		EXTI_Init(&EXTI_InitStruct);

/*****************************************************************************
*        LED pin  (PC6)    Вывод моргания светодиодом при CAN обновлении     *
******************************************************************************/
		GPIO_InitStruct.GPIO_Pin=LEDPIN;
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_Init(LEDPORT,&GPIO_InitStruct);			
		

/*****************************************************************************/
/* 															Инициализация таймера TIM2									 */	
/*****************************************************************************/

/* Настраиваем выводы для таймера TIM2: PA0 как Input pull-up  (TIM2_CH1_ETR)*/
/*																			PA1 как Alternate function push-pull  (TIM2_CH2)*/
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
		
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
/*Настройка канала захвата*/
	TIM2->SMCR |=TIM_SMCR_TS_0|				//Источник синхронизации внешнего сигнала Filtered Timer Input 1(TI1FP1)
							 TIM_SMCR_TS_2|				// TS[2:0] 101
					  	 TIM_SMCR_SMS_1|
							 TIM_SMCR_SMS_2;			// SMS[2:0] 110 Slave mode selection выбираем Trigger mode
	TIM2->CCMR1 |=TIM_CCMR1_CC1S_0;		// CC1 Channel is configured as input, IC1 is mapped on TI1

// Настроим входной фильтр триггера для захвата входного сигнала по спадающему фронту
	TIM2->CCER|=TIM_CCER_CC1P;
	
/* Настройка канала сравнения*/
// Выбираем режим  PWM mode 2  установкой битов OC1M регистра TIM2->CCMR1 [2:0]  111
	TIM2->CCMR1 |=TIM_CCMR1_OC2M|TIM_CCMR1_OC2PE; 			//Output compare 2 preload enable
//Зададим коэффициент деления для CK_CNT=CK_PSC/(PSC[15:0]+1)  90000000/(899+1) =100KHz
	TIM2->PSC=719;
	//Включим режим одного импульcа и предварительной загрузки регистра ARR
	TIM2->CR1 |=TIM_CR1_OPM|TIM_CR1_ARPE;

/*****************************************************************************/
/* 															Инициализация таймера TIM6									 */	
/*****************************************************************************/
	TIM6->PSC = 36000 - 1; 			// Настраиваем делитель что таймер тикал 2000 раз в секунду
	TIM6->ARR = 200 ; 					// Чтоб прерывание случалось 10 раз в секунду
	TIM6->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера обновления структуры GUI_PID_STATE
	TIM6->EGR = TIM_EGR_UG;		//генерируем "update event". ARR и PSC грузятся из предварительного в теневой регистр. 
	TIM6->SR&=~TIM_SR_UIF; 			//Сбрасываем флаг UIF
	NVIC_ClearPendingIRQ(TIM6_IRQn);
/*****************************************************************************/
/* 															Инициализация таймера TIM7									 */	
/*****************************************************************************/
	TIM7->PSC = 36000 - 1; 			// Настраиваем делитель что таймер тикал 2000 раз в секунду
	TIM7->ARR = 60000 ; 				// Чтоб прерывание случалось  раз в 30 секунд
  TIM7->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера подсветки дисплея
	TIM7->EGR = TIM_EGR_UG;			//генерируем "update event". ARR и PSC грузятся из предварительного в теневой регистр. 
	TIM7->SR&=~TIM_SR_UIF; 			//Сбрасываем флаг UIF
	NVIC_ClearPendingIRQ(TIM7_IRQn);
/********************************************************************/
/* 										Инициализация контроллера NVIC							 	*/
/********************************************************************/
	NVIC_SetPriority(TIM2_IRQn,2);
	NVIC_SetPriority(TIM6_IRQn,1);
	NVIC_SetPriority(TIM7_IRQn,2);
	NVIC_SetPriority(RTC_IRQn,2);
	NVIC_SetPriority(SDIO_IRQn,0);
	NVIC_SetPriority(EXTI0_IRQn,2);
	NVIC_SetPriority(EXTI2_IRQn,0);
	
	NVIC_SetPriority(RTCAlarm_IRQn,2);
	//DBGMCU->CR|=DBGMCU_CR_DBG_SLEEP;
	//DBGMCU->APB1FZ|=DBGMCU_APB1_FZ_DBG_TIM2_STOP;
}

/****************************************************************/
/*											Функция  сallback Boot_menu							*/
/****************************************************************/
static void _cbBoot_menu(WM_MESSAGE* pMsg) {
  int     NCode;
  int     Id;
	
	switch(pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				switch(Id){
					case ID_BUTTON_YES:
					switch(NCode){
							case WM_NOTIFICATION_CLICKED:
								yes=0;
								
							break;
						}
					break;
					case ID_BUTTON_NO:
					switch(NCode){
							case WM_NOTIFICATION_CLICKED:
								no=0;
								
							break;
						}
					break;	
					}	
	break;
	default:
    WM_DefaultProc(pMsg);
  }
}					
							
/****************************************************************/
/*					       Функция     Boot_menu                 	      */
/****************************************************************/
SD_Error Boot_menu (void){
	//void (*pMainTask)(void);
	uint32_t MULT,memory_capacity;
	float sd_size;
	
	yes=1; no=1;
	GUI_SetFont(&GUI_FontArial16);
	GUI_SetColor(GUI_YELLOW);
	if(sd_error!=SD_OK)
		{
		sd_error=SD_Init();
		if(sd_error!=SD_OK) return sd_error;
			
		sd_error=SD_GetCardInfo(&sd_cardinfo);
		if(sd_error!=SD_OK) return sd_error;
			
		sd_error=SD_SelectDeselect((u32)sd_cardinfo.RCA<<16);
		if(sd_error!=SD_OK) return sd_error;
			
		sd_error=SD_EnableWideBusOperation(SDIO_BusWide_4b);
		if(sd_error!=SD_OK) return sd_error;
			
		sd_error=SD_SetDeviceMode(SD_DMA_MODE);
		if(sd_error!=SD_OK) return sd_error;
			
		f_mount (0,&fs);
		}		
	 
	 GUI_DispStringAt("ЗАГРУЗКА С SD КАРТЫ ? ",170,50);
	 BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
   
	 hButton_YES=BUTTON_CreateEx(150,100,40,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_YES);
	 BUTTON_SetFont(hButton_YES, &GUI_FontArial16);
	 BUTTON_SetText(hButton_YES, "ДА");
	 
	 hButton_NO=BUTTON_CreateEx(300,100,40,30,WM_HBKWIN,WM_CF_SHOW,0,ID_BUTTON_NO);
	 BUTTON_SetFont(hButton_NO, &GUI_FontArial16);
	 BUTTON_SetText(hButton_NO,"НЕТ");
	 
	 WM_SetCallback(WM_HBKWIN, _cbBoot_menu);
		while(yes&&no)
		{
			GUI_Exec();
		}
		WM_DeleteWindow(hButton_YES);
		WM_DeleteWindow(hButton_NO);
		hButton_YES=0;
		hButton_NO=0;
		GUI_Clear();
		if(!yes)
		{
		f_mount (0,&fs);
			fresult=f_opendir(&dir, _T("0:"));
		if(fresult==FR_OK)
			{					
			GUI_DispStringAt(" CARD TYPE : ",5,10);
			GUI_DispString(CardType[sd_cardinfo.CardType]);
										
			GUI_DispStringAt(" SD_CARD SIZE :  ",5,30);
			MULT=1<<(sd_cardinfo.SD_csd.DeviceSizeMul+2);
			memory_capacity=(sd_cardinfo.SD_csd.DeviceSize+1)*MULT*(1<<sd_cardinfo.SD_csd.RdBlockLen);
			sd_size=memory_capacity/1048576.0f;
			GUI_DispFloat(sd_size,6);
			GUI_DispString(" MBYTE");
					
			GUI_DispStringAt(" SECTOR SIZE : ",5,50);
			GUI_DispDec(SD_SECTOR_SIZE,3);
			GUI_DispString("  BYTE");
				
			GUI_DispStringAt(" RRODUCT SN : ",5,70);
			GUI_DispDec(sd_cardinfo.SD_cid.ProdSN,10);
			
			GUI_DispStringAt(" RCA : ",5,90);
			GUI_DispDec(sd_cardinfo.RCA,5);
					
			GUI_DispStringAt(" MANUFACTURER ID : ",5,110);
			GUI_DispDec(sd_cardinfo.SD_cid.ManufacturerID,2);
		  GUI_MessageBox("SD card is OK!", "Message", 0);
			return sd_error;
			//fresult=f_open (&fil, _T("0:stm32.hex"), FA_READ);	// open file
			/*if(fresult==FR_OK)	
				{	
					for(;;)
						{
							fresult=f_read(&fil,(uint32_t*)(SDRAM_BASE+0x30000),fil.fsize,&br);
							if (fresult || br == 0) 
									break;    // error or eof 
						}		
						f_close(&fil);
						Hex_to_Bin((uint8_t*)(SDRAM_BASE+0x30000));			
						pMainTask=(void(*)(void))*(__IO uint32_t*)(SDRAM_BASE+4);	
						SCB->VTOR=SDRAM_BASE;
						__set_MSP(*(__IO uint32_t*)SDRAM_BASE);
						pMainTask();
				}*/
			}				
			else
			{	
			GUI_MessageBox("SD card is not initialize", "Message", 0);
			fresult=f_mount (0,NULL);
			}	
			
		}
		else
		{
			__nop();
			
		}
	
	return sd_error;
}	


void DateCalc(void){																		// Функция Вычисления даты по значению секунд 
	int day=1, year=0,month=0, i,leap;										// с регистра RTC->CNT
	day+=(RTC->CNTH<<16 |RTC->CNTL)/86400;
	// Сначала вычислим год. Сохраним значение в ячейке year	
		while (day>=366)
			{		
				day-=366;
				year++;
				for (i=0;i<3;i++)
					{	
						if (day<=365)
						break;
						day-=365;
						year++;
					}
			 }
			 // Проверим является ли полученный год високосным	
	 if(year%4!=0)
			leap=0;
		else
			leap=1;
		// Остаток дней от вычисления года переведем в месяцы и дни		
		for (i=0; day>days[leap][i];i++)
				{
				month++;
				day-=days[leap][i];
				}
				// Запишем все в соответ. ячейки памяти предварительно приведя к виду удобному для индикации 		
		Time.year=year;
		Time.month=month;
		Time.day=day;
		GUI_SetFont(&GUI_Font8x16);
		GUI_DispDecAt(Time.day,5,0,2);
		GUI_DispString(":");
		GUI_DispDec(Time.month,2);
		GUI_DispString(":20");
		GUI_DispDec(Time.year,2);		
		
}
void AlarmCalc(void){																		// Функция Вычисления даты и времени будильника по значению секунд 
	int day=1,sec,year=0, month=0, i,leap,tmp;						// взятых c регистра BKP->DR1 BKP->DR2
	//sec=((RTC->ALRH)<<16 |RTC->ALRL)%86400;
	sec=((BKP->DR1)<<16 |BKP->DR2)%86400;
	// Сначала вычислим часы и минуты . Запишем в соответ. ячейки памяти 	 
		
		Alarm.hour =sec/3600;
		tmp =sec%3600; 
		Alarm.min =tmp /60;
		
	
	// Далее вычислим число дней.
	//day+=(RTC->ALRH<<16 |RTC->ALRL)/86400;
	day+=(BKP->DR1<<16 |BKP->DR2)/86400;
	//По количеству дней будем определять год. Сохраним полученное значение в ячейке year/	
	while (day>=366)
		{		
			day-=366;
			year++;
			for (i=0;i<3;i++)
				{	
					if (day<=365)
					break;
					day-=365;
					year++;
				 }
			}
			// Проверим полученный год на високосность	
	if(year%4!=0)
		leap=0;
	else
		leap=1;
	// Из остатка дней вычислим сначала месяц а остаток будет днеём месяца	
	for (i=0; day>days[leap][i];i++)
			{
			month++;
			day-=days[leap][i];
			}
			// Запишем все в соответ. ячейки памяти приведя к виду убодному для индикации.	
	Alarm.year=year;
	Alarm.month=month;
	Alarm.day=day;
}

/****************************************************************/
/*					                MAIN	                      	      */
/****************************************************************/
int main(void){
	//pBoot_menu=(SD_Error(*)(void))Boot_menu;
	//pGUI_Init=(void(*)(void))GUI_Init;
	
	Periph_Init();
	Init_FSMC_GPIO();							
	RTC_Init();	
	MX25_LowLevel_Init();
	TSC2046_LowLevel_Init();
	bxCAN_Init();
		
	if(!brightness)
		brightness=BRIGHTNESS_MED;
	
	/*SCB->SCR|=SCB_SCR_SLEEPDEEP;					// Разрешаем SLEEPDEEP по команде WFI WFE
	PWR->CR&= ~PWR_CR_PDDS;									// Сбрасываем бит PDDS (Stop mode)
	PWR->CR|=	PWR_CR_LPDS;			*/					// Voltage regulator low-power during Stop mode
	
	DBGMCU->CR|=DBGMCU_CR_DBG_STOP;
	DBGMCU->CR|=DBGMCU_CR_DBG_CAN1_STOP;
	
	NVIC_EnableIRQ(TIM2_IRQn); 									//Разрешение TIM2_IRQn прерывания
	NVIC_EnableIRQ(TIM6_IRQn); 									//Разрешение TIM6_DAC_IRQn прерывания
	NVIC_EnableIRQ(TIM7_IRQn); 									//Разрешение TIM7_IRQn прерывания
	NVIC_EnableIRQ(RTC_IRQn);										//Разрешение RTC_IRQn прерывания
	NVIC_EnableIRQ(SDIO_IRQn);									//Разрешение SDIO_IRQn прерывания
	
	NVIC_EnableIRQ(EXTI2_IRQn);		
	NVIC_EnableIRQ(RTCAlarm_IRQn);
	
	GUI_Init();
	
	TIM6->CR1 |= TIM_CR1_CEN; 	// Начать отсчёт!	
	TIM7->CR1 |= TIM_CR1_CEN; 	// Начать отсчёт!
	
	if(BKP->DR4==0)
		Touch_calibration();
	
	
	GUI_UC_SetEncodeUTF8();
	if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)==0)
		{
			if(Boot_menu()!=SD_OK)
			{
			 MainTask();
			}
			else
			{
			 __nop();
			 MainTask();
			}
		}		
	else
		{
		 __nop();
		 MainTask();
		}
	while(1) {
	
		
	}
}
