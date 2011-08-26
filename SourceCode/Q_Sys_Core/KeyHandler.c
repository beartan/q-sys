#include "System.h"

enum{ //������Ϊ�ⲿ�������䲻ͬ�ĺ���
	ExtiKey0KV=0,
	ExtiKey1KV,
	ExtiKey2KV,
};
extern void ExtiKeyHandler(u8 KeyId,u8 KeyStaus);

//���ڲ�ѯ�ⲿ����״̬
void KeysHandler_Task(void *Task_Parameters )
{
	u32 KeyMap=0;
	
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)!=ReadBit(KeyMap,ExtiKey0KV))//�б仯
		{
			KeyMap^=(1<<(ExtiKey0KV));
			ExtiKeyHandler(ExtiKey0KV,GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2));
		}			

#if(QXW_PRODUCT_ID==116)
		if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)!=ReadBit(KeyMap,ExtiKey1KV))//�б仯
		{
			KeyMap^=(1<<(ExtiKey1KV));
			ExtiKeyHandler(ExtiKey1KV,GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3));
		}		

		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)!=ReadBit(KeyMap,ExtiKey2KV))//�б仯
		{
			KeyMap^=(1<<(ExtiKey2KV));
			ExtiKeyHandler(ExtiKey2KV,GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8));
		}		
#endif				
		OS_TaskDelayMs(100);
	}
}

