#pragma once
#include "cMyTimer.h"
#define Max_Player_Num	4 //总玩家人数
#define Max_Card_Num 52	//总牌数
#define Per_Player_CardNum	3 //每个人牌的张数
#define Max_Think_Time	30	//每个人最多思考

enum Game_Status
{
	Stauts_Flush,	//洗牌
	Stauts_Send,	//发牌
	Status_Play,	//游戏中
	Status_Result	//游戏结束结算中
};

enum Timer_Type
{
	Timer_Draw,	//绘制
	Timer_Flush,	//洗牌
	Timer_Send,	//发牌
	Timer_TimeOut,	//游戏中的倒计时
	Timer_RobotAction
};

typedef struct stPlayerInfo
{
	char playerName[20];	//名字
	int allMoney;			//金币
	int curPushMoney;		//当局下注金币数
	char iCard[Per_Player_CardNum];	//牌
	bool bGiveUp;			//是否已出局
}ST_PLAYERINFO;
//FPS
class cZaJinHua:public cMyTimer
{
public:
	cZaJinHua(HWND hWnd);
	~cZaJinHua(void);

	//游戏开始
	void GameBegin();
	//这一把游戏结束
	void GameOver();

	//洗牌
	void FlushCard();
	//发牌
	void SendCard();
	//玩家动作
	void PlayerAction(); 
	//获得下一位说话玩
	int GetNextActionPlayer();
	//机器人AI
	void RobotAction();
	//获得剩余未出局玩家数
	int GetLeavePlayerNum();

	//绘制
	void DrawAll();
	//绘制扑克
	void DrawPoke(HDC dcMem);

	//定时器回调函数
	virtual int OnTimer(int id,int iParam,string str);

	//获得花色
	int GetCardColor(char iCard);
	//获得大小
	int GetCardNum(char iCard);
private:
	HWND m_hWnd;
	Game_Status m_gameStatus;	//保存当前游戏状态
	ST_PLAYERINFO m_playerInfo[Max_Player_Num];	//所有玩家信息
	int m_curActionPlayer;	//保存当前动作玩家
	int m_actionLeaveTime;	//剩余思考时间
};

