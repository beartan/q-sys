#ifndef QSYS_CONTROL_OBJ_H
#define QSYS_CONTROL_OBJ_H

typedef enum{
	COT_NULL=0,
	COT_ImgTch,
	COT_CharTch,
	COT_DynImg,
	COT_DynChar,
#ifdef QSYS_FRAME_FULL	
	COT_YesNo,
	COT_NumBox,
	COT_StrOpt,
	COT_StrInput,
#endif
}CTRL_OBJ_TYPE;

typedef struct{
	u8 ImgTchNum;//ͼƬ������Ŀ�������ҳ��ֻ�õ���ͼƬ��ť
	u8 CharTchNum;//���ְ�����Ŀ�������ҳ��ֻ�õ���ͼƬ��ť�����ֵ����Ϊ0����
	u8 DynImgTchNum;//��̬ͼƬ�����ĸ���
	u8 DynCharTchNum;//��̬���ְ����ĸ���
#ifdef QSYS_FRAME_FULL	
	u8 YesNoNum;//yes or no ѡ��ĸ���
	u8 NumBoxNum;//���ֿ�ĸ���
	u8 StrOptBoxNum;//�ַ���ѡ���ĸ���
	u8 StrInputBoxNum;//�ַ��������ĸ���
#endif
}PAGE_CONTROL_NUM;//ҳ��Ķ�̬�ؼ�����

//4	����ҳ���UserEventHandler����ֵ��ּ�ڸ���ϵͳ��һЩ����
#define TCH_MSG u32
//bit0-bit15 �����û��Զ��巵��ֵ
#define TM_RET_MASK 0xffff
//bit16-30���ڻش������ϵͳ������ֻ֧��15���ش�����
#define TM_CMD_MASK 0x7fff0000
#define TM_CMD_OFFSET 16
#define TM_TouchOff (1<<16)//��TouchEventHandler���ش�ֵ���رմ�����Ӧ
#define TM_TouchOn (1<<17)//��TouchEventHandler���ش�ֵ������������Ӧ
#define TM_ExtiKeyOff (1<<18)//��TouchEventHandler���ش�ֵ���ر��ⲿ��������
#define TM_ExtiKeyOn (1<<19)//��TouchEventHandler���ش�ֵ�������ⲿ��������
//bit31 ���ڻش�״̬��ϵͳ
#define TM_STATE_MASK 0x80000000
#define TM_STATE_OFFSET 31
#define TM_State_OK 0
#define TM_State_Faile 0x80000000

typedef enum {
	Tch_Press=0,//��������
	Tch_Continue,//���ִ���״̬�����ڴ��ݳ���ʱ��ʵʱ����
	Tch_Release,//����Ч�������ɿ�,��Ч����ָPressʱ����ע������
	Tch_ReleaseVain,//�ڷ���Ч�����ɿ�
	Tch_Normal,//���ڸı�ͼ�꣬��ʵ��case
}TCH_EVT;//4	����TouchEventHandler���¼�

typedef struct {//sizeof=8
	u16 x;	//����x����
	u16 y;	//����y����
	u16 Id;	//ֻ�а��²Żᵼ��id++,������ҳ�����ƥ��id���ж��ǲ���ͬһ�δ�������
	u16 TimeStamp;//ÿ��������ʱ�䣬���ֵ����λms
}TOUCH_INFO;//4		����TouchEventHandler�Ĳ���

//��һ����Ϊ��ֵ���ڶ�����Ϊ�����¼�����������Ϊ������Ϣ
typedef TCH_MSG (*TouchHandlerFunc)(u8 ,TCH_EVT , TOUCH_INFO *);
#ifdef QSYS_FRAME_FULL	
//��һ������Ϊ��ֵ���ڶ�������Ϊ��ǰֵ����ֵ�仯ʱ����
typedef TCH_MSG (*YesNoHandlerFunc)(u8 ,bool );
typedef TCH_MSG (*NumBoxHanderFunc)(u8 ,TCH_EVT ,bool ,TOUCH_INFO *);
typedef TCH_MSG (*StrOptBoxHandlerFunc)(u8 ,TCH_EVT ,bool ,TOUCH_INFO *);
typedef TCH_MSG (*StrInputBoxHandlerFunc)(u8 ,TCH_EVT ,bool ,TOUCH_INFO *);
#endif	

//4	���������OptionMask����ֵ(���֧��16������)
//ע:��ΪTouch�̺߳�Input�߳���Դ�������޵�Ե�ʣ������¼����������ڵ�8λ
#define PrsMsk (1<<0) //Press�¼����룬���ھ��������Ƿ���ӦPress�¼���������ͬ
#define CotMsk (1<<1) //Continue�¼�����
#define RelMsk (1<<2)  //Release�¼�����
#define ReVMsk (1<<3) //ReleaseVain�¼�����

//�ǰ����¼�����ɷŸ�λ
#define LandMsk (1<<4) //���ھ��������Ƿ��ú���ģʽ��ʾ���ֻ���ͼƬ
#define BinMsk		(1<<5) //������ʱ������ʹ��bmp�ļ�������ʹ��bin�ļ���ͼ�꣬��ʱͼ����������һ����
#define DbgMsk	(1<<6) //����ģʽ��������ʾ��������ͼƬ�����򲻻���ʾͼƬ��ֻ��ʾ����
#define F16Msk	(1<<7) //�ı����ִ�СΪ16x16
#define B14Msk	(1<<8) //�ı�����ΪB14���壬����Ascii�ַ���Ч
#define PathMsk (1<<9) //������λ��Чʱ��BmpPathPrefixָ�����ַ�����������ͼƬ��·��ǰ׺����λ��Чʱ��BmpPathPrefix���theme·����Ϊͼ��·����
#define RoueMsk (1<<10) //Round Edge Բ��
//#define CirbMsk 	(1<<11) //Circular Bead Բ�ǣ��ݲ�֧��

typedef struct {
	u8 *Name;//������ͼ�갴�������ڲ�����ʾͼ��ʱ����ʾ�����ְ���������������ʾ��
	
	//��ֵ,�����������İ���
	// 1.С��USER_KEY_VALUE_START�ļ�ֵ������ϵͳʹ�ã����������������ⲿ���������İ���
	// 2.���������ͼƬ�����������ʣ����ֵ��ΧΪUSER_KEY_VALUE_START-0xff��ע�ⲻ��ʾͼ��Ҳ������ͼƬ�������͡�
	// 3.��������ļ�ֵ������ͬ������ͼƬ���ʺ���������֮�䶼��������ͬ��ֵ�İ�����ͬһ��ֵ�İ�����������Ӧ��
	u8 ObjID;

	//���ڶ���һЩ���������ԣ������¼���
	// 1.�����¼������������ͺ�ͼƬ�Ͷ���Ч�����ھ�����������Ҫ��Ӧ��Щ�¼���ȡֵ��PrsMsk|CotMsk|RelMsk|ReVMsk
	// 2.����ѡ����������ͺ�ͼƬ�Ͷ���Ч�����ھ����������Ƿ������ʾ��ע�����ʱ��˼άת��
	// 3.����ģʽ�����������ͺ�ͼƬ�Ͷ���Ч��������ʾ�������򣬷������ʱʹ��
	// 4.���ִ�Сѡ����������ͺ�ͼƬ�Ͷ���Ч�����ھ����Ƿ����16x16�����ִ�С��Ĭ��Ϊ12x12
	// 5.Բ�Ǳ�����ֻ������������Ч���������ֱ����Ľ�ΪԲ��
	// 6.Բ�߱�����ֻ������������Ч���������ֱ�������ΪԲ��
	// ��������ѡ��ɹ��棬��ֵʱʹ��"|"���ţ���PrsMsk|LandMsk|F16Msk
	u16 OptionsMask;
	
	u16 x; 	//������������xֵ����Ļ���ϵ�Ϊ0��0���ꡣ����Ǻ���ģʽ������Ҫ�����ʾ����ָ����ʾ��������ϵ㡣
	u16 y;	//������������yֵ
	u16 w; //����������,��������Ǻ���ģʽ,ע������ʾ��������
	u16 h;	//��������߶�,��������Ǻ���ģʽ,ע������ʾ��������	
	
	u16 ImgX;// ��������ͼƬ����ʾ���xֵ,������������Ͻǵ����ֵ
	u16 ImgY;//��������ͼƬ����ʾ���yֵ,������������Ͻǵ����ֵ

	//����ͼ��·��ǰ׺��Ϊ�˽�ʡrom�ռ䣬1.2��֮��ָ��
	//����ͼ�����ͬǰ׺����ָ��ÿ�ֶ�����Ӧ�ĺ�׺��ĸ
	//����:���BmpPathprefix="Dir/SubDir/Button",NormalSuffix='N',
	//��õ���ͨ״̬����·��Ϊ"Dir/SubDir/ButtonN.bmp"
	u8 *BmpPathPrefix;//���Ҫ����60���ַ�
	//u8 NormalSuffix;//��ͨ״̬��׺�����ֽڣ����ָ��Ϊ0������ʾ
	//u8 PressSuffix;//����״̬��׺�����ֽڣ����ָ��Ϊ0������ʾ
	//u8 ReleaseSuffix;//�ͷ�״̬��׺�����ֽڣ����ָ��Ϊ0������ʾ
	//u8 ReserveU8;//����
	COLOR_TYPE TransColor; //32λ͸��ɫ��NO_TRANSP��ʾ����͸��ɫ
}IMG_TCH_OBJ;//4		��������������

typedef struct {
	u8 *Name;//������ͼ�갴�������ڲ�����ʾͼ��ʱ����ʾ�����ְ���������������ʾ��
	
	//��ֵ,�����������İ���
	// 1.С��0x40�ļ�ֵ������ϵͳʹ�ã����������������ⲿ���������İ���
	// 2.���������ͼƬ�����������ʣ����ֵ��ΧΪ0x40-0xff��ע�ⲻ��ʾͼ��Ҳ������ͼƬ�������͡�
	// 3.��������ļ�ֵ������ͬ������ͼƬ���ʺ���������֮�䶼��������ͬ��ֵ�İ�����ͬһ��ֵ�İ�����������Ӧ��
	u8 ObjID;

	//���ڶ���һЩ���������ԣ������¼���
	// 1.�����¼������������ͺ�ͼƬ�Ͷ���Ч�����ھ�����������Ҫ��Ӧ��Щ�¼���ȡֵ��PrsMsk|CotMsk|RelMsk|ReVMsk
	// 2.����ѡ����������ͺ�ͼƬ�Ͷ���Ч�����ھ����������Ƿ������ʾ��ע�����ʱ��˼άת��
	// 3.����ģʽ�����������ͺ�ͼƬ�Ͷ���Ч��������ʾ�������򣬷������ʱʹ��
	// 4.���ִ�Сѡ����������ͺ�ͼƬ�Ͷ���Ч�����ھ����Ƿ����16x16�����ִ�С��Ĭ��Ϊ12x12
	// 5.Բ�Ǳ�����ֻ������������Ч���������ֱ����Ľ�ΪԲ��
	// 6.Բ�߱�����ֻ������������Ч���������ֱ�������ΪԲ��
	// ��������ѡ��ɹ��棬��ֵʱʹ��"|"���ţ���PrsMsk|CotMsk|RelMsk|ReVMsk��
	u16 OptionsMask;
	
	u16 x; 	//������������xֵ����Ļ���ϵ�Ϊ0��0���ꡣ����Ǻ���ģʽ������Ҫ�����ʾ����ָ����ʾ��������ϵ㡣
	u16 y;	//������������yֵ
	u16 w; //����������,��������Ǻ���ģʽ,ע������ʾ��������
	u16 h;	//��������߶�,��������Ǻ���ģʽ,ע������ʾ��������	
	
	u8 CharX;// �����������ֵ���ʾ���xֵ,������������Ͻǵ����ֵ
	u8 CharY;//�����������ֵ���ʾ���yֵ,������������Ͻǵ����ֵ
	u8 Margin;//����λ��ʾ�������ҵ������԰ף�����λ��ʾ�������µ������԰ס�����ֵ��Ϊ���ô���������ڱ�������h��w��ȥ��Ӧ��Margin���ڱ���ɫʵ�ʵ�h��w��
	u8 Space;//����λ��ʾ�ּ�࣬����λ��ʾ�м��

	//TRANSPARENT��ʾʹ��͸��ɫ
	COLOR_TYPE NormalColor;//16λɫ�ʣ�ָ����ͨ״̬������ɫ
	COLOR_TYPE NormalBG;//16λɫ�ʣ�ָ����ͨ״̬����ɫ
	COLOR_TYPE PressColor;//16λɫ�ʣ�ָ������״̬������ɫ
	COLOR_TYPE PressBG;//16λɫ�ʣ�ָ������״̬����ɫ	
	COLOR_TYPE ReleaseColor;//16λɫ�ʣ�ָ���ͷ�״̬������ɫ
	COLOR_TYPE ReleaseBG;//16λɫ�ʣ�ָ���ͷ�״̬����ɫ
}CHAR_TCH_OBJ;//4		��������������

#ifdef QSYS_FRAME_FULL	
typedef struct {		
	u8 ObjID;//��ʶ����ҳ���ڱ���Ψһ�������������ؼ�����

	bool DefVal;//Ĭ��ֵ��TRUE����FALSE
	
	u16 x; 	//������������xֵ����Ļ���ϵ�Ϊ0��0���ꡣ����Ǻ���ģʽ������Ҫ�����ʾ����ָ����ʾ��������ϵ㡣
	u16 y;	//������������yֵ
}YES_NO_OBJ;//4		yes no�ؼ�����

typedef enum{
	NBT_Num,
	NBT_NumList,
	NBT_NumEnum,
}NUM_BOX_TYPE;

typedef struct{
	u8 ObjID;//��ʶ����ҳ���ڱ���Ψһ�������������ؼ�����
	NUM_BOX_TYPE Type;//num box����

	u16 x; 	//������������xֵ����Ļ���ϵ�Ϊ0��0���ꡣ����Ǻ���ģʽ������Ҫ�����ʾ����ָ����ʾ��������ϵ㡣
	u16 y;	//������������yֵ
	u16 w; //����������,��������Ǻ���ģʽ,ע������ʾ��������
	
	s32 Value;
}NUM_BOX_OBJ;//4		���������

typedef struct{
	u8 ObjID;//��ʶ����ҳ���ڱ���Ψһ�������������ؼ�����
	NUM_BOX_TYPE Type;//num box����

	u16 x; 	//������������xֵ����Ļ���ϵ�Ϊ0��0���ꡣ����Ǻ���ģʽ������Ҫ�����ʾ����ָ����ʾ��������ϵ㡣
	u16 y;	//������������yֵ
	u16 w; //����������,��������Ǻ���ģʽ,ע������ʾ��������

	s32 Value;
	s32 Max;
	s32 Min;
	s32 Step;
}NUM_LIST_BOX_OBJ;//4		���������

typedef struct{
	u8 ObjID;//��ʶ����ҳ���ڱ���Ψһ�������������ؼ�����
	NUM_BOX_TYPE Type;//num box����

	u16 x; 	//������������xֵ����Ļ���ϵ�Ϊ0��0���ꡣ����Ǻ���ģʽ������Ҫ�����ʾ����ָ����ʾ��������ϵ㡣
	u16 y;	//������������yֵ
	u16 w; //����������,��������Ǻ���ģʽ,ע������ʾ��������

	u8 Idx;//��ǰֵ����
	u8 Num;//��ǰֵ����
	u8 Total;//����������
	s32 EnumList[1];
}NUM_ENUM_BOX_OBJ;//4		���������

typedef struct{
	u8 ObjID;//��ʶ����ҳ���ڱ���Ψһ�������������ؼ�����
}STR_OPT_OBJ;//4		�ַ���ѡ���

typedef struct{
	u8 ObjID;//��ʶ����ҳ���ڱ���Ψһ�������������ؼ�����
}STR_INPUT_OBJ;//4		�ַ��������
#endif

typedef u8 REP_IMG_SUFX;

//api

//���ĵ�ǰ������ĳ����������ʾͼƬ
// 1.ֻ�Ե�ǰҳ����Ч,ת��ҳ�����ҳ���ʧЧ
// 2.ֻ��ָ����׺������ԭ������Դͼ����"MusicN.bmp"
//    ָ��Suffix='T'����ͼ����Դ���"MusicT.bmp"
//���Suffix=0����ָ�ԭʼͼ��
void Q_ChangeImgTchImg(u8 Key,u8 Suffix);

//��ChangeKeyImg��ԣ���ȡ��ǰ��ͼ���滻��׺ֵ
//����0��ʾ��Ĭ��ֵ
//���򷵻��滻�ĺ�׺
u8 Q_ReadImgTchImg(u8 Key);

//���ĵ�ǰ������ĳ�����ְ�������ʾ����
// 1.ֻ�Ե�ǰҳ����Ч,ת��ҳ�����ҳ���ʧЧ
//���NewName=NULL����ָ�ԭʼ����
void Q_ChangeCharTchName(u8 Key,u8 *NewName);

//��ChangeKeyName��ԣ���ȡ��ǰ��ͼ���滻��׺ֵ
//����NULL��ʾ��Ĭ��ֵ����key������Χ
u8 *Q_ReadCharTchName(u8 Key);

//���̳���ָ��������ֵ��ͼ������֣������д˼�ֵ�İ�������Ч
void Q_PresentTch(u8 Key,TCH_EVT Type);

//���������µĶ�̬ͼ�갴��
bool Q_SetDynamicImgTch(u8 Idx,IMG_TCH_OBJ *pTchReg);

//���������µĶ�̬���ְ���
bool Q_SetDynamicCharTch(u8 Idx,CHAR_TCH_OBJ *pTchReg);

#ifdef QSYS_FRAME_FULL
bool Q_SetYesNo(u8 Idx,YES_NO_OBJ *pYesNo);
#endif

#endif

