
// evtWinDlg.h : 헤더 파일
//

#pragma once
#include "CEvt.h"
#include "IClassifyNoti.h"
#include "afxcmn.h"
#include "afxwin.h"

#define MAX_LOG	(1024*8)

class CRecCount{
public:
	CRecCount(){ reset(); }
	void reset() { m_uNewDev = m_uKeyEvt = m_uTouchEvt = m_uErr = m_uRecords = 0; }

	unsigned int m_uNewDev;
	unsigned int m_uKeyEvt;
	unsigned int m_uTouchEvt;
	unsigned int m_uErr;
	unsigned int m_uRecords;
};

// CEvtWinDlg 대화 상자
class CEvtWinDlg : public CDialogEx, public IClassifyNoti
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
	afx_msg LRESULT OnConnectionFailed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedRecordButton();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedPlayButton();
	afx_msg void OnBnClickedPartPlayButton();
	afx_msg void OnBnClickedStepPlayButton();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

	void iniRead();
	void iniWrite();
	void LCString(CString str);
	static void CStringToCharBuffer(char* pBuffer, int nBufferSize, CString& str);
	static UINT connect(LPVOID pParam);

	void onNewDevice(int device, DEV_TYPE devType, int index);
	void onKeyEvent(int device, int startIndex, int endIndex);
	void onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex);
	void onError(ERR_CODE code);

	CEvt m_er;
	CIPAddressCtrl m_ctrlServerIp;
	CString m_strIp;
	UINT m_uServerport;
	CListBox m_logList;
	CWinThread *m_pConThread;

	CBitmap	m_ctrlBmpGrey;
	CBitmap	m_ctrlBmpGreen;
	CBitmap m_ctrlBmpRed;
	CStatic m_ctrlConnLED;
	BOOL	m_bConnect;
	BOOL	m_bRecord;

	CRecCount m_recCount;
	CString m_strRecords;
	CString m_strTouch;
	CString m_strKey;
	CString m_strErrors;
};
