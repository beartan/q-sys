#include "Os_Wrap.h"
#include "User.h" //for debug
#define OS_Debug(x,y...) //Debug


#if OS_USE_UCOS

#define QS_TASK_RECORD 1//������ϵͳ�����¼����
#define QS_TASK_RECORD_NUM	16 //����¼������

#if QS_TASK_RECORD
//�����ջ��¼
typedef struct{
	const u8 *Name;
	void *Stk;
	u16 StkSize;
	u8 Priority;
}QSYS_TASK_RECORD;
static u8 QsTaskTotalNum=0;
static QSYS_TASK_RECORD QsTaskRecord[QS_TASK_RECORD_NUM];
#endif

void OS_WrapInit(void)
{
	OSInit(); //Initialize "uC/OS-II, The Real-Time Kernel".    
	QS_HeapInit();
}

void OS_StartRun(void)
{
	OSTimeSet(0);
	OSStart();  //Start multitasking (i.e. give control to uC/OS-II).  
}

//���񴴽�����
u8 OS_TaskCreate(TASK_FUNC TaskFunc, const u8 *TaskName, u16 StackSizeByte, void *pParam, u8 Priority)
{
	u8 error;
	void *ptr=OS_Mallco(StackSizeByte+4);//��mallco����һ����4byte align��������Ҫ8byte align�����Զ����4��

#if QS_TASK_RECORD
	MemSet(ptr,0x5a,StackSizeByte);//��¼��ջ
#endif

	error=OSTaskCreate(TaskFunc,pParam,(OS_STK *)(((((u32)ptr)+StackSizeByte+4)&0xfffffff8)-4),Priority);
	
	if(OS_ERR_NONE!=error)
	{
		Debug("!!!Create Task error %d\n\r",error);
		OS_SchedLock();
		while(1);
	}

	OSTaskNameSet(Priority,(void *)TaskName,&error);

#if QS_TASK_RECORD	
	QsTaskRecord[QsTaskTotalNum].Name=TaskName;
	QsTaskRecord[QsTaskTotalNum].Priority=Priority;
	QsTaskRecord[QsTaskTotalNum].Stk=ptr;
	QsTaskRecord[QsTaskTotalNum++].StkSize=StackSizeByte;	
	OS_Debug("Create Task %s @ STK 0x%x-0x%x\n\r",TaskName,(u32)ptr,(u32)ptr+StackSizeByte);
#endif

	return error;
}

void OS_TaskStkCheck(bool Display)
{
#if QS_TASK_RECORD	
	u8 i;
	u8 *p;

	if(Display)
	{
		Debug("----------------------Task Record----------------------\n\r");
		for(i=0;i<QsTaskTotalNum;i++)
		{
			for(p=(u8 *)QsTaskRecord[i].Stk;*p==0x5a;p++);
			Debug("Task %s,Stk 0x%x,Remain %d\n\r",QsTaskRecord[i].Name,QsTaskRecord[i].Stk,
				(u32)p-(u32)QsTaskRecord[i].Stk);
		}
		Debug("----------------------Task Record----------------------\n\r");
	}
	else
	{
		for(i=0;i<QsTaskTotalNum;i++)
		{
			if(*(u32 *)QsTaskRecord[i].Stk!=0x5a5a5a5a) 
			{
				Debug("!!!Task %s stk is overflow!Stk 0x%x\n\r",QsTaskRecord[i].Name,(u32)QsTaskRecord[i].Stk);
				OS_SchedLock();
				Debug("----------------------Task Record----------------------\n\r");
				for(i=0;i<QsTaskTotalNum;i++)
				{
					for(p=(u8 *)QsTaskRecord[i].Stk;*p==0x5a;p++);
					Debug("Task %s,Stk 0x%x,Remain %d\n\r",QsTaskRecord[i].Name,QsTaskRecord[i].Stk,
						(u32)p-(u32)QsTaskRecord[i].Stk);
				}
				Debug("----------------------Task Record----------------------\n\r");
				while(1);
			}
		}
	}
#endif
}

u8 OS_TaskDelete(u8 Prio)
{
	return OSTaskDel(Prio);
}

void OS_TaskDelay (u16 Ticks)
{
	OSTimeDly(Ticks);
}

void OS_TaskDelayMs(u16 Ms)
{
	OSTimeDly(Ms/OS_TICK_RATE_MS);
}

u8 OS_TaskSuspend(u8 Prio)
{
	return OSTaskSuspend(Prio);
}

u8 OS_TaskResume(u8 Prio)
{
	return OSTaskResume(Prio);
}

#if 0
OS_QueueHandle OS_QueueCreate(void **Start, u16 Size)
{
	return OSQCreate(Start,Size);
}

u8 OS_QueueSend(OS_QueueHandle pQueue, void *pMsg, u16 WaitTicks)
{
	return OSQPost(pQueue,pMsg);
}

u8 OS_QueueSendToFront(OS_QueueHandle pQueue, void *pMsg, u16 WaitTicks)
{
	return OSQPostFront(pQueue,pMsg);
}
 
u8 OS_QueueReceive(OS_QueueHandle pQueue, void *pItem,u8 ItemSize,u16 WaitTicks)
{
	u8 error;
	u8 *ptr=pItem,*pTemp;

	if(WaitTicks==OS_NO_DELAY)
	{
		pTemp=(u8 *)OSQAccept(pQueue,&error);

		if(pTemp==(u8 *)NULL)
		{
			if(error!=OS_ERR_Q_EMPTY) OS_Debug("QueueAccept error:%d\n\r",error);
			return error;
		}
	}
	else
	{
		pTemp=(u8 *)OSQPend(pQueue,WaitTicks,&error);
	}

	for(WaitTicks=0;WaitTicks<ItemSize;WaitTicks++) 
		ptr[WaitTicks]=pTemp[WaitTicks];

	if(error)OS_Debug("QueueReceive error:%d\n\r",error);
	return error;
}
#endif

/********************************************
*  ����      ���������ڴ�
*  ���      blksize���ʼ���С 
*			 nblks���ʼ�����
*  ����      �ɹ��������ڴ������ƿ�ָ��
*  		     ʧ�ܣ����ؿ�ָ��
*  ��������  ������
*********************************************/
static QSYS_MSG_BOX_MEM *QMBMemCreat(u16 blksize,u8 nblks)
{
	QSYS_MSG_BOX_MEM    *pmem;//�����ݴ������ڴ������ƿ��ַ
	void   *addr;             //�����ݴ������ڴ��ַ
    u8     *pblk;			  //�����ݴ��ʼ�ͷ��ַ
    void   **plink;			  //�������Ӹ��ʼ�
	u16    ablksize;          //�������ʼ���С
    u32    i;
	if( blksize==0 || nblks==0 )//��������
	{
		Debug("QMBMemCreat parameter error!blksize and nblks must >0\r\n");
		return NULL;
	}
	ablksize = (((blksize-1)>>2)+1)<<2;//��֤ÿ���ʼ����׵�ַ�������ֽڶ���
	addr = QS_Mallco(ablksize*nblks);//��������ڴ�
	if(addr==NULL)
	{
		Debug("QMBMemCreat can't get mem from heap to creat QSYS_MSG_BOX_MEM\r\n");
		return NULL;
	}
    pmem = QS_Mallco(sizeof(QSYS_MSG_BOX_MEM));//��������ڴ������ƿ��ڴ�                             
    if (pmem == NULL)
	{ 
		Debug("QMBMemCreat can't get mem from heap to creat QSYS_MSG_BOX_MEM\r\n");
		QS_Free(addr);//�ͷ�֮ǰ������ڴ�
        return NULL;
	}
	//�ӹ������ڴ�
    plink = (void **)addr;                
    pblk  = (u8 *)((u32)addr + ablksize);  
    for (i = 0; i < (nblks - 1); i++)
	{    
       *plink = (void *)pblk;                //��ǰ���״�����һ���ַ
        plink = (void **)pblk;               //��ǰ�����˳��
        pblk  = (u8 *)((u32)pblk + ablksize); //�����һ���ַ
    }
    *plink               = NULL;        //ĩ����״����ָ��
	//�������ڴ���ƿ鸳ֵ
    pmem->QMBMemAddr     = addr;            
    pmem->QMBMemFreeList = addr;            
    pmem->QMBMemNFree    = nblks;                     
    pmem->QMBMemBlks     = nblks;
    pmem->QMBMemBlkSize  = blksize; 
    return pmem;//�ɹ����������ڴ���ƿ�ĵ�ַ
}

/* *******************************************
*	����	  ��ȡ�����ʼ�
*   ���	  pQMBMem�������ڴ������ƿ�ָ��
*   ����      �ɹ���ָ��һ�������ʼ���ָ��
*   		  ʧ�ܣ����ؿ�ָ��
*   ��������  ������
*********************************************/
static void *QMBMemGet(QSYS_MSG_BOX_MEM *pQMBMem)
{
	void      *pblk;
    OS_CPU_SR  cpu_sr = 0;
	if(pQMBMem==NULL) return NULL;
    OS_ENTER_CRITICAL();//���ж�
    if (pQMBMem->QMBMemNFree > 0) {                       //�鿴�Ƿ��п��ʼ�
        pblk                    = pQMBMem->QMBMemFreeList; //�ӿ��������ȡ�����ʼ�ָ��
        pQMBMem->QMBMemFreeList = *(void **)pblk;         //������������
        pQMBMem->QMBMemNFree--;                           //�����ʼ�����һ
        OS_EXIT_CRITICAL();//���ж�
        return pblk;                                      //���ؿ����ʼ�ָ��
    }
	else
	{
		OS_EXIT_CRITICAL();//���ж�
		Debug("QMBMemGet can't get get free block\r\n");
		return NULL;         
	}                   
}

/******************************************** 
*   ����	 �Ż������˵��ʼ�
*	���	 pQMBMem�������ڴ������ƿ�ָ�� 
*			 pblk�������ʼ�ָ��
*	����	 �ɹ�������1
*			 ʧ�ܣ�����0
*	�������� ������
*********************************************/  
static u8 QMBMemPut(QSYS_MSG_BOX_MEM *pQMBMem,void *pblk)
{
    OS_CPU_SR  cpu_sr = 0;
	if(pQMBMem==NULL || pblk==NULL) return NULL;
    OS_ENTER_CRITICAL();//���ж�
    if (pQMBMem->QMBMemNFree >= pQMBMem->QMBMemBlks) 
	{  
        OS_EXIT_CRITICAL();//���ж�
		Debug("QMBMemPut can't put pblk,the pblk maybe illegal\r\n");
        return 0;
    }
    *(void **)pblk          = pQMBMem->QMBMemFreeList;   //�������ʼ����ӵ���������
    pQMBMem->QMBMemFreeList = pblk;
    pQMBMem->QMBMemNFree++;                             //�����ʼ�����һ
    OS_EXIT_CRITICAL();//���ж�
    return 1;                       
}

/********************************************* 
*	����	  ������ϵͳ����
*	���	  Name�������� 
*			  ItemSize���������ʼ��Ĵ�С 
*			  ItemNum���������ʼ�������
*   ����	  �ɹ�����ϵͳ������ƿ�ָ��
*			  ʧ�ܣ����ؿ�ָ��
*	��������  ������
*********************************************/
OS_MsgBoxHandle OS_MsgBoxCreate(const u8 *Name,u16 ItemSize,u8 ItemNum)
{
	QSYS_MSG_BOX *pMsgBox;
	void *ptr;
	if(ItemNum==0 || ItemSize==0 || Name==NULL)
	{ 
		Debug("OS_MsgBoxCreate parameter error!blksize and nblks must >0\r\n");
		return NULL;
	}
	if((pMsgBox=(QSYS_MSG_BOX *)QS_Mallco(sizeof(QSYS_MSG_BOX)))==NULL)
	{
		Debug("OS_MsgBoxCreate can't get mem from heap to creat QSYS_MSG_BOX\r\n");
		return NULL; 
	}
	if((pMsgBox->Mem = QMBMemCreat(ItemSize, ItemNum))==NULL)
	{
		Debug("OS_MsgBoxCreate failed because QMBMemCreat failed\r\n");
		QS_Free(pMsgBox);
		return NULL; 
	}
	if((ptr = QS_Mallco(ItemNum*4))==NULL)
	{
		Debug("OS_MsgBoxCreate can't get mem from heap to creat OSMsg\r\n");
		QS_Free(pMsgBox);
	}
	if((pMsgBox->Queue = OSQCreate (ptr,ItemNum))== (OS_EVENT *)0)
	{
		Debug("OS_MsgBoxCreate failed because OSQCreate failed\r\n");
		QS_Free(pMsgBox);
		QS_Free(ptr);
		return NULL; 
	}
	if((pMsgBox->Sem = OSSemCreate (ItemNum)) == (OS_EVENT *)0 )
	{
		Debug("OS_MsgBoxCreate failed because OSSemCreate failed\r\n");
		QS_Free(pMsgBox);
		QS_Free(ptr);
		return NULL; 
	}
	pMsgBox->Name = Name;
	return 	pMsgBox;
}
/*************************************************** 
*	����	  ���ϵͳ���䷢���ʼ�
*	���	  pMsgBox����ϵͳ������ƿ�ָ�� 
*		      Msg����Ϣָ�� 
*		      WaitTicks��
*					OS_NO_DELAY  ���ȴ� 
*					OS_MAX_DELAY һֱ�ȴ�
*					1-0xfffe     �ȴ�������
*			  IfPostFront:
*					TRUE  ��������ͷ(�����ڽ����¼�)
*					FALSE ��������β(������һ���¼�)
*	����	  �ɹ���OS_NO_ERR
*			  ʧ�ܣ�����OS_ERR_MBOX_FULL
*	��������  ������
****************************************************/
u8 OS_MsgBoxSend(OS_MsgBoxHandle pMsgBox,void *Msg, u16 WaitTicks,bool IfPostFront)
{
	u8 error;
	void *pTempMsg;
	if(WaitTicks==OS_NO_DELAY)
	{
		if(OSSemAccept(pMsgBox->Sem)==NULL) 
		{
			Debug("OS_MsgBoxSend failed when WaitTicks==OS_NO_DELAY\r\n");
			return OS_ERR_MBOX_FULL;
		}
		else
		{
			pTempMsg = QMBMemGet(pMsgBox->Mem);//�������ڴ���һ������ʼ�
			MemCpy( pTempMsg, Msg, pMsgBox->Mem->QMBMemBlkSize );//����������Ϣ���Ƶ��ʼ���
			if(IfPostFront==FALSE)
				OSQPostOpt (pMsgBox->Queue, pTempMsg, OS_POST_OPT_NONE  );//���ʼ����������β
			else
				OSQPostOpt (pMsgBox->Queue, pTempMsg, OS_POST_OPT_FRONT  );//���ʼ����������ͷ
			//Debug("$$%s send\r\n",pMsgBox->Name);
			return OS_NO_ERR;
		}
	}
	else
	{
		OSSemPend (pMsgBox->Sem, WaitTicks, &error);
		if(error!=OS_ERR_NONE)
		{
			Debug("OS_MsgBoxSend failed when timeout\r\n");
			return OS_ERR_MBOX_FULL;
		}
		else
		{
			pTempMsg = QMBMemGet(pMsgBox->Mem);//�������ڴ���һ������ʼ�
			MemCpy( pTempMsg, Msg, pMsgBox->Mem->QMBMemBlkSize );//����������Ϣ���Ƶ��ʼ���
			if(IfPostFront==FALSE)
				OSQPostOpt (pMsgBox->Queue, pTempMsg, OS_POST_OPT_NONE  );//���ʼ����������β
			else
				OSQPostOpt (pMsgBox->Queue, pTempMsg, OS_POST_OPT_FRONT  );//���ʼ����������ͷ
			//Debug("$$%s send\r\n",pMsgBox->Name);
			return OS_NO_ERR;
		}
	}
}
/************************************************** 
*	����	  �ӿ�ϵͳ��������ʼ�
*	���	  pMsgBox����ϵͳ������ƿ�ָ�� 
*		  	  Msg��������Ϣ��ָ�� 
*		  	  WaitTicks��
*			  	   OS_NO_DELAY���ȴ� 
*				   OS_MAX_DELAYһֱ�ȴ�
*				   1-0xfffe �ȴ�������
*	����	  �ɹ���OS_NO_ERR
*			  ʧ�ܣ�����OS_TIMEOUT
*	��������  ������
**************************************************/
u8 OS_MsgBoxReceive(OS_MsgBoxHandle pMsgBox,void *Msg, u16 WaitTicks)
{
	u8 error;
	void *pTempMsg;
	if(WaitTicks==OS_NO_DELAY)
	{
		pTempMsg = OSQAccept (pMsgBox->Queue, &error);
		if(pTempMsg==NULL)
		{
		//	Debug("OS_MsgBoxReceive failed when WaitTicks==OS_NO_DELAY\r\n");
			return OS_TIMEOUT;
		}
		else
		{
			MemCpy( Msg ,pTempMsg , pMsgBox->Mem->QMBMemBlkSize );//���ʼ����ݿ�������Ϣ����
			QMBMemPut (pMsgBox->Mem, pTempMsg);//�ͷ�һ���ʼ��������ʼ�����
			OSSemPost (pMsgBox->Sem);//�ͷŵ�һ���ź���
			//Debug("$$%s receive\r\n",pMsgBox->Name);
			return OS_NO_ERR;
		}
	}
	else
	{
		pTempMsg = OSQPend (pMsgBox->Queue, WaitTicks, &error);
		if(pTempMsg==NULL)
		{
			Debug("OS_MsgBoxReceive failed when timeout\r\n");
			return OS_TIMEOUT;
		}
		else
		{
			MemCpy( Msg ,pTempMsg , pMsgBox->Mem->QMBMemBlkSize );//���ʼ����ݿ�������Ϣ����
			QMBMemPut (pMsgBox->Mem, pTempMsg);//�ͷ�һ���ʼ��������ʼ�����
			OSSemPost (pMsgBox->Sem);//�ͷŵ�һ���ź���
			//Debug("$$%s receive\r\n",pMsgBox->Name);
			return OS_NO_ERR;
		}
	}
}

#if 0
//Query msg box idle num
u8 OS_MsgBoxQueryIdleNum(OS_MsgBoxHandle pMsgBox)
{
	if(pMsgBox->Flag==MSG_BOX_FULL) return 0;
	else if(pMsgBox->Flag==MSG_BOX_IDLE) return pMsgBox->ItemNum;
	else
	{
		u8 i=pMsgBox->Idx;
		u8 n=0;
		CPU_SR cpu_sr;
		u8 *pItem;
	
		OS_EnterCritical();
		while(1)//�ҵ�һ��
		{
			pItem=(u8 *)((u32)pMsgBox->Msg+pMsgBox->ItemSize*i);
			if(pItem[0]) break;
			if(++i==pMsgBox->ItemNum) i=0;
			if(i==pMsgBox->Idx) OS_Debug("Find MsgBox Item Error!\n\r");
			n++;
		}		
		OS_ExitCritical();

		return n;
	}
}
#endif

OS_SemaphoreHandle OS_SemaphoreCreate(u8 Cnt)
{

	return OSSemCreate(Cnt);
}

u8 OS_SemaphoreTake(OS_SemaphoreHandle Sem, u16 WaitTicks)
{
	u8 error;
	
	OSSemPend(Sem,WaitTicks,&error);

	if(error) OS_Debug("SemaphoreTake error:%d\n\r",error);
	return error;
}

u8 OS_SemaphoreGive(OS_SemaphoreHandle Sem)
{
	return OSSemPost(Sem);
}

OS_MutexHandler OS_MutexCreate(void)
{
	u8 error;
	static u8 Prio=SYS_MUTEX_MAX_PRIORITY;

	OS_MutexHandler pEvent=OSMutexCreate(Prio++,&error);

	if(error) Debug("Mutex Create %d\n\r",error);
	
	return pEvent;
}

u8 OS_MutexTake(OS_MutexHandler Mutex, u16 WaitTicks)
{
	u8 error;
	
	OSMutexPend(Mutex,WaitTicks,&error);

	if(error) OS_Debug("MutexTake error:%d\n\r",error);
	return error;
}

u8 OS_MutexGive(OS_MutexHandler Mutex)
{
	return OSMutexPost(Mutex);
}

u32 OS_GetCurrentTick(void)
{
	return OSTimeGet();
}

u32 OS_GetCurrentSysMs(void)	
{
	return (OSTimeGet()*OS_TICK_RATE_MS);
}







#elif OS_USE_FREERTOS

#endif

