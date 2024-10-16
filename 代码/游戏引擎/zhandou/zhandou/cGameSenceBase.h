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
	Layer_Main_Map,				//地图
	Layer_Main_Player,			//人物，建筑物，特效
	Layer_Main_UI,				//UI
	Layer_Main_Num
};

typedef struct stAniPicture
{
	bool bAni;			//标识是动画还是图片（静态图）
	tagPOINT ptPos;		//位置
	vector<HBITMAP> bimapVec;	//位图列表
	int curIndex;				//针对于动画的，当前播放的动画帧索引
	DWORD rate;					//动画帧的频率
	DWORD lastSwitchTime;		//上次切换动画帧的时间，毫秒
	//图层，决定了谁可以挡住谁
	Layer_Main_Type layerMain;	//图层
	UINT transparentRGB;			//透明色

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
	ST_ANIPICTURE* AddPicture(wstring picPath,tagPOINT pt,Layer_Main_Type layer,UINT rgb=RGB(0,0,0));	//添加一张图片
	ST_ANIPICTURE* AddAnimation(list<wstring> listPath,tagPOINT pt,Layer_Main_Type layer,
		UINT rgb= RGB(0,0,0),int rate= 50);
private:
	HWND m_hWnd;
	list<ST_BITMAPINFO> m_listBitmapInfo;					//保存已经加载过的资源
	list<ST_ANIPICTURE> m_listAniPic[Layer_Main_Num];		//保存对应图层的对象
};

