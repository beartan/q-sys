#ifndef QSYS_Q_SHELL_H  
#define QSYS_Q_SHELL_H

#define USE_Q_SHELL_FUNCTION 1//ѡ���Ƿ�ʹ��Q_Shell��� 1:ʹ�� 0:��ʹ��

#if USE_Q_SHELL_FUNCTION
/*-----------------------------------------------------------------------
������ ע�����ļ�¼����
ע  ��
	  1.ÿ��ע��Ķ���Ψһ��ӳ�䵽һ����¼
	  2.��¼�Ƿ���ROM�еģ���ռ��RAM
	  3.�����ͱ����ļ�¼��Ϣ������������ͬ�Ķ���
	    ������ע����Ϣ����qShellFunTab���У���QSH_FUN_REG����
		������ע����Ϣ����qShellVarTab���У���QSH_VAR_REG����
------------------------------------------------------------------------*/
typedef const struct{
	const char*		name;		//�����������
	const char*		desc;		//�������������
	void *          addr;	    //�����������ַ
}QSH_RECORD;

/*-----------------------------------------------------------------------
�깦�ܣ���һ������ע�ᵽqShellFunTab����
���    name ������   (��������ţ���ͬ)
		desc ��������
����  ����
ʹ�þ���:
unsigned int CheckStackUsed(char *task,unsigned char ifdisplay)
{
	return stack_usedbytes;
}
QSH_FUN_REG(CheckStackUsed,unsigned int CheckStackUsed(char *task,unsigned char ifdisplay))		
------------------------------------------------------------------------*/
#define QSH_FUN_REG(name, desc)					                                           \
const   char  qsh_fun_##__FILE__##_name##_name[]  = #name;				                   \
const   char  qsh_fun_##__FILE__##_name##_desc[]  = #desc;						           \
QSH_RECORD qsh_fun_##__FILE__##_name##_record  __attribute__((section("qShellFunTab"))) =  \
{							                                                               \
	qsh_fun_##__FILE__##_name##_name,	                                                   \
	qsh_fun_##__FILE__##_name##_desc,	                                                   \
	(void *)&name		                                                                   \
};

/*-----------------------------------------------------------------------
�깦�ܣ���һ������ע�ᵽqShellVarTab����
���    name ������   (��������ţ���ͬ)
		desc ��������
����    ��
ʹ�þ���:
unsigned int stack_usedbytes;
QSH_VAR_REG(stack_usedbytes,unsigned int stack_usedbytes)		
------------------------------------------------------------------------*/
#define QSH_VAR_REG(name, desc)					                                           \
const   char  qsh_var_##__FILE__##_name##_name[] = #name;				                   \
const   char  qsh_var_##__FILE__##_name##_desc[] = #desc;				                   \
QSH_RECORD qsh_var_##__FILE__##_name##_record  __attribute__((section("qShellVarTab"))) =  \
{							                                                               \
	qsh_var_##__FILE__##_name##_name,	                                                   \
	qsh_var_##__FILE__##_name##_desc,	                                                   \
	(void *)&name		                                                                   \
};

/*-----------------------------------------------------------------------
�������ܣ�shell��ʼ�� ������ʼ��shell�����ͳ�ʼ��shell����
��Σ���
���أ���
ע��  ʹ��shellǰ�����ȵ��ô˺�����ʼ��
------------------------------------------------------------------------*/
void Q_Shell_Init(void);

/*-----------------------------------------------------------------------
�������ܣ�ִ������
���    
        ifctrl 0:���յ������ַ�
			  !0:���յ������ַ���
		cmd    �����ַ���
����    ���ɹ� 1
		 ʧ�� 0  
------------------------------------------------------------------------*/
unsigned int Q_Sh_CmdHandler(unsigned int ifctrl,char *cmd);

#else
#define QSH_FUN_REG(name, desc)
#define QSH_VAR_REG(name, desc)
void Q_Shell_Init(void);
unsigned int Q_Sh_CmdHandler(unsigned int ifctrl,char *cmd);
#endif

#endif
