#include "stdafx.h"
#include "cRunningMan.h"

#pragma comment(lib,"Msimg32.lib")

//921700006  VIPQQ，咨询
//431493983		//拿视频资料 学习交流群

cRunning::cRunning()
{
	m_curMapStep = 0;
	m_curPlayerStatus = Status_Run;
	m_curActionIndex = 0;
	m_curPlayerPos.x = 150;
	m_curPlayerPos.y = 265;
	m_bIsKeyDown = false;
	m_usJumpTime= 0;
	m_iMoney = 0;
}

cRunning::~cRunning()
{

}

void cRunning::BeginGame(HWND hWnd)
{
	m_hWnd = hWnd;
	m_curMapStep = 0;
	m_curPlayerStatus = Status_Run;
	m_curActionIndex = 0;
	m_curPlayerPos.x = 150;
	m_curPlayerPos.y = 264;
	m_bIsKeyDown = false;
	m_usJumpTime= 0;
	m_iMoney = 0;

	for (int i =0;i<100;i++)
	{
		tagPOINT pt;
		pt.x = 960+i*800;
		if (i%2)
		{
			pt.y = 40;
			m_listPost.push_back(pt);
		}
		else
		{
			pt.y = 365;
			m_listBase.push_back(pt);
		}
	}

	for (int i = 0;i<100;i++)
	{
		tagPOINT pt;
		pt.x = 960 + i*150;
		pt.y = 160;
		m_listGold.push_back(pt);
	}

	AddTimer(Timer_Draw,50);
}

void cRunning::GameOver()
{
	DeleteTimer(Timer_Draw);
	DeleteTimer(Timer_ControlJump);
	Sleep(2000);
	m_listBase.clear();
	m_listGold.clear();
	m_listPost.clear();
	BeginGame(m_hWnd);
}

void cRunning::KeyDown(WPARAM wParam)
{
	if (m_bIsKeyDown)
	{
		return;
	}
	if (m_curPlayerStatus == Status_Jump ||
		m_curPlayerStatus == Status_Roll ||
		m_curPlayerStatus == Status_Fall)
	{
		return;
	}
	if (wParam == VK_UP)
	{
		m_curPlayerStatus = Status_Jump;
		m_usJumpTime = GetTickCount();
		AddTimer(Timer_ControlJump,400);
	}
	else if (wParam == VK_DOWN)
	{
		m_curPlayerStatus = Status_Slip;
	}
}

void cRunning::KeyUp(WPARAM wParam)
{
	if (wParam == VK_UP || wParam == VK_DOWN)
	{
		if (m_curPlayerStatus == Status_Jump ||
			m_curPlayerStatus == Status_Roll ||
			m_curPlayerStatus == Status_Fall)
		{
			return;
		}
		m_bIsKeyDown = false;
		m_curPlayerStatus = Status_Run;
	}
}

int cRunning::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_Draw:
		DrawAll();
		CheckCollide();
		break;
	case Timer_ControlJump:
		ControlJump();
		break;
	}
	return 1;
}

void cRunning::DrawAll()
{
	HDC hDc = GetWindowDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}

	HDC dcMem = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,960,640);
	SelectObject(dcMem,hBitmap);
	//把所有的内容绘制到缓存DC上 dcMem
	DrawMap(dcMem);
	DrawPlayer(dcMem);
	DrawObjects(dcMem);

	SetBkMode(dcMem,0);
	SetTextColor(dcMem,RGB(255,204,14));
	wchar_t strText[100] ={0};
	wsprintf(strText,_T("金币:%d"),m_iMoney);
	TextOut(dcMem,480,30,strText,lstrlen(strText));

	BitBlt(hDc,3,25,960,640,dcMem,0,0,SRCCOPY);
	ReleaseDC(m_hWnd,hDc);
}

void cRunning::DrawMap(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL,_T("images\\bg.bmp"),
		IMAGE_BITMAP,960,640,LR_LOADFROMFILE);
	SelectObject(dcTemp,bitmap);
	BitBlt(hDc,0,0,960-m_curMapStep,640,dcTemp,m_curMapStep,0,SRCCOPY);

	BitBlt(hDc,960-m_curMapStep,0,m_curMapStep,640,dcTemp,
		0,0,SRCCOPY);
	m_curMapStep = (m_curMapStep+30)%960;
	DeleteObject(dcTemp);
	DeleteObject(bitmap);
}

void cRunning::DrawPlayer(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	wchar_t szFileName[100];
	if (m_curPlayerStatus == Status_Run)
	{
		m_curActionIndex = (m_curActionIndex+1)%6;
		wsprintf(szFileName,_T("images\\playerRun%d.bmp"),
			m_curActionIndex);
	}
	else if (m_curPlayerStatus == Status_Jump)
	{
		wsprintf(szFileName,_T("images\\playerJump.bmp"));
		unsigned timeNow = GetTickCount();
		unsigned timePass = timeNow-m_usJumpTime;
		int h = 600*(timePass/1000.0)-1500*(timePass/1000.0)*
			(timePass/1000.0)/2.0;
		m_curPlayerPos.y = 265-h;
	}
	else if (m_curPlayerStatus == Status_Roll)
	{
		m_curActionIndex = (m_curActionIndex+1)%3;
		wsprintf(szFileName,_T("images\\playerRoll%d.bmp"),
			m_curActionIndex);
	}
	else if (m_curPlayerStatus == Status_Fall)
	{
		wsprintf(szFileName,_T("images\\playerFall.bmp"));
		unsigned timeNow = GetTickCount();
		unsigned timePass = timeNow-(m_usJumpTime+800);
		int h = 1500*(timePass/1000.0)*(timePass/1000.0)/2.0;
		m_curPlayerPos.y = 265-120+h;
	}
	else if (m_curPlayerStatus == Status_Slip)
	{
		wsprintf(szFileName,_T("images\\playerSlip.bmp"));
	}

	HBITMAP bitmap = (HBITMAP)LoadImage(NULL,szFileName,IMAGE_BITMAP,
		138,138,LR_LOADFROMFILE);
	SelectObject(dcTemp,bitmap);
	TransparentBlt(hDc,m_curPlayerPos.x,m_curPlayerPos.y,138,138,
		dcTemp,0,0,138,138,RGB(34,177,76));

	DeleteObject(dcTemp);
	DeleteObject(bitmap);
}

void cRunning::DrawObjects(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmap1 = (HBITMAP)LoadImage(NULL,_T("images\\post.bmp"),
		IMAGE_BITMAP,82,310,LR_LOADFROMFILE);
	HBITMAP bitmap2 = (HBITMAP)LoadImage(NULL,_T("images\\baseplate.bmp"),
		IMAGE_BITMAP,120,38,LR_LOADFROMFILE);
	HBITMAP bitmap3 = (HBITMAP)LoadImage(NULL,_T("images\\gold.bmp"),
		IMAGE_BITMAP,69,64,LR_LOADFROMFILE);
	//画柱子
	SelectObject(dcTemp,bitmap1);
	for (list<tagPOINT>::iterator it = m_listPost.begin();
		it != m_listPost.end();)
	{
		it->x -= 30;
		if (it->x > 2000)
		{
			it++;
			continue;
		}
		TransparentBlt(hDc,it->x,it->y,82,310,dcTemp,0,0,82,
			310,RGB(34,177,76));
		if (it->x <= -82)
		{
			it = m_listPost.erase(it);
			continue;
		}
		it++;
	}

	//画盘子
	SelectObject(dcTemp,bitmap2);
	for (list<tagPOINT>::iterator it = m_listBase.begin();
		it != m_listBase.end();)
	{
		it->x -= 30;
		if (it->x > 2000)
		{
			it++;
			continue;
		}
		TransparentBlt(hDc,it->x,it->y,120,38,dcTemp,0,0,120,
			38,RGB(34,177,76));
		if (it->x <= -120)
		{
			it = m_listBase.erase(it);
			continue;
		}
		it++;
	}

	//画金币
	SelectObject(dcTemp,bitmap3);
	for (list<tagPOINT>::iterator it = m_listGold.begin();
		it != m_listGold.end();)
	{
		it->x -= 30;
		if (it->x > 2000)
		{
			it++;
			continue;
		}
		TransparentBlt(hDc,it->x,it->y,69,64,dcTemp,0,0,69,
			64,RGB(34,177,76));
		if (it->x <= -69)
		{
			it = m_listGold.erase(it);
			continue;
		}
		it++;
	}
}

void cRunning::ControlJump()
{
	if (m_curPlayerStatus == Status_Jump)
	{
		m_curPlayerStatus = Status_Roll;
	}
	else if (m_curPlayerStatus == Status_Roll)
	{
		m_curPlayerStatus = Status_Fall;
	}
	else if (m_curPlayerStatus == Status_Fall)
	{
		m_curPlayerStatus = Status_Run;
		m_bIsKeyDown = false;
		m_curPlayerPos.y = 265;
		DeleteTimer(Timer_ControlJump);
	}
}

void cRunning::CheckCollide()
{
	//有几种碰撞
	//人和金币，人和柱子，人和盘子
	for (list<tagPOINT>::iterator it = m_listGold.begin();
		it != m_listGold.end();)
	{
		if (m_curPlayerStatus == Status_Jump ||
			m_curPlayerStatus == Status_Fall ||
			m_curPlayerStatus == Status_Run)
		{
			tagPOINT centerPt1 = *it;
			tagPOINT centerPt2 = m_curPlayerPos;
			centerPt1.x += 35;
			centerPt1.y += 32;
			centerPt2.x += 69;
			centerPt2.y += 69;
			if (abs(centerPt1.x-centerPt2.x) <= 35+69-10
				&& abs(centerPt1.y - centerPt2.y) <= 32+69-20)
			{
				//金币被吃
				it = m_listGold.erase(it);
				m_iMoney += 100;
				continue;
			}
			it++;
		}
		else if (m_curPlayerStatus == Status_Roll)
		{
			tagPOINT centerPt1 = *it;
			tagPOINT centerPt2 = m_curPlayerPos;
			centerPt1.x += 35;
			centerPt1.y += 32;
			centerPt2.x += 69;
			centerPt2.y += 69;
			if (abs(centerPt1.x-centerPt2.x) <= 35+54-5
				&& abs(centerPt1.y - centerPt2.y) <= 32+54-5)
			{
				//金币被吃
				it = m_listGold.erase(it);
				m_iMoney += 100;
				continue;
			}
			it++;
		}
		else if (m_curPlayerStatus == Status_Slip)
		{
			tagPOINT centerPt1 = *it;
			tagPOINT centerPt2 = m_curPlayerPos;
			centerPt1.x += 35;
			centerPt1.y += 32;
			centerPt2.x += 69;
			centerPt2.y += 138-25;
			if (abs(centerPt1.x-centerPt2.x) <= 35+45-5
				&& abs(centerPt1.y - centerPt2.y) <= 32+25-5)
			{
				//金币被吃
				it = m_listGold.erase(it);
				m_iMoney += 100;
				continue;
			}
			it++;
		}
	}
	//人和柱子
	for (list<tagPOINT>::iterator it = m_listPost.begin();
		it != m_listPost.end();it++)
	{
		if (m_curPlayerStatus == Status_Jump ||
			m_curPlayerStatus == Status_Fall ||
			m_curPlayerStatus == Status_Run)
		{
			tagPOINT centerPt1 = *it;
			tagPOINT centerPt2 = m_curPlayerPos;
			centerPt1.x += 40-5;
			centerPt2.x += 69;
			centerPt2.y += 69;
			if (abs(centerPt1.x-centerPt2.x) <= 35+69-10)
			{
				GameOver();
				return;
			}
		}
		else if (m_curPlayerStatus == Status_Roll)
		{
			tagPOINT centerPt1 = *it;
			tagPOINT centerPt2 = m_curPlayerPos;
			centerPt1.x += 35;
			centerPt2.x += 69;
			centerPt2.y += 69;
			if (abs(centerPt1.x-centerPt2.x) <= 35+54-5)
			{
				GameOver();
				return;
			}
		}
	}
}