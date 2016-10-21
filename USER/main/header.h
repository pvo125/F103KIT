
#ifndef _HEADER_H_
#define _HEADER_H_

#include <stm32f10x.h>

#include "GUI.h"
#include "DIALOG.h"
#include "ff.h"
#include "sdcard.h"
#include "ssd1963.h"
#include "mx25l8005.h"
#include "LCDConf.h"
#include "tsc2046.h"
//#include "SDRAM.h"

typedef struct	
{	
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t day;
	uint8_t month;
	uint8_t year;
}TimeAlarm_TypeDef;



typedef struct 
{	
	uint8_t time[8];
	uint8_t date[10];
}Alarm_Type;

extern TimeAlarm_TypeDef Time,Alarm;

#define ID_PROGBAR_MEM     (GUI_ID_USER + 0x27)

#define Msg_Err 0
#define Msg_Msg 1

#define SRAM
//#define NAND

#ifdef SRAM 
#define LCD_BASE           ((uint32_t)(0x60000000))
#define LCD_REG_ADDRESS   (*(volatile U16*)LCD_BASE)
#define LCD_DATA_ADDRESS  (*(volatile U16*)(LCD_BASE + 0x20000))
#endif
#ifdef NAND
#define LCD_BASE           ((uint32_t)(0x70000000))
#define LCD_REG_ADDRESS   (*(volatile U16*)LCD_BASE )
#define LCD_DATA_ADDRESS  (*(volatile U16*)(LCD_BASE+0x10000))
#endif

#define ID_PROGBAR_MEM     (GUI_ID_USER + 0x27)

#define NONE 			(uint16_t)0x0000
#define START_PWM (uint16_t)0x0001
#define STOP_PWM	(uint16_t)0x0002
#define START_ADC (uint16_t)0x0003
#define STOP_ADC 	(uint16_t)0x0004

#define exitt   			0x900B0000
#define photo					0x900B1214
#define screen 				0x900B2428				
#define paint  				0x900B363C				
#define next   				0x900B4850
#define prev   				0x900B5A64
#define date   				0x900B6C78
#define pwm    				0x900B7E8C
#define sd     				0x900B90A0
#define alarm  				0x900BA2B4

#define AlarmA  			0x900BB4C8
#define AlarmB				0x900BB95C
#define Alarm_d				0x900BBDF0
#define add_folder		0x900BC284
#define del_folder   	0x900BC718
	

#define fexit   0x00
#define fphoto	0x1300
#define fscreen 0x2600				
#define fpaint  0x3900				
#define fnext   0x4C00
#define fprev   0x5F00
#define fdate   0x7200
#define fpwm    0x8500
#define fsd     0x9800
#define falarm  0xAB00

#define fAlarmA  			0xBE00
#define fAlarmB				0xC300
#define fAlarm_d			0xC800
#define fadd_folder		0xCD00
#define fdel_folder   0xD200


#define BRIGHTNESS_LOW  0x0030
#define BRIGHTNESS_MED  0x0090
#define BRIGHTNESS_HIGH 0x00f0
typedef enum{
	 BACKLIGHT_OFF=0,
	 BACKLIGHT_LOW,
	 BACKLIGHT_ON
}BACKLIGHT_TypeDef;


extern WIDGET_DRAW_ITEM_FUNC *pWIDGET_DRAW_ITEM_FUNC;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial18;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontArial16;
extern uint8_t countSD_files_window;
extern SD_Error (*pBoot_menu)(void);
extern void (*pGUI_Init)(void);

extern uint16_t ALARMA_ACTION;
extern uint8_t ALARM_INT;
extern WM_HWIN PROGBAR_MEM;
extern int usedbytes;
extern int freebytes;

extern  uint16_t folders,files;
extern unsigned char level_DIR[];
extern TCHAR Path[];
extern char buffer[];
extern char drawmode;

extern uint8_t sleep_mode;
extern BACKLIGHT_TypeDef backlight;
extern uint16_t brightness;
extern uint8_t backlight_delay;
extern uint8_t  time_show;
extern  uint8_t days [2][13];

extern GUI_PID_STATE State;

extern uint8_t count_SD_open;

extern uint8_t SD_buff[];

extern SD_Error sd_error;
extern SD_CardInfo sd_cardinfo;
extern uint16_t SD_SECTOR_SIZE;
extern FATFS fs;
extern FRESULT fresult;
extern FIL fil;	
extern DIR dir;
extern FILINFO finfo;

void DateCalc(void);
void AlarmCalc(void);

extern uint8_t _FileJPG(TCHAR *Path,TCHAR *fn);

extern float BrezErr;
extern uint8_t Brez_Count;
extern int BrezPower;
extern float BrezKoeff;
extern int PhaseBrez;
extern int PhasePower;
extern int TimerONOFF;

extern WM_HWIN Message(const char *p,int flag );
WM_HWIN CreateMENU(void);
WM_HWIN CreateTIMER(void);
WM_HWIN CreatePAINT(void);
WM_HWIN CreateALARM(void);
WM_HWIN CreateTIME_DATE(void);
TREEVIEW_Handle SD_Files(void);

extern void CreateStart(void);
extern GUI_CONST_STORAGE GUI_BITMAP bmexit;
extern GUI_CONST_STORAGE GUI_BITMAP bmphoto;
extern GUI_CONST_STORAGE GUI_BITMAP bmscreen;
extern GUI_CONST_STORAGE GUI_BITMAP bmpaint;
extern GUI_CONST_STORAGE GUI_BITMAP bmnext;
extern GUI_CONST_STORAGE GUI_BITMAP bmprev;
extern GUI_CONST_STORAGE GUI_BITMAP bmdate;
extern GUI_CONST_STORAGE GUI_BITMAP bmpwm;
extern GUI_CONST_STORAGE GUI_BITMAP bmsd;
extern GUI_CONST_STORAGE GUI_BITMAP bmalarm;
extern GUI_CONST_STORAGE GUI_BITMAP bmAlarmB;
extern GUI_CONST_STORAGE GUI_BITMAP bmAlarmA;
extern GUI_CONST_STORAGE GUI_BITMAP bmAlarm_D;
extern GUI_CONST_STORAGE GUI_BITMAP bmadd_folder;
extern GUI_CONST_STORAGE GUI_BITMAP bmdel_folder;



extern GUI_CONST_STORAGE unsigned short _acscreen[];
extern GUI_CONST_STORAGE unsigned short _acpaint[];

extern void wcstrcat(TCHAR *string_1,const TCHAR *string_2);
extern TCHAR *wcstrchr(TCHAR *string,TCHAR c);
extern void Set_COL_PAGE_Address(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
extern void Read_MX25L_FSMC(uint32_t address,uint32_t words,uint16_t *destination);
extern void Init_ssd1963(void);
extern uint8_t SPIBuffer[];
#endif
