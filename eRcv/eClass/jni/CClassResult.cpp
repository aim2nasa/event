#include "CClassResult.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

CClassResult::CClassResult()
:_fp(NULL)
{

}

CClassResult::~CClassResult()
{

}

void CClassResult::writeIndex(DEV_TYPE devType,int device,int startIndex,int endIndex)
{
	int rtn = ACE_OS::fprintf(_fp, "%s(%d) %d %d", (devType==KEY)?"Key":"Touch",device,startIndex,endIndex);
	ACE_ASSERT(rtn > 0);
}

void CClassResult::onNewDevice(int device, DEV_TYPE devType, int index)
{
	int rtn = ACE_OS::fprintf(_fp, "New device %d type:%s index:%d\n", device, (devType == KEY) ? "Key" : "Touch",index);
	ACE_ASSERT(rtn > 0);
	ACE_DEBUG((LM_DEBUG, "%INew device(%d) type:%d\n", device, devType));
}

void CClassResult::onKeyEvent(int device, int code, int value, int startIndex, int endIndex)
{
	writeIndex(KEY,device,startIndex,endIndex);
	ACE_OS::fprintf(_fp, "\n");
	ACE_DEBUG((LM_DEBUG, "%I+Key(%d): index(%d~%d)\n",device,startIndex,endIndex));
}

void CClassResult::onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
{
	writeIndex(TOUCH,device,startIndex,endIndex);
	ACE_OS::fprintf(_fp, " %s", (type == SWIPE) ? "Swipe" : "Tap");
	ACE_OS::fprintf(_fp, "\n");
	ACE_DEBUG((LM_DEBUG, "%I-Touch(%d): %s,index(%d~%d)\n", device,(type==SWIPE)?"Swipe":"Tap",startIndex, endIndex));
}

void CClassResult::onError(ERR_CODE code)
{
	ACE_DEBUG((LM_DEBUG, "%I-***Error:%d***\n", code));
	ACE_ASSERT(false);
}

bool CClassResult::open(const char *name)
{
	_fp = ACE_OS::fopen(name, ACE_TEXT("w"));
	if (!_fp) return false;
	return true;
}

void CClassResult::close()
{
	ACE_OS::fclose(_fp);
}