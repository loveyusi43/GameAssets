#include "StdAfx.h"
#include "cJelly.h"
#include <windowsx.h>

#define JellyLen 50	//�����ı߳�
#define MinDrawX 15
#define MinDrawY 140
#define MAX_LEVEL_VALUE	5 //���ؿ���

#pragma comment(lib,"msimg32.lib")

const int g_levelStepInfo[MAX_LEVEL_VALUE] = {5,10,15,20,25};//������Ϣ
const int g_levelDesInfo[MAX_LEVEL_VALUE] = {1,2,3,4,5};	//Ŀ�������Ϣ

cJelly::cJelly(HWND hWnd)
{
	m_hWnd = hWnd;
	m_curLevel = 1;	//��ǰ��

	AddTimer(Timer_Draw,50);
	m_bIsButtonDown = false;
	m_posCurDragJelly.x = -1;
	m_posCurDragJelly.y = -1;

	m_firstWillClearJelly.x = 0;	//��һ��Ҫ�����Ĺ�����λ��x- ��  y-��
	m_firstWillClearJelly.y = 0;
	m_bRow = false;	//�Ƿ��Ǻ�������
	m_iClearNum = 0;	//һ��Ҫ�������ٸ�����
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
	//����Ŀ��ݻ���
	//max_row*max_rank-1
	int createNum= 0;
	while(createNum < g_levelDesInfo[m_curLevel-1])
	{
		int randValue = rand()%(Max_Rank*Max_Row);
		//ͨ�����ֵ�������
		int row = randValue/Max_Rank;	//��
		int rank = randValue%Max_Rank;	//��
		if (m_JellyArray[row][rank] == JellyDes)
		{
			continue;
		}
		m_JellyArray[row][rank] = JellyDes;
		createNum++;
	}

	//��ʼ��������Ϣ
	for (int i = 0;i<Max_Row;i++)
	{
		for (int j = 0;j<Max_Rank;j++)
		{
			//��������һ�У��ͷ�Ҫ�ݻٵ�Ŀ��
			/*if (i == Max_Row-1)
			{
			m_JellyArray[i][j] = JellyDes;
			}*/
			if(m_JellyArray[i][j] != JellyDes)
			{
				//���һ����������
				int randValue = rand()%JellyTypeNum;
				m_JellyArray[i][j] = (JellyType)randValue;
				while(CheckClear(m_JellyArray))
				{
					//����Ƿ��п���������
					randValue = rand()%JellyTypeNum;
					m_JellyArray[i][j] = (JellyType)randValue;
				}
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
	case Timer_ClearJellyStep1:
		//��ʼը
		ClearJellyProc();	//��������������Ϊը��
		AddTimer(Timer_ClearJellyStep2,1000);
		return 0;
	case Timer_ClearJellyStep2:
		//���¹���
		UpdateJelly();
		int curLeaveDes = GetDesNum();	//ʣ����ٸ���δը
		if (curLeaveDes == 0)
		{
			//ը�꣬������һ��
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
			//�жϲ����Ƿ����꣬������꣬����ʧ�ܣ�����
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

	//���ƹ���
	DrawJelly(memDc);
	//���ƹؿ���Ϣ
	SetBkMode(memDc,0);
	SetTextColor(memDc,RGB(0,0,255));//����������ɫ
	char lpText[100] = {0};
	sprintf(lpText,"��%d��",m_curLevel);
	TextOutA(memDc,120,90,lpText,strlen(lpText));
	sprintf(lpText,"ʣ�ಽ����%d",m_curLeaveStep);
	TextOutA(memDc,200,90,lpText,strlen(lpText));

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
			else if(jellyType == ClearPicRow)
			{
				//����ը
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
				//����ը
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
	m_curLeaveStep--;//ʣ�ಽ��-1
	//��ʼ����
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
				if (jellyInfo[i][k] == JellyDes ||
					jellyInfo[i][k] == JellyNULL)
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
				if (jellyInfo[k][j] == JellyDes ||
					jellyInfo[k][j] == JellyNULL)
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

void cJelly::Clear()
{
	//��¼��Ϣ��Ҫ������������Ļ�����ߵ��Ǹ�������λ�ã��Ǻ���������
	//��������
	bool bFind = false;
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
				if (m_JellyArray[i][k] == JellyDes ||
					m_JellyArray[i][k] == JellyNULL)
				{
					continue;
				}
				if (m_JellyArray[i][k] == m_JellyArray[i][k+1] &&
					m_JellyArray[i][k] == m_JellyArray[i][k+2])
				{
					//��������
					m_firstWillClearJelly.x = i;
					m_firstWillClearJelly.y = k;
					m_bRow = true;
					m_iClearNum = 3;
					if (k+3 < Max_Rank && 
						(m_JellyArray[i][k+3] == m_JellyArray[i][k] ||
						m_JellyArray[i][k+3] == JellyDes))
					{
						//��������
						m_iClearNum++;
					}
					if (m_iClearNum == 4 && k+4<Max_Rank &&
						m_JellyArray[i][k+3] != JellyDes)
					{
						//�������������
						if (m_JellyArray[i][k+4] == m_JellyArray[i][k] ||
							m_JellyArray[i][k+4] == JellyDes)
						{
							m_iClearNum++;
						}
					}
					//����ǲ���Ŀ�꣬����ǣ�ҲҪը��
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
			//������
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
					//��������
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
						//�������������
						if (m_JellyArray[k+4][j] == m_JellyArray[k][j]
						|| m_JellyArray[k+4][j] == JellyDes)
						{
							m_iClearNum++;
						}
					}

					//���������Ŀ�꣬Ҫը��
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
		//��ʼ����
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
	//������������ơ��������������µĹ���
	if (m_bRow)
	{
		//����Ǻ���
		//ÿ��Ҫ�ƶ��ٸ�
		for (int i = m_firstWillClearJelly.x-1;i>= 0;i--)
		{
			//Ҫ�ƶ�����
			for (int j = m_firstWillClearJelly.y;
				j<m_firstWillClearJelly.y+m_iClearNum;
				j++)
			{
				m_JellyArray[i+1][j] = m_JellyArray[i][j];
			}
		}
		//����һ�������µĹ���
		for (int j = m_firstWillClearJelly.y;j<m_firstWillClearJelly.y+m_iClearNum;
			j++)
		{
			int randValue = rand()%JellyTypeNum;
			m_JellyArray[0][j] = (JellyType)randValue;
		}
	}
	else
	{
		//�������
		for (int i = m_firstWillClearJelly.x-1;i>=0;i--)
		{
			m_JellyArray[i+m_iClearNum][m_firstWillClearJelly.y]=
				m_JellyArray[i][m_firstWillClearJelly.y];
		}
		//��һ���������µ�λ�ô�Ҫ�����µĹ���
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