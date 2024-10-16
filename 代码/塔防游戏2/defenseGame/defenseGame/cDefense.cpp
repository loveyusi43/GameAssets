#include "stdafx.h"
#include "cDefense.h"
#include "windowsx.h"

#pragma comment(lib,"msimg32.lib")

int g_ptDesArray[9][2] = 
{
	{328,169},
	{287,238},
	{179,269},
	{163,338},
	{208,418},
	{380,414},
	{482,440},
	{583,358},
	{730,352}
};

unsigned g_curIndexMax = 0;

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
	case Timer_MonsterMove:
		MonsterMove();
		break;
	case Timer_Attack:
		BuildingAttack();
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
	AddTimer(Timer_AddMonster,2000);	//������
	AddTimer(Timer_MonsterMove,50);
	AddTimer(Timer_Attack,300);
}

void cDefense::GameOver()
{
	DeleteTimer(Timer_Draw);
	DeleteTimer(Timer_AddMonster);
	DeleteTimer(Timer_MonsterMove);
	DeleteTimer(Timer_Attack);
}

void cDefense::DrawAll()
{
	HDC hDc = GetDC(m_hWnd);
	HDC dcMem = CreateCompatibleDC(hDc);
	HBITMAP hBitmep = CreateCompatibleBitmap(hDc,700,600);
	SelectObject(dcMem,hBitmep);
	//Ҫ�����е�ͼ�λ��Ƶ�dcMem��

	DrawMap(dcMem);
	DrawItem(dcMem);//�������ͼ�꣬������ȫ��������������
	DrawMonster(dcMem);

	wchar_t szText[100] = {0};
	wsprintf(szText,L"��ң�%d",m_iMoney);
	SetBkMode(dcMem,0);
	SetTextColor(dcMem,RGB(255,255,50));
	TextOut(dcMem,120,20,szText,lstrlen(szText));
	//��dcMem���Ƶ�hDc��
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
	BulletMove();
	//����ͼ��
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

	//���ƽ�����
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
	DrawBullet(dcMem);
	//��������ϵĽ�����
	if (m_mouseItemIndex >= 0 && m_mouseItemIndex< Max_Building_Num)
	{
		wchar_t szFileName[100];
		wsprintf(szFileName,L"images\\Fortress\\%d_16.bmp",m_mouseItemIndex+1);
		HBITMAP bitmapMouse = (HBITMAP)LoadImage(NULL,szFileName,
			IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		//�������λ��������
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

	//�жϵ��������
	int baseIndex = -1;
	if (IsInRect(ptCur,m_rectBuilding[0]))
	{
		//����˵�һ������ͼ��
		m_mouseItemIndex = 0;
	}
	else if (IsInRect(ptCur,m_rectBuilding[1]))
	{
		//����˵ڶ�������ͼ��
		m_mouseItemIndex = 1;
	}
	else if ((baseIndex = GetBaseIndex(ptCur)) != -1)
	{
		//��ĳ���ػ��ϵ���һ��
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
	//���ӽ�����
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
	//��Ҫ�ҵ��ػ�������

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
		i++;
	}

	//���ݵػ���������������������
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
	//����ĳ�ʼ����㶼һ����
	stMonsterInfo monsterInfo;
	monsterInfo.pt.x = 327;
	monsterInfo.pt.y = -40;

	monsterInfo.curDesPtIndex = 0;
	monsterInfo.curPicIndex = 1;
	srand(GetTickCount());
	if (rand()%100 < 20)
	{
		monsterInfo.type = Monster_Type1;
		monsterInfo.curHp = 500;
		monsterInfo.speed = 2;
	}
	else
	{
		monsterInfo.type = Monster_Type2;
		monsterInfo.curHp = 100;
		monsterInfo.speed = 3;
	}
	monsterInfo.id = g_curIndexMax++;
	m_monsterList.push_back(monsterInfo);
}

void cDefense::MonsterMove()
{
	//����ӵ�ǰ�㵽Ŀ����ƶ�
	for (MonsterList::iterator it = m_monsterList.begin();it != m_monsterList.end();
		it++)
	{
		tagPOINT ptDes;
		ptDes.x = g_ptDesArray[it->curDesPtIndex][0];
		ptDes.y = g_ptDesArray[it->curDesPtIndex][1];

		if (ptDes.x == it->pt.x)
		{
			if (it->pt.y > ptDes.y)
			{
				it->pt.y -= it->speed;
			}
			else
			{
				it->pt.y += it->speed;
			}
			if (Distance(ptDes,it->pt) <= 5)
			{
				it->curDesPtIndex++;
			}
			if (it->pt.x >= 690)
			{
				GameOver();
				return;
			}
			continue;
		}
		//lenY/lenX = abs(ptDes.y-it->pt.y)/abs(ptDes.x-it->pt.x)
		//lenY*lenY + lenX*lenX = speed*speed
		//lenY = lenX*temp
		float temp= (float)abs(ptDes.y-it->pt.y)/abs(ptDes.x-it->pt.x);
		float lenX = sqrt((float)it->speed*it->speed/(1+temp*temp));
		float lenY = lenX*temp;
		if (it->pt.y > ptDes.y)
		{
			it->pt.y -= lenY;
		}
		else
		{
			it->pt.y += lenY;
		}

		if (it->pt.x > ptDes.x)
		{
			it->pt.x -= lenX;
		}
		else
		{
			it->pt.x += lenX;
		}
		if (Distance(ptDes,it->pt) <= 5)
		{
			it->curDesPtIndex++;
		}
		if (it->pt.x >= 690)
		{
			GameOver();
			return;
		}
	}
}

float cDefense::Distance(tagPOINT pt1,tagPOINT pt2)
{
	return (float)sqrt((float)((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y)));
}

void cDefense::DrawMonster(HDC dcMem)
{
	//Ҫ���ݷ���������
	HDC dcTemp = CreateCompatibleDC(NULL);
	for (MonsterList::reverse_iterator it = m_monsterList.rbegin();it != m_monsterList.rend();it++)
	{
		//ȷ������
		wchar_t szFileName[100];
		if (it->curDesPtIndex <= 4)
		{
			//����
			wsprintf(szFileName,L"images\\monster%d\\down_%d.bmp",it->type+1,it->curPicIndex);
			it->curPicIndex++;
			if (it->type == Monster_Type1 && it->curPicIndex>12)
			{
				it->curPicIndex = 1;
			}
			else if (it->type == Monster_Type2 && it->curPicIndex>13)
			{
				it->curPicIndex = 1;
			}
		}
		else
		{
			//����
			wsprintf(szFileName,L"images\\monster%d\\right_%d.bmp",
				it->type+1,it->curPicIndex);
			it->curPicIndex++;
			if (it->type == Monster_Type1 && it->curPicIndex>16)
			{
				it->curPicIndex = 1;
			}
			else if (it->type == Monster_Type2 && it->curPicIndex>13)
			{
				it->curPicIndex = 1;
			}
		}
		HBITMAP hBitmap = (HBITMAP)LoadImage(NULL,szFileName,IMAGE_BITMAP,0,0,
			LR_LOADFROMFILE);
		BITMAP bitinfo;
		GetObject(hBitmap,sizeof(BITMAP),&bitinfo);
		SelectObject(dcTemp,hBitmap);
		TransparentBlt(dcMem,it->pt.x-bitinfo.bmWidth/2,it->pt.y-bitinfo.bmHeight/2,
			bitinfo.bmWidth,bitinfo.bmHeight,dcTemp,0,0,bitinfo.bmWidth,bitinfo.bmHeight,
			RGB(34,177,76));
		DeleteObject(hBitmap);
	}
	DeleteObject(dcTemp);
}

void cDefense::BuildingAttack()
{
	for (BuildList::iterator it = m_buildList.begin();it != m_buildList.end();it++)
	{
		stMonsterInfo stMonster;
		//�����̷�Χ���������Ĺֹ���
		if (GetNearestMonster(*it,stMonster))
		{
			//�����ӵ�
			stBulletInfo bulletInfo;
			bulletInfo.monsterId = stMonster.id;
			bulletInfo.type = it->type;
			if (it->type == Building_Type1)
			{
				bulletInfo.pt.x = it->pt.x +30;
				bulletInfo.pt.y = it->pt.y +10;
			}
			else
			{
				bulletInfo.pt.x = it->pt.x+50;
				bulletInfo.pt.y = it->pt.y+10;
			}
			bulletInfo.ptDes.x = stMonster.pt.x;
			bulletInfo.ptDes.y = stMonster.pt.y;
			m_bulletList.push_back(bulletInfo);
		}
	}
}

bool cDefense::GetNearestMonster(stBuildingInfo stBuild,stMonsterInfo& stMonster)
{
	float disMin = 100000.0;
	bool bFind = false;
	for (MonsterList::iterator it = m_monsterList.begin();it != m_monsterList.end();
		it++)
	{
		tagPOINT ptStart;
		if (stBuild.type == Building_Type1)
		{
			ptStart.x = stBuild.pt.x +48;
			ptStart.y = stBuild.pt.y +60;
		}
		else if (stBuild.type == Building_Type2)
		{
			ptStart.x = stBuild.pt.x + 63;
			ptStart.y = stBuild.pt.y + 85;
		}

		float dis = Distance(ptStart,it->pt);
		if (dis <= 200 && dis < disMin)
		{
			disMin = dis;
			stMonster = *it;
			bFind = true;
		}
	}
	
	return bFind;
}

void cDefense::BulletMove()
{
	for (BulletList::iterator it = m_bulletList.begin();it != m_bulletList.end();
		)
	{
		tagPOINT ptDes;
		ptDes.x = it->ptDes.x;
		ptDes.y = it->ptDes.y;
		int bulletSpeed = 20;
		if (ptDes.x == it->pt.x)
		{
			if (it->pt.y > ptDes.y)
			{
				it->pt.y -= bulletSpeed;
			}
			else
			{
				it->pt.y += bulletSpeed;
			}
			if (Distance(ptDes,it->pt) <= 12)
			{
				//��������
				AttackMonster(it->monsterId,it->type);
				it = m_bulletList.erase(it);
				continue;
			}
			it++;
			continue;
		}
		//lenY/lenX = abs(ptDes.y-it->pt.y)/abs(ptDes.x-it->pt.x)
		//lenY*lenY + lenX*lenX = speed*speed
		//lenY = lenX*temp
		float temp= (float)abs(ptDes.y-it->pt.y)/abs(ptDes.x-it->pt.x);
		float lenX = sqrt((float)bulletSpeed*bulletSpeed/(1+temp*temp));
		float lenY = lenX*temp;
		if (it->pt.y > ptDes.y)
		{
			it->pt.y -= lenY;
		}
		else
		{
			it->pt.y += lenY;
		}

		if (it->pt.x > ptDes.x)
		{
			it->pt.x -= lenX;
		}
		else
		{
			it->pt.x += lenX;
		}
		if (Distance(ptDes,it->pt) <= 12)
		{
			//��������
			AttackMonster(it->monsterId,it->type);
			it = m_bulletList.erase(it);
			continue;
		}
		it++;
	}
}

void cDefense::AttackMonster(int id,BuildingType type)
{
	for (MonsterList::iterator it = m_monsterList.begin();it != m_monsterList.end();
		it++)
	{
		if (it->id == id)
		{
			it->curHp -= type == Building_Type1?10:20;
			if (it->curHp<= 0)
			{
				m_iMoney += it->type== Monster_Type1?5:10;
				m_monsterList.erase(it);
			}
			break;
		}
	}
}

void cDefense::DrawBullet(HDC dcMem)
{
	HDC dcTemp = CreateCompatibleDC(NULL);
	HBITMAP hBitmap1 = (HBITMAP)LoadImage(NULL,L"images\\bullet1.bmp",IMAGE_BITMAP,0,
		0,LR_LOADFROMFILE);
	HBITMAP hBitmap2 = (HBITMAP)LoadImage(NULL,L"images\\bullet2.bmp",IMAGE_BITMAP,0,
		0,LR_LOADFROMFILE);

	BITMAP bitInfo;
	GetObject(hBitmap1,sizeof(BITMAP),&bitInfo);
	for (BulletList::iterator it = m_bulletList.begin();it != m_bulletList.end();
		it++)
	{
		if (it->type == Building_Type1)
		{
			SelectObject(dcTemp,hBitmap1);
		}
		else
		{
			SelectObject(dcTemp,hBitmap2);
		}
		TransparentBlt(dcMem,it->pt.x,it->pt.y,bitInfo.bmWidth,bitInfo.bmHeight,
			dcTemp,0,0,bitInfo.bmWidth,bitInfo.bmHeight,RGB(34,177,76));
	}
	DeleteObject(dcTemp);
	DeleteObject(hBitmap1);
	DeleteObject(hBitmap2);
}