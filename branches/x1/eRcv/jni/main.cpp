#include "ace/SOCK_Connector.h" 
#include "ace/Log_Msg.h" 
#include "ace/OS.h"
#include "CEvtRcv.h"
#include <iostream>

#define EVENT_RECORD_START	(0xffabc001)
#define EVENT_RECORD_STOP	(0xffabc002)
#define TERMINATE_SERVER        (0xffabcfff)

static char* SERVER_HOST = "127.0.0.1";
static u_short SERVER_PORT = 25252;

ssize_t sendCmd(ACE_SOCK_Stream& stream, unsigned int cmdCode);

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
    ACE_TRACE(ACE_TEXT("main"));

    ACE_LOG_MSG->priority_mask( LM_INFO|LM_ERROR, ACE_Log_Msg::PROCESS);

    const char *server_host = argc > 1 ? argv[1] : SERVER_HOST;
    u_short server_port = argc > 2 ? ACE_OS::atoi(argv[2]) : SERVER_PORT;

    ACE_DEBUG((LM_INFO, "(%P|%t) server info(addr:%s,port:%d)\n",server_host, server_port));

    ACE_SOCK_Stream client_stream;
    ACE_INET_Addr remote_addr(server_port, server_host);
    ACE_SOCK_Connector connector;

    ACE_DEBUG((LM_INFO, "(%P|%t) Starting connect to %s: %d \n",
        remote_addr.get_host_name(), remote_addr.get_port_number()));

    if (connector.connect(client_stream, remote_addr) == -1)
        ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "connection failed"), -1);
    else
	ACE_DEBUG((LM_INFO, "(%P|%t) connected to %s \n", remote_addr.get_host_name()));

    ACE_OS::sleep(2);

    //연결이 제대로 되는지 검사 테스트
    ACE_DEBUG((LM_INFO, "(%P|%t) Connection testing...\n"));
    char inpBuff[128];
    ACE_Time_Value timeout(1);
    ssize_t result = client_stream.recv(inpBuff, sizeof(inpBuff),&timeout);
    if (result == -1 && ACE_OS::last_error()==ETIME)
        ACE_DEBUG((LM_INFO, "(%P|%t) Connection test ok\n"));
    else
        ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "Connection test failed"), -1);

    CEvtRcv er(&client_stream);
    er.activate();

    bool bRun = true;
    while (bRun)
    {
        ACE_DEBUG((LM_INFO,
            "\n<-Menu: Record Start[r],Record Stop:[t],Terminate Server[x]->\n"));
	std::cin >> inpBuff;
	std::cout << "Choice:" << inpBuff << std::endl;

	switch (inpBuff[0])
	{
	case 'r':
            //Record start
            if(sendCmd(client_stream,EVENT_RECORD_START)!=sizeof(unsigned int))
	        ACE_ERROR_RETURN((LM_ERROR,"(%P|%t) %p\n","error sendCmd(EVENT_RECORD_START)"),-1);
	    ACE_DEBUG((LM_INFO, "(%P|%t) EVENT Record start\n"));
	    break;
	case 't':
            //Record stop
            if(sendCmd(client_stream,EVENT_RECORD_STOP)!=sizeof(unsigned int))
	        ACE_ERROR_RETURN((LM_ERROR,"(%P|%t) %p\n","error sendCmd(EVENT_RECORD_STOP)"),-1);
	    ACE_DEBUG((LM_INFO, "(%P|%t) EVENT Record stop\n"));
	    break;
	case 'x':
            //Terminate
	    bRun = false;
            sendCmd(client_stream,TERMINATE_SERVER);
	    ACE_DEBUG((LM_INFO, "(%P|%t) terminate server\n"));
	    break;
	default:
	    break;
	}
    }

    if (client_stream.close() == -1)
        ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "close"), -1);

    er.wait();

    ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) end\n")));
    ACE_RETURN(0);
}

ssize_t sendCmd(ACE_SOCK_Stream& stream,unsigned int cmdCode)
{
    ssize_t nSent = stream.send_n(&cmdCode, sizeof(unsigned int));
    ACE_ASSERT(nSent == sizeof(unsigned int));
    return nSent;
}
