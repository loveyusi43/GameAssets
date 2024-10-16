#pragma once
#include "cMyTimer.h"
#include "list"
using namespace std;

#define Monster_Type_Max	4	//一共多少种怪

#define Player_Max_HP	1000
#define PlayerMoveSpeed	10		//人物移动速度
#define FireMoveSpeed	15		//炮速度
#define MonsterMoveSpeed 10		//怪物的移动速度
#define FireAttackValue		20	//炮的攻击力

#define Timer_Draw 1	//绘制的定时器
#define Timer_Move	2	//移动定时器
#define TImer_Fire	3	//开火定时器
#define Timer_CreateMonster	4	//创建怪物定时器

enum MoveDir
{
	Dir_Up,
	Dir_Down,
	Dir_Left,
	Dir_Right,
	Dir_Num
};

//炮弹，怪，子弹
struct stObject
{
	int x;
	int y;
	int attackValue;
	int curHp;
	int monsterType;
};

typedef list<stObject> ListObject;
typedef list<stObject>::iterator itListObject;

class cLogic:public cMyTimer
{
public:
	cLogic();
	~cLogic();

	virtual int OnTimer(int id, int iParam=0, string str="");

	void Begin(HWND hWnd);


	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);

	void DrawAll();	//绘制所有内容
	void DrawMap(HDC hDc);	//绘制地图
	void DrawPlayer(HDC hDc);		//绘制玩家
	void DrawHpPrograss(HDC hDc);	//绘制血量进度条
	void DrawFire(HDC hDc);		//绘制炮弹
	void DrawMonster(HDC hDc);	//绘制怪物

	void Move();	//移动
	void PlayerMove();	//人物移动
	void Fire();	//开火
	void FireMove();	//移动炮弹
	void RandCreateMonster();	//创建怪物
	void CreateMonster(int type);	//创建某一队形的怪
	void MonsterMove();		//怪物移动
	void AttackCheck();	//攻击检测
private:
	HWND m_hWnd;

	tagPOINT m_playerPos;	//人物当前位置
	int m_curHp;			//当前血量

	bool m_bIsKeyDown;	//当前是否已按下某个键
	MoveDir m_curDir;	//当前移动的方向

	ListObject m_listFire;	//炮弹
	ListObject m_listMonster;	//怪物

	HBITMAP m_hMonsterBimap[Monster_Type_Max];	//怪物的位图
};

