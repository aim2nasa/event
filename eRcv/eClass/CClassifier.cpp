#include "CClassifier.h"
#include "ace/Log_Msg.h"

CClassifier::CClassifier()
{

}

CClassifier::~CClassifier()
{

}

int CClassifier::addEvt(int device, long sec, long usec, int type, int code, int value)
{
	ACE_DEBUG((LM_DEBUG, "[%T] dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",device,sec,usec,type,code,value));
	return 0;
}