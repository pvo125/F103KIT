#include <stm32f10x.h>

#include "header.h"
#include "tjpgd.h"

typedef struct{
	uint16_t x,y;
	FIL *fp;
	BYTE *fbuf;
	UINT wfbuf;
} IODEV;
/*****************************/
static IODEV iodev;
static BYTE jdwork[8192];
static JDEC jdec;
static FIL File;
static int Time_1;
extern volatile int Times;

UINT in_func(JDEC *jdec,BYTE *buff,UINT nbyte){
 UINT br;
	IODEV *pdev=(IODEV*)jdec->device;
	if(buff)
	{
		fresult=f_read(pdev->fp,buff,nbyte,&br);
		return br;
	}
	else
		return f_lseek(pdev->fp,f_tell(pdev->fp)+nbyte)?0:nbyte;
}

/****************************************************************/
/*																															*/
/****************************************************************/
UINT out_func(JDEC *jdec,void *bitmap,JRECT *rect){
	uint16_t i;
	uint16_t *bmp=(uint16_t*)bitmap;
	IODEV *pdev=(IODEV*)jdec->device;
		
	Set_COL_PAGE_Address(pdev->x-rect->right,pdev->y-rect->bottom,pdev->x-rect->left,pdev->y-rect->top);
	for(i=0;i<((rect->right-rect->left+1)*(rect->bottom-rect->top+1));i++)
		LCD_DATA_ADDRESS=bmp[i];
	return 1;


}

/****************************************************************/
/*																															*/
/****************************************************************/
uint8_t _FileJPG(TCHAR *Path,TCHAR *fn){
	uint8_t scale,tmp;
	JRESULT jresult;
		
	fresult=f_open(&File,Path,FA_READ);
	if(fresult)	
	{	
		f_close(&File); return 1;
	}
	iodev.fp=&File;
	jresult=jd_prepare(&jdec,in_func,jdwork,sizeof(jdwork),&iodev);
	if(jresult)
	{ 
		f_close(&File); return 2;
	}
	if(jdec.width>jdec.height)
		tmp=jdec.width/481;
	else
		tmp=jdec.height/273;
	switch(tmp){
		case 0:
		scale=0;
		tmp=1;
		break;
		case 1:
		scale=1;
		tmp=2;
		break;	
		case 2:
		scale=2;
		tmp=4;
		break;
		case 3:
		scale=2;
		tmp=4;
		break;
		case 4:
		scale=3;
		tmp=8;
		break;	
		case 5:
		scale=3;
		tmp=8;
		break;
		case 6:
		scale=3;
		tmp=8;
		break;
		case 7:
		scale=3;
		tmp=8;
		break;
		case 8:
		scale=3;
		tmp=8;
		break;
		}
	iodev.x=240+(jdec.width/(2*tmp));
	iodev.y=136+(jdec.height/(2*tmp));	
	Times=0;
	jresult=jd_decomp(&jdec,out_func,scale);
	Time_1=Times;
	if(jresult)
	{	
		f_close(&File); return 3;
	}
	GUI_SetAlpha(0x80);
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("File : ",0,257);
	GUI_SetFont(&GUI_FontArial16);
	GUI_UC_DispString(fn);
	GUI_SetFont(&GUI_Font8x16);
	GUI_DispChars(' ',2);
	GUI_DispDecMin(jdec.width);
	GUI_DispString(" x ");
	GUI_DispDecMin(jdec.height);
	GUI_DispStringAt("Time decomress (ms): ",40,0);
	GUI_DispDecMin(Time_1);
	GUI_DispString("    File size (KB): ");
	GUI_DispDecMin(File.fsize/1024);
	f_close (&File);	
	return 0;
}	

/****************************************************************/
/*																															*/
/****************************************************************/
