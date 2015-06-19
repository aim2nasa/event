#include "CClassifier.h"
#include "ace/Log_Msg.h"

CClassifier::CClassifier()
:_prevDev(-1)
{

}

CClassifier::~CClassifier()
{

}

void CClassifier::reset()
{
	_devMap.clear();
}

int CClassifier::addEvt(int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",device,sec,usec,type,code,value));

	std::pair<std::map<int,DEV_TYPE>::iterator, bool> ret;
	if (EV_KEY == (DEV_TYPE)type) {
		ret = _devMap.insert(std::make_pair(device, KEY));
		if(ret.second) ACE_DEBUG((LM_DEBUG, "[%T] dev:%02d type:%04x KEY TYPE registered(map size:%d)\n",device, type,_devMap.size()));
	}else if (EV_ABS == (DEV_TYPE)type){
		ret = _devMap.insert(std::make_pair(device, TOUCH));
		if (ret.second) ACE_DEBUG((LM_DEBUG, "[%T] dev:%02d type:%04x TOUCH TYPE registered(map size:%d)\n",device, type, _devMap.size()));
	}

	_prevDev = device;
	return 0;
}