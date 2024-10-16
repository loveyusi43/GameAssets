#include "StdAfx.h"
#include "cJelly.h"
#include <windowsx.h>

#define JellyLen 50	//果冻的边长
#define MinDrawX 15
#define MinDrawY 140
#define MAX_LEVEL_VALUE	5 //最大关卡数

#pragma comment(lib,"msimg32.lib")

const int g_levelStepInfo[MAX_LEVEL_VALUE] = {5,10,15,20,25};//步数信息
const int g_levelDesInfo[MAX_LEVEL_VALUE] = {1,2,3,4,5};	//目标个数信息

cJelly::cJelly(HWND hWnd)
{
	m_hWnd = hWnd;
	m_curLevel = 1;	//当前关

	AddTimer(Timer_Draw,50);
	m_bIsButtonDown = false;
	m_posCurDragJelly.x = -1;
	m_posCurDragJelly.y = -1;

	m_firstWillClearJelly.x = 0;	//第一个要消除的果冻的位置x- 行  y-列
	m_firstWillClearJelly.y = 0;
	m_bRow = false;	//是否是横向消除
	m_iClearNum = 0;	//一共要消除多少个格子
	InitGame();
}


cJelly::~cJelly(void)
{
}

void cJelly::InitGame()
{
	m_curLeaveStep = g_levelStepInfo[m_curLevel-1];
	for (int i = 0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			m_JellyArray[i][j] = JellyNULL;
		}
	}
	srand(GetTickCount());
	//生成目标摧毁物
	//max_row*max_rank-1
	int createNum= 0;
	while(createNum < g_levelDesInfo[m_curLevel-1])
	{
		int randValue = rand()%(Max_Rank*Max_Row);
		//通过这个值算出行列
		int row = randValue/Max_Rank;	//行
		int rank = randValue%Max_Rank;	//列
		if (m_JellyArray[row][rank] == JellyDes)
		{
			continue;
		}
		m_JellyArray[row][rank] = JellyDes;
		createNum++;
	}

	//初始化果冻信息
	for (int i = 0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			//如果是最后一行，就放要摧毁的目标
			/*if (i == Max_Row-1)
			{
			m_JellyArray[i][j] = JellyDes;
			}*/
			if(m_JellyArray[i][j] != JellyDes)
			{
				//随机一个果冻类型
				int randValue = rand()%JellyTypeNum;
				m_JellyArray[i][j] = (JellyType)randValue;
				while(CheckClear(m_JellyArray))
				{
					//检测是否有可以消除的
					randValue = rand()%JellyTypeNum;
					m_JellyArray[i][j] = (JellyType)randValue;
				}
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
	case Timer_ClearJellyStep1:
		//开始炸
		ClearJellyProc();	//消除果冻，并设为炸弹
		AddTimer(Timer_ClearJellyStep2,1000);
		return 0;
	case Timer_ClearJellyStep2:
		//更新果冻
		UpdateJelly();
		int curLeaveDes = GetDesNum();	//剩余多少个点未炸
		if (curLeaveDes == 0)
		{
			//炸完，进入下一关
			m_curLevel++;
			if (m_curLevel > MAX_LEVEL_VALUE)
			{
				m_curLevel=1;
			}
			InitGame();
			return 0;
		}
		if (CheckClear(m_JellyArray))
		{
			Clear();
		}
		else
		{
			//判断步数是否用完，如果用完，过关失败，重来
			//m_curLevel = 1;
			if (m_curLeaveStep == 0)
			{
				InitGame();
			}
		}

		return 0;
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

	//绘制果冻
	DrawJelly(memDc);
	//绘制关卡信息
	SetBkMode(memDc,0);
	SetTextColor(memDc,RGB(0,0,255));//设置文字颜色
	char lpText[100] = {0};
	sprintf(lpText,"第%d关",m_curLevel);
	TextOutA(memDc,120,90,lpText,strlen(lpText));
	sprintf(lpText,"剩余步数：%d",m_curLeaveStep);
	TextOutA(memDc,200,90,lpText,strlen(lpText));

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
			else if(jellyType == ClearPicRow)
			{
				//横向炸
				HBITMAP hBitmapRow = (HBITMAP)LoadImageA(NULL,"picRow.bmp",
					IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
				HDC dcBomb = CreateCompatibleDC(NULL);
				SelectObject(dcBomb,hBitmapRow);
				TransparentBlt(memDc,j*JellyLen+MinDrawX,i*JellyLen+MinDrawY,
					JellyLen,JellyLen,dcBomb,0,0,JellyLen,JellyLen,
					RGB(34,177,76));
				DeleteObject(hBitmapRow);
				DeleteObject(dcBomb);
			}
			else if (jellyType == ClearPicRank)
			{
				//纵向炸
				HBITMAP hBitmapRank = (HBITMAP)LoadImageA(NULL,"picRank.bmp",
					IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
				HDC dcBomb = CreateCompatibleDC(NULL);
				SelectObject(dcBomb,hBitmapRank);
				TransparentBlt(memDc,j*JellyLen+MinDrawX,i*JellyLen+MinDrawY,
					JellyLen,JellyLen,dcBomb,0,0,JellyLen,JellyLen,
					RGB(34,177,76));
				DeleteObject(hBitmapRank);
				DeleteObject(dcBomb);
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
	m_curLeaveStep--;//剩余步数-1
	//开始消除
	Clear();

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
				if (jellyInfo[i][k] == JellyDes ||
					jellyInfo[i][k] == JellyNULL)
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
				if (jellyInfo[k][j] == JellyDes ||
					jellyInfo[k][j] == JellyNULL)
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

void cJelly::Clear()
{
	//记录信息：要消除的最上面的或最左边的那个果冻的位置，是横向还是纵向
	//消除几个
	bool bFind = false;
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
				if (m_JellyArray[i][k] == JellyDes ||
					m_JellyArray[i][k] == JellyNULL)
				{
					continue;
				}
				if (m_JellyArray[i][k] == m_JellyArray[i][k+1] &&
					m_JellyArray[i][k] == m_JellyArray[i][k+2])
				{
					//可以消除
					m_firstWillClearJelly.x = i;
					m_firstWillClearJelly.y = k;
					m_bRow = true;
					m_iClearNum = 3;
					if (k+3 < Max_Rank && 
						(m_JellyArray[i][k+3] == m_JellyArray[i][k] ||
						m_JellyArray[i][k+3] == JellyDes))
					{
						//可以消除
						m_iClearNum++;
					}
					if (m_iClearNum == 4 && k+4<Max_Rank &&
						m_JellyArray[i][k+3] != JellyDes)
					{
						//第五个可以消除
						if (m_JellyArray[i][k+4] == m_JellyArray[i][k] ||
							m_JellyArray[i][k+4] == JellyDes)
						{
							m_iClearNum++;
						}
					}
					//左边是不是目标，如果是，也要炸掉
					if (k-1>= 0 && m_JellyArray[i][k-1] == JellyDes)
					{
						m_iClearNum++;
						m_firstWillClearJelly.y = k-1;
					}
					bFind = true;
					break;
				}


			}
			if (bFind)
			{
				break;
			}
			//纵向检测
			for (int k = i-2;k<=i;k++)
			{
				if(k<0 || k+1>=Max_Row || k+2>= Max_Row)
				{
					continue;
				}
				if (m_JellyArray[k][j] == JellyDes ||
					m_JellyArray[k][j] == JellyNULL)
				{
					continue;
				}
				if (m_JellyArray[k][j] == m_JellyArray[k+1][j] &&
					m_JellyArray[k][j] == m_JellyArray[k+2][j])
				{
					//可以消除
					m_firstWillClearJelly.x = k;
					m_firstWillClearJelly.y = j;
					m_bRow = false;
					m_iClearNum = 3;

					if (k+3<Max_Row && (m_JellyArray[k+3][j] ==
						m_JellyArray[k][j] || m_JellyArray[k+3][j] == JellyDes))
					{
						m_iClearNum++;
					}
					if (m_iClearNum == 4 && k+4<Max_Row &&
						m_JellyArray[k+3][j] != JellyDes)
					{
						//第五个可以消除
						if (m_JellyArray[k+4][j] == m_JellyArray[k][j]
						|| m_JellyArray[k+4][j] == JellyDes)
						{
							m_iClearNum++;
						}
					}

					//上面可能是目标，要炸弹
					if (k-1 >= 0 && m_JellyArray[k-1][j] == JellyDes)
					{
						m_iClearNum++;
						m_firstWillClearJelly.x = k-1;
					}
					bFind = true;
					break;
				}
				
			}
			if (bFind)
			{
				break;
			}
		}
		if (bFind)
		{
			break;
		}
	}

	if (bFind)
	{
		//开始消除
		AddTimer(Timer_ClearJellyStep1,1000);
	}
}

void cJelly::ClearJellyProc()
{
	if (m_bRow)
	{
		for (int i = 0;i<m_iClearNum;i++)
		{
			m_JellyArray[m_firstWillClearJelly.x]
			[m_firstWillClearJelly.y+i] = ClearPicRow;
		}
	}
	else
	{
		for (int i = 0;i<m_iClearNum;i++)
		{
			m_JellyArray[m_firstWillClearJelly.x+i]
			[m_firstWillClearJelly.y] = ClearPicRank;
		}
	}
}

void cJelly::UpdateJelly()
{
	//把上面的往下移。再在上面生成新的果冻
	if (m_bRow)
	{
		//如果是横向
		//每列要移多少个
		for (int i = m_firstWillClearJelly.x-1;i>= 0;i--)
		{
			//要移多少列
			for (int j = m_firstWillClearJelly.y;
				j<m_firstWillClearJelly.y+m_iClearNum;
				j++)
			{
				m_JellyArray[i+1][j] = m_JellyArray[i][j];
			}
		}
		//最上一行生成新的果冻
		for (int j = m_firstWillClearJelly.y;j<m_firstWillClearJelly.y+m_iClearNum;
			j++)
		{
			int randValue = rand()%JellyTypeNum;
			m_JellyArray[0][j] = (JellyType)randValue;
		}
	}
	else
	{
		//如果纵向
		for (int i = m_firstWillClearJelly.x-1;i>=0;i--)
		{
			m_JellyArray[i+m_iClearNum][m_firstWillClearJelly.y]=
				m_JellyArray[i][m_firstWillClearJelly.y];
		}
		//这一列上面移下的位置处要生成新的果冻
		for (int i = 0;i<m_iClearNum;i++)
		{
			int randValue = rand()%JellyTypeNum;
			m_JellyArray[i][m_firstWillClearJelly.y] = (JellyType)randValue;
		}
	}
}

int cJelly::GetDesNum()
{
	int num = 0;
	for (int i = 0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			if (m_JellyArray[i][j] == JellyDes)
			{ 
				num++;
			}
		}
	}
	return num;
}