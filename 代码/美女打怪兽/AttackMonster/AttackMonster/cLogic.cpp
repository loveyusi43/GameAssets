#include "stdafx.h"
#include "cLogic.h"
#pragma comment(lib,"Msimg32.lib")



cLogic::cLogic()
{
	m_hWnd = NULL;
	m_curHp = Player_Max_HP;
	for (int i = 0; i < Monster_Type_Max;i++)
	{
		wchar_t fileName[100];
		wsprintf(fileName, _T("images\\monster%d.bmp"), i + 1);
		m_hMonsterBimap[i] = (HBITMAP)LoadImage(NULL, fileName,
			IMAGE_BITMAP, 80, 72, LR_LOADFROMFILE);
	}
}

cLogic::~cLogic()
{
}

void cLogic::KeyDown(WPARAM wParam)
{
	if (m_bIsKeyDown)
	{
		return;
	}
	m_bIsKeyDown = true;
	switch (wParam)
	{
	case VK_UP:
		m_curDir = Dir_Up;
		break;
	case VK_DOWN:
		m_curDir = Dir_Down;
		break;
	case VK_LEFT:
		m_curDir = Dir_Left;
		break;
	case VK_RIGHT:
		m_curDir = Dir_Right;
		break;
	default:
		break;
	}
}

void cLogic::KeyUp(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		m_bIsKeyDown = false;
		m_curDir = Dir_Num;
		break;
	}
}

void cLogic::Begin(HWND hWnd)
{
	m_hWnd = hWnd;
	m_playerPos.x = (480 - 129) / 2;
	m_playerPos.y = (720 - 73) - 80;
	m_curHp = Player_Max_HP;
	m_curDir = Dir_Num;
	m_bIsKeyDown = false;

	AddTimer(Timer_Draw, 50);	//添加一个绘制的定时器

	AddTimer(Timer_Move, 50);	//移动的定时器，包括人物移动，炮火移动，怪移动

	AddTimer(TImer_Fire, 300);	//开火定时器

	AddTimer(Timer_CreateMonster, 3000);	//创建怪物定时器
}

int cLogic::OnTimer(int id, int iParam, string str)
{
	switch (id)
	{
	case Timer_Draw:
		DrawAll();
		break;
	case Timer_Move:
		Move();
		AttackCheck();
		break;
	case TImer_Fire:
		Fire();
		break;
	case Timer_CreateMonster:
		RandCreateMonster();
		break;
	default:
		break;
	}
	return 1;
}

void cLogic::DrawAll()
{
	HDC hDc = GetWindowDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}
	HDC dcMem = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc, 480, 720);
	SelectObject(dcMem, hBitmap);
	//把所有内容绘制到缓存DC上
	DrawMap(dcMem);
	DrawPlayer(dcMem);
	DrawFire(dcMem);
	DrawMonster(dcMem);

	DrawHpPrograss(dcMem);

	//把缓存 DC的图绘制到设备DC    hDc
	BitBlt(hDc, 3, 20, 480, 720, dcMem, 0, 0, SRCCOPY);
	ReleaseDC(m_hWnd,hDc);
	DeleteObject(dcMem);
	DeleteObject(hBitmap);
}

void cLogic::DrawMap(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, _T("images\\map.bmp"),
		IMAGE_BITMAP, 480, 720, LR_LOADFROMFILE);
	SelectObject(dcTemp, bitmap);
	BitBlt(hDc, 0, 0, 480, 720, dcTemp, 0, 0, SRCCOPY);
	DeleteObject(dcTemp);
	DeleteObject(bitmap);
}

void cLogic::DrawPlayer(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, _T("images\\hero.bmp"),
		IMAGE_BITMAP, 129, 73, LR_LOADFROMFILE);
	SelectObject(dcTemp, bitmap);
	//BitBlt(hDc, m_playerPos.x,m_playerPos.y, 129, 73, dcTemp, 0, 0, SRCCOPY);
	TransparentBlt(hDc, m_playerPos.x, m_playerPos.y, 129, 73, dcTemp, 0, 0, 129, 73,
		RGB(237,28,36));
	DeleteObject(dcTemp);
	DeleteObject(bitmap);
}

void cLogic::DrawHpPrograss(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmapFrame = (HBITMAP)LoadImage(NULL, _T("images\\hpFrame.bmp"),
		IMAGE_BITMAP, 315, 80, LR_LOADFROMFILE);
	HBITMAP bitmapFill = (HBITMAP)LoadImage(NULL, _T("images\\hpFill.bmp"),
		IMAGE_BITMAP, 217, 18, LR_LOADFROMFILE);
	SelectObject(dcTemp, bitmapFrame);
	TransparentBlt(hDc,(480-315)/2,720-80, 315, 80, dcTemp, 0, 0, 315, 80,
		RGB(237, 28, 36));
	SelectObject(dcTemp, bitmapFill);
	TransparentBlt(hDc, (480 - 315) / 2+48, 720 - 80+49, 
		217*((float)m_curHp/Player_Max_HP), 18, dcTemp,0, 0, 
		217 * ((float)m_curHp / Player_Max_HP), 18, RGB(237, 28, 36));
	DeleteObject(dcTemp);
	DeleteObject(bitmapFill);
	DeleteObject(bitmapFrame);
}

void cLogic::DrawFire(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, _T("images\\fire.bmp"),
		IMAGE_BITMAP, 46, 46, LR_LOADFROMFILE);
	SelectObject(dcTemp, bitmap);
	for (itListObject it = m_listFire.begin(); it != m_listFire.end();++it)
	{
		TransparentBlt(hDc, it->x, it->y, 46, 46, 
			dcTemp, 0, 0, 46, 46,RGB(237, 28, 36));
	}
	DeleteObject(dcTemp);
	DeleteObject(bitmap);
}

void cLogic::DrawMonster(HDC hDc)
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	for (itListObject it = m_listMonster.begin(); it != m_listMonster.end();
		++it)
	{
		SelectObject(dcTemp, m_hMonsterBimap[it->monsterType - 1]);
		TransparentBlt(hDc, it->x, it->y, 80, 72,
			dcTemp, 0, 0, 80, 72, RGB(237, 28, 36));
	}
	DeleteObject(dcTemp);
}

void cLogic::Move()
{
	PlayerMove();
	FireMove();
	MonsterMove();
}

void cLogic::FireMove()
{
	for (itListObject it = m_listFire.begin(); it != m_listFire.end(); )
	{
		it->y -= FireMoveSpeed;
		if (it->y <= -46)
		{
			it = m_listFire.erase(it);
			continue;
		}
		++it;
	}
}

void cLogic::PlayerMove()
{
	if (m_curDir == Dir_Num)
	{
		return;
	}
	switch (m_curDir)
	{
	case Dir_Up:
		m_playerPos.y -= PlayerMoveSpeed;
		if (m_playerPos.y <= 0)
		{
			m_playerPos.y = 0;
		}
		break;
	case Dir_Down:
		m_playerPos.y += PlayerMoveSpeed;
		if (m_playerPos.y >= 720-80-73)
		{
			m_playerPos.y = 720 - 80 - 73;
		}
		break;
	case Dir_Left:
		m_playerPos.x -= PlayerMoveSpeed;
		if (m_playerPos.x <= 0)
		{
			m_playerPos.x = 0;
		}
		break;
	case Dir_Right:
		m_playerPos.x += PlayerMoveSpeed;
		if (m_playerPos.x >= 480-129)
		{
			m_playerPos.x = 480 - 129;
		}
		break;
	case Dir_Num:
		break;
	default:
		break;
	}
}

void cLogic::MonsterMove()
{
	for (itListObject itMonster = m_listMonster.begin(); itMonster !=
		m_listMonster.end();)
	{
		itMonster->y += MonsterMoveSpeed;
		if (itMonster->y >= 720)
		{
			itMonster = m_listMonster.erase(itMonster);
			continue;
		}
		itMonster++;
	}
}

void cLogic::Fire()
{
	//创建一个炮火
	stObject fireObj;
	fireObj.y = m_playerPos.y - 46;
	fireObj.x = m_playerPos.x + (129 - 46) / 2;
	m_listFire.push_back(fireObj);
}

void cLogic::RandCreateMonster()
{
	//随机创建一种队形的怪，一共4种队形
	int randType = 0;
	srand(GetTickCount());
	randType = rand() % 4 + 1; //随机得到一个1-4的数
	CreateMonster(4);
}

void cLogic::CreateMonster(int type)
{
	srand(GetTickCount());
	int randValue = rand();
	switch (type)
	{
	case 1:
	{
			  int x = randValue % 400 + 1;
			  for (int i = 0; i < 4;i++)
			  {
				  stObject stMonster;
				  stMonster.attackValue = 10;
				  stMonster.curHp = 100;
				  stMonster.x = x;
				  stMonster.y = -72 - i * 72 - i * 25;
				  stMonster.monsterType = type;
				  m_listMonster.push_back(stMonster);
			  }
			  break;
	}
	case 2:
	{
			  int x = randValue % 300 + 1;
			  for (int i = 0; i < 4;++i)
			  {
				  stObject stMonster;
				  stMonster.attackValue = 12;
				  stMonster.curHp = 120;
				  stMonster.monsterType = type;
				  stMonster.y = -72 - i * 72 - i * 25;
				  stMonster.x = x + (i % 2) * 100;
				  m_listMonster.push_back(stMonster);
			  }
			  break;
	}
	case 3:
	{
			  int x = 40;
			  for (int i = 0; i < 4;++i)
			  {
				  stObject stMonster;
				  stMonster.attackValue = 14;
				  stMonster.curHp = 140;
				  stMonster.monsterType = type;
				  stMonster.y = -72 - i * 72 - i * 25;
				  stMonster.x = x + i * 100;
				  m_listMonster.push_back(stMonster);
			  }
			  break;
	}
	case 4:
	{
			  int x = 40;
			  for (int i = 0; i < 4; ++i)
			  {
				  stObject stMonster;
				  stMonster.attackValue = 16;
				  stMonster.curHp = 160;
				  stMonster.monsterType = type;
				  stMonster.y = -72;
				  stMonster.x = x + i * 100;
				  m_listMonster.push_back(stMonster);
			  }
			  break;
	}
	default:
		break;
	}
}

void cLogic::AttackCheck()
{
	//炮火和怪，怪才主角
	for (itListObject itFire = m_listFire.begin();
		itFire != m_listFire.end();)
	{
		bool bAttacked = false;
		for (itListObject itMonster = m_listMonster.begin();
			itMonster != m_listMonster.end();++itMonster)
		{
			if (itMonster->y - itFire->y >= -72+10)
			{
				if (itFire->x -itMonster->x >= -46 +10 &&
					itFire->x - itMonster->x <= 80-10)
				{
					//碰到
					bAttacked = true;
					itMonster->curHp -= FireAttackValue;
					if (itMonster->curHp <= 0)
					{
						m_listMonster.erase(itMonster);
						break;
					}
				}
			}
		}
		if (bAttacked)
		{
			itFire = m_listFire.erase(itFire);
			continue;
		}
		itFire++;
	}
}