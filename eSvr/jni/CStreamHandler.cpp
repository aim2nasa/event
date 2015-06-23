#include "CStreamHandler.h"
#include <ace/Log_Msg.h>
#include "def.h"
#include "CEvtRec.h"
#include "CSender.h"
#include <linux/input.h>
#include "CEvtPlay.h"

class CMon : public IEvtPlay{
public:
    virtual ~CMon(){}

    int play(int order,int dev,long sec,long uSec,int type,int code,int value)
    {
        ACE_DEBUG((LM_DEBUG,"%d dev:%02d,time:%d.%06d,type:%04x,code:%04x,val:%08x\n",
            order,dev,sec,uSec,type,code,value));
        return 0;
    }
};

CStreamHandler::CStreamHandler()
: noti_(0, this, ACE_Event_Handler::WRITE_MASK),snd_(NULL),fSize_(0)
{

}

CStreamHandler::~CStreamHandler()
{
    CEvtRec::delInstance();
}

int CStreamHandler::open(void *)
{
    ACE_TRACE("Stream_Handler::open");
    if (super::open() == -1)
        return -1;

    noti_.reactor(this->reactor());
    this->msg_queue()->notification_strategy(&noti_);
    if (this->peer().get_remote_addr(remote_addr_) == 0) {
	ACE_DEBUG((LM_INFO, "(%t) Connected: %s:%u\n",
		remote_addr_.get_host_addr(), remote_addr_.get_port_number()));
        snd_ = new CSender();
        snd_->_pPeer = &peer();
        CEvtRec::instance()->evtDump(snd_);
    }
    return 0;
}

int CStreamHandler::handle_input(ACE_HANDLE handle)
{
    ACE_TRACE("Stream_Handler::handle_input");
    int msg;
    ssize_t size;
    if ((size = this->peer().recv_n(&msg,sizeof(int))) <= 0) {
	return -1;
    }
    ACE_DEBUG((LM_DEBUG, "command msg(0x%x)\n",msg));

    switch(msg){
    case EVENT_RECORD_INIT:
        ACE_DEBUG((LM_DEBUG, "Event record init command(0x%x)...\n",msg));
        send(EVENT_RECORD_INIT); //ack
        send(onEventRecordInit());
        ACE_DEBUG((LM_DEBUG, "Event record init command(0x%x) processed\n",msg));
        break; 
    case EVENT_RECORD_START:
        ACE_DEBUG((LM_DEBUG, "Event record start command(0x%x)...\n",msg));
        onEventRecordStart();
        send(EVENT_RECORD_START); //ack
        ACE_DEBUG((LM_DEBUG, "Event record start command(0x%x) processed\n",msg));
        break; 
    case EVENT_RECORD_STOP:
        ACE_DEBUG((LM_DEBUG, "Event record stop command(0x%x)...\n",msg));
        onEventRecordStop();
        send(EVENT_RECORD_STOP); //ack
        ACE_DEBUG((LM_DEBUG, "Event record stop command(0x%x) processed\n",msg));
        break; 
    case EVENT_FILE_UP_PREPARE:
        ACE_DEBUG((LM_DEBUG, "Event file upload prepare command(0x%x)...\n",msg));
        send(EVENT_FILE_UP_PREPARE); //ack
        send(onEventFileUpPrepare());
        ACE_DEBUG((LM_DEBUG, "Event file upload prepare command(0x%x) processed\n",msg));
        break; 
    case EVENT_FILE_UPLOAD:
        ACE_DEBUG((LM_DEBUG, "Event file upload command(0x%x)...\n",msg));
        send(EVENT_FILE_UPLOAD); //ack
        send(onEventFileUpload());
        ACE_DEBUG((LM_DEBUG, "Event file upload command(0x%x) processed\n",msg));
        break; 
    case EVENT_PLAY_FULL:
        ACE_DEBUG((LM_DEBUG, "Event Play full command(0x%x)...\n",msg));
        send(EVENT_PLAY_FULL); //ack
        send(onEventPlayFull());
        ACE_DEBUG((LM_DEBUG, "Event Play full command(0x%x) processed\n",msg));
        break; 
    case EVENT_PLAY_PART:
        ACE_DEBUG((LM_DEBUG, "Event Play part command(0x%x)...\n",msg));
        send(EVENT_PLAY_PART); //ack
        send(onEventPlayPart());
        ACE_DEBUG((LM_DEBUG, "Event Play part command(0x%x) processed\n",msg));
        break; 
    case TERMINATE_SERVER:
        ACE_DEBUG((LM_DEBUG, "Terminate server command(0x%x)...\n",msg));
        send(TERMINATE_CLIENT);
        ACE_DEBUG((LM_DEBUG, "Terminate server command(0x%x) processed\n",msg));
        break; 
    default:
        ACE_DEBUG((LM_DEBUG, "Undefined command(0x%x)\n",msg));
        break; 
    }

    ACE_DEBUG((LM_INFO, "(%t) Stream_Handler::handle_input received(%d),msg(0x%x)\n",size,msg));
    return 0;
}

int CStreamHandler::handle_output(ACE_HANDLE handle)
{
    ACE_TRACE("Stream_Handler::handle_output");
    ACE_Message_Block *mb;
    ACE_Time_Value nowait(ACE_OS::gettimeofday());
    while (this->getq(mb, &nowait) != -1)
    {
        ssize_t send_cnt = this->peer().send(mb->rd_ptr(), mb->length());
	if (send_cnt == -1)
   	    ACE_ERROR((LM_ERROR, "[ERROR%T](%N:%l) - %p\n","Stream_Handler::handle_output"));
	else {
            mb->rd_ptr(send_cnt);
	}

	if (mb->length() > 0)
	{
	    this->ungetq(mb);
	    break;
	}
	mb->release();
    }

    if (this->msg_queue()->is_empty())
        this->reactor()->cancel_wakeup(this, ACE_Event_Handler::WRITE_MASK);
    else
        this->reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);

    return 0;
}

int CStreamHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
    ACE_TRACE("Stream_Handler::handle_close");
    delete snd_;
    ACE_DEBUG((LM_INFO, "(%t) Close connection %s:%u\n",
        remote_addr_.get_host_addr(), remote_addr_.get_port_number()));
    return super::handle_close(handle, close_mask);
}

int CStreamHandler::send(int msg)
{
    ssize_t rcvSize = this->peer().send_n(&msg,sizeof(int));
    if (rcvSize <= 0)
        ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) send error(0x%x)\n"),msg),-1);
    return 0;
}

int CStreamHandler::recv_int(int& msg)
{
    ssize_t size;
    if((size = this->peer().recv_n(&msg,sizeof(int))) <= 0)
	return -1;
    return size;
}

int CStreamHandler::onEventRecordInit()
{
    ACE_TRACE("onEventRecordInit");

    int bytes;
    if(recv_int(bytes)<0) return ERROR_RECV_INT_SIZE;

    ACE_DEBUG((LM_DEBUG,"msg:"));
    for(int i=0;i<bytes;i++) {
        int msg;
        if(recv_int(msg)<0) return ERROR_RECV_INT;
     
        CEvtRec::instance()->addDevice(msg);
        ACE_DEBUG((LM_DEBUG," %d ",msg));
    }
    ACE_DEBUG((LM_DEBUG,"\n"));
    ACE_DEBUG((LM_DEBUG,"CEvtRec devices:%d\n",CEvtRec::instance()->devices()));

    if(CEvtRec::instance()->devices()<=0) return ERROR_EVTREC_NO_DEV;
    return 0;
}

int CStreamHandler::onEventRecordStart()
{
    if(!CEvtRec::instance()->devOpen()) return ERROR_EVTREC_DEVOPEN;
    return CEvtRec::instance()->start();
}

int CStreamHandler::onEventRecordStop()
{
    CEvtRec::instance()->stop();
    CEvtRec::instance()->wait();
    CEvtRec::instance()->devClose();
    return 0;
}

int CStreamHandler::onEventFileUpPrepare()
{
    ACE_TRACE("onEventFileUpPrepare");

    int bytes;
    if(recv_int(bytes)<0) return ERROR_RECV_INT_SIZE;

    ssize_t size;
    if((size = this->peer().recv_n(buffer_,bytes))!=bytes) return ERROR_RECEIVE;
    buffer_[bytes]=0;
    fName_ = buffer_;

    long fSize;
    if((size = this->peer().recv_n(&fSize,sizeof(long)))!=sizeof(long)) return ERROR_RECEIVE;
    fSize_ = fSize;

    ACE_DEBUG((LM_DEBUG,"Upload file:%s size:%dbytes\n",fName_.c_str(),fSize_));
    return 0;
}

int CStreamHandler::onEventFileUpload()
{
    ACE_TRACE("onEventFileUpload");

    int bytes;
    if(recv_int(bytes)<0) return ERROR_RECV_INT_SIZE;

    ACE_TString dumpFile("_");
    dumpFile+=fName_;
    
    FILE *fp = ACE_OS::fopen(dumpFile.c_str(),ACE_TEXT("wb"));
    size_t readUnit = sizeof(int)+sizeof(struct input_event),totalRead=0,totalWrite=0,size;
    while(totalRead<bytes){
        if((size = this->peer().recv_n(buffer_,readUnit))!=readUnit) break;
        totalRead += size;
 
        size = ACE_OS::fwrite(buffer_,1,readUnit,fp);
        if(size!=readUnit) break;
        totalWrite += size;
    }
    ACE_OS::fclose(fp);

    ACE_DEBUG((LM_DEBUG,"%dbytes read,wrote down %dbytes to %s\n",
        totalRead,totalWrite,fName_.c_str()));
    return 0;
}

int CStreamHandler::onEventPlayFull()
{
    ACE_TRACE("onEventPlayFull");

    int bytes;
    if(recv_int(bytes)<0) return ERROR_RECV_INT_SIZE;

    ssize_t size;
    if((size = this->peer().recv_n(buffer_,bytes))!=bytes) return ERROR_RECEIVE;
    buffer_[bytes]=0;

    ACE_TString fname("_");
    fname += buffer_;
    ACE_DEBUG((LM_DEBUG,"play request %s\n",fname.c_str()));

    CMon m;
    CEvtPlay ep;

    int rtn = -1;
    ACE_DEBUG((LM_DEBUG,"initializing event player with (%s)\n",fname.c_str()));
    if((rtn=ep.init(fname.c_str(),&m))!=0) return -1;
    ACE_DEBUG((LM_DEBUG,"play(%s) total events(%d)\n",fname.c_str(),ep.events()));

    ep.play(0,ep.events()-1);
    ACE_DEBUG((LM_DEBUG,"Playing full file(%s) done\n",fname.c_str()));
    return 0;
}

int CStreamHandler::onEventPlayPart()
{
    ACE_TRACE("onEventPlayPart");

    int bytes;
    if(recv_int(bytes)<0) return ERROR_RECV_INT_SIZE;

    ssize_t size;
    if((size = this->peer().recv_n(buffer_,bytes))!=bytes) return ERROR_RECEIVE;
    buffer_[bytes]=0;

    ACE_TString fname("_");
    fname += buffer_;
    ACE_DEBUG((LM_DEBUG,"play request %s\n",fname.c_str()));

    long long loc[2];
    for(int i=0;i<2;i++) {
        if((size = this->peer().recv_n(&loc[i],sizeof(long long)))!=sizeof(long long))
            return ERROR_RECEIVE;
    }

    CMon m;
    CEvtPlay ep;

    int rtn = -1;
    ACE_DEBUG((LM_DEBUG,"init event player with (%s)\n",fname.c_str()));
    if((rtn=ep.init(fname.c_str(),&m))!=0) return -1;
    ACE_DEBUG((LM_DEBUG,"play(%s) (%Q~%Q) events(%Q/%Q)\n",
        fname.c_str(),loc[0],loc[1],loc[1]-loc[0]+1,ep.events()));

    if((rtn=ep.play(loc[0],loc[1]))!=0) { 
        ACE_DEBUG((LM_ERROR,"Playing file(%s) part(%Q~%Q) error(%d)\n",
            fname.c_str(),loc[0],loc[1],rtn));
        return -1;
    }
    ACE_DEBUG((LM_DEBUG,"Playing file(%s) part(%Q~%Q)done\n",fname.c_str(),loc[0],loc[1]));
    return 0;
}
