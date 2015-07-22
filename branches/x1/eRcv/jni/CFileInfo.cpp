#include "CFileInfo.h"
#include "CRecord.h"
#include "CClassifier.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

int CFileInfo::analyze(const char *file,CResult *result)
{
	FILE* fp = ACE_OS::fopen(file, ACE_TEXT("rb"));
	if (!fp) {
		ACE_DEBUG((LM_ERROR, "%s open error\n", file));
		ACE_RETURN(-10);
	}

	CClassifier cf;
	cf.notify(result);
	CRecord rec;
	size_t size, totalRead = 0;
	long index = 0,err=0;
	while (1){
		size = ACE_OS::fread(&rec, 1, sizeof(rec), fp);
		if (size != sizeof(rec)) break;
		totalRead += size;
		if (cf.addEvt(index, rec._device, rec._event.time.tv_sec, rec._event.time.tv_usec, rec._event.type, rec._event.code, rec._event.value) < 0) {
			err++;
			ACE_DEBUG((LM_ERROR, "[%T] error in add event\n"));
			break;
		}
		index++;
	}
	ACE_OS::fclose(fp);
	if (err == 0)
		ACE_RETURN(0);
	else
		ACE_RETURN(-20);
}

CResult::CResult()
:_pList(new UEVT_LIST())
{

}

CResult::~CResult()
{
	delete _pList;
}

void CResult::reset()
{
	_pList->clear();
}

UEVT_LIST& CResult::list()
{
	return *_pList;
}

void CResult::onNewDevice(int device, DEV_TYPE type, int index)
{

}

void CResult::onKeyEvent(int device, int code, int value, int startIndex, int endIndex)
{
	CUserEvent ue(KEY,device,startIndex,endIndex);
	_pList->push_back(ue);
}

void CResult::onTouchEvent(int device, TOUCH_TYPE type, int startIndex, int endIndex)
{
	CUserEvent ue(TOUCH, device, startIndex, endIndex, type);
	_pList->push_back(ue);
}

void CResult::onError(ERR_CODE code)
{

}