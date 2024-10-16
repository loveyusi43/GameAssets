#pragma once
#include "cMyTimer.h"

#define LatticeLen 100	//���ӵı߳�
#define Map_MoveStep 20	//ÿ�ε�ͼ�ƶ�����

class cMapEdit:public cMyTimer
{
public:
	cMapEdit(HWND hWnd = NULL);
	~cMapEdit(void);

	virtual int OnTimer(int id,int iParam,string str);
	
	void Draw();
	void LoadMap();	//���ص�ͼ��ԴͼƬ
	void DrawObstruct(HDC dcMem);	//�����赲
	void MoveMap(WPARAM wParam);	//�ƶ���ͼλ��
	void SetObstruct(LPARAM lParam);	//����һ���赲��
	void CancelObstruct(LPARAM lParam);	//ȡ��һ���赲��

	void SaveFile();	//�����ļ�
	void OpenFile();	//���ļ�
	void SaveFileToPath(wchar_t* filePath);	//����
	void LoadFileFromPath(wchar_t* filePath);	//����

	int GetRow(tagPOINT pt);
	int GetRank(tagPOINT pt);

	list<tagPOINT> m_listObstruct;	//�赲����б�
	int m_mapRow;	//��
	int m_mapRank;	//��

private:
	HWND m_hWnd;
	wchar_t m_mapPath[MAX_PATH];

	HBITMAP m_mapBitmap;	//��������

	tagPOINT m_ptClientToMap;	//���ڿͻ�������ڵ�ͼ������

	

	
};

