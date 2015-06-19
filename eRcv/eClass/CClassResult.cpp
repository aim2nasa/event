#include "CClassResult.h"
#include "ace/Log_Msg.h"

CClassResult::CClassResult()
{

}

CClassResult::~CClassResult()
{

}

void CClassResult::onKeyEvent(int startIndex, int endIndex)
{
	ACE_DEBUG((LM_DEBUG, "%I+Key: index(%d~%d)\n",startIndex,endIndex));
}

void CClassResult::onTouchEvent(TOUCH_TYPE type, int startIndex, int endIndex)
{
	ACE_DEBUG((LM_DEBUG, "%I-Touch: %s,index(%d~%d)\n", (type==SWIPE)?"Swipe":"Tap",startIndex, endIndex));
}