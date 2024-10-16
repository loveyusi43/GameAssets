#pragma once
#include "cMyTimer.h"

//果冻行列数
#define Max_Row 7
#define Max_Rank 9

enum Timer_Type
{
	Timer_Draw	//绘制定时器
};

//格子类型（果冻类型)
enum JellyType
{
	JellyType1,
	JellyType2,
	JellyType3,
	JellyType4,
	JellyType5,
	JellyType6,
	JellyType7,
	JellyTypeNum,
	JellyDes,	//要摧毁的目标
};

class cJelly:public cMyTimer
{
public:
	cJelly(HWND hWnd);
	~cJelly(void);

	//绘制函数
	void Draw();
	//绘制果冻
	void DrawJelly(HDC memDc);

	//定时器回调函数
	virtual int OnTimer(int id,int iParam,string str);

	//初始化游戏数据
	void InitGame();
	//鼠标事件响应
	void OnLButtonDown(LPARAM lParam);

	void OnLButtonUp(LPARAM lParam);

	void OnMouseMove(LPARAM lParam);

	//判断当前鼠标所在行，列是否在被拖动格子的旁边
	bool IsNearDragJelly(int row,int rank);
	//判断是否有能消除的
	bool CheckClear(JellyType jellyInfo[][Max_Rank]);
private:
	HWND m_hWnd; //窗口句柄

	JellyType m_JellyArray[Max_Row][Max_Rank];	//保存每个格子是什么
	bool m_bIsButtonDown;	//鼠标是否是按下状态
	tagPOINT m_posCurDragJelly;	//当前拖动的果冻的行列x- 行  y-列
};

