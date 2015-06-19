#ifndef __CSTREAMHANDLER_H__
#define __CSTREAMHANDLER_H__

#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#include <ace/Reactor_Notification_Strategy.h>
#include <ace/Svc_Handler.h>

class CSender;

class CStreamHandler : public ACE_Svc_Handler < ACE_SOCK_STREAM, ACE_MT_SYNCH > {
public:
    CStreamHandler();
    ~CStreamHandler();

    virtual int open(void * = 0);
    virtual int handle_input(ACE_HANDLE handle = ACE_INVALID_HANDLE);
    virtual int handle_output(ACE_HANDLE handle = ACE_INVALID_HANDLE);
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

    int send(int msg);
    int recv_int(int& msg);

    int onEventRecordInit();
    int onEventRecordStart();
    int onEventRecordStop();
    int onEventFileUpPrepare();
    int onEventFileUpload();
    int onEventPlayFull();

private:
    typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> super;

    ACE_INET_Addr remote_addr_;
    ACE_Reactor_Notification_Strategy noti_;
    CSender *snd_;
    char buffer_[BUFSIZ];
    ACE_TString fName_;
    long fSize_;
};

#endif