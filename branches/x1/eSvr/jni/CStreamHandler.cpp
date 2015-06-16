#include "CStreamHandler.h"
#include <ace/Log_Msg.h>
#include "../../common/def.h"

CStreamHandler::CStreamHandler()
: noti_(0, this, ACE_Event_Handler::WRITE_MASK)
{

}

CStreamHandler::~CStreamHandler()
{

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
    case EVENT_RECORD_START:
        ACE_DEBUG((LM_DEBUG, "Event record start command(0x%x)\n",msg));
        break; 
    case EVENT_RECORD_STOP:
        ACE_DEBUG((LM_DEBUG, "Event record stop command(0x%x)\n",msg));
        break; 
    case TERMINATE_SERVER:
        send(TERMINATE_CLIENT);
        ACE_DEBUG((LM_DEBUG, "Terminate server command(0x%x)\n",msg));
        break; 
    default:
        ACE_DEBUG((LM_DEBUG, "Undefined command(0x%x)\n",msg));
        break; 
    }

    ACE_DEBUG((LM_INFO, "(%t) Stream_Handler::handle_input received(%d)\n",size));
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
