



#include "DIALOG.h"
#include "header.h"

extern const char *_hours[];/*{"00","01","02","03","04","05","06","07","08","09",
												"10","11","12","13","14","15","16","17",
												"18","19","20","21","22","23",NULL};*/
extern const char *_minute[];/*{"00","01","02","03","04","05","06","07","08","09",
												"10","11","12","13","14","15","16","17",
												"18","19","20","21","22","23","24","25",
												"26","27","28","29","30","31","32","33",
												"34","35","36","37","38","39","40","41",
												"42","43","44","45","46","47","48","49",
												"50","51","52","53","54","55","56","57","58","59",NULL};*/
													
extern const char *_days[];/*{"01","02","03","04","05","06","07","08","09","10",
											"11","12","13","14","15","16","17","18","19","20",
											"21","22","23","24","25","26","27","28","29","30","31",NULL};*/			
extern const char *_months[];/*{"January","February","March","April","May","June",
												"July","August","September","October","November","December",NULL};*/
extern const char *_years[];/*{"2000","2001","2002","2003","2004","2005","2006","2007","2008","2009","2010",
											"2011","2012","2013","2014","2015","2016","2017","2018","2019","2020","2021",
											"2022","2023","2024","2025","2026","2027","2028","2029","2030",NULL};*/
													
WM_HWIN hWin_time;
extern WM_HWIN hWin_menu;
char Time_Alarm;
extern Time_Type Time;
extern Time_Type Alarm;

uint8_t hour,minute,month,year;
uint32_t sec;
uint16_t day;										
/*********************************************************************
*
*       Defines
*
**********************************************************************/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x0F)
#define ID_DROPDOWN_0     (GUI_ID_USER + 0x10)
#define ID_LISTWHEEL_0		(GUI_ID_USER + 0x11)
#define ID_LISTWHEEL_1		(GUI_ID_USER + 0x12)
#define ID_LISTWHEEL_2		(GUI_ID_USER + 0x13)
#define ID_LISTWHEEL_3		(GUI_ID_USER + 0x14)
#define ID_LISTWHEEL_4		(GUI_ID_USER + 0x18)
#define ID_BUTTON_SAVE	   (GUI_ID_USER + 0x15)
#define ID_TEXT_TIME				(GUI_ID_USER + 0x16)
#define ID_TEXT_DATE				(GUI_ID_USER + 0x17)
												
// USER START (Optionally insert additional defines)
// USER END




/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "TIME_DATE", ID_FRAMEWIN_0 , 130, 40, 300, 200, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 10, 120, 70, 35, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, "LISTWHEEL", ID_LISTWHEEL_0, 10, 20, 30, 83, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, "LISTWHEEL", ID_LISTWHEEL_1, 45, 20, 30, 83, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, "LISTWHEEL", ID_LISTWHEEL_2, 100, 20, 30, 83, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, "LISTWHEEL", ID_LISTWHEEL_3, 135, 20, 70, 83, 0, 0x0, 0 },
	{ LISTWHEEL_CreateIndirect, "LISTWHEEL", ID_LISTWHEEL_4, 210, 20, 40, 83, 0, 0x0, 0 },
	{  BUTTON_CreateIndirect, "SAVE", ID_BUTTON_SAVE, 220, 130, 50, 30, 0, 0x0, 0 },
	{  TEXT_CreateIndirect, "TIME", ID_TEXT_TIME, 30, 5, 50, 20, 0, 0x0, 0 },
	{  TEXT_CreateIndirect, "TIME", ID_TEXT_DATE, 120, 5, 50, 20, 0, 0x0, 0 },
	// USER START (Optionally insert additional widgets)
 // USER END
};




/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*    
*/

static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id,temp,temp1;
	char leap,i;
  switch (pMsg->MsgId) {
			case WM_INIT_DIALOG:
				hItem = pMsg->hWin;
				FRAMEWIN_SetTitleHeight(hItem, 25);
				FRAMEWIN_SetFont(hItem,GUI_FONT_16_1);
				FRAMEWIN_SetText(hItem, "TIME/DATE");
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_TIME);
				TEXT_SetFont(hItem, GUI_FONT_16_1);	
				TEXT_SetText(hItem,"TIME");
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_DATE);
				TEXT_SetFont(hItem, GUI_FONT_16_1);	
				TEXT_SetText(hItem,"DATE");
			
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
				DROPDOWN_AddString(hItem, "TIME");
				DROPDOWN_AddString(hItem, "ALARM");
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
				LISTWHEEL_SetText(hItem,_hours);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
			  LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,Time.time[0]+1);
				LISTWHEEL_SetVelocity(hItem,3);
							
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
				LISTWHEEL_SetText(hItem,_minute);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
				LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,Time.time[1]+1);
				LISTWHEEL_SetVelocity(hItem,3);
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
				LISTWHEEL_SetText(hItem,_days);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
				LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,Time.date[0]);
				LISTWHEEL_SetVelocity(hItem,3);
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
				LISTWHEEL_SetText(hItem,_months);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
				LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,Time.date[1]);
				LISTWHEEL_SetVelocity(hItem,3);
				
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
				LISTWHEEL_SetText(hItem,_years);
				LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
				LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
				LISTWHEEL_SetSnapPosition(hItem,32);
				LISTWHEEL_SetPos(hItem,Time.date[2]+1);
				LISTWHEEL_SetVelocity(hItem,3);
				
				
			
			break;
			case WM_DELETE:
				hWin_time=0;
			break;
			case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);
				NCode = pMsg->Data.v;
				switch(Id) {
					case ID_DROPDOWN_0: // Notifications sent by 'Dropdown'
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
							hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
							Time_Alarm=DROPDOWN_GetSel(hItem);
							if(Time_Alarm==0)
								{
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
									LISTWHEEL_SetText(hItem,_hours);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Time.time[0]+1);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);			
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
									LISTWHEEL_SetText(hItem,_minute);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Time.time[1]+1);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
									LISTWHEEL_SetText(hItem,_days);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Time.date[0]);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
									LISTWHEEL_SetText(hItem,_months);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Time.date[1]);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
									LISTWHEEL_SetText(hItem,_years);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Time.date[2]+1);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
								}
							else
								{
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
									LISTWHEEL_SetText(hItem,_hours);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Alarm.time[0]+1);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);			
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
									LISTWHEEL_SetText(hItem,_minute);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Alarm.time[1]+1);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
									LISTWHEEL_SetText(hItem,_days);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Alarm.date[0]);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
									LISTWHEEL_SetText(hItem,_months);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Alarm.date[1]);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
									hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
									LISTWHEEL_SetText(hItem,_years);
									LISTWHEEL_SetTextAlign(hItem,GUI_TA_HCENTER);
									LISTWHEEL_SetFont(hItem,GUI_FONT_16_1);
									LISTWHEEL_SetSnapPosition(hItem,32);
									LISTWHEEL_SetPos(hItem,Alarm.date[2]+1);
									LISTWHEEL_SetVelocity(hItem,3);
									WM_Paint(hItem);
								}		
							break;
							}
					break;
					case ID_LISTWHEEL_0: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_0);
								hour=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,hour);
								WM_Paint(hItem);
							break;
							}
					break;
					case ID_LISTWHEEL_1: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_1);
								minute=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,minute);
								WM_Paint(hItem);
							break;
							}
					break;
					case ID_LISTWHEEL_2: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_2);
								day=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,day);
								WM_Paint(hItem);
							break;
							}
					break;
					case ID_LISTWHEEL_3: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_3);
								month=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,month);
								WM_Paint(hItem);
							break;
							}
					break;		
					case ID_LISTWHEEL_4: 
						switch(NCode) {
							case WM_NOTIFICATION_SEL_CHANGED:
								hItem=WM_GetDialogItem(pMsg->hWin, ID_LISTWHEEL_4);
								year=LISTWHEEL_GetPos(hItem);
								LISTWHEEL_SetSel(hItem,year);
								WM_Paint(hItem);
							break;
							}
					break;		
							
							
							
							
					case ID_BUTTON_SAVE: 
						switch(NCode) {
							case WM_NOTIFICATION_CLICKED:
										temp1=year;
										temp=0;
										if (temp1%4!=0)
											leap=0;
										else
											leap=1;
										for (i=0;i<(month+1);i++)
											temp+=days[leap][i];
										while (temp1>0)
										{
											temp+=366;
											temp1--;
												for(i=0;i<3;i++)
													if (temp1<=0)
													 break;
													else
													{
													 temp+=365;
													 temp1--;
													}
										 }
										sec=hour*3600+minute*60+(day+temp)*86400; 
										/*	if (sec==0 && day==0)
												sec=sec_;
											else if(day==0) 
												sec+=(sec_/86400)*86400;
											else if(sec==0)
												sec=(day-1)*86400+sec_%86400;
											else
												sec+=(day-1)*86400;	*/
									
									if(Time_Alarm==0)
										{
											
											RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
											RTC->CNTL = sec;			
											RTC->CNTH = sec>>16;	
											RTC->CRL &=~ RTC_CRL_CNF;																		
											while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}			// Дождемся готовности часов для записи		=hour*3600+minute*60
										}
										else
										{
											
											RTC->CRL |= RTC_CRL_CNF;																			// Установим флаг CNF для входа в режим конфигурации
											RTC->ALRL = sec;			
											BKP->DR2 = sec;
											RTC->ALRH = sec>>16;
											BKP->DR1 =sec>>16;		
											RTC->CRL &=~ RTC_CRL_CNF;																		
											while ((RTC->CRL & RTC_CRL_RTOFF)!= RTC_CRL_RTOFF){}			// Дождемся готовности часов для записи
										}
										DateCalc();
										AlarmCalc();
							break;
							}
					break;				
					}
			break;
			default:
				WM_DefaultProc(pMsg);
			break;
		}
}	
			
// USER START (Optionally insert additional public code)
// USER END
WM_HWIN CreateTIME_DATE(void) {
  
  hWin_time = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, hWin_menu, 0, 0);
  FRAMEWIN_AddCloseButton(hWin_time, FRAMEWIN_BUTTON_RIGHT,0);
	return hWin_time;
}

/*************************** End of file ****************************/

