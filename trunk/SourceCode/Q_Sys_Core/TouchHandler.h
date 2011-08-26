#ifndef QSYS_TOUCH_HANDLER_H
#define QSYS_TOUCH_HANDLER_H

typedef struct{
	u16 x;
	u16 y;
	u16 w;
	u16 h;
	u8 ObjID;
	u8 Type;
	u8 Index;
	u8 OptionsMask;
}TOUCH_REGION;
//����touch handler���ԣ���ֻ�����Ӧ�����㵽ע���������
//����ҵ���ǰ�����Ӧ�����򣬲������뿪�ţ����Ͱ������ͺ�����ֵ���¼����ں�

extern u8 gTouchRegionNum;//��¼����ע����������ı���
extern TOUCH_REGION *gpTouchRegions;//��¼����ע�������ָ�����


#endif

