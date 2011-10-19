/**************** Q-SYS *******************
 * PageName : TestPage
 * Author : YourName
 * Version : 
 * Base Q-Sys Version :
 * Description :
 * WebSite : Www.Q-ShareWe.Com
 ***************** Q-SYS *******************/
 
#include "User.h"
#include "Theme.h"
#include "TestPage.h"

//��������
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam);
static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam);
static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo);

//-----------------------��ҳϵͳ����������-----------------------
//����ҳ�水����Ҫ�õ���ö�٣�����������Ψһ�ĺ궨��
typedef enum
{
	ExtiKeyDown=EXTI_KEY_VALUE_START,//ϵͳĬ�Ͻ��ⲿ�жϰ������͵���һ����ֵ
	ExtiKeyUp,
	ExtiKeyEnter,
	
	//�����Ӳ���汾��֧�ָ����ⲿ�жϰ����������߼��̣�
	//���Դ�����ֵ��USER_KEY_VALUE_START��ʼ����ǰ��ļ�ֵ��������
	BackKV=USER_KEY_VALUE_START,
	LeftArrowKV,
	DotKV,
	RightArrowKV,
	DoneKV,

	HomeKV,
	MessageKV,
	MusicKV,
	PepoleKV,
}TestPage_KEY_NAME;

//����ҳ���Ӧ�õĴ������򼯣��൱�ڶ��尴��
//֧�ֵ�������������ΪMAX_TOUCH_REGION_NUM
//ϵͳ��ʾ�ʹ�������������ϵ������Ļ���Ͻ�Ϊԭ��(x 0,y 0)�����½�Ϊ(x 320,y 240)
static const IMG_TCH_OBJ ImgTchRegCon[]={
	//KeyName,ObjID,OptionMask,Tch_x,Tch_y,Tch_w,Tch_h,Img_x,Img_y,BmpPathPrefix,NormalSuf,PressSuf,ReleaseSuf,TransColor},
	{"Back",	BackKV,RelMsk|PathMsk,3,287,54,31,0,0,"Common/Btn/Back",FatColor(NO_TRANS)},
	{"<<",		LeftArrowKV,RelMsk|PathMsk,65,287,39,31,0,0,"Common/Btn/LeftArr",FatColor(NO_TRANS)},
	//{"Dot",		DotKV,RelMsk|PathMsk,109,292,22,22,0,0,"Common/Btn/Dot",FatColor(NO_TRANS)},
	{">>",		RightArrowKV,RelMsk|PathMsk,136,287,39,31,0,0,"Common/Btn/RightArr",FatColor(NO_TRANS)},
	{"Done",	DoneKV,RelMsk|PathMsk,183,287,54,31,0,0,"Common/Btn/Done",FatColor(NO_TRANS)},
	
	//Һ�����������ʾ������ĸ���
	{"",HomeKV,RelMsk,0,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MessageKV,RelMsk,60,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MusicKV,RelMsk,120,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",PepoleKV,RelMsk,180,320,60,30,0,0,"",FatColor(NO_TRANS)},
};

static const CHAR_TCH_OBJ CharTchRegCon[]={
	//KeyName,ObjID,OptionMask,Tch_x,Tch_y,Tch_w,Tch_h,
		//Char_x,Char_y,MarginXY,SpaceXY,NormalColor,NormalBG,PressColor,PressBG,ReleaseColor,ReleaseBG},
	{"���ֺ�",21,RoueMsk|PrsMsk|CotMsk|RelMsk|ReVMsk,18,83,50,19,
		7,4,0x22,0x00,FatColor(0xffffff),FatColor(0x666666),FatColor(0xffffff),FatColor(0x333333),FatColor(0xffffff),FatColor(0x666666)},
	{"������",22,RoueMsk|PrsMsk|CotMsk|RelMsk|ReVMsk,69,83,50,19,
		7,4,0x22,0x00,FatColor(0xffffff),FatColor(0x666666),FatColor(0xffffff),FatColor(0x333333),FatColor(0xffffff),FatColor(0x666666)},
	{"ͼƬ",23,RoueMsk|PrsMsk|CotMsk|RelMsk|ReVMsk,120,83,50,19,
		14,4,0x82,0x00,FatColor(0xffffff),FatColor(0x666666),FatColor(0xffffff),FatColor(0x333333),FatColor(0xffffff),FatColor(0x666666)},
	{"����",24,RoueMsk|PrsMsk|CotMsk|RelMsk|ReVMsk,171,83,50,19,
		13,4,0x82,0x00,FatColor(0xffffff),FatColor(0x666666),FatColor(0xffffff),FatColor(0x333333),FatColor(0xffffff),FatColor(0x666666)},
};

//����ҳ�����Ӧ�õ����Լ�
const PAGE_ATTRIBUTE TestPage={
	"TestPage",//page name
	"Author",//page author name
	"Page Description",//page description
	NORMAL_PAGE,//page type
	0,//page function mask bit.

	{//control object num records.
		sizeof(ImgTchRegCon)/sizeof(IMG_TCH_OBJ), //number of image touch control object
		0,//sizeof(CharTchRegCon)/sizeof(CHAR_TCH_OBJ), //number of char touch control object,
		0,//number of dynamic image touch control object
		0,//number of dynamic char touch control object
#ifdef QSYS_FRAME_FULL	
		0,//number of yes no control object
		0,//number of num box control object
		0,//number of string option control object
		0,//number of string input frame control object
#endif
	},
	ImgTchRegCon, //image touch ctrl obj
	CharTchRegCon,//char touch ctrl obj
	
	SystemEventHandler,//handler of system event
	PeripheralsHandler,//handler of Peripherals event
	Bit(Perip_KeyPress)|Bit(Perip_KeyRelease)|Bit(Perip_UartInput),//mask bits of peripherals event
	TouchEventHandler,//handler of all touch control object
#ifdef QSYS_FRAME_FULL	
	NULL,//handler of yes no control object
	NULL,//handler of num box control object
	NULL,//handler of string option control object
	NULL,//handler of string input control object
#endif
};

//-----------------------��ҳ�Զ����������-----------------------
static bool Tip=FALSE;

//-----------------------��ҳ�Զ��庯��-----------------------

//-----------------------��ҳϵͳ����----------------------

//����ĳЩ�¼�ʱ���ᴥ���ĺ���
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam)
{
	GUI_REGION DrawRegion;
	
	switch(SysEvent)
	{
		case Sys_PreGotoPage:
			break;
		case Sys_PageInit:		//ϵͳÿ�δ����ҳ�棬�ᴦ������¼�				
		case Sys_SubPageReturn:	//�������ҳ�淵��,�Ͳ��ᴥ��Sys_Page_Init�¼�,����Sys_SubPage_Return

			Tip=FALSE;

			//��������
			DrawRegion.x=DrawRegion.y=0;
			DrawRegion.w=240;
			DrawRegion.h=21;
			DrawRegion.Color=FatColor(NO_TRANS);
			Gui_FillImgArray((u8 *)gImage_StatusBar1,1,21,&DrawRegion);	
			DrawTitle1(ASC14B_FONT,"TestPage",(240-strlen("TestPage")*GUI_ASC14B_ASCII_WIDTH)>>1,strlen("TestPage"),FatColor(0xe0e0e0));//д����
			
			//������
			DrawRegion.x=0;
			DrawRegion.y=21;
			DrawRegion.w=240;
			DrawRegion.h=320-21-39;
			DrawRegion.Color=FatColor(0x8b8a8a);
			Gui_FillBlock(&DrawRegion);

			//������
			DrawRegion.x=0;
			DrawRegion.y=320-39;
			DrawRegion.w=240;
			DrawRegion.h=39;
			DrawRegion.Color=FatColor(NO_TRANS);
			Gui_FillImgArray((u8 *)gImage_BottomBar1,1,39,&DrawRegion);

			//����
			DrawFrame1(25,252);	

			//����
			{GUI_REGION DrawRegTmp={109,292,22,22,0,FatColor(NO_TRANS)};
			Gui_Draw24Bmp("Theme/F/Common/Btn/DotN.bmp",&DrawRegTmp);}
			break;
		case Sys_TouchSetOk:
		case Sys_TouchSetOk_SR:

			break;
		case Sys_PageClean:
		case Sys_PreSubPage:
		
			break;
		default:
			//��Ҫ��Ӧ���¼�δ����
			Debug("%s SystemEventHandler:This System Event Handler case unfinish! SysEvent:%d\n\r",Q_GetCurrPageName(),SysEvent);
			//while(1);
	}
	
	return 0;
}

static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam)
{
	switch(PeripEvent)
	{
		case Perip_KeyPress:
			if(IntParam==ExtiKeyEnter)
			{
			
			}
			else if(IntParam==ExtiKeyUp)
			{
				Q_PresentTch(LeftArrowKV,Tch_Press);
			}
			else if(IntParam==ExtiKeyDown)
			{
				Q_PresentTch(RightArrowKV,Tch_Press);
			}
			break;
		case Perip_KeyRelease:
			if(IntParam==ExtiKeyEnter)
			{
				PrtScreen();
			}
			else if(IntParam==ExtiKeyUp)
			{
				Q_PresentTch(LeftArrowKV,Tch_Release);
			}
			else if(IntParam==ExtiKeyDown)
			{
				Q_PresentTch(RightArrowKV,Tch_Release);
			}
			break;
		case Perip_UartInput:
			if((IntParam>>16)==1)//����1
			{
				Q_Sh_CmdHandler(IntParam&0xffff,pParam);
			}
			break;
	}

	return 0;
}

//��ʹ���߰��±�ҳTouchRegionSet�ﶨ��İ���ʱ���ᴥ�����������Ķ�Ӧ�¼�
static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo)
{		
	//GUI_REGION DrawRegion;
	
	switch(Key)
	{
		
		case BackKV:
			Q_GotoPage(GotoNewPage,Q_GetPageByTrack(1)->Name,0,NULL);//����ǰһ��ҳ��
			break;
		case DoneKV:
			Q_GotoPage(GotoNewPage,Q_GetPageByTrack(1)->Name,0,NULL);//����ǰһ��ҳ��
			break;	
		case LeftArrowKV:
			break;
		case DotKV:
			if(Tip)
			{
				GUI_REGION DrawRegTmp={109,292,22,22,0,FatColor(NO_TRANS)};
				Gui_Draw24Bmp("Theme/F/Common/Btn/DotN.bmp",&DrawRegTmp);
				Tip=FALSE;
			}
			else
			{
				GUI_REGION DrawRegTmp={109,292,22,22,0,FatColor(NO_TRANS)};
				Gui_Draw24Bmp("Theme/F/Common/Btn/DotP.bmp",&DrawRegTmp);
				Tip=TRUE;
			}
			break;
		case RightArrowKV:
			break;
		case HomeKV:
		case MessageKV:
		case MusicKV:
		case PepoleKV:
			break;
		default:
			//��Ҫ��Ӧ���¼�δ����
			Debug("%s TouchEventHandler:This Touch Event Handler case unfinish! Key:%d\n\r",Q_GetCurrPageName(),Key);
			///while(1);
	}
	
	return 0;
}


