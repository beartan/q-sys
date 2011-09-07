//��ҳ���ڴ�����
#include "System.h"

//����ϵͳ�ĸ����ط������дspi flash
bool Q_SpiFlashSync(FLASH_CMD cmd,u32 addr,u32 len,u8 *buf)
{
#if(QXW_PRODUCT_ID==114)
	OS_DeclareCritical();

	OS_EnterCritical();
#endif
	
	switch(cmd)
	{
		case FlashRead:
			M25P16_Fast_Read_Data(addr,len,buf);
			break;
		case FlashWrite:
			M25P16_Page_Program(addr,len,buf);
			break;
		case FlashSectorEarse:
			M25P16_Sector_Erase(addr);
			break;
		case FlashBlukEarse:
			M25P16_Bulk_Erase();
			break;
	}

#if(QXW_PRODUCT_ID==114)
	OS_ExitCritical();
#endif

	return TRUE;
}

//TimId����ѡ��ʱ������Sys_Timer�¼�����ʱ��IntParam�������ݵľ��Ǵ�id
//uS_Base ��ʾ��λ(ʱ��)��Ϊ1ʱ����λ��us��Ϊ100ʱ����λ��100us����Сֵ1�����ֵ900
//Val���ֵ65535
//���ն�ʱֵ= Val x uS_Base x 1us
//�¶�ʱ�趨�Ḳ�Ǿ��趨
//AutoReload�����趨��һ�ζ�ʱ����ѭ����ʱ
//val��uS_Base��������һ��Ϊ0����ֹͣ��ǰ��ʱ��
void Q_TimSet(Q_TIM_ID TimId,u16 Val,u16 uS_Base, bool AutoReload)
{
	switch(TimId)
	{
		case Q_TIM1:
			Tim2_Set(Val,uS_Base,AutoReload);
			break;
		case Q_TIM2:
			Tim4_Set(Val,uS_Base,AutoReload);
			break;
		case Q_TIM3:
			Tim5_Set(Val,uS_Base,AutoReload);
			break;
		default:
			Debug("No Such Timer in Q-sys!\n\r");
	}
}

//ͨ�õĴ������봦��
//Len �ַ�������
//pStr �ַ���
//��������������:
//Go [PageName] :��GotoNewPage��ʽ�л���ָ��ҳ�棬����Ϊ0��NULL
//Press [KeyNum] :ģ�ⰴ��
#define UART_CMD_MAX_PARAM_NUM 4//�����
void Q_UartCmdHandler(u16 Len,u8 *pStr)
{
	u16 i,n;
	u8 *pCmd=NULL;
	u8 *pParam[UART_CMD_MAX_PARAM_NUM];
	
	if(Len==0)//�����ַ�
	{
		if(((u16 *)pStr)[0]==0x445b)
		{

		}
		else if(((u16 *)pStr)[0]==0x435b)
		{

		}
		else
		{
			Debug("CtrlCode:%x\n\r",((u16 *)pStr)[0]);
		}	
		return ;
	}

	for(n=0;n<UART_CMD_MAX_PARAM_NUM;n++)	pParam[n]=NULL;//��ղ���
		
	pCmd=pStr;
	for(i=0,n=0;pStr[i];i++)//ȡ����
	{
		if(pStr[i]==' ')
		{
			pStr[i]=0;
			if(pStr[i+1]&&pStr[i+1]!=' ')
			{
				if(n>=UART_CMD_MAX_PARAM_NUM) break;
				pParam[n++]=&pStr[i+1];
			}
		}
	}

	Len=strlen((void *)pCmd);
	for(i=0;i<=Len;i++)//�����ַ���ȫ��תСд
	{
		if(pCmd[i]>='A' && pCmd[i]<='Z')
			pCmd[i]=pCmd[i]+32;
	}

	switch(pCmd[0])
	{
		case 'g':
			if(strcmp((void *)pCmd,"goto")==0)
			{
				if(pParam[0]&&pParam[0][0]=='?')//help
				{
					Debug("\n\r----------------------------------------------\n\r");
					Debug("Goto [PageName]\n\r");
					Debug("Goto [PageName] [IntParam] [pParam]\n\r");
					Debug("----------------------------------------------\n\r");
				}
				else if(pParam[0]&&pParam[0][0])
				{
					if(pParam[1]&&pParam[1][0])
						Q_GotoPage(GotoNewPage,pParam[0],StrToUint(pParam[1]),pParam[2]);		
					else
						Q_GotoPage(GotoNewPage,pParam[0],0,NULL);		
				}
			}
			break;
		case 'p':
			if(strcmp((void *)pCmd,"prtscreen")==0)
			{
				if(pParam[0]&&pParam[0][0]=='?')//help
				{
					Debug("\n\r----------------------------------------------\n\r");
					Debug("PrtScreen\n\r");
					Debug("----------------------------------------------\n\r");
				}
				else if(pParam[0]==NULL)
				{
					PrtScreen();
				}
			}
			break;
		case 'r':
			if(strcmp((void *)pCmd,"release")==0)
			{
				if(pParam[0]&&pParam[0][0]=='?')//help
				{
					Debug("\n\r----------------------------------------------\n\r");
					Debug("Release [KeyNum]\n\r");
					Debug("----------------------------------------------\n\r");
				}
				else if(pParam[0]&&pParam[0][0])
				{
					INPUT_EVENT KeyEvtParam;
					TOUCH_INFO  *pTouchInfo=&KeyEvtParam.Info.TouchInfo;
					u16 Idx=StrToUint(pParam[0]);

					pTouchInfo->Id=pTouchInfo->x=pTouchInfo->y=0;
					pTouchInfo->TimeStamp=OS_GetCurrentSysMs();
					KeyEvtParam.uType=Touch_Type;
					KeyEvtParam.EventType=Input_TchRelease;
					KeyEvtParam.Num=Idx;//������������ȥ
					OS_MsgBoxSend(gInputHandler_Queue,&KeyEvtParam,100,FALSE);
				}
			}		
			break;
	}
}



