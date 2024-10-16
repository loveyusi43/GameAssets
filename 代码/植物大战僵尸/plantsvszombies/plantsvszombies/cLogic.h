#pragma once
#include "list"
#include "cMyTimer.h"
using namespace std;

//QQ资料群：431493983 
//蔡老师QQ：1939008478
//VIP服务老师QQ：921700006

#define GetFruitTime 10000	//花出金币频率
#define ShootTime	2000	//发射子弹频率

enum ObjectType
{
	Type_Flower,	//花
	Type_Gun,		//枪
	Type_Zombie,	//僵尸
	Type_Bullet,	//子弹
	Type_Num
};

enum Timer_Type
{
	Timer_Draw,	//绘制定时器
	Timer_Shoot,	//检测发射的时器
	Timer_Move,		//控制移动
	Timer_CreateZombie,	//创建僵尸
	Timer_CheckAddMoney,	//检测加金币
};

struct stObjectInfo
{
	int x;	//X坐标
	int y;	//Y坐标
	ObjectType type;
	unsigned speed;//对于花：结金币的频率，枪：发射子弹的频率，对于僵尸和子弹 移动的速度
	unsigned lastActionTime;	//上次动作时间
	short curPicIndex;			//当前动画播放到哪一帧
	unsigned hpValue;			//僵尸的血量
	unsigned attackValue;		//子弹的攻击力
	stObjectInfo()
	{
		x = 0;
		y = 0;
		type = Type_Num;
		lastActionTime = 0;
		hpValue = 100;
		attackValue = 20;
		curPicIndex = 0;
		speed = 10;
	}
};

typedef list<stObjectInfo> ObjectList;
typedef list<stObjectInfo>::iterator itObjectList;

class cLogic:public cMyTimer
{
public: 
	cLogic(){}
	~cLogic(){}

	void Begin(HWND hWnd);	//游戏开始
	void GameEnd();		//游戏结束

	virtual int OnTimer(int id,int iParam,string str);

	void Draw();	//绘制
	void DrawBackPlants(HDC hDc);	//画背景
	void DrawFlowerAndGun(HDC hDc);	//画枪和花
	void DrawBullet(HDC hDc);		//画子弹
	void DrawZombies(HDC hDc);		//画僵尸
	void DrawText(HDC hDc);			//画文字
	void DrawMouseItem(HDC hDc);	//绘制鼠标物品

	void Shoot();	//发射
	void Move();	//移动  把子弹移动和僵尸移动全写在这里面
	void RandCreateZombie();	//创建僵尸
	void CheckAttack();	//检测攻击
	void CheckGetMoney();	//检测加钱

	void LButtonUp();	//左键点击
	void RButtonUp();	//右键点击
	ObjectType GetInRect();
	bool CanPut(int& x,int& y);
private:
	unsigned m_iMoney;	//当前金币数
	ObjectList m_listFlower;
	ObjectList m_listGun;
	ObjectList m_listZombies;
	ObjectList m_listBullet;	

	HWND m_hWnd;

	ObjectType m_MouseType;	//当前鼠标上是什么东西
};