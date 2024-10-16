
// musicProjDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "list"
#include "afxcmn.h"
using namespace std;


// CmusicProjDlg �Ի���
class CmusicProjDlg : public CDialogEx
{
// ����
public:
	CmusicProjDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MUSICPROJ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddmusic();
	CListBox m_listMusic;
	list<CString> m_pathLis;

	DWORD m_curDeviceID;

public:
	void PlayMusic(CString strPath,CString strName);
	void StopMusic();
	void AutoPlayNextMusic();

	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_progress;

	int m_curPlayIndex;
};
