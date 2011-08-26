/**************** Q-SYS *******************
 * PageName : QWebPage
 * Author : YourName
 * Version : 
 * Base Q-Sys Version :
 * Description :
 * WebSite : Www.Q-ShareWe.Com
 ***************** Q-SYS *******************/
 
#include "User.h"
#include "Theme.h"
#include "QWebPage.h"

//��������
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam);
static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam);
static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo);
static TCH_MSG YesNoHandler(u8 ObjID,bool NowValue);

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
	ListKV,

	HomeKV,
	MessageKV,
	MusicKV,
	PepoleKV,
}QWebPage_KEY_NAME;

//��ҳ�궨��
#define DEVICE_INFO_MAX_CLIENT_RECORD 7//����¼20��client
#define DEVICE_INFO_MAX_LIST_NUM 7//һҳ�����ʾ7����Ϣ
#define DEVICE_NAME_MAX_LEN	16//���ֳ���
#define DEVICE_INFO_START_X 12
#define DEVICE_INFO_START_Y 78
#define DEVICE_INFO_W 216
#define DEVICE_INFO_H 20
#define DEVICE_INFO_LIST_SPACE 6
#define DEVICE_INFO_LIST_BG FatColor(0xcccccc)
#define DEVICE_INFO_BG_COLOR FatColor(0xf0f0f0)
#define DEVICE_INFO_FONT_COLOR FatColor(0x000000)
#define DEVICE_INFO_HL_BG_COLOR FatColor(0xf0f0f0)
#define DEVICE_INFO_HL_FONT_COLOR FatColor(0xff0000)
#define DEVICE_INFO_PRS_BG_COLOR FatColor(0xff0000)
#define DEVICE_INFO_PRS_FONT_COLOR FatColor(0xffffff)

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

	//device list region
	{"", ListKV,PrsMsk|RelMsk,DEVICE_INFO_START_X,DEVICE_INFO_START_Y,DEVICE_INFO_W,DEVICE_INFO_MAX_LIST_NUM*(DEVICE_INFO_H+DEVICE_INFO_LIST_SPACE),
			0,0,"",FatColor(NO_TRANS)},
	
	//Һ�����������ʾ������ĸ���
	{"",HomeKV,RelMsk,0,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MessageKV,RelMsk,60,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MusicKV,RelMsk,120,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",PepoleKV,RelMsk,180,320,60,30,0,0,"",FatColor(NO_TRANS)},
};

//����ҳ�����Ӧ�õ����Լ�
const PAGE_ATTRIBUTE QWebPage={
	"QWebPage",
	"Author",
	"Page Description",
	NORMAL_PAGE,
	0,//THREAD_PAGE��Ҫ�Ķ�ջ��С��

	{
		sizeof(ImgTchRegCon)/sizeof(IMG_TCH_OBJ), //size of touch region array
		0,//sizeof(CharTchRegCon)/sizeof(CHAR_TCH_OBJ), //size of touch region array,
		0,
		0,
		1,
	},
	ImgTchRegCon, //touch region array
	NULL,
	
	SystemEventHandler,
	PeripheralsHandler,
	Bit(Perip_KeyPress)|Bit(Perip_KeyRelease)|Bit(Perip_QWebJoin)|Bit(Perip_QWebQueryName)|
	Bit(Perip_QWebRecv)|Bit(Perip_QWebSendFailed)|Bit(Perip_QWebSendOk)|Bit(Perip_Timer)|
	Bit(Perip_UartInput),
	TouchEventHandler,
	YesNoHandler,
};

//-----------------------��ҳ�Զ����������-----------------------
typedef struct{
	u8 Addr;
	u8 DispIdx;
	bool IsHiLight;
	u16 NameChk;
	u8 Name[DEVICE_NAME_MAX_LEN];
}CLIENT_RECORD;

typedef struct{
	YES_NO_OBJ YesNo;
	u8 NowDispNum;// from 0 start
	CLIENT_RECORD ClientRecord[DEVICE_INFO_MAX_CLIENT_RECORD];
}QWEB_PAGE_VAR;
static QWEB_PAGE_VAR *gpVar;

//-----------------------��ҳ�Զ��庯��-----------------------

//-----------------------��ҳϵͳ����----------------------
static void DrawState(void)//���µ�ǰq��״̬��ʾ
{
	GUI_REGION DrawRegion;
	u8 Str[32];
	
	//q������
	DrawRegion.x=10;
	DrawRegion.y=34;
	DrawRegion.w=160;
	DrawRegion.h=16;
	DrawRegion.Space=0x00;
	DrawRegion.Color=FatColor(0xe0e0e0);
	Gui_FillBlock(&DrawRegion);
	DrawRegion.Color=FatColor(0x000000);
	if(QWA_GetMyAddr()==QW_ADDR_DEF)//δ����
	{
		Gui_DrawFont(GBK16_FONT,"Q Web State",&DrawRegion);
	}
	else if(QWA_GetMyAddr()==QW_ADDR_HOST)//����
	{
		sprintf((void *)Str,"[H%d]%s",QWA_GetMyAddr(),QWA_MyQWebName(NULL));
		Gui_DrawFont(GBK16_FONT,Str,&DrawRegion);
	}
	else//�ӻ�
	{
		sprintf((void *)Str,"[S%d]%s",QWA_GetMyAddr(),QWA_MyQWebName(NULL));
		Gui_DrawFont(GBK16_FONT,Str,&DrawRegion);
	}
	
	gpVar->YesNo.DefVal=QWA_QWebState();
	gpVar->YesNo.ObjID=1;
	gpVar->YesNo.x=170;
	gpVar->YesNo.y=33;
	Q_SetYesNo(1,&gpVar->YesNo);
}


//��ʾһ����¼��Ϣ
//Idx��1��ʼ�����DEVICE_INFO_MAX_LIST_NUM
enum{
	NormalDisp=0,//��ͨ��ʾ
	PressDisp,//������ʾ
	HighLightDisp,//������ʾ
	CleanDisp,//�����ʾ
};
static bool DrawDeviceInfo(u8 Idx,u8 Addr,u8 *Name,u8 Action)
{
	GUI_REGION DrawRegion;
	u8 StrBuf[DEVICE_NAME_MAX_LEN<<1];
	
	if((Idx<1)||(Idx>DEVICE_INFO_MAX_LIST_NUM)) return FALSE;
	Idx--;
	
	//bg
	DrawRegion.x=DEVICE_INFO_START_X;
	DrawRegion.y=DEVICE_INFO_START_Y+Idx*(DEVICE_INFO_H+DEVICE_INFO_LIST_SPACE);
	DrawRegion.w=DEVICE_INFO_W;
	DrawRegion.h=DEVICE_INFO_H;
	if(Action==NormalDisp) DrawRegion.Color=DEVICE_INFO_BG_COLOR;
	else if(Action==PressDisp) DrawRegion.Color=DEVICE_INFO_PRS_BG_COLOR;
	else if(Action==HighLightDisp) DrawRegion.Color=DEVICE_INFO_HL_BG_COLOR;
	else	DrawRegion.Color=DEVICE_INFO_LIST_BG;
	Gui_FillBlock(&DrawRegion);

	//font
	if(Action!=CleanDisp)
	{
		DrawRegion.x+=4;
		DrawRegion.y+=4;
		DrawRegion.Space=0x00;
		if(Action==PressDisp) DrawRegion.Color=DEVICE_INFO_PRS_FONT_COLOR;
		else if(Action==HighLightDisp) DrawRegion.Color=DEVICE_INFO_HL_FONT_COLOR;
		else	DrawRegion.Color=DEVICE_INFO_FONT_COLOR;
		if(Name[0]==0)
			sprintf((void *)StrBuf,"[%c%d]Waitting for get name...",(Addr==QW_ADDR_HOST)?'H':'S',Addr);
		else
			sprintf((void *)StrBuf,"[%c%d]%s",(Addr==QW_ADDR_HOST)?'H':'S',Addr,Name);
		Gui_DrawFont(GBK12_FONT,StrBuf,&DrawRegion);
	}

	return TRUE;
}

//����һ����¼����ͬ����ʾ
//Addr����Ϊ0��Name����Ϊ�գ�nameΪ�գ����޸�name
static void AddOneDevice(u8 Addr,u8 *Name)
{
	u8 i;
	u8 Len=0;
	u16 NameChk;

	if(Name!=NULL) Len=strlen((void *)Name);
	if(Len>=DEVICE_NAME_MAX_LEN) Len=DEVICE_NAME_MAX_LEN-1;
	NameChk=MakeHash33(Name,Len);

	for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
	{
		//�Աȵ�ַ�����֣������ַ��ȣ�ֱ���޸����֣����������ͬ�����޸ĵ�ַ��
		if((gpVar->ClientRecord[i].Addr==Addr)||((gpVar->ClientRecord[i].Addr!=0)&&(gpVar->ClientRecord[i].NameChk==NameChk)))
		{//�޸��ֳɵ�
			gpVar->ClientRecord[i].Addr=Addr;
			
			if((Name!=NULL)&&(gpVar->ClientRecord[i].NameChk!=NameChk))//�޸�����
			{
				MemCpy(gpVar->ClientRecord[i].Name,Name,Len);
				gpVar->ClientRecord[i].Name[Len]=0;
			}

			if(gpVar->ClientRecord[i].IsHiLight==TRUE)
				DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx,Addr,gpVar->ClientRecord[i].Name,HighLightDisp);
			else
				DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx,Addr,gpVar->ClientRecord[i].Name,NormalDisp);
			break;
		}
	}

	if(i==DEVICE_INFO_MAX_CLIENT_RECORD)//not found ,add new one.
	{//���µ�
		for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
			if(gpVar->ClientRecord[i].Addr==0)
			{
				gpVar->ClientRecord[i].Addr=Addr;
				MemCpy(gpVar->ClientRecord[i].Name,Name,Len);
				gpVar->ClientRecord[i].Name[Len]=0;
				gpVar->ClientRecord[i].NameChk=MakeHash33(gpVar->ClientRecord[i].Name,Len);
				gpVar->ClientRecord[i].DispIdx=++gpVar->NowDispNum;
				DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx,Addr,gpVar->ClientRecord[i].Name,NormalDisp);
				break;
			}
	}	

	DrawState();//������״̬
}

//ɾ��һ���豸��¼
static void DeleteOneDevice(u8 Addr)
{
	u8 i;
	u8 DispIdx=0xff;
	
	for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
	{
		if(gpVar->ClientRecord[i].Addr==Addr)
		{
			DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx,Addr,NULL,CleanDisp);
			DispIdx=gpVar->ClientRecord[i].DispIdx;
			gpVar->ClientRecord[i].Addr=0;
			gpVar->ClientRecord[i].Name[0]=0;
			gpVar->NowDispNum--;
		}
	}

	for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
	{
		if((gpVar->ClientRecord[i].Addr!=0)&&(gpVar->ClientRecord[i].DispIdx>DispIdx))
		{
			DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx--,gpVar->ClientRecord[i].Addr,gpVar->ClientRecord[i].Name,CleanDisp);
			DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx,gpVar->ClientRecord[i].Addr,gpVar->ClientRecord[i].Name,NormalDisp);
		}
	}
}

//����һ���豸
static void HighLightOneDevice(u8 Addr)
{
	u8 i;
	
	for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
	{
		//�Աȵ�ַ�������ַ��ȣ�����
		if(gpVar->ClientRecord[i].Addr==Addr)
		{
			gpVar->ClientRecord[i].IsHiLight=TRUE;
			DrawDeviceInfo(gpVar->ClientRecord[i].DispIdx,Addr,gpVar->ClientRecord[i].Name,HighLightDisp);
			break;
		}
	}

	//û�ҵ���˵�����µ��豸����
	if(i==DEVICE_INFO_MAX_CLIENT_RECORD)//not found ,add new one.
	{//���µ�
		AddOneDevice(Addr,NULL);
		HighLightOneDevice(Addr);
	}	

	DrawState();//������״̬
}

//����ĳЩ�¼�ʱ���ᴥ���ĺ���
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam)
{
	GUI_REGION DrawRegion;
	
	switch(SysEvent)
	{
		case Sys_PreGotoPage:
			break;
		case Sys_PageInit:		//ϵͳÿ�δ����ҳ�棬�ᴦ������¼�				
			 gpVar=Q_PageMallco(sizeof(QWEB_PAGE_VAR));
			 MemSet(gpVar,0,sizeof(QWEB_PAGE_VAR));
		case Sys_SubPageReturn:	//�������ҳ�淵��,�Ͳ��ᴥ��Sys_Page_Init�¼�,����Sys_SubPage_Return
			Q_TimSet(Q_TIM1,10000,500,TRUE);//5s poll
			//��������
			DrawRegion.x=DrawRegion.y=0;
			DrawRegion.w=240;
			DrawRegion.h=21;
			DrawRegion.Color=FatColor(NO_TRANS);
			Gui_FillImgArray((u8 *)gImage_StatusBar1,1,21,&DrawRegion);	
			DrawTitle1(ASC14B_FONT,"QWebPage",(240-strlen("QWebPage")*GUI_ASC14B_ASCII_WIDTH)>>1,strlen("QWebPage"),FatColor(0xe0e0e0));//д����
			
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

			DrawState();

			//device info list bg
			DrawRegion.x=6;
			DrawRegion.y=56;
			DrawRegion.w=228;
			DrawRegion.h=212;
			DrawRegion.Color=DEVICE_INFO_LIST_BG;
			Gui_FillBlock(&DrawRegion);

			//device list
			DrawRegion.x=76;
			DrawRegion.y=56+4;
			DrawRegion.w=100;
			DrawRegion.h=16;
			DrawRegion.Space=0x00;
			DrawRegion.Color=DEVICE_INFO_FONT_COLOR;
			Gui_DrawFont(ASC14B_FONT,"Device List",&DrawRegion);		

			if(QWA_GetMyAddr()!=QW_ADDR_DEF)//�Ѿ�����
			{
				u8 i;
				
				for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)//����������ڲ���
				{
					if(gpVar->ClientRecord[i].Addr!=0)
						AddOneDevice(gpVar->ClientRecord[i].Addr,(void *)gpVar->ClientRecord[i].Name);
				}
			}
			break;
		case Sys_TouchSetOk:
		case Sys_TouchSetOk_SR:
			
			break;
		case Sys_PageClean:
			if(QWA_QWebState()) QWA_StopQWeb();
			Q_PageFree(gpVar);
		case Sys_PreSubPage:
			Q_TimSet(Q_TIM1,0,0,FALSE);//stop poll
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
			switch(IntParam){
				case ExtiKeyEnter:
					break;
				case ExtiKeyUp:
					break;
				case ExtiKeyDown:
					break; 
			}break;
		case Perip_KeyRelease:
			switch(IntParam){
				case ExtiKeyEnter:
					//PrtScreen();
					QWA_Show();
					break;
				case ExtiKeyUp:
					break;
				case ExtiKeyDown:
					break; 
			}break;

		case Perip_Timer://��ѯ���q��
			Q_TimSet(Q_TIM1,10000,500,TRUE);//5s poll
			if(QWA_GetMyAddr()!=QW_ADDR_DEF)//�Ѿ�����
			{
				u8 i;
				
				for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)//����������ڲ���
				{
					if(gpVar->ClientRecord[i].Addr!=0)
						if(QWA_QueryOnline(gpVar->ClientRecord[i].Addr)==0)
						{Debug("@@ Delete %d\n\r",gpVar->ClientRecord[i].Addr);
							DeleteOneDevice(gpVar->ClientRecord[i].Addr);
						}
						else if(gpVar->ClientRecord[i].Name[0]==0)//û���ֵģ���ѯ����
						{
							QWA_QueryName(gpVar->ClientRecord[i].Addr);
							Q_TimSet(Q_TIM1,10000,200,TRUE);// 2s poll
							break;
						}
				}
			}
			break;
			
		case Perip_QWebJoin:
			if(QWA_GetMyAddr()==QW_ADDR_HOST)//��Ϊ�����õ��ӻ���query ack
			{
				Debug("QWeb Join [%d]%s\n\r",IntParam,pParam);
				AddOneDevice(IntParam,pParam);
			}
			else if(QWA_GetMyAddr()!=QW_ADDR_DEF)//��Ϊ�ӻ����õ�������Ϣ
			{
				u8 Addr;
				u8 MyAddr=QWA_GetMyAddr();
				
				Debug("QWeb Get Host Info [%d]%s\n\r",IntParam,pParam);
				AddOneDevice(IntParam,pParam);

				//��ѯ���ߴӻ�
				QWA_QueryNextOnline(TRUE);
				while((Addr=QWA_QueryNextOnline(FALSE))!=0)
				{
					if(Addr!=MyAddr) AddOneDevice(Addr,NULL);//��������豸
				}
			}
			break;
		case Perip_QWebQueryName:
			Debug("QWeb Query [%d]%s\n\r",IntParam,pParam);
			AddOneDevice(IntParam,pParam);
			break;
		case Perip_QWebRecv://�յ���Ϣ
			HighLightOneDevice(IntParam>>24);
			break;

		case Perip_UartInput:
			if((IntParam>>16)==1)//����1
			{
				Q_UartCmdHandler(IntParam&0xffff,pParam);
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
			Q_GotoPage(GotoNewPage,"AppListPage",0,NULL);//����ǰһ��ҳ��
			break;
		case DoneKV:
			break;	
		case LeftArrowKV:

			break;
		case ListKV:
			if(InEvent==Tch_Press)
			{
				u8 i;
				Key=((pTouchInfo->y-DEVICE_INFO_START_Y)/(DEVICE_INFO_H+DEVICE_INFO_LIST_SPACE))+1;
				for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
				{
					if(gpVar->ClientRecord[i].DispIdx==Key)
					{
						DrawDeviceInfo(Key,gpVar->ClientRecord[i].Addr,gpVar->ClientRecord[i].Name,PressDisp);
						break;
					}
				}
			}
			else if(InEvent==Tch_Release)
			{
				u8 i;
				Key=((pTouchInfo->y-DEVICE_INFO_START_Y)/(DEVICE_INFO_H+DEVICE_INFO_LIST_SPACE))+1;
				for(i=0;i<DEVICE_INFO_MAX_CLIENT_RECORD;i++)
				{
					if(gpVar->ClientRecord[i].DispIdx==Key)
					{
						gpVar->ClientRecord[i].IsHiLight=FALSE;
						DrawDeviceInfo(Key,gpVar->ClientRecord[i].Addr,gpVar->ClientRecord[i].Name,NormalDisp);
						Q_GotoPage(GotoSubPage,"ChatPage",gpVar->ClientRecord[i].Addr,gpVar->ClientRecord[i].Name);
						break;
					}
				}
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

static TCH_MSG YesNoHandler(u8 ObjID,bool NowValue)
{
	switch(ObjID)
	{
		case 1:
			if(NowValue==TRUE) QWA_StartQWeb();
			else QWA_StopQWeb();
			break;
	}
	
	return 0;
}

