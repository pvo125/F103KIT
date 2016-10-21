
#include "header.h"
#include "mx25l8005.h"
#include "ssd1963_lcd.h"
extern SPI_InitTypeDef						SPI_InitStruct;
extern GPIO_InitTypeDef 					GPIO_InitStruct;
extern DMA_InitTypeDef						DMA_InitStruct;

int DummyByte=0;


void MX25_LowLevel_Init(void)
{
	/* SPI output - port A */	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_5|				/* SPI1  SCK */
														GPIO_Pin_7;				/*	SPI1 MOSI */
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
/* SPI input - port A */
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;				/* SPI1  MISO */
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
/* SPI output CS SPI FLASH - port A4 */				/* SPI1 FLASH CS */
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	CS_HIGH();
	/*	SPI configuration */
	SPI_InitStruct.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStruct.SPI_CRCPolynomial=7;
	SPI_InitStruct.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode=SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS=SPI_NSS_Soft;
	SPI_Init(SPI1, &SPI_InitStruct);
	
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
	
/* Configure RX DMA1 Channel2 by SPI1 */
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr=Bank1_LCD_Data;;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&SPI1->DR;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_Low;
	DMA_Init(DMA1_Channel2,&DMA_InitStruct);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

/* Configure TX DMA1 Channel3 by SPI1 */	
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)&DummyByte;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Disable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&SPI1->DR;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_Low;
	DMA_Init(DMA1_Channel3,&DMA_InitStruct);


}


/*---------------------------------------------------------------------------------------------------*/
void WriteDisable(void){
	CS_LOW();
	SPI1->DR=WRDI;			// WRDI= 0x04
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)==SET){}	
	CS_HIGH();
}

/*---------------------------------------------------------------------------------------------------*/
void WriteEnable(void){
	CS_LOW();
	SPI1->DR=WREN;			//WREN=0x06
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY)==SET){}	
	CS_HIGH();
}

/*---------------------------------------------------------------------------------------------------*/
void WriteStatusReg_MX25L(uint8_t status){
	WriteEnable();
	CS_LOW();
	SPI1->DR=WRSR;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET)	{}
	SPI1->DR=status;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)==SET)	{}	
	CS_HIGH();
	
		/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}
	
		
		
}

/*---------------------------------------------------------------------------------------------------*/
uint8_t ReadStatusReg_MX25L(void){
	uint8_t temp;
	CS_LOW();
	SPI1->DR=RDSR;		//	RDSR= 0x05	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY)==SET)	{}
	temp=SPI1->DR;			// Clear RXNE 
	SPI1->DR=0;
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET)	{}	
	temp=SPI1->DR;	
	CS_HIGH();
	return temp;
	
}

/*---------------------------------------------------------------------------------------------------*/
uint32_t ReadID_MX25L (void){
	uint32_t tmp=0;	
	uint8_t i;
	CS_LOW();
		SPI1->DR=RDID;
		while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
		i=SPI1->DR;			// Clear RXNE
	for (i=0;i<3;i++)
			{	
			SPI1->DR=0;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}
			tmp|=SPI1->DR<<(16-8*i);
			}
	CS_HIGH();		
			return tmp;
}

/*---------------------------------------------------------------------------------------------------*/
void Read_MX25L_FSMC(uint32_t address,uint32_t words,uint16_t *destination){
	int8_t i;
	uint32_t m;
	CS_LOW();
	SPI1->DR=READ;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
		SPI1->CR1&=~SPI_CR1_SPE;
		SPI1->CR1|=SPI_CR1_DFF;
		SPI1->CR1|=SPI_CR1_SPE;
	m=SPI1->DR;			//  Clear RXNE	
	for(m=0;m<words;m++)
			{
			SPI1->DR=0;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}	
			destination[0]=SPI1->DR;
			}
		SPI1->CR1&=~SPI_CR1_SPE;
		SPI1->CR1&=~SPI_CR1_DFF;
		SPI1->CR1|=SPI_CR1_SPE;
	CS_HIGH();		
}

/*---------------------------------------------------------------------------------------------------*/
void Read_MX25L(uint32_t address,uint32_t bytes,uint8_t *destination){
	int8_t i;
	uint32_t m;
	CS_LOW();
	SPI1->DR=READ;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
	i=SPI1->DR;			//  Clear RXNE	
	for(m=0;m<bytes;m++)
			{
			SPI1->DR=0;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}
			destination[m]=SPI1->DR;
			}
	CS_HIGH();		
}
/*---------------------------------------------------------------------------------------------------*/
void FastRead_MX25L(uint32_t address,uint16_t bytes,uint8_t *destination){
	int8_t i;
	uint16_t m;
	CS_LOW();
	SPI1->DR=FastRead;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	SPI1->DR=0;
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
	i=SPI1->DR;			//  Clear RXNE	
	for(m=0;m<bytes;m++)
			{
			SPI1->DR=DummyByte;
			while((SPI1->SR&SPI_SR_RXNE)!=SPI_SR_RXNE)	{}
			destination[m]=SPI1->DR;
			}
	CS_HIGH();		
}

/*---------------------------------------------------------------------------------------------------*/
void SectorErase_MX25L(uint32_t address){
	int8_t i;	
	WriteEnable();
	CS_LOW();
	SPI1->DR=SE;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
		{
		SPI1->DR=address>>(i*8);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
		}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}
/*---------------------------------------------------------------------------------------------------*/
void BlockErase_MX25L(uint32_t address){
	int8_t i;	
	WriteEnable();
	CS_LOW();
	SPI1->DR=BE;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}

/*---------------------------------------------------------------------------------------------------*/
void ChipErase_MX25L(void){
	WriteEnable();
	CS_LOW();
	SPI1->DR=CE;
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}
/*---------------------------------------------------------------------------------------------------*/
void PagePrg_MX25L(uint32_t address,uint8_t *SourceBuffer){
	int8_t i;
	uint16_t m;
	WriteEnable();
	CS_LOW();
	SPI1->DR=PP;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
		{
		SPI1->DR=address>>(i*8);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
		}
	for (m=0;m<256;m++)
		{
		SPI1->DR=SourceBuffer[m];
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
		}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	CS_HIGH();
	/*Читаем Status Registr ждем сброса бита WIP*/	
	while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}		
		
}

/*---------------------------------------------------------------------------------------------------*/
void Prg_MX25L_8(uint32_t address, uint32_t bytes,const uint8_t *SourceBuffer){
	int8_t i;	
	uint16_t temp,k,m;
	uint32_t j=0;
	
	temp=bytes/256;
	//WriteEnable();
	//CS_LOW();
		for (k=0;k<=temp;k++)
				{
				WriteEnable();
				CS_LOW();
				SPI1->DR=PP;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
				for (i=2;i>=0;i--)
					{
					SPI1->DR=address>>(i*8);
					while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
					}
				if(k<temp)
				{
					for (m=0;m<256;m++)
							{
							SPI1->DR=SourceBuffer[j];
							j++;	
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}
							}
				}
				else
				{
					for (m=0;m<(bytes%256);m++)
							{
							SPI1->DR=SourceBuffer[j];
							j++;	
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
							}
				}				
				while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
				CS_HIGH();
				/*Читаем Status Registr ждем сброса бита WIP*/	
				while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}
				address+=256;
				}
}


/*---------------------------------------------------------------------------------------------------*/
void Prg_MX25L_16(uint32_t address, uint32_t bytes,const unsigned short *SourceBuffer){
	int8_t i;	
	uint16_t temp,k,m;
	uint32_t j=0;
	
	temp=bytes/128;
		for (k=0;k<=temp;k++)
				{
				WriteEnable();
				CS_LOW();
				SPI1->DR=PP;
				while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
				for (i=2;i>=0;i--)
					{
					SPI1->DR=address>>(i*8);
					while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
					}
				if(k<temp)
				{
					for (m=0;m<128;m++)
							{
							SPI1->DR=SourceBuffer[j]>>8;
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}
							SPI1->DR=SourceBuffer[j];			
							j++;	
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}
							}
				}
				else
				{
					for (m=0;m<(bytes%128);m++)
							{
							SPI1->DR=SourceBuffer[j]>>8;
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}
							SPI1->DR=SourceBuffer[j];	
							j++;	
							while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
							}
				}				
				while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
				CS_HIGH();
				/*Читаем Status Registr ждем сброса бита WIP*/	
				while((ReadStatusReg_MX25L()&((uint8_t)0x01))==SET)	{}
				address+=256;
				}
}
/*--------------------------------------------------------------------------------------------------*/
void Read_MX25L_DMA(uint32_t address,uint16_t bytes){
	int8_t i;
	CS_LOW();
	SPI1->DR=READ;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}	
	for (i=2;i>=0;i--)
			{
			SPI1->DR=address>>(i*8);
			while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET){}		
			}
	while((SPI1->SR&SPI_SR_BSY)==SPI_SR_BSY)	{}
	/*после отправки команды и адреса в приемнике мусор очищаем RXNE перед циклом приема полезных байт*/			
	i=SPI1->DR;			//  Clear RXNE	
	DMA1_Channel2->CNDTR=bytes;
	DMA1_Channel3->CNDTR=bytes;	
	DMA_Cmd(DMA1_Channel2,ENABLE);
	DMA_Cmd(DMA1_Channel3,ENABLE);	
		
}
