
// chatClientDlg.h : ͷ�ļ�
//

#pragma once
#include "string"
#include "list"
#include "afxwin.h"
using namespace std;

enum Message_Type
{
	Message_Min,
	Message_Login,
	Message_LoginOut,
	Message_Chat,
	Message_ChatToMe,
	Message_ChatToOther,
	Message_Max
};

// CchatClientDlg �Ի���
class CchatClientDlg : public CDialogEx
{
// ����
public:
	CchatClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHATCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	bool Connect(SOCKET sock);
	bool ConnectServer();
	static void Receive(void* p);

	void SendToServer(Message_Type msgType,wstring str= L"");
	void RecvMessage(wstring bufStr);
	void UpdateMsg();

	list<wstring> m_listUser;

	list<wstring> m_listMsg;
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listUserCtrl;
	afx_msg void OnBnClickedSendmsg();
	CEdit m_ChatText;
};
