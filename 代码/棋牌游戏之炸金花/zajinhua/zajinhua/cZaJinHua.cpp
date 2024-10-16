#include "StdAfx.h"
#include "cZaJinHua.h"


//������  ɢ��<����<˳��<ͬ��<ͬ��˳<����
//���ȵ�����-��ϴ��-������
//�ĸ���ɫ  ÿ����ɫ A 2,3,4,5,6,7,8,9,10,J,Q,K     
//0x 1 1       0x0d
//  00000001    00001101  
//  00000000 00000000 00000000  0000 0000
//								��ɫ ��ֵ
char g_iCard[Max_Card_Num] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d
};

tagPOINT g_ptCardPos[Max_Player_Num];	//�ĸ����˿˵�λ����Ϣ


cZaJinHua::cZaJinHua(HWND hWnd)
{
	m_hWnd = hWnd;
	m_gameStatus = Stauts_Flush;
	AddTimer(Timer_Draw,50);
	m_curActionPlayer = -1;
	m_actionLeaveTime = Max_Think_Time;
	for (int i = 0;i<Max_Player_Num;i++)
	{
		m_playerInfo[i].allMoney = 100000;
		m_playerInfo[i].bGiveUp = false;
		m_playerInfo[i].curPushMoney = 0;
		memset(m_playerInfo[i].iCard,0,3);
		if (i == 0)
		{
			strcpy(m_playerInfo[i].playerName,"����˭");
		}
		else
		{
			sprintf(m_playerInfo[i].playerName,"����%d",i);
		}
	}

	//��ʼ���ĸ����Ƶ�λ��
	g_ptCardPos[0].x = 427;
	g_ptCardPos[0].y = 470;
	g_ptCardPos[1].x = 728;
	g_ptCardPos[1].y = 323;
	g_ptCardPos[2].x = 427;
	g_ptCardPos[2].y = 254;
	g_ptCardPos[3].x = 168;
	g_ptCardPos[3].y = 323;
}


cZaJinHua::~cZaJinHua(void)
{
	
}

void cZaJinHua::GameBegin()
{
	//ϴ��
	AddTimer(Timer_Flush,2000);
}

void cZaJinHua::GameOver()
{

}

int cZaJinHua::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_Draw:
		DrawAll();
		break;
	case Timer_Flush:
		FlushCard();
		AddTimer(Timer_Send,2000);
		return 0;
	case Timer_Send:
		SendCard();
		PlayerAction();	//ĳ�����˵����������
		AddTimer(Timer_TimeOut,1000);
		return 0;
	case Timer_TimeOut:
		m_actionLeaveTime--;
		if (m_actionLeaveTime <= 0)
		{
			//��ʱ�ˣ���Ϊ�������¸���˵��
			m_playerInfo[m_curActionPlayer].bGiveUp = true;
			PlayerAction();
			m_actionLeaveTime = Max_Think_Time;
		}
		break;
	case Timer_RobotAction:
		RobotAction();
		//PlayerAction();	//ĳ�����˵����������
		break;
	}
	return 1;
}

void cZaJinHua::DrawAll()
{
	HDC hDc = GetDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}
	//�Ȼ��Ƶ�һ������DC���ٴӻ���DC���Ƶ��豸DC��hDc)
	//����һ������DC
	HDC dcMem = CreateCompatibleDC(hDc);
	RECT rect;
	GetClientRect(m_hWnd,&rect);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,rect.right,rect.bottom);
	SelectObject(dcMem,hBitmap);

	//���Ʊ���
	HBITMAP hBitmapBack = (HBITMAP)LoadImageA(NULL,"images//Back.bmp",
		IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	HDC dcTemp = CreateCompatibleDC(NULL);
	SelectObject(dcTemp,hBitmapBack);
	BitBlt(dcMem,0,0,rect.right,rect.bottom,dcTemp,0,0,SRCCOPY);

	//�ƣ�����ʱ��������Ϣ���������ж��ٳ����
	DrawPoke(dcMem);

	
	BitBlt(hDc,0,0,rect.right,rect.bottom,dcMem,0,0,SRCCOPY);
	DeleteObject(dcMem);
	DeleteObject(dcTemp);
	DeleteObject(hBitmapBack);
	DeleteObject(hBitmap);
	ReleaseDC(m_hWnd,hDc);
}

void cZaJinHua::DrawPoke(HDC dcMem)
{
	if (m_gameStatus != Status_Play && m_gameStatus != Status_Result)
	{
		//�������˿�
		return;
	}
	HBITMAP hBitmapPoke = (HBITMAP)LoadImageA(NULL,"images//cardskin//game_card.bmp",
		IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	HDC dcTemp = CreateCompatibleDC(NULL);
	SelectObject(dcTemp,hBitmapPoke);
	HBITMAP hBitmapPokeBack = (HBITMAP)LoadImageA(NULL,"images//cardskin//game_card_back.bmp",
		IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	HDC dcTempBack = CreateCompatibleDC(NULL);
	SelectObject(dcTempBack,hBitmapPokeBack);
	BITMAP bitInfo;
	GetObject(hBitmapPoke,sizeof(BITMAP),&bitInfo);
	int perWidth = bitInfo.bmWidth/13;
	int perHeight = bitInfo.bmHeight/4;
	for (int i = 0;i<Max_Player_Num;i++)
	{
		if (i == 0)
		{
			//�����Լ�����
			for (int j = 0;j<Per_Player_CardNum;j++)
			{
				//����ƵĻ�ɫ�ʹ�С,��ɫ��������һ�У���С��������һ��
				int cardColor = GetCardColor(m_playerInfo[i].iCard[j]);
				int cardNum = GetCardNum(m_playerInfo[i].iCard[j]);
				int xPos = (cardNum-1)*perWidth;
				int yPos = cardColor*perHeight;
				BitBlt(dcMem,g_ptCardPos[i].x+j*20,g_ptCardPos[i].y,
					perWidth,perHeight,dcTemp,xPos,yPos,SRCCOPY);
			}
		}
		else
		{
			//���Ƶ���
			//������
			for (int j = 0;j<Per_Player_CardNum;j++)
			{
				BitBlt(dcMem,g_ptCardPos[i].x+j*20,g_ptCardPos[i].y,
					perWidth,perHeight,dcTempBack,0,0,SRCCOPY);
			}
		}
	}
}

void cZaJinHua::FlushCard()
{
	srand(GetTickCount());
	m_gameStatus = Stauts_Send;
	for (int i = 0;i<Max_Card_Num-2;i++)
	{
		//������ķ�Χ��i+1  -   Max_Card_Num-1
		/*10-20
		20-10+1  11
		rand()%11  0-10  +10  ->10-20*/

		int randValue = rand();
		int index = randValue%(Max_Card_Num-1-(i+1)+1) + i+1;
		int temp = g_iCard[i];
		g_iCard[i] = g_iCard[index];
		g_iCard[index] = temp;
	}
}

void cZaJinHua::SendCard()
{
	for (int i = 0;i<Max_Player_Num;i++)
	{
		memcpy(m_playerInfo[i].iCard,g_iCard+i*Per_Player_CardNum,
			Per_Player_CardNum*sizeof(char));
	}
	m_gameStatus = Status_Play;
}

void cZaJinHua::PlayerAction()
{
	//ȷ��˭��˵��
	//���ֻ��һ�����ˣ����ʣ�µ��˾�Ӯ��
	if (GetLeavePlayerNum() == 1)
	{
		//ֻ��һ������ˣ���Ϸ����
		GameOver();

		return;
	}

	m_curActionPlayer = GetNextActionPlayer();
	if (m_curActionPlayer == 0)
	{
		//����Լ�
	}
	else
	{
		//���� ��
		//���һ��ʱ�䣬�����ʱ������ٶ�������Ϊ˼��ʱ�䣩
		int randValue = rand()%(Max_Think_Time-10)+2;
		AddTimer(Timer_RobotAction,randValue*1000);
	}
}

int cZaJinHua::GetNextActionPlayer()
{
	if (m_curActionPlayer == -1)
	{
		//�������һ�����˵��
		return rand()%Max_Player_Num;
	}
	else
	{
		int iNext = (m_curActionPlayer+1)%Max_Player_Num;
		while(iNext != m_curActionPlayer)
		{
			if (!m_playerInfo[iNext].bGiveUp)
			{
				return iNext;
			}
			iNext = (iNext+1)%Max_Player_Num;
		}
	}
	return -1;
}

void cZaJinHua::RobotAction()
{
	//AI
}

int cZaJinHua::GetLeavePlayerNum()
{
	int leaveNum = 0;
	for (int i =0 ;i<Max_Player_Num;i++)
	{
		if (!m_playerInfo[i].bGiveUp)
		{
			leaveNum++;
		}
	}
	return leaveNum;
}

int cZaJinHua::GetCardColor(char iCard)
{
	return iCard >> 4;
}

int cZaJinHua::GetCardNum(char iCard)
{
	return iCard & 0xf;
}