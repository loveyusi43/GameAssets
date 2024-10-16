#include "StdAfx.h"
#include "cJelly.h"
#include <windowsx.h>

#define JellyLen 50	//�����ı߳�
#define MinDrawX 15
#define MinDrawY 140

#pragma comment(lib,"msimg32.lib")

cJelly::cJelly(HWND hWnd)
{
	m_hWnd = hWnd;
	AddTimer(Timer_Draw,50);
	m_bIsButtonDown = false;
	m_posCurDragJelly.x = -1;
	m_posCurDragJelly.y = -1;
	InitGame();
}


cJelly::~cJelly(void)
{
}

void cJelly::InitGame()
{
	srand(GetTickCount());
	//��ʼ��������Ϣ
	for (int i = 0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			//��������һ�У��ͷ�Ҫ�ݻٵ�Ŀ��
			if (i == Max_Row-1)
			{
				m_JellyArray[i][j] = JellyDes;
			}
			else
			{
				//���һ����������
				int randValue = rand()%JellyTypeNum;
				m_JellyArray[i][j] = (JellyType)randValue;
			}
		}
	}
}

//��ʱ���ص�����
int cJelly::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_Draw:
		Draw();
		break;
	}
	return 1;
}
//���ƺ���
void cJelly::Draw()
{
	HDC hDc = GetDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}
	HDC memDc = CreateCompatibleDC(hDc);
	RECT rect;
	GetClientRect(m_hWnd,&rect);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,rect.right-rect.left,
		rect.bottom-rect.top);
	SelectObject(memDc,hBitmap);

	//������
	HDC dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmapBack = (HBITMAP)LoadImageA(NULL,"aaa.bmp",IMAGE_BITMAP,0,0,
		LR_LOADFROMFILE);
	SelectObject(dcTemp,hBitmapBack);

	BitBlt(memDc,0,0,rect.right-rect.left,rect.bottom-rect.top,dcTemp,0,0,SRCCOPY);

	DrawJelly(memDc);

	//�ӻ���DC���Ƶ��豸DC
	BitBlt(hDc,0,0,rect.right-rect.left,rect.bottom-rect.top,memDc,0,0,SRCCOPY);

	DeleteObject(memDc);
	DeleteObject(hBitmap);
	DeleteObject(hBitmapBack);
	DeleteObject(dcTemp);
	ReleaseDC(m_hWnd,hDc);
}

void cJelly::DrawJelly(HDC memDc)
{
	HDC dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImageA(NULL,"candy.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	SelectObject(dcTemp,hBitmap);

	for (int i = 0;i<Max_Row;i++)
	{
		for (int j =0;j<Max_Rank;j++)
		{
			//�ӿ�ʼλ��һֱλ�õ����
			JellyType jellyType = m_JellyArray[i][j];
			if (jellyType == JellyDes)
			{
				TransparentBlt(memDc,j*JellyLen+MinDrawX,i*JellyLen+MinDrawY,
					JellyLen,JellyLen,dcTemp,200,100,JellyLen,JellyLen,
					RGB(34,177,76));
			}
			else
			{
				if (m_posCurDragJelly.x == i &&
					m_posCurDragJelly.y == j)
				{
					//������ڱ��϶�
					continue;
				}
				TransparentBlt(memDc,j*JellyLen+MinDrawX,i*JellyLen+MinDrawY,
					JellyLen,JellyLen,dcTemp,jellyType*JellyLen,0,JellyLen,JellyLen,
					RGB(34,177,76));
			}
		}
	}
	//�����϶����������
	if (m_posCurDragJelly.x != -1)
	{
		int jellyType = m_JellyArray[m_posCurDragJelly.x][m_posCurDragJelly.y];
		tagPOINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(m_hWnd,&ptCursor);

		int xPos = ptCursor.x-JellyLen/2;
		int yPos = ptCursor.y-JellyLen/2;
		TransparentBlt(memDc,xPos,yPos,JellyLen,JellyLen,dcTemp,jellyType*JellyLen,
			0,JellyLen,JellyLen,RGB(34,177,76));
	}
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
}

void cJelly::OnLButtonDown(LPARAM lParam)
{
	if (m_bIsButtonDown)
	{
		return;
	}
	//�������
	//��õ�ǰ����
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	//�ж����λ�����ĸ������ϣ������ϣ�
	if (xPos < MinDrawX || yPos < MinDrawY)
	{
		return;
	}
	int rank = (xPos-MinDrawX)/JellyLen;
	int row = (yPos-MinDrawY)/JellyLen;
	if (rank >= Max_Rank || row >= Max_Row)
	{
		return;
	}
	if (m_JellyArray[row][rank] == JellyDes)
	{
		return;
	}
	m_bIsButtonDown = true;
	m_posCurDragJelly.x = row;
	m_posCurDragJelly.y = rank;
}

void cJelly::OnLButtonUp(LPARAM lParam)
{
	if (!m_bIsButtonDown)
	{
		return;
	}
	//�������
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	//�ж����λ�����ĸ������ϣ������ϣ�
	if (xPos < MinDrawX || yPos < MinDrawY)
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	int rank = (xPos-MinDrawX)/JellyLen;
	int row = (yPos-MinDrawY)/JellyLen;
	if (rank >= Max_Rank || row >= Max_Row)
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	if (m_JellyArray[row][rank] == JellyDes)
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	//�жϵ�ǰ��������У����Ƿ��ڱ��϶����ӵ��Ա�
	if (!IsNearDragJelly(row,rank))
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	//�ж��Ƿ����������������ܣ��϶�ʧ�ܣ�����ܣ����϶��ɹ������Զ�����
	JellyType jellyInfo[Max_Row][Max_Rank];
	memcpy(jellyInfo,m_JellyArray,Max_Rank*Max_Row*sizeof(JellyType));
	//���������ӶԻ�
	JellyType temp = jellyInfo[m_posCurDragJelly.x][m_posCurDragJelly.y];
	jellyInfo[m_posCurDragJelly.x][m_posCurDragJelly.y] = jellyInfo[row][rank];
	jellyInfo[row][rank] = temp;
	if (!CheckClear(jellyInfo))
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	//�϶��ɹ����ͻ�
	memcpy(m_JellyArray,jellyInfo,Max_Row*Max_Rank*sizeof(JellyType));
	//��ʼ����
	//Clear();

	m_posCurDragJelly.x = -1;
	m_posCurDragJelly.y = -1;
	m_bIsButtonDown = false;
}

void cJelly::OnMouseMove(LPARAM lParam)
{
	if (!m_bIsButtonDown)
	{
		return;
	}
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	//�ж����λ�����ĸ������ϣ������ϣ�
	if (xPos < MinDrawX || yPos < MinDrawY)
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	int rank = (xPos-MinDrawX)/JellyLen;
	int row = (yPos-MinDrawY)/JellyLen;
	if (rank >= Max_Rank || row >= Max_Row)
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	if (m_JellyArray[row][rank] == JellyDes)
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	//�жϵ�ǰ��������У����Ƿ��ڱ��϶����ӵ��Ա�
	if (!IsNearDragJelly(row,rank)
		&& !(m_posCurDragJelly.x == row && m_posCurDragJelly.y == rank))
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
}

bool cJelly::IsNearDragJelly(int row,int rank)
{
	//����ͬһ��
	if (m_posCurDragJelly.x == row && abs(m_posCurDragJelly.y-rank) == 1)
	{
		return true;
	}
	//����ͬһ��
	if (m_posCurDragJelly.y == rank && abs(m_posCurDragJelly.x-row) == 1)
	{
		return true;
	}
	return false;
}

bool cJelly::CheckClear(JellyType jellyInfo[][Max_Rank])
{
	for (int i =0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			//���ĳ�������Ƿ�������
			//������
			for (int k = j-2;k<=j;k++)
			{
				if (k<0 || k+1>=Max_Rank || k+2>=Max_Rank)
				{
					continue;
				}
				if (m_JellyArray[i][k] == JellyDes)
				{
					continue;
				}
				if (jellyInfo[i][k] == jellyInfo[i][k+1] &&
					jellyInfo[i][k] == jellyInfo[i][k+2])
				{
					//��������
					return true;
				}

			}
			//������
			for (int k = i-2;k<=i;k++)
			{
				if(k<0 || k+1>=Max_Row || k+2>= Max_Row)
				{
					continue;
				}
				if (jellyInfo[k][j] == JellyDes)
				{
					continue;
				}
				if (jellyInfo[k][j] == jellyInfo[k+1][j] &&
					jellyInfo[k][j] == jellyInfo[k+2][j])
				{
					//��������
					return true;
				}
			}
		}
	}
	return false;
}