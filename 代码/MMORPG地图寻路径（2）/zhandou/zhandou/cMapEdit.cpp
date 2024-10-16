#include "StdAfx.h"
#include "cMapEdit.h"
#include "commdlg.h"
#include "windowsx.h"

cMapEdit::cMapEdit(HWND hWnd)
{
	m_hWnd = hWnd;
	m_mapBitmap = NULL;
	ZeroMemory(m_mapPath,MAX_PATH);
	m_ptClientToMap.x = 0;
	m_ptClientToMap.y = 0;
	if (m_hWnd != NULL)
	{
		AddTimer(1,50);
	}
}


cMapEdit::~cMapEdit(void)
{
}

int cMapEdit::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case 1:
		Draw();
		break;
	}
	return 1;
}

void cMapEdit::LoadMap()
{
	OPENFILENAME stfile;
	ZeroMemory(&stfile,sizeof(OPENFILENAME));
	wchar_t filePath[MAX_PATH] = {0};
	stfile.lpstrFile = filePath;
	stfile.nMaxFile = MAX_PATH;
	stfile.lpstrFilter = L"*.bmp";
	stfile.lpstrDefExt = L"bmp";
	stfile.lStructSize = sizeof(OPENFILENAME);
	stfile.hwndOwner = m_hWnd;

	if (GetOpenFileName(&stfile))
	{
		lstrcpy(m_mapPath,stfile.lpstrFile);
		m_mapBitmap = (HBITMAP)LoadImage(NULL,m_mapPath,
			IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if (m_mapBitmap == NULL)
		{
			MessageBox(m_hWnd,L"���ص�ͼ��Դʧ��",L"����",MB_OK);
			return;
		}
	}
}

void cMapEdit::Draw()
{
	HDC hDc = GetDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}
	//����DC
	HDC dcMem = CreateCompatibleDC(hDc);
	RECT rect;
	GetClientRect(m_hWnd,&rect);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDc,
		rect.right-rect.left,rect.bottom-rect.top);
	SelectObject(dcMem,hBitmap);

	HDC dcTemp = CreateCompatibleDC(NULL);
	SelectObject(dcTemp,m_mapBitmap);
	BITMAP bitInfo;
	GetObject(m_mapBitmap,sizeof(BITMAP),&bitInfo);

	//���Ƶ�ͼ
	BitBlt(dcMem,-m_ptClientToMap.x,-m_ptClientToMap.y,
		bitInfo.bmWidth,bitInfo.bmHeight,dcTemp,0,0,SRCCOPY);

	//�����赲
	DrawObstruct(dcMem);

	//����һϵ�е������ѵ�ͼ���ֳ�һ��һ����С����
	//���ƺ��ߺ�����
	tagPOINT ptStart;
	tagPOINT ptEnd;
	ptStart.x = -m_ptClientToMap.x;
	ptStart.y = -m_ptClientToMap.y;

	int indexRow = 1;
	ptEnd.x = -m_ptClientToMap.x + bitInfo.bmWidth;

	HPEN pen = CreatePen(PS_SOLID,1,RGB(255,0,0));
	SelectObject(dcMem,pen);
	//���ƺ���
	while(true)
	{
		ptStart.y = -m_ptClientToMap.y +indexRow*LatticeLen;
		if (indexRow*LatticeLen >= bitInfo.bmHeight)
		{
			break;
		}
		ptEnd.y = ptStart.y;

		tagPOINT pt;
		MoveToEx(dcMem,ptStart.x,ptStart.y,&pt);
		LineTo(dcMem,ptEnd.x,ptEnd.y);

		indexRow++;
	}
	m_mapRow = indexRow;

	//������
	int indexRank = 1;

	ptStart.y = -m_ptClientToMap.y;
	ptEnd.y = -m_ptClientToMap.y + bitInfo.bmHeight;
	while(true)
	{
		ptStart.x = -m_ptClientToMap.x+indexRank*LatticeLen;
		if (indexRank*LatticeLen >= bitInfo.bmWidth)
		{
			break;
		}
		ptEnd.x = ptStart.x;

		tagPOINT pt;
		MoveToEx(dcMem,ptStart.x,ptStart.y,&pt);
		LineTo(dcMem,ptEnd.x,ptEnd.y);
		indexRank++;
	}
	m_mapRank = indexRank;
	//��dcMem�ϵ����ݻ��Ƶ��豸DC��
	BitBlt(hDc,0,0,rect.right-rect.left,rect.bottom-rect.top,
		dcMem,0,0,SRCCOPY);

	DeleteObject(dcMem);
	DeleteObject(dcTemp);
	DeleteObject(hBitmap);
	ReleaseDC(m_hWnd,hDc);
	DeleteObject(pen);
}

void cMapEdit::MoveMap(WPARAM wParam)
{
	if (!m_mapBitmap)
	{
		return;
	}

	BITMAP bitInfo;
	GetObject(m_mapBitmap,sizeof(BITMAP),&bitInfo);


	RECT rect;
	GetClientRect(m_hWnd,&rect);
	int clientWidth = rect.right-rect.left;
	int clientHeight = rect.bottom - rect.top;

	switch(wParam)
	{
	case VK_UP:
		m_ptClientToMap.y -= Map_MoveStep;
		if (m_ptClientToMap.y < 0)
		{
			m_ptClientToMap.y = 0;
		}
		break;
	case VK_DOWN:
		m_ptClientToMap.y += Map_MoveStep;
		if (m_ptClientToMap.y > bitInfo.bmHeight-clientHeight)
		{
			m_ptClientToMap.y = bitInfo.bmHeight-clientHeight;
		}
		break;
	case VK_LEFT:
		m_ptClientToMap.x -= Map_MoveStep;
		if (m_ptClientToMap.x < 0)
		{
			m_ptClientToMap.x = 0;
		}
		break;
	case VK_RIGHT:
		m_ptClientToMap.x += Map_MoveStep;
		if (m_ptClientToMap.x > bitInfo.bmWidth-clientWidth)
		{
			m_ptClientToMap.x = bitInfo.bmWidth-clientWidth;
		}
		break;
	}
}

void cMapEdit::SetObstruct(LPARAM lParam)	//����һ���赲��
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	//ͨ�������жϳ������ĸ���������
	tagPOINT pt;//���������ڴ��ͼ������
	pt.x = m_ptClientToMap.x+x;
	pt.y = m_ptClientToMap.y+y;

	//���ӵ���������ֵ
	int indexRow = pt.y/LatticeLen;
	int indexRank = pt.x/LatticeLen;

	//�������Ͻǵ�����
	tagPOINT ptDes;
	ptDes.x = indexRank*LatticeLen;
	ptDes.y = indexRow*LatticeLen;
	m_listObstruct.push_back(ptDes);

}
void cMapEdit::CancelObstruct(LPARAM lParam)	//ȡ��һ���赲��
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	//ͨ�������жϳ������ĸ���������
	tagPOINT pt;//���������ڴ��ͼ������
	pt.x = m_ptClientToMap.x+x;
	pt.y = m_ptClientToMap.y+y;

	//���ӵ���������ֵ
	int indexRow = pt.y/LatticeLen;
	int indexRank = pt.x/LatticeLen;

	//�������Ͻǵ�����
	tagPOINT ptDes;
	ptDes.x = indexRank*LatticeLen;
	ptDes.y = indexRow*LatticeLen;

	for (list<tagPOINT>::iterator it = m_listObstruct.begin();
		it != m_listObstruct.end();it++)
	{
		if (it->x == ptDes.x && it->y == ptDes.y)
		{
			m_listObstruct.erase(it);
			break;
		}
	}
}

void cMapEdit::DrawObstruct(HDC dcMem)
{
	RECT rect;
	HBRUSH brush = CreateSolidBrush(RGB(255,255,255));
	HBRUSH brushOld = (HBRUSH)SelectObject(dcMem,brush);
	for (list<tagPOINT>::iterator it = m_listObstruct.begin();
		it != m_listObstruct.end();it++)
	{
		rect.left = it->x - m_ptClientToMap.x;
		rect.right = rect.left+LatticeLen;
		rect.top = it->y - m_ptClientToMap.y;
		rect.bottom = rect.top+LatticeLen;
		FillRect(dcMem,&rect,brush);
	}

	SelectObject(dcMem,brush);
	DeleteObject(brush);
}

void cMapEdit::SaveFile()	//�����ļ�
{
	OPENFILENAME stfile;
	ZeroMemory(&stfile,sizeof(OPENFILENAME));
	wchar_t filePath[MAX_PATH] = {0};
	stfile.lpstrFile = filePath;
	stfile.nMaxFile = MAX_PATH;
	stfile.lpstrFilter = L"*.Map";
	stfile.lpstrDefExt = L"Map";
	stfile.lStructSize = sizeof(OPENFILENAME);
	stfile.hwndOwner = m_hWnd;

	if (GetSaveFileName(&stfile))
	{
		SaveFileToPath(stfile.lpstrFile);
	}
}

void cMapEdit::SaveFileToPath(wchar_t* filePath)
{
	HANDLE hCreateFile = CreateFile(filePath,GENERIC_WRITE,FILE_SHARE_READ,NULL,
		CREATE_ALWAYS/*CREATE_NEW 	| OPEN_EXISTING*/,NULL,NULL);
	int err = GetLastError();
	if (hCreateFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,L"�����ļ�ʧ��",L"����",MB_OK);
		return;
	}
	//д�ļ�
	//��ͼ·����·������ ·������  
	//�赲����Ϣ���赲�����   ÿ���赲��
	//��ͼ��������Ϣ

	int pathLen = lstrlen(m_mapPath)*2;
	DWORD size = 0;
	WriteFile(hCreateFile,&pathLen,sizeof(int),&size,NULL);
	WriteFile(hCreateFile,m_mapPath,pathLen,&size,NULL);

	//д�赲��
	int obstructNum = m_listObstruct.size();
	WriteFile(hCreateFile,&obstructNum,sizeof(int),&size,NULL);
	for (list<tagPOINT>::iterator it = m_listObstruct.begin();
		it != m_listObstruct.end();it++)
	{
		WriteFile(hCreateFile,&(*it),sizeof(tagPOINT),&size,NULL);
	}

	//��д�С�����
	WriteFile(hCreateFile,&m_mapRow,sizeof(int),&size,NULL);
	WriteFile(hCreateFile,&m_mapRank,sizeof(int),&size,NULL);

	CloseHandle(hCreateFile);

}

void cMapEdit::OpenFile()	//���ļ�
{
	OPENFILENAME stfile;
	ZeroMemory(&stfile,sizeof(OPENFILENAME));
	wchar_t filePath[MAX_PATH] = {0};
	stfile.lpstrFile = filePath;
	stfile.nMaxFile = MAX_PATH;
	stfile.lpstrFilter = L"*.Map";
	stfile.lpstrDefExt = L"Map";
	stfile.lStructSize = sizeof(OPENFILENAME);
	stfile.hwndOwner = m_hWnd;

	if (GetOpenFileName(&stfile))
	{
		LoadFileFromPath(stfile.lpstrFile);
	}
}

void cMapEdit::LoadFileFromPath(wchar_t* filePath)
{
	m_listObstruct.clear();
	HANDLE hOpenFile = CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,NULL,NULL);
	if (hOpenFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,L"�����ļ�ʧ��",L"����",MB_OK);
		return;
	}

	int lenPath = 0;
	DWORD size = 0;
	ReadFile(hOpenFile,&lenPath,sizeof(int),&size,NULL);
	ReadFile(hOpenFile,m_mapPath,lenPath,&size,NULL);

	int obstructLen = 0;
	ReadFile(hOpenFile,&obstructLen,sizeof(int),&size,NULL);
	for (int i = 0;i<obstructLen;i++)
	{
		tagPOINT pt;
		ReadFile(hOpenFile,&pt,sizeof(tagPOINT),&size,NULL);
		m_listObstruct.push_back(pt);
	}

	ReadFile(hOpenFile,&m_mapRow,sizeof(int),&size,NULL);
	ReadFile(hOpenFile,&m_mapRank,sizeof(int),&size,NULL);

	//�����������ݼ��ص�����
	if(m_mapBitmap != NULL)
	{
		DeleteObject(m_mapBitmap);
	}
	m_mapBitmap = (HBITMAP)LoadImage(NULL,m_mapPath,
		IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	m_ptClientToMap.x = 0;
	m_ptClientToMap.y = 0;
	CloseHandle(hOpenFile);
}

int cMapEdit::GetRow(tagPOINT pt)
{
	//���ӵ���������ֵ
	return pt.y/LatticeLen;
}

int cMapEdit::GetRank(tagPOINT pt)
{
	return pt.x/LatticeLen;
}