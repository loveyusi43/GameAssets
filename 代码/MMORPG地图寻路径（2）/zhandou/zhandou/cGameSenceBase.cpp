#include "StdAfx.h"
#include "cGameSenceBase.h"
#pragma comment(lib,"msimg32.lib")

cGameSenceBase::cGameSenceBase(void)
{
	m_ptScreenToMap.x = 0;
	m_ptScreenToMap.y = 0;
}


cGameSenceBase::~cGameSenceBase(void)
{
	//循环删除所有位图

}

void cGameSenceBase::Init(HWND hWnd)
{
	 m_hWnd = hWnd;
	 AddTimer(Timer_Draw,50);
}

int cGameSenceBase::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_Draw:
		Draw();
		break;
	}
	return 1;
}

void cGameSenceBase::Draw()
{
	HDC hDc = GetDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}
	HDC dcMem = CreateCompatibleDC(hDc);
	RECT rect;
	GetClientRect(m_hWnd,&rect);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,rect.right-rect.left,
		rect.bottom-rect.top);
	SelectObject(dcMem,hBitmap);

	HBRUSH hBrushBack = CreateSolidBrush(RGB(255,255,255));
	SelectObject(dcMem,hBrushBack);
	FillRect(dcMem,&rect,hBrushBack);

	//把所有要绘制的东西绘制到dcMem上
	HDC dcTemp = CreateCompatibleDC(NULL);
	for (int i = Layer_Main_Map;i<Layer_Main_Num;i++)
	{
		DWORD timeNow = GetTickCount();
		for (list<ST_ANIPICTURE>::iterator it = m_listAniPic[i].begin();
			it != m_listAniPic[i].end();it++)
		{
			tagPOINT ptToScreen;
			ptToScreen.x = it->ptPos.x;
			ptToScreen.y = it->ptPos.y;
			if (i == Layer_Main_Player)
			{
				ptToScreen.x = it->ptPos.x-m_ptScreenToMap.x;
				ptToScreen.y = it->ptPos.y-m_ptScreenToMap.y;
				
			}
			if (it->bAni)
			{
				//绘制动画
				int curIndex = it->curIndex;
				if (timeNow-it->lastSwitchTime >= it->rate)
				{
					curIndex = (curIndex+1)%it->bimapVec.size();
					it->lastSwitchTime = timeNow;
				}
				it->curIndex = curIndex;
				BITMAP bitmap;
				GetObject(it->bimapVec[curIndex],sizeof(BITMAP),&bitmap);
				SelectObject(dcTemp,it->bimapVec[curIndex]);

				TransparentBlt(dcMem,ptToScreen.x,ptToScreen.y,
					bitmap.bmWidth,bitmap.bmHeight,dcTemp,0,0,
					bitmap.bmWidth,bitmap.bmHeight,it->transparentRGB);
			}
			else
			{
				//绘制单张图
				BITMAP bitmap;
				GetObject(it->bimapVec[0],sizeof(BITMAP),&bitmap);
				SelectObject(dcTemp,it->bimapVec[0]);
				TransparentBlt(dcMem,ptToScreen.x,ptToScreen.y,
					bitmap.bmWidth,bitmap.bmHeight,dcTemp,0,0,
					bitmap.bmWidth,bitmap.bmHeight,it->transparentRGB);
			}
		}
	}

	//再把dcMem上的内容绘制到hDc上
	BitBlt(hDc,0,0,rect.right-rect.left,rect.bottom-rect.top,dcMem,0,0,
		SRCCOPY);

	DeleteObject(dcMem);
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
	ReleaseDC(m_hWnd,hDc);
	DeleteObject(hBrushBack);
}

bool cmp(ST_ANIPICTURE st1,ST_ANIPICTURE st2)
{
	return st1.ptPos.y < st2.ptPos.y;
}

ST_ANIPICTURE* cGameSenceBase::AddPicture(wstring picPath,tagPOINT pt,
	Layer_Main_Type layer,UINT rgb)
{
	if (layer < Layer_Main_Map || layer >= Layer_Main_Num)
	{
		return NULL;
	}
	ST_ANIPICTURE st;
	memcpy(&st.ptPos,&pt,sizeof(tagPOINT));
	st.transparentRGB = rgb;
	st.layerMain = layer;

	HBITMAP hBitmap = GetBitmapFromList(picPath);
	if (hBitmap == NULL)
	{
		hBitmap = (HBITMAP)LoadImage(NULL,picPath.c_str(),
			IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if (hBitmap == NULL)
		{
			return NULL;
		}

		stBitmapInfo stBit;
		stBit.strPath = picPath;
		stBit.hBitmap = hBitmap;
		m_listBitmapInfo.push_back(stBit);
	}
	

	st.bimapVec.push_back(hBitmap);

	m_listAniPic[layer].push_back(st);

	ST_ANIPICTURE* p = &*(m_listAniPic[layer].rbegin());
	m_listAniPic[layer].sort(cmp);
	return p;
}

ST_ANIPICTURE* cGameSenceBase::AddAnimation(list<wstring> listPath,tagPOINT pt,Layer_Main_Type layer,
	UINT rgb,int rate)
{
	if (layer < Layer_Main_Map || layer >= Layer_Main_Num)
	{
		return NULL;
	}
	ST_ANIPICTURE st;
	memcpy(&st.ptPos,&pt,sizeof(tagPOINT));
	st.transparentRGB = rgb;
	st.layerMain = layer;
	st.rate = rate;
	st.bAni = true;

	for (list<wstring>::iterator it = listPath.begin();it != listPath.end();
		it++)
	{
		HBITMAP hBitmap = GetBitmapFromList(*it);
		if (hBitmap == NULL)
		{
			hBitmap = (HBITMAP)LoadImage(NULL,it->c_str(),
				IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
			if (hBitmap == NULL)
			{
				return NULL;
			}

			stBitmapInfo stBit;
			stBit.strPath = *it;
			stBit.hBitmap = hBitmap;
			m_listBitmapInfo.push_back(stBit);
		}
		st.bimapVec.push_back(hBitmap);
	}
	m_listAniPic[layer].push_back(st);
	ST_ANIPICTURE* p = &*(m_listAniPic[layer].rbegin());
	m_listAniPic[layer].sort(cmp);
	return p;
}


HBITMAP cGameSenceBase::GetBitmapFromList(wstring picPath)
{
	for (list<ST_BITMAPINFO>::iterator it = m_listBitmapInfo.begin();it !=
		m_listBitmapInfo.end();it++)
	{
		if (it->strPath == picPath)
		{
			return it->hBitmap;
		}
	}
	return NULL;
}