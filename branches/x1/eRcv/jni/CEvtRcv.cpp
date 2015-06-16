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

int CEvtRcv::svc()
{
    ACE_TRACE("CEvtRcv::svc");

    int msg;
    char buf[BUFSIZ];
    while (1){
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) waiting to receive...\n")));
        ssize_t rcvSize = _pStream->recv_n(&msg,sizeof(int));
	if (rcvSize <= 0) {
	    ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) error in receiving msg\n")));
	    return -1;
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) %d received\n"),rcvSize));

        switch(msg){
        case EVENT_RECORD_START:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Start(0x%x)\n"),msg));
            break;
        case EVENT_RECORD_DATA:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Data(0x%x)\n"),msg));
            break;
        case EVENT_RECORD_STOP:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Event Record Stop(0x%x)\n"),msg));
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
