#ifndef QSYS_CTRL_OBJ_IMG_H
#define QSYS_CTRL_OBJ_IMG_H


//yes no �ؼ�
#define CO_YES_NO_W	58 //yes no�ؼ��Ŀ��
#define CO_YES_NO_H	18 //yes no�ؼ��ĸ߶�
#define CO_YES_NO_TRAN_COLOR	FatColor(0x0000ff)//yes no�ؼ���͸��ɫ
extern const unsigned char gCtrlObj_On[]; //yes no�ؼ���onͼƬ����
extern const unsigned char gCtrlObj_Off[]; //yes no�ؼ���yesͼƬ����
extern const unsigned char gCtrlObj_OnOff[]; //yes no�ؼ���yesno����

//num�ؼ� 
#define CO_NUM_H 18 //num�ؼ��߶�
#define CO_NUM_FRAME_W 4 //num�ؼ����ұ߿���
#define CO_NUM_ARROW_W 18 //num�ؼ���ͷ���
#define CO_NUM_MIDDLE_W	1 //num�ؼ��м���䲿�ֵĵ�λ���
#define CO_NUM_TRAN_COLOR FatColor(0xff0000) //num�ؼ�͸��ɫ
#define CO_NUM_FONT_STYLE 	ASC14B_FONT //num�ؼ���ʾ����
#define CO_NUM_FONT_W	8 //num�ؼ�ascii������
#define CO_NUM_FONT_SPACE 0 //num�ؼ�����������
#define CO_NUM_FONT_COLOR FatColor(0x333333) //num�ؼ�������ɫ
#define CO_NUM_FONT_COLOR_H FatColor(0xf1aa00) //num�ؼ���������ɫ
extern const unsigned char gCtrlObj_NumLeft[]; //num�ؼ���߿�
extern const unsigned char gCtrlObj_NumLeftArrow[]; //num�ؼ����ͷ
extern const unsigned char gCtrlObj_NumMiddle[]; //num�ؼ��м����
extern const unsigned char gCtrlObj_NumRight[]; //num�ؼ��ұ߿�
extern const unsigned char gCtrlObj_NumRightArrow[]; //num�ؼ��Ҽ�ͷ
extern const unsigned char gCtrlObj_NumLeftH[]; //num�ؼ���߿����
extern const unsigned char gCtrlObj_NumLeftArrowH[]; //num�ؼ����ͷ����
extern const unsigned char gCtrlObj_NumMiddleH[]; //num�ؼ��м�������
extern const unsigned char gCtrlObj_NumRightH[]; //num�ؼ��ұ߿����
extern const unsigned char gCtrlObj_NumRightArrowH[]; //num�ؼ��Ҽ�ͷ����

#endif

