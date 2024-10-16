#pragma once
#include "cMyTimer.h"

#define Max_Building_Num 2

enum Timer_Type
{
	Timer_Draw,
	Timer_AddMonster
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
};

typedef list<stBuildingInfo> BuildList;
typedef list<stMonsterInfo> MonsterList;

class cDefense:public cMyTimer 
{
public:
	cDefense();
	~cDefense();

	virtual int OnTimer(int id,int iParam=0,string str="");

	void BeginGame(HWND hWnd);
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
private:
	HWND m_hWnd;
	list<tagPOINT> m_ptBaseList;	//�ػ�
	RECT m_rectBuilding[Max_Building_Num];	//ͼ�� rect

	int m_mouseItemIndex;	//��ʶ��ʶ��������ʲô

	int m_iMoney;

	BuildList m_buildList;
	MonsterList m_monsterList;
};