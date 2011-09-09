/* ��ϵͳ ˵��
�����ϵͳ���κ�ҳ������ȵ����ļ���ע�ᣬע����2��:
1.��ҳ���ͷ�ļ������ڴ��ļ���
2.��ҳ������Խṹ��PAGE_ATTRIBUTE������AllPage�ṹ����

��Ҫע�����:
1.AllPage��ÿ��ҳ������е�����id������һ����Ա��
��id���ǳ�֮Ϊע��id(RegID or RID)��RID�����˲�ṹ��Ϣ�����棬�����ṹ
����0�ǵ�һ�㣬0.1���ǵڶ����һ���ڵ㣬0.1.2���ǵ�����ڶ����ڵ�
ĿǰRegID��Ψһ���þ���������ע�Ͳ���ҳ�棬����ֻ��ҪΨһ�Լ���
���ǽ��䶨��Ϊ���ṹ��Ϊ���Ժ�����չ����Ҫ

2.ϵͳ��ÿ��ҳ�涼�����Ƕ����ģ���һ��ҳ���Դ�벻�ܵ�����һ��
ҳ��ĺ����������ͱ�֤�˾������ҳ��ɾ����Ҳ����Ӱ������ҳ�档
��������Կ�����ϵͳ�ĺܶ�ҳ���ﶼ�ظ�����ͬ�����ܵĺ�����
���⣬ҳ���е��Զ��庯����ȫ�ֱ����������staticǰ׺����������Ϊ��
���õı�����Դ��ѧ��c++�����ѿ��Խ�ҳ�濴�ɶ���

3.��ϵͳ�У�ĳЩҳ����Ҫ���ظ����ã����ļ��б�ҳ�棬��ʱ������
���Խ���ҳ������Ͷ���ΪREG_SUB_PAGE_TYPE������REG_PAGE_TYPE�����ʾ���
ҳ���ǿ��Ա����ҳ�����(����)�ġ���ʵ���ϣ���ҳ��Ҳֻ����������
ҳ��ʹ�����䶨������ͣ���û���ñ��ҳ��ʹ���亯�������ô�ҳ��
�Ĳ������ݣ�ͨ��GotPage��ɡ�

4.�ڴ��ļ����ҳ�����ע��󣬲�����ʾϵͳ������ʾ���¼ӵ�ҳ�档
����Ҫ������ҳ���е���GotoPage()�����ſ��Խ����ҳ��
*/

#ifndef QSYS_ALL_PAGE_H
#define QSYS_ALL_PAGE_H

#include "MainPage.h"
#include "MusicPage.h"
#include "FileListPage.h"
#include "EBookPage.h"
#include "PicturePage.h"
#include "SettingsPage.h"
#include "QWebPage.h"
#include "TestPage.h"
#include "AppListPage.h"
#include "KeyBoardPage.h"
#include "SnakePage.h"
#include "TouchCheckPage.h"
#include "ChatPage.h"
#include "NewsPage.h"

const PAGE_CONGREGATION AllPage[]={//ҳ���������ô���255��RID����Ϊ��
	{0x1,&MainPage},// 1
	{0x11,&MusicPage},// 1.1
	{0x111,&FileListPage},// 1.1.1
	{0x12,&EBookPage},// 1.2
	{0x13,&PicturePage},// 1.3
	{0x14,&SettingsPage},// 1.4
	{0x15,&TestPage},
	{0x16,&KeyBoardPage},
	{0x17,&QWebPage},
	{0x18,&ChatPage},
	{0x19,&NewsPage},
	{0x2,&AppListPage},
	{0x21,&SnakePage},
	{0x23,&TouchCheckPage},
};

#define PAGE_TOTAL (sizeof(AllPage)/sizeof(PAGE_CONGREGATION))//ҳ��������������255
const u8 gPageTotal=PAGE_TOTAL;

#endif

