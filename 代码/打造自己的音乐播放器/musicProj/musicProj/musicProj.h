
// musicProj.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmusicProjApp:
// �йش����ʵ�֣������ musicProj.cpp
//

class CmusicProjApp : public CWinApp
{
public:
	CmusicProjApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CmusicProjApp theApp;