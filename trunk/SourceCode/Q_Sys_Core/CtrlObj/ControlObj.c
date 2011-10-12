#include "System.h"
#include "Theme.h"
#include "CtrlObjImg.h"

#define CO_Debug Debug

//***********************************��������*********************************************
static const PAGE_CONTROL_NUM *gpCtrlObjNum=NULL;//��ǰҳ��ؼ�������¼

//���ڼ�¼ҳ����ʱ������ָ�����顣[0]δʹ�ã�����Ҫ��һ��Ԫ��
static void *gPageDataPtrRecord[MAX_PAGE_LAYER_NUM+1];

//ͼ�갴���滻����ʱ��׺�洢����
static REP_IMG_SUFX gRepImgSufx[MAX_IMG_KEY_NUM];

//���ְ����滻����ʱ�ַ���ָ��洢����
static u8 *gRepKeyNameCon[MAX_CHAR_KEY_NUM];

static const IMG_TCH_OBJ *gpCurImgTchCon=NULL; //��ǰ�������򼯺�
static const CHAR_TCH_OBJ *gpCurCharTchCon=NULL; //��ǰ�������򼯺�

//��̬�ؼ��洢ʵ��ָ��
static void *gCtrlObjPtrBuf[MAX_DYN_CTRL_OBJ_NUM];//�����洢���пؼ�ָ���ʵ��
//IMG_TCH_OBJ **gpDynImgTchCon=NULL; //��̬ͼƬ��������
//CHAR_TCH_OBJ **gpDynCharTchCon=NULL;//��̬���ְ�������
#ifdef QSYS_FRAME_FULL	
//YES_NO_OBJ **gpYesNoCon=NULL;
//NUM_BOX_OBJ **gpNumBoxCon=NULL;
//STR_OPT_OBJ **gpStrOptCon=NULL;
//STR_INPUT_OBJ **gpStrInputCon=NULL;
#endif

//***********************************�ⲿ����*********************************************
extern u8 GetCurLayerNum(void);
extern const PAGE_ATTRIBUTE *GetPageByIdx(u8 PageIdx);
extern TOUCH_REGION *GetTouchInfoByIdx(u8 Idx);

//***********************************��������*********************************************
void ControlObjInit(void)//ϵͳ����ʱ�ĳ�ʼ��
{	
	u16 i;
	
	MemSet(gPageDataPtrRecord,0,(MAX_PAGE_LAYER_NUM+1)*sizeof(void *));//�����ʱҳ���������ָ��

	for(i=0;i<Q_GetPageTotal();i++)
	{
		if((GetPageByIdx(i)->CtrlObjNum.ImgTchNum>MAX_IMG_KEY_NUM)
			||(GetPageByIdx(i)->CtrlObjNum.CharTchNum>MAX_CHAR_KEY_NUM))
		{//ҳ��İ������򳬹�֧����Ŀ
			Debug("%s touch region num is too much!%d %d\n\r",GetPageByIdx(i)->Name,
			GetPageByIdx(i)->CtrlObjNum.ImgTchNum,GetPageByIdx(i)->CtrlObjNum.CharTchNum);
			Q_ErrorStopScreen("!!!New page's touch region num is over MAX_IMG_KEY_NUM or MAX_CHAR_KEY_NUM");
		}

		if((GetPageByIdx(i)->CtrlObjNum.DynImgTchNum
			+GetPageByIdx(i)->CtrlObjNum.DynCharTchNum
#ifdef QSYS_FRAME_FULL	
			+GetPageByIdx(i)->CtrlObjNum.YesNoNum
			+GetPageByIdx(i)->CtrlObjNum.NumBoxNum
			+GetPageByIdx(i)->CtrlObjNum.StrOptBoxNum
			+GetPageByIdx(i)->CtrlObjNum.StrInputBoxNum
#endif
			)>MAX_DYN_CTRL_OBJ_NUM)
		{//ҳ��Ŀؼ���������֧����Ŀ
			Debug("%s control object num is too much!\n\r",GetPageByIdx(i)->Name);
			Q_ErrorStopScreen("!!!Control Object num is over MAX_DYN_CTRL_OBJ_NUM!!!");
		}
	}
}

//������ҳ��ʱ�������пؼ�������ͳһ�Ĵ洢��
//�����������
//����˳������:
//ͼƬ����
//���ְ���
//��̬ͼƬ����
//��̬���ְ���
//YesNo
//NumBox
//StrOpt
//StrInput
static void CopyCtrlObjTouchReg(TOUCH_REGION *TouchRegsBuf)
{
	u8 Idx;
	u8 Cnt=0;

	IMG_TCH_OBJ **pDynImgTchCon=(void *)gCtrlObjPtrBuf; //��̬ͼƬ��������
	CHAR_TCH_OBJ **pDynCharTchCon=(void *)&pDynImgTchCon[gpCtrlObjNum->DynImgTchNum];//��̬���ְ�������
#ifdef QSYS_FRAME_FULL	
	YES_NO_OBJ **pYesNoCon=(void *)&pDynCharTchCon[gpCtrlObjNum->DynCharTchNum];
	NUM_BOX_OBJ **pNumBoxCon=(void *)&pYesNoCon[gpCtrlObjNum->YesNoNum];
	STR_OPT_OBJ **pStrOptCon=(void *)&pNumBoxCon[gpCtrlObjNum->NumBoxNum];
	STR_INPUT_OBJ **pStrInputCon=(void *)&pStrOptCon[gpCtrlObjNum->StrOptBoxNum];
#endif

	//ͼƬ��������
	for(Idx=0;Idx<gpCtrlObjNum->ImgTchNum;Idx++,Cnt++)
	{
		TouchRegsBuf[Cnt].x=gpCurImgTchCon[Idx].x;
		TouchRegsBuf[Cnt].y=gpCurImgTchCon[Idx].y;
		TouchRegsBuf[Cnt].w=gpCurImgTchCon[Idx].w;
		TouchRegsBuf[Cnt].h=gpCurImgTchCon[Idx].h;
		TouchRegsBuf[Cnt].ObjID=gpCurImgTchCon[Idx].ObjID;
		TouchRegsBuf[Cnt].Type=COT_ImgTch;
		TouchRegsBuf[Cnt].Index=Idx;
		TouchRegsBuf[Cnt].OptionsMask=(u8)(gpCurImgTchCon[Idx].OptionsMask&0xff);
	}
				
	//���ְ�������
	for(Idx=0;Idx<gpCtrlObjNum->CharTchNum;Idx++,Cnt++)
	{
		TouchRegsBuf[Cnt].x=gpCurCharTchCon[Idx].x;
		TouchRegsBuf[Cnt].y=gpCurCharTchCon[Idx].y;
		TouchRegsBuf[Cnt].w=gpCurCharTchCon[Idx].w;
		TouchRegsBuf[Cnt].h=gpCurCharTchCon[Idx].h;
		TouchRegsBuf[Cnt].ObjID=gpCurCharTchCon[Idx].ObjID;
		TouchRegsBuf[Cnt].Type=COT_CharTch;
		TouchRegsBuf[Cnt].Index=Idx;
		TouchRegsBuf[Cnt].OptionsMask=(u8)(gpCurCharTchCon[Idx].OptionsMask&0xff);
	}

	//��̬ͼƬ��������
	for(Idx=0;Idx<gpCtrlObjNum->DynImgTchNum;Idx++,Cnt++)
	{
		if(pDynImgTchCon[Idx]!=NULL)
		{
			TouchRegsBuf[Cnt].x=pDynImgTchCon[Idx]->x;
			TouchRegsBuf[Cnt].y=pDynImgTchCon[Idx]->y;
			TouchRegsBuf[Cnt].w=pDynImgTchCon[Idx]->w;
			TouchRegsBuf[Cnt].h=pDynImgTchCon[Idx]->h;
			TouchRegsBuf[Cnt].ObjID=pDynImgTchCon[Idx]->ObjID;
			TouchRegsBuf[Cnt].Type=COT_DynImg;
			TouchRegsBuf[Cnt].Index=Idx;
			TouchRegsBuf[Cnt].OptionsMask=(u8)(pDynImgTchCon[Idx]->OptionsMask&0xff);
		}
	}

	//��̬���ְ�������
	for(Idx=0;Idx<gpCtrlObjNum->DynCharTchNum;Idx++,Cnt++)
	{
		if(pDynCharTchCon[Idx]!=NULL)
		{
			TouchRegsBuf[Cnt].x=pDynCharTchCon[Idx]->x;
			TouchRegsBuf[Cnt].y=pDynCharTchCon[Idx]->y;
			TouchRegsBuf[Cnt].w=pDynCharTchCon[Idx]->w;
			TouchRegsBuf[Cnt].h=pDynCharTchCon[Idx]->h;
			TouchRegsBuf[Cnt].ObjID=pDynCharTchCon[Idx]->ObjID;
			TouchRegsBuf[Cnt].Type=COT_DynChar;
			TouchRegsBuf[Cnt].Index=Idx;
			TouchRegsBuf[Cnt].OptionsMask=(u8)(pDynCharTchCon[Idx]->OptionsMask&0xff);
		}
	}

#ifdef QSYS_FRAME_FULL	
	//yes no
	for(Idx=0;Idx<gpCtrlObjNum->YesNoNum;Idx++,Cnt++)
	{
		if(pYesNoCon[Idx]!=NULL)
		{
			TouchRegsBuf[Cnt].x=pYesNoCon[Idx]->x;
			TouchRegsBuf[Cnt].y=pYesNoCon[Idx]->y;
			TouchRegsBuf[Cnt].w=CO_YES_NO_W;
			TouchRegsBuf[Cnt].h=CO_YES_NO_H;
			TouchRegsBuf[Cnt].ObjID=pYesNoCon[Idx]->ObjID;
			TouchRegsBuf[Cnt].Type=COT_YesNo;
			TouchRegsBuf[Cnt].Index=Idx;
			TouchRegsBuf[Cnt].OptionsMask=(u8)(PrsMsk|RelMsk|ReVMsk);
		}
	}

	//num box
	for(Idx=0;Idx<gpCtrlObjNum->NumBoxNum;Idx++,Cnt++)
	{
		if(pNumBoxCon[Idx]!=NULL)
		{
			TouchRegsBuf[Cnt].x=pNumBoxCon[Idx]->x;
			TouchRegsBuf[Cnt].y=pNumBoxCon[Idx]->y;
			TouchRegsBuf[Cnt].w=pNumBoxCon[Idx]->w;
			TouchRegsBuf[Cnt].h=CO_NUM_H;
			TouchRegsBuf[Cnt].ObjID=pNumBoxCon[Idx]->ObjID;
			TouchRegsBuf[Cnt].Type=COT_NumBox;
			TouchRegsBuf[Cnt].Index=Idx;
			TouchRegsBuf[Cnt].OptionsMask=0;//(u8)(pNumBoxCon[Idx]->OptionsMask&0xff);
		}
	}
#if 0
	//str option
	for(Idx=0;Idx<gpCtrlObjNum->StrOptBoxNum;Idx++,Cnt++)
	{
		if(pStrOptCon[Idx]!=NULL)
		{
			TouchRegsBuf[Cnt].x=pStrOptCon[Idx]->x;
			TouchRegsBuf[Cnt].y=pStrOptCon[Idx]->y;
			TouchRegsBuf[Cnt].w=pStrOptCon[Idx]->w;
			TouchRegsBuf[Cnt].h=CO_STR_OPT_H;
			TouchRegsBuf[Cnt].ObjID=pStrOptCon[Idx]->ObjID;
			TouchRegsBuf[Cnt].Type=COT_StrOpt;
			TouchRegsBuf[Cnt].Index=Idx;
			TouchRegsBuf[Cnt].OptionsMask=(u8)(pStrOptCon[Idx]->OptionsMask&0xff);
		}
	}

	//str input
	for(Idx=0;Idx<gpCtrlObjNum->StrInputBoxNum;Idx++,Cnt++)
	{
		if(pStrInputCon[Idx]!=NULL)
		{
			TouchRegsBuf[Cnt].x=pStrInputCon[Idx]->x;
			TouchRegsBuf[Cnt].y=pStrInputCon[Idx]->y;
			TouchRegsBuf[Cnt].w=pStrInputCon[Idx]->w;
			TouchRegsBuf[Cnt].h=pStrInputCon[Idx]->h;
			TouchRegsBuf[Cnt].ObjID=pStrInputCon[Idx]->ObjID;
			TouchRegsBuf[Cnt].Type=COT_StrInput;
			TouchRegsBuf[Cnt].Index=Idx;
			TouchRegsBuf[Cnt].OptionsMask=(u8)(pStrInputCon[Idx]->OptionsMask&0xff);
		}
	}
#endif
#endif
}

//�л�ҳ��ʱִ�еĿؼ��������ݴ���
void PageSwithcCtrlObjDataHandler(const PAGE_ATTRIBUTE *pNewPage)
{
	gpCtrlObjNum=&gpCurrentPage->CtrlObjNum;//�õ��ؼ������ṹ��

	gpCurImgTchCon=gpCurrentPage->pImgTchRegCon;//�л�������������ָ��
	gpCurCharTchCon=gpCurrentPage->pCharTchRegCon;
	
	if(gTouchRegionNum) Q_Free(gpTouchRegions);//�ջ��ϸ�ҳ����ڴ�
	
	gTouchRegionNum=gpCtrlObjNum->ImgTchNum
									+gpCtrlObjNum->CharTchNum
									+gpCtrlObjNum->DynImgTchNum
									+gpCtrlObjNum->DynCharTchNum
#ifdef QSYS_FRAME_FULL	
									+gpCtrlObjNum->YesNoNum//yes or no ѡ��ĸ���
									+gpCtrlObjNum->NumBoxNum//���ֿ�ĸ���
									+gpCtrlObjNum->StrOptBoxNum//�ַ���ѡ���ĸ���
									+gpCtrlObjNum->StrInputBoxNum//�ַ��������ĸ���
#endif
	;
	if(gTouchRegionNum)//�������ڴ����ڴ��������Ϣ
	{
		gpTouchRegions=Q_Mallco(gTouchRegionNum*sizeof(TOUCH_REGION));//���ƴ�������
		CopyCtrlObjTouchReg(gpTouchRegions);//�����пؼ���������Ϣ���Ƶ����������¼�ṹ�壬�������߳�ʹ��
	}
}

//ͨ����ֵ��Ѱ������������0xff��ʾδ�ҵ�
//ֻ֧���ҵ��ĵ�һ��!!����һ����ֵ��������������֧�ֺ���İ���
static u8 FindImgTchIdx(u8 Key)
{
	u8 i=0;

	for(;i<gpCtrlObjNum->ImgTchNum;i++)
		if(gpCurImgTchCon[i].ObjID==Key) return i;

	return 0xff;
}

static u8 FindCharTchIdx(u8 Key)
{
	u8 i=0;

	for(;i<gpCtrlObjNum->CharTchNum;i++)
		if(gpCurCharTchCon[i].ObjID==Key) return i;

	return 0xff;
}

//������ʾͼƬ���������ͼƬ
static bool ImgTchDisplay(u8 Index,INPUT_EVT_TYPE InType,bool IsDyn)
{
	IMG_TCH_OBJ *pTouchRegion;
	GUI_REGION BmpRegion;
	GUI_REGION BgRegion;
	u8 PathBuf[MAX_BMP_PATH_LEN]="";
	u8 ImgSuffix=0,RepSuffix=0;
	bool LandScapeMode=Gui_GetLandScapeMode();//�洢��ǰ����ģʽ
	bool Ret=TRUE;
	COLOR_TYPE RegionColor;

	if(IsDyn)//�Ƿ���ҳ�涯̬���ͼ��
	{
		pTouchRegion=gCtrlObjPtrBuf[Index];

		switch(InType)//����¼�����
		{
			case Input_TchNormal:
				RegionColor=FatColor(NORMAL_REPLACE_COLOR);
				ImgSuffix=IMG_KEY_NOR_DEF_SUFFIX;
				break;
			case Input_TchPress:
				RegionColor=FatColor(PRESS_REPLACE_COLOR);
				ImgSuffix=IMG_KEY_PRS_DEF_SUFFIX;
				break;
			case Input_TchRelease:
			case Input_TchReleaseVain:
				RegionColor=FatColor(RELEASE_REPLACE_COLOR);
				ImgSuffix=IMG_KEY_REL_DEF_SUFFIX;
				break;
			default:
				return FALSE;
		}
	}
	else //ҳ��̶�����
	{
		pTouchRegion=(void *)&gpCurImgTchCon[Index];
		if(gRepImgSufx[Index])	 RepSuffix=gRepImgSufx[Index];
		
		switch(InType)//����¼�����
		{
			case Input_TchNormal:
				RegionColor=FatColor(NORMAL_REPLACE_COLOR);
				ImgSuffix=IMG_KEY_NOR_DEF_SUFFIX;
				break;
			case Input_TchPress:
				RegionColor=FatColor(PRESS_REPLACE_COLOR);
				ImgSuffix=IMG_KEY_PRS_DEF_SUFFIX;
				break;
			case Input_TchRelease:
			case Input_TchReleaseVain:
				RegionColor=FatColor(RELEASE_REPLACE_COLOR);
				ImgSuffix=IMG_KEY_REL_DEF_SUFFIX;
				break;
			default:
				return FALSE;
		}
	}
	
	if(pTouchRegion->ImgPathPrefix[0])//ָ����·��ǰ׺��˵����Ҫ��ʾ����ͼ��
	{
		if(pTouchRegion->OptionsMask&PathMsk)//ʹ���Զ���·����ֻ���������·��
		{
			if(strlen((void *)Q_GetNowThemePath())+strlen((void *)pTouchRegion->ImgPathPrefix)>=(MAX_BMP_PATH_LEN-7))
			{
				Debug("!!!Error:Path is too long!!!");
				return FALSE;//·��̫��
			}
			if(RepSuffix) sprintf((void *)PathBuf,"%s%s%c%c",Q_GetNowThemePath(),pTouchRegion->ImgPathPrefix,RepSuffix,ImgSuffix);
			else sprintf((void *)PathBuf,"%s%s%c",Q_GetNowThemePath(),pTouchRegion->ImgPathPrefix,ImgSuffix);
		}
		else //��ʹ���Զ���·�������������·����ҳ�����ļ���
		{
			if((strlen((void *)Q_GetNowThemePath())+strlen((void *)gpCurrentPage->Name)
				+strlen((void *)pTouchRegion->ImgPathPrefix))>=(MAX_BMP_PATH_LEN-7))
			{
				Debug("!!!Error:Path is too long!!!");
				return FALSE;//·��̫��
			}
			if(RepSuffix) sprintf((void *)PathBuf,"%s%s/Btn/%s%c%c",Q_GetNowThemePath(),gpCurrentPage->Name,pTouchRegion->ImgPathPrefix,RepSuffix,ImgSuffix);//�õ�·��
			else sprintf((void *)PathBuf,"%s%s/Btn/%s%c",Q_GetNowThemePath(),gpCurrentPage->Name,pTouchRegion->ImgPathPrefix,ImgSuffix);//�õ�·��
		}
		
		if(pTouchRegion->OptionsMask&BinMsk)//�Ƿ����bin�ļ���ͼ���ļ�
			strcat((void *)PathBuf,".bin");
		else
			strcat((void *)PathBuf,".bmp");
		
		//������ʾģʽ��������
		if(pTouchRegion->OptionsMask&LandMsk)
		{//����ģʽ
			Gui_SetLandScapeMode(TRUE);
			BmpRegion.x=pTouchRegion->x-pTouchRegion->ImgX;
			BmpRegion.y=pTouchRegion->y+pTouchRegion->ImgY;
			BmpRegion.w=pTouchRegion->w-(pTouchRegion->ImgY<<1);
			BmpRegion.h=pTouchRegion->h-(pTouchRegion->ImgX<<1);
		}
		else		
		{//����ģʽ
			Gui_SetLandScapeMode(FALSE);
			BmpRegion.x=pTouchRegion->x+pTouchRegion->ImgX;
			BmpRegion.y=pTouchRegion->y+pTouchRegion->ImgY;
			BmpRegion.w=pTouchRegion->w-(pTouchRegion->ImgX<<1);
			BmpRegion.h=pTouchRegion->h-(pTouchRegion->ImgY<<1);
		}		
		
		BmpRegion.Color=pTouchRegion->TransColor;
		if((pTouchRegion->OptionsMask&DbgMsk)||
			((pTouchRegion->OptionsMask&BinMsk)?
			(Gui_DrawImgBin(PathBuf,&BmpRegion)==Gui_No_Such_File):
			(Gui_Draw24Bmp(PathBuf,&BmpRegion)==Gui_No_Such_File)))
		{//����ģʽ���ߴ�ͼ���ļ�ʧ�ܣ�����ʾ��������
			BgRegion.x=pTouchRegion->x;
			BgRegion.y=pTouchRegion->y;
			BgRegion.w=pTouchRegion->w;
			BgRegion.h=pTouchRegion->h;
			BgRegion.Color=FatColor(REGION_BG_COLOR);
			Gui_FillBlock(&BgRegion);	
			BmpRegion.Color=RegionColor;
			Gui_FillBlock(&BmpRegion);

			if(pTouchRegion->Name&&pTouchRegion->Name[0])
			{
				BmpRegion.Color=FatColor(KEY_NAME_COLOR);
				BmpRegion.Space=0;
				if(pTouchRegion->OptionsMask&F16Msk)
					Gui_DrawFont(GBK16_FONT,(void *)pTouchRegion->Name,&BmpRegion);
				else if(pTouchRegion->OptionsMask&B14Msk)
					Gui_DrawFont(ASC14B_FONT,(void *)pTouchRegion->Name,&BmpRegion);
				else
					Gui_DrawFont(GBK12_FONT,(void *)pTouchRegion->Name,&BmpRegion);
			}

			Ret=FALSE;
		}		
		
		Gui_SetLandScapeMode(LandScapeMode);
	}
	else if(pTouchRegion->OptionsMask&DbgMsk)//û��ָ����׺�����ǿ�����DbgMsk,������ʾ�κ�����
	{
		//������ʾģʽ��������
		if(pTouchRegion->OptionsMask&LandMsk)
		{//����ģʽ
			Gui_SetLandScapeMode(TRUE);
		}
		else		
		{//����ģʽ
			Gui_SetLandScapeMode(FALSE);
		}		
		
		//��ʾ��������
		BgRegion.x=pTouchRegion->x;
		BgRegion.y=pTouchRegion->y;
		BgRegion.w=pTouchRegion->w;
		BgRegion.h=pTouchRegion->h;
		BgRegion.Color=FatColor(REGION_BG_COLOR);
		Gui_FillBlock(&BgRegion);	

		if(pTouchRegion->Name&&pTouchRegion->Name[0])//��ʾ�ַ�
		{
			BgRegion.x++;
			BgRegion.y++;
			BgRegion.w-=2;
			BgRegion.h-=2;
			BgRegion.Color=FatColor(KEY_NAME_COLOR);
			BgRegion.Space=0;
			if(pTouchRegion->OptionsMask&F16Msk)
				Gui_DrawFont(GBK16_FONT,(void *)pTouchRegion->Name,&BgRegion);
			else
				Gui_DrawFont(GBK12_FONT,(void *)pTouchRegion->Name,&BgRegion);
		}
	
		Gui_SetLandScapeMode(LandScapeMode);
	}

	return Ret;
}

//������ʾ���ִ������������
static bool CharTchDisplay(u8 Index,INPUT_EVT_TYPE InType,bool IsDyn)
{
	const CHAR_TCH_OBJ *pTouchRegion;
	GUI_REGION CharRegion;
	GUI_REGION BgRegion;
	bool LandScapeMode=Gui_GetLandScapeMode();//�洢��ǰ����ģʽ
	bool Ret=TRUE;
	u8 *pName;
	COLOR_TYPE CharColor,BgColor;

	if(IsDyn)//����ǲ��Ƕ�̬���ְ�����
	{
		pTouchRegion=gCtrlObjPtrBuf[gpCtrlObjNum->DynImgTchNum+Index];
		pName=pTouchRegion->Name;
	}
	else//��ҳ��Ĺ̶����ְ�����
	{
		pTouchRegion=&gpCurCharTchCon[Index];

		if(gRepKeyNameCon[Index]!=NULL)	pName=gRepKeyNameCon[Index];
		else	pName=pTouchRegion->Name;
	}

	switch(InType)//����¼�����
	{
		case Input_TchNormal:
			CharColor=pTouchRegion->NormalColor;
			BgColor=pTouchRegion->NormalBG;		
			break;
		case Input_TchPress:
			CharColor=pTouchRegion->PressColor;
			BgColor=pTouchRegion->PressBG;	
			break;
		case Input_TchRelease:
		case Input_TchReleaseVain:
			CharColor=pTouchRegion->ReleaseColor;
			BgColor=pTouchRegion->ReleaseBG;	
			break;
		default:
			return FALSE;
	}
			
	if(pName!=NULL&&pName[0])//��������ʾ
	{		
		//4	�ȸ�����ʾģʽ��������
		if(pTouchRegion->OptionsMask&LandMsk)
		{//����ģʽ
			Gui_SetLandScapeMode(TRUE);
			//����
			if(CharColor!=FatColor(TRANS_COR))//�������͸��ɫ
			{
				CharRegion.x=pTouchRegion->x-pTouchRegion->CharX;
				CharRegion.y=pTouchRegion->y+pTouchRegion->CharY;
				CharRegion.w=pTouchRegion->w-pTouchRegion->CharY;
				CharRegion.h=pTouchRegion->h-pTouchRegion->CharX;
			}

			if(pTouchRegion->OptionsMask&DbgMsk)//����ģʽ��������������ʾ����
			{
				BgRegion.x=pTouchRegion->x;
				BgRegion.y=pTouchRegion->y;
				BgRegion.w=pTouchRegion->w;
				BgRegion.h=pTouchRegion->h;
				BgRegion.Color=FatColor(REGION_BG_COLOR);
				Gui_FillBlock(&BgRegion);
			}

			//����
			if(BgColor!=FatColor(TRANS_COR))//�������͸��ɫ
			{
				if(pTouchRegion->OptionsMask&RoueMsk) //Բ��
				{
					BgRegion.h=pTouchRegion->h-2*(pTouchRegion->Margin>>4);
					BgRegion.w=pTouchRegion->w-2*(pTouchRegion->Margin&0x0f)-BgRegion.h;					
					BgRegion.x=pTouchRegion->x-(pTouchRegion->Margin&0x0f);
					BgRegion.y=pTouchRegion->y+(pTouchRegion->Margin>>4)+(BgRegion.h>>1);
					BgRegion.Color=BgColor;
					Gui_FillBlock(&BgRegion);

					BgRegion.x-=(BgRegion.h>>1);
					BgRegion.w=(BgRegion.h>>1);
					BgRegion.Space=0xc3;
					Gui_DrawCircle(&BgRegion,TRUE);

					BgRegion.y=pTouchRegion->y-(BgRegion.h>>1)+pTouchRegion->w-(pTouchRegion->Margin>>4)-1;
					BgRegion.Space=0x3c;
					Gui_DrawCircle(&BgRegion,TRUE);
				}
				else
				{
					BgRegion.x=pTouchRegion->x-(pTouchRegion->Margin&0x0f);
					BgRegion.y=pTouchRegion->y+(pTouchRegion->Margin>>4);
					BgRegion.w=pTouchRegion->w-2*(pTouchRegion->Margin&0x0f);
					BgRegion.h=pTouchRegion->h-2*(pTouchRegion->Margin>>4);
					BgRegion.Color=BgColor;
					Gui_FillBlock(&BgRegion);//4	��ʾ����
				}
			}
		}
		else		
		{//����ģʽ
			Gui_SetLandScapeMode(FALSE);
			//����
			if(CharColor!=FatColor(TRANS_COR))//�������͸��ɫ
			{
				CharRegion.x=pTouchRegion->x+pTouchRegion->CharX;
				CharRegion.y=pTouchRegion->y+pTouchRegion->CharY;
				CharRegion.w=pTouchRegion->w-pTouchRegion->CharX;
				CharRegion.h=pTouchRegion->h-pTouchRegion->CharY;
			}

			if(pTouchRegion->OptionsMask&DbgMsk)//����ģʽ��������������ʾ����
			{
				BgRegion.x=pTouchRegion->x;
				BgRegion.y=pTouchRegion->y;
				BgRegion.w=pTouchRegion->w;
				BgRegion.h=pTouchRegion->h;
				BgRegion.Color=FatColor(REGION_BG_COLOR);
				Gui_FillBlock(&BgRegion);
			}
			
			//����
			if(BgColor!=FatColor(TRANS_COR))//�������͸��ɫ
			{
				if(pTouchRegion->OptionsMask&RoueMsk) //Բ��
				{
					BgRegion.h=pTouchRegion->h-2*(pTouchRegion->Margin&0x0f);
					BgRegion.w=pTouchRegion->w-2*(pTouchRegion->Margin>>4)-BgRegion.h;
					BgRegion.x=pTouchRegion->x+(pTouchRegion->Margin>>4)+(BgRegion.h>>1);
					BgRegion.y=pTouchRegion->y+(pTouchRegion->Margin&0x0f);
					BgRegion.Color=BgColor;
					Gui_FillBlock(&BgRegion);

					BgRegion.y+=(BgRegion.h>>1);
					BgRegion.w=(BgRegion.h>>1);
					BgRegion.Space=0xf0;
					Gui_DrawCircle(&BgRegion,TRUE);

					BgRegion.x=pTouchRegion->x-(BgRegion.h>>1)+pTouchRegion->w-(pTouchRegion->Margin>>4)-1;
					BgRegion.Space=0x0f;
					Gui_DrawCircle(&BgRegion,TRUE);
				}
				else
				{
					BgRegion.x=pTouchRegion->x+(pTouchRegion->Margin>>4);
					BgRegion.y=pTouchRegion->y+(pTouchRegion->Margin&0x0f);
					BgRegion.w=pTouchRegion->w-2*(pTouchRegion->Margin>>4);
					BgRegion.h=pTouchRegion->h-2*(pTouchRegion->Margin&0x0f);
					BgRegion.Color=BgColor;
					Gui_FillBlock(&BgRegion);//4	��ʾ����
				}
			}
		}		

		//4	��ʾ����
		if(CharColor!=FatColor(TRANS_COR))//�������͸��ɫ
		{
			CharRegion.Space=pTouchRegion->Space;
			CharRegion.Color=CharColor;
			if(pTouchRegion->OptionsMask&F16Msk)
				Gui_DrawFont(GBK16_FONT,(void *)pName,&CharRegion);
			else if(pTouchRegion->OptionsMask&B14Msk)
				Gui_DrawFont(ASC14B_FONT,(void *)pName,&CharRegion);
			else
				Gui_DrawFont(GBK12_FONT,(void *)pName,&CharRegion);
		}

		Gui_SetLandScapeMode(LandScapeMode);//��ԭ��Ļģʽ
	}
	else if(pTouchRegion->OptionsMask&DbgMsk)//û��ָ�����֣����ǿ�����DbgMsk,������ʾ�κ�����
	{		
		//4	�ȸ�����ʾģʽ��������
		if(pTouchRegion->OptionsMask&LandMsk)
		{//����ģʽ
			Gui_SetLandScapeMode(TRUE);
		}
		else		
		{//����ģʽ
			Gui_SetLandScapeMode(FALSE);
		}		

		BgRegion.x=pTouchRegion->x;
		BgRegion.y=pTouchRegion->y;
		BgRegion.w=pTouchRegion->w;
		BgRegion.h=pTouchRegion->h;
		BgRegion.Color=FatColor(REGION_BG_COLOR);
		Gui_FillBlock(&BgRegion);
		
		Gui_SetLandScapeMode(LandScapeMode);//��ԭ��Ļģʽ
	}

	return Ret;
}

//����ͬ��ֵ�İ�������ʾ
void SameOidTchDisplay(u8 ObjID,INPUT_EVT_TYPE InType)
{
	u8 i;
	IMG_TCH_OBJ **pDynImgTchCon=(void *)gCtrlObjPtrBuf; //��̬ͼƬ��������
	CHAR_TCH_OBJ **pDynCharTchCon=(void *)&pDynImgTchCon[gpCtrlObjNum->DynImgTchNum];//��̬���ְ�������

	//��ͬ��ֵ�����ְ���
	for(i=0;i<gpCtrlObjNum->CharTchNum;i++)//����ͬ��ֵ�ļ�
	{
		if(gpCurCharTchCon[i].ObjID==ObjID)
			CharTchDisplay(i,InType,FALSE);
	}

	//��ͬ��ֵ�Ķ�̬���ְ���
	for(i=0;i<gpCtrlObjNum->DynCharTchNum;i++)
	{
		if(pDynCharTchCon[i]!=NULL)
			if(pDynCharTchCon[i]->ObjID==ObjID)
				CharTchDisplay(i,InType,TRUE);
	}
	
	//��ͬ��ֵ��ͼ�񰴼�
	for(i=0;i<gpCtrlObjNum->ImgTchNum;i++)//����ͬ��ֵ�ļ�
	{
		if(gpCurImgTchCon[i].ObjID==ObjID)
			ImgTchDisplay(i,InType,FALSE);
	}

	//��ͬ��ֵ�Ķ�̬ͼ�񰴼�
	for(i=0;i<gpCtrlObjNum->DynImgTchNum;i++) 
	{
		if(pDynImgTchCon[i]!=NULL)
			if(pDynImgTchCon[i]->ObjID==ObjID)
				ImgTchDisplay(i,InType,TRUE);
	}
}

//�д�������ʱ�������������¼�
TCH_MSG TchCtrlObjHandler(INPUT_EVT_TYPE InType,u8 Idx,TOUCH_INFO *pTouchInfo)
{
	TCH_MSG TchMsg=TM_State_OK;
	u8 TchEvtMsk=GetTouchInfoByIdx(Idx)->OptionsMask;//�¼�����
	u8 ObjID=GetTouchInfoByIdx(Idx)->ObjID;//��ֵ

	switch(InType)//�ж��¼�
	{
		case Input_TchPress:		
			//����ʱͼ��仯
			SameOidTchDisplay(ObjID,InType);
			if(TchEvtMsk&PrsMsk)//��Ҫ����
				TchMsg=gpCurrentPage->TchEvtHandler(ObjID,Tch_Press,pTouchInfo);
			break;
		case Input_TchContinue:
			//����Ҫ����Ƿ񴥷�����Ϊ��Touch�߳������Ѿ������
			TchMsg=gpCurrentPage->TchEvtHandler(ObjID,Tch_Continue,pTouchInfo);
			break;
		case Input_TchRelease:
			//�����ͷŸı䰴ťͼ�겢�����¼�
			SameOidTchDisplay(ObjID,InType);
			if(TchEvtMsk&RelMsk)//��Ҫ����
				TchMsg=gpCurrentPage->TchEvtHandler(ObjID,Tch_Release,pTouchInfo);
			Allow_Touch_Input();
			break;
		case Input_TchReleaseVain:	
			SameOidTchDisplay(ObjID,InType);
			if(TchEvtMsk&ReVMsk)//��Ҫ����
				TchMsg=gpCurrentPage->TchEvtHandler(ObjID,Tch_ReleaseVain,pTouchInfo);
			Allow_Touch_Input();
			break;
	}

	return TchMsg;
}

#ifdef QSYS_FRAME_FULL	
//�д�������ʱ������yesnoѡ���¼�
//IdxΪ���д������򼯺ϵ�����
TCH_MSG YesNoCtrlObjHandler(INPUT_EVT_TYPE InType,u8 Idx)
{
	YES_NO_OBJ *pYesNo=(void *)gCtrlObjPtrBuf[Idx-gpCtrlObjNum->ImgTchNum-gpCtrlObjNum->CharTchNum];
	GUI_REGION DrawRegion;
	TCH_MSG TchMsg=TM_State_OK;
	u8 ObjID=GetTouchInfoByIdx(Idx)->ObjID;//��ֵ

	DrawRegion.x=pYesNo->x;
	DrawRegion.y=pYesNo->y;
	DrawRegion.w=CO_YES_NO_W;
	DrawRegion.h=CO_YES_NO_H;
	DrawRegion.Color=CO_YES_NO_TRAN_COLOR;
			
	switch(InType)//�ж��¼�
	{
		case Input_TchPress:		//����ʱͼ��仯
			Gui_DrawImgArray(gCtrlObj_OnOff,&DrawRegion);
			break;
		case Input_TchRelease://�����ͷŸı䰴ťͼ�겢�����¼�
			pYesNo->DefVal=(bool)!pYesNo->DefVal;
			if(pYesNo->DefVal==FALSE)	Gui_DrawImgArray(gCtrlObj_Off,&DrawRegion);
			else 	Gui_DrawImgArray(gCtrlObj_On,&DrawRegion);
			
			TchMsg=gpCurrentPage->YesNoHandler(ObjID,pYesNo->DefVal);
			Allow_Touch_Input();
			break;
		case Input_TchReleaseVain:	
			if(pYesNo->DefVal==FALSE)	Gui_DrawImgArray(gCtrlObj_Off,&DrawRegion);
			else 	Gui_DrawImgArray(gCtrlObj_On,&DrawRegion);
			
			Allow_Touch_Input();
			break;
	}
	
	return TchMsg;
}

//�д�������ʱ������NumBox�¼�
//IdxΪ���д������򼯺ϵ�����
TCH_MSG NumBoxCtrlObjHandler(INPUT_EVT_TYPE InType,u8 Idx,TOUCH_INFO *pTouchInfo)
{
	NUM_BOX_OBJ *pNumBox=(void *)gCtrlObjPtrBuf[Idx-gpCtrlObjNum->ImgTchNum-gpCtrlObjNum->CharTchNum];

	switch(InType)
	{
		case Input_TchPress:
			break;
		case Input_TchRelease:
			Q_GotoPage(GotoSubPage,"NumCtrlObjPage",0,pNumBox);//����NumBoxPage����
			Allow_Touch_Input();
			break;
		case Input_TchReleaseVain:
			Allow_Touch_Input();
			break;
	}

	
	return 0;
}

//�д�������ʱ������StrOpt�¼�
TCH_MSG StrOptCtrlObjHandler(INPUT_EVT_TYPE InType,u8 Idx,TOUCH_INFO *pTouchInfo)
{
	return 0;
}

TCH_MSG StrInputCtrlObjHandler(INPUT_EVT_TYPE InType,u8 Idx,TOUCH_INFO *pTouchInfo)
{
	return 0;
}
#endif

//������ҳ��ǰ������ҳ�����ʱ����
//Ҫ�����������:
// 1.ҳ��Ķ�̬������
//ps ��鿴������Ҫ��ǿ
void PushPageCtrlObjData(void)
{
	void *p;

	Debug("PushPageCtrlObjData:%d\n\r",GetCurLayerNum());

	//Debug("%d\n\r",sizeof(IMG_TCH_OBJ*)*MAX_DYNAMIC_IMG_KEY_NUM);
	//Debug("%d\n\r",sizeof(CHAR_TCH_OBJ*)*MAX_DYNAMIC_CHAR_KEY_NUM);
	//Debug("%d\n\r",sizeof(REP_IMG_SUFX)*MAX_IMG_KEY_NUM);
	//Debug("%d\n\r",sizeof(u8 *)*MAX_CHAR_KEY_NUM);
	
	p=gPageDataPtrRecord[GetCurLayerNum()]=Q_Mallco(sizeof(void*)*MAX_DYN_CTRL_OBJ_NUM
								+sizeof(REP_IMG_SUFX)*MAX_IMG_KEY_NUM+sizeof(u8 *)*MAX_CHAR_KEY_NUM);
	
	memcpy(p,gCtrlObjPtrBuf,sizeof(void*)*MAX_DYN_CTRL_OBJ_NUM);
	p=(void *)((u32)p+sizeof(void*)*MAX_DYN_CTRL_OBJ_NUM);
	memcpy(p,(void *)gRepImgSufx,sizeof(REP_IMG_SUFX)*MAX_IMG_KEY_NUM);
	p=(void *)((u32)p+sizeof(REP_IMG_SUFX)*MAX_IMG_KEY_NUM);
	memcpy(p,(void *)gRepKeyNameCon,sizeof(u8 *)*MAX_CHAR_KEY_NUM);
}

//����ҳ������
void PopPageCtrlObjData(bool DeleteAll)
{	
	if(DeleteAll)
	{
		u8 i;

		Debug("PopPageCtrlObjData Delete All\n\r");
		for(i=0;i<=MAX_PAGE_LAYER_NUM;i++)
			if(gPageDataPtrRecord[i]!=NULL) 
				Q_Free(gPageDataPtrRecord[i]);
	}
	else if(gPageDataPtrRecord[GetCurLayerNum()]!=NULL)
	{
		void *p=gPageDataPtrRecord[GetCurLayerNum()];

		Debug("PopPageCtrlObjData:%d\n\r",GetCurLayerNum());
		
		memcpy((void *)gCtrlObjPtrBuf,p,sizeof(void*)*MAX_DYN_CTRL_OBJ_NUM);
		p=(void *)((u32)p+sizeof(void*)*MAX_DYN_CTRL_OBJ_NUM);
		memcpy((void *)gRepImgSufx,p,sizeof(REP_IMG_SUFX)*MAX_IMG_KEY_NUM);
		p=(void *)((u32)p+sizeof(REP_IMG_SUFX)*MAX_IMG_KEY_NUM);
		memcpy((void *)gRepKeyNameCon,p,sizeof(u8 *)*MAX_CHAR_KEY_NUM);

		Q_Free(gPageDataPtrRecord[GetCurLayerNum()]);
		gPageDataPtrRecord[GetCurLayerNum()]=NULL;
	}
}

//���ҳ������
void CleanPageCtrlObjData(void)
{
	u8 i;

	Debug("CleanPageCtrlObjData\n\r");
	
	//��տؼ�
	for(i=0;i<MAX_DYN_CTRL_OBJ_NUM;i++) gCtrlObjPtrBuf[i]=NULL;

	//�����ʱ�滻ͼ��ĺ�׺�洢
	for(i=0;i<MAX_IMG_KEY_NUM;i++)
		gRepImgSufx[i]=0;
	for(i=0;i<MAX_CHAR_KEY_NUM;i++)
		gRepKeyNameCon[i]=NULL;
}

//���ǰҳ���µĴ�������
SYS_MSG CurrPageCtrlObjInit(INPUT_EVT_TYPE EventType,int IntParam,void *pInfoParam)
{
	IMG_TCH_OBJ **pDynImgTchCon=(void *)gCtrlObjPtrBuf; //��̬ͼƬ��������
	CHAR_TCH_OBJ **pDynCharTchCon=(void *)&pDynImgTchCon[gpCtrlObjNum->DynImgTchNum];//��̬���ְ�������
	SYS_MSG SysMsg=SM_State_Faile;
	u8 Index;
	
	CO_Debug("%s : %s\n\r",__FUNCTION__,gpCurrentPage->Name);

	//4 	�������ִ�������
	for(Index=0;Index<gpCtrlObjNum->CharTchNum;Index++)
	{
		CharTchDisplay(Index,Input_TchNormal,FALSE);
	}

	//4 	���ƶ�̬���ִ�������
	for(Index=0;Index<gpCtrlObjNum->DynCharTchNum;Index++)
	{//��ʾͼ��
		if(pDynCharTchCon[Index]!=NULL)
			CharTchDisplay(Index,Input_TchNormal,TRUE);
	}

	//4 	����ͼƬ��������
	for(Index=0;Index<gpCtrlObjNum->ImgTchNum;Index++)
	{//��ʾͼ��
		ImgTchDisplay(Index,Input_TchNormal,FALSE);
	}

	//4 	���ƶ�̬ͼƬ��������
	for(Index=0;Index<gpCtrlObjNum->DynImgTchNum;Index++)
	{//��ʾͼ��
		if(pDynImgTchCon[Index]!=NULL)
			ImgTchDisplay(Index,Input_TchNormal,TRUE);
	}

	//�����¼�
	switch(EventType)
	{
		case Input_GotoSubPage://������ҳ��
		case Input_SubPageTranslate://�����µ�ͬ����ҳ��
		case Input_GotoNewPage://������ҳ��
				SysMsg=gpCurrentPage->SysEvtHandler(Sys_TouchSetOk,IntParam,pInfoParam);
			break;
		case Input_SubPageReturn://����ҳ�淵��
				SysMsg=gpCurrentPage->SysEvtHandler(Sys_TouchSetOk_SR,IntParam,pInfoParam);
			break;
	}
	
	CO_Debug("%s end: %s\n\r",__FUNCTION__,gpCurrentPage->Name);
	return SysMsg;
}

#if 1//user api
//���ĵ�ǰ������ĳ����������ʾͼƬ
// 1.ֻ�Ե�ǰҳ����Ч,ת��ҳ�����ҳ���ʧЧ
// 2.ֻ��ָ����׺������ԭ������Դͼ����"MusicN.bmp"
//    ָ��Suffix='T'����ͼ����Դ���"MusicT.bmp"
//���Suffix=0����ָ�ԭʼͼ��
void Q_ChangeImgTchImg(u8 ObjID,u8 Suffix)
{
	OS_DeclareCritical();
	
	if((ObjID=FindImgTchIdx(ObjID))==0xff) return;
	
	if(ObjID>=MAX_IMG_KEY_NUM)
	{
		Debug("%s:ObjID is error!\n\r",__FUNCTION__);
		return;
	}
	
	OS_EnterCritical();
	gRepImgSufx[ObjID]=Suffix;
	OS_ExitCritical();
}

//��Q_ChangeImgTchImg��ԣ���ȡ��ǰ��ͼ���滻��׺ֵ
//����0��ʾ��Ĭ��ֵ
//���򷵻��滻�ĺ�׺
u8 Q_ReadImgTchImg(u8 ObjID)
{
	if((ObjID=FindImgTchIdx(ObjID))==0xff) return 0;
	
	if(ObjID>=MAX_IMG_KEY_NUM)
	{
		Debug("%s:ObjID is error!\n\r",__FUNCTION__);
		return 0;
	}

	return gRepImgSufx[ObjID];
}

//���ĵ�ǰ������ĳ�����ְ�������ʾ����
// 1.ֻ�Ե�ǰҳ����Ч,ת��ҳ�����ҳ���ʧЧ
//���NewName=NULL����ָ�ԭʼ����
void Q_ChangeCharTchName(u8 ObjID,u8 *NewName)
{
	OS_DeclareCritical();
	
	if((ObjID=FindCharTchIdx(ObjID))==0xff) return;
	
	if(ObjID>=MAX_CHAR_KEY_NUM)
	{
		Debug("%s:ObjID is error!\n\r",__FUNCTION__);
		return;
	}
	
	OS_EnterCritical();
	gRepKeyNameCon[ObjID]=NewName;
	OS_ExitCritical();
}

//��Q_ChangeCharTchName��ԣ���ȡ��ǰ��ͼ���滻��׺ֵ
//����NULL��ʾ��Ĭ��ֵ����key������Χ
u8 *Q_ReadCharTchName(u8 ObjID)
{
	if((ObjID=FindCharTchIdx(ObjID))==0xff) return NULL;
	
	if(ObjID>=MAX_CHAR_KEY_NUM)
	{
		Debug("%s:ObjID is error!\n\r",__FUNCTION__);
		return NULL;
	}
	
	return gRepKeyNameCon[ObjID];
}

//���̳���ָ��������ֵ��ͼ������֣������д˼�ֵ�İ�������Ч
void Q_PresentTch(u8 ObjID,TCH_EVT Type)
{
	switch(Type)
	{
		case Tch_Normal:
			SameOidTchDisplay(ObjID,Input_TchNormal);
			break;
		case Tch_Press:		
			SameOidTchDisplay(ObjID,Input_TchPress);
			break;
		case Tch_Release:
			SameOidTchDisplay(ObjID,Input_TchRelease);
			break;
	}	
}

//���������µĶ�̬ͼ�갴��
//Idxָ�������ţ���1��ʼ�����ô���page��DynImgTchNum����ֵ
bool Q_SetDynamicImgTch(u8 Idx,IMG_TCH_OBJ *pTchReg)
{
	IMG_TCH_OBJ **pDynImgTchCon=(void *)gCtrlObjPtrBuf; //��̬ͼƬ��������

	if((Idx==0)||(Idx>gpCtrlObjNum->DynImgTchNum)) return FALSE;
	Idx--;

	pDynImgTchCon[Idx]=pTchReg;

	if(pTchReg!=NULL)
	{
		u8 Num=gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum+Idx;
		gpTouchRegions[Num].x=pDynImgTchCon[Idx]->x;
		gpTouchRegions[Num].y=pDynImgTchCon[Idx]->y;
		gpTouchRegions[Num].w=pDynImgTchCon[Idx]->w;
		gpTouchRegions[Num].h=pDynImgTchCon[Idx]->h;
		gpTouchRegions[Num].ObjID=pDynImgTchCon[Idx]->ObjID;
		gpTouchRegions[Num].Type=COT_DynImg;
		gpTouchRegions[Num].Index=Idx;
		gpTouchRegions[Num].OptionsMask=(u8)(pDynImgTchCon[Idx]->OptionsMask&0xff);
		ImgTchDisplay(Idx,Input_TchNormal,TRUE);
	}
	else
	{
		MemSet(&gpTouchRegions[gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum+Idx],0,sizeof(TOUCH_REGION));
	}

	return FALSE;
}

//���������µĶ�̬���ְ���
//Idxָ�������ţ���1��ʼ�����ô���page��DynCharTchNum����ֵ
bool Q_SetDynamicCharTch(u8 Idx,CHAR_TCH_OBJ *pTchReg)
{
	CHAR_TCH_OBJ **pDynCharTchCon=(void *)&gCtrlObjPtrBuf[gpCtrlObjNum->DynImgTchNum];//��̬���ְ�������
		
	if((Idx==0)||(Idx>gpCtrlObjNum->DynCharTchNum)) return FALSE;
	Idx--;

	pDynCharTchCon[Idx]=pTchReg;

	if(pTchReg!=NULL)
	{
		u8 Num=gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum+gpCtrlObjNum->DynImgTchNum+Idx;
		gpTouchRegions[Num].x=pDynCharTchCon[Idx]->x;
		gpTouchRegions[Num].y=pDynCharTchCon[Idx]->y;
		gpTouchRegions[Num].w=pDynCharTchCon[Idx]->w;
		gpTouchRegions[Num].h=pDynCharTchCon[Idx]->h;
		gpTouchRegions[Num].ObjID=pDynCharTchCon[Idx]->ObjID;
		gpTouchRegions[Num].Type=COT_DynChar;
		gpTouchRegions[Num].Index=Idx;
		gpTouchRegions[Num].OptionsMask=(u8)(pDynCharTchCon[Idx]->OptionsMask&0xff);
		CharTchDisplay(Idx,Input_TchNormal,TRUE);
	}
	else
	{
		MemSet(&gpTouchRegions[gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum+gpCtrlObjNum->DynImgTchNum+Idx],0,sizeof(TOUCH_REGION));
	}
	
	return FALSE;
}

#ifdef QSYS_FRAME_FULL
//����yes noѡ�pYesNoָ����ڴ��ڵ����꺯���󲻿�ע��
//һ�����ã�������ҳ��ʱ�����õ����ڴ�
//���Ե�ҳ�滹����ʱ�����뱣֤���ڴ����
//Idx��1��ʼ
bool Q_SetYesNo(u8 Idx,YES_NO_OBJ *pYesNo)
{
	YES_NO_OBJ **pYesNoCon=(void *)&gCtrlObjPtrBuf[gpCtrlObjNum->DynImgTchNum+gpCtrlObjNum->DynCharTchNum];

	if(Idx>gpCtrlObjNum->YesNoNum) return FALSE;
	Idx--;

	pYesNoCon[Idx]=pYesNo;

	if(pYesNo!=NULL)
	{
		GUI_REGION DrawRegion;
		u8 Num=gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum
			+gpCtrlObjNum->DynImgTchNum+gpCtrlObjNum->DynCharTchNum+Idx;
		gpTouchRegions[Num].x=pYesNo->x;
		gpTouchRegions[Num].y=pYesNo->y;
		gpTouchRegions[Num].w=CO_YES_NO_W;
		gpTouchRegions[Num].h=CO_YES_NO_H;
		gpTouchRegions[Num].ObjID=pYesNo->ObjID;
		gpTouchRegions[Num].Type=COT_YesNo;
		gpTouchRegions[Num].Index=Idx;
		gpTouchRegions[Num].OptionsMask=(u8)(PrsMsk|RelMsk|ReVMsk);
		
		//draw
		DrawRegion.x=pYesNo->x;
		DrawRegion.y=pYesNo->y;
		DrawRegion.w=CO_YES_NO_W;
		DrawRegion.h=CO_YES_NO_H;
		DrawRegion.Color=CO_YES_NO_TRAN_COLOR;
		if(pYesNo->DefVal==FALSE)
			Gui_DrawImgArray(gCtrlObj_Off,&DrawRegion);
		else 
			Gui_DrawImgArray(gCtrlObj_On,&DrawRegion);
	}
	else
	{//��մ��������¼
		MemSet(&gpTouchRegions[gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum
			+gpCtrlObjNum->DynImgTchNum+gpCtrlObjNum->DynCharTchNum+Idx],0,sizeof(TOUCH_REGION));
	}
	
	return FALSE;
}

//����num boxѡ�pNumBoxָ����ڴ��ڵ����꺯���󲻿�ע��
//һ�����ã�������ҳ��ʱ�����õ����ڴ�
//���Ե�ҳ�滹����ʱ�����뱣֤���ڴ����
//Idx��1��ʼ
bool Q_SetNumCtrlObj(u8 Idx,NUM_BOX_OBJ *pNumBox)
{
	NUM_BOX_OBJ **pNumBoxCon=(void *)&gCtrlObjPtrBuf[gpCtrlObjNum->DynImgTchNum+gpCtrlObjNum->DynCharTchNum
																								+gpCtrlObjNum->YesNoNum];
	if(Idx>gpCtrlObjNum->NumBoxNum) return FALSE;
	Idx--;
	
	pNumBoxCon[Idx]=pNumBox;

	if(pNumBox!=NULL)
	{
		GUI_REGION DrawRegion;
		u8 Num=gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum
			+gpCtrlObjNum->DynImgTchNum+gpCtrlObjNum->DynCharTchNum
			+gpCtrlObjNum->YesNoNum+Idx;
		gpTouchRegions[Num].x=pNumBox->x;
		gpTouchRegions[Num].y=pNumBox->y;
		gpTouchRegions[Num].w=pNumBox->w;
		gpTouchRegions[Num].h=CO_NUM_H;
		gpTouchRegions[Num].ObjID=pNumBox->ObjID;
		gpTouchRegions[Num].Type=COT_NumBox;
		gpTouchRegions[Num].Index=Idx;
		gpTouchRegions[Num].OptionsMask=(u8)(PrsMsk|RelMsk|ReVMsk);
		
		{//draw
			u8 NumStr[32];
			DrawRegion.x=pNumBox->x+CO_NUM_ARROW_W-CO_NUM_FRAME_W;
			DrawRegion.y=pNumBox->y;
			DrawRegion.w=CO_NUM_FRAME_W;
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
			Gui_DrawImgArray(gCtrlObj_NumLeft,&DrawRegion);//��߿�

			DrawRegion.x=pNumBox->x+CO_NUM_ARROW_W;
			DrawRegion.y=pNumBox->y;
			DrawRegion.w=CO_NUM_MIDDLE_W;
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
			Gui_FillImgArray_H(gCtrlObj_NumMiddle,pNumBox->w-(CO_NUM_ARROW_W<<1),&DrawRegion);	

			DrawRegion.x=pNumBox->x+pNumBox->w-CO_NUM_ARROW_W;
			DrawRegion.y=pNumBox->y;
			DrawRegion.w=CO_NUM_FRAME_W;
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_TRAN_COLOR;
			Gui_DrawImgArray(gCtrlObj_NumRight,&DrawRegion);//�ұ߿�
			
			sprintf((void *)NumStr,"%d",pNumBox->Value);
			DrawRegion.x=pNumBox->x+((pNumBox->w-strlen((void *)NumStr)*CO_NUM_FONT_W)>>1);
			DrawRegion.y=pNumBox->y+3;
			DrawRegion.w=pNumBox->w-(CO_NUM_ARROW_W<<1);
			DrawRegion.h=CO_NUM_H;
			DrawRegion.Color=CO_NUM_FONT_COLOR;
			DrawRegion.Space=CO_NUM_FONT_SPACE;
			Gui_DrawFont(CO_NUM_FONT_STYLE,NumStr,&DrawRegion);
		}
	}
	else
	{//��մ��������¼
		MemSet(&gpTouchRegions[gpCtrlObjNum->ImgTchNum+gpCtrlObjNum->CharTchNum
			+gpCtrlObjNum->DynImgTchNum+gpCtrlObjNum->DynCharTchNum
			+gpCtrlObjNum->YesNoNum+Idx],0,sizeof(TOUCH_REGION));
	}
	
	return FALSE;
}


#endif

#endif



