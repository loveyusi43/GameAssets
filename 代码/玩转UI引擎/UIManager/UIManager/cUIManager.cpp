#include "stdafx.h"
#include "cUIManager.h"

#pragma comment(lib,"msimg32.lib")

using namespace myui;
/************************************************************************/
/*全局变量                                                                   */
/************************************************************************/
cUIManager* g_UIManager = NULL;//UI管理器
HHOOK g_hhkmouse = 0;	//钩子

/************************************************************************/
/* 全局函数                                                                     */
/************************************************************************/
myui::cUIManager* CreateUI(HWND hWnd)
{
	cUIManager* uimanager = new cUIManager(hWnd);
	g_UIManager = uimanager;
	return uimanager;
}

LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if (g_UIManager == NULL)
	{
		return CallNextHookEx(g_hhkmouse,nCode,wParam,lParam);
	}
	HWND hWnd = GetActiveWindow();
	if (hWnd == g_UIManager->GetBindHwnd())
	{
		//才响应消息
		switch(wParam)
		{
		case WM_LBUTTONDOWN:
			//响应左键点击消息事件
			g_UIManager->OnLButtonDown();
			break;
		}
	}


	return CallNextHookEx(g_hhkmouse,nCode,wParam,lParam);
}

bool IsInRect(RECT rect,tagPOINT pt)
{
	if (pt.x >= rect.left && pt.x <= rect.right
		&& pt.y >= rect.top && pt.y <= rect.bottom)
	{
		return true;
	}
	return false;
}

/************************************************************************/
/* UI管理器                                                                     */
/************************************************************************/

cUIManager::cUIManager(HWND hWnd)
{
	m_hWnd = hWnd;
	AddTimer(1,50);
	//安装钩子
	g_hhkmouse =  SetWindowsHookEx(WH_MOUSE_LL,MouseProc,NULL,0);
}

cUIManager::~cUIManager()
{
	//卸装钩子
	UnhookWindowsHookEx(g_hhkmouse);
}

HWND cUIManager::GetBindHwnd()
{
	return m_hWnd;
}

cPicture* cUIManager::CreatePicture(LPCSTR lpFilePath)
{
	cPicture* pic = new cPicture;
	if (!pic->Create(lpFilePath))
	{
		delete pic;
		return NULL;
	}
	m_uiList.push_back(pic);
	return pic;
}

int cUIManager::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case 1:
		Draw();
		break;
	}
	return 1;
}

void cUIManager::Draw()
{
	if (m_hWnd == NULL)
	{
		return;
	}
	HDC hDc = GetDC(m_hWnd);
	HDC memDc = CreateCompatibleDC(hDc);
	RECT rect;
	GetClientRect(m_hWnd,&rect);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,rect.right-rect.left,
		rect.bottom-rect.top);
	SelectObject(memDc,hBitmap);
	HBRUSH hBrushBack = CreateSolidBrush(RGB(255,255,255));
	SelectObject(memDc,hBrushBack);
	FillRect(memDc,&rect,hBrushBack);
	//把控件绘制到memDc
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if(!(*it))
		{
			continue;
		}
		if ((*it)->GetControlType() == type_picture)
		{
			//绘制图片
			cPicture* pic = (cPicture*)(*it);
			if (pic == NULL)
			{
				continue;
			}
			tagPOINT pt = pic->GetPos();
			HDC dcTemp = CreateCompatibleDC(NULL);
			SelectObject(dcTemp,pic->GetHBitmap());
			TransparentBlt(memDc,pt.x,pt.y,pic->GetWidth(),pic->GetHeight(),
				dcTemp,0,0,pic->GetWidth(),pic->GetHeight(),RGB(0,0,0));
			DeleteObject(dcTemp);
		}
	}

	//再把memDc绘制到hDc上
	BitBlt(hDc,0,0,rect.right-rect.left,
		rect.bottom-rect.top,memDc,0,0,SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(memDc);
	ReleaseDC(m_hWnd,hDc);
}

void cUIManager::AddEvent(cBaseUI* pUI,EventType eventType,EventFunc func)
{
	stEvent event;
	event.pUI = pUI;
	event.func = func;
	event.eventType = eventType;
	m_eventList.push_back(event);
}

void cUIManager::OnLButtonDown()
{
	tagPOINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<stEvent>::iterator it = m_eventList.begin();
		it != m_eventList.end();it++)
	{
		if (it->eventType != event_lbuttondown)
		{
			continue;
		}
		//判断是否点击到了这个控件
		if (IsInRect(it->pUI->GetRect(),pt))
		{
			//调用回调函数
			it->func();
		}
	}
}
/************************************************************************/
/*UI基类 cBaseUI 
*/
/************************************************************************/

cBaseUI::cBaseUI()
{
	m_ptPos.x = 0;
	m_ptPos.y = 0;
	m_type = type_num;
}

cBaseUI::~cBaseUI()
{

}

void cBaseUI::SetPos(tagPOINT pt)
{
	m_ptPos = pt;
}
//获得控件位置
tagPOINT cBaseUI::GetPos()
{
	return m_ptPos;
}

RECT cBaseUI::GetRect()
{
	RECT rect;
	rect.left = m_ptPos.x;
	rect.top = m_ptPos.y;
	rect.right = rect.left+GetWidth();
	rect.bottom = rect.top+GetHeight();
	return rect;
}
/************************************************************************/
/*图片类   cPicture 
*/
/************************************************************************/
cPicture::cPicture()
{
	m_hBitmap = NULL;
	m_type = type_picture;
}
cPicture::~cPicture()
{

}

bool cPicture::Create(LPCSTR lpFilePath)
{
	HBITMAP hBitmap = (HBITMAP)LoadImageA(NULL,lpFilePath,
		IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if (!hBitmap)
	{
		return false;
	}
	m_hBitmap = hBitmap;
	return true;
}

HBITMAP cPicture::GetHBitmap()
{
	return m_hBitmap;
}

unsigned cPicture::GetWidth()
{
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmWidth;
}

//获得位图的高度
unsigned cPicture::GetHeight()
{
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}