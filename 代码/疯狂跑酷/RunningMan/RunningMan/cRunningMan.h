#pragma once
#include "cMyTimer.h"
#include "list"
using namespace std;

enum Type_Timer
{
	Timer_Draw,			//绘制的定时器
	Timer_ControlJump,
};

enum PlayerStatus
{
	Status_Run,
	Status_Jump,
	Status_Roll,
	Status_Fall,
	Status_Slip
};

class cRunning: public cMyTimer
{
public:
	cRunning();
	~cRunning();
	virtual int OnTimer(int id,int iParam=0,string str="");

	void BeginGame(HWND hWnd);
	void GameOver();

	void DrawAll();
	void DrawMap(HDC hDc);
	void DrawPlayer(HDC hDc);
	void DrawObjects(HDC hDc);

	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);

	void ControlJump();		//控制跳跃
	void CheckCollide();	//碰撞检测
	
private:
	HWND m_hWnd;

	int m_curMapStep;	//标识背景向左移动了多少  0-960

	PlayerStatus m_curPlayerStatus;	//人物当前的状态
	int m_curActionIndex;			//人物的当前动作帧 

	tagPOINT m_curPlayerPos;		//人物当前坐标点

	bool m_bIsKeyDown;		//是否已经按下方向键

	unsigned m_usJumpTime;	//记录跳跃开始时间

	list<tagPOINT> m_listPost;	//柱子
	list<tagPOINT> m_listBase;	//盘子
	list<tagPOINT> m_listGold;	//金币

	unsigned m_iMoney;			//金币
};