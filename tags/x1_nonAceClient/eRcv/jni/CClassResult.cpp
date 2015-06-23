#include "CClassResult.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

CClassResult::CClassResult()
{

}

CClassResult::~CClassResult()
{

}

void CClassResult::onNewDevice(int device, DEV_TYPE devType, int index)
{
    ACE_DEBUG((LM_DEBUG, "%INew device(%d) type:%d\n", device, devType));
}

void CClassResult::onKeyEvent(int device, int startIndex, int endIndex)
{
    ACE_DEBUG((LM_DEBUG, "%I+Key(%d): index(%d~%d)\n",device,startIndex,endIndex));
}

void CClassResult::onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
{
    ACE_DEBUG((LM_DEBUG, "%I-Touch(%d): %s,index(%d~%d)\n",
        device,(type==SWIPE)?"Swipe":"Tap",startIndex, endIndex));
}

void CClassResult::onError(ERR_CODE code)
{
    ACE_DEBUG((LM_DEBUG, "%I-***Error:%d***\n", code));
}
