#include "CEvtProxy.h"
#include "ace/SOCK_Stream.h"
#include "def.h"
#include <linux/input.h>
#include "ace/Date_Time.h"

CEvtProxy::CEvtProxy(ACE_SOCK_Stream* p)
:_pStream(p),_fp(NULL)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtProxy Constructor\n")));
}

CEvtProxy::~CEvtProxy()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtProxy() Destructor\n")));
}

std::size_t CEvtProxy::addDevice(int n)
{
    _devList.push_back(n);
    return devices();
}

std::size_t CEvtProxy::devices()
{
    return _devList.size();
}

std::list<int>& CEvtProxy::devList()
{
    return _devList;
}

ACE_THR_FUNC_RETURN CEvtProxy::initResponse(void *p)
{
    CEvtProxy *pThis = reinterpret_cast<CEvtProxy*>(p);

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

int CEvtProxy::init()
{
    std::list<int> seq;
    seq.push_back(EVENT_RECORD_INIT);
    seq.push_back(_devList.size());

    for(std::list<int>::iterator it=_devList.begin();it!=_devList.end();++it)
        seq.push_back(*it);

    int sent = send(seq);

    ACE_thread_t tid;
    if(ACE_Thread_Manager::instance()->spawn(CEvtProxy::initResponse,
        (void*)this,THR_NEW_LWP|THR_JOINABLE,&tid)==-1) return -2;

    ACE_Time_Value tv(3); 
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting init response(%dsec)...\n"),tv.sec()));
    int wait=-1;
    if((wait=_iEvt.wait(&tv,0))!=0) return -3;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting(%dsec)=%d done\n"),tv.sec(),wait));

    return sent;
}

int CEvtProxy::start()
{
    return activate();
}

int CEvtProxy::stop()
{
    ACE_TRACE("CEvtProxy::stop");

    int msg = TERMINATE_SERVER;
    ssize_t rcvSize = _pStream->send_n(&msg,sizeof(int));
    if (rcvSize!=sizeof(int)) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) error in sending msg(0x%x)\n"),msg));
        return -1;
    }
    return 0;
}

int CEvtProxy::send(int msg)
{
    ssize_t rcvSize = _pStream->send_n(&msg,sizeof(int));
    if (rcvSize!=sizeof(int)) {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) error in sending msg(0x%x)\n"),msg));
        return -1;
    }
    return 0;
}

int CEvtProxy::send(std::list<int>& seq)
{
    int sent=0;
    for(std::list<int>::iterator it=seq.begin();it!=seq.end();++it)
        if(send(*it)==0) sent++;
    return sent;
}

int CEvtProxy::recv_int(int& msg)
{
    ssize_t rcvSize = _pStream->recv_n(&msg,sizeof(int));
    if (rcvSize <= 0) return -1;
    return rcvSize;
}

int CEvtProxy::recordStart()
{
    ACE_TRACE("CEvtProxy::recordStart");

    ACE_Date_Time dt(ACE_OS::gettimeofday());
    ACE_TCHAR filename[512];
    ACE_OS::sprintf(filename,ACE_TEXT("Evt_%04d%02d%02d_%02d%02d%02d_%03d.bin"),
        dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), dt.microsec());

    _fp = ACE_OS::fopen(filename, ACE_TEXT("wb"));
    return send(EVENT_RECORD_START);
}

int CEvtProxy::recordStop()
{
    ACE_TRACE("CEvtProxy::recordStop");
    ACE_OS::fclose(_fp);
    return send(EVENT_RECORD_STOP);
}

int CEvtProxy::upload(const char* file)
{
    size_t length;
    if((length=ACE_OS::strlen(file))<=0) return -1;
    if(send(EVENT_FILE_UPLOAD)<0) return -2;
    if(send(length)<0) return -3;

    ssize_t size = _pStream->send_n(file,length);
    if(size!=length) return -4;

    return 0;
}

int CEvtProxy::play(const char* file)
{
    return 0;
}

int CEvtProxy::onEventRecordData()
{
    int size,rcvSize;
    if((rcvSize=recv_int(size))<0)
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) size read error\n")),-1);;

    rcvSize = _pStream->recv_n(_buffer,sizeof(int)+sizeof(struct input_event));
    if(rcvSize <= 0) return -2;

    size_t written = ACE_OS::fwrite(_buffer,1,rcvSize,_fp);
    if(written!=rcvSize) return -3;

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) EventRecordData(%d)\n"),rcvSize));
    return 0;
}

int CEvtProxy::svc()
{
    ACE_TRACE("CEvtProxy::svc");

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
            onEventRecordData();
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
