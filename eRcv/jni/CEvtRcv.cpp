#include "CEvtRcv.h"
#include "ace/SOCK_Stream.h"
#include "../../common/def.h"

CEvtRcv::CEvtRcv(ACE_SOCK_Stream* p)
:_pStream(p)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtRcv Constructor\n")));
}

CEvtRcv::~CEvtRcv()
{
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

ACE_THR_FUNC_RETURN CEvtRcv::initResponse(void *p)
{
    CEvtRcv *pThis = reinterpret_cast<CEvtRcv*>(p);

    int resp[2];
    memset(resp,-1,sizeof(resp));
    for(int i=0;i<2;i++){
        int msg;
        if(pThis->recv_int(msg)>0) resp[i]=msg;
    }

    if(resp[0]==EVENT_RECORD_INIT && resp[1]==OK) pThis->_iEvt.signal();
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) initResponse end,[0]:%x[1]:%x\n"),resp[0],resp[1]));
    return 0;
}

int CEvtRcv::init()
{
    std::list<int> seq;
    seq.push_back(EVENT_RECORD_INIT);
    seq.push_back(_devList.size());

    for(std::list<int>::iterator it=_devList.begin();it!=_devList.end();++it)
        seq.push_back(*it);

    int sent = send(seq);

    ACE_thread_t tid;
    if(ACE_Thread_Manager::instance()->spawn(CEvtRcv::initResponse,
        (void*)this,THR_NEW_LWP|THR_JOINABLE,&tid)==-1) return -2;

    ACE_Time_Value tv(3); 
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting init response(%dsec)...\n"),tv.sec()));
    int wait=-1;
    if((wait=_iEvt.wait(&tv,0))!=0) return -3;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting(%dsec)=%d done\n"),tv.sec(),wait));

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
