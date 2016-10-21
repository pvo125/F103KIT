

#include <stm32f10x.h>
#include "header.h"

#include "ssd1963.h"

extern GPIO_InitTypeDef 										GPIO_InitStruct;
extern DMA_InitTypeDef											DMA_InitStruct;
FSMC_NORSRAMTimingInitTypeDef				FSMC_NORSRAMTimingInitStruct;
FSMC_NORSRAMInitTypeDef							FSMC_NORSRAMInitStruct;
extern SPI_InitTypeDef											SPI_InitStruct;



void delay(uint32_t uSec){
	SysTick->LOAD=(uSec*9);
	SysTick->VAL=0;
	while(!(SysTick->CTRL &SysTick_CTRL_COUNTFLAG_Msk)){}	
}

//=====================================================================================

//	

//=====================================================================================

#define  HT  		532	 // horisontal total         
#define  HDP	 	479  // horizontal width     480
#define  HFP   	8		// horizontal front porch 2-8
#define  HBP   	42		//horizontal back porch   8-43
#define  HS    	2	// horizontal pulse width   1-

#define  VT    288 		// vertical total
#define  VDP	 271	// vertical width
#define  VFP   2			// vertical front porch
#define  VBP   6			// vertical back porch	
#define  VS    6			// vertical pulse width


//=====================================================================================
static __INLINE void LcdWriteReg(U16 Data) {
  // ... TBD by user
	LCD_REG_ADDRESS=Data;
}

//********************************************************************

static __INLINE  void LcdWriteData(U16 Data) {
  // ... TBD by user
	LCD_DATA_ADDRESS=Data;
}



void LCD_WR_REG(unsigned int command)
{
	*(__IO uint16_t *) (Bank1_LCD_Ctrl) =  command;
}


void LCD_WR_Data(unsigned int val)
{   
	*(__IO uint16_t *) (Bank1_LCD_Data) =  val; 	
}



void LCD_WR_CMD(unsigned int index, unsigned int val)
{	
	*(__IO uint16_t *) (Bank1_LCD_Ctrl) =  index;	
	*(__IO uint16_t *) (Bank1_LCD_Data) =  val;
}


unsigned int LCD_RD_REG(unsigned int command)
{
	 *(__IO uint16_t *) (Bank1_LCD_Ctrl) =  command;

	return *(__IO uint16_t *) (Bank1_LCD_Data);
}


unsigned int LCD_RD_Data(void)
{
	unsigned int a = 0;
	//a = (*(__IO uint16_t *) (Bank1_LCD_Data)); 	//Dummy
	//a =  *(__IO uint16_t *) (Bank1_LCD_Data);  	//H
	//a = a<<8;
	a = *(__IO uint16_t *) (Bank1_LCD_Data); //L

	return(a);	
}


void Set_COL_PAGE_Address(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	
  // Set the start column and page addresses. 
	// sequential 8 bit values. 
	LCD_WR_REG(CMD_SET_COL_ADDRESS); // Manual p.29	
	LCD_WR_Data(x1 >> 8);	// start column high byte    
	LCD_WR_Data(x1 & 0x00ff);	// start column low byte
	LCD_WR_Data(x2 >> 8);  	// end column # high byte	    
	LCD_WR_Data(x2 & 0x00ff); // end column # low byte

  LCD_WR_REG(CMD_SET_PAGE_ADDRESS); // Manual p.29	
	LCD_WR_Data(y1 >> 8);	// start row high byte     
	LCD_WR_Data(y1 & 0x00ff);	// start row low byte
	LCD_WR_Data(y2 >> 8);  	// end row high byte	    
	LCD_WR_Data(y2 & 0x00ff);  // end row low byte
	// Set writing to commence
	LCD_WR_REG(CMD_WRITE_MEM_START);

	return;
} 


void LCD_DrawEllipse(int x1, int y1, int x2, int y2, short color)
{  	

	double x0, y0, a,b, dtest;
	int x, y;
 	int lineWidth;
	int height;
	
	lineWidth  =  x2 - x1 + 1;
	height  =  y2 - y1 + 1;

	Set_COL_PAGE_Address(x1,y1,x2,y2);

	// This is drawing in the [x1,y1] to [x2,y2] region
	x0 = (x1+x2) / 2;
	y0 = (y1+y2) / 2;
	a = (x2-x1) / 2;
	b = (y2-y1) / 2;
	a = a*a;
	b = b*b;

	for(y = y1;y < y1 + height; y++)
	{
		for(x = x1; x < x1 + lineWidth; x++)
		{   
			// Test if the point is in the ellipse and draw it
			dtest =	((x-x0)*(x-x0)/a + (y-y0)*(y-y0)/b);
			if (1.0 > dtest)
          		LCD_WR_Data(color);
			else // increment the counter without writing
			{	
				LCD_WR_Data(0xffff); // TODO: just incr the counter
			}
		}
	}

}

// PiXCLe command primitive
void LCD_DrawBackground(short color)
{
    unsigned int lineWidth  =  HDP + 1,height  =  VDP + 1, w;
	Set_COL_PAGE_Address(0,0,HDP,VDP);
	
	// The 4.3" LCD is 480x272
	while(lineWidth--)
	{
	    for(w = 0;w < height;w++)
		{   
			// Write a single pixel.
          	LCD_WR_Data(color);
		}
	}
}


void LCD_DrawRectFull(uint16_t x,uint16_t y,uint16_t width,uint16_t heigh,uint16_t color)
{
	uint32_t i,square=width*heigh;
	
	Set_COL_PAGE_Address(x,y,x+width-1,y+heigh-1);
	for(i=0;i<square;i++)
		LCD_WR_Data(color);
}

void LCD_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t heigh,uint16_t color)
{
	LCD_DrawLineH(x,y,width,color);
	LCD_DrawLineH(x,y+heigh-1,width,color);
	LCD_DrawLineV(x,y,heigh,color);
	LCD_DrawLineV(x+width-1,y,heigh,color);
}

void LCD_DrawLineH(uint16_t x,uint16_t y,uint16_t width, uint16_t color)
{	
	uint16_t i;
	Set_COL_PAGE_Address(x,y,x+width-1,y);
	for(i=0;i<width;i++)
		LCD_WR_Data(color);
}


void LCD_DrawLineV(uint16_t x,uint16_t y,uint16_t heigh, uint16_t color)
{	
	uint16_t i;
	Set_COL_PAGE_Address(x,y,x,y+heigh-1);
	for(i=0;i<heigh;i++)
		LCD_WR_Data(color);
}



void LCD_PutPixel(uint16_t x,uint16_t y,uint16_t color)
{
	Set_COL_PAGE_Address(x,y,x,y);
	*(__IO uint16_t *) (Bank1_LCD_Data) =  color;
}



void LCD_DrawBMP(const unsigned short *bitmap)
{
	uint32_t i;
	Set_COL_PAGE_Address(0,0,HDP,VDP);
	for(i=0;i<130560;i++)
		LCD_WR_Data(bitmap[i]);
	
}

  

void SetScrollArea(unsigned int top, unsigned int scroll, unsigned int bottom)
{


//	WriteCommand(CMD_SET_SCROLL_AREA);
//	CS_LAT_BIT  =  0;
	LCD_WR_REG(0x33);
	LCD_WR_Data(top>>8);
	LCD_WR_Data(top);
	LCD_WR_Data(scroll>>8);
	LCD_WR_Data(scroll);
	LCD_WR_Data(bottom>>8);
	LCD_WR_Data(bottom);
//	CS_LAT_BIT  =  1;	
}




void SetScrollStart(unsigned int line)
{

//	LCD_WR_REG(0x002A);	
//    LCD_WR_Data(0);	  
//	LCD_WR_REG(CMD_SET_SCROLL_START);
	LCD_WR_REG(0x37);
//	CS_LAT_BIT  =  0;
	LCD_WR_Data(line>>8);
	LCD_WR_Data(line);	
//	CS_LAT_BIT  =  1;
}



void SetTearingCfg(unsigned char state, unsigned char mode)
{


	if(state  == 1)
	{
		LCD_WR_REG(0x35);
		//CS_LAT_BIT  =  0;
		LCD_WR_Data(mode&0x01);
		//CS_LAT_BIT  =  1;
	}
	else
	{
		LCD_WR_REG(0x34);
	}

	
}



void Init_FSMC_GPIO(void)
{
/*=============================================================*/
/*											GPIO Init															 */
/*=============================================================*/	
/* FSMC output - port D */	
	
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0|			/*	DB2	*/
														GPIO_Pin_1|			/*	DB3	*/
														GPIO_Pin_4|			/*	NOE	*/
														GPIO_Pin_5|			/*	NWE	*/
														GPIO_Pin_7|			/*	NE1	*/
														GPIO_Pin_8|			/*	DB13 */
														GPIO_Pin_9|			/*	DB14 */
														GPIO_Pin_10|		/*	DB15 */
														GPIO_Pin_11|		/*	A16	*/
														GPIO_Pin_14|		/*	DB0	*/
														GPIO_Pin_15;		/*	DB1	*/
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStruct);	
/*  FSMC output - port E  */
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7|			/* DB4  */
														GPIO_Pin_8|			/* DB5  */
														GPIO_Pin_9|			/* DB6  */
														GPIO_Pin_10|		/* DB7  */	
														GPIO_Pin_11|		/* DB8  */
														GPIO_Pin_12|		/* DB9  */
														GPIO_Pin_13|		/* DB10  */
														GPIO_Pin_14|		/* DB11  */
														GPIO_Pin_15;		/* DB12  */	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);

/* LCD reset(GPIO output) - port E */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;	/* PE1  - LCD RESET */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOE, &GPIO_InitStruct);	
/* Backlight(GPIO output) - port D */
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;	/* PD13 - BACKLIGHT */
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOD, &GPIO_InitStruct);	
	GPIO_SetBits(GPIOE, GPIO_Pin_1);

/*==============================================================================*/
/*										FSMC Init																									*/
/*==============================================================================*/
		FSMC_NORSRAMTimingInitStruct.FSMC_AccessMode=FSMC_AccessMode_A;
		FSMC_NORSRAMTimingInitStruct.FSMC_AddressHoldTime=1;
		FSMC_NORSRAMTimingInitStruct.FSMC_AddressSetupTime=2;
		FSMC_NORSRAMTimingInitStruct.FSMC_BusTurnAroundDuration=0;
		FSMC_NORSRAMTimingInitStruct.FSMC_CLKDivision=0;
		FSMC_NORSRAMTimingInitStruct.FSMC_DataLatency=1;
		FSMC_NORSRAMTimingInitStruct.FSMC_DataSetupTime=2;
	FSMC_NORSRAMInitStruct.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStruct.FSMC_Bank=FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStruct.FSMC_BurstAccessMode=FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_DataAddressMux=FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ExtendedMode=FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_MemoryDataWidth=FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStruct.FSMC_MemoryType=FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignal=FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalActive=FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStruct.FSMC_WaitSignalPolarity=FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStruct.FSMC_WrapMode=FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStruct.FSMC_WriteOperation=FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStruct.FSMC_WriteBurst=FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStruct.FSMC_ReadWriteTimingStruct=&FSMC_NORSRAMTimingInitStruct;	
	FSMC_NORSRAMInitStruct.FSMC_WriteTimingStruct=&FSMC_NORSRAMTimingInitStruct;
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStruct);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

/*=======================================================================================*/
/*										DMA1 Channel1 Init																													 */
/*=======================================================================================*/
			
	DMA_InitStruct.DMA_BufferSize=1;
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M=DMA_M2M_Enable;
	DMA_InitStruct.DMA_MemoryBaseAddr=Bank1_LCD_Data;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
	//DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)_accortex;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Enable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	//DMA_Cmd(DMA1_Channel1, ENABLE);
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);

}


void Init_ssd1963(void){
		
	LCD_Reset();
	// Set up the Phase Lock Loop circuits. This only has to be done once.
	LcdWriteReg(CMD_SET_PLL_MN); // PLL multiplier, set PLL clock to 100MHz
	LcdWriteData(0x001D); // M = 29 
	LcdWriteData(0x0002); // N = 2
	LcdWriteData(0x0004); // dummy value, can be anything
	
	LcdWriteReg(CMD_SET_PLL);  	// PLL enable
	LcdWriteData(0x0001);  // Use PLL output as system clock
	// Wait 200us, allows the PLL to stabilize */
	GUI_Delay(1);

	LcdWriteReg(CMD_SET_PLL);
	LcdWriteData(0x0003); // SSD1963 is switched to PLL output after PLL has stabilized.
	
	/*this is 500 us, allows the PLL to stabilize */
	GUI_Delay(1);
	LcdWriteReg(CMD_SOFT_RESET); // software reset, see SSD1963 manual p.20 Command Table.
	GUI_Delay(10);	

	LcdWriteReg(CMD_SET_PIXCLK_FREQ);	//PLL setting for PCLK, depends on LCD resolution
	// For the 4.3" LCD			//		532*288*60=9192960 Hz  LCDC_FPR=99929
	LcdWriteData(0x0001);
	LcdWriteData(0x0086);
	LcdWriteData(0x0059);


	LcdWriteReg(CMD_SET_LCD_MODE);	// LCD SPECIFICATION
	// We have to write 7 parameter values. Various bit values are
	// set. See manual p.43.
	LcdWriteData(0x0020);  // 24 bit, FRC dithering off, TFT dithering off, other stuff
	LcdWriteData(0x0000);  // LFRAME signal polarities
  LcdWriteData(0x0001);  // 2 entries for Set Horizontal width
	LcdWriteData(0x00DF);
  LcdWriteData(0x0001);  // 2 entries for Set vertical width
	LcdWriteData(0x000F);
  LcdWriteData(0x0000); // Even line RGB sequence 000 == RGB

	LcdWriteReg(CMD_SET_HORIZ_PERIOD);	//HSYNC
	LcdWriteData(((HT-1)>>8)&0x00ff);  //Set HT= HT(532) -1=531 0x0213
	LcdWriteData((HT-1)&0x00ff);
	LcdWriteData(((HS+HBP)>>8)&0x00ff);  //Set HTS= HS+HBP=2+42=42  0x2D
	LcdWriteData((HS+HBP)&0x00ff);
	LcdWriteData((HS-1)&0x00ff);		 //Set HTW = HPW-1=1
	LcdWriteData(0x0000);  //Set LPS=0000
	LcdWriteData(0x0000);
	LcdWriteData(0x0000);	// LPSPP. Dummy byte for TFT interface

	LcdWriteReg(CMD_SET_VERT_PERIOD); //VSYNC
	LcdWriteData(((VT-1)>>8)&0x00ff);   //Set VT=VP-1=287 
	LcdWriteData((VT-1)&0x00ff);
	LcdWriteData(((VS+VBP)>>8)&0x00ff);  //Set VPS= VPW+VBP=10+12=22 
	LcdWriteData((VS+VBP)&0x00ff);
	LcdWriteData((VS-1)&0x00ff);  //Set VPW=VPW-1
	LcdWriteData(0X0000);  //Set FPS
	LcdWriteData(0X0000);
	//PWM
	LcdWriteReg(CMD_SET_PWM_CONF); //set PWM for Backlight. Manual p.53
	// 6 parameters to be set
	LcdWriteData(0x0004); // PWM Freq =100MHz/(256*(PWMF[7:0]+1))/256  PWMF[7:0]=4 PWM Freq=305Hz
	LcdWriteData(0x0000); // PWM duty cycle(6%)
	LcdWriteData(0x0001); // PWM controlled by host, PWM enabled
	LcdWriteData(0x00f0); // brightness level 0x00 - 0xFF
	LcdWriteData(0x0000); // minimum brightness level =  0x00 - 0xFF
	LcdWriteData(0x0000); // brightness prescalar 0x0 - 0xF
	
	LcdWriteReg(CMD_SET_COL_ADDRESS);
	LcdWriteData(0x0000);
	LcdWriteData(0x0000);
	LcdWriteData(0x0001);
	LcdWriteData(0x00DF);
	
	LcdWriteReg(CMD_SET_PAGE_ADDRESS);
	LcdWriteData(0x0000);
	LcdWriteData(0x0000);
	LcdWriteData(0x0001);
	LcdWriteData(0x000F);
	
	LcdWriteReg(CMD_SET_ADDRESS_MODE); //rotation, see p.18
	LcdWriteData(0x00C0);							//rotation mirror_x || mirror_y 	
	
/*----------------------------------------------------------------------------------*/	
	LcdWriteReg(CMD_SET_PIXDATA_IF_FMT); //pixel data interface 16bit (565 format)
	LcdWriteData(0x0003);
	
	LcdWriteReg(CMD_SET_DISPLAY_ON); //display on
	
	LcdWriteReg(CMD_WRITE_MEM_START);

}



/*void LCD_BacklightOn(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
}



void LCD_BacklightOff(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
}
*/


/*****END OF FILE****/
