#include "ace/Log_Msg.h" 
#include "ace/OS.h"
#include "ace/SString.h"
#include "CClassifier.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
	ACE_TRACE(ACE_TEXT("main"));
	ACE_LOG_MSG->priority_mask(LM_INFO|LM_ERROR|LM_DEBUG, ACE_Log_Msg::PROCESS);

	if (argc < 2) {
		ACE_DEBUG((LM_INFO, "usage:eClass <event dump file>\n"));
		ACE_RETURN(-1);
	}
	ACE_TString filename(argv[1]);
	ACE_DEBUG((LM_INFO, "[%T] event dump file:%s\n", filename.c_str()));
	ACE_DEBUG((LM_DEBUG, "[%T] sizeof(timeval)=%dbytes", sizeof(timeval)));
	ACE_DEBUG((LM_DEBUG, " sizeof(event)=%dbytes", sizeof(struct input_event)));
	ACE_DEBUG((LM_DEBUG, " sizeof(CRecord)=%dbytes\n", sizeof(CRecord)));

	CClassifier cf;
	CRecord rec;
	size_t size,totalRead=0;
	FILE* fp = ACE_OS::fopen(filename.c_str(), ACE_TEXT("rb"));
	if (!fp) {
		ACE_DEBUG((LM_ERROR, "%s open error\n", filename.c_str()));
		ACE_RETURN(-1);
	}

	long index = 0;
	while (1){
		size = ACE_OS::fread(&rec, 1, sizeof(rec), fp);
		if (size != sizeof(rec)) break;
		totalRead += size;
		if (cf.addEvt(index, rec._device, rec._event.time.tv_sec, rec._event.time.tv_usec, rec._event.type, rec._event.code, rec._event.value) < 0) {
			ACE_DEBUG((LM_ERROR, "[%T] error in add event\n"));
			break;
		}
		index++;
	}
	ACE_OS::fclose(fp);
	ACE_DEBUG((LM_INFO, "[%T] %dbytes, %d events read from %s\n",totalRead,totalRead/sizeof(CRecord),filename.c_str()));

	ACE_DEBUG((LM_INFO, "[%T] end\n"));
	ACE_RETURN(0);
}