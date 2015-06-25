#include "CEvtProxy.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/OS.h"
#include "def.h"
#include "ace/Date_Time.h"
#include "CClassifier.h"
#include "input.h"
#include "CRecord.h"
#include "CFileInfo.h"

CEvtProxy::CEvtProxy()
:_pStream(NULL), _fp(NULL), _fSize(0), _pClass(new CClassifier()), _index(0), _pEventsInfo(new CResult())
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtProxy Constructor\n")));
}

CEvtProxy::~CEvtProxy()
{
	delete _pEventsInfo;
    delete _pClass;
	delete reinterpret_cast<ACE_SOCK_Stream*>(_pStream);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) CEvtProxy() Destructor\n")));
}

CResult *CEvtProxy::eventsInfo()
{
	return _pEventsInfo;
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

int CEvtProxy::init(const char *addr, unsigned short port)
{
	_pStream = reinterpret_cast<void*>(new ACE_SOCK_Stream());
	ACE_INET_Addr remote_addr(port,addr);
	ACE_SOCK_Connector connector;

	ACE_DEBUG((LM_DEBUG, "(%P|%t) Starting connect to %s: %d \n",
		remote_addr.get_host_name(), remote_addr.get_port_number()));

	if (connector.connect(*reinterpret_cast<ACE_SOCK_Stream*>(_pStream), remote_addr) == -1) {
		ACE_DEBUG((LM_DEBUG, "(%P|%t) connection failed to addr=%s,port=%d\n", remote_addr.get_host_name(),remote_addr.get_port_number()));
		return -10;
	}else
		ACE_DEBUG((LM_DEBUG, "(%P|%t) connected to to addr=%s,port=%d\n", remote_addr.get_host_name(), remote_addr.get_port_number()));

	ACE_OS::sleep(1);

	//연결이 제대로 되는지 검사 테스트
	ACE_DEBUG((LM_DEBUG, "(%P|%t) Connection testing...\n"));
	char inpBuff[128];
	ACE_Time_Value timeout(1);
	ssize_t result = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->recv(inpBuff, sizeof(inpBuff), &timeout);
	if (result == -1 && ACE_OS::last_error() == ETIME)
		ACE_DEBUG((LM_DEBUG, "(%P|%t) Connection test ok\n"));
	else {
		ACE_DEBUG((LM_DEBUG, "(%P|%t) Connection test failed\n"));
		return -30;
	}

    if(send(EVENT_RECORD_INIT)<0) return -35;

    ACE_thread_t tid;
    if(ACE_Thread_Manager::instance()->spawn(CEvtProxy::initResponse,
        (void*)this,THR_NEW_LWP|THR_JOINABLE,&tid)==-1) return -40;

    ACE_Time_Value tv(3); 
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting init response(%dsec)...\n"),tv.sec()));
    int wait=-1;
    if((wait=_iEvt.wait(&tv,0))!=0) return -50;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting(%dsec)=%d done\n"),tv.sec(),wait));

    return 0;
}

int CEvtProxy::start()
{
    return activate();
}

int CEvtProxy::stop()
{
    ACE_TRACE("CEvtProxy::stop");
    if(send(TERMINATE_SERVER)<0) return -1;
	wait();
    return 0;
}

int CEvtProxy::send(int msg)
{
	ssize_t rcvSize = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(&msg, sizeof(int));
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
	ssize_t rcvSize = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->recv_n(&msg, sizeof(int));
    if (rcvSize <= 0) return -1;
    return rcvSize;
}

int CEvtProxy::recordStart(IClassifyNoti *p)
{
    ACE_TRACE("CEvtProxy::recordStart");

    ACE_Date_Time dt(ACE_OS::gettimeofday());
    ACE_TCHAR filename[512];
    ACE_OS::sprintf(filename,ACE_TEXT("Evt_%04d%02d%02d_%02d%02d%02d_%03d.bin"),
        dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), dt.microsec());

    _fp = ACE_OS::fopen(filename, ACE_TEXT("wb"));
    _pClass->notify(p);
    _index = 0;
    return send(EVENT_RECORD_START);
}

int CEvtProxy::recordStop()
{
    ACE_TRACE("CEvtProxy::recordStop");
    ACE_OS::fclose(_fp);
    return send(EVENT_RECORD_STOP);
}

long CEvtProxy::fileSize(const char* file)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("fileSize(%s)\n"),file));
    FILE *fp = ACE_OS::fopen(file,ACE_TEXT("rb"));
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("fileSize(%s), fileopen=0x%x\n"),file,fp));
	if (!fp) {
		ACE_DEBUG((LM_ERROR, ACE_TEXT("fileopen(%s) GetLastError(%d) in fileSize\n"), file,ACE_OS::last_error()));
		return -1;
	}
    ACE_OS::fseek(fp,0,SEEK_END);
    long size = ACE_OS::ftell(fp); 
    ACE_OS::fclose(fp);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("return fileSize(%s)=%d\n"), file, size));
    return size;
}

long CEvtProxy::recordCount(const char* file)
{
	long fs;
	if ((fs=fileSize(file)) == -1) return fs;
	return fs / sizeof(CRecord);
}

int CEvtProxy::upPrepare(const char* file)
{
    size_t length;
    if((length=ACE_OS::strlen(file))<=0) return -10;
    if(send(EVENT_FILE_UP_PREPARE)<0) return -20;
    if(send(length)<0) return -30;

	ssize_t size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(file, length);
    if(size!=length) return -40;

    long fSize = fileSize(file);
    if(fSize<=0) return -50;
    _fSize = fSize;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %s filesize:%dbytes\n"),file,fSize));

	size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(&fSize, sizeof(long));
    if(size!=sizeof(long)) -60;

    ACE_Time_Value tv(3);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting for prepare response(%dsec)...\n"),tv.sec()));
    int wait;
    if((wait=_upEvt.wait(&tv,0))!=0) return -70;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting for prepare response done(%d)\n"),wait));
    return 0;
}

int CEvtProxy::upload(const char* file)
{
    if(send(EVENT_FILE_UPLOAD)<0) return -10;

	ssize_t size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(&_fSize, sizeof(long));
    if(size!=sizeof(long)) return -20;

	char buffer[BUFSIZ];
    FILE *fp = ACE_OS::fopen(file,ACE_TEXT("rb"));
    if(!fp) return -30;
    size_t readUnit = sizeof(int)+EVENT_SIZE,totalRead=0,totalSent=0;
    while(1){
        size_t readSize = ACE_OS::fread(buffer,1,readUnit,fp);
        if(readSize!=readUnit) break;
        totalRead += readSize;

		size_t size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(buffer, readSize);
        if(size!=readSize) return -40;
        totalSent += size;
    }
    ACE_OS::fclose(fp);
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) readin:%dbytes sent:%dbytes\n"),totalRead,totalSent));

	_pEventsInfo->reset();
	if(CFileInfo::analyze(file, _pEventsInfo)!=0) return -45;
    return 0;
}

int CEvtProxy::play(const char* file)
{
    int rtn;
    if((rtn=upPrepare(file))!=0) return rtn;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) prepared to upload:%s\n"),file));
    if((rtn=upload(file))!=0) return rtn;

    size_t length;
    if((length=ACE_OS::strlen(file))<=0) return -100;

    if(send(EVENT_PLAY_FULL)<0) return -110;
    if(send(length)<0) return -120;

	ssize_t size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(file, length);
    if(size!=length) return -130;

    return 0;
}

int CEvtProxy::play(const char* file,long long startLoc,long long endLoc)
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) play(%s,%Q,%Q)\n"),file,startLoc,endLoc));

    int rtn;
    if((rtn=upPrepare(file))!=0) return rtn;
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) prepared to upload:%s\n"),file));
    if((rtn=upload(file))!=0) return rtn;

    size_t length;
    if((length=ACE_OS::strlen(file))<=0) return -100;

    if(send(EVENT_PLAY_PART)<0) return -110;
    if(send(length)<0) return -120;

	ssize_t size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(file, length);
    if(size!=length) return -130;

	size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(&startLoc, sizeof(long long));
    if(size!=sizeof(long long)) return -140;

	size = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->send_n(&endLoc, sizeof(long long));
    if(size!=sizeof(long long)) return -150;

    return 0;
}

int CEvtProxy::onEventRecordData()
{
    int size,rcvSize;
    if((rcvSize=recv_int(size))<0)
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) size read error\n")),-1);;

    CRecord rec;
	rcvSize = reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->recv_n(&rec, sizeof(rec));
    if(rcvSize!=sizeof(rec)) return -2;

    size_t written = ACE_OS::fwrite(&rec,1,sizeof(rec),_fp);
    if(written!=rcvSize) return -3;

    if(_pClass->addEvt(_index,rec._device,rec._event.time.tv_sec,rec._event.time.tv_usec,
          rec._event.type, rec._event.code, rec._event.value) < 0) return -4;
    _index++;

    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T EventRecordData(%d)\n"),rcvSize));
    return 0;
}

int CEvtProxy::svc()
{
    ACE_TRACE("CEvtProxy::svc");

    int msg,rcvSize,err;
    while (1){
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T waiting to receive...\n")));
        if((rcvSize=recv_int(msg))<0)
            ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) recv error\n")),-1);;

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T %d received(0x%x)\n"),rcvSize,msg));

        switch(msg){
        case EVENT_RECORD_START:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Event Record Started(0x%x)\n"),msg));
            break;
        case EVENT_RECORD_DATA:
            onEventRecordData();
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Event Record Data received(0x%x)\n"),msg));
            break;
        case EVENT_RECORD_STOP:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Event Record Stopped(0x%x)\n"),msg));
            break;
        case EVENT_FILE_UP_PREPARE:
            recv_int(err);
            if(err==OK) _upEvt.signal();
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T ready to upload(x%x)=0x%x\n"),msg,err));
            break;
        case EVENT_FILE_UPLOAD:
            recv_int(err);
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T File uploaded(x%x)=0x%x\n"),msg,err));
            break;
        case EVENT_PLAY_FULL:
            recv_int(err);
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Play full file done(x%x)=0x%x\n"),msg,err));
            break;
        case EVENT_PLAY_PART:
            recv_int(err);
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Play part file done(x%x)=0x%x\n"),msg,err));
            break;
        case TERMINATE_CLIENT:
			reinterpret_cast<ACE_SOCK_Stream*>(_pStream)->close();
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Connection Terminated(0x%x)\n"),msg));
            return 0;
        default:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %T Undefined command(0x%x)\n"),msg));
            break;
        }
    }
    return 0;
}
