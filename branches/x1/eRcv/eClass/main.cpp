#include "ace/Log_Msg.h" 
#include "ace/OS.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));
	ACE_LOG_MSG->priority_mask(LM_INFO|LM_ERROR|LM_DEBUG, ACE_Log_Msg::PROCESS);

	if (argc < 2) {
		ACE_DEBUG((LM_INFO, "usage:eClass <event dump file>\n"));
		ACE_RETURN(-1);
	}

	FILE* fp = ACE_OS::fopen(argv[1], ACE_TEXT("rb"));
	ACE_OS::fclose(fp);

	ACE_DEBUG((LM_INFO, "[%T] end\n"));
	ACE_RETURN(0);
}