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

void CClassResult::writeIndex(DEV_TYPE devType,int startIndex, int endIndex)
{
	int rtn = ACE_OS::fprintf(_fp, "%s %d %d", (devType==KEY)?"Key":"Touch",startIndex, endIndex);
	ACE_ASSERT(rtn > 0);
}

void CClassResult::onKeyEvent(int startIndex, int endIndex)
{
	writeIndex(KEY,startIndex, endIndex);
	ACE_OS::fprintf(_fp, "\n");
	ACE_DEBUG((LM_DEBUG, "%I+Key: index(%d~%d)\n",startIndex,endIndex));
}

void CClassResult::onTouchEvent(TOUCH_TYPE type, int startIndex, int endIndex)
{
	writeIndex(TOUCH,startIndex, endIndex);
	ACE_OS::fprintf(_fp, " %s", (type == SWIPE) ? "Swipe" : "Tap");
	ACE_OS::fprintf(_fp, "\n");
	ACE_DEBUG((LM_DEBUG, "%I-Touch: %s,index(%d~%d)\n", (type==SWIPE)?"Swipe":"Tap",startIndex, endIndex));
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