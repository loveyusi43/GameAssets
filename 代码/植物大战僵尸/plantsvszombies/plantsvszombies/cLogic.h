#pragma once
#include "list"
#include "cMyTimer.h"
using namespace std;

//QQ����Ⱥ��431493983 
//����ʦQQ��1939008478
//VIP������ʦQQ��921700006

#define GetFruitTime 10000	//�������Ƶ��
#define ShootTime	2000	//�����ӵ�Ƶ��

enum ObjectType
{
	Type_Flower,	//��
	Type_Gun,		//ǹ
	Type_Zombie,	//��ʬ
	Type_Bullet,	//�ӵ�
	Type_Num
};

enum Timer_Type
{
	Timer_Draw,	//���ƶ�ʱ��
	Timer_Shoot,	//��ⷢ���ʱ��
	Timer_Move,		//�����ƶ�
	Timer_CreateZombie,	//������ʬ
	Timer_CheckAddMoney,	//���ӽ��
};

struct stObjectInfo
{
	int x;	//X����
	int y;	//Y����
	ObjectType type;
	unsigned speed;//���ڻ������ҵ�Ƶ�ʣ�ǹ�������ӵ���Ƶ�ʣ����ڽ�ʬ���ӵ� �ƶ����ٶ�
	unsigned lastActionTime;	//�ϴζ���ʱ��
	short curPicIndex;			//��ǰ�������ŵ���һ֡
	unsigned hpValue;			//��ʬ��Ѫ��
	unsigned attackValue;		//�ӵ��Ĺ�����
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

	void Begin(HWND hWnd);	//��Ϸ��ʼ
	void GameEnd();		//��Ϸ����

	virtual int OnTimer(int id,int iParam,string str);

	void Draw();	//����
	void DrawBackPlants(HDC hDc);	//������
	void DrawFlowerAndGun(HDC hDc);	//��ǹ�ͻ�
	void DrawBullet(HDC hDc);		//���ӵ�
	void DrawZombies(HDC hDc);		//����ʬ
	void DrawText(HDC hDc);			//������
	void DrawMouseItem(HDC hDc);	//���������Ʒ

	void Shoot();	//����
	void Move();	//�ƶ�  ���ӵ��ƶ��ͽ�ʬ�ƶ�ȫд��������
	void RandCreateZombie();	//������ʬ
	void CheckAttack();	//��⹥��
	void CheckGetMoney();	//����Ǯ

	void LButtonUp();	//������
	void RButtonUp();	//�Ҽ����
	ObjectType GetInRect();
	bool CanPut(int& x,int& y);
private:
	unsigned m_iMoney;	//��ǰ�����
	ObjectList m_listFlower;
	ObjectList m_listGun;
	ObjectList m_listZombies;
	ObjectList m_listBullet;	

	HWND m_hWnd;

	ObjectType m_MouseType;	//��ǰ�������ʲô����
};