#pragma once
#include "cMyTimer.h"
#include "list"
using namespace std;

enum Type_Timer
{
	Timer_Draw,			//���ƵĶ�ʱ��
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

	void ControlJump();		//������Ծ
	void CheckCollide();	//��ײ���
	
private:
	HWND m_hWnd;

	int m_curMapStep;	//��ʶ���������ƶ��˶���  0-960

	PlayerStatus m_curPlayerStatus;	//���ﵱǰ��״̬
	int m_curActionIndex;			//����ĵ�ǰ����֡ 

	tagPOINT m_curPlayerPos;		//���ﵱǰ�����

	bool m_bIsKeyDown;		//�Ƿ��Ѿ����·����

	unsigned m_usJumpTime;	//��¼��Ծ��ʼʱ��

	list<tagPOINT> m_listPost;	//����
	list<tagPOINT> m_listBase;	//����
	list<tagPOINT> m_listGold;	//���

	unsigned m_iMoney;			//���
};