#pragma once
#include "cMyTimer.h"
#include "vector"
#include "list"
#include "string"

using namespace std;
enum Timer_Type
{
	Timer_Draw
};

enum Layer_Main_Type
{
	Layer_Main_Map,				//��ͼ
	Layer_Main_Player,			//����������Ч
	Layer_Main_UI,				//UI
	Layer_Main_Num
};

typedef struct stAniPicture
{
	bool bAni;			//��ʶ�Ƕ�������ͼƬ����̬ͼ��
	tagPOINT ptPos;		//λ��
	vector<HBITMAP> bimapVec;	//λͼ�б�
	int curIndex;				//����ڶ����ģ���ǰ���ŵĶ���֡����
	DWORD rate;					//����֡��Ƶ��
	DWORD lastSwitchTime;		//�ϴ��л�����֡��ʱ�䣬����
	//ͼ�㣬������˭���Ե�ס˭
	Layer_Main_Type layerMain;	//ͼ��
	UINT transparentRGB;			//͸��ɫ

	stAniPicture()
	{
		bAni = false;
		curIndex = 0;
		rate = 50;
		lastSwitchTime = 0;
		layerMain = Layer_Main_Num;
		transparentRGB = RGB(0,0,0);
	}

}ST_ANIPICTURE;

typedef struct stBitmapInfo
{
	wstring strPath;
	HBITMAP hBitmap;
}ST_BITMAPINFO;

class cGameSenceBase:public cMyTimer
{
public:
	cGameSenceBase(void);
	~cGameSenceBase(void);

	

	void Init(HWND hWnd);

	virtual int OnTimer(int id,int iParam,string str);

	void Draw();
	HBITMAP GetBitmapFromList(wstring picPath);
	ST_ANIPICTURE* AddPicture(wstring picPath,tagPOINT pt,Layer_Main_Type layer,UINT rgb=RGB(0,0,0));	//���һ��ͼƬ
	ST_ANIPICTURE* AddAnimation(list<wstring> listPath,tagPOINT pt,Layer_Main_Type layer,
		UINT rgb= RGB(0,0,0),int rate= 50);
private:
	HWND m_hWnd;
	list<ST_BITMAPINFO> m_listBitmapInfo;					//�����Ѿ����ع�����Դ
	list<ST_ANIPICTURE> m_listAniPic[Layer_Main_Num];		//�����Ӧͼ��Ķ���
};

