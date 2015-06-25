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

    if(argc<3) {
        ACE_DEBUG((LM_DEBUG, "argument is %d\n",argc));
        ACE_DEBUG((LM_INFO, "usage:eRcv <ip> <port>\n"));
        ACE_RETURN(-1);
    }
    const char *server_host = argv[1];
    u_short server_port = ACE_OS::atoi(argv[2]);
    ACE_DEBUG((LM_INFO, "(%P|%t) server info(addr:%s,port:%d)\n",server_host, server_port));

    CEvtProxy er;
    CClassResult classResult;
    er.notify(&classResult);

    int rtn;
    if((rtn=er.init(server_host,server_port))<0) {
        ACE_DEBUG((LM_ERROR, "init error(%d)",rtn));
        return -1;
    }
    ACE_DEBUG((LM_INFO, "init ok(%d)\n",rtn));

    er.start();

	char inpBuff[128];
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

    ACE_DEBUG((LM_INFO, ACE_TEXT("(%P|%t) end\n")));
    ACE_RETURN(0);
}
