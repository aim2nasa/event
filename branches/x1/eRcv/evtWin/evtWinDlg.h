
// evtWinDlg.h : ��� ����
//

#pragma once
#include "CEvt.h"
#include "afxcmn.h"

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
	DECLARE_MESSAGE_MAP()

	CEvt m_er;
	CIPAddressCtrl m_ctrlServerIp;
	UINT m_uServerport;
};
