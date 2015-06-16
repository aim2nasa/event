#include "CEvtRcv.h"
#include "ace/SOCK_Stream.h"
#include "../../common/def.h"
#include "ace/Auto_Event.h"

CEvtRcv::CEvtRcv(ACE_SOCK_Stream* p)
:_pStream(p),_pInit(new ACE_Auto_Event)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtRcv Constructor\n")));
}

CEvtRcv::~CEvtRcv()
{
    delete _pInit;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtRcv() Destructor\n")));
}

std::size_t CEvtRcv::addDevice(int n)
{
    _devList.push_back(n);
    return devices();
}

std::size_t CEvtRcv::devices()
{
    return _devList.size();
}

std::list<int>& CEvtRcv::devList()
{
    return _devList;
}

int CEvtRcv::init()
{
    std::list<int> seq;
    seq.push_back(EVENT_RECORD_INIT);
    seq.push_back(_devList.size());

    for(std::list<int>::iterator it=_devList.begin();it!=_devList.end();++it)
        seq.push_back(*it);

    int sent = send(seq);
    ACE_Time_Value tv(2); 
    if(_pInit->wait(&tv,0)!=0) return ERROR_EVTREC_IN_TIMEOUT;
    return sent;
}

int CEvtRcv::start()
{
    return activate();
}

int CEvtRcv::stop()
{
    ACE_TRACE("CEvtRcv::stop");

    int msg = TERMINATE_SERVER;
    ssize_t rcvSize = _pStream->send_n(&msg,sizeof(int));
    if (rcvSize <= 0) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) error in sending msg(0x%x)\n"),msg));
        return -1;
    }
    return 0;
}

int CEvtRcv::send(int msg)
{
    ssize_t rcvSize = _pStream->send_n(&msg,sizeof(int));
    if (rcvSize <= 0) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) error in sending msg(0x%x)\n"),msg));
        return -1;
    }
    return 0;
}

int CEvtRcv::send(std::list<int>& seq)
{
    int sent=0;
    for(std::list<int>::iterator it=seq.begin();it!=seq.end();++it)
        if(send(*it)==0) sent++;
    return sent;
}

int CEvtRcv::recv_int(int& msg)
{
    ssize_t rcvSize = _pStream->recv_n(&msg,sizeof(int));
    if (rcvSize <= 0) return -1;
    return rcvSize;
}

int CEvtRcv::recordStart()
{
    ACE_TRACE("CEvtRcv::recordStart");
    return send(EVENT_RECORD_START);
}

int CEvtRcv::recordStop()
{
    ACE_TRACE("CEvtRcv::recordStop");
    return send(EVENT_RECORD_STOP);
}

int CEvtRcv::svc()
{
    ACE_TRACE("CEvtRcv::svc");

    int msg,rcvSize;
    char buf[BUFSIZ];
    while (1){
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting to receive...\n")));
        if((rcvSize=recv_int(msg))<0)
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) recv error\n")),-1);;

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %d received\n"),rcvSize));

        int r;
        switch(msg){
        case EVENT_RECORD_INIT:
            recv_int(r);
            if(r!=OK)
                ACE_ERROR_RETURN((LM_ERROR,ACE_TEXT("(%P|%t) Event Record Init Err(0x%x)\n"),r),-1);
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Init(0x%x),Res=(0x%x)\n"),msg,r));
            break;
        case EVENT_RECORD_START:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Started(0x%x)\n"),msg));
            break;
        case EVENT_RECORD_DATA:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Data received(0x%x)\n"),msg));
            break;
        case EVENT_RECORD_STOP:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Stopped(0x%x)\n"),msg));
            break;
        case TERMINATE_CLIENT:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection Terminated(0x%x)\n"),msg));
            return 0;
        default:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Undefined command(0x%x)\n"),msg));
            break;
        }
    }
    return 0;
}
