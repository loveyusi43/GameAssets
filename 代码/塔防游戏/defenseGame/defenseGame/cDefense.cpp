#include "stdafx.h"
#include "cDefense.h"
#include "windowsx.h"

#pragma comment(lib,"msimg32.lib")

//921700006
cDefense::cDefense()
{
	m_rectBuilding[0].left = 20;
	m_rectBuilding[0].top = 15;
	m_rectBuilding[0].right = m_rectBuilding[0].left+40;
	m_rectBuilding[0].bottom = m_rectBuilding[0].top+37;

	m_rectBuilding[1].left = 70;
	m_rectBuilding[1].top = 10;
	m_rectBuilding[1].right = m_rectBuilding[1].left+40;
	m_rectBuilding[1].bottom = m_rectBuilding[1].top+45;
	m_mouseItemIndex = -1;
	m_iMoney = 100;
}
cDefense::~cDefense()
{

}

int cDefense::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_Draw:
		DrawAll();
		break;
	case Timer_AddMonster:
		AddMonster();
		break;
	}
	return 1;
}

void cDefense::BeginGame(HWND hWnd)
{
	m_hWnd = hWnd;
	m_mouseItemIndex = -1;
	m_iMoney = 1000;
	InitBase();
	AddTimer(Timer_Draw,50);
	AddTimer(Timer_AddMonster,2000);	//创建怪
}

void cDefense::DrawAll()
{
	HDC hDc = GetDC(m_hWnd);
	HDC dcMem = CreateCompatibleDC(hDc);
	HBITMAP hBitmep = CreateCompatibleBitmap(hDc,700,600);
	SelectObject(dcMem,hBitmep);
	//要把所有的图形绘制到dcMem上

	DrawMap(dcMem);
	DrawItem(dcMem);//建筑物的图标，建筑物全放这个函数里绘制


	wchar_t szText[100] = {0};
	wsprintf(szText,L"金币：%d",m_iMoney);
	SetBkMode(dcMem,0);
	SetTextColor(dcMem,RGB(255,255,50));
	TextOut(dcMem,120,20,szText,lstrlen(szText));
	//把dcMem绘制到hDc上
	BitBlt(hDc,0,0,700,600,dcMem,0,0,SRCCOPY);
	ReleaseDC(m_hWnd,hDc);
	DeleteObject(dcMem);
	DeleteObject(hBitmep);
}

void cDefense::DrawMap(HDC dcMem)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL,L"images\\maps\\bg.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	SelectObject(dcTemp,hBitmap);

	BITMAP bitInfo;
	GetObject(hBitmap,sizeof(BITMAP),&bitInfo);
	BitBlt(dcMem,0,0,bitInfo.bmWidth,bitInfo.bmHeight,dcTemp,0,0,SRCCOPY);

	HBITMAP hBitmapBase1 = (HBITMAP)LoadImage(NULL,L"images\\base\\base1.bmp",IMAGE_BITMAP,0,0,
		LR_LOADFROMFILE);
	HBITMAP hBitmapBase2 = (HBITMAP)LoadImage(NULL,L"images\\base\\base2.bmp",IMAGE_BITMAP,0,0,
		LR_LOADFROMFILE);

	BITMAP bitInfo2;
	GetObject(hBitmapBase1,sizeof(BITMAP),&bitInfo2);
	SelectObject(dcTemp,hBitmapBase1);

	for (list<tagPOINT>::iterator it = m_ptBaseList.begin();it != m_ptBaseList.end();it++)
	{
		if (m_mouseItemIndex >= 0 && m_mouseItemIndex < Max_Building_Num)
		{
			tagPOINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_hWnd,&pt);
			RECT rect;
			rect.left = it->x-bitInfo2.bmWidth/2;
			rect.top = it->y - bitInfo2.bmHeight/2;
			rect.right = rect.left+bitInfo2.bmWidth;
			rect.bottom = rect.top+bitInfo2.bmHeight;

			if (IsInRect(pt,rect))
			{
				SelectObject(dcTemp,hBitmapBase2);
			}
			else
			{
				SelectObject(dcTemp,hBitmapBase1);
			}
		}
		TransparentBlt(dcMem,it->x-bitInfo2.bmWidth/2,it->y-bitInfo2.bmHeight/2,
			bitInfo2.bmWidth,bitInfo2.bmHeight,dcTemp,0,0,bitInfo2.bmWidth,bitInfo2.bmHeight,
			RGB(34,177,76));
	}

	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
	DeleteObject(hBitmapBase1);
	DeleteObject(hBitmapBase2);
}

void cDefense::InitBase()
{
	tagPOINT pt;
	pt.x = 205;
	pt.y = 185;
	m_ptBaseList.push_back(pt);

	pt.x = 265;
	pt.y = 340;
	m_ptBaseList.push_back(pt);

	pt.x = 465;
	pt.y = 350;
	m_ptBaseList.push_back(pt);

	pt.x = 595;
	pt.y = 440;
	m_ptBaseList.push_back(pt);
}

void cDefense::DrawItem(HDC dcMem)
{
	//绘制图标
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(NULL);
	HBITMAP bitmap1 = (HBITMAP)LoadImage(NULL,L"images\\Fortress\\small1.bmp",IMAGE_BITMAP,
		0,0,LR_LOADFROMFILE);
	SelectObject(dcTemp,bitmap1);
	TransparentBlt(dcMem,m_rectBuilding[0].left,m_rectBuilding[0].top,
		m_rectBuilding[0].right-m_rectBuilding[0].left,
		m_rectBuilding[0].bottom-m_rectBuilding[0].top,dcTemp,0,0,
		m_rectBuilding[0].right-m_rectBuilding[0].left,
		m_rectBuilding[0].bottom-m_rectBuilding[0].top,RGB(34,177,76));
	HBITMAP bitmap2 = (HBITMAP)LoadImage(NULL,L"images\\Fortress\\small2.bmp",IMAGE_BITMAP,
		0,0,LR_LOADFROMFILE);
	SelectObject(dcTemp,bitmap2);
	TransparentBlt(dcMem,m_rectBuilding[1].left,m_rectBuilding[1].top,
		m_rectBuilding[1].right-m_rectBuilding[1].left,
		m_rectBuilding[1].bottom-m_rectBuilding[1].top,dcTemp,0,0,
		m_rectBuilding[1].right-m_rectBuilding[1].left,
		m_rectBuilding[1].bottom-m_rectBuilding[1].top,RGB(34,177,76));

	//绘制建筑物
	for (BuildList::iterator it = m_buildList.begin();it != m_buildList.end();it++)
	{
		wchar_t szFileName[100] = {0};
		if (it->type == Building_Type1)
		{
			wsprintf(szFileName,L"images\\Fortress\\1_%d.bmp",it->curIndex);
		}
		else if (it->type == Building_Type2)
		{
			wsprintf(szFileName,L"images\\Fortress\\2_%d.bmp",it->curIndex);
		}
		it->curIndex = (it->curIndex+1)%18+1;

		HBITMAP bitmapBuilding = (HBITMAP)LoadImage(NULL,szFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		SelectObject(dcTemp,bitmapBuilding);
		BITMAP bitInfo;
		GetObject(bitmapBuilding,sizeof(bitInfo),&bitInfo);
		TransparentBlt(dcMem,it->pt.x,it->pt.y,bitInfo.bmWidth,bitInfo.bmHeight,dcTemp,0,0,
			bitInfo.bmWidth,bitInfo.bmHeight,RGB(34,177,76));
		DeleteObject(bitmapBuilding);
	}

	//绘制鼠标上的建筑物
	if (m_mouseItemIndex >= 0 && m_mouseItemIndex< Max_Building_Num)
	{
		wchar_t szFileName[100];
		wsprintf(szFileName,L"images\\Fortress\\%d_16.bmp",m_mouseItemIndex+1);
		HBITMAP bitmapMouse = (HBITMAP)LoadImage(NULL,szFileName,
			IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		//根据鼠标位置来绘制
		tagPOINT ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(m_hWnd,&ptCursor);
		BITMAP bitInfo;
		GetObject(bitmapMouse,sizeof(BITMAP),&bitInfo);
		tagPOINT ptPos;
		ptPos.x = ptCursor.x - bitInfo.bmWidth/2;
		ptPos.y = ptCursor.y - bitInfo.bmHeight*2/3;
		SelectObject(dcTemp,bitmapMouse);
		TransparentBlt(dcMem,ptPos.x,ptPos.y,bitInfo.bmWidth,bitInfo.bmHeight,dcTemp,
			0,0,bitInfo.bmWidth,bitInfo.bmHeight,RGB(34,177,76));
		DeleteObject(bitmapMouse);
	}

	DeleteObject(dcTemp);
	DeleteObject(bitmap1);
	DeleteObject(bitmap2);
}

void cDefense::OnLButtonUp(LPARAM lParam)
{
	tagPOINT ptCur;
	ptCur.x = GET_X_LPARAM(lParam);
	ptCur.y = GET_Y_LPARAM(lParam);

	//判断点击在哪里
	int baseIndex = -1;
	if (IsInRect(ptCur,m_rectBuilding[0]))
	{
		//点击了第一个建筑图标
		m_mouseItemIndex = 0;
	}
	else if (IsInRect(ptCur,m_rectBuilding[1]))
	{
		//点击了第二个建筑图标
		m_mouseItemIndex = 1;
	}
	else if ((baseIndex = GetBaseIndex(ptCur)) != -1)
	{
		//在某个地基上点了一下
		AddBuilding(baseIndex);
	}

}

void cDefense::OnRButtonDown(LPARAM lParam)
{
	m_mouseItemIndex = -1;
}

bool cDefense::IsInRect(tagPOINT pt,RECT rect)
{
	if (pt.x <= rect.right && pt.x >= rect.left
	   && pt.y <= rect.bottom && pt.y >= rect.top)
	{
		return true;
	}
	return false;
}

int cDefense::GetBaseIndex(tagPOINT pt)
{
	int i = 0;
	for (list<tagPOINT>::iterator it = m_ptBaseList.begin();
		it != m_ptBaseList.end();it++)
	{
		RECT rect;
		rect.left = it->x-31;
		rect.top = it->y-30;
		rect.right = rect.left+63;
		rect.bottom = rect.top+60;
		if (IsInRect(pt,rect))
		{
			return i;
		}
		i++;
	}
	return -1;
}

void cDefense::AddBuilding(int baseIndex)
{
	//增加建筑物
	if (m_mouseItemIndex < 0 || m_mouseItemIndex>= Max_Building_Num)
	{
		return;
	}
	if (m_mouseItemIndex == 0 && m_iMoney < 60)
	{
		return;
	}
	if (m_mouseItemIndex == 1 && m_iMoney < 100)
	{
		return;
	}

	stBuildingInfo stInfo;
	//先要找到地基的坐标

	int i =0;
	tagPOINT basePt;
	for (list<tagPOINT>::iterator it = m_ptBaseList.begin();it != m_ptBaseList.end();it++)
	{
		if (i == baseIndex)
		{
			basePt.x = it->x;
			basePt.y = it->y;
			m_ptBaseList.erase(it);
			break;
		}
	}

	//根据地基的坐标算出建筑物的坐标
	if (m_mouseItemIndex == 0)
	{
		stInfo.pt.x = basePt.x - 31 -20;
		stInfo.pt.y = basePt.y - 30 -20;
		stInfo.type = Building_Type1;
		stInfo.curIndex = 0;
	}
	else if (m_mouseItemIndex == 1)
	{
		stInfo.pt.x = basePt.x - 31 -30;
		stInfo.pt.y = basePt.y - 30 -50;
		stInfo.type = Building_Type2;
		stInfo.curIndex = 0;
	}
	m_buildList.push_back(stInfo);
	m_mouseItemIndex = -1;
	m_iMoney -= stInfo.type == Building_Type1?60:100;
}

void cDefense::AddMonster()
{
	//怪物的初始坐标点都一样的
	stMonsterInfo monsterInfo;
	monsterInfo.pt.x = 327;
	monsterInfo.pt.y = -40;

	
}