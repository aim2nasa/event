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

int CEvtRcv::svc()
{
    ACE_TRACE("CEvtRcv::svc");

    int msg;
    char buf[BUFSIZ];
    while (1){
        ssize_t rcvSize = _pStream->recv_n(&msg,sizeof(int));
	if (rcvSize <= 0) {
	    ACE_DEBUG((LM_ERROR, ACE_TEXT("(%P|%t) error in receiving msg\n")));
	    return -1;
	}

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
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Terminate(0x%x)\n"),msg));
            return 0;
        default:
	    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Undefined command(0x%x)\n"),msg));
            break;
        }
    }
    return 0;
}
