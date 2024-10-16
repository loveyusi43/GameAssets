#include "StdAfx.h"
#include "cJelly.h"
#include <windowsx.h>

#define JellyLen 50	//果冻的边长
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
	//初始化果冻信息
	for (int i = 0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			//如果是最后一行，就放要摧毁的目标
			if (i == Max_Row-1)
			{
				m_JellyArray[i][j] = JellyDes;
			}
			else
			{
				//随机一个果冻类型
				int randValue = rand()%JellyTypeNum;
				m_JellyArray[i][j] = (JellyType)randValue;
			}
		}
	}
}

//定时器回调函数
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
//绘制函数
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

	//画背景
	HDC dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmapBack = (HBITMAP)LoadImageA(NULL,"aaa.bmp",IMAGE_BITMAP,0,0,
		LR_LOADFROMFILE);
	SelectObject(dcTemp,hBitmapBack);

	BitBlt(memDc,0,0,rect.right-rect.left,rect.bottom-rect.top,dcTemp,0,0,SRCCOPY);

	DrawJelly(memDc);

	//从缓存DC绘制到设备DC
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
			//从开始位置一直位置到最后
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
					//这个正在被拖动
					continue;
				}
				TransparentBlt(memDc,j*JellyLen+MinDrawX,i*JellyLen+MinDrawY,
					JellyLen,JellyLen,dcTemp,jellyType*JellyLen,0,JellyLen,JellyLen,
					RGB(34,177,76));
			}
		}
	}
	//绘制拖动的这个果冻
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
	//左键按下
	//获得当前坐标
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	//判断鼠标位置在哪个果冻上（格子上）
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
	//左键弹起
	int xPos = GET_X_LPARAM(lParam);
	int yPos = GET_Y_LPARAM(lParam);
	//判断鼠标位置在哪个果冻上（格子上）
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
	//判断当前鼠标所在行，列是否在被拖动格子的旁边
	if (!IsNearDragJelly(row,rank))
	{
		m_bIsButtonDown = false;
		m_posCurDragJelly.x = -1;
		m_posCurDragJelly.y = -1;
		return;
	}
	//判断是否可以消除，如果不能，拖动失败，如果能，就拖动成功后再自动消除
	JellyType jellyInfo[Max_Row][Max_Rank];
	memcpy(jellyInfo,m_JellyArray,Max_Rank*Max_Row*sizeof(JellyType));
	//把两个格子对换
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
	//拖动成功，就换
	memcpy(m_JellyArray,jellyInfo,Max_Row*Max_Rank*sizeof(JellyType));
	//开始消除
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
	//判断鼠标位置在哪个果冻上（格子上）
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
	//判断当前鼠标所在行，列是否在被拖动格子的旁边
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
	//当是同一行
	if (m_posCurDragJelly.x == row && abs(m_posCurDragJelly.y-rank) == 1)
	{
		return true;
	}
	//当是同一列
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
			//检测某个果冻是否能消除
			//横向检测
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
					//可以消除
					return true;
				}

			}
			//横向检测
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
					//可以消除
					return true;
				}
			}
		}
	}
	return false;
}