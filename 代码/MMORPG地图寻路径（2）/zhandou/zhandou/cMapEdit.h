#pragma once
#include "cMyTimer.h"

#define LatticeLen 100	//格子的边长
#define Map_MoveStep 20	//每次地图移动像素

class cMapEdit:public cMyTimer
{
public:
	cMapEdit(HWND hWnd = NULL);
	~cMapEdit(void);

	virtual int OnTimer(int id,int iParam,string str);
	
	void Draw();
	void LoadMap();	//加载地图资源图片
	void DrawObstruct(HDC dcMem);	//绘制阻挡
	void MoveMap(WPARAM wParam);	//移动地图位置
	void SetObstruct(LPARAM lParam);	//设置一个阻挡点
	void CancelObstruct(LPARAM lParam);	//取消一个阻挡点

	void SaveFile();	//保存文件
	void OpenFile();	//打开文件
	void SaveFileToPath(wchar_t* filePath);	//保存
	void LoadFileFromPath(wchar_t* filePath);	//加载

	int GetRow(tagPOINT pt);
	int GetRank(tagPOINT pt);

	list<tagPOINT> m_listObstruct;	//阻挡点的列表
	int m_mapRow;	//行
	int m_mapRank;	//列

private:
	HWND m_hWnd;
	wchar_t m_mapPath[MAX_PATH];

	HBITMAP m_mapBitmap;	//用来绘制

	tagPOINT m_ptClientToMap;	//窗口客户区相对于地图的坐标

	

	
};

