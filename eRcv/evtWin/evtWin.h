
// evtWin.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CEvtWinApp:
// �� Ŭ������ ������ ���ؼ��� evtWin.cpp�� �����Ͻʽÿ�.
//

class CEvtWinApp : public CWinApp
{
public:
	CEvtWinApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CEvtWinApp theApp;