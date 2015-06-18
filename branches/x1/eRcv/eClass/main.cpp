#include "ace/Log_Msg.h" 

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));
	ACE_LOG_MSG->priority_mask(LM_INFO|LM_ERROR|LM_DEBUG, ACE_Log_Msg::PROCESS);

	ACE_DEBUG((LM_INFO, "[%T] end\n"));
	ACE_RETURN(0);
}