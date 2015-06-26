#include "stdafx.h"
#include "CClassRes.h"

CClassRes::CClassRes()
{

}

CClassRes::~CClassRes()
{

}

void CClassRes::onNewDevice(int device, DEV_TYPE devType, int index)
{
	TRACE(_T("New device(%d) type:%d\n"), device, devType);
}

void CClassRes::onKeyEvent(int device, int startIndex, int endIndex)
{
	TRACE(_T("+Key(%d): index(%d~%d)\n"), device, startIndex, endIndex);
}

void CClassRes::onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
{
	TRACE(_T("-Touch(%d): %s,index(%d~%d)\n"), device, (type == SWIPE) ? _T("Swipe") : _T("Tap"), startIndex, endIndex);
}

void CClassRes::onError(ERR_CODE code)
{
	TRACE(_T("***Error:%d***\n"), code);
}