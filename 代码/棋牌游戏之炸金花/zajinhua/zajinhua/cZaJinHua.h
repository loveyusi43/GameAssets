#pragma once
#include "cMyTimer.h"
#define Max_Player_Num	4 //���������
#define Max_Card_Num 52	//������
#define Per_Player_CardNum	3 //ÿ�����Ƶ�����
#define Max_Think_Time	30	//ÿ�������˼��

enum Game_Status
{
	Stauts_Flush,	//ϴ��
	Stauts_Send,	//����
	Status_Play,	//��Ϸ��
	Status_Result	//��Ϸ����������
};

enum Timer_Type
{
	Timer_Draw,	//����
	Timer_Flush,	//ϴ��
	Timer_Send,	//����
	Timer_TimeOut,	//��Ϸ�еĵ���ʱ
	Timer_RobotAction
};

typedef struct stPlayerInfo
{
	char playerName[20];	//����
	int allMoney;			//���
	int curPushMoney;		//������ע�����
	char iCard[Per_Player_CardNum];	//��
	bool bGiveUp;			//�Ƿ��ѳ���
}ST_PLAYERINFO;
//FPS
class cZaJinHua:public cMyTimer
{
public:
	cZaJinHua(HWND hWnd);
	~cZaJinHua(void);

	//��Ϸ��ʼ
	void GameBegin();
	//��һ����Ϸ����
	void GameOver();

	//ϴ��
	void FlushCard();
	//����
	void SendCard();
	//��Ҷ���
	void PlayerAction(); 
	//�����һλ˵����
	int GetNextActionPlayer();
	//������AI
	void RobotAction();
	//���ʣ��δ���������
	int GetLeavePlayerNum();

	//����
	void DrawAll();
	//�����˿�
	void DrawPoke(HDC dcMem);

	//��ʱ���ص�����
	virtual int OnTimer(int id,int iParam,string str);

	//��û�ɫ
	int GetCardColor(char iCard);
	//��ô�С
	int GetCardNum(char iCard);
private:
	HWND m_hWnd;
	Game_Status m_gameStatus;	//���浱ǰ��Ϸ״̬
	ST_PLAYERINFO m_playerInfo[Max_Player_Num];	//���������Ϣ
	int m_curActionPlayer;	//���浱ǰ�������
	int m_actionLeaveTime;	//ʣ��˼��ʱ��
};

