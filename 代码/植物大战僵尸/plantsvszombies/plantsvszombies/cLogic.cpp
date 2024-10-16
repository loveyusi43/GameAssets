#include "stdafx.h"
#include "cLogic.h"
#pragma comment(lib,"Msimg32.lib")

void cLogic::Begin(HWND hWnd)	//游戏开始
{
	m_hWnd = hWnd;
	m_iMoney = 50;	//初始化金币
	m_MouseType = Type_Num;
	stObjectInfo stFlower;
	stFlower.x = 90;
	stFlower.y = 160;
	stFlower.lastActionTime = GetTickCount();
	stFlower.speed = GetFruitTime;
	stFlower.type = Type_Flower;
	m_listFlower.push_back(stFlower);

	stObjectInfo stGun;
	stGun.x = 90+112;
	stGun.y = 160+160;
	stGun.lastActionTime = GetTickCount();
	stGun.speed = ShootTime;
	stGun.type = Type_Gun;
	m_listGun.push_back(stGun);

	AddTimer(Timer_Draw,100);	//绘制定时器
	AddTimer(Timer_Shoot,1000);	//发射定时器
	AddTimer(Timer_Move,30);	//控制移动
	AddTimer(Timer_CreateZombie,5000);	//创建僵尸
	AddTimer(Timer_CheckAddMoney,1000);	//检测加钱
}

void cLogic::GameEnd()
{

}

int cLogic::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_Draw:
		Draw();
		break;
	case Timer_Shoot:
		Shoot();
		break;
	case Timer_Move:
		Move();
		CheckAttack();
		break;
	case Timer_CreateZombie:
		RandCreateZombie();
		break;
	case Timer_CheckAddMoney:
		CheckGetMoney();
		break;
	}
	return 1;
}

void cLogic::Draw()
{
	HDC hDc = GetWindowDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}

	HDC dcMem = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,1280,720);
	SelectObject(dcMem,hBitmap);
	//绘制到dcMem上
	DrawBackPlants(dcMem);
	DrawFlowerAndGun(dcMem);
	DrawBullet(dcMem);
	DrawZombies(dcMem);
	DrawText(dcMem);
	DrawMouseItem(dcMem);

	//绘制到设备DC上
	BitBlt(hDc,0,0,1280,720,dcMem,0,0,SRCCOPY);
	ReleaseDC(m_hWnd,hDc);
}

void cLogic::DrawBackPlants(HDC hDc)	//画背景
{
	HDC dcTemp;
	dcTemp = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL,_T("map.bmp"),IMAGE_BITMAP,1280,720,
		LR_LOADFROMFILE);
	SelectObject(dcTemp,bitmap);
	BitBlt(hDc,0,0,1280,720,dcTemp,0,0,SRCCOPY);
	DeleteObject(dcTemp);
	DeleteObject(bitmap);
}

void  cLogic::DrawFlowerAndGun(HDC hDc)	//画枪和花
{
	HDC dcMem = CreateCompatibleDC(hDc);
	HBITMAP bitmap1 = (HBITMAP)LoadImage(NULL,_T("plants1.bmp"),IMAGE_BITMAP,83,103,LR_LOADFROMFILE);
	SelectObject(dcMem,bitmap1);//枪

	HDC dcMem2 = CreateCompatibleDC(hDc);
	HBITMAP bitmap2 = (HBITMAP)LoadImage(NULL,_T("plants2.bmp"),IMAGE_BITMAP,83,103,LR_LOADFROMFILE);
	SelectObject(dcMem2,bitmap2);//花

	//先绘制上面的花和枪
	TransparentBlt(hDc,300,20,83,103,dcMem,0,0,83,103,RGB(223,38,219));
	TransparentBlt(hDc,400,20,83,103,dcMem2,0,0,83,103,RGB(223,38,219));

	for (itObjectList it = m_listFlower.begin();it != m_listFlower.end();++it)
	{
		int x = it->x;
		int y = it->y;
		TransparentBlt(hDc,x,y,83,103,dcMem2,0,0,83,103,RGB(223,38,219));
	}
	for (itObjectList it = m_listGun.begin();it != m_listGun.end();++it)
	{
		int x = it->x;
		int y = it->y;
		TransparentBlt(hDc,x,y,83,103,dcMem,0,0,83,103,RGB(223,38,219));
	}

	DeleteObject(dcMem);
	DeleteObject(dcMem2);
	DeleteObject(bitmap1);
	DeleteObject(bitmap2);
}

void  cLogic::DrawBullet(HDC hDc)
{//画子弹
	HDC dcMem = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL,_T("bullet.bmp"),
		IMAGE_BITMAP,20,18,LR_LOADFROMFILE);
	SelectObject(dcMem,bitmap);
	for (itObjectList it = m_listBullet.begin();
		it != m_listBullet.end();++it)
	{
		int x = it->x;
		int y = it->y;
		TransparentBlt(hDc,x,y,20,18,dcMem,0,0,20,18,RGB(223,38,219));
	}
	DeleteObject(dcMem);
	DeleteObject(bitmap);
}
void  cLogic::DrawZombies(HDC hDc)		//画僵尸
{
	HDC dcMem = CreateCompatibleDC(hDc);
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL,_T("zombie.bmp"),
		IMAGE_BITMAP,308,596,LR_LOADFROMFILE);
	SelectObject(dcMem,bitmap);

	for (itObjectList it = m_listZombies.begin();
		it != m_listZombies.end();++it)
	{
		int x = it->x;
		int y = it->y;
		int index = it->curPicIndex;
		int srcX = index%4*77;	//x坐标
		int srcY = index/4*149;	//y坐标
		TransparentBlt(hDc,x,y,77,149,dcMem,srcX,srcY,77,149,RGB(223,38,219));
	}

	DeleteObject(dcMem);
	DeleteObject(bitmap);
}
void  cLogic::DrawText(HDC hDc)			//画文字
{
	SetBkMode(hDc,0);
	SetTextColor(hDc,RGB(204,217,45));
	wchar_t strText[100] = {0};
	wsprintf(strText,_T("金币：%d"),m_iMoney);
	TextOut(hDc,500,50,strText,lstrlen(strText));
}

void cLogic::DrawMouseItem(HDC hDc)
{
	if (m_MouseType == Type_Num)
	{
		return;
	}
	HDC dcMem = CreateCompatibleDC(hDc);
	HBITMAP bitmap1 = (HBITMAP)LoadImage(NULL,_T("plants1.bmp"),IMAGE_BITMAP,83,103,LR_LOADFROMFILE);
	SelectObject(dcMem,bitmap1);//枪

	HDC dcMem2 = CreateCompatibleDC(hDc);
	HBITMAP bitmap2 = (HBITMAP)LoadImage(NULL,_T("plants2.bmp"),IMAGE_BITMAP,83,103,LR_LOADFROMFILE);
	SelectObject(dcMem2,bitmap2);//花

	if (m_MouseType == Type_Flower)
	{
		tagPOINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_hWnd,&pt);
		pt.x -= 41;
		pt.y -= 51;
		TransparentBlt(hDc,pt.x,pt.y,83,103,dcMem2,0,0,83,103,RGB(223,38,219));
	}
	else if (m_MouseType == Type_Gun)
	{
		tagPOINT pt;
		GetCursorPos(&pt);
		ScreenToClient(m_hWnd,&pt);
		pt.x -= 41;
		pt.y -= 51;
		TransparentBlt(hDc,pt.x,pt.y,83,103,dcMem,0,0,83,103,RGB(223,38,219));
	}
}

void cLogic::Shoot()
{
	unsigned timeNow = GetTickCount();
	for (itObjectList it = m_listGun.begin(); it != m_listGun.end();++it)
	{
		if (timeNow - it->lastActionTime >= it->speed)
		{
			//发射子弹
			stObjectInfo stBullet;
			stBullet.speed = 6;
			stBullet.type = Type_Bullet;
			stBullet.x = it->x + 65;
			stBullet.y = it->y + 17;
			stBullet.attackValue = 20;
			m_listBullet.push_back(stBullet);
			//重置上次发射时间
			it->lastActionTime = timeNow;
		}
	}
}

void cLogic::Move()
{
	for (itObjectList it = m_listBullet.begin(); it != m_listBullet.end();)
	{
		it->x += it->speed;
		if (it->x >= 1280 + 10)
		{
			it = m_listBullet.erase(it);
			continue;
		}
		++it;
	}

	for (itObjectList it = m_listZombies.begin();it != m_listZombies.end();
		++it)
	{
		it->x -= it->speed;
		it->curPicIndex = (it->curPicIndex+1)%14;
		if (it->x <= 0)
		{
			GameEnd();
		}
	}
}

void cLogic::RandCreateZombie()
{
	srand(GetTickCount());
	int index = rand()%3;
	stObjectInfo stZombie;
	stZombie.curPicIndex = 0;
	stZombie.hpValue = 100;
	stZombie.speed = 1;
	stZombie.type = Type_Zombie;
	stZombie.x = 1280;
	stZombie.y = 100+index*160;
	m_listZombies.push_back(stZombie);
}

void cLogic::CheckAttack()
{
	for (itObjectList itBullet = m_listBullet.begin();itBullet != m_listBullet.end();)
	{
		int xBullet = itBullet->x;
		int yBullet = itBullet->y;
		bool bAttack = false;
		for (itObjectList itZombie = m_listZombies.begin();itZombie != m_listZombies.end();
			++itZombie)
		{
			int xZombie = itZombie->x;
			int yZombie = itZombie->y;
			if (xBullet - xZombie >= 10 && yBullet-yZombie > 0 && yBullet-yZombie <= 100)
			{
				//攻击到了
				itZombie->hpValue -= itBullet->attackValue;
				if (itZombie->hpValue <= 0)
				{
					m_listZombies.erase(itZombie);
				}
				itBullet = m_listBullet.erase(itBullet);
				bAttack = true;
				break;
			}
		}
		if (!bAttack)
		{
			++itBullet;
		}
	}
}

void cLogic::CheckGetMoney()
{
	unsigned timeNow = GetTickCount();
	for (itObjectList it = m_listFlower.begin();it != m_listFlower.end();++it)
	{
		if (timeNow - it->lastActionTime >= GetFruitTime)
		{
			m_iMoney += 50;
			it->lastActionTime = timeNow;
		}
	}
}

void cLogic::LButtonUp()	//左键点击
{
	if (m_iMoney < 50)
	{
		return;
	}
	ObjectType type = GetInRect();
	if (m_MouseType == Type_Num)
	{
		m_MouseType = type;
	}
	else if (m_MouseType == Type_Flower)
	{
		int x,y;
		if (CanPut(x,y))
		{
			//在该位置创建一个花
			stObjectInfo stFlower;
			stFlower.x = x;
			stFlower.y = y;
			stFlower.lastActionTime = GetTickCount();
			stFlower.speed = GetFruitTime;
			stFlower.type = Type_Flower;
			m_listFlower.push_back(stFlower);

			
		}
	}
	else if (m_MouseType == Type_Gun)
	{
		int x,y;
		if (CanPut(x,y))
		{
			//在该位置创建一个枪
			stObjectInfo stGun;
			stGun.x = x;
			stGun.y = y;
			stGun.lastActionTime = GetTickCount();
			stGun.speed = ShootTime;
			stGun.type = Type_Gun;
			m_listGun.push_back(stGun);
		}
	}
}

void cLogic::RButtonUp()	//右键点击
{
	m_MouseType = Type_Num;
}

ObjectType cLogic::GetInRect()
{
	tagPOINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	if (pt.x >= 320 && pt.x <= 320+83-10 && pt.y >= 30 && pt.y <= 80)
	{
		return Type_Gun;
	}
	if (pt.x >= 420 && pt.x <= 420+83-10 && pt.y >= 30 && pt.y <= 80)
	{
		return Type_Flower;
	}
	return Type_Num;
}

bool cLogic::CanPut(int& x,int& y)
{
	tagPOINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	pt.x -= 90;
	pt.y -= -160;
	if (pt.x < 0 || pt.y < 0 || pt.x > 112*10 || pt.y >= 160*3)
	{
		return false;
	}

	int row = pt.y/160;	//行
	int rank = pt.x/112;	//列

	x = rank*112+90;
	y = row*160 + 160;

	for (itObjectList it = m_listFlower.begin();it != m_listFlower.end();++it)
	{
		if (it->x == x && it->y == y)
		{
			return false;
		}
	}
	for (itObjectList it = m_listGun.begin();it != m_listGun.end();++it)
	{
		if (it->x == x && it->y == y)
		{
			return false;
		}
	}
	return true;
}