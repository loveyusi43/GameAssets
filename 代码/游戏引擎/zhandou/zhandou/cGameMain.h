#pragma once
#include "cGameSenceBase.h"
#define MAP_MOVE_STEP 10

enum Game_Timer_Type
{
	Timer_MoveMap,
};

enum MoveDir
{
	Dir_Left,
	Dir_LeftDown,
	Dir_Down,
	Dir_DownRight,
	Dir_Right,
	Dir_RightUp,
	Dir_Up,
	Dir_UpLeft,
	Dir_Num
};

class cGameMain:public cMyTimer
{
public:
	cGameMain(void);


	~cGameMain(void);

	virtual int OnTimer(int id,int iParam,string str);

	void Init(HWND hWnd);
	void MoveMap();   //�ƶ��Ӿ���ͼ
	void GetNextPosByDir(tagPOINT& ptRes,MoveDir dir);	//����ƶ����λ��
private:
	HWND m_hWnd;
	cGameSenceBase* m_pSence;
	ST_ANIPICTURE* m_pMap;

	tagPOINT m_seeSencePt;		//�Ӿ��������꣨����������������ͼ������㣩
};

