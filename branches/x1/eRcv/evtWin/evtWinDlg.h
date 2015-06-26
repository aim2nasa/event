
// evtWinDlg.h : ��� ����
//

#pragma once
#include "CEvt.h"
#include "afxcmn.h"

#define MAX_LOG	(1024*8)

// CEvtWinDlg ��ȭ ����
class CEvtWinDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CEvtWinDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_EVTWIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedConnectButton();
	afx_msg void OnBnClickedExitButton();
	afx_msg void OnBnClickedConnectionCloseButton();
	afx_msg LRESULT OnConnectionFailed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnected(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void iniRead();
	void iniWrite();
	void LCString(CString str);
	static void CStringToCharBuffer(char* pBuffer, int nBufferSize, CString& str);
	static UINT connect(LPVOID pParam);

	CEvt m_er;
	CIPAddressCtrl m_ctrlServerIp;
	CString m_strIp;
	UINT m_uServerport;
	CListBox m_logList;
	CWinThread *m_pConThread;
};