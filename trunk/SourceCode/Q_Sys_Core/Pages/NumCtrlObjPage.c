/**************** Q-SYS *******************
 * PageName : NumCtrlObjPage
 * Author : YourName
 * Version : 
 * Base Q-Sys Version :
 * Description :
 * WebSite : Www.Q-ShareWe.Com
 ***************** Q-SYS *******************/
 
#include "User.h"
#include "Theme.h"
#include "NumCtrlObjPage.h"
#include "CtrlObjImg.h"

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
	RightArrowKV,
	NumBoxKV,
	
	Key1KV,
	Key2KV,
	Key3KV,
	Key4KV,
	Key5KV,
	Key6KV,
	Key7KV,
	Key8KV,
	Key9KV,
	Key0KV,
	KeyMinusKv,
	KeyDotKV,
	KeyCancleKV,
	KeyOkKV,
	KeyPreKV,
	KeyNextKV,

	HomeKV,
	MessageKV,
	MusicKV,
	PepoleKV,
}NumCtrlObjPage_KEY_NAME;

//����ҳ���Ӧ�õĴ������򼯣��൱�ڶ��尴��
//֧�ֵ�������������ΪMAX_TOUCH_REGION_NUM
//ϵͳ��ʾ�ʹ�������������ϵ������Ļ���Ͻ�Ϊԭ��(x 0,y 0)�����½�Ϊ(x 320,y 240)
static const IMG_TCH_OBJ ImgTchRegCon[]={
	//KeyName,ObjID,OptionMask,Tch_x,Tch_y,Tch_w,Tch_h,Img_x,Img_y,BmpPathPrefix,NormalSuf,PressSuf,ReleaseSuf,TransColor},
	//Һ�����������ʾ������ĸ���
	{"",HomeKV,RelMsk,0,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MessageKV,RelMsk,60,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",MusicKV,RelMsk,120,320,60,30,0,0,"",FatColor(NO_TRANS)},
	{"",PepoleKV,RelMsk,180,320,60,30,0,0,"",FatColor(NO_TRANS)},
};

static const CHAR_TCH_OBJ NumTchRegCon[]={
	//KeyName,ObjID,OptionMask,Tch_x,Tch_y,Tch_w,Tch_h,
		//Char_x,Char_y,MarginXY,SpaceXY,NormalColor,NormalBG,PressColor,PressBG,ReleaseColor,ReleaseBG},
	{"1",Key1KV,B14Msk|RelMsk,4,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"2",Key2KV,B14Msk|RelMsk,28,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"3",Key3KV,B14Msk|RelMsk,52,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"4",Key4KV,B14Msk|RelMsk,76,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"5",Key5KV,B14Msk|RelMsk,100,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"-",KeyMinusKv,B14Msk|RelMsk,124,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"X",KeyCancleKV,B14Msk|RelMsk,148,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},

	{"6",Key6KV,B14Msk|RelMsk,4,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"7",Key7KV,B14Msk|RelMsk,28,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"8",Key8KV,B14Msk|RelMsk,52,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"9",Key9KV,B14Msk|RelMsk,76,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"0",Key0KV,B14Msk|RelMsk,100,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{".",KeyDotKV,B14Msk|RelMsk,124,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{"O",KeyOkKV,B14Msk|RelMsk,148,28,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
};

static const CHAR_TCH_OBJ ArrowTchRegCon[]={
	//KeyName,ObjID,OptionMask,Tch_x,Tch_y,Tch_w,Tch_h,
		//Char_x,Char_y,MarginXY,SpaceXY,NormalColor,NormalBG,PressColor,PressBG,ReleaseColor,ReleaseBG},
	{"<",Key1KV,B14Msk|RelMsk,4,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
	{">",Key2KV,B14Msk|RelMsk,28,4,24,24,
		8,5,0x22,0x00,FatColor(0x555555),FatColor(0xcccccc),FatColor(0xffffff),FatColor(0xfc9400),FatColor(0x555555),FatColor(0xcccccc)},
};

//����ҳ�����Ӧ�õ����Լ�
const PAGE_ATTRIBUTE NumCtrlObjPage={
	"NumCtrlObjPage",
	"Author",
	"Page Description",
	POP_PAGE,
	0,//

	{
		sizeof(ImgTchRegCon)/sizeof(IMG_TCH_OBJ), //size of touch region array
		0,//sizeof(NumTchRegCon)/sizeof(CHAR_TCH_OBJ), //size of touch region array,
		3,
	},
	ImgTchRegCon, //touch region array
	NumTchRegCon,
	
	SystemEventHandler,
	PeripheralsHandler,
	Bit(Perip_KeyPress)|Bit(Perip_KeyRelease)|Bit(Perip_UartInput),
	TouchEventHandler,
};

//-----------------------��ҳ�Զ����������-----------------------
//����ѡ��
#define TOUCH_REGION_MARGIN 4//margin�����ӿɵ������Χ

//��̬����ģ��
const IMG_TCH_OBJ ArrowTchTmp={"",0,PrsMsk|RelMsk|ReVMsk,0,0,CO_NUM_ARROW_W,CO_NUM_H,0,0,"",FatColor(NO_TRANS)};
const IMG_TCH_OBJ NumTchTmp={"",0,RelMsk,0,0,0,CO_NUM_H,0,0,"",FatColor(NO_TRANS)};

typedef struct{
	NUM_BOX_TYPE HandlerType;//�ؼ�����
	void *ObjHandler;//�ؼ�ʵ��ָ��
	s32 DefValue;//�ؼ�Ĭ��ֵ
	u16 LeftDispBuf[CO_NUM_ARROW_W*CO_NUM_H];//�ؼ����ͷ����
	u16 RightDispBuf[CO_NUM_ARROW_W*CO_NUM_H];//�ؼ��Ҽ�ͷ����
	IMG_TCH_OBJ LeftArrowTch;//���ͷ��̬����ʵ��
	IMG_TCH_OBJ RightArrowTch;//�Ҽ�ͷ��̬����ʵ��
	IMG_TCH_OBJ NumTch;//�м����ֿ�̬����ʵ��
	GUI_REGION NumKeySaveReg;//С���̷�Χ
}NUM_CTRL_OBJ_PAGE_VARS;
static NUM_CTRL_OBJ_PAGE_VARS *gpNbpVars;//ֻ��Ҫ����һ��ָ�룬����ȫ�ֱ�����ʹ��

//-----------------------��ҳ�Զ��庯��-----------------------
typedef enum{
	DNA_Null=0,//ά��ԭ��
	DNA_Normal,//��ͨ
	DNA_HiLight,//����
	DNA_NormalArrow,//��ͨ��ͷ
	DNA_HiLightArrow//������ͷ
}DRAW_NL_ACT;
//����Ҫ�����Num�ؼ�
static void DrawNumCtrlObj(NUM_BOX_OBJ *pNumBoxObj,DRAW_NL_ACT Left,DRAW_NL_ACT Middle,DRAW_NL_ACT Right)
{
	NUM_LIST_OBJ *pNumListObj;
	NUM_ENUM_OBJ *pNumEnumObj;
	GUI_REGION DrawRegion;
	u8 NumStr[32];

	pNumListObj=(void *)pNumBoxObj;
	pNumEnumObj=(void *)pNumBoxObj;
	
	if(pNumBoxObj->Type==NBT_NumBox)
	{
	}
	else 	if((pNumListObj->Type==NBT_NumList)||(pNumEnumObj->Type==NBT_NumEnum))
	{
		if(Left != DNA_Null)//��߲���
		{
			DrawRegion.y=pNumListObj->y;
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
		}
		if(Left == DNA_Normal)
		{
			DrawRegion.x=pNumListObj->x+CO_NUM_ARROW_W-CO_NUM_FRAME_W;
			DrawRegion.w=CO_NUM_FRAME_W;
			Gui_DrawImgArray(gCtrlObj_NumLeft,&DrawRegion);//��߿�
		}
		else if(Left == DNA_HiLight)
		{
			DrawRegion.x=pNumListObj->x+CO_NUM_ARROW_W-CO_NUM_FRAME_W;
			DrawRegion.w=CO_NUM_FRAME_W;
			Gui_DrawImgArray(gCtrlObj_NumLeftH,&DrawRegion);//��߿�
		}
		else if(Left == DNA_NormalArrow)
		{
			DrawRegion.x=pNumListObj->x;
			DrawRegion.w=CO_NUM_ARROW_W;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
			Gui_DrawImgArray(gCtrlObj_NumLeftArrow,&DrawRegion);//���ͷ�滭
		}
		else if(Left == DNA_HiLightArrow)
		{
			DrawRegion.x=pNumListObj->x;
			DrawRegion.w=CO_NUM_ARROW_W;
			Gui_DrawImgArray(gCtrlObj_NumLeftArrowH,&DrawRegion);//���ͷ�滭
		}

		if(Middle != DNA_Null)//�м�򲿷�
		{
			DrawRegion.x=pNumListObj->x+CO_NUM_ARROW_W;
			DrawRegion.y=pNumListObj->y;
			DrawRegion.w=CO_NUM_MIDDLE_W;
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
		}
		if(Middle == DNA_Normal)
			Gui_FillImgArray_H(gCtrlObj_NumMiddle,pNumListObj->w-(CO_NUM_ARROW_W<<1),&DrawRegion);	//������
		else if(Middle == DNA_HiLight)
			Gui_FillImgArray_H(gCtrlObj_NumMiddleH,pNumListObj->w-(CO_NUM_ARROW_W<<1),&DrawRegion);	//��

		if(Middle != DNA_Null)//�м����ֲ���
		{
			sprintf((void *)NumStr,"%d",pNumListObj->Value);
			DrawRegion.x=pNumListObj->x+((pNumListObj->w-strlen((void *)NumStr)*CO_NUM_FONT_W)>>1);
			DrawRegion.y=pNumListObj->y+3;
			DrawRegion.w=pNumListObj->w-(CO_NUM_ARROW_W<<1);
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Space=CO_NUM_FONT_SPACE;		
		}
		if(Middle == DNA_Normal)
		{
			DrawRegion.Color=CO_NUM_FONT_COLOR;
			Gui_DrawFont(CO_NUM_FONT_STYLE,NumStr,&DrawRegion);//����
		}
		else if(Middle == DNA_HiLight)
		{
			DrawRegion.Color=CO_NUM_FONT_COLOR_H;
			Gui_DrawFont(CO_NUM_FONT_STYLE,NumStr,&DrawRegion);//����
		}
		
		if(Right != DNA_Null)//�ұ߲���
		{
			DrawRegion.x=pNumListObj->x+pNumListObj->w-CO_NUM_ARROW_W;
			DrawRegion.y=pNumListObj->y;
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
		}
		if(Right == DNA_Normal)
		{
			DrawRegion.w=CO_NUM_FRAME_W;
			Gui_DrawImgArray(gCtrlObj_NumRight,&DrawRegion);//�ұ߿�
		}
		else if(Right == DNA_HiLight)
		{
			DrawRegion.w=CO_NUM_FRAME_W;
			Gui_DrawImgArray(gCtrlObj_NumRightH,&DrawRegion);//�ұ߿�
		}
		else if(Right == DNA_NormalArrow)
		{
			DrawRegion.w=CO_NUM_ARROW_W;
			Gui_DrawImgArray(gCtrlObj_NumRightArrow,&DrawRegion);//�Ҽ�ͷ�滭
		}
		else if(Right == DNA_HiLightArrow)
		{
			DrawRegion.w=CO_NUM_ARROW_W;
			Gui_DrawImgArray(gCtrlObj_NumRightArrowH,&DrawRegion);//�Ҽ�ͷ�滭			
		}
	}
}
//-----------------------��ҳϵͳ����----------------------

//����ĳЩ�¼�ʱ���ᴥ���ĺ���
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam)
{
	NUM_BOX_OBJ *pNumBoxObj=pSysParam;
	NUM_LIST_OBJ *pNumListObj;
	NUM_ENUM_OBJ *pNumEnumObj;
	GUI_REGION DrawRegion;
	
	switch(SysEvent)
	{
		case Sys_PreGotoPage:
			if((pNumBoxObj->Type == NBT_NumBox) &&(Q_DB_GetStatus(Status_FsInitFinish,NULL)==FALSE)) //�ļ�ϵͳû�й���
			{
				Debug("File system not mount!!!\n\r");
				return SM_State_Faile|SM_NoGoto;
			}			
			return SM_State_OK;
		case Sys_PageInit:		//ϵͳÿ�δ����ҳ�棬�ᴦ������¼�		
			pNumListObj=pSysParam;
			pNumEnumObj=pSysParam;
			gpNbpVars=Q_PageMallco(sizeof(NUM_CTRL_OBJ_PAGE_VARS));
			gpNbpVars->HandlerType=pNumBoxObj->Type;
			if(pNumBoxObj->Type == NBT_NumBox)
			{
				PrtScreenToBin("/NumBoxPS.buf",0,0,176,56);
				DrawRegion.x=0;
				DrawRegion.y=0;
				DrawRegion.w=176;
				DrawRegion.h=56;
				DrawRegion.Color=FatColor(0xffffff);
				Gui_FillBlock(&DrawRegion);

				gpNbpVars->ObjHandler=pNumBoxObj;
				gpNbpVars->DefValue=pNumBoxObj->Value;
			}
			else if((pNumListObj->Type == NBT_NumList)||(pNumEnumObj->Type == NBT_NumEnum))
			{
				DrawRegion.x=pNumListObj->x;
				DrawRegion.y=pNumListObj->y;
				DrawRegion.w=CO_NUM_ARROW_W;
				DrawRegion.h=CO_NUM_H;
				Gui_ReadRegion16Bit(gpNbpVars->LeftDispBuf,&DrawRegion);//�����ͼ

				DrawRegion.x=pNumListObj->x+pNumListObj->w-CO_NUM_ARROW_W;
				DrawRegion.y=pNumListObj->y;
				DrawRegion.w=CO_NUM_ARROW_W;
				DrawRegion.h=CO_NUM_H;
				Gui_ReadRegion16Bit(gpNbpVars->RightDispBuf,&DrawRegion);//�����ͼ

				DrawNumCtrlObj((void *)pNumListObj,DNA_NormalArrow,DNA_HiLight,DNA_NormalArrow);//����ͼ

				MemCpy(&gpNbpVars->LeftArrowTch,&ArrowTchTmp,sizeof(IMG_TCH_OBJ));//���ͷ
				gpNbpVars->LeftArrowTch.ObjID=LeftArrowKV;
				gpNbpVars->LeftArrowTch.x=pNumListObj->x-TOUCH_REGION_MARGIN;
				gpNbpVars->LeftArrowTch.y=pNumListObj->y-TOUCH_REGION_MARGIN;
				gpNbpVars->LeftArrowTch.w+=(TOUCH_REGION_MARGIN<<1);
				gpNbpVars->LeftArrowTch.h+=(TOUCH_REGION_MARGIN<<1);
				Q_SetDynamicImgTch(1,&gpNbpVars->LeftArrowTch);//������̬��������

				MemCpy(&gpNbpVars->RightArrowTch,&ArrowTchTmp,sizeof(IMG_TCH_OBJ));//�Ҽ�ͷ
				gpNbpVars->RightArrowTch.ObjID=RightArrowKV;
				gpNbpVars->RightArrowTch.x=pNumListObj->x+pNumListObj->w-CO_NUM_ARROW_W-TOUCH_REGION_MARGIN;
				gpNbpVars->RightArrowTch.y=pNumListObj->y-TOUCH_REGION_MARGIN;
				gpNbpVars->RightArrowTch.w+=(TOUCH_REGION_MARGIN<<1);
				gpNbpVars->RightArrowTch.h+=(TOUCH_REGION_MARGIN<<1);
				Q_SetDynamicImgTch(2,&gpNbpVars->RightArrowTch);//������̬��������

				MemCpy(&gpNbpVars->NumTch,&NumTchTmp,sizeof(IMG_TCH_OBJ));//���ֿ�ģ��
				gpNbpVars->NumTch.ObjID=NumBoxKV;
				gpNbpVars->NumTch.x=pNumListObj->x+CO_NUM_ARROW_W+TOUCH_REGION_MARGIN;
				gpNbpVars->NumTch.y=DrawRegion.y;
				gpNbpVars->NumTch.w=pNumListObj->w-(CO_NUM_ARROW_W<<1)-(TOUCH_REGION_MARGIN<<1);
				Q_SetDynamicImgTch(3,&gpNbpVars->NumTch);//������̬��������

				gpNbpVars->ObjHandler=pNumListObj;//��¼�ؼ�ָ��			
				gpNbpVars->DefValue=pNumListObj->Value;
			}
			else
			{
			}
			break;
		case Sys_TouchSetOk:
		case Sys_TouchSetOk_SR:

			break;
		case Sys_PageClean:
			if(gpNbpVars!=NULL)
			{
				if(gpNbpVars->HandlerType==NBT_NumBox)
				{
					pNumBoxObj=(void *)gpNbpVars->ObjHandler;
					
					DrawRegion.x=0;
					DrawRegion.y=0;
					DrawRegion.w=176;
					DrawRegion.h=56;
					DrawRegion.Color=FatColor(NO_TRANS);
					Gui_DrawImgBin("/NumBoxPS.buf",&DrawRegion);
				}
				else if((gpNbpVars->HandlerType==NBT_NumList)||(gpNbpVars->HandlerType==NBT_NumEnum))
				{
					pNumListObj=(void *)gpNbpVars->ObjHandler;
					
					DrawRegion.x=pNumListObj->x;
					DrawRegion.y=pNumListObj->y;
					DrawRegion.w=CO_NUM_ARROW_W;
					DrawRegion.h=CO_NUM_H;
					DrawRegion.Color=CO_NUM_TRAN_COLOR;
					Gui_DrawImgArray((void *)gpNbpVars->LeftDispBuf,&DrawRegion);//��ԭ

					DrawRegion.x=pNumListObj->x+pNumListObj->w-CO_NUM_ARROW_W;
					DrawRegion.y=pNumListObj->y;
					DrawRegion.w=CO_NUM_ARROW_W;
					DrawRegion.h=CO_NUM_H;
					DrawRegion.Color=CO_NUM_TRAN_COLOR;
					Gui_DrawImgArray((void *)gpNbpVars->RightDispBuf,&DrawRegion);//��ԭ

					if(gpNbpVars->DefValue == pNumListObj->Value)
						DrawNumCtrlObj((void *)pNumListObj,DNA_Normal,DNA_Normal,DNA_Normal);//δ�ı�ԭ��ֵ
					else
						DrawNumCtrlObj((void *)pNumListObj,DNA_HiLight,DNA_HiLight,DNA_HiLight);//�ı���ԭ��ֵ
				}
				else
				{
				}

				Q_PageFree(gpNbpVars);
			}
			return SM_State_OK;
		default:
			//��Ҫ��Ӧ���¼�δ����
			Debug("%s SystemEventHandler:This System Event Handler case unfinish! SysEvent:%d\n\r",Q_GetCurrPageName(),SysEvent);
			//while(1);
	}
	
	return SM_State_OK;
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
			}
			else if(IntParam==ExtiKeyDown)
			{
			}
			break;
		case Perip_KeyRelease:
			if(IntParam==ExtiKeyEnter)
			{PrtScreen();
			}
			else if(IntParam==ExtiKeyUp)
			{
			}
			else if(IntParam==ExtiKeyDown)
			{
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
		case LeftArrowKV:
			if(InEvent == Tch_Press)
			{
				NUM_LIST_OBJ *pNumList=(void *)gpNbpVars->ObjHandler;
				DrawNumCtrlObj((void *)pNumList,DNA_HiLightArrow,DNA_Null,DNA_Null);//����ͼ
			}
			else
			{
				NUM_LIST_OBJ *pNumList=(void *)gpNbpVars->ObjHandler;
				NUM_ENUM_OBJ *pNumEnum=(void *)gpNbpVars->ObjHandler;
				if(InEvent == Tch_Release)
				{
					if(pNumList->Type == NBT_NumList)
					{
						if((pNumList->Value-pNumList->Step)>=pNumList->Min)
							pNumList->Value-=pNumList->Step;
						else pNumList->Value=pNumList->Max;
					}
					else if(pNumEnum->Type == NBT_NumEnum)
					{
						if((pNumEnum->Idx-1)>=0) pNumEnum->Idx--;
						else pNumEnum->Idx=(pNumEnum->Total-1);
						pNumEnum->Value=pNumEnum->pEnumList[pNumEnum->Idx];			
					}
				}
				DrawNumCtrlObj((void *)pNumList,DNA_NormalArrow,DNA_HiLight,DNA_Null);//����ͼ
			}
			break;
		case RightArrowKV:
			if(InEvent == Tch_Press)
			{
				NUM_LIST_OBJ *pNumList=(void *)gpNbpVars->ObjHandler;
				DrawNumCtrlObj((void *)pNumList,DNA_Null,DNA_Null,DNA_HiLightArrow);//����ͼ
			}
			else
			{					
				NUM_LIST_OBJ *pNumList=(void *)gpNbpVars->ObjHandler;
				NUM_ENUM_OBJ *pNumEnum=(void *)gpNbpVars->ObjHandler;
				if(InEvent == Tch_Release)
				{
					if(pNumList->Type == NBT_NumList)
					{
						if((pNumList->Value+pNumList->Step)<=pNumList->Max)
							pNumList->Value+=pNumList->Step;
						else pNumList->Value=pNumList->Min;
					}
					else if(pNumEnum->Type == NBT_NumEnum)
					{
						if((pNumEnum->Idx+1)<pNumEnum->Num) pNumEnum->Idx++;
						else pNumEnum->Idx=0;
						pNumEnum->Value=pNumEnum->pEnumList[pNumEnum->Idx];		
					}
				}
				DrawNumCtrlObj((void *)pNumList,DNA_Null,DNA_HiLight,DNA_NormalArrow);//����ͼ
			}
			break;
		case NumBoxKV:
			Q_GotoPage(SubPageReturn,"",0,NULL);//����ǰһ��ҳ��
			break;
			
		case HomeKV:
			Q_GotoPage(SubPageReturn,"",0,NULL);//����ǰһ��ҳ��
			break;
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



