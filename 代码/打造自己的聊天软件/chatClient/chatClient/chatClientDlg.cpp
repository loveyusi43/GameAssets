
// chatClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "chatClient.h"
#include "chatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET g_clientSock = 0;	//�ͷ��������ӵ�SOCKET

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CchatClientDlg �Ի���




CchatClientDlg::CchatClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CchatClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CchatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listUserCtrl);
	DDX_Control(pDX, IDC_MsgText, m_ChatText);
}

BEGIN_MESSAGE_MAP(CchatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SendMsg, &CchatClientDlg::OnBnClickedSendmsg)
END_MESSAGE_MAP()


// CchatClientDlg ��Ϣ�������

BOOL CchatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	if (!ConnectServer())
	{
		MessageBox(L"���ӷ�����ʧ��",L"��ʾ",MB_OK);
	}
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CchatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CchatClientDlg::OnPaint()
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
HCURSOR CchatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CchatClientDlg::Receive(void* p)
{
	char buf[2048] = {0};
	CchatClientDlg* pDlg = (CchatClientDlg*)p;
	if (pDlg == NULL)
	{
		return;
	}
	while(1)
	{
		int bytes;
		if ((bytes = recv(g_clientSock,buf,sizeof(buf),0)) == SOCKET_ERROR)
		{
			_endthread();
			return ;
		}
		buf[bytes] = '\0';
		wchar_t bufTest[1024];
		memcpy(bufTest,buf,bytes);
		bufTest[bytes/2] = '\0';
		pDlg->RecvMessage(bufTest);
	}
	return ;
}

bool CchatClientDlg::ConnectServer()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return false;
	}

	SOCKET clientSocket;
	if ((clientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
	{
		return false;
	}
	g_clientSock = clientSocket;
	if(!Connect(clientSocket))	//���ӷ�����
	{
		return false;
	}

	_beginthread(&CchatClientDlg::Receive,0,this);

	//������Ϣ�������������߷������ҵļ������
	SendToServer(Message_Login);
	return true;
}

bool CchatClientDlg::Connect(SOCKET sock)
{
	struct sockaddr_in serverAddress;
	memset(&serverAddress,0,sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	serverAddress.sin_port = htons(1986);

	//��ʼ����
	if (connect(sock,(sockaddr*)&serverAddress,
		sizeof(serverAddress)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

void CchatClientDlg::SendToServer(Message_Type msgType,wstring str)
{
	wchar_t sendBuf[1024];
	switch(msgType)
	{
	case Message_Login:
		{
			wchar_t computerName[MAX_COMPUTERNAME_LENGTH];
			DWORD len = MAX_COMPUTERNAME_LENGTH;
			GetComputerName(computerName,&len);
			wsprintf(sendBuf,L"%d_%s",Message_Login,computerName);
			break;
		}
	case Message_Chat:
		{
			wsprintf(sendBuf,L"%d_%s",Message_Chat,str.c_str());
			m_ChatText.SetWindowTextW(L"");
			break;
		}
	}
	send(g_clientSock,(char*)sendBuf,lstrlen(sendBuf)*2,0);
}

void CchatClientDlg::RecvMessage(wstring strBuf)
{
	//��ò�������
	int pos = strBuf.find('_');
	int msgType = Message_Min;
	if (pos != -1)
	{
		wstring bufTemp = strBuf.substr(0,pos);
		strBuf = strBuf.substr(pos+1);
		char* temp = (char*)bufTemp.c_str();
		msgType = atoi(temp);
	}
	else
	{
		char* temp = (char*)strBuf.c_str();
		msgType = atoi(temp);
	}
	if (msgType <= Message_Min || msgType >= Message_Max)
	{
		return;
	}
	switch(msgType)
	{
	case Message_Login:
		{
			//ĳ���û�����
			m_listUser.push_back(strBuf);
			m_listUserCtrl.AddString(strBuf.c_str());
			break;
		}
	case Message_LoginOut:
		{
			wstring loginOutName = strBuf;
			int index = m_listUserCtrl.FindString(-1,
				loginOutName.c_str());
			if (index != -1)
			{
				m_listUserCtrl.DeleteString(index);
			}
			break;
		}
	case Message_ChatToMe:
	case Message_ChatToOther:
		{
			pos = strBuf.find('_');
			if (pos == -1)
			{
				return;
			}
			wstring strNameSend = strBuf.substr(0,pos);//������
			strBuf = strBuf.substr(pos+1);
			pos = strBuf.find('_');
			if (pos == -1)
			{
				return;
			}
			wstring strNameSendTo = strBuf.substr(0,pos);//������
			wstring strText = strBuf.substr(pos+1);//��Ϣ����
			wstring msg;
			if (msgType == Message_ChatToMe)
			{
				msg = L"��" +strNameSend + L"������˵��"+strText;
			}
			else
			{
				msg = L"��ԡ�" + strNameSendTo+L"��˵��"+strText;
			}
			m_listMsg.push_back(msg);
			UpdateMsg();
			break;
		}
	}
}

void CchatClientDlg::UpdateMsg()
{
	if (m_listMsg.size() > 200)
	{
		m_listMsg.pop_front();
	}
	wstring strText;
	for (list<wstring>::iterator it = m_listMsg.begin();
		it != m_listMsg.end();it++)
	{
		strText = strText+(*it)+L"\r\n";
	}
	SetDlgItemText(IDC_MsgInfo,strText.c_str());
}

void CchatClientDlg::OnBnClickedSendmsg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_ChatText.GetWindowTextW(str);
	if (str.GetLength() == 0)
	{
		MessageBox(L"��Ϣ����Ϊ��",L"��ʾ",MB_OK);
		return;
	}
	if (str.GetLength() >= 800)
	{
		MessageBox(L"��Ϣ̫��������800)",L"��ʾ",MB_OK);
		return;
	}

	//ȷ��Ҫ���͵���
	int index = m_listUserCtrl.GetCurSel();
	if (index == -1)
	{
		MessageBox(L"��ѡ��Ҫ���͵Ķ���",L"��ʾ",MB_OK);
		return;
	}
	CString name;
	m_listUserCtrl.GetText(index,name);

	wstring strTemp = name;
	wstring strText = str;
	strTemp = strTemp+L"_"+strText;
	SendToServer(Message_Chat,strTemp);
}
