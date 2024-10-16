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
		case WM_MOUSEMOVE:
			//����ƶ�
			g_UIManager->OnMouseMove();
			break;
		case WM_LBUTTONUP:
			//����������
			g_UIManager->OnLButtonUp();
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

cText* cUIManager::CreateText(LPCSTR lpText,UINT testColor,TextType textType)
{
	cText* pText = new cText;
	if (!pText->Create(lpText,testColor,textType))
	{
		delete pText;
		return NULL;
	}
	m_uiList.push_back(pText);
	return pText;
}

cButton* cUIManager::CreateButton(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,
	LPCSTR lpPushDownImagePath)
{
	cButton* pButton = new cButton;
	if (!pButton->Create(lpNormalImagePath,lpMouseOnImagePath,lpPushDownImagePath))
	{
		delete pButton;
		return NULL;
	}
	m_uiList.push_back(pButton);
	return pButton;
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
		else if ((*it)->GetControlType() == type_text)
		{
			//��������
			cText* pText = (cText*)(*it);
			if (pText == NULL)
			{
				continue;
			}
			DrawText(pText,memDc);
		}
		else if ((*it)->GetControlType() == type_button)
		{
			//����BUTTON
			cButton* pButton = (cButton*)(*it);
			if (pButton == NULL)
			{
				continue;
			}
			HBITMAP hBitmap = pButton->GetCurStatusHBitmap();
			if (hBitmap == NULL)
			{
				continue;
			}
			HDC dcTemp = CreateCompatibleDC(NULL);
			SelectObject(dcTemp,hBitmap);
			tagPOINT pt = pButton->GetPos();
			TransparentBlt(memDc,pt.x,pt.y,pButton->GetWidth(),pButton->GetHeight(),
				dcTemp,0,0,pButton->GetWidth(),pButton->GetHeight(),
				RGB(255,255,255));
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

void cUIManager::DrawText(cText* pText,HDC memDc)
{
	HDC dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = CreateCompatibleBitmap(memDc,pText->GetWidth(),pText->GetHeight());
	SelectObject(dcTemp,hBitmap);
	SetBkMode(dcTemp,0);

	BitBlt(dcTemp,0,0,pText->GetWidth(),pText->GetHeight(),
		memDc,pText->GetPos().x,pText->GetPos().y,SRCCOPY);

	tagPOINT pt = pText->GetPos();
	switch(pText->GetTextType())
	{
	case text_twinkle:
		{
			//��˸����
			UINT color = pText->GetTextColor();
			srand(GetTickCount());
			int randValue = rand()%256;
			UINT rValue = rand()%256;//0xff & color;
			UINT gValue = rand()%256;//;0xff & (color >> 8);
			UINT bValue = rand()%256;//0xff & (color >> 16);
		/*	rValue = (rValue+50)%256;
			gValue = (gValue+50)%256;
			bValue = (bValue+50)%256;*/
			pText->SetTextColor(RGB(rValue,gValue,bValue));
		}
	case text_normal:
		{
			UINT colorOld = SetTextColor(dcTemp,pText->GetTextColor());
			LOGFONT logfont;	//�ı��������
			ZeroMemory(&logfont,sizeof(LOGFONT));
			logfont.lfCharSet = GB2312_CHARSET;
			logfont.lfHeight = pText->GetTextSize();//���������С
			logfont.lfWeight = 700;	//�Ӵ�
			HFONT hFont = CreateFontIndirect(&logfont);
			HGDIOBJ hOldFont = SelectObject(dcTemp,hFont);
			TextOutA(dcTemp,0,0,
				pText->GetText(),strlen(pText->GetText()));
			SetTextColor(dcTemp,colorOld);
			SelectObject(dcTemp,hOldFont);
			DeleteObject(hFont);
			break;
		}
	case text_scoll:
		{
			UINT colorOld = SetTextColor(dcTemp,pText->GetTextColor());
			LOGFONT logfont;	//�ı��������
			ZeroMemory(&logfont,sizeof(LOGFONT));
			logfont.lfCharSet = GB2312_CHARSET;
			logfont.lfHeight = pText->GetTextSize();//���������С
			logfont.lfWeight = 700;	//�Ӵ�
			HFONT hFont = CreateFontIndirect(&logfont);
			HGDIOBJ hOldFont = SelectObject(dcTemp,hFont);

			int len = strlen(pText->GetText())*10;
			TextOutA(dcTemp,pText->m_pos.x,pText->m_pos.y,
				pText->GetText(),strlen(pText->GetText()));
			pText->m_pos.x += 5;
			int controlWidth = pText->GetWidth();
			if (pText->m_pos.x >= controlWidth)
			{
				pText->m_pos.x = -len;
			}
			SetTextColor(dcTemp,colorOld);
			SelectObject(dcTemp,hOldFont);
			DeleteObject(hFont);
			break;
		}
	}
	BitBlt(memDc,pText->GetPos().x,pText->GetPos().y,pText->GetWidth(),
		pText->GetHeight(),dcTemp,0,0,SRCCOPY);
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
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
	//����Ƿ��ڰ�Ŧ��ɨ�����
	CheckClickButton();
}


void cUIManager::OnLButtonUp()
{
	//�������Ƿ���BUTTON��
	CheckButtonUp();
}

//����ƶ�
void cUIManager::OnMouseMove()
{
	CheckOnButton();
}

//����Ƿ�Ӱ�Ŧ�ϵ���
void cUIManager::CheckButtonUp()
{
	tagPOINT pt ;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if ((*it)->GetControlType() != type_button)
		{
			continue;
		}
		cButton* pButton = (cButton*)(*it);
		if (pButton == NULL)
		{
			continue;
		}
		if (IsInRect(pButton->GetRect(),pt))
		{
			pButton->SetStatus(status_mouseon);
		}
	}
}

//����Ƿ��ƶ�����Ŧ
void cUIManager::CheckOnButton()
{
	tagPOINT pt ;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if ((*it)->GetControlType() != type_button)
		{
			continue;
		}
		cButton* pButton = (cButton*)(*it);
		if (pButton == NULL)
		{
			continue;
		}
		if (IsInRect(pButton->GetRect(),pt))
		{
			if (pButton->GetStatus() == status_pushdown)
			{
				continue;
			}
			pButton->SetStatus(status_mouseon);
		}
		else
		{
			pButton->SetStatus(status_normal);
		}
	}
}

//����Ƿ��ڰ�Ŧ�ϰ���
void cUIManager::CheckClickButton()
{
	tagPOINT pt ;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if ((*it)->GetControlType() != type_button)
		{
			continue;
		}
		cButton* pButton = (cButton*)(*it);
		if (pButton == NULL)
		{
			continue;
		}
		if (IsInRect(pButton->GetRect(),pt))
		{
			pButton->SetStatus(status_pushdown);
		}
		else
		{
			pButton->SetStatus(status_normal);
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

/************************************************************************/
/* ���ֿؼ�                                                                     */
/************************************************************************/
cText::cText()
{
	m_type = type_text;
	m_textSize = 18;	//�����С

	m_width = 100;	//���
	m_height = 50;	//�߶�

	m_pos.x = 0;
	m_pos.y = 0;
}
cText::~cText()
{

}
//�������ԣ������С��������ɫ
//��������
bool cText::Create(LPCSTR lpText,UINT testColor,TextType textType)
{
	m_text = lpText;
	m_textColor = testColor;
	m_textType = textType;

	int len = strlen(lpText)*10;
	m_pos.x = -len;
	return true;
}

TextType cText::GetTextType()
{
	return m_textType;
}

UINT cText::GetTextColor()
{
	return m_textColor;
}

void cText::SetTextColor(UINT rgbColor)
{
	m_textColor = rgbColor;
}

UINT cText::GetTextSize()
{
	return m_textSize;
}

void cText::SetTextSize(UINT size)
{
	m_textSize = size;
}

LPCSTR cText::GetText()
{
	return m_text.c_str();
}

//��ÿ��
unsigned cText::GetWidth()
{
	return m_width;
}

//��ø߶�
unsigned cText::GetHeight()
{
	return m_height;
}
//���ÿ��
void cText::SetWidth(UINT width)
{
	m_width = width;
}
//���ø߶�
void cText::SetHeight(UINT height)
{
	m_height = height;
}


/************************************************************************/
/* BUTTON  ��Ŧ��                                                                     */
/************************************************************************/
cButton::cButton()
{
	m_hBitmapMouseOn = NULL;
	m_hBitmapNormal = NULL;
	m_hBitmapPushDown = NULL;
	m_status = status_normal;
	m_type = type_button;
}
cButton::~cButton()
{
	if (m_hBitmapNormal)
	{
		DeleteObject(m_hBitmapNormal);
	}
	if (m_hBitmapMouseOn)
	{
		DeleteObject(m_hBitmapMouseOn);
	}
	if (m_hBitmapPushDown)
	{
		DeleteObject(m_hBitmapPushDown);
	}
}

//����״̬������״̬������ڰ�Ŧ�ϣ���갴�£����¼���Ӧ

//����һ��BUTTON
bool cButton::Create(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,
	LPCSTR lpPushDownImagePath)
{
	HBITMAP hBitmap = (HBITMAP)LoadImageA(NULL,lpNormalImagePath,IMAGE_BITMAP,
		0,0,LR_LOADFROMFILE);
	if (hBitmap == NULL)
	{
		return false;
	}
	m_hBitmapNormal = hBitmap;
	hBitmap = (HBITMAP)LoadImageA(NULL,lpMouseOnImagePath,IMAGE_BITMAP,
		0,0,LR_LOADFROMFILE);
	if (hBitmap == NULL)
	{
		return false;
	}
	m_hBitmapMouseOn = hBitmap;

	hBitmap = (HBITMAP)LoadImageA(NULL,lpPushDownImagePath,IMAGE_BITMAP,
		0,0,LR_LOADFROMFILE);
	if (hBitmap == NULL)
	{
		return false;
	}
	m_hBitmapPushDown = hBitmap;
	return true;
}

//���BUTTON״̬
ButtonStatus cButton::GetStatus()
{
	return m_status;
}
//����״̬
void cButton::SetStatus(ButtonStatus status)
{
	m_status = status;
}

HBITMAP cButton::GetCurStatusHBitmap()
{
	if (m_status == status_normal)
	{
		return m_hBitmapNormal;
	}
	if (m_status == status_mouseon)
	{
		return m_hBitmapMouseOn;
	}
	if (m_status == status_pushdown)
	{
		return m_hBitmapPushDown;
	}
	return NULL;
}

//��ÿ��
unsigned cButton::GetWidth()
{
	BITMAP bitinfo;
	GetObject(m_hBitmapNormal,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmWidth;
}

//��ø߶�
unsigned cButton::GetHeight()
{
	BITMAP bitinfo;
	GetObject(m_hBitmapNormal,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}