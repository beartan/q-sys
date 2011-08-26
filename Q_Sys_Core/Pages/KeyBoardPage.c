#include "User.h"
#include "KeyBoardPage.h"
#include "PinYinForKeyBoardPage.h"

//-------------------------------------------�궨��&��������-------------------------------------------------------
#define KB_WHITE				    (FatColor(0xFFFFFF))
#define KB_BLACK					(FatColor(0x000000))
#define	KB_ORANGE					(FatColor(0xE47833))

#define	KB_TEXTBOXCROSS_LINE			5		 //�޻س�ȫӢ�������һ�η�ҳ����������
#define	KB_TEXTBOX_POS_X				7        //�ı���x����
#define	KB_TEXTBOX_POS_Y				5		 //�ı���y����
#define	KB_TEXTBOX_W					213		 //�ı�����
#define	KB_TEXTBOX_H					110		 //�ı���߶�
#define	KB_TEXTBOX_CHAR_SPACE			1		 //�ı����ּ��
#define	KB_TEXTBOX_ROW_SPACE			1		 //�ı����м��
#define	KB_TEXTBOXCHAR_COLOUR			KB_BLACK    //�ı���������ɫ
#define	KB_TEXTBOXBG_COLOUR				KB_WHITE	 //�ı��򱳾���ɫ
#define	KB_CROSSBAR_COLOUR				KB_ORANGE     //��������ɫ
#define KB_CROSSBOT_SIZE				14	 	 //��������ť��С
#define	KB_TEXTBOXCROSS_NUM				(KB_TEXTBOX_W/(8+KB_TEXTBOX_CHAR_SPACE)*KB_TEXTBOXCROSS_LINE)
#define	KB_CROSSBAR_INI_X				(KB_TEXTBOX_POS_X+KB_TEXTBOX_W+5)						
#define	KB_CROSSBAR_INI_Y				(KB_TEXTBOX_POS_Y+KB_CROSSBOT_SIZE)
#define	KB_CROSSBAR_INI_W				(KB_CROSSBOT_SIZE-2)						
#define	KB_CROSSBAR_INI_H				(KB_TEXTBOX_H-KB_CROSSBOT_SIZE*2+1)
#define	KB_PINYINBUF_MAX  	 	 	 	6      	//ƴ�����������ɵ��ַ���
#define	KB_PINYINBOXCHAR_COLOUR			KB_BLACK   //ƴ����ɫ
#define	KB_PINYINBOXCHARBG_COLOUR		KB_WHITE   //ƴ��������ɫ
#define	KB_MAYBEBOXCHAR_COLOUR			KB_BLACK	//���ƺ�����ɫ
#define	KB_MAYBEBOXCHARBG_COLOUR		KB_WHITE   //���ƺ��ֱ�����ɫ
#define	KB_PINYINBOX_POS_X				1   	//ƴ����x����
#define	KB_PINYINBOX_POS_Y				122		//ƴ����y����
#define	KB_PINYINBOX_W					49		//ƴ������
#define	KB_PINYINBOX_H					16		//ƴ����߶�
#define	KB_PINYINBOX_CHAR_SPACE	 		0		//ƴ�����ּ��
#define	KB_PINYINBOX_ROW_SPACE		 	0		//ƴ�����м��
#define	KB_MAYBEBOX_POS_X				51  	//���ƺ����ӿ�x����
#define	KB_MAYBEBOX_POS_Y				122		//���ƺ����ӿ�y����
#define	KB_MAYBEBOX_W					187		//���ƺ����ӿ���
#define	KB_MAYBEBOX_H					34		//���ƺ����ӿ�߶�
#define	KB_MAYBEBOX_CHAR_SPACE	 	 	3		//���ƺ����ӿ��ּ��
#define	KB_MAYBEBOX_ROW_SPACE		 	2		//���ƺ����ӿ��м��
#define KB_NUM_MODE	   					0x1		//����ģʽ
#define KB_EN_MODE	   					0x2		//Ӣ��ģʽ
#define KB_PINYIN_MODE					0x4		//ƴ��ģʽ
#define KB_CAP_MODE						0x10	//��д��ģʽ
#define	KB_TEMPBUF_MAX  	 	 	 	10      //�����С 
#define KB_DKTIME						5000    //�����ж϶����ʱ��

typedef enum{
	//���̰���
	KB_PRESS_1=USER_KEY_VALUE_START,  
	KB_PRESS_2,  KB_PRESS_3,  KB_PRESS_4,  KB_PRESS_5,  KB_PRESS_6,  
	KB_PRESS_7,  KB_PRESS_8,  KB_PRESS_9,  KB_PRESS_10, KB_PRESS_11, 
	KB_PRESS_12, KB_PRESS_13, KB_PRESS_14, KB_PRESS_15, KB_PRESS_16, 
	KB_PRESS_17, KB_PRESS_18, KB_PRESS_19, KB_PRESS_20, KB_PRESS_21, 
	//��ҳ��
	KB_PRESS_22,	KB_PRESS_23,
	//ƴ��ѡ������
	KB_PY_SELECT,
	//ƴ����ҳ
	KB_PRESS_24,	KB_PRESS_25
}KeyBoardPage_KEY_NAME;
//------------------------��������-------------------------------
//ע:�ɶ���������ã�����ֻ�ı���������״̬�ĺ�������Ϊ˽�к���
//	 �ɶ���������ã����ҽ��ı����������״̬�ĺ�������Ϊ���ݺ���
//	 �ӿں���������������ʸö����Ψһ��ʽ
//---------------------------------------------------------------
//�ı��������������
typedef struct{	
	//��������
	u8  *elem;      					//������
	u16	OUTBUF_SIZE;					//�����������С
	s32 tailnextnext;	     		    //���������һ��Ԫ�ص�����һ��Ԫ�ص�λ��
	s32 ShowBegin;               		//�ӵ�ShowBegin���ֽڿ�ʼ��ʾ
	u32 TotalPage;						//��ҳ�� 
	u32 CurPage;                 		//��ǰ�ǵڼ�ҳ
	GUI_REGION TextBoxRegion;    		//�ı���ʾ����
	GUI_REGION ScrollbarRegion;  		//��������ʾ����
	//˽�к���
	void (*ShowFromPos) (void);	    	//��ShowBeginλ�ÿ�ʼ���ı�������ʾ�ַ���
	void (*RefuCrossBar)(void);			//ˢ�¹�����
	//���ݺ��� ��
	//�ӿں���
	void (*Init)(u8 *p,s32 bufsizemsg); //�����ʼ��
	bool (*InsTail)(u8 inchar);			//��β�������ַ�
	bool (*InsTailChain)(u8 *str);		//��β�������ַ���
	void (*ClrTail)(void);				//ɾ��β�����ַ�
	void (*ClearAll)(void);				//ɾ�������ַ�
	void (*UpPage)(void);				//�Ϸ�ҳ
	void (*DownPage)(void);				//�·�ҳ
	void (*ShowPrepa)(u8 *str);			//��β����ʾԤ���ַ���
	void (*EndEdit)(void);				//�����༭
}KB_TextBoxDef;

//ƴ���������������
typedef struct{
	u8  elem[KB_PINYINBUF_MAX+1];       	//������
	u8	tailnext;					 	//���������һ��Ԫ�ص���һ��Ԫ�ص�λ��
	const u8 *pmbstr;	 				//ָ�������ַ���
	u8 curbase;	 						//�����ַ�����ʾ����λ��
	u8 strnum; 							//�����ַ����ĳ���
	GUI_REGION PinYinRegion;    		//ƴ����ʾ����
	GUI_REGION MayBeRegion;  			//���ƺ�������ʾ����
	//˽�к���
	void (*ClrPinYinShow)(void);	 	//���ƴ����ʾ
	void (*ClrMayBeShow)(void);		 	//������ƺ�����ʾ
	void (*ShowPinYinBox)(void);	 	//��ʾ������������
	void (*ShowMayBeFromPos)(void);	 	//��curbaseλ�ÿ�ʼ�����ƺ��ֿ�����ʾ�ַ���
	void (*ClrMayBe)(void);			 	//��������ַ���
	//���ݺ��� 	
	void (*Upload)(KB_TextBoxDef *ptextbox,u8 order);//��ѡ�еĺ����ϴ����ı������
	void (*UploadHalf)(KB_TextBoxDef *ptextbox,u8 order);//��ѡ�еİ���ַ��ϴ����ı������	
	//�ӿں���
	void (*Init)(void);   			 	//�����ʼ��
	void (*Clr)(void);   		     	//�������
	void (*LinkMayBe)(const u8* str);	//����ȫ�ǵ������ַ���
	void (*LinkMayBeHalf)(const u8* str);//���Ӱ�ǵ������ַ���
	bool (*InsTailChain)(u8 *str);	 	//��β�������ַ���
	void (*ClrTail)(void);			 	//ɾ��β�����ַ�
	void (*NextMayBePage)(void);	 	//�·�ҳ����ȫ���ַ���
	void (*NextMayBePageHalf)(void);	 //�·�ҳ���ư���ַ���
	void (*PreMayBePage)(void);	 		//�Ϸ�ҳ����ȫ���ַ���
	void (*PreMayBePageHalf)(void);	 	//�Ϸ�ҳ���ư���ַ���

	void (*PleaseUpload)(KB_TextBoxDef *ptextbox,u8 order);//�������ѡ�еĺ����ϴ����ı������
	void (*PleaseUploadHalf)(KB_TextBoxDef *ptextbox,u8 order);//�������ѡ�еİ���ַ��ϴ����ı������
	bool (*IfEditPinYin)(void);			//�Ƿ��ڱ༭ƴ��
	void (*ShowPrepa)(u8 *str);			//��β����ʾԤ���ַ���				
}KB_PinYinBoxDef;

//��ʱ������������������
typedef struct{
	u8  elem[KB_TEMPBUF_MAX+1];       		//������
	u8  decodebuf[KB_TEMPBUF_MAX+1];       //���뻺����
	u8	tailnext;				   		//���������һ��Ԫ�ص���һ��Ԫ�ص�λ��
	//˽�к���
	void (*Decode)(void);   	   		//�Զ��󻺴������н��룬�Ӷ����ֳ�����˫�����
	//���ݺ���
	void (*UploadText)(KB_TextBoxDef *ptextbox);//����ʱ���������ַ����ϴ����ı������													  
	void (*UploadPinYin)(KB_PinYinBoxDef *ppinyinbox);//����ʱ���������ַ����ϴ���ƴ�������			
	//�ӿں���
	void (*Init)(void);   		   		//��ʼ������
	void (*Clr)(void);   		   		//��ն���
	bool (*InsTail)(u8 inchar);	   		//�ڶ���β�������ַ�	
	void (*PleaseUploadText)(KB_TextBoxDef *ptextbox);//���������ʱ���������ַ����ϴ����ı������													  
	void (*PleaseUploadPinYin)(KB_PinYinBoxDef *ppinyinbox);//���������ʱ���������ַ����ϴ���ƴ�������	
}KB_TempBufDef;

//ģʽ�������������
typedef struct{
	u8  ModeFlag;   					 //ģʽ��־
	bool IfOffPinYin;					 //�Ƿ�ر�ƴ��ģʽ��־
	bool IfOffNum;						 //�Ƿ�ر�����ģʽ��־
	bool IfOffEn;						 //�Ƿ�ر�Ӣ��ģʽ��־
	bool IfOffPunc;						 //�Ƿ�رձ��
	//˽�к���
	void (*ShowMode)(void);			  	 //ģʽ����ʾ��ǰģʽ
	//���ݺ���
	void (*ChangeModeAct)(KB_PinYinBoxDef *ppinyinbox,u8 newmode);	//ִ��ģʽ�л�
	//�ӿں���
	void (*Init)(s32 modemsg);   		  //��ʼ������
	u8 (*GetMode)(void);				  //��õ�ǰģʽ
	void (*Capital)(void);				  //�ô�д
	void (*UnCapital)(void);			  //ȡ����д
	bool (*IfCapital)(void);			  //�жϵ�ǰ�Ƿ��д
	void (*PleaseChangeMode)(KB_PinYinBoxDef *ppinyinbox,u8 newmode); //����������л�����ģʽ	
}KB_ModeBoxDef;

//------------ȫ�ֱ�����������-------------			
typedef struct{
   KB_TextBoxDef   KB_TextBox;   //�ı������
   KB_TempBufDef   KB_TempBuf;   //��ʱ�������
   KB_PinYinBoxDef KB_PinYinBox; //ƴ�������
   KB_ModeBoxDef   KB_ModeBox;   //ģʽ����
}KEYBOARD_PAGE_VARS;

//---------------------------------------------��������-----------------------------------------------------------
void KB_TextBox_Init(u8 *p,s32 bufsizemsg);   			    //��ʼ���ı������
void KB_TextBox_ShowFromPos(void);	    					//�ı�������ָ��λ�ÿ�ʼ����ʾ�ַ���
void KB_TextBox_RefuCrossBar(void);						//�ı������ˢ�¹�����
bool KB_TextBox_InsTail(u8 inchar);						//���ı������β�������ַ�
bool KB_TextBox_InsTailChain(u8 *str);						//���ı���β����������ַ���
void KB_TextBox_ClrTail(void);								//ɾ���ı������β�����ַ�
void KB_TextBox_ClearAll(void);								//ɾ���ı�������������ַ�
void KB_TextBox_UpPage(void);								//�Ϸ�ҳ�ı������
void KB_TextBox_DownPage(void);							//�·�ҳ�ı������
void KB_TextBox_ShowPrepa(u8 *str);						//��β����ʾԤ���ַ���
void KB_TextBox_EndEdit(void);								//�����༭
void KB_TempBuf_Init(void);   		   						//��ʼ����ʱ����������
void KB_TempBuf_Decode(void);   	   						//�Ի��������н��룬�Ӷ����ֳ�����˫�����
void KB_TempBuf_Clr(void);   		   						//�����ʱ����������
bool KB_TempBuf_InsTail(u8 inchar);	   					//����ʱ������β�������ַ�
void KB_TempBuf_UploadText(KB_TextBoxDef *ptextbox);			//��ʱ����������ʱ���������ַ����ϴ����ı������
void KB_TempBuf_UploadPinYin(KB_PinYinBoxDef *ppinyinbox);	//��ʱ����������ʱ���������ַ����ϴ����ı������
void KB_TempBuf_PleaseUploadText(KB_TextBoxDef *ptextbox);	//���������ʱ���������ַ����ϴ����ı������													  
void KB_TempBuf_PleaseUploadPinYin(KB_PinYinBoxDef *ppinyinbox);//���������ʱ���������ַ����ϴ���ƴ�������
void KB_PinYinBox_ClrPinYinShow(void);		     			//ƴ�������������ƺ�����ʾ
void KB_PinYinBox_ClrMayBeShow(void);		     			//ƴ�������������ƺ�����ʾ
void KB_PinYinBox_ShowPinYinBox(void);	 					//ƴ���������ʾƴ��
void KB_PinYinBox_ShowMayBeFromPos(void);	 				//ƴ��������curbaseλ�ÿ�ʼ�����ƺ��ֿ�����ʾ�ַ���
void KB_PinYinBox_ClrMayBe(void);			 				//ƴ���������������ַ���
void KB_PinYinBox_LinkMayBe(const u8* str);				//ƴ�������ȫ���ַ����ӵ������ַ���
void KB_PinYinBox_LinkMayBeHalf(const u8* str);			//ƴ������󽫰���ַ����ӵ������ַ���
void KB_PinYinBox_NextMayBePage(void);	 					//�·�ҳ����ȫ���ַ���
void KB_PinYinBox_NextMayBePageHalf(void);	 					//�·�ҳ����ȫ���ַ���
void KB_PinYinBox_PreMayBePage(void);	 					//�Ϸ�ҳ���ư���ַ���
void KB_PinYinBox_PreMayBePageHalf(void);	 					//�Ϸ�ҳ���ư���ַ���
void KB_PinYinBox_Init(void);   			 				//��ʼ��ƴ�������
void KB_PinYinBox_Clr(void);   		     				//���ƴ�������
bool KB_PinYinBox_InsTailChain(u8 *str);	 				//��ƴ�������β�������ַ���
void KB_PinYinBox_ClrTail(void);			 				//ɾ��ƴ�������β�����ַ�
bool KB_PinYinBox_IfEditPinYin(void);						//�Ƿ��ڱ༭ƴ��		
void KB_PinYinBox_Upload(KB_TextBoxDef *ptextbox,u8 order);	//ƴ�������ѡ�е�ȫ���ַ��ϴ����ı������
void KB_PinYinBox_UploadHalf(KB_TextBoxDef *ptextbox,u8 order);//ƴ�������ѡ�еİ���ַ��ϴ����ı������
void KB_PinYinBox_PleaseUpload(KB_TextBoxDef *ptextbox,u8 order);//�������ѡ�е�ȫ���ַ��ϴ����ı������
void KB_PinYinBox_PleaseUploadHalf(KB_TextBoxDef *ptextbox,u8 order);//�������ѡ�еİ���ַ��ϴ����ı������
void KB_PinYinBox_ShowPrepa(u8 *str);						//��β����ʾԤ���ַ���
void KB_ModeBox_ShowMode(void);			  				//ģʽ����ʾ��ǰģʽ
void KB_ModeBox_Init(s32 modemsg);   		   	  				    //�����ʼ��
void KB_ModeBox_PleaseChangeMode(KB_PinYinBoxDef *ppinyinbox,u8 newmode); //�л�����ģʽ
u8 	 KB_ModeBox_GetMode(void);				  				//��õ�ǰģʽ
void KB_ModeBox_ChangeModeAct(KB_PinYinBoxDef *ppinyinbox,u8 newmode);//ִ��ģʽ�л�
void KB_ModeBox_Capital(void);				  				//�ô�д
void KB_ModeBox_UnCapital(void);			 			 	//ȡ����д
bool KB_ModeBox_IfCapital(void);			  				//�жϵ�ǰ�Ƿ��д
static void KB_DrawBG(void);								//������
static void KB_DrawBlock(void);							//������
static void KB_DrawPress(u8 xx,u8 yy);							//ѡ����ʾ
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam);
static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam);
static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo);
//---------------------------------------------ҳ����Դ����-----------------------------------------------------------

static const IMG_TCH_OBJ ImgTchRegCon[]={
	{"\n ����",KB_PRESS_1,			RelMsk,0,157,39,31,0,0,"Comma",FatColor(NO_TRANS)},	
	{"\n ���",KB_PRESS_6,			RelMsk,0,190,39,31,0,0,"Period",FatColor(NO_TRANS)},
	{"\nʡ�Ժ�",KB_PRESS_11,		RelMsk,0,223,39,31,0,0,"Points",FatColor(NO_TRANS)},
	{"\n��̾��",KB_PRESS_16,		RelMsk,0,256,39,31,0,0,"Excla",FatColor(NO_TRANS)},
	{"\n �ʺ�",KB_PRESS_21,		RelMsk,0,289,39,31,0,0,"Ask",FatColor(NO_TRANS)},

	{"\n 1",KB_PRESS_2,			RelMsk,40,157,52,40,0,0,"1",FatColor(NO_TRANS)},
	{"\n 2 abc",KB_PRESS_3,		RelMsk,93,157,52,40,0,0,"2",FatColor(NO_TRANS)},
	{"\n 3 def",KB_PRESS_4,		RelMsk,146,157,52,40,0,0,"3",FatColor(NO_TRANS)},
	{"\n ɾ��",KB_PRESS_5,			RelMsk,199,157,40,40,0,0,"Delete",FatColor(NO_TRANS)},
	
	{"\n 4 ghi",KB_PRESS_7,		RelMsk,40,198,52,40,0,0,"4",FatColor(NO_TRANS)},
	{"\n 5 jkl",KB_PRESS_8,		RelMsk,93,198,52,40,0,0,"5",FatColor(NO_TRANS)},
	{"\n 6 mno",KB_PRESS_9,		RelMsk,146,198,52,40,0,0,"6",FatColor(NO_TRANS)},
	{"\n ����\n ģʽ",KB_PRESS_10,	RelMsk,199,198,40,40,0,0,"123",FatColor(NO_TRANS)},

	{"\n 7 pqrs",KB_PRESS_12,		RelMsk,40,239,52,40,0,0,"7",FatColor(NO_TRANS)},
	{"\n 8 tuv",KB_PRESS_13,		RelMsk,93,239,52,40,0,0,"8",FatColor(NO_TRANS)},
	{"\n 9 wxyz",KB_PRESS_14,		RelMsk,146,239,52,40,0,0,"9",FatColor(NO_TRANS)},
	{"\n Ӣ��\n ģʽ",KB_PRESS_15,	RelMsk,199,239,40,40,0,0,"abc",FatColor(NO_TRANS)},

	{"\n ���",KB_PRESS_17,		RelMsk,40,280,52,40,0,0,"Ok",FatColor(NO_TRANS)},
	{"\n 0 �ո�",KB_PRESS_18,		RelMsk,93,280,52,40,0,0,"0",FatColor(NO_TRANS)},
	{"\n �س�",KB_PRESS_19,		RelMsk,146,280,52,40,0,0,"Enter",FatColor(NO_TRANS)},
	{"\n ƴ��\n ģʽ",KB_PRESS_20,	RelMsk,199,280,40,40,0,0,"PinYin",FatColor(NO_TRANS)},

	{"��",KB_PRESS_22,RelMsk,KB_CROSSBAR_INI_X,KB_CROSSBAR_INI_Y-KB_CROSSBOT_SIZE-3,14,14,0,0,"UpArrow",FatColor(NO_TRANS)},
	{"��",KB_PRESS_23,RelMsk,KB_CROSSBAR_INI_X-1,KB_CROSSBAR_INI_Y+KB_CROSSBAR_INI_H+3,14,14,0,0,"DownArrow",FatColor(NO_TRANS)},

	{">>",KB_PRESS_24,RelMsk,KB_MAYBEBOX_POS_X-25,KB_MAYBEBOX_POS_Y+17,24,18,0,0,"MBDownArrow",FatColor(NO_TRANS)},
	{"<<",KB_PRESS_25,RelMsk,KB_MAYBEBOX_POS_X-50,KB_MAYBEBOX_POS_Y+17,24,18,0,0,"MBUPArrow",FatColor(NO_TRANS)},

	{"",KB_PY_SELECT,RelMsk|PrsMsk,KB_MAYBEBOX_POS_X,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_W,KB_MAYBEBOX_H,0,0,"",FatColor(NO_TRANS)}
};

//����ҳ�����Ӧ�õ����Լ�
const PAGE_ATTRIBUTE KeyBoardPage={
	"KeyBoardPage",
	"Author:ChenYu","keyboard",
	NORMAL_PAGE,
	0,
	{
		sizeof(ImgTchRegCon)/sizeof(IMG_TCH_OBJ), //size of touch region array
		0,//sizeof(CharTchRegCon)/sizeof(CHAR_TCH_OBJ), //size of touch region array,
	},
	ImgTchRegCon, 
	0,
	SystemEventHandler,
	PeripheralsHandler,
	Bit(Perip_Timer)|Bit(Perip_UartInput),
	TouchEventHandler,

};

//---------------------------------------ȫ�ֱ���ָ��----------------------------------------------
static KEYBOARD_PAGE_VARS *gVars;					
//-------------------------------------����Ҫ������ϵͳ����----------------------------------------
static SYS_MSG SystemEventHandler(SYS_EVT SysEvent ,int IntParam, void *pSysParam){
	switch(SysEvent){
		case Sys_PreGotoPage:
			break;
		case Sys_PageInit:		
			KB_DrawBG();//��ҳ�汳��			
			gVars=(KEYBOARD_PAGE_VARS *)Q_PageMallco(sizeof(KEYBOARD_PAGE_VARS));//����ռ�				
			 if(gVars==0){
				Q_ErrorStopScreen("gVars malloc fail !\n\r");
			 }
			MemSet(gVars,0,sizeof(KEYBOARD_PAGE_VARS));
			//��ʼ�����ж���
			gVars->KB_TextBox.Init=KB_TextBox_Init;
			gVars->KB_PinYinBox.Init=KB_PinYinBox_Init;
			gVars->KB_ModeBox.Init=KB_ModeBox_Init;
			gVars->KB_TempBuf.Init=KB_TempBuf_Init;
			gVars->KB_TextBox.Init(pSysParam,IntParam);			
			gVars->KB_ModeBox.Init(IntParam);
			gVars->KB_PinYinBox.Init();
			gVars->KB_TempBuf.Init();			
			break;
		case Sys_SubPageReturn:	//�������ҳ�淵��,�Ͳ��ᴥ��Sys_Page_Init�¼�,����Sys_SubPage_Return
			break;

		case Sys_PageClean:
			Q_PageFree(gVars);
			break;
		default:
			Debug("%s SystemEventHandler:This System Event Handler case unfinish! SysEvent:%d\n\r",Q_GetCurrPageName(),SysEvent);
			//while(1);
	}
	return 0;
}
static SYS_MSG PeripheralsHandler(PERIP_EVT PeripEvent, int IntParam, void *pParam)
{
	u8 mainmode;
	switch(PeripEvent)
	{
		case Perip_Timer:
		{	 //��Ӣ��ģʽ�����ı������ַ�
			mainmode=gVars->KB_ModeBox.GetMode() & 0x0f;
			switch(mainmode){
				case KB_EN_MODE:
				   gVars->KB_TempBuf.PleaseUploadText(&gVars->KB_TextBox);
				   break;
				case KB_PINYIN_MODE:
				   gVars->KB_TempBuf.PleaseUploadPinYin(&gVars->KB_PinYinBox);
				   break;
			}							
		}break;
		case Perip_UartInput:
		{
			gVars->KB_TextBox.ClearAll();
			gVars->KB_TextBox.InsTailChain((u8 *)pParam);
		}break;
	}

	return 0;
}
//��ʹ���߰��±�ҳTouchRegionSet�ﶨ��İ���ʱ���ᴥ�����������Ķ�Ӧ�¼�
static TCH_MSG TouchEventHandler(u8 Key,TCH_EVT InEvent , TOUCH_INFO *pTouchInfo){
	u8 mainmode;

	mainmode = gVars->KB_ModeBox.GetMode() & 0xf;

	if(mainmode&KB_NUM_MODE){
		switch(Key){
				case KB_PRESS_1:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail(',');
					break;			
				case KB_PRESS_2:gVars->KB_TextBox.InsTail('1');break;									
				case KB_PRESS_3:gVars->KB_TextBox.InsTail('2');break;							
				case KB_PRESS_4:gVars->KB_TextBox.InsTail('3');break;			 					 
				case KB_PRESS_5:gVars->KB_TextBox.ClrTail();   break;									
				case KB_PRESS_6:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail('.');
					break;		
				case KB_PRESS_7:gVars->KB_TextBox.InsTail('4');break;				 				   	
				case KB_PRESS_8:gVars->KB_TextBox.InsTail('5');break;			 							
				case KB_PRESS_9:gVars->KB_TextBox.InsTail('6');break;				 					
				case KB_PRESS_10:break;			
				case KB_PRESS_11:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("...");
					break;				
				case KB_PRESS_12:gVars->KB_TextBox.InsTail('7');break;			 						 
				case KB_PRESS_13:gVars->KB_TextBox.InsTail('8');break;	
				case KB_PRESS_14:gVars->KB_TextBox.InsTail('9');break;	
				case KB_PRESS_15:gVars->KB_ModeBox.PleaseChangeMode(&gVars->KB_PinYinBox,KB_EN_MODE);break;
				case KB_PRESS_16:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail('!');
					break;	
				case KB_PRESS_17:gVars->KB_TextBox.EndEdit();break;
				case KB_PRESS_18:gVars->KB_TextBox.InsTail('0');break;	
				case KB_PRESS_19:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("\r\n");
					break;
				case KB_PRESS_20:gVars->KB_ModeBox.PleaseChangeMode(&gVars->KB_PinYinBox,KB_PINYIN_MODE);break;
				case KB_PRESS_21:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail('?');
					break;			
				case KB_PRESS_22:gVars->KB_TextBox.UpPage();    break;
				case KB_PRESS_23:gVars->KB_TextBox.DownPage();  break;
	    }	
	}else if(mainmode&KB_EN_MODE){
		switch(Key){
				case KB_PRESS_1:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail(',');
					break;		
				case KB_PRESS_2:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_PinYinBox.LinkMayBeHalf (":@;.+-*/%=#$&()<>{}~");
					break;
				case KB_PRESS_3:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('A');
					else
						gVars->KB_TempBuf.InsTail('a');
				}break;		
				case KB_PRESS_4:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('D');
					else
						gVars->KB_TempBuf.InsTail('d');
				}break;			
				case KB_PRESS_5:
					if(gVars->KB_PinYinBox.IfEditPinYin()){
					 	gVars->KB_PinYinBox.ClrTail();						
						break;								 	
					}else{
						gVars->KB_TextBox.ClrTail(); 													
						break;
					}		
				case KB_PRESS_6:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail('.');
					break;		
				case KB_PRESS_7:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('G');
					else
						gVars->KB_TempBuf.InsTail('g');
				}break;				
				case KB_PRESS_8:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('J');
					else
						gVars->KB_TempBuf.InsTail('j');
				}break;			
				case KB_PRESS_9:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('M');
					else
						gVars->KB_TempBuf.InsTail('m');
				}break;			
				case KB_PRESS_10:gVars->KB_ModeBox.PleaseChangeMode(&gVars->KB_PinYinBox,KB_NUM_MODE);break;
				case KB_PRESS_11:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("...");
					break;	
				case KB_PRESS_12:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('P');
					else
						gVars->KB_TempBuf.InsTail('p');
				}break;				
				case KB_PRESS_13:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('T');
					else
						gVars->KB_TempBuf.InsTail('t');
				}break;				
				case KB_PRESS_14:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_TempBuf.InsTail('W');
					else
						gVars->KB_TempBuf.InsTail('w');
				}break;				
				case KB_PRESS_15:{
					if(gVars->KB_ModeBox.IfCapital())
						gVars->KB_ModeBox.UnCapital();
					else
						gVars->KB_ModeBox.Capital();
				}break;
				case KB_PRESS_16:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail('!');
					break;		
				case KB_PRESS_17:gVars->KB_TextBox.EndEdit();break;
				case KB_PRESS_18:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail(' ');
					break;		
				case KB_PRESS_19:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("\r\n");
					break;
				case KB_PRESS_20:gVars->KB_ModeBox.PleaseChangeMode(&gVars->KB_PinYinBox,KB_PINYIN_MODE);break;
				case KB_PRESS_21:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTail('?');
					break;		
				case KB_PRESS_22:gVars->KB_TextBox.UpPage();    break;
				case KB_PRESS_23:gVars->KB_TextBox.DownPage();  break;
				case KB_PY_SELECT:{
						static u8 i=0,j=0;
						Debug("i=%d j=%d \r\n",i,j);
						if(gVars->KB_PinYinBox.IfEditPinYin()==FALSE)
							break;						
						if( InEvent!=Tch_Release){
							i=(pTouchInfo->x-KB_MAYBEBOX_POS_X)/19;
							j=(pTouchInfo->y-KB_MAYBEBOX_POS_Y)/18;
							if(gVars->KB_PinYinBox.curbase+(i+10*j)<gVars->KB_PinYinBox.strnum)
								KB_DrawPress(KB_MAYBEBOX_POS_X+19*i,KB_MAYBEBOX_POS_Y+18*j);
							else
								break;	
						}										
						else{	
							gVars->KB_PinYinBox.PleaseUploadHalf(&gVars->KB_TextBox,i+10*j);
							i=0;
							j=0;
						}
				}break;
				case KB_PRESS_24:gVars->KB_PinYinBox.NextMayBePageHalf();break;
				case KB_PRESS_25:gVars->KB_PinYinBox.PreMayBePageHalf();break;		  
		}
	}else if(mainmode&KB_PINYIN_MODE){
		switch(Key){
				case KB_PRESS_1:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("��");
					break;
				case KB_PRESS_2:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_PinYinBox.LinkMayBe ("���������������������z�{�ۣݣ������������������£�����ܡݣ����ڡۡ֡ԡ١�������������������������������ߣ��D���");
					break;
				case KB_PRESS_3:gVars->KB_TempBuf.InsTail('a');break;		
				case KB_PRESS_4:gVars->KB_TempBuf.InsTail('d');break;		
				case KB_PRESS_5:{
					if(gVars->KB_PinYinBox.IfEditPinYin()){
					 	gVars->KB_PinYinBox.ClrTail();						
						break;
					}else{
						gVars->KB_TextBox.ClrTail(); 													
						break;
					}	
				}																			 
				case KB_PRESS_6:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("��");
					break;
				case KB_PRESS_7:gVars->KB_TempBuf.InsTail('g');break;		
				case KB_PRESS_8:gVars->KB_TempBuf.InsTail('j');break;		
				case KB_PRESS_9:gVars->KB_TempBuf.InsTail('m');break;		
				case KB_PRESS_10:gVars->KB_ModeBox.PleaseChangeMode(&gVars->KB_PinYinBox,KB_NUM_MODE);break;
				case KB_PRESS_11:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("......");
					break;
				case KB_PRESS_12:gVars->KB_TempBuf.InsTail('p');break;		
				case KB_PRESS_13:gVars->KB_TempBuf.InsTail('t');break;		
				case KB_PRESS_14:gVars->KB_TempBuf.InsTail('w');break;
				case KB_PRESS_15:gVars->KB_ModeBox.PleaseChangeMode(&gVars->KB_PinYinBox,KB_EN_MODE);break;
				case KB_PRESS_16:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("��");
					break;
				case KB_PRESS_17:gVars->KB_TextBox.EndEdit();break;
				case KB_PRESS_18:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("��");
					break;
				case KB_PRESS_19:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("\r\n");
					break;
				case KB_PRESS_20:;break;
				case KB_PRESS_21:
					if(gVars->KB_ModeBox.IfOffPunc==FALSE)
						gVars->KB_TextBox.InsTailChain("��");
					break;
				case KB_PRESS_22:gVars->KB_TextBox.UpPage();    break;
				case KB_PRESS_23:gVars->KB_TextBox.DownPage();  break;
				case KB_PY_SELECT:{
						static u8 i=0,j=0;
						Debug("i=%d j=%d \r\n",i,j);
						if(gVars->KB_PinYinBox.IfEditPinYin()==FALSE)
							break;						
						if( InEvent!=Tch_Release){
							i=(pTouchInfo->x-KB_MAYBEBOX_POS_X)/19;
							j=(pTouchInfo->y-KB_MAYBEBOX_POS_Y)/18;
							if(gVars->KB_PinYinBox.curbase+(i+10*j)*2<gVars->KB_PinYinBox.strnum)
								KB_DrawPress(KB_MAYBEBOX_POS_X+19*i,KB_MAYBEBOX_POS_Y+18*j);
							else
								break;	
						}										
						else{	
							gVars->KB_PinYinBox.PleaseUpload(&gVars->KB_TextBox,i+10*j);
							i=0;
							j=0;
						}
				}break;
				case KB_PRESS_24:gVars->KB_PinYinBox.NextMayBePage();break;
				case KB_PRESS_25:gVars->KB_PinYinBox.PreMayBePage();break;		  
			}
	}	
	return 0;
}
static void KB_DrawBG(void){//������
	GUI_REGION DrawRegion;
	DrawRegion.x=0;DrawRegion.y=0;
	DrawRegion.w=240;DrawRegion.h=320;
	DrawRegion.Color=FatColor(0x809090);
	Gui_FillBlock(&DrawRegion);
	DrawRegion.Color=FatColor(NO_TRANS);
	Gui_DrawImgBin("Theme/F/KeyBoardPage/Bg/Bg.bin",&DrawRegion);	
}
static void KB_DrawBlock(void){//������
	Gui_DrawLine(KB_MAYBEBOX_POS_X,KB_MAYBEBOX_POS_Y+16,KB_MAYBEBOX_POS_X+KB_MAYBEBOX_W, KB_MAYBEBOX_POS_Y+16,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+1*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+1*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+2*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+2*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+3*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+3*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+4*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+4*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+5*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+5*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+6*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+6*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+7*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+7*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+8*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+8*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
	Gui_DrawLine(KB_MAYBEBOX_POS_X-2+9*19,KB_MAYBEBOX_POS_Y,KB_MAYBEBOX_POS_X-2+9*19, KB_MAYBEBOX_POS_Y+34,KB_ORANGE);
}
static void KB_DrawPress(u8 xx,u8 yy){//ѡ����ʾ
	u8 x,y,x0,y0,x1,y1;
	u16 Color;
	x0=xx;
	y0=yy;
	x1=xx+18;
	y1=yy+17;	
	for(y=y0;y<y1;y++)
		for(x=x0;x<x1;x++)
		{
			Color=Gui_ReadPixel16Bit(x,y);
			if(Color==KB_WHITE)
				Gui_WritePixel(x,y,KB_ORANGE);
		}	
}
//-----------------------------------------�����Ա��������---------------------------------------------	
//----------�ı�������Ա��������--------
void KB_TextBox_Init(u8 *p,s32 bufsizemsg){
	u16 bufsize;
	u32 i=0;
	bufsize=bufsizemsg&0xFFFF;
	gVars->KB_TextBox.elem=p;
	gVars->KB_TextBox.OUTBUF_SIZE=bufsize;
	while(*p++)
		i++;						   			
	gVars->KB_TextBox.elem[i]='_';
	gVars->KB_TextBox.elem[i+1]=0; 
	gVars->KB_TextBox.tailnextnext=i+1;
	gVars->KB_TextBox.ShowBegin=0;
	gVars->KB_TextBox.TotalPage=(i/KB_TEXTBOXCROSS_NUM)+1;
	gVars->KB_TextBox.CurPage=1;		
	gVars->KB_TextBox.TextBoxRegion.x=KB_TEXTBOX_POS_X;
	gVars->KB_TextBox.TextBoxRegion.y=KB_TEXTBOX_POS_Y;
	gVars->KB_TextBox.TextBoxRegion.w=KB_TEXTBOX_W;
	gVars->KB_TextBox.TextBoxRegion.h=KB_TEXTBOX_H;
	gVars->KB_TextBox.TextBoxRegion.Space=(KB_TEXTBOX_CHAR_SPACE<<4)|(KB_TEXTBOX_ROW_SPACE);
	gVars->KB_TextBox.TextBoxRegion.Color=KB_TEXTBOXCHAR_COLOUR;			 
	gVars->KB_TextBox.ScrollbarRegion.x=KB_CROSSBAR_INI_X;
	gVars->KB_TextBox.ScrollbarRegion.y=KB_CROSSBAR_INI_Y;
	gVars->KB_TextBox.ScrollbarRegion.w=KB_CROSSBAR_INI_W;
	gVars->KB_TextBox.ScrollbarRegion.h=KB_CROSSBAR_INI_H;
	gVars->KB_TextBox.ScrollbarRegion.Color=KB_CROSSBAR_COLOUR;
	gVars->KB_TextBox.ShowFromPos=KB_TextBox_ShowFromPos; 
	gVars->KB_TextBox.RefuCrossBar=KB_TextBox_RefuCrossBar;
	gVars->KB_TextBox.InsTail=KB_TextBox_InsTail;
	gVars->KB_TextBox.InsTailChain=KB_TextBox_InsTailChain;
	gVars->KB_TextBox.ClrTail=KB_TextBox_ClrTail;
	gVars->KB_TextBox.ClearAll=KB_TextBox_ClearAll;
	gVars->KB_TextBox.UpPage=KB_TextBox_UpPage;
	gVars->KB_TextBox.DownPage=KB_TextBox_DownPage;
	gVars->KB_TextBox.ShowPrepa=KB_TextBox_ShowPrepa;
	gVars->KB_TextBox.EndEdit=KB_TextBox_EndEdit;
	gVars->KB_TextBox.ShowFromPos();
	gVars->KB_TextBox.RefuCrossBar();		
}
void KB_TextBox_ShowFromPos(void){
	gVars->KB_TextBox.TextBoxRegion.Color=KB_TEXTBOXBG_COLOUR;
	Gui_FillBlock(&gVars->KB_TextBox.TextBoxRegion);		
	gVars->KB_TextBox.TextBoxRegion.Color=KB_TEXTBOXCHAR_COLOUR;
	Gui_DrawFont(GBK16_FONT,&(gVars->KB_TextBox.elem[gVars->KB_TextBox.ShowBegin]),&gVars->KB_TextBox.TextBoxRegion);
}
void KB_TextBox_RefuCrossBar(void){
	gVars->KB_TextBox.ScrollbarRegion.Color=KB_WHITE;
	Gui_FillBlock(&gVars->KB_TextBox.ScrollbarRegion);		
	gVars->KB_TextBox.ScrollbarRegion.Color=KB_CROSSBAR_COLOUR;			
	gVars->KB_TextBox.ScrollbarRegion.h = KB_CROSSBAR_INI_H/gVars->KB_TextBox.TotalPage;
	gVars->KB_TextBox.ScrollbarRegion.y=KB_CROSSBAR_INI_Y+KB_CROSSBAR_INI_H/gVars->KB_TextBox.TotalPage*(gVars->KB_TextBox.CurPage-1);	
	Gui_FillBlock(&gVars->KB_TextBox.ScrollbarRegion);		
}
bool KB_TextBox_InsTail(u8 inchar){
	if(gVars->KB_TextBox.tailnextnext==gVars->KB_TextBox.OUTBUF_SIZE-1){
		Debug("TextBuf is Full!!!\n\r");
		return(FALSE);
	}
 	gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-1]=inchar;
	gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext]='_';
	gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext+1]=0;
	gVars->KB_TextBox.tailnextnext++;
	gVars->KB_TextBox.ShowFromPos();
	return(TRUE);
}
bool KB_TextBox_InsTailChain(u8 *str){
	while(*str!=0){
		if(gVars->KB_TextBox.tailnextnext==gVars->KB_TextBox.OUTBUF_SIZE-1){
			gVars->KB_TextBox.elem[gVars->KB_TextBox.OUTBUF_SIZE-1]=0;
			gVars->KB_TextBox.ShowFromPos();
			Debug("TextBuf is Full\n\r");
			return(FALSE);
		}
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-1]=*str;
		gVars->KB_TextBox.tailnextnext++;
		str++;		
	}
	gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-1]='_';
	gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext]=0;
	gVars->KB_TextBox.ShowFromPos();
	return(TRUE);
}
void KB_TextBox_ClrTail(void){
	if(gVars->KB_TextBox.tailnextnext-1==0)
		return;
	if(gVars->KB_TextBox.tailnextnext-2==0){
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-2]='_';
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-1]=0;
		gVars->KB_TextBox.tailnextnext--;
		gVars->KB_TextBox.ShowFromPos();
		return;
	}
	if(gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-2]<126){
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-2]='_';
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-1]=0;
		gVars->KB_TextBox.tailnextnext--;

	}else{
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-3]='_';
		gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-2]=0;
		gVars->KB_TextBox.tailnextnext--;
		gVars->KB_TextBox.tailnextnext--;
	}
	gVars->KB_TextBox.ShowFromPos();
}
void KB_TextBox_ClearAll(void)
{
	gVars->KB_TextBox.elem[0]='_';
	gVars->KB_TextBox.elem[1]=0; 
	gVars->KB_TextBox.tailnextnext=1;
	gVars->KB_TextBox.ShowBegin=0;
	gVars->KB_TextBox.TotalPage=1;
	gVars->KB_TextBox.CurPage=1;	
	gVars->KB_TextBox.ShowFromPos();
	gVars->KB_TextBox.RefuCrossBar();	
}
void KB_TextBox_UpPage(void){
	if(gVars->KB_TextBox.ShowBegin<KB_TEXTBOXCROSS_NUM)
		return;
	gVars->KB_TextBox.ShowBegin-=KB_TEXTBOXCROSS_NUM;
	gVars->KB_TextBox.ShowFromPos();
	gVars->KB_TextBox.CurPage-=1;
	gVars->KB_TextBox.RefuCrossBar();
}
void KB_TextBox_DownPage(void){
	if(gVars->KB_TextBox.ShowBegin>=gVars->KB_TextBox.tailnextnext-1-KB_TEXTBOXCROSS_NUM)		
		return;
	gVars->KB_TextBox.ShowBegin+=KB_TEXTBOXCROSS_NUM;
	gVars->KB_TextBox.ShowFromPos();
	gVars->KB_TextBox.CurPage+=1;
	if( gVars->KB_TextBox.CurPage > gVars->KB_TextBox.TotalPage )		
		gVars->KB_TextBox.TotalPage = gVars->KB_TextBox.CurPage;
	gVars->KB_TextBox.RefuCrossBar();
}
void KB_TextBox_ShowPrepa(u8 *str){
	u32 index;
	index=gVars->KB_TextBox.tailnextnext;	
	while(*str!=0){
		if(index==gVars->KB_TextBox.OUTBUF_SIZE-1){
			Debug("TextBuf will be Full\n\r");
			return;
		}
		gVars->KB_TextBox.elem[index-1]=*str;
		index++;
		str++;		
	}
	gVars->KB_TextBox.elem[index-1]='_';
	gVars->KB_TextBox.elem[index]=0;
	gVars->KB_TextBox.ShowFromPos();
}
void KB_TextBox_EndEdit(void){
	gVars->KB_TextBox.elem[gVars->KB_TextBox.tailnextnext-1]=0;
	Q_GotoPage(SubPageReturn,"",sizeof(gVars->KB_TextBox.elem),gVars->KB_TextBox.elem);
}
//--------------��ʱ�����������Ա��������-----------
void KB_TempBuf_Init(void){
	gVars->KB_TempBuf.elem[0]=0;
	gVars->KB_TempBuf.tailnext=0;
	gVars->KB_TempBuf.Decode=KB_TempBuf_Decode;
	gVars->KB_TempBuf.Clr=KB_TempBuf_Clr;
	gVars->KB_TempBuf.InsTail=KB_TempBuf_InsTail;
	gVars->KB_TempBuf.UploadText=KB_TempBuf_UploadText;
	gVars->KB_TempBuf.UploadPinYin=KB_TempBuf_UploadPinYin;
	gVars->KB_TempBuf.PleaseUploadText=KB_TempBuf_PleaseUploadText;
	gVars->KB_TempBuf.PleaseUploadPinYin=KB_TempBuf_PleaseUploadPinYin;		
}
void KB_TempBuf_Decode(void){
	u8 compare,index=1,realnum=0,repeat=0;
	compare=gVars->KB_TempBuf.elem[0];
	if(gVars->KB_TempBuf.tailnext==0)
		return;
	while(index<=gVars->KB_TempBuf.tailnext){			
		if(gVars->KB_TempBuf.elem[index]!=compare){
			gVars->KB_TempBuf.decodebuf[realnum]=gVars->KB_TempBuf.elem[index-1]+repeat;
			realnum++;
			compare=gVars->KB_TempBuf.elem[index];
			repeat=0;
		}else{
			repeat++;
			if( compare=='p' || compare=='w' )
				repeat=repeat%4;
			else
				repeat=repeat%3;
		}
		index++;		
	}
	gVars->KB_TempBuf.decodebuf[realnum]=0;		
}
void KB_TempBuf_Clr(void){
	 gVars->KB_TempBuf.elem[0]=0;
	 gVars->KB_TempBuf.tailnext=0;
}
bool KB_TempBuf_InsTail(u8 inchar){
	u8 mainmode;
	if(gVars->KB_TempBuf.tailnext==KB_TEMPBUF_MAX){
		Debug("KB_TempBuf is Full\n\r");
		return(FALSE);
	}
 	gVars->KB_TempBuf.elem[gVars->KB_TempBuf.tailnext]=inchar;
	gVars->KB_TempBuf.elem[gVars->KB_TempBuf.tailnext+1]=0;
	gVars->KB_TempBuf.tailnext++;
	gVars->KB_TempBuf.Decode();
	mainmode=gVars->KB_ModeBox.GetMode() & 0x0f;
	switch(mainmode){
		case KB_EN_MODE:
		   gVars->KB_TextBox.ShowPrepa(gVars->KB_TempBuf.decodebuf);
		   break;
		case KB_PINYIN_MODE:
		   gVars->KB_PinYinBox.ShowPrepa(gVars->KB_TempBuf.decodebuf);	
		   break;
	}					 
	//�򿪶�ʱ��
	Tim2_Set(KB_DKTIME,100,(bool)1);
	return(TRUE);
}
void KB_TempBuf_PleaseUploadText(KB_TextBoxDef *ptextbox){
	KB_TempBuf_UploadText(ptextbox);
}	
void KB_TempBuf_PleaseUploadPinYin(KB_PinYinBoxDef *ppinyinbox){
	KB_TempBuf_UploadPinYin(ppinyinbox);
}
void KB_TempBuf_UploadText(KB_TextBoxDef *ptextbox){	
	ptextbox->InsTailChain(gVars->KB_TempBuf.decodebuf);
	gVars->KB_TempBuf.Clr();  		 
	//�رն�ʱ��
	Tim2_Set(0,0,(bool)1);	
}	
void KB_TempBuf_UploadPinYin(KB_PinYinBoxDef *ppinyinbox){ 
	ppinyinbox->InsTailChain(gVars->KB_TempBuf.decodebuf);
	gVars->KB_TempBuf.Clr();	 		 
	//�رն�ʱ��
	Tim2_Set(0,0,(bool)1);	
}
//----------------ƴ��������Ա��������----------------
void KB_PinYinBox_ClrPinYinShow(void){	
	gVars->KB_PinYinBox.PinYinRegion.Color=KB_PINYINBOXCHARBG_COLOUR;
	Gui_FillBlock(&gVars->KB_PinYinBox.PinYinRegion);	
	gVars->KB_PinYinBox.PinYinRegion.Color=KB_PINYINBOXCHAR_COLOUR;
}
void KB_PinYinBox_ClrMayBeShow(void){
	gVars->KB_PinYinBox.MayBeRegion.Color=KB_MAYBEBOXCHARBG_COLOUR;
	Gui_FillBlock(&gVars->KB_PinYinBox.MayBeRegion);	
	gVars->KB_PinYinBox.MayBeRegion.Color=KB_MAYBEBOXCHAR_COLOUR;
}
void KB_PinYinBox_ShowPinYinBox(void){
	gVars->KB_PinYinBox.ClrPinYinShow();
	Gui_DrawFont(GBK16_FONT,gVars->KB_PinYinBox.elem,&gVars->KB_PinYinBox.PinYinRegion);
}
void KB_PinYinBox_ShowMayBeFromPos(void){
	gVars->KB_PinYinBox.ClrMayBeShow();
	if(gVars->KB_PinYinBox.pmbstr!=NULL){
		Gui_DrawFont(GBK16_FONT,gVars->KB_PinYinBox.pmbstr+gVars->KB_PinYinBox.curbase,&gVars->KB_PinYinBox.MayBeRegion);
		KB_DrawBlock();
	}
}
void KB_PinYinBox_ClrMayBe(void){
	gVars->KB_PinYinBox.pmbstr=NULL;
	gVars->KB_PinYinBox.curbase=0;
	gVars->KB_PinYinBox.strnum=0;
	gVars->KB_PinYinBox.ClrMayBeShow();	
}
void KB_PinYinBox_LinkMayBe(const u8* str){
	gVars->KB_PinYinBox.ClrMayBeShow();
	gVars->KB_PinYinBox.pmbstr=str;
	gVars->KB_PinYinBox.curbase=0;
	gVars->KB_PinYinBox.strnum=0;
	while(*str++)
		 gVars->KB_PinYinBox.strnum++;
	gVars->KB_PinYinBox.ShowMayBeFromPos();
}
void KB_PinYinBox_LinkMayBeHalf(const u8* str){	
	gVars->KB_PinYinBox.ClrMayBeShow();
	gVars->KB_PinYinBox.pmbstr=str;
	gVars->KB_PinYinBox.curbase=0;
	gVars->KB_PinYinBox.strnum=0;
	while(*str++)
		 gVars->KB_PinYinBox.strnum++;
	gVars->KB_PinYinBox.MayBeRegion.Space=((KB_MAYBEBOX_CHAR_SPACE+8)<<4)|(KB_MAYBEBOX_ROW_SPACE);
	gVars->KB_PinYinBox.ShowMayBeFromPos();
	gVars->KB_PinYinBox.MayBeRegion.Space=(KB_MAYBEBOX_CHAR_SPACE<<4)|(KB_MAYBEBOX_ROW_SPACE);
}
void KB_PinYinBox_Init(void){
	gVars->KB_PinYinBox.elem[0]=0;
	gVars->KB_PinYinBox.tailnext=0;					
	gVars->KB_PinYinBox.pmbstr=0;	 		
	gVars->KB_PinYinBox.curbase=0;	 				
	gVars->KB_PinYinBox.strnum=0; 					
	gVars->KB_PinYinBox.PinYinRegion.x=KB_PINYINBOX_POS_X;
	gVars->KB_PinYinBox.PinYinRegion.y=KB_PINYINBOX_POS_Y;
	gVars->KB_PinYinBox.PinYinRegion.w=KB_PINYINBOX_W;
	gVars->KB_PinYinBox.PinYinRegion.h=KB_PINYINBOX_H;
	gVars->KB_PinYinBox.PinYinRegion.Color=KB_PINYINBOXCHAR_COLOUR;
	gVars->KB_PinYinBox.PinYinRegion.Space=(KB_PINYINBOX_CHAR_SPACE<<4)|(KB_PINYINBOX_ROW_SPACE);	
	gVars->KB_PinYinBox.MayBeRegion.x=KB_MAYBEBOX_POS_X;
	gVars->KB_PinYinBox.MayBeRegion.y=KB_MAYBEBOX_POS_Y;
	gVars->KB_PinYinBox.MayBeRegion.w=KB_MAYBEBOX_W;
	gVars->KB_PinYinBox.MayBeRegion.h=KB_MAYBEBOX_H;
	gVars->KB_PinYinBox.MayBeRegion.Color=KB_MAYBEBOXCHAR_COLOUR;
	gVars->KB_PinYinBox.MayBeRegion.Space=(KB_MAYBEBOX_CHAR_SPACE<<4)|(KB_MAYBEBOX_ROW_SPACE);
	gVars->KB_PinYinBox.ClrPinYinShow=KB_PinYinBox_ClrPinYinShow;
	gVars->KB_PinYinBox.ClrMayBeShow=KB_PinYinBox_ClrMayBeShow;	
	gVars->KB_PinYinBox.ShowPinYinBox=KB_PinYinBox_ShowPinYinBox;	
	gVars->KB_PinYinBox.ShowMayBeFromPos=KB_PinYinBox_ShowMayBeFromPos;
	gVars->KB_PinYinBox.ClrMayBe=KB_PinYinBox_ClrMayBe;		
	gVars->KB_PinYinBox.LinkMayBe=KB_PinYinBox_LinkMayBe;
	gVars->KB_PinYinBox.LinkMayBeHalf=KB_PinYinBox_LinkMayBeHalf;
	gVars->KB_PinYinBox.NextMayBePage=KB_PinYinBox_NextMayBePage;
	gVars->KB_PinYinBox.NextMayBePageHalf=KB_PinYinBox_NextMayBePageHalf;	
	gVars->KB_PinYinBox.PreMayBePage=KB_PinYinBox_PreMayBePage;
	gVars->KB_PinYinBox.PreMayBePageHalf=KB_PinYinBox_PreMayBePageHalf;			
	gVars->KB_PinYinBox.Clr=KB_PinYinBox_Clr; 		    
	gVars->KB_PinYinBox.InsTailChain=KB_PinYinBox_InsTailChain; 
	gVars->KB_PinYinBox.ClrTail=KB_PinYinBox_ClrTail;	
	gVars->KB_PinYinBox.IfEditPinYin=KB_PinYinBox_IfEditPinYin;		
	gVars->KB_PinYinBox.Upload=KB_PinYinBox_Upload;
	gVars->KB_PinYinBox.UploadHalf=KB_PinYinBox_UploadHalf;
	gVars->KB_PinYinBox.PleaseUpload=KB_PinYinBox_PleaseUpload;
	gVars->KB_PinYinBox.PleaseUploadHalf=KB_PinYinBox_PleaseUploadHalf;
	gVars->KB_PinYinBox.ShowPrepa=KB_PinYinBox_ShowPrepa;
}
void KB_PinYinBox_Clr(void){
	gVars->KB_PinYinBox.ClrMayBe();
	gVars->KB_PinYinBox.elem[0]=0;
	gVars->KB_PinYinBox.tailnext=0;
	gVars->KB_PinYinBox.ClrPinYinShow();
}
bool KB_PinYinBox_InsTailChain(u8 *str){
	while(*str!=0){
		if(gVars->KB_PinYinBox.tailnext==KB_PINYINBUF_MAX){
			gVars->KB_PinYinBox.elem[KB_PINYINBUF_MAX]=0;
			Debug("KB_PinYinBox is Full!!!\n\r");
			return(FALSE);
		}
		gVars->KB_PinYinBox.elem[gVars->KB_PinYinBox.tailnext]=*str;		
		gVars->KB_PinYinBox.tailnext++;
		gVars->KB_PinYinBox.elem[gVars->KB_PinYinBox.tailnext]=0;	
		gVars->KB_PinYinBox.ShowPinYinBox();
		str++;		
	}
	gVars->KB_PinYinBox.LinkMayBe(py_to_str(gVars->KB_PinYinBox.elem));	
	return(TRUE);
}
void KB_PinYinBox_ClrTail(void){
   if(gVars->KB_PinYinBox.tailnext==0){
   		gVars->KB_PinYinBox.ClrMayBe();
		return;
	}
	gVars->KB_PinYinBox.elem[gVars->KB_PinYinBox.tailnext-1]=0;
	gVars->KB_PinYinBox.tailnext--;
	gVars->KB_PinYinBox.ShowPinYinBox();
	gVars->KB_PinYinBox.LinkMayBe(py_to_str(gVars->KB_PinYinBox.elem));	
}
void KB_PinYinBox_NextMayBePage(void){
	 if(gVars->KB_PinYinBox.pmbstr==NULL)
		return;
	gVars->KB_PinYinBox.ClrMayBeShow();
	if(gVars->KB_PinYinBox.strnum>gVars->KB_PinYinBox.curbase+40)
		gVars->KB_PinYinBox.curbase+=40;	
	gVars->KB_PinYinBox.ShowMayBeFromPos();
}
void KB_PinYinBox_NextMayBePageHalf(void){
	 if(gVars->KB_PinYinBox.pmbstr==NULL)
		return;
	gVars->KB_PinYinBox.ClrMayBeShow();
	if(gVars->KB_PinYinBox.strnum>gVars->KB_PinYinBox.curbase+20)
		gVars->KB_PinYinBox.curbase+=20;
	gVars->KB_PinYinBox.MayBeRegion.Space=((KB_MAYBEBOX_CHAR_SPACE+8)<<4)|(KB_MAYBEBOX_ROW_SPACE);	
	gVars->KB_PinYinBox.ShowMayBeFromPos();
	gVars->KB_PinYinBox.MayBeRegion.Space=(KB_MAYBEBOX_CHAR_SPACE<<4)|(KB_MAYBEBOX_ROW_SPACE);
}
void KB_PinYinBox_PreMayBePage(void){
	 if(gVars->KB_PinYinBox.pmbstr==NULL)
		return;
	gVars->KB_PinYinBox.ClrMayBeShow();
	if(gVars->KB_PinYinBox.curbase>=40)
		gVars->KB_PinYinBox.curbase-=40;			
	gVars->KB_PinYinBox.ShowMayBeFromPos();	
}
void KB_PinYinBox_PreMayBePageHalf(void){
	 if(gVars->KB_PinYinBox.pmbstr==NULL)
		return;
	gVars->KB_PinYinBox.ClrMayBeShow();
	if(gVars->KB_PinYinBox.curbase>=20)
		gVars->KB_PinYinBox.curbase-=20;
	gVars->KB_PinYinBox.MayBeRegion.Space=((KB_MAYBEBOX_CHAR_SPACE+8)<<4)|(KB_MAYBEBOX_ROW_SPACE);	
	gVars->KB_PinYinBox.ShowMayBeFromPos();
	gVars->KB_PinYinBox.MayBeRegion.Space=(KB_MAYBEBOX_CHAR_SPACE<<4)|(KB_MAYBEBOX_ROW_SPACE);
}
bool KB_PinYinBox_IfEditPinYin(void){
	if(gVars->KB_PinYinBox.pmbstr==NULL && gVars->KB_PinYinBox.elem[0]!='i' && gVars->KB_PinYinBox.elem[0]!='u' && gVars->KB_PinYinBox.elem[0]!='v')
		return 	FALSE;
	else
		return TRUE;
}
void KB_PinYinBox_PleaseUpload(KB_TextBoxDef *ptextbox,u8 order){
	 KB_PinYinBox_Upload(ptextbox,order);
}
void KB_PinYinBox_PleaseUploadHalf(KB_TextBoxDef *ptextbox,u8 order){
	 KB_PinYinBox_UploadHalf(ptextbox,order);
}	
void KB_PinYinBox_Upload(KB_TextBoxDef *ptextbox,u8 order){
	u8 *p;
	if(gVars->KB_PinYinBox.pmbstr==NULL)
		return;
	if(gVars->KB_PinYinBox.curbase+order*2>=gVars->KB_PinYinBox.strnum)
		return;	
	p=(u8 *)gVars->KB_PinYinBox.pmbstr+gVars->KB_PinYinBox.curbase+order*2;	
	ptextbox->InsTail(*p++);
	ptextbox->InsTail(*p);
	gVars->KB_PinYinBox.Clr();		
}
void KB_PinYinBox_UploadHalf(KB_TextBoxDef *ptextbox,u8 order){
	u8 *p;
	if(gVars->KB_PinYinBox.pmbstr==NULL)
		return;
	if(gVars->KB_PinYinBox.curbase+order>=gVars->KB_PinYinBox.strnum)
		return;	
	p=(u8 *)gVars->KB_PinYinBox.pmbstr+gVars->KB_PinYinBox.curbase+order;
	ptextbox->InsTail(*p);
	gVars->KB_PinYinBox.Clr();		
}

void KB_PinYinBox_ShowPrepa(u8 *str){
	u8 index;
	index=gVars->KB_PinYinBox.tailnext;
	while(*str!=0){
		if(index==KB_PINYINBUF_MAX){
			Debug("KB_PinYinBox will be Full!!!\n\r");
			return;
		}
		gVars->KB_PinYinBox.elem[index]=*str;		
		index++;
		gVars->KB_PinYinBox.elem[index]=0;	
		gVars->KB_PinYinBox.ShowPinYinBox();
		str++;		
	}
}		
//----------------ģʽ������Ա��������----------------
void KB_ModeBox_ShowMode(void){
	u8 mainmode;
	mainmode=(gVars->KB_ModeBox.ModeFlag)&0xf;
	switch(mainmode){
		case  KB_NUM_MODE:{
			Q_ChangeImgTchImg(KB_PRESS_15,'T');
			Q_PresentTch(KB_PRESS_15,Tch_Release);
			Q_ChangeImgTchImg(KB_PRESS_20,'T');
			Q_PresentTch(KB_PRESS_20,Tch_Release);
			Q_ChangeImgTchImg(KB_PRESS_10,'S');
			Q_PresentTch(KB_PRESS_10,Tch_Release);
			break;
		}
		case  KB_EN_MODE:{			
			Q_ChangeImgTchImg(KB_PRESS_10,'T');
			Q_PresentTch(KB_PRESS_10,Tch_Release);
			Q_ChangeImgTchImg(KB_PRESS_20,'T');
			Q_PresentTch(KB_PRESS_20,Tch_Release);
			if( (gVars->KB_ModeBox.ModeFlag) & KB_CAP_MODE){
				Q_ChangeImgTchImg(KB_PRESS_15,'U');
				Q_PresentTch(KB_PRESS_15,Tch_Release);

			}else{
				Q_ChangeImgTchImg(KB_PRESS_15,'S');
				Q_PresentTch(KB_PRESS_15,Tch_Release);
			}
			break;
		}
		case  KB_PINYIN_MODE:{		
			Q_ChangeImgTchImg(KB_PRESS_10,'T');
			Q_PresentTch(KB_PRESS_10,Tch_Release);
			Q_ChangeImgTchImg(KB_PRESS_15,'T');
			Q_PresentTch(KB_PRESS_15,Tch_Release);
			Q_ChangeImgTchImg(KB_PRESS_20,'S');
			Q_PresentTch(KB_PRESS_20,Tch_Release);
			break;
		}
	}
}
void KB_ModeBox_Init(s32 modemsg){
	if(modemsg&KBMSK_NUM_DEF){
		gVars->KB_ModeBox.ModeFlag=KB_NUM_MODE;
		Q_ChangeImgTchImg(KB_PRESS_10,'S');
	}
	else if(modemsg&KBMSK_LETTER_DEF){
	  	gVars->KB_ModeBox.ModeFlag=KB_EN_MODE;
		Q_ChangeImgTchImg(KB_PRESS_15,'S');
	}
	else if(modemsg&KBMSK_PINYIN_DEF){
		gVars->KB_ModeBox.ModeFlag=KB_PINYIN_MODE;
		Q_ChangeImgTchImg(KB_PRESS_20,'S');
	}
	else{
		gVars->KB_ModeBox.ModeFlag=KB_PINYIN_MODE;
		Q_ChangeImgTchImg(KB_PRESS_20,'S');
	}

	if(modemsg&KBMSK_PINYIN_DIS)
		gVars->KB_ModeBox.IfOffPinYin=TRUE;
	else
		gVars->KB_ModeBox.IfOffPinYin=FALSE;
	
	if(modemsg&KBMSK_LETTER_DIS)
		gVars->KB_ModeBox.IfOffEn=TRUE;
	else
		gVars->KB_ModeBox.IfOffEn=FALSE;

	if(modemsg&KBMSK_NUM_DIS)
		gVars->KB_ModeBox.IfOffNum=TRUE;
	else
		gVars->KB_ModeBox.IfOffNum=FALSE;
	
	if(modemsg&KBMSK_NAME_MODE)
		gVars->KB_ModeBox.IfOffPunc=TRUE;
	else
		gVars->KB_ModeBox.IfOffPunc=FALSE;
									     
	gVars->KB_ModeBox.ShowMode=KB_ModeBox_ShowMode;		 	   	 
	gVars->KB_ModeBox.PleaseChangeMode=KB_ModeBox_PleaseChangeMode; 
	gVars->KB_ModeBox.GetMode=KB_ModeBox_GetMode;				 
	gVars->KB_ModeBox.ChangeModeAct=KB_ModeBox_ChangeModeAct;	
	gVars->KB_ModeBox.Capital=KB_ModeBox_Capital;
	gVars->KB_ModeBox.UnCapital=KB_ModeBox_UnCapital;
	gVars->KB_ModeBox.IfCapital=KB_ModeBox_IfCapital;
	gVars->KB_ModeBox.ShowMode();
}
void KB_ModeBox_PleaseChangeMode(KB_PinYinBoxDef *ppinyinbox,u8 newmode){
	if( gVars->KB_ModeBox.IfOffPinYin && newmode==KB_PINYIN_MODE )
		return;	
	if( gVars->KB_ModeBox.IfOffNum && newmode==KB_NUM_MODE )
		return;
	if( gVars->KB_ModeBox.IfOffEn && newmode==KB_EN_MODE )
		return;	
	gVars->KB_ModeBox.ModeFlag=newmode;
	gVars->KB_ModeBox.ChangeModeAct(ppinyinbox,newmode);
}
u8 	KB_ModeBox_GetMode(void){
	return gVars->KB_ModeBox.ModeFlag;
}
void KB_ModeBox_ChangeModeAct(KB_PinYinBoxDef *ppinyinbox,u8 newmode){
	ppinyinbox->Clr();
	gVars->KB_ModeBox.ShowMode();
}
void KB_ModeBox_Capital(void){
	(gVars->KB_ModeBox.ModeFlag) |= KB_CAP_MODE;
	gVars->KB_ModeBox.ShowMode();
}
void KB_ModeBox_UnCapital(void){
	(gVars->KB_ModeBox.ModeFlag) &= (~KB_CAP_MODE);
	gVars->KB_ModeBox.ShowMode();
}
bool KB_ModeBox_IfCapital(void){
	if( (gVars->KB_ModeBox.ModeFlag) & KB_CAP_MODE)
		return TRUE;
	else
		return FALSE;
}

