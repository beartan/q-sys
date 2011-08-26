/*																										
 * FileName:        	User.h													       
 * Author:          	YuanYin															  
 * Hardware Version:	QXW-Summer-V1.x												
 * Date: 				2010-11-04																
 * Description:		���ļ����������ͺͺ�������ֻ�ᱻҳ��ʹ��			
 * ҳ��Ҳֻ��ʹ�ñ�ҳ����������ͺͺ�������										
 * �Ժ�ϵͳ��ܸ�����																			
 * version:				Q-OS V1.1												   			 */
 
/*****************************************************
 *  ��ѧ��	Q-Share We	����-��Դ-����						
 *  						   													
 *  ���������ǵĲ�Ʒ���κν������������ϵ����! 	
 *						   														
 *  �Ա�רӪ��Q-ShareWe.TaoBao.Com	    						
 *  ������̳��Www.Q-ShareWe.Com									
 *****************************************************/

#ifndef QSYS_USER_H
#define QSYS_USER_H

/* Ports Macro*/
#define PACKED	__packed//������֧�ֵİ������������ṹ��
#define NULL ((void *)0)
#define Bit(bit)		(1<<bit)
#define SetBit(reg,bit)  reg|=(1<<bit)
#define ClrBit(reg,bit) reg&=~(1<<bit)
#define ReadBit(reg,bit) ((reg>>bit)&1)

//1																�궨��                               
//2												ϵͳ�趨												
//3						ҳ�津��������						
#define MAX_IMG_KEY_NUM	32//ÿ��ҳ���Զ�������ͼƬ������Ŀ
#define MAX_CHAR_KEY_NUM	32//ÿ��ҳ���Զ����������ְ�����Ŀ
#define MAX_DYN_CTRL_OBJ_NUM	24//ÿ��ҳ������Ķ�̬�ؼ�������Ŀ��������̬������yesno��numbox...�ȿؼ�
#define EXTI_KEY_VALUE_START 0x00 //ÿ��ҳ�涨����ⲿ����ö�٣�����Ӵ����ֿ�ʼ
#define USER_KEY_VALUE_START 0x40	//ÿ��ҳ�涨��İ���ö�٣�����Ӵ����ֿ�ʼ
#define MAX_TOUCH_PRESS_MS	5000 //�����������Ч����ʱ��
#define IMG_KEY_NOR_DEF_SUFFIX 'N'//������ͨ״̬ͼ��Ĭ�Ϻ�׺
#define IMG_KEY_PRS_DEF_SUFFIX 'P' //��������״̬ͼ��Ĭ�Ϻ�׺
#define IMG_KEY_REL_DEF_SUFFIX 'R' //�����ͷ�״̬ͼ��Ĭ�Ϻ�׺
//3					ҳ�津��������	����					

//3								������									

//3								������									

//3							����											
#define QSYS_FRAME_FULL 1 //����˺��ʾȫ���ܰ�
#define EXTI_KEY_MAX_NUM	3 	//�ⲿ��������
#define MAX_PATH_LEN		512 //ϵͳ�·���ֽ���Ϊ1024
#define MAX_PAGE_TRACK_NUM 32//����ҳ��ۼ���¼��
#define MAX_PAGE_LAYER_NUM 8//����ҳ�������¼��
#define MAX_GOBAL_SYSEVT	32 //ϵͳ֧�ֵ����ȫ���¼���Ŀ
#define UART_BUF_LEN 128 //���ڻ����С
#define MAX_BMP_PATH_LEN  64//֧�����bmp��Դ·������
#define MAX_SAME_KEY_VALUE_NUM 8//֧�����8����������ļ�ֵ��ͬ

//��������ͼ���޷�����ʱ��ʹ���������ɫ
#define REGION_BG_COLOR		0xe0e0e0
#define NORMAL_REPLACE_COLOR 	0xbbbbbb
#define PRESS_REPLACE_COLOR		0x999999
#define RELEASE_REPLACE_COLOR	0xbbbbbb
#define KEY_NAME_COLOR	0x333333

//3						����		����									
//2										ϵͳ�趨	����											


//2													ϵͳ��												
//#define MAX_LEN_FILENAME (_MAX_LFN * (_DF1S ? 2 : 1) + 1)//�ļ�ϵͳ���ļ�������
#define MAX_LEN_FILENAME (NAME_MAX + 1)//�ļ�ϵͳ���ļ�������

//2											ϵͳ��		����										



//1														ͷ�ļ�����																	
//2									 Standard includes.										 
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>

//2									 RTOS includes.												
#include "Os_Wrap.h"

//2							 FileSystem includes 											
#include "FsWrap.h"

//2								Drivers includes.												
#include "Drivers.h"
#include "Debug.h"
#include "Rtc.h"
#include "Time.h"

//2								 Q-Sys includes													
#include "Uart.h"
#include "Debug.h"
#include "Q_Heap.h"
#include "Q_Gui.h"
#include "QWebApi.h"
#include "Resources.h"
#include "PublicFunc.h"
#include "ControlObj.h"
//1													ͷ�ļ�����			����													




//2											ȫ���߳����ȼ�										
//����ucos���ԣ�����ԽС���ȼ�Խ��
typedef enum{
	SYSTEM_TASK_PRIORITY=OS_HIGHEST_PRIORITIES,
	SYS_MUTEX_MAX_PRIORITY,
	MUSIC_TASK_PRIORITY=SYS_MUTEX_MAX_PRIORITY+5,//֧�����5��������
	TOUCH_TASK_PRIORITY,
	INPUT_TASK_PRIORITY,
	RF_DATA_TASK_PRIORITY,
	KEYS_TASK_PRIORITY,
}TASK_PRIORITY;
//2									ȫ���߳����ȼ�	����									
//1														�궨�� ����                              



//1														��������																		
typedef enum {
	// ��ͨҳ��
	NORMAL_PAGE=0,

	//����ҳ�棬ֻ����Ϊ��ҳ����롣
	//����ͨҳ����ȣ�POP_PAGE���ز������»���ҳ��İ�����
	//����Ҳ���ᴥ����ҳ���Sys_TouchSetOk_SR�¼���
	POP_PAGE=1,
}PAGE_TYPE;//4		ҳ������

typedef enum {
	GotoNewPage=0,//����һ����ҳ�棬ԭ��������ҳ������Ƚ�����ķ�ʽִ��Clean
	GotoSubPage,//����һ����ҳ�棬ҳ��㼶����һ��
	SubPageReturn,//���ص���һ��ҳ�棬ҳ��㼶��һ�㣬��ǰҳ���ִ��Clean
	SubPageTranslate,//��һ����ҳ����ת������һ��ͬ������ҳ�棬��ǰҳ���ִ��Clean
}PAGE_ACTION;

//4	����ҳ���SystemHandler����ֵ��ּ�ڸ���ϵͳ��һЩ����
#define SYS_MSG u32
//bit0-bit15 �����û��Զ��巵��ֵ
#define SM_RET_MASK 0xffff
//bit16-30���ڻش������ϵͳ������ֻ֧��15���ش�����
#define SM_CMD_MASK 0x7fff0000
#define SM_CMD_OFFSET 16
#define SM_NoGoto (1<<16)//��Sys_GotoPage���أ����Բ�����GotoPage����
#define SM_NoTouchInit (1<<17)//��Sys_Page_Init/Sys_SubPage_Return����·��ش�ֵ������ֹͣҳ�水������Ļ���
#define SM_TouchOff (1<<18)//��SystemEventHandler���ش�ֵ����������ҳ��Ĵ�����Ӧ
#define SM_TouchOn (1<<19)//��SystemEventHandler���ش�ֵ����������ҳ��Ĵ�����Ӧ
#define SM_ExtiKeyOff (1<<20) //��SystemEventHandler���ش�ֵ���ر��ⲿ��������
#define SM_ExtiKeyOn (1<<21) //��SystemEventHandler���ش�ֵ�������ⲿ��������

//bit31 ���ڻش�״̬��ϵͳ
#define SM_STATE_MASK 0x80000000
#define SM_STATE_OFFSET 31
#define SM_State_OK 0
#define SM_State_Faile 0x80000000

typedef enum {
	Sys_PreGotoPage=0,//��һ��ҳ�����GotoPage/GotoSubPage/SubPageReturn����ʱ���ᴥ����Ҫ����ҳ��Ĵ��¼���
									//Sys_PreGotoPage���case���൱����ǰ��ҳ����Ӻ����������ǵ�ǰҳ��ĺ��������Ǻ�Sys_PageInit�ı�������
									//�����case����SM_NoGoto����ô���൱��ֻ�����˴�����ҳ��Դ���е�һ���������ѣ��������ᴥ����ҳ��Ķ�����
									
	Sys_PageInit,	//ÿ�δ�ҳ�����Ȼᴥ�����¼���
	Sys_SubPageReturn,//����ҳ���أ���Sys_PageInit��ԡ�
	
	Sys_TouchSetOk,//ҳ��򿪺󣬰������������ᴥ�����¼���
	Sys_TouchSetOk_SR,//��Sys_TouchSetOk��ԣ�ֻ������ҳ�Ǵ���ҳ�淵�صġ�
	
	Sys_PageSync,		//���û��̷߳���ͬ����Ϣʱ���ᴥ�����¼���
	
	Sys_PageClean,	//ҳ���ɵ�ǰҳ���ɷǵ�ǰҳ��ʱ����,���˳�ʱ���á�
	Sys_PreSubPage,//��Ҫ������ҳ�棬��Sys_PageClean����Եġ�ParamTable(SYS_EVT,NewPageRegID,NewPagePtr)
	
	Sys_Error,//����

	SysEvt_MaxNum,
}SYS_EVT;//4	����SystemEventHandler���¼�

typedef enum{
	Perip_KeyPress,//�ⲿ�������¡�ParamTable(PERIP_EVT,U16 KeyValue,EXIT_KEY_INFO*)
	Perip_KeyRelease,//�ⲿ�����ͷ�
	
	Perip_RtcSec,		//�������֣�ÿ�붼�ᴥ�����¼����������Σ�Ĭ�Ϲر�
	Perip_RtcMin,	//ÿ�ֶ��ᷢ�����¼����������Σ�Ĭ�Ϲر�
	Perip_RtcAlarm,//RTC���ڱ������������Σ�Ĭ�Ϲر�
	Perip_Timer,		//��ʱ�����ڣ�����֧�ֶ����ʱ������IntParam����ͬ��ʱ�������˳�ҳ��ʱ����رյ�ǰҳ�Ķ�ʱ��!!!
	
	Perip_LcdOff,//lcd��ʱϨ�𴥷����¼����������Σ�Ĭ�Ϲر�
	Perip_LcdOn,//lcd Ϩ��󣬱��������µ����ᴥ�����¼����������Σ�Ĭ�Ϲر�

	Perip_UartInput,//�������롣ParamTable(PERIP_EVT,H16Bit_ComNum|L16Bit_StrBytes,u8 *Str)

	Perip_MscPlay,//��ʼ����һ�������ļ�ʱ����
	Perip_MscPause,//�����ļ����ű���ͣʱ����
	Perip_MscContinue,//�����ļ�����ͣ�����������ʱ������
	Perip_MscStop,//�����ļ��������ʱ������

	Perip_QWebJoin,//�´ӻ���������Լ���ȡ���ӻ���ַ
	Perip_QWebRecv,//�յ�Q������
	Perip_QWebSendOk,//����Q���������
	Perip_QWebSendFailed,//����Q������ʧ��
	Perip_QWebHostConflict,//������ͻ
	Perip_QWebQueryName,//�յ�һ��queryӦ�����ڲ�ѯ�豸��
	Perip_QWebError,//����
}PERIP_EVT;//4	����PeripheralsHandler���¼�

typedef struct{//sizeof=8
	s32 IntParam;
	void *pParam;
}SYNC_INFO;//ͬ���������ͣ�ҳ��ͬ���������������Ҫ�õ�������

typedef struct{
	u16 Id;	//ֻ�а��²Żᵼ��id++,������ҳ�����ƥ��id���ж��ǲ���ͬһ�δ�������
	u16 TimeStamp;//ÿ��������ʱ�䣬���ֵ����λms
}EXIT_KEY_INFO;//��������ⲿ�����¼���Ϣ��Perip_Key�¼�����ʱ���ᴫ��PeripheralsHandlerFunc�ĵ���������

typedef enum {
	NoHasSysEvt=0,//���账��
	HasPagePeripEvt,//ҳ���¼�
	HasGobalSysEvt//ȫ���¼�
}INSPECT_SYSEVT_RET;//InspectSysEvt��������ֵ

//��һ��������ϵͳ�¼��������������û��Զ��塣
//�û����ݵ�һ�������ж�Ҫ�����ĸ���֧��
typedef SYS_MSG (*SystemHandlerFunc)(SYS_EVT ,int , void *);

typedef SYS_MSG (*PeripheralsHandlerFunc)(PERIP_EVT, int, void *);

//4	ҳ���OptionMask����ֵ(���֧��16������)
//#define ThemeMsk (1<<0)  //һ����������ʾʹ������Ŀ¼����ô��ť����ʱ�����Բ���·��ǰ׺

typedef struct PAGE_ATTRIBUTE{
	u8 * Name; //ҳ�����ƣ�����Ϊ�ַ�����������GotoPage�����õ�
	u8 * Author; //����
	u8 * Description;//����
	PAGE_TYPE Type;//ҳ������
	u16 OptionsMask;//ҳ�湦������
	
	PAGE_CONTROL_NUM CtrlObjNum;//�ؼ�������¼
	
	const IMG_TCH_OBJ *pImgTchRegCon;//ҳ��İ�ť���򼯣���������ҳ�����е�ͼƬ����
	const CHAR_TCH_OBJ *pCharTchRegCon;//ҳ��İ�ť���򼯣���������ҳ�����е����ְ���

	SystemHandlerFunc SysEvtHandler;//ҳ���SystemEventHandlerʵ�壬����ϵͳ�¼�����Ҫ��ҳ���л�������Ϊ��
	PeripheralsHandlerFunc PeripEvtHandler;//ҳ��PeripheralsHandlerFuncʵ�壬������Χ�¼�������Ϊ��
	u32 PeripEvtInitMask;
	TouchHandlerFunc TchEvtHandler;//ҳ���TouchEventHandlerʵ�塣����Ϊ��
#ifdef QSYS_FRAME_FULL	
	YesNoHandlerFunc YesNoHandler;
	NumBoxHanderFunc NumBoxHander;
	StrOptBoxHandlerFunc StrOptBoxHandler;
	StrInputBoxHandlerFunc StrInputBoxHandler;
#endif	
}PAGE_ATTRIBUTE;//4	ÿ��ҳ�涼�ᶨ������Խṹ��

//ȫ���¼�������
//��һ�������ݵ�ǰҳ��ṹ��ָ��
typedef SYS_MSG (*GobalPeripEvtHandlerFunc)(const PAGE_ATTRIBUTE * ,int , void *);

//4	���ݿ���
typedef enum {
//-------------------ϵͳ����-----------------------
	//������ϵͳ���ݣ�ϵͳ���ݻᱻ�浽flash��
	Setting_BgLightTime=0,
	Setting_BgLightScale,
	
	Setting_DBMaxNum,		//���һ��ֵ��Ӱ�����ݿ�汾

//-------------------ϵͳ״̬-----------------------
	//������ϵͳ״̬��ϵͳ״̬���ᱻ�浽flash��
	//����Ĭ��ֵ����0
	Status_ValueStart,
	Status_FsInitFinish,//�ļ�ϵͳ�Ƿ���ɹ���

	Setting_Status_MaxNum,//���ú�״̬�������Ŀ
}SETTING_NAME;
//4	���ݿ������

//4	spi flash��
typedef enum{
	FlashNothing=0,
	FlashRead,
	FlashWrite,
	FlashSectorEarse,
	FlashBlukEarse
}FLASH_CMD;
//4	spi flash�����

//4		��ʱ����
typedef enum{//�󲿷�ҳ��ֻ����һ����ʱ����Q_TIM1��������ֲʱ����ֻ��ֲ1����ʱ��
	Q_TIM1=0,//ʵ��ʹ��Tim2
	Q_TIM2,//ʵ��ʹ��Tim4
	Q_TIM3,//ʵ��ʹ��Tim5
}Q_TIM_ID;
//4��ʱ�������

//1												��������		����																

//1												ȫ�ֱ�������																		


//1												ȫ�ֱ�������		����														

//1													��������																			
//2										��ϵͳ����											

#if HEAP_TRACK_DEBUG ==1
#define Q_PageMallco(n) _Q_PageMallco(n,(void *)__func__,__LINE__)
#define Q_PageFree(p) _Q_PageFree(p,(void *)__func__,__LINE__)
void *_Q_PageMallco(u16 Size,u8 *pFuncName,u32 Lines);
void _Q_PageFree(void *Ptr,u8 *pFuncName,u32 Lines);
#else
#define Q_PageMallco _Q_PageMallco //ҳ��ʹ�õ��ڴ���亯�� 
#define Q_PageFree _Q_PageFree  //ҳ��ʹ�õ��ڴ��ͷź���
void *_Q_PageMallco(u16 Size);
void _Q_PageFree(void *Ptr);
#endif

//ҳ����ת����,��ڲ���NameΪҳ������
//����ҳ�棬ҲҪѸ�ٸ������ָ��ָ�������
//��Ϊ���ָ��ָ����ڴ���ʱ�ᱻע��
//�ɹ�����TRUE
//ֻ����ҳ���SystemEventHandler��TouchEventHandler�����е��ã�������ܻ�����һЩ��Ԥ�ϵĴ���
SYS_MSG Q_GotoPage(PAGE_ACTION PageAction, u8 *Name, int IntParam, void *pSysParam);

//��ȡҳ������
u8 Q_GetPageTotal(void);

//ָ����Ե�ǰҳ��ĺۼ�ƫ��ֵ������ҳ��ָ��
//��GetPageByTrack(0)���ص�ǰҳ��ָ��
//Q_GetPageByTrack(1)����ǰһҳ��ָ��
const PAGE_ATTRIBUTE *Q_GetPageByTrack(u8 Local);

//LayerOffset=0,���ص�ǰҳ��ָ��
//LayerOffset=1,������һ��ҳ��ָ��
const PAGE_ATTRIBUTE *Q_GetPageByLayerOffset(u8 LayerOffset);

//�õ�ָ�����ҳ��ָ��
//LayerNum=1,�õ�����
//LayerNum=2,�õ��ڶ���
const PAGE_ATTRIBUTE *Q_GetPageByLayer(u8 LayerNum);

//����һ��ҳ�����ƣ����Է������Ӧ��RID
//���û�ҵ�������0
//���Name==NULL����Name[0]=0���򷵻ص�ǰҳ���RID
u32 Q_FindRidByPageName(u8 *PageName);

//���ص�ǰҳ�������
u8 *Q_GetCurrPageName(void);

//��ȡ��ǰҳ��������
u16 Q_GetPageEntries(void);

//�������������룬�ⲿ��������
void Q_EnableInput(void);

//�رմ��������룬�ⲿ��������
void Q_DisableInput(void);

//������һ�麯�����ڿ�����Ӧ�¼�����
void Q_SetPeripEvt(u32 RegID,u32 SysEvtCon);//��������¼���Ӧ����SetSysEvt((1<<Sys_TouchSetOk)|(1<<Perip_RtcSec));
void Q_ClrPeripEvt(u32 RegID,u32 SysEvtCon);//�رն���¼���Ӧ����ClrSysEvt((1<<Sys_TouchSetOk)|(1<<Perip_RtcSec));
void Q_EnablePeripEvt(u32 RegID,PERIP_EVT SysEvent);//����һ���¼���Ӧ����EnableSysEvt(Perip_RtcSec);
void Q_DisablePeripEvt(u32 RegID,PERIP_EVT SysEvent);//�ر�һ���¼���Ӧ����DisableSysEvt(Perip_RtcSec);
INSPECT_SYSEVT_RET Q_InspectPeripEvt(u32 RegID,PERIP_EVT SysEvent);//���ָ���¼��Ƿ���Ա���Ӧ����InspectSysEvt(Perip_RtcSec);

//����ȫ���¼����κ�ҳ���£����ᴥ���¼��Ĵ�����SysEventHandler
//����Sys_PreGotoPage������
void Q_EnableGobalPeripEvent(PERIP_EVT SysEvent,GobalPeripEvtHandlerFunc SysEventHandler);

//ע��ȫ���¼�
void Q_DisableGobalPeripEvent(PERIP_EVT SysEvent,GobalPeripEvtHandlerFunc SysEventHandler);

//���ڴ���ֹͣ���˺������ܷ��أ�ֱ����ѭ��
#define Q_ErrorStopScreen(Msg) Q_ErrorStop(__FILE__,__func__,__LINE__,Msg);
void Q_ErrorStop(const char *FileName,const char *pFuncName,const u32 Line,const char *Msg);

//��Ϊsummer v1.4�İ汾�vs��spi flash����һ��spi�ߣ�Ϊ��ֹ����
//ֻ�ܵ��ô˺�����ȡflash������
bool Q_SpiFlashSync(FLASH_CMD cmd,u32 addr,u32 len,u8 *buf);

//�����û��Զ�ʱ�������ã������ö����ʱ������ֲʱ���ɿ���ֻ��ֲ1������
void Q_TimSet(Q_TIM_ID TimId,u16 Val,u16 uS_Base, bool AutoReload);

//����ҳ��Perip_UartInput�¼��£���������������
void Q_UartCmdHandler(u16 Len,u8 *pStr);

//ҳ��Run�̺߳�ҳ��ͬ���ĺ���
//�ɹ�����TRUE
bool Q_PageSync(int IntParam,void *pSysParam,u32 TicksToWait);

//���ݿ������
u32 Q_DB_GetValue(SETTING_NAME Name,void *Val);

//���ݿ�д����,������DB_BurnToSpiFlash�Ļ�,�����ʧȥ
//�ɹ�����TRUE
bool Q_DB_SetValue(SETTING_NAME Name,u32 IntParam,void *pParam,u8 ByteLen);

//��ϵͳ��ȡ״̬,ϵͳ״̬Ϊ��ʱ����
u32 Q_DB_GetStatus(SETTING_NAME Name,void *Val);

//����ϵͳ״̬
bool Q_DB_SetStatus(SETTING_NAME Name,u32 IntParam,void *pParam,u8 ByteLen);

//�����ݿ�д��spi flash��
//�ɹ�����TRUE
bool Q_DB_BurnToSpiFlash(void);



//2									��ϵͳ����	����									



//1													��������		����															

#endif

