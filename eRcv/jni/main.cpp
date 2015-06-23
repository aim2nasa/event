#include "ace/SOCK_Connector.h" 
#include "ace/Log_Msg.h" 
#include "ace/OS.h"
#include "CEvtProxy.h"
#include <iostream>
#include "def.h"
#include <list>
#include "CClassResult.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
    ACE_TRACE(ACE_TEXT("main"));

    ACE_LOG_MSG->priority_mask( LM_INFO|LM_ERROR|LM_DEBUG, ACE_Log_Msg::PROCESS);

    if(argc<4) {
        ACE_DEBUG((LM_DEBUG, "argument is %d\n",argc));
        ACE_DEBUG((LM_INFO, "usage:eRcv <ip> <port> <event#> ... <event#>\n"));
        ACE_RETURN(-1);
    }
    const char *server_host = argv[1];
    u_short server_port = ACE_OS::atoi(argv[2]);
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

    CEvtProxy er(&client_stream);
    CClassResult classResult;
    er.notify(&classResult);

    std::list<int>& evtNos = er.devList();
    for(int i=3;i<argc;i++) evtNos.push_back(atoi(argv[i]));

    ACE_DEBUG((LM_INFO, "event#: "));
    for(std::list<int>::iterator it=evtNos.begin();it!=evtNos.end();it++)
        ACE_DEBUG((LM_INFO, " %d ",*it));
    ACE_DEBUG((LM_INFO, "\n%d event# read\n",evtNos.size()));

    int rtn;
    if((rtn=er.init())<0) {
        ACE_DEBUG((LM_ERROR, "init error(%d)",rtn));
        return -1;
    }
    ACE_DEBUG((LM_INFO, "init ok(%d)\n",rtn));

    ACE_ASSERT(rtn==(evtNos.size()+2)); //include message,size
    er.start();

    ACE_TString filename;
    bool bRun = true;
    while (bRun)
    {
        ACE_DEBUG((LM_INFO,
            "\n<-Menu: Record[r],Stop[t],Play[p],PartPlay[i],Close[x]->\n"));
	std::cin >> inpBuff;
	std::cout << "Choice:" << inpBuff << std::endl;

	switch (inpBuff[0])
	{
	case 'r':
            //Record start
            if(er.recordStart()<0)
                ACE_ERROR_RETURN((LM_ERROR,"(%P|%t) %p\n","error recordStart"),-1); ;
	    ACE_DEBUG((LM_INFO, "(%P|%t) EVENT Record start\n"));
	    break;
	case 't':
            //Record stop
            if(er.recordStop()<0)
                ACE_ERROR_RETURN((LM_ERROR,"(%P|%t) %p\n","error recordStop"),-1); ;
	    ACE_DEBUG((LM_INFO, "(%P|%t) EVENT Record stop\n"));
	    break;
        case 'p':
            std::cout<<"filename:";
	    std::cin >> inpBuff;
	    std::cout << "Given filename:" << inpBuff << std::endl;
            int rtn; 
            if((rtn=er.play(inpBuff))!=0) {
	        ACE_DEBUG((LM_ERROR, "(%P|%t) Play error(%d)\n",rtn));
                return -1;
            }
	    ACE_DEBUG((LM_INFO, "(%P|%t) EVENT Play\n"));
            break;
        case 'i':
            std::cout<<"filename:";
	    std::cin >> inpBuff;
            filename=inpBuff;
	    std::cout << "Given filename:" << filename.c_str() << std::endl;
            long long loc[2];
            for(int i=0;i<2;i++){
                std::cout<<"index["<<i<<"]";
	        std::cin >> inpBuff;
                loc[i] = ACE_OS::atol(inpBuff);
            }
	    std::cout << "start index:"<<loc[0]<<",end index:" <<loc[1]<< std::endl;

            if((rtn=er.play(filename.c_str(),loc[0],loc[1]))!=0) {
	        ACE_DEBUG((LM_ERROR, "(%P|%t) Play error(%d)\n",rtn));
                return -1;
            }
	    ACE_DEBUG((LM_INFO, "(%P|%t) EVENT Play Part\n"));
            break;
	case 'x':
            //Terminate
	    bRun = false;
	    ACE_DEBUG((LM_INFO, "(%P|%t) terminating...\n"));
            er.stop();
	    break;
	default:
	    break;
	}
    }

    ACE_DEBUG((LM_INFO, "(%P|%t) waiting...\n"));
    er.wait();
    ACE_DEBUG((LM_INFO, "(%P|%t) waiting done\n"));

    if (client_stream.close() == -1)
        ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p \n", "close"), -1);

    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) end\n")));
    ACE_RETURN(0);
}
