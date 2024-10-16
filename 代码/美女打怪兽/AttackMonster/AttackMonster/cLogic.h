#pragma once
#include "cMyTimer.h"
#include "list"
using namespace std;

#define Monster_Type_Max	4	//һ�������ֹ�

#define Player_Max_HP	1000
#define PlayerMoveSpeed	10		//�����ƶ��ٶ�
#define FireMoveSpeed	15		//���ٶ�
#define MonsterMoveSpeed 10		//������ƶ��ٶ�
#define FireAttackValue		20	//�ڵĹ�����

#define Timer_Draw 1	//���ƵĶ�ʱ��
#define Timer_Move	2	//�ƶ���ʱ��
#define TImer_Fire	3	//����ʱ��
#define Timer_CreateMonster	4	//�������ﶨʱ��

enum MoveDir
{
	Dir_Up,
	Dir_Down,
	Dir_Left,
	Dir_Right,
	Dir_Num
};

//�ڵ����֣��ӵ�
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

	void DrawAll();	//������������
	void DrawMap(HDC hDc);	//���Ƶ�ͼ
	void DrawPlayer(HDC hDc);		//�������
	void DrawHpPrograss(HDC hDc);	//����Ѫ��������
	void DrawFire(HDC hDc);		//�����ڵ�
	void DrawMonster(HDC hDc);	//���ƹ���

	void Move();	//�ƶ�
	void PlayerMove();	//�����ƶ�
	void Fire();	//����
	void FireMove();	//�ƶ��ڵ�
	void RandCreateMonster();	//��������
	void CreateMonster(int type);	//����ĳһ���εĹ�
	void MonsterMove();		//�����ƶ�
	void AttackCheck();	//�������
private:
	HWND m_hWnd;

	tagPOINT m_playerPos;	//���ﵱǰλ��
	int m_curHp;			//��ǰѪ��

	bool m_bIsKeyDown;	//��ǰ�Ƿ��Ѱ���ĳ����
	MoveDir m_curDir;	//��ǰ�ƶ��ķ���

	ListObject m_listFire;	//�ڵ�
	ListObject m_listMonster;	//����

	HBITMAP m_hMonsterBimap[Monster_Type_Max];	//�����λͼ
};

