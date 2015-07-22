#include "CSender.h"
#include <linux/input.h>
#include "ace/SOCK_Stream.h"
#include "ace/Log_Msg.h"
#include "def.h"

CSender::CSender()
:_pPeer(NULL)
{
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%P|%t) CSender constructor\n")));
}

CSender::~CSender()
{
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%P|%t) CSender destructor\n")));
}

int CSender::evtDmp(int dev,struct input_event& event)
{
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%P|%t) evtDmp called(dev:%d)\n"),dev));
    if(_pPeer)
    {
        ssize_t size;
        int recSize = 3*sizeof(int)+sizeof(event);
        int msg = EVENT_RECORD_DATA;
        if((size=_pPeer->send_n(&msg,sizeof(int)))!=sizeof(int)) return -1;
        if((size=_pPeer->send_n(&recSize,sizeof(int)))!=sizeof(int)) return -1;
        if((size=_pPeer->send_n(&dev,sizeof(int)))!=sizeof(int)) return -2;
        if((size=_pPeer->send_n(&event,sizeof(event)))!=sizeof(event)) return -3;

        ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%P|%t)dev:%02d,type:%04x,code:%04x,value:%08x\n"),
            dev,event.type,event.code,event.value));
    }
    return 0;
}
