#include "Q_Shell.h"
#include "string.h"
#include "stdio.h"

#if USE_Q_SHELL_FUNCTION
/*��������ö��*/
typedef enum{
	QSCT_CALL_FUN,    //���ú���
	QSCT_GET_VAR,     //�鿴����ֵ
	QSCT_SET_VAR,     //��������ֵ
	QSCT_GET_BY_ADDR, //���ָ����ַ���ı�����Ĵ�����ֵ(32λ)	
	QSCT_SET_BY_ADDR, //����ָ����ַ���ı�����Ĵ���ָ��ֵ(32λ)	
	QSCT_LIST_FUN,    //�г����пɱ����ʵĺ�����������Ϣ
	QSCT_LIST_VAR,    //�г����пɱ����ʵı�����������Ϣ
	QSCT_ERROR        //��Ч��������
}Q_SH_CALL_TYPE;

/* ������Ϣ */
#define QSH_FUN_PARA_MAX_NUM 	     4                //�������ֻ�ɽ���4������
#define QSH_FUN_PARA_TYPE_NOSTRING   0
#define QSH_FUN_PARA_TYPE_STRING     1
typedef struct{
	char             *CmdStr;                         //�����ַ����ĵ�ַ
	unsigned char     ParaStart[QSH_FUN_PARA_MAX_NUM];//����������������ʼλ��
	unsigned char     ParaEnd[QSH_FUN_PARA_MAX_NUM];  //�������������Ľ���λ��
	unsigned char	  ParaTypes[QSH_FUN_PARA_MAX_NUM];//������������������
	unsigned char     ParaNum;                        //������������
	unsigned char     CallStart;                      //��������ʼλ��
	unsigned char     CallEnd;                        //����������λ��
	Q_SH_CALL_TYPE    CallType;                       //��������
}Q_SH_MSG_DEF;
static Q_SH_MSG_DEF Q_Sh_Msg;//ռ��20�ֽ�RAM

/*--------------------------------------------------- 
�������ܣ���ȡ�ַ���
���
		base   ĸ�ַ�����ʼ��ַ
		start  �����ַ�����ʼλ��ƫ����
		end    �����ַ�������λ��ƫ����
����    ��ȡ���ַ����׵�ַ
 ---------------------------------------------------*/
static char *Q_Sh_StrCut(char *Base,unsigned int Start,unsigned int End)
{
	Base[End+1]=0;
	return (char *)((unsigned int)Base+Start);
}

/*--------------------------------------------------- 
�������ܣ����������ַ��������������������ȫ�ֱ���Q_Sh_Msg��
���
		cmdstr   :�����ַ�����ַ
����    �ɹ� 1
		ʧ�� 0
ע      �������������ַ�������������ո�
 ---------------------------------------------------*/
static unsigned int Q_Sh_AnalyCmdStr(char *CmdStr)
{
	unsigned int Index=0;
	unsigned int TmpPos=0;
	unsigned int ParamNum=0;
	Q_Sh_Msg.CmdStr=CmdStr;
	Q_Sh_Msg.CallStart=0;
	/* ����������*/
	while(1)
	{
		if( CmdStr[Index]<33||CmdStr[Index]>126 )//�Ƿ��ַ����
		{
			Q_Sh_Msg.CallType=QSCT_ERROR;
			return 0;
		}
		if(CmdStr[Index]=='(')
		{
			if(Index==0)
			{
				Q_Sh_Msg.CallType=QSCT_ERROR;
				return 0;
			}
			Q_Sh_Msg.CallEnd=(Index-1);
			Index++;
			break;
		}
		Index++;
	}
	/* ��������*/
	TmpPos=Index;
	while(1)
	{
		if( CmdStr[Index]<33||CmdStr[Index]>126 )//�Ƿ��ַ����
		{
			Q_Sh_Msg.CallType=QSCT_ERROR;
			return 0;
		}
		if(CmdStr[Index]==',')
		{
			if(Index==TmpPos)
			{
				Q_Sh_Msg.CallType=QSCT_ERROR;
				return 0;
			}
			if(CmdStr[Index-1]=='\"')
			{
				Q_Sh_Msg.ParaTypes[ParamNum]=QSH_FUN_PARA_TYPE_STRING;
				Q_Sh_Msg.ParaStart[ParamNum]=TmpPos+1;
				Q_Sh_Msg.ParaEnd[ParamNum]=Index-2;
			}
			else
			{
				Q_Sh_Msg.ParaTypes[ParamNum]=QSH_FUN_PARA_TYPE_NOSTRING;
				Q_Sh_Msg.ParaStart[ParamNum]=TmpPos;
				Q_Sh_Msg.ParaEnd[ParamNum]=Index-1;
			}
			ParamNum++;
			TmpPos=Index+1;
		}
		if(CmdStr[Index]==')')
		{
			if(Index==Q_Sh_Msg.CallEnd+2)//�޲���
			{
				Q_Sh_Msg.ParaNum=0;
				break;
			}
			if(Index==TmpPos)
			{
				Q_Sh_Msg.CallType=QSCT_ERROR;
				return 0;
			}
			if(CmdStr[Index-1]=='\"')
			{
				Q_Sh_Msg.ParaTypes[ParamNum]=QSH_FUN_PARA_TYPE_STRING;
				Q_Sh_Msg.ParaStart[ParamNum]=TmpPos+1;
				Q_Sh_Msg.ParaEnd[ParamNum]=Index-2;
			}
			else
			{
				Q_Sh_Msg.ParaTypes[ParamNum]=QSH_FUN_PARA_TYPE_NOSTRING;
				Q_Sh_Msg.ParaStart[ParamNum]=TmpPos;
				Q_Sh_Msg.ParaEnd[ParamNum]=Index-1;
			}
			ParamNum++;
			Q_Sh_Msg.ParaNum=ParamNum;
			break;
		}
		Index++;
	}
	/*  ������������*/
	if( strcmp(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd),"lf")==0 )
		Q_Sh_Msg.CallType=QSCT_LIST_FUN;//��ʾ�����б�
	else if(strcmp(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd),"lv")==0)
		Q_Sh_Msg.CallType=QSCT_LIST_VAR;//��ʾ�����б�
	else if(strcmp(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd),"get")==0)
		Q_Sh_Msg.CallType=QSCT_GET_VAR;//�鿴����ֵ
	else if(strcmp(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd),"set")==0)
		Q_Sh_Msg.CallType=QSCT_SET_VAR;//���ñ���ֵ
	else if(strcmp(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd),"read")==0)
		Q_Sh_Msg.CallType=QSCT_GET_BY_ADDR;//�鿴ָ����ַ�ڴ��ֵ�ֵ
	else if(strcmp(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd),"write")==0)
		Q_Sh_Msg.CallType=QSCT_SET_BY_ADDR;//����ָ����ַ�ڴ��ֵ�ֵ
	else
		Q_Sh_Msg.CallType=QSCT_CALL_FUN;//���ú���
	return 1;
}

/*-----------------------------------------------------------------------
����������ֵ����ռ��ʵ���ڴ�
------------------------------------------------------------------------*/
extern int QShellFunTab$$Base;	 //qShellFunTab���׵�ַ
extern int QShellFunTab$$Limit;	 //qShellFunTab��β��ַ
extern int QShellVarTab$$Base;	 //qShellVarTab���׵�ַ
extern int QShellVarTab$$Limit;	 //qShellVarTab��β��ַ

/*-----------------------------------------------------------------------
��¼shell��η�Χ��Ϣ
------------------------------------------------------------------------*/
static QSH_RECORD *qsh_fun_table_begin = (void *)0; //��¼qShellFunTab���׵�ַ
static QSH_RECORD *qsh_fun_table_end   = (void *)0; //��¼qShellFunTab��β��ַ
static QSH_RECORD *qsh_var_table_begin = (void *)0; //��¼qShellVarTab���׵�ַ
static QSH_RECORD *qsh_var_table_end   = (void *)0; //��¼qShellVarTab��β��ַ

/*-----------------------------------------------------------------------
������ͳһ�ĺ�������
ע  ��ʹ���������Ϳ��Խ��ܲ�ͬ�����Ĳ���
------------------------------------------------------------------------*/
typedef unsigned int (*QSH_FUNC_TYPE)();
/*-----------------------------------------------------------------------
������ͳһ�ı�������,ֻ֧��u32���͵ı���
------------------------------------------------------------------------*/
typedef unsigned int QSH_VAR_TYPE;

/*-----------------------------------------------------------------------
�������ܣ���ѯָ�������ļ�¼
���    ��������
����    ���ɹ� ��Ӧ�ļ�¼
		  ʧ�� 0
ע      ����¼�а�������Ϣ�У�
		  ������name����������desc��������ַaddr 
------------------------------------------------------------------------*/
static QSH_RECORD* Q_Sh_FunFind(char* Name)
{
	QSH_RECORD* Index;
	for (Index = qsh_fun_table_begin; Index < qsh_fun_table_end; Index ++)
	{
		if (strcmp(Index->name, Name) == 0)
			return Index;
	}
	return (void *)0;
}

/*-----------------------------------------------------------------------
�������ܣ���ѯָ�������ļ�¼
���    ��������
����    ���ɹ� ��Ӧ�ļ�¼
		  ʧ�� 0
ע      ����¼�а�������Ϣ�У�������name����������desc��������ַaddr
------------------------------------------------------------------------*/
static QSH_RECORD* Q_Sh_VarFind(char* Name)
{
	QSH_RECORD* Index;
	for (Index = qsh_var_table_begin; Index < qsh_var_table_end; Index ++)
	{
		if (strcmp(Index->name, Name) == 0)
			return Index;
	}
	return (void *)0;
}

/*-----------------------------------------------------------------------
�������ܣ��г�����ע�ᵽQShellFunTab���еĺ���
���    ����
����    ����
------------------------------------------------------------------------*/
static void Q_Sh_ListFun(void)
{
	QSH_RECORD* Index;
	for (Index = qsh_fun_table_begin; Index < qsh_fun_table_end; Index++)
	{
		printf("%s\r\n",Index->desc);
	}
}

/*-----------------------------------------------------------------------
�������ܣ��г�����ע�ᵽQShellVarTab���еı���
���    ����
����    ����
------------------------------------------------------------------------*/
static void Q_Sh_ListVar(void)
{
	QSH_RECORD* Index;
	for (Index = qsh_var_table_begin; Index < qsh_var_table_end; Index++)
	{
		printf("%s\r\n",Index->desc);
	}
}

/*--------------------------------------------------- 
�������ܣ�����m��n�η�
 ---------------------------------------------------*/
static unsigned int Q_Sh_Pow(unsigned int m,unsigned int n)
{
	unsigned int Result=1;	 
	while(n--)Result*=m;    
	return Result;
}

/*--------------------------------------------------------------	    
�������ܣ����ַ���תΪ����
         ֧��16����,֧�ִ�Сд
         ��֧�ָ��� 
��Σ�
		str:�����ַ���ָ��
		res:ת����Ľ����ŵ�ַ.
����ֵ: �ɹ�             0
        ���ݸ�ʽ����     1
	    16����λ��Ϊ0    2
	    ��ʼ��ʽ����.    3
	    ʮ����λ��Ϊ0    4
--------------------------------------------------------------*/
static unsigned int Q_Sh_Str2num(char*Str,unsigned int *Res)
{
	unsigned int t;
	unsigned int bNum=0;	//���ֵ�λ��
	char *p;		  
	unsigned int HexDec=10;//Ĭ��Ϊʮ��������
	p=Str;
	*Res=0;//����.
	while(1)//ȫ��ת��ΪСд��ĸ
	{
		if((*p>='A'&&*p<='F')||(*p=='X'))
			*p=*p+0x20;
		else if(*p=='\0')break;
		p++;
	}
	p=Str;
	while(1)
	{
		if((*p<='9'&&*p>='0')||(*p<='f'&&*p>='a')||(*p=='x'&&bNum==1))//�����Ϸ�
		{
			if(*p>='a')HexDec=16;	//�ַ����д�����ĸ,Ϊ16���Ƹ�ʽ.
			bNum++;					//λ������.
		}else if(*p=='\0')break;	//����������,�˳�.
		else return 1;				//��ȫ��ʮ���ƻ���16��������.
		p++; 
	} 
	p=Str;			    //���¶�λ���ַ�����ʼ�ĵ�ַ.
	if(HexDec==16)		//16��������
	{
		if(bNum<3)return 2;			//λ��С��3��ֱ���˳�.��Ϊ0X��ռ��2��,���0X���治������,������ݷǷ�.
		if(*p=='0' && (*(p+1)=='x'))//������'0x'��ͷ.
		{
			p+=2;	//ƫ�Ƶ�������ʼ��ַ.
			bNum-=2;//��ȥƫ����	 
		}else return 3;//��ʼͷ�ĸ�ʽ����
	}else if(bNum==0)return 4;//λ��Ϊ0��ֱ���˳�.	  
	while(1)
	{
		if(bNum)bNum--;
		if(*p<='9'&&*p>='0')t=*p-'0';	//�õ����ֵ�ֵ
		else t=*p-'a'+10;				//�õ�A~F��Ӧ��ֵ	    
		*Res+=t*Q_Sh_Pow(HexDec,bNum);		   
		p++;
		if(*p=='\0')break;//���ݶ�������.	
	}
	return 0;//�ɹ�ת��
}	

//����Ϊ���⺯��
/*-----------------------------------------------------------------------
�������ܣ���ʼ��
��Σ���
���أ���
------------------------------------------------------------------------*/
void Q_Shell_Init(void)
{	
	qsh_fun_table_begin = (QSH_RECORD*) &QShellFunTab$$Base;
	qsh_fun_table_end   = (QSH_RECORD*) &QShellFunTab$$Limit;
	qsh_var_table_begin = (QSH_RECORD*) &QShellVarTab$$Base;
	qsh_var_table_end   = (QSH_RECORD*) &QShellVarTab$$Limit;
}

/*-----------------------------------------------------------------------
�������ܣ�ִ������
���    
        ifctrl 0:���յ������ַ�
			  !0:���յ������ַ���
		cmd    �����ַ���
����    ���ɹ� 1
		 ʧ�� 0  
------------------------------------------------------------------------*/
unsigned int Q_Sh_CmdHandler(unsigned int IfCtrl,char *Cmd)
{
	QSH_RECORD *pRecord;
	unsigned int ret=1;
	unsigned int fun_para[QSH_FUN_PARA_MAX_NUM];
	unsigned int fun_resu;
	unsigned int var_set;
	unsigned int var_addr;
	unsigned int i;
	if(IfCtrl==0)//�����ַ�
	{
		if(((unsigned short *)Cmd)[0]==0x445b)
		{

		}
		else if(((unsigned short *)Cmd)[0]==0x435b)
		{

		}
		else
		{
			printf("CtrlCode:%x\n\r",((unsigned short *)Cmd)[0]);
		}	
		return 0;
	}
	Q_Sh_AnalyCmdStr(Cmd);
	switch(Q_Sh_Msg.CallType)//����ִ��
	{
		case QSCT_ERROR://�������ʧ��
		{
			printf("illegal Cmd string\r\n");
			ret=0;
		}
		break;
		
		case QSCT_CALL_FUN://���ú���
		{
			if(Q_Sh_Msg.ParaNum>4)//�����������ܴ����ĸ�
			{	
				printf("fun's para can't over 4!!!\r\n");
				ret=0;
				break;
			}
			//��QShellFunTab���в��ұ����ļ�¼
			pRecord=Q_Sh_FunFind(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.CallStart,Q_Sh_Msg.CallEnd));
			if( pRecord == 0 )//��û�ҵ���Ӧ�ļ�¼��˵������ĺ���û��ע��
			{
				printf("the fun have not been regist\r\n");
				ret=0;
				break;
			}
			for(i=0;i<Q_Sh_Msg.ParaNum;i++)//�õ�����ֵ
			{
				if(Q_Sh_Msg.ParaTypes[i]==QSH_FUN_PARA_TYPE_STRING)//������Ϊ�ַ�������
				{
					fun_para[i]=(unsigned int)Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[i],Q_Sh_Msg.ParaEnd[i]);					
				}
				else//������Ϊ��ֵ����
				{
					if(Q_Sh_Str2num(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[i],Q_Sh_Msg.ParaEnd[i]),&(fun_para[i]))!=0)
					{
						printf("num str format error\r\n");
						ret=0;
						break;
					}
				}
			}
			fun_resu=((QSH_FUNC_TYPE)(pRecord->addr))(fun_para[0],fun_para[1],fun_para[2],fun_para[3]);
			printf("return %d\r\n",fun_resu);
		}
		break;
		
		case QSCT_GET_VAR://�鿴����ֵ
		{
			if(Q_Sh_Msg.ParaNum!=1)//�������Ƿ�Ϊ1������������˵�������ַ�����ʽ����
			{	
				printf("Cmd get must have only one para!!!\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Msg.ParaTypes[0]==QSH_FUN_PARA_TYPE_STRING)//��һ�����������Ͳ������ַ���
			{
				printf("Cmd get's para can not be string\r\n");
				ret=0;
				break;
			}
			pRecord=Q_Sh_VarFind(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[0],Q_Sh_Msg.ParaEnd[0]));//��QShellVarTab���в��ұ����ļ�¼
			if( pRecord == 0 )//��û�ҵ���Ӧ�ļ�¼��˵������ı�����û��ע��
			{
				printf("the var have not been regist\r\n");
				ret=0;
				break;
			}
			printf("%s=%d\r\n",pRecord->name,*(QSH_VAR_TYPE *)(pRecord->addr));//��ӡ����ֵ 
		}
		break;

		case QSCT_SET_VAR://��������ֵ
		{
			if(Q_Sh_Msg.ParaNum!=2)//�������Ƿ�Ϊ2������������˵�������ַ�����ʽ����
			{	
				printf("Cmd set must have two para!!!\r\n");
				ret=0;
				break;
			}
			pRecord=Q_Sh_VarFind(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[0],Q_Sh_Msg.ParaEnd[0]));//��QShellVarTab���в��ұ����ļ�¼
			if( pRecord == 0 )//��û�ҵ���Ӧ�ļ�¼��˵������ı�����û��ע��
			{
				printf("the var have not been regist\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Msg.ParaTypes[1]==QSH_FUN_PARA_TYPE_STRING)//�ڶ������������Ͳ������ַ���
			{
				printf("can't set var string type\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Str2num(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[1],Q_Sh_Msg.ParaEnd[1]),&var_set)!=0)//�������ֵ
			{
				printf("num str format error\r\n");
				ret=0;
				break;
			}
			*(QSH_VAR_TYPE *)(pRecord->addr)=var_set;//��ָ��������ֵ	
			printf("%s=%d\r\n",pRecord->name,*(QSH_VAR_TYPE *)(pRecord->addr)); //���Ա���ֵ			
		}
		break;
		
		
		case QSCT_GET_BY_ADDR://���ָ����ַ���ı�����Ĵ�����ֵ(32λ)	
		{
			if(Q_Sh_Msg.ParaNum!=1)//�������Ƿ�Ϊ1������������˵�������ַ�����ʽ����
			{	
				printf("Cmd read must have only one para!!!\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Msg.ParaTypes[0]==QSH_FUN_PARA_TYPE_STRING)//��һ�����������Ͳ������ַ���
			{
				printf("Cmd read's para can not be string\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Str2num(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[0],Q_Sh_Msg.ParaEnd[0]),&var_addr)!=0)//��õ�ֵַ
			{
				printf("num str format error\r\n");
				ret=0;
				break;
			}
			printf("*(0x%x)=0x%x\r\n",var_addr,*(QSH_VAR_TYPE *)var_addr);//��ӡ����ֵ
		}
		break;
		
		case QSCT_SET_BY_ADDR://����ָ����ַ���ı�����Ĵ���ָ��ֵ(32λ)	
		{
			if(Q_Sh_Msg.ParaNum!=2)//�������Ƿ�Ϊ2������������˵�������ַ�����ʽ����
			{	
				printf("Cmd write must have two para!!!\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Msg.ParaTypes[0]==QSH_FUN_PARA_TYPE_STRING)//��һ�����������Ͳ������ַ���
			{
				printf("Cmd write's first para can not be string\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Msg.ParaTypes[1]==QSH_FUN_PARA_TYPE_STRING)//�ڶ������������Ͳ������ַ���
			{
				printf("Cmd write's second para can not be string\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Str2num(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[0],Q_Sh_Msg.ParaEnd[0]),&var_addr)!=0)//��õ�ֵַ
			{
				printf("num str format error\r\n");
				ret=0;
				break;
			}
			if(Q_Sh_Str2num(Q_Sh_StrCut(Q_Sh_Msg.CmdStr,Q_Sh_Msg.ParaStart[1],Q_Sh_Msg.ParaEnd[1]),&var_set)!=0)//�������ֵ
			{
				printf("num str format error\r\n");
				ret=0;
				break;
			}
			*(QSH_VAR_TYPE *)var_addr=var_set;
			printf("*(0x%x)=0x%x\r\n",var_addr,*(QSH_VAR_TYPE *)var_addr);//���Ա���ֵ
		}
		break;
		
		case QSCT_LIST_FUN://�г����пɱ�shell���ʵĺ�����������Ϣ
		{
			Q_Sh_ListFun();
		}
		break;
		
		case QSCT_LIST_VAR://�г����пɱ�shell���ʵı�����������Ϣ
		{
			Q_Sh_ListVar();
		}
		break;
		
		default:
		{	
			printf("shell error\r\n");
			ret=0;
		}
	}
	return ret;
}
#else
void Q_Shell_Init(void)
{}

unsigned int Q_Sh_CmdHandler(unsigned int IfCtrl,char *Cmd)
{
	return 1;
}
#endif
