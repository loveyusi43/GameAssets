
// musicProjDlg.cpp : ʵ���ļ�
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


// CmusicProjDlg �Ի���




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


// CmusicProjDlg ��Ϣ�������

BOOL CmusicProjDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);
	SetTimer(1,100,NULL);
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CmusicProjDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

		
		//��ӽ���ļ����ǵ�һ�׸裬��ӽ������Զ�����
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
	//��
	mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
		(DWORD)&mciOpen);
	m_curDeviceID = mciOpen.wDeviceID;
	MCI_PLAY_PARMS mciPlay;
	mciSendCommand(m_curDeviceID,MCI_PLAY,MCI_NOTIFY,
		(DWORD)&mciPlay);
}

//˫��list
void CmusicProjDlg::OnLbnDblclkList1()
{
	//��������ڲ��ŵ����� ��ֹͣ���ٲ���˫�������
	if (m_curDeviceID != 0)
	{
		StopMusic();
	}
	int index = m_listMusic.GetCurSel();
	CString strName;
	m_listMusic.GetText(index,strName);

	//���·��
	for (list<CString>::iterator it = m_pathLis.begin();it != m_pathLis.end();
		it++)
	{
		if (it->Find(strName) != -1)
		{
			//�ҵ���˵���������·��
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
	// ����
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
	if (str == "��ͣ")
	{
		//��ͣ����
		SetDlgItemTextW(IDC_Pause,L"����");
		MCI_PLAY_PARMS mciPlay;
		mciSendCommand(m_curDeviceID,MCI_PAUSE,MCI_NOTIFY,(DWORD)&mciPlay);
	}
	else
	{
		//��������
		SetDlgItemTextW(IDC_Pause,L"��ͣ");
		MCI_PLAY_PARMS mciPlay;
		mciSendCommand(m_curDeviceID,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);
	}
}


void CmusicProjDlg::OnBnClickedStop()
{
	// ֹͣ
	StopMusic();
}


void CmusicProjDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_curDeviceID == 0)
	{
		CDialogEx::OnTimer(nIDEvent);
		return;
	}
	//������ֵ���ʱ�䣬��ǰ���Ž���ʱ��
	MCI_STATUS_PARMS mciStatus;
	mciStatus.dwItem = MCI_STATUS_LENGTH;
	mciSendCommand(m_curDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mciStatus);
	DWORD allLen = mciStatus.dwReturn;	//�����ܺ�����

	mciStatus.dwItem = MCI_STATUS_POSITION;
	mciSendCommand(m_curDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&mciStatus);
	DWORD curLen = mciStatus.dwReturn;	//��ǰ�����˵ĺ�����

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
		//��ǰ���׸貥�������
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
	seekParam.dwTo = allLen*pos/100;//������
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