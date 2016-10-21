#include <stm32f10x.h>

#include "header.h"
#include "GUI.h"



TimeAlarm_TypeDef  Time,Alarm;

uint16_t ALARMA_ACTION;
uint8_t ALARM_INT;

WIDGET_DRAW_ITEM_FUNC *pWIDGET_DRAW_ITEM_FUNC;

uint8_t countSD_files_window;
WM_HWIN PROGBAR_MEM;
int usedbytes;
int freebytes;
uint16_t folders,files;
uint8_t count_SD_open;

void (*pGUI_Init)(void);
SD_Error (*pBoot_menu)(void);

char buffer[50];

unsigned char level_DIR[1];
TCHAR Path[128]={'0',':','\0'};
uint8_t  time_show;
uint16_t SD_SECTOR_SIZE=512;
SD_Error sd_error=SD_ERROR;
SD_CardInfo sd_cardinfo;

uint8_t sleep_mode=0;

BACKLIGHT_TypeDef backlight=BACKLIGHT_ON;
uint16_t brightness;
//uint8_t backlight=2;
uint8_t backlight_delay=0;

FATFS fs;
FRESULT fresult=FR_OK;
FIL fil;
DIR	dir;
FILINFO finfo;


GUI_PID_STATE State;

char drawmode;


/*--------------------------------------- Массив дней для високосного и не високосного года----------------------------------*/

uint8_t days [2][13]={
	{0,31,28,31,30,31,30,31,31,30,31,30,31},
	{0,31,29,31,30,31,30,31,31,30,31,30,31}
	};		
