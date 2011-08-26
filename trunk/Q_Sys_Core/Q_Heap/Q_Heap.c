#include "System.h"
#include "Q_Heap.h"

//����:����һ���򵥵Ķѹ������
//����Ҫ˼���ǽ��ڴ����Ϊ���ɸ���С��Ԫ�������ڴ�ʱ���������������С��Ԫ���ڴ�
//��Ϊ���ڴ����Ϊ��С��Ԫ�ˣ�����������¼�ڴ�������ı���ֻ��Ҫ16λ����
//�����С��Ԫ��4byte����ô�˴�����Թ���ĶѴ�СΪ0xffff��4��
//��ʵ���ϣ�16bit���棬���1λ������ʾ�ڴ���Ƿ�ʹ�ã�����ʵ�ʴ�С��0x7fff��4��
//�˶ѹ�����ƵĶ��⿪����2����
// 1.��Ҫһ����¼��������¼�ڴ����Ϣ(��������HEAP_RECORD)��ÿһ���ڴ��Ӧһ����¼���Ա
//   �����¼��Ĵ�СMAX_RECORD_NUM���Ը���ʵ������޸�
// 2.������Ŀռ䲻����С��Ԫ��������+2ʱ�����ж�����ڴ����ģ����2���ֽ����ڴ�ŷ����ǡ�
//   ���統��С��Ԫ��4byte����������СΪ7����ʵ�ʷ�����ڴ���12��Ϊ4����������
//�ڶѷ���ʱ���˻��Ʋ���������ռ�����
//�öѹ������û�н��������:�ڴ���Ƭ

#if HEAP_TRACK_DEBUG == 1
#define Need_Debug 1
#else
#define Need_Debug 0
#endif

#if Need_Debug
#define Heap_Debug Debug
#else
#define Heap_Debug(x,y...)
#endif

//Ϊ��֤���߳��¶ѷ����ԭ�Ӳ����������������ϵͳ�ٽ���
//����˻��Ʋ������ڶ��̣߳�����Զ���������������Ϊ�հ�
#define Q_HEAP_CRIT_SAVE_DATA_DEFINE CPU_SR cpu_sr
#define Q_HEAP_ENTER_CRIT OS_EnterCritical()
#define Q_HEAP_EXIT_CRIT OS_ExitCritical()

//������֤�ڴ��Ƿ񱻳�ٵı�ʶ
//ռ��2���ֽڣ������ڴ�ǰ�󱻳�ٶ����ܲ���ĸ���Ϊ65535��֮1
#define HEAP_UNIT_REWRITE_ID 0xa55a

#define HEAP_SIZE_BYTE (24*1024)//��ջ�ܴ�С����Ϊֻ��16bit��ʾ�ڴ������������ʵ�ʴ�С����С��(0xffff*�ڴ浥Ԫ��С)

static struct
{
	u32 dDummy;//align
	u16 Heap[ HEAP_SIZE_BYTE/2 ];//mem
}H;

//�������������ͬʱ�޸�
#define HEAP_MINI_UNIT 4 //�ڴ浥Ԫ��С����λByte�����ǵ�32λ��������4�ֽڶ��룬��ֵ����Ϊ4����
#define HEAP_UNIT_OFFSET 2 // 2��HEAP_UNIT_OFFSET�η��������HEAP_MINI_UNIT

#define UNIT_MAX_NUM (HEAP_SIZE_BYTE/HEAP_MINI_UNIT)//��Ԫ����

//�ڴ������
#define UnitIdxMask 0x7fff //�õ�15λ��ʾ�ڴ����������Ե�ԪΪ��λ
#define GetUnitIdx(x) (HeapRecord[x].UnitIdx&UnitIdxMask)//xΪ��¼����ţ������ڴ��ĵ�һ����Ԫ����
#define SetUnitIdx(x,Index) {HeapRecord[x].UnitIdx=((HeapRecord[x].UnitIdx&(~UnitIdxMask))|((Index)&UnitIdxMask));}
#define GetUnitSize(x) (GetUnitIdx(GetNext((x)))-GetUnitIdx((x)))//xΪ��¼����ţ������ڴ��ռ�õ�Ԫ��
//#define UnitIdxEnd UNIT_MAX_NUM

//����ָ����Ķ���־����ڲ���xΪ��¼�����
#define UsedMask 0x8000
#define UNUSE 0x0000
#define USED 0x8000
#define GetUsedFlag(x) (HeapRecord[x].UnitIdx&(UsedMask))//���ط�0��ʾ��ʹ�ã�����0��ʾ����
#define SetUsedFlag(x,v) {HeapRecord[x].UnitIdx=((HeapRecord[x].UnitIdx&(~UsedMask))|(v?USED:UNUSE));}

//�����������ݼ�¼����Ż�ȡ��һ�����Ӧ��¼������
#define GetNext(x) (HeapRecord[x].Next)
#define SetNext(x,v) {HeapRecord[x].Next=(v);}
#define GetPrev(x) (HeapRecord[x].Prev)
#define SetPrev(x,v) {HeapRecord[x].Prev=(v);}

//���ָ����ļ�¼
#define CleanRecord(x) {HeapRecord[x].Prev=HeapRecord[x].Next=HeapRecord[x].UnitIdx=0;}

//For Track
#if HEAP_TRACK_DEBUG == 1
#define SetRecdName(x,p) HeapRecord[x].pFuncName=p
#define SetRecdLines(x,s) HeapRecord[x].Lines=s
#define GetRecdName(x) HeapRecord[x].pFuncName
#define GetRecdLines(x) HeapRecord[x].Lines
#else
#define SetRecdName(x,p)
#define SetRecdLines(x,s)
#define GetRecdName(x)
#define GetRecdLines(x)
#endif

//��Ԫ���ֽڵ�ת��
#define Unit2Byte(U) ((U)<<HEAP_UNIT_OFFSET) //ת����Ԫ�����ֽ���,��λ�ƴ���˷�
#define Byte2Unit(B) ((B)?((((B)-1)>>HEAP_UNIT_OFFSET)+1):0) //ת���ֽ�����ռ�õ�Ԫ������λ�ƴ������

typedef struct {
	u8 Prev;//��¼��һ���ڴ��ļ�¼�����
	u8 Next;//��¼��һ���ڴ��ļ�¼�����
	u16 UnitIdx;//��¼�ڴ����ʼ��Ԫλ��
#if HEAP_TRACK_DEBUG == 1
	u8 *pFuncName;
	u32 Lines;
#endif
}HEAP_RECORD;
#define MAX_RECORD_NUM 256
#define RECORD_START 0
#define RECORD_END (MAX_RECORD_NUM-1)
static HEAP_RECORD HeapRecord[MAX_RECORD_NUM];

//���ڵ���
void DebugHeap(void)
{
	u16 Index;//��ʾ�����ʼ��Ԫ
	Q_HEAP_CRIT_SAVE_DATA_DEFINE;
	
	Debug("--------------Heap Record--0x%08x----R:0x%08x-0x%08x--------\n\r",(u32)H.Heap,(u32)HeapRecord,(u32)HeapRecord+MAX_RECORD_NUM*sizeof(HEAP_RECORD));
	Q_HEAP_ENTER_CRIT;
	for(Index=RECORD_START;;Index=GetNext(Index))
	{
		Debug("%3d:Used %d,Unit 0x%04x-0x%04x, Addr 0x%08x-0x%08x, Size %5dBytes, Prev %3d,Next %3d",
			Index,GetUsedFlag(Index)?1:0,
			GetUnitIdx(Index),GetUnitIdx(GetNext(Index)),
			(u32)H.Heap+Unit2Byte(GetUnitIdx(Index)),(u32)H.Heap+Unit2Byte(GetUnitIdx(GetNext(Index))),
			Unit2Byte(GetUnitSize(Index)),
			GetPrev(Index),GetNext(Index));
#if HEAP_TRACK_DEBUG == 1
		Debug(",Function %s@%d",GetRecdName(Index),GetRecdLines(Index));
#endif
		Debug("\n\r");

		if(GetNext(Index)==RECORD_END) break;
	}
	Q_HEAP_EXIT_CRIT;
	Debug("--------------Heap Record End--0x%08x---------\n\r",(u32)H.Heap+HEAP_SIZE_BYTE);
}

void QS_HeapInit(void)
{
	u16 Index;
	Q_HEAP_CRIT_SAVE_DATA_DEFINE;

	if( (Unit2Byte(1)!=HEAP_MINI_UNIT)||(Byte2Unit(HEAP_MINI_UNIT)!=1) )
	{
		Debug("HEAP_MINI_UNIT & HEAP_UNIT_OFFSET Marco Define Error!!!\n\r");
		while(1);
	}
		
	Q_HEAP_ENTER_CRIT;
	for(Index=0;Index<(sizeof(HeapRecord)/sizeof(HEAP_RECORD));Index++)
	{
		CleanRecord(Index);
	}

	//����ͷ��¼
	SetPrev(RECORD_START,RECORD_END);
	SetNext(RECORD_START,RECORD_END);
	SetUsedFlag(RECORD_START,UNUSE);
	SetUnitIdx(RECORD_START,0);
#if HEAP_TRACK_DEBUG == 1
	SetRecdName(RECORD_START,"<Idle>");
	SetRecdLines(RECORD_START,0);
#endif

	//����β��¼
	SetPrev(RECORD_END,RECORD_START);
	SetNext(RECORD_END,RECORD_START);
	SetUsedFlag(RECORD_END,USED);
	SetUnitIdx(RECORD_END,UNIT_MAX_NUM);
#if HEAP_TRACK_DEBUG == 1
	SetRecdName(RECORD_END,"<Idle>");
	SetRecdLines(RECORD_END,0);
#endif

	//�������ڴ����ͷ���
	H.dDummy=HEAP_UNIT_REWRITE_ID;
	H.dDummy<<=16;
	
	DebugHeap();
	Q_HEAP_EXIT_CRIT;
}

//����һ���洢���¼����¼����
//Index:�����뵽�˼�¼,
//Index�����Ǹ���¼���ڴ�ļ�¼��
static bool InsertRecord(u16 Index,u16 NeedUnit)
{	
	u16 i;

	//Heap_Debug("##Insert UnitNum 0x%x @ %d\n\r",NeedUnit,Index);

	if(NeedUnit<GetUnitSize(Index))//��Ҫ�Ŀռ�С�ڿ��пռ�
	{
		//���½���һ���ڴ��¼��
		for(i=(RECORD_START+1);i<RECORD_END;i++)
		{
			if(GetUnitIdx(i)==0)
			{
				//�������ڴ�飬����һ�����п�
				SetPrev(i,Index);//prev����
				SetNext(i,GetNext(Index));//nextת��
				SetUnitIdx(i,GetUnitIdx(Index)+NeedUnit);
				SetUsedFlag(i,UNUSE);

				//����ԭ�����е��ڴ��֮����ڴ��
				SetPrev(GetNext(Index),i);
				
				//����Ҫʹ�õ��ڴ��
				SetUsedFlag(Index,USED);//���ʹ�ñ�־
				SetNext(Index,i);//nextת��
				
#if Need_Debug
				//DebugHeap();
#endif
				return TRUE;
			}
		}
		
		if(i==RECORD_END)
		{
			Debug("!!!Record Num is not enough!!!\n\r");
		}
		return FALSE;
	}
	else //�����Ҫ������ڴ��Ϳ����ڴ��պ����
	{
		SetUsedFlag(Index,USED);//��Ǳ�ʹ��
		return TRUE;
	}
}

//UnitIdxΪ��ʼ��Ԫ��������HEAP_MINI_UNITΪ��λ
//����0��ʾ���󣬷��򷵻�free���ڴ���С
#if HEAP_TRACK_DEBUG == 1
static u16 DeleteRecord(u16 UnitIdx,u8 *pFuncName,u32 Lines)
#else
static u16 DeleteRecord(u16 UnitIdx)
#endif
{
	u16 Index,Size=0;
	u16 i;

	//Debug("DeleteRecord Unit 0x%x\n\r",UnitIdx);

	for(Index=RECORD_START;;Index=GetNext(Index))
	{
		if(GetUnitIdx(Index)==UnitIdx)//�ҵ�ƥ������
		{
#if HEAP_TRACK_DEBUG == 1
			Debug("##Free   Func:%s Line:%d (For %s %d)\n\r",pFuncName,Lines,GetRecdName(Index),GetRecdLines(Index));
			SetRecdName(Index,"<Idle>");
			SetRecdLines(Index,0);
#endif
			Size=Unit2Byte(GetUnitSize(Index));//��ȡ�ڴ���С
			
			SetUsedFlag(Index,UNUSE);//���õ�ǰΪδʹ��
			
			//�ȿ�ǰ��һ���ǲ��ǿյ�
			i=GetPrev(Index);
			if(GetUsedFlag(i)==UNUSE)//ǰ��һ���ǿյģ��ϲ�ǰ��һ��
			{
				SetNext(i,GetNext(Index));
				SetPrev(GetNext(Index),i);
				CleanRecord(Index);		
				Index=i;
			}

			//�ٿ�����һ���ǲ��ǿյ�
			i=GetNext(Index);
			if(GetUsedFlag(i)==UNUSE)
			{
				SetNext(Index,GetNext(i));
				SetPrev(GetNext(i),Index);
				CleanRecord(i);
			}

#if Need_Debug
			//DebugHeap();
#endif
			return Size;
		}
		
		if(GetNext(Index)==RECORD_END) break;//�������һ�����ˣ��˳���
	}

	return 0;	
}

#if HEAP_TRACK_DEBUG == 1
void *QS_Mallco(u16 Size,u8 *pFuncName,u32 Lines)
#else
void *QS_Mallco(u16 Size)
#endif
{
	u16 UnitNum,Index;
	u16 *Mem=NULL;
	Q_HEAP_CRIT_SAVE_DATA_DEFINE;

	if(Size==0) return NULL;
	
	UnitNum=Byte2Unit(Size);//����Ҫ������Ԫ
	
	//���һ����Ԫ��������֤�ڴ�ͷβ�Ƿ񱻳�١�
	//���Size�����ͱ���С��Ԫ��������С2����С3���Ͳ��ö����һ����Ԫ�ˡ�
	Size%=HEAP_MINI_UNIT;
	if(Size==0||(HEAP_MINI_UNIT-Size)<2) UnitNum++;

	Q_HEAP_ENTER_CRIT;
	for(Index=RECORD_START;;Index=GetNext(Index))
	{
		if(GetUsedFlag(Index)==UNUSE)//�ҵ�һ���յĵ�Ԫ
		{
			if(GetUnitSize(Index)>=UnitNum)
			{
				if(InsertRecord(Index,UnitNum)==TRUE)
				{
					Mem=(void *)((u32)H.Heap+Unit2Byte(GetUnitIdx(Index)));
					Mem[(Unit2Byte(UnitNum)>>1)-1]=HEAP_UNIT_REWRITE_ID;
					Heap_Debug("##Mallco Size %d, Addr 0x%x,Unit 0x%x\n\r",Unit2Byte(UnitNum),(u32)Mem,GetUnitIdx(Index));
#if HEAP_TRACK_DEBUG == 1
					SetRecdName(Index,pFuncName);
					SetRecdLines(Index,Lines);
					Debug("##Mallco Func:%s Line:%d\n\n\r",pFuncName,Lines);
#endif
					Q_HEAP_EXIT_CRIT;		
					return (void *)Mem;
				}
				else 
					break;
			}
		}			
		
		if(GetNext(Index)==RECORD_END) 
			break;
	}
	Q_HEAP_EXIT_CRIT;
	
	DebugHeap();
	Debug("!!!No Get Heap!!!\n\r");
	return 0;
//	OS_SchedLock();
//	while(1);
}

#if HEAP_TRACK_DEBUG == 1
bool QS_Free(void *Ptr,u8 *pFuncName,u32 Lines)
#else
bool QS_Free(void *Ptr)
#endif
{	
	u16 *Mem=Ptr;
	u16 Size;
	Q_HEAP_CRIT_SAVE_DATA_DEFINE;

	if(Ptr)
	{
		Q_HEAP_ENTER_CRIT;

#if HEAP_TRACK_DEBUG == 1
		Size=DeleteRecord(Byte2Unit((u32)Ptr-(u32)H.Heap),pFuncName,Lines);
#else
		Size=DeleteRecord(Byte2Unit((u32)Ptr-(u32)H.Heap));
#endif

		if(Size==0)
		{
			Debug("##Free Error!!!Can not find the match memory!!!0x%x\n\r",(u32)Ptr);
#if HEAP_TRACK_DEBUG == 1
			Debug("##Free   Func:%s Line:%d\n\r",pFuncName,Lines);
#endif
			while(1);
		}
		else 
			Heap_Debug("##Free   Size %d, Addr 0x%x,Unit 0x%x\n\n\r",Size,(u32)Ptr,Byte2Unit((u32)Ptr-(u32)H.Heap));
			
		//����ڴ������
		if(Mem[(Size>>1)-1]!=HEAP_UNIT_REWRITE_ID)
		{
			Debug("Memory end be rewrited!!!%x %d\n\r",Mem[(Size>>1)-1],Size);
#if HEAP_TRACK_DEBUG == 1
			Debug("##Free   Func:%s Line:%d\n\r",pFuncName,Lines);
#endif
			while(1);
		}

		Mem--;
		if(Mem[0]!=HEAP_UNIT_REWRITE_ID)
		{
			Debug("Memory header be rewrited!!!%x\n\r",Mem[0]);
#if HEAP_TRACK_DEBUG == 1
			Debug("##FuncName:%s Lines:%d\n\r",pFuncName,Lines);
#endif
			while(1);
		}
				
		Q_HEAP_EXIT_CRIT;
		return TRUE;
	}
	else
		return FALSE;
}

//�����ڴ���Ƭ���
/*Ϊ�������ڴ���Ƭ�ж��٣�������һ��������fragindices����������
fragindices������Ϊ��
fragindices = (float)MaxSize/(float)TotalSize
����MaxSize��ʾ�������ɷ�������������ڴ��С
TotalSize��ʾ�������п����ڴ���ܺ�
�ɼ���ϵͳ����Ƭʱfragindices = 1
����ϵͳ���ڴ�ɷ���ʱfragindices = 0 
fragindicesԽС��ʾ��ƬԽ��*/
void QS_MonitorFragment(void)
{
	u16 Index,Size,MaxSize=0,TotalSize=0;
	
	for(Index=RECORD_START;;Index=GetNext(Index))
	{ 
		if(GetUsedFlag(Index)==UNUSE)
		{
			Size=Unit2Byte(GetUnitSize(Index));//��ȡ�ڴ���С
			TotalSize+=Size;
			if(Size>MaxSize)
				MaxSize=Size;
		}
		if(GetNext(Index)==RECORD_END) break;
	}

	Debug("********** Heap Monitor **********\n\r");
	Debug(" TotalFreeMem=%d Byte\n\r",TotalSize);
	Debug(" MaxFreeMem=%d Byte\n\r",MaxSize);
	Debug(" Fragindices=%.2f\n\r",(float)MaxSize/(float)TotalSize);
	Debug("**********************************\n\r");
}


//���������ڴ���Ƭ
bool QS_HeapArrange(void)
{//unfinish
	return TRUE;
}


