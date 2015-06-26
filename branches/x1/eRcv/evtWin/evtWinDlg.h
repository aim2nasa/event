
// evtWinDlg.h : 헤더 파일
//

#pragma once
#include "CEvt.h"
#include "afxcmn.h"

// CEvtWinDlg 대화 상자
class CEvtWinDlg : public CDialogEx
{
// 생성입니다.
public:
	CEvtWinDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EVTWIN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
