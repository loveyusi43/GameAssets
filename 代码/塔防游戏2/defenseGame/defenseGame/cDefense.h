#pragma once
#include "cMyTimer.h"

#define Max_Building_Num 2

enum Timer_Type
{
	Timer_Draw,
	Timer_AddMonster,
	Timer_MonsterMove,
	Timer_Attack
};

enum BuildingType
{
	Building_Type1,
	Building_Type2,
	Building_Num
};

enum MonsterType
{
	Monster_Type1,
	Monster_Type2
};

struct stBuildingInfo
{
	tagPOINT pt;
	BuildingType type;
	int curIndex;
};

struct stMonsterInfo
{
	tagPOINT pt;
	int curHp;
	MonsterType type;
	int speed;
	int curPicIndex;	//��ǰ����֡
	unsigned id;		//�����ID
	int curDesPtIndex;	//���ߵ�Ŀ���
};

struct stBulletInfo
{
	tagPOINT pt;
	tagPOINT ptDes;
	unsigned monsterId;
	BuildingType type;
};

typedef list<stBuildingInfo> BuildList;
typedef list<stMonsterInfo> MonsterList;
typedef list<stBulletInfo> BulletList;

class cDefense:public cMyTimer 
{
public:
	cDefense();
	~cDefense();

	virtual int OnTimer(int id,int iParam=0,string str="");

	void BeginGame(HWND hWnd);
	void GameOver();
	void DrawAll();
	void DrawMap(HDC dcMem);
	void InitBase();
	void DrawItem(HDC dcMem);
	void OnLButtonUp(LPARAM lParam);
	void OnRButtonDown(LPARAM lParam);
	bool  IsInRect(tagPOINT pt,RECT rect);
	int GetBaseIndex(tagPOINT pt);
	void AddBuilding(int baseIndex);
	void AddMonster();
	void MonsterMove();
	float Distance(tagPOINT pt1,tagPOINT pt2);
	void DrawMonster(HDC dcMem);
	void BuildingAttack();
	bool GetNearestMonster(stBuildingInfo stBuild,stMonsterInfo& stMonster);
	void BulletMove();
	void AttackMonster(int id,BuildingType type);
	void DrawBullet(HDC dcMem);
private:
	HWND m_hWnd;
	list<tagPOINT> m_ptBaseList;	//�ػ�
	RECT m_rectBuilding[Max_Building_Num];	//ͼ�� rect

	int m_mouseItemIndex;	//��ʶ��ʶ��������ʲô

	int m_iMoney;

	BuildList m_buildList;
	MonsterList m_monsterList;
	BulletList m_bulletList;
};