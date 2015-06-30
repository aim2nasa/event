
// evtWinDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "evtWin.h"
#include "evtWinDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_CONNECION_FAILED		(WM_USER+100)
#define WM_CONNECTED			(WM_USER+101)

#define RECORD_UI_REFRESH_TIMER	1000
#define RECORD_UI_REFRESH_UNIT	100

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CEvtWinDlg ��ȭ ����



CEvtWinDlg::CEvtWinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEvtWinDlg::IDD, pParent)
	, m_uServerport(0)
	, m_pConThread(NULL)
	, m_bConnect(FALSE)
	, m_bRecord(FALSE)
	, m_strRecords(_T(""))
	, m_strTouch(_T(""))
	, m_strKey(_T(""))
	, m_strErrors(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEvtWinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_IPADDRESS, m_ctrlServerIp);
	DDX_Text(pDX, IDC_SERVER_PORT_EDIT, m_uServerport);
	DDX_Control(pDX, IDC_LOG_LIST, m_logList);
	DDX_Control(pDX, IDC_CONNECT_LED_STATIC, m_ctrlConnLED);
	DDX_Text(pDX, IDC_RECORDS_STATIC, m_strRecords);
	DDX_Text(pDX, IDC_TOUCH_STATIC, m_strTouch);
	DDX_Text(pDX, IDC_KEY_STATIC, m_strKey);
	DDX_Text(pDX, IDC_ERRORS_STATIC, m_strErrors);
}

BEGIN_MESSAGE_MAP(CEvtWinDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CEvtWinDlg::OnBnClickedConnectButton)
	ON_MESSAGE(WM_CONNECION_FAILED, OnConnectionFailed)
	ON_MESSAGE(WM_CONNECTED, OnConnected)
	ON_BN_CLICKED(IDC_RECORD_BUTTON, &CEvtWinDlg::OnBnClickedRecordButton)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PLAY_BUTTON, &CEvtWinDlg::OnBnClickedPlayButton)
	ON_BN_CLICKED(IDC_STEP_PLAY_BUTTON, &CEvtWinDlg::OnBnClickedStepPlayButton)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CEvtWinDlg �޽��� ó����

BOOL CEvtWinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	LCString(_T("Loading parameters..."));
	iniRead();

	CString str;
	str.Format(_T("server ip:%s port:%u"), m_strIp.GetBuffer(m_strIp.GetLength()), m_uServerport);
	LCString(str);

	m_ctrlBmpGrey.LoadBitmap(IDB_GREY);
	m_ctrlBmpGreen.LoadBitmap(IDB_GREEN);
	m_ctrlBmpRed.LoadBitmap(IDB_RED);

	m_ctrlConnLED.SetBitmap(m_ctrlBmpGrey);

	GetDlgItem(IDC_SERVER_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_SERVER_PORT_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(FALSE);

	GetDlgItem(IDC_RECORDS_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOUCH_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_KEY_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ERRORS_STATIC)->ShowWindow(SW_HIDE);

	m_er.playNoti(this);
	UpdateData(FALSE);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CEvtWinDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CEvtWinDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CEvtWinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEvtWinDlg::CStringToCharBuffer(char* pBuffer, int nBufferSize, CString& str)
{
	WideCharToMultiByte(CP_ACP, 0, str.GetBuffer(str.GetLength()), -1, pBuffer, nBufferSize, NULL, NULL);
}

void CEvtWinDlg::OnBnClickedConnectButton()
{
	if (!m_bConnect){
		GetDlgItem(IDC_SERVER_IPADDRESS)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVER_PORT_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(_T("Connecting"));

		UpdateData(TRUE);

		BYTE nField0, nFiled1, nField2, nField3;
		m_ctrlServerIp.GetAddress(nField0, nFiled1, nField2, nField3);
		m_strIp.Format(_T("%d.%d.%d.%d"), nField0, nFiled1, nField2, nField3);
		iniWrite();

		CString str;
		str.Format(_T("Connecting to ip:%s port:%u..."), m_strIp.GetBuffer(), m_uServerport);
		LCString(str);

		m_pConThread = AfxBeginThread(connect, (LPVOID)this);
	}else{
		m_er.close();
		m_bConnect = FALSE;
		m_ctrlConnLED.SetBitmap(m_ctrlBmpGrey);
		LCString(_T("Connection closed"));

		GetDlgItem(IDC_SERVER_IPADDRESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_SERVER_PORT_EDIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(_T("Connect"));
		GetDlgItem(IDC_RECORDS_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TOUCH_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_KEY_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ERRORS_STATIC)->ShowWindow(SW_SHOW);
	}
}

void CEvtWinDlg::iniRead()
{
	TCHAR szPathName[MAX_PATH],szSection[MAX_PATH],sztemp[MAX_PATH];

	GetModuleFileName(NULL, szPathName, MAX_PATH);
	PathRenameExtension(szPathName, _T(".ini"));

	CString strFileName(szPathName);

	_stprintf_s(szSection, _T("%s"), _T("SERVER"));
	GetPrivateProfileString(szSection, _T("ip"), _T(""), sztemp, sizeof(sztemp), strFileName);
	m_strIp = sztemp;
	m_uServerport = GetPrivateProfileInt(szSection, _T("port"), 0, strFileName);

	BYTE nField[4];
	CString resToken;
	int curPos = 0, i=0;
	resToken = m_strIp.Tokenize(_T("."), curPos);
	nField[0] = _ttoi(resToken.GetBuffer(resToken.GetLength()));
	for (int i = 1; i < 4;i++)
	{
		resToken = m_strIp.Tokenize(_T("."), curPos);
		nField[i] = _ttoi(resToken.GetBuffer(resToken.GetLength()));
	}
	m_ctrlServerIp.SetAddress(nField[0], nField[1], nField[2], nField[3]);
}

void CEvtWinDlg::iniWrite()
{
	TCHAR szPathName[MAX_PATH], szSection[MAX_PATH], sztemp[MAX_PATH];

	GetModuleFileName(NULL, szPathName, MAX_PATH);
	PathRenameExtension(szPathName, _T(".ini"));

	CString strFileName(szPathName);

	_stprintf_s(szSection, _T("%s"), _T("SERVER"));
	WritePrivateProfileString(szSection, _T("ip"), m_strIp.GetBuffer(), strFileName);

	_stprintf_s(sztemp, _T("%u"), m_uServerport);
	WritePrivateProfileString(szSection, _T("port"), sztemp, strFileName);
}

void CEvtWinDlg::LCString(CString str)
{
	if (m_logList.GetCount() >= MAX_LOG) m_logList.ResetContent();

	CString strTime = CTime::GetCurrentTime().Format(_T("[%m.%d %H:%M:%S] "));

	m_logList.AddString(strTime + str);
	m_logList.SetTopIndex((m_logList.GetCount() > 0) ? m_logList.GetCount() - 1 : 0);
}

UINT CEvtWinDlg::connect(LPVOID pParam)
{
	CEvtWinDlg *pDlg = reinterpret_cast<CEvtWinDlg*>(pParam);
	ASSERT(pDlg);

	char buffer[MAX_PATH];
	CStringToCharBuffer(buffer, sizeof(buffer), pDlg->m_strIp);
	int rtn;
	if ((rtn = pDlg->m_er.open(buffer, pDlg->m_uServerport)) < 0) {
		pDlg->PostMessage(WM_CONNECION_FAILED, (WPARAM)rtn);
		return -1;
	}
	pDlg->PostMessage(WM_CONNECTED);
	return 0;
}

LRESULT CEvtWinDlg::OnConnectionFailed(WPARAM wParam, LPARAM lParam)
{
	m_bConnect = FALSE;
	m_ctrlConnLED.SetBitmap(m_ctrlBmpRed);
	CString str;
	str.Format(_T("Connection failed to server, error code(%d)"),(int)wParam);
	LCString(str);

	GetDlgItem(IDC_SERVER_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_SERVER_PORT_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(_T("Connect"));
	UpdateData(FALSE);
	return 0;
}

LRESULT CEvtWinDlg::OnConnected(WPARAM wParam, LPARAM lParam)
{
	m_bConnect = TRUE;
	m_ctrlConnLED.SetBitmap(m_ctrlBmpGreen);
	LCString(_T("Connected to server"));

	GetDlgItem(IDC_SERVER_IPADDRESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SERVER_PORT_EDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT_BUTTON)->SetWindowText(_T("Disconnect"));
	GetDlgItem(IDC_RECORDS_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TOUCH_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_KEY_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_ERRORS_STATIC)->ShowWindow(SW_SHOW);
	UpdateData(FALSE);
	return 0;
}

void CEvtWinDlg::OnBnClickedRecordButton()
{
	if (!m_bRecord){
		m_recCount.reset();
		if (m_er.recordStart(this) < 0){
			LCString(_T("recordStart error"));
			return;
		}
		SetTimer(RECORD_UI_REFRESH_TIMER, RECORD_UI_REFRESH_UNIT, NULL);
		m_bRecord = TRUE;
		GetDlgItem(IDC_RECORD_BUTTON)->SetWindowText(_T("Stop"));
		GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(FALSE);
		LCString(_T("recording started"));
	}else{
		KillTimer(RECORD_UI_REFRESH_TIMER);
		if (m_er.recordStop() < 0){
			LCString(_T("recordStop error"));
			return;
		}
		m_bRecord = FALSE;
		GetDlgItem(IDC_RECORD_BUTTON)->SetWindowText(_T("Record"));
		GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(TRUE);
		LCString(_T("recording stopped"));
	}
}


void CEvtWinDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_er.close();
}

void CEvtWinDlg::onNewDevice(int device, DEV_TYPE devType, int index)
{
	m_recCount.m_uNewDev++;
	CString str;
	str.Format(_T("New device(%d) type:%d\n"), device, devType);
	LCString(str);
}

void CEvtWinDlg::onKeyEvent(int device, int startIndex, int endIndex)
{
	m_recCount.m_uKeyEvt++;
	m_recCount.m_uRecords += (endIndex - startIndex) + 1;
	CString str;
	str.Format(_T("Key(%d): index(%d~%d)\n"), device, startIndex, endIndex);
	LCString(str);
}

void CEvtWinDlg::onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
{
	m_recCount.m_uTouchEvt++;
	m_recCount.m_uRecords += (endIndex - startIndex) + 1;
	CString str;
	str.Format(_T("Touch(%d): %s,index(%d~%d)\n"), device, (type == SWIPE) ? _T("Swipe") : _T("Tap"), startIndex, endIndex);
	LCString(str);
}

void CEvtWinDlg::onError(ERR_CODE code)
{
	m_recCount.m_uErr++;
	CString str;
	str.Format(_T("Error:%d\n"), code);
	LCString(str);
}

void CEvtWinDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == RECORD_UI_REFRESH_TIMER){
		m_strRecords.Format(_T("Records: %u"),m_recCount.m_uRecords);
		m_strTouch.Format(_T("Touch: %u"),m_recCount.m_uTouchEvt);
		m_strKey.Format(_T("Key: %u"),m_recCount.m_uKeyEvt);
		m_strErrors.Format(_T("Errors: %u"), m_recCount.m_uErr);
	}

	UpdateData(FALSE);
	__super::OnTimer(nIDEvent);
}

void CEvtWinDlg::OnBnClickedPlayButton()
{
	GetDlgItem(IDC_PLAY_BUTTON)->SetWindowText(_T("Playing"));
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(FALSE);

	TCHAR szFilter[] = _T("All Files(*.*) | *.*");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal()) {
		CString str = dlg.GetPathName();

		char buffer[256];
		CStringToCharBuffer(buffer,sizeof(buffer),str);

		LCString(CString(_T("Playing "))+str);
		int rtn;
		if ((rtn = m_er.play(buffer)) != 0) {
			CString msg(_T("play error:"));
			LCString(msg + str);
			return;
		}
		LCString(CString(_T("Play requested")));
	}
	GetDlgItem(IDC_PLAY_BUTTON)->SetWindowText(_T("Play"));
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(TRUE);
}

void CEvtWinDlg::OnBnClickedPartPlayButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CEvtWinDlg::OnBnClickedStepPlayButton()
{
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(FALSE);

	TCHAR szFilter[] = _T("All Files(*.*) | *.*");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal()) {
		CString str = dlg.GetPathName();

		char buffer[256];
		CStringToCharBuffer(buffer, sizeof(buffer), str);

		int nCount;
		if ((nCount = m_er.stepCount(buffer)) < 0) {
			CString msg;
			msg.Format(_T("error stepCount"));
			LCString(msg);
			GetDlgItem(IDC_PLAY_BUTTON)->SetWindowText(_T("Step Play"));
			GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(TRUE);
			GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
			GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(TRUE);
			GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(TRUE);
			return;
		}
		CString msg;
		msg.Format(_T("%d Steps for "),nCount);
		LCString( msg+str);

		int rtn;
		for (int i = 0; i < nCount ; i++){
			msg.Format(_T("Play Step [%d/%d]?"),i+1,nCount);
			if (IDYES == AfxMessageBox(msg, MB_YESNO)){
				if (i == 0) {
					if ((rtn = m_er.playFirst(buffer)) != 0) {
						msg.Format(_T("%d Step playFirst error"), i);
						LCString(msg);
					}
				}else{
					if ((rtn = m_er.playNext()) != 0) {
						msg.Format(_T("%d Step playNext error"), i);
						LCString(msg);
					}
				}
			}else{
				break;
			}
		}
	}

	GetDlgItem(IDC_STEP_PLAY_BUTTON)->SetWindowText(_T("Step Play"));
	GetDlgItem(IDC_PLAY_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_STEP_PLAY_BUTTON)->EnableWindow(TRUE);
}

void CEvtWinDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_logList.GetSafeHwnd()) {
		CRect rc;
		GetClientRect(&rc);

		rc.left = 0;
		rc.top = 131;
		rc.bottom -= 0;
		rc.right -= 0;
		m_logList.MoveWindow(&rc);
	}
}

void CEvtWinDlg::fullPlay(int err)
{
	CString str;
	str.Format(_T("full play done(%d)"),err);
	LCString(str);
}

void CEvtWinDlg::partPlay(int err)
{
	CString str;
	str.Format(_T("part play done(%d)"), err);
	LCString(str);
}