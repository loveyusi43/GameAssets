
// chatClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "chatClient.h"
#include "chatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET g_clientSock = 0;	//和服务器连接的SOCKET

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CchatClientDlg 对话框




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


// CchatClientDlg 消息处理程序

BOOL CchatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	if (!ConnectServer())
	{
		MessageBox(L"连接服务器失败",L"提示",MB_OK);
	}
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CchatClientDlg::OnPaint()
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
	if(!Connect(clientSocket))	//连接服务器
	{
		return false;
	}

	_beginthread(&CchatClientDlg::Receive,0,this);

	//发送消息给服务器，告诉服务器我的计算机名
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

	//开始连接
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
	//获得操作类型
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
			//某个用户在线
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
			wstring strNameSend = strBuf.substr(0,pos);//发送人
			strBuf = strBuf.substr(pos+1);
			pos = strBuf.find('_');
			if (pos == -1)
			{
				return;
			}
			wstring strNameSendTo = strBuf.substr(0,pos);//接收人
			wstring strText = strBuf.substr(pos+1);//消息内容
			wstring msg;
			if (msgType == Message_ChatToMe)
			{
				msg = L"【" +strNameSend + L"】对你说："+strText;
			}
			else
			{
				msg = L"你对【" + strNameSendTo+L"】说："+strText;
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
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_ChatText.GetWindowTextW(str);
	if (str.GetLength() == 0)
	{
		MessageBox(L"消息不能为空",L"提示",MB_OK);
		return;
	}
	if (str.GetLength() >= 800)
	{
		MessageBox(L"消息太长（少于800)",L"提示",MB_OK);
		return;
	}

	//确定要发送的人
	int index = m_listUserCtrl.GetCurSel();
	if (index == -1)
	{
		MessageBox(L"请选择要发送的对象",L"提示",MB_OK);
		return;
	}
	CString name;
	m_listUserCtrl.GetText(index,name);

	wstring strTemp = name;
	wstring strText = str;
	strTemp = strTemp+L"_"+strText;
	SendToServer(Message_Chat,strTemp);
}
