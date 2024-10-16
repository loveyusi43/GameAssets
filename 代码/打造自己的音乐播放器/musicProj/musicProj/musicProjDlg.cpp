
// musicProjDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "musicProj.h"
#include "musicProjDlg.h"
#include "afxdialogex.h"
#include "mmsystem.h"

#pragma comment(lib,"winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmusicProjDlg 对话框




CmusicProjDlg::CmusicProjDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CmusicProjDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_curDeviceID = 0;
	m_curPlayIndex= -1;
}

void CmusicProjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listMusic);
	DDX_Control(pDX, IDC_SLIDER1, m_progress);
}

BEGIN_MESSAGE_MAP(CmusicProjDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_AddMusic, &CmusicProjDlg::OnBnClickedAddmusic)
	ON_LBN_DBLCLK(IDC_LIST1, &CmusicProjDlg::OnLbnDblclkList1)
	ON_BN_CLICKED(IDC_Play, &CmusicProjDlg::OnBnClickedPlay)
	ON_BN_CLICKED(IDC_Pause, &CmusicProjDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDC_Stop, &CmusicProjDlg::OnBnClickedStop)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CmusicProjDlg 消息处理程序

BOOL CmusicProjDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);
	SetTimer(1,100,NULL);
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CmusicProjDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CmusicProjDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CmusicProjDlg::OnBnClickedAddmusic()
{
	CFileDialog fileDlg(TRUE);
	CString filePath;
	CString fileName;
	if(fileDlg.DoModal() == IDOK)
	{
		fileName = fileDlg.GetFileName();
		filePath = fileDlg.GetPathName();

		if (m_curDeviceID != 0)
		{
			CString strCurPlay;
			m_listMusic.GetText(m_curPlayIndex,strCurPlay);
			if (strCurPlay.Compare(fileName) > 0)
			{
				m_curPlayIndex++;
			}
		}

		m_listMusic.AddString(fileName);
		m_pathLis.push_back(filePath);

		
		//添加进入的假如是第一首歌，添加进来后自动播放
		if (m_listMusic.GetCount() == 1)
		{
			PlayMusic(filePath,fileName);
			m_listMusic.SetCurSel(0);
			m_curPlayIndex = 0;
		}
	}
}

void CmusicProjDlg::PlayMusic(CString strPath,CString strName)
{
	SetDlgItemTextW(IDC_MusicName,strName);

	MCI_OPEN_PARMS mciOpen;
	mciOpen.lpstrDeviceType = L"mpegvideo";
	mciOpen.lpstrElementName = strPath;
	//打开
	mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
		(DWORD)&mciOpen);
	m_curDeviceID = mciOpen.wDeviceID;
	MCI_PLAY_PARMS mciPlay;
	mciSendCommand(m_curDeviceID,MCI_PLAY,MCI_NOTIFY,
		(DWORD)&mciPlay);
}

//双击list
void CmusicProjDlg::OnLbnDblclkList1()
{
	//如果有正在播放的音乐 ，停止，再播放双击的这个
	if (m_curDeviceID != 0)
	{
		StopMusic();
	}
	int index = m_listMusic.GetCurSel();
	CString strName;
	m_listMusic.GetText(index,strName);

	//获得路径
	for (list<CString>::iterator it = m_pathLis.begin();it != m_pathLis.end();
		it++)
	{
		if (it->Find(strName) != -1)
		{
			//找到，说明就是这个路径
			PlayMusic(*it,strName);
			m_curPlayIndex = index;
			break;
		}
	}
}

void CmusicProjDlg::StopMusic()
{
	if (m_curDeviceID != 0)
	{
		MCI_GENERIC_PARMS gp;
		gp.dwCallback = NULL;
		mciSendCommand(m_curDeviceID,MCI_CLOSE,MCI_WAIT,(DWORD)&gp);
		m_curDeviceID = 0;
	}
}

void CmusicProjDlg::OnBnClickedPlay()
{
	// 播放
	OnLbnDblclkList1();
}


void CmusicProjDlg::OnBnClickedPause()
{
	// 
	if (m_curDeviceID == 0)
	{
		return;
	}
	CString str;
	GetDlgItemText(IDC_Pause,str);
	if (str == "暂停")
	{
		//暂停播放
		SetDlgItemTextW(IDC_Pause,L"继续");
		MCI_PLAY_PARMS mciPlay;
		mciSendCommand(m_curDeviceID,MCI_PAUSE,MCI_NOTIFY,(DWORD)&mciPlay);
	}
	else
	{
		//继续播放
		SetDlgItemTextW(IDC_Pause,L"暂停");
		MCI_PLAY_PARMS mciPlay;
		mciSendCommand(m_curDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);
	}
}


void CmusicProjDlg::OnBnClickedStop()
{
	// 停止
	StopMusic();
}


void CmusicProjDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_curDeviceID == 0)
	{
		CDialogEx::OnTimer(nIDEvent);
		return;
	}
	//获得音乐的总时间，当前播放进度时间
	MCI_STATUS_PARMS mciStatus;
	mciStatus.dwItem = MCI_STATUS_LENGTH;
	mciSendCommand(m_curDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mciStatus);
	DWORD allLen = mciStatus.dwReturn;	//音乐总毫秒数

	mciStatus.dwItem = MCI_STATUS_POSITION;
	mciSendCommand(m_curDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mciStatus);
	DWORD curLen = mciStatus.dwReturn;	//当前播放了的毫秒数

	int secondsAll = allLen/1000%60;
	int minsAll = allLen/1000/60;
	int secondsCur = curLen/1000%60;
	int minsCur = curLen/1000/60;
	CString str;
	str.Format(L"%02d:%02d/%02d:%02d",minsCur,secondsCur,minsAll,secondsAll
		);
	SetDlgItemTextW(IDC_Time,str);

	m_progress.SetPos(curLen*100/allLen);

	if (curLen == allLen)
	{
		//当前这首歌播放完成了
		StopMusic();
		AutoPlayNextMusic();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CmusicProjDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_curDeviceID == 0)
	{
		m_progress.SetPos(0);
		return;
	}

	int pos = m_progress.GetPos();
	MCI_STATUS_PARMS mciStatus;
	mciStatus.dwItem = MCI_STATUS_LENGTH;
	mciSendCommand(m_curDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mciStatus);
	DWORD allLen = mciStatus.dwReturn;

	MCI_SEEK_PARMS seekParam;
	seekParam.dwTo = allLen*pos/100;//毫秒数
	mciSendCommand(m_curDeviceID,MCI_SEEK,MCI_TO | MCI_WAIT,(DWORD)&seekParam);

	MCI_PLAY_PARMS mciPlay;
	mciSendCommand(m_curDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);
}

void CmusicProjDlg::AutoPlayNextMusic()
{
	CString strName;
	int num = m_listMusic.GetCount();
	int index = (m_curPlayIndex+1)%m_listMusic.GetCount();
	m_listMusic.GetText(index,strName);
	for (list<CString>::iterator it = m_pathLis.begin();it != m_pathLis.end();
		it++)
	{
		if (it->Find(strName) != -1)
		{
			StopMusic();
			PlayMusic(*it,strName);
			m_curPlayIndex = index;
			m_listMusic.SetCurSel(m_curPlayIndex);
		}
	}
}