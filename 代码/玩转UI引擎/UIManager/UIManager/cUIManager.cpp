#include "stdafx.h"
#include "cUIManager.h"

#pragma comment(lib,"msimg32.lib")

using namespace myui;
/************************************************************************/
/*ȫ�ֱ���                                                                   */
/************************************************************************/
cUIManager* g_UIManager = NULL;//UI������
HHOOK g_hhkmouse = 0;	//����

/************************************************************************/
/* ȫ�ֺ���                                                                     */
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
		//����Ӧ��Ϣ
		switch(wParam)
		{
		case WM_LBUTTONDOWN:
			//��Ӧ��������Ϣ�¼�
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
/* UI������                                                                     */
/************************************************************************/

cUIManager::cUIManager(HWND hWnd)
{
	m_hWnd = hWnd;
	AddTimer(1,50);
	//��װ����
	g_hhkmouse =  SetWindowsHookEx(WH_MOUSE_LL,MouseProc,NULL,0);
}

cUIManager::~cUIManager()
{
	//жװ����
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
	//�ѿؼ����Ƶ�memDc
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if(!(*it))
		{
			continue;
		}
		if ((*it)->GetControlType() == type_picture)
		{
			//����ͼƬ
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

	//�ٰ�memDc���Ƶ�hDc��
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
		//�ж��Ƿ�����������ؼ�
		if (IsInRect(it->pUI->GetRect(),pt))
		{
			//���ûص�����
			it->func();
		}
	}
}
/************************************************************************/
/*UI���� cBaseUI 
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
//��ÿؼ�λ��
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
/*ͼƬ��   cPicture 
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

//���λͼ�ĸ߶�
unsigned cPicture::GetHeight()
{
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}