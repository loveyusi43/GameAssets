#include "StdAfx.h"
#include "cGameMain.h"


cGameMain::cGameMain(void)
{
	m_pSence = NULL;
	m_pMap= NULL;
	m_seeSencePt.x = 0;
	m_seeSencePt.y = 0;
}


cGameMain::~cGameMain(void)
{
	if (m_pSence != NULL)
	{
		delete m_pSence;
	}
}

int cGameMain::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_MoveMap:
		MoveMap();
		break;
	}
	return 1;
}

void cGameMain::Init(HWND hWnd)
{
	m_hWnd = hWnd;
	m_pSence = new cGameSenceBase;
	if (m_pSence == NULL)
	{
		return;
	}
	m_pSence->Init(m_hWnd);
	tagPOINT pt;
	pt.x =0;
	pt.y = 0;
	m_pMap = m_pSence->AddPicture(L"mapBig.bmp",pt,Layer_Main_Map);

	AddTimer(Timer_MoveMap,10);
}

void cGameMain::MoveMap()
{
	if (m_pMap == NULL)
	{
		return;
	}
	tagPOINT pt;
	GetCursorPos(&pt);	//是相对于整个电脑屏幕的鼠标位置
	RECT rect;
	GetWindowRect(m_hWnd,&rect);
	MoveDir moveDir = Dir_Num;
	if (pt.x < rect.left)
	{
		if (pt.y < rect.top)
		{
			moveDir = Dir_UpLeft;
		}
		else if (pt.y > rect.bottom)
		{
			moveDir = Dir_LeftDown;
		}
		else
		{
			moveDir = Dir_Left;
		}
	}
	else if (pt.x > rect.right)
	{
		if (pt.y < rect.top)
		{
			moveDir = Dir_RightUp;
		}
		else if (pt.y > rect.bottom)
		{
			moveDir = Dir_DownRight;
		}
		else
		{
			moveDir = Dir_Right;
		}
	}
	else if (pt.y < rect.top)
	{
		moveDir = Dir_Up;
	}
	else if (pt.y > rect.bottom)
	{
		moveDir = Dir_Down;
	}
	else
	{
		moveDir = Dir_Num;
	}
	if (moveDir == Dir_Num)
	{
		return;
	}
	GetNextPosByDir(m_seeSencePt,moveDir);
	m_pMap->ptPos.x = -m_seeSencePt.x;
	m_pMap->ptPos.y = -m_seeSencePt.y;
}

void cGameMain::GetNextPosByDir(tagPOINT& ptRes,MoveDir dir)
{
	//获得大地图的宽和高，以及窗口的宽和高
	int mapWidth;
	int mapHeight;
	BITMAP bitmap;
	GetObject(m_pMap->bimapVec[0],sizeof(BITMAP),&bitmap);
	mapWidth = bitmap.bmWidth;
	mapHeight = bitmap.bmHeight;

	int wndWidth;
	int wndHeight;
	RECT rectWnd;
	GetClientRect(m_hWnd,&rectWnd);
	wndHeight = rectWnd.bottom-rectWnd.top;
	wndWidth = rectWnd.right-rectWnd.left;
	switch(dir)
	{
	case  Dir_Left:
		ptRes.x -= MAP_MOVE_STEP;
		if (ptRes.x < 0)
		{
			ptRes.x = 0;
		}
		break;
	case Dir_LeftDown:
		ptRes.x -= MAP_MOVE_STEP;
		if (ptRes.x < 0)
		{
			ptRes.x = 0;
		}
		ptRes.y += MAP_MOVE_STEP;
		if (ptRes.y >mapHeight-wndHeight)
		{
			ptRes.y =mapHeight-wndHeight;
		}
		break;
	case Dir_Down:
		ptRes.y += MAP_MOVE_STEP;
		if (ptRes.y >mapHeight-wndHeight)
		{
			ptRes.y =mapHeight-wndHeight;
		}
		break;
	case Dir_DownRight:
		ptRes.y += MAP_MOVE_STEP;
		if (ptRes.y >mapHeight-wndHeight)
		{
			ptRes.y =mapHeight-wndHeight;
		}
		ptRes.x += MAP_MOVE_STEP;
		if (ptRes.x > mapWidth-wndWidth)
		{
			ptRes.x = mapWidth-wndWidth;
		}
		break;
	case Dir_Right:
		ptRes.x += MAP_MOVE_STEP;
		if (ptRes.x > mapWidth-wndWidth)
		{
			ptRes.x = mapWidth-wndWidth;
		}
		break;
	case Dir_RightUp:
		ptRes.x += MAP_MOVE_STEP;
		if (ptRes.x > mapWidth-wndWidth)
		{
			ptRes.x = mapWidth-wndWidth;
		}
		ptRes.y -= MAP_MOVE_STEP;
		if (ptRes.y<0)
		{
			ptRes.y =0;
		}
		break;
	case Dir_Up:
		ptRes.y -= MAP_MOVE_STEP;
		if (ptRes.y<0)
		{
			ptRes.y =0;
		}
		break;
	case Dir_UpLeft:
		ptRes.y -= MAP_MOVE_STEP;
		if (ptRes.y<0)
		{
			ptRes.y =0;
		}
		ptRes.x -= MAP_MOVE_STEP;
		if (ptRes.x < 0)
		{
			ptRes.x = 0;
		}
		break;
	}
}