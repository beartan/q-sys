/**************** Q-SYS *******************
 * PageName : MainPage
 * Author : YuanYin
 * Version : 2.0
 * Base Q-Sys Version : 2.x
 * Description : This the first page in Q-sys
 * WebSite : Www.Q-ShareWe.Com
 ***************** Q-SYS *******************/
 
#include "User.h"
#include "MusicHandler.h"
#include "MainPage.h"
#include "FileListPage.h"

static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam);
static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam);
static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo);

//���廥���ź�����Ϊϵͳ��ҳ���Ӧ��֮��Э��
enum
{
	ExtiKey0=EXTI_KEY_VALUE_START,//ϵͳĬ�Ͻ��ⲿ�жϰ������͵���һ����ֵ
	ExtiKey1,
	ExtiKey2,

	//�����Ӳ���汾��֧�ָ����ⲿ�жϰ����������߼��̣�
	//���Դ�����ֵ��0x40��ʼ����ǰ��ļ�ֵ��������	
	MusicKV=USER_KEY_VALUE_START,
	EBookKV,
	PictureKV,
	AppListKV,
	AdcKV,
	CalculatorKV,
	PrevKV,
	NextKV,
	
	HomeKV,
	MailKV,
	RingKV,
	CallKV,
};

//����ҳ���Ӧ�õĴ������򼯣��൱�ڶ��尴��
static const IMG_TCH_OBJ ImgTchRegCon[]={
	//{key,gLandScapeMode,x,y,width,hight,image x,image y,normal bmp path,release bmp path,press bmp path,transparent color,key name}
	{"<",				PrevKV,		RelMsk,0,33,17,70,0,25,"Left",FatColor(NO_TRANS)},
	{"Music",		MusicKV,	RelMsk,18,33,50,70,0,0,"Music",FatColor(0xff0000)},
	{"EBook",		EBookKV,	RelMsk,69,33,50,70,0,0,"EBook",FatColor(0xff0000)},
	{"Picture",	PictureKV,	RelMsk,120,33,50,70,0,0,"Picture",FatColor(0xff0000)},
	{"AppList",	AppListKV,	RelMsk,171,33,50,70,0,0,"AppList",FatColor(0xff0000)},
	{">",				NextKV,	RelMsk,222,33,18,70,0,25,"Right",FatColor(NO_TRANS)},
	
	//Һ�����������ʾ������ĸ���
	{"",HomeKV,RelMsk,0,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MailKV,RelMsk,60,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",RingKV,RelMsk,120,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",CallKV,RelMsk,180,320,60,30,0,0,"",FatColor(NO_TRANS)},
};

//����ҳ�����Ӧ�õ����Լ�
const PAGE_ATTRIBUTE MainPage={
	"MainPage", //name of page or app
	"YuanYin",
	"Main page",
	NORMAL_PAGE, //type,must be page or app
	0,

	{
		sizeof(ImgTchRegCon)/sizeof(IMG_TCH_OBJ), //size of touch region array
		0,//sizeof(CharTchRegCon)/sizeof(CHAR_TCH_OBJ), //size of touch region array,
		0,0,0,1
	},
	
	ImgTchRegCon, //touch region array
	NULL,//CharTchRegCon,
	
	SystemEventHandler, //init page or app function  
	PeripheralsHandler,
	Bit(Perip_RtcMin)|Bit(Perip_LcdOff)|Bit(Perip_LcdOn)|Bit(Perip_UartInput)|
	Bit(Perip_Timer)|Bit(Perip_RtcAlarm)|Bit(Perip_KeyPress)|Bit(Perip_KeyRelease),
	TouchEventHandler, //touch input event handler function
};

static const char Week[][4]={"һ","��","��","��","��","��","��"};
static void DispTime(void)
{
	RTC_TIME Time;
	char TimeMsg[64];
	GUI_REGION DrawRegion;

	if(Gui_GetBgLightVal()==0) return;//����û�����������

	RTC_GetTime(&Time);
	sprintf(TimeMsg,"%02d��%02d�� ��%s",Time.mon,Time.day,Week[Time.week]);

	DrawRegion.x=0;
	DrawRegion.y=0;
	DrawRegion.w=240;
	DrawRegion.h=18;
	DrawRegion.Color=FatColor(NO_TRANS);
	Gui_Draw24Bmp("Theme/F/MainPage/Bg/A01.Bmp",&DrawRegion);
	
	DrawRegion.x=4;
	DrawRegion.y=4;
	DrawRegion.w=120;
	DrawRegion.h=16;
	DrawRegion.Space=0x11;
  	DrawRegion.Color=FatColor(0xf2f2f2);
	Gui_DrawFont(GBK12_FONT,(void *)TimeMsg,&DrawRegion);

	DrawRegion.x=200;
	DrawRegion.Space=0x00;
	sprintf(TimeMsg,"%02d:%02d",Time.hour,Time.min);
	Gui_DrawFont(GBK12_FONT,(void *)TimeMsg,&DrawRegion);
}

//��ÿ�α�ҳ����ϵͳ��ǰҳ��ʱ����õĳ���
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent,int IntParam,void *pSysParam)
{
	GUI_REGION DrawRegion;
	int Time;
	Debug("## MainPage %d\n\r",SysEvent);

	switch(SysEvent)
	{
		case Sys_PreGotoPage:
			break;
		case Sys_PageInit:
		case Sys_SubPageReturn:
			if(Q_GetPageEntries()==1)  //ҳ���״δ���ʱ��Ҫ��������
			{

			}
			Time=OS_GetCurrentTick();
			DrawRegion.x=DrawRegion.y=0;
			DrawRegion.w=240;
			DrawRegion.h=320;	
			DrawRegion.Color=FatColor(0xffffff);
      		Gui_FillBlock(&DrawRegion);	
			DrawRegion.Color=FatColor(NO_TRANS);
			Gui_DrawImgBin("Theme/F/MainPage/Bg/BG2.bin",&DrawRegion);		

			DrawRegion.x=25;
			DrawRegion.y=119+5;
			DrawRegion.w=189;
			DrawRegion.h=105;			
			DrawRegion.Space=0x19;
			Gui_DrawFont(GBK12_FONT,"��ϵͳ V2.0\n����һ�����ڿ�ѧ�濪�����ϵͳ��ܣ������������Ӧ�ã����뿪Դ��������Ϣ��������ٷ���վ!",&DrawRegion);
			
			Debug("Display Bmp Time:%d\n\r",(OS_GetCurrentTick()-Time)<<1);
			DispTime();
			break;
		case Sys_TouchSetOk:
			break;
		case Sys_TouchSetOk_SR:		
			DispTime();
			break;
		default:
			Debug("You should not here! %d\n\r",SysEvent);
			//while(1);
	}
	
	return 0;
}

			//NUM_BOX_OBJ NumBoxObj;
			NUM_LIST_OBJ NumBoxObj;
			//NUM_ENUM_OBJ NumBoxObj;
			
static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam)
{
	switch(PeripEvent)
	{
		case Perip_KeyPress:
			switch(IntParam){
				case ExtiKey0:
					Debug("Key%d,%d\n\r",ExtiKey0,Perip_KeyPress);
					Q_PresentTch(NextKV,Tch_Press);
					break;
				case ExtiKey1:
					Debug("Key%d,%d\n\r",ExtiKey1,Perip_KeyPress);
					Q_PresentTch(PrevKV,Tch_Press);
					break;
				case ExtiKey2:
					Debug("Key%d,%d\n\r",ExtiKey2,Perip_KeyPress);

					break; 
			}break;
		case Perip_KeyRelease:
			switch(IntParam){
				case ExtiKey0:
					Debug("Key%d,%d\n\r",ExtiKey0,Perip_KeyRelease);
					Q_PresentTch(NextKV,Tch_Release);
					break;
				case ExtiKey1:
					Debug("Key%d,%d\n\r",ExtiKey1,Perip_KeyRelease);
					Q_PresentTch(PrevKV,Tch_Release);
					break;
				case ExtiKey2:
					Debug("Key%d,%d\n\r",ExtiKey2,Perip_KeyRelease);
					Q_GotoPage(GotoNewPage,"AppListPage",0,NULL);
					break; 
			}break;
		case Perip_Timer:			
			break;
		case Perip_RtcMin:		
			DispTime();
			break;
		case Perip_RtcAlarm:	
			Debug("Alarm!!!\n\r");
			break;
		case Perip_LcdOff:
			Debug("LCD bg light disable\n\r");
			break;
		case Perip_LcdOn:
			Debug("LCD bg light enable\n\r");
			DispTime();
			break;
		case Perip_UartInput:
			if((IntParam>>16)==1)//����1
			{
				Q_Sh_CmdHandler(IntParam&0xffff,pParam);
			}
			if((IntParam>>16)==3)//����3
			{
				IntParam&=0xffff;

				if(IntParam) Debug("###----Uart3----\n\r%s\n\r###----Uart3----\n\r",(u8 *)pParam);
			}
			break;
	}

	return 0;
}

static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo)
{
	MUSIC_EVENT MusicEvent;

	//GUI_REGION DrawRegion;
	
	//u32 cpu_sr;
	//u8 buf[]="12345\n\r";
	//FILELIST_SET FileListPageParam;

	switch(Key)
	{
		case NextKV:		
			break;
		case PrevKV:
			break;
		case MusicKV:
			Q_GotoPage(GotoNewPage,"MusicPage",0,NULL);
			break;
		case EBookKV:
			Q_GotoPage(GotoNewPage,"EBookPage",0,NULL);
			break;
		case PictureKV:
			Q_GotoPage(GotoNewPage,"PicturePage",0,NULL);
			break;
		case AppListKV:
			Debug("go to app list\n\r");
			Q_GotoPage(GotoNewPage,"AppListPage",0,NULL);
			break;
		case AdcKV:
			Q_GotoPage(GotoNewPage,"AdcPage",0,NULL);
			break;
		case CalculatorKV:
			Q_GotoPage(GotoNewPage,"CalculatorPage",0,NULL);
			break;
 		case HomeKV:
			NumBoxObj.ObjID=100;
			NumBoxObj.Type=NBT_NumList;
			NumBoxObj.x=100;
			NumBoxObj.y=200;
			NumBoxObj.w=100;
			NumBoxObj.Value=50;
			NumBoxObj.Min=0;
			NumBoxObj.Max=100;
			NumBoxObj.Step=10;
			Q_SetNumList(1,&NumBoxObj);
			//Q_GotoPage(GotoSubPage,"NumBoxPage",0,&NumBoxObj);
			
 			//Gui_SetBgLight(0);
 			break;
		case MailKV:
			Gui_SetBgLight(50);
			break;
		case RingKV:
#if 1//debug by karlno
	Key=0;
	Q_SpiFlashSync(FlashRead,0,4,(void *)&Key);
	Debug("buf %d\n\r",Key);
#endif	
			break;
		case CallKV:
		#if 1//debug by karlno
	MusicEvent.Cmd=MusicPlay;
	MusicEvent.pFilePath="Music/3.mp3";
	MusicEvent.ParamU16=0xff;
	//MusicEvent.pParam=NULL;
	Q_MusicSync(&MusicEvent);
	OS_TaskDelayMs(100);
#endif	
			break;
		default:
			Debug("You should not here!Key:%d\n\r",Key);
			//while(1);
	}
	
	return 0;
}




