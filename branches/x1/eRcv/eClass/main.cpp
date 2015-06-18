#include "ace/Log_Msg.h" 
#include "ace/OS.h"
#include "ace/SString.h"
#include "CClassifier.h"

typedef unsigned short _u16;
typedef unsigned int _u32;
typedef short _s16;
typedef int _s32;

struct input_event {
	struct timeval time;
	_u16 type;
	_u16 code;
	_s32 value;
};

struct record {
	_u32 device;
	struct input_event event;
};

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
	ACE_DEBUG((LM_DEBUG, " sizeof(record)=%dbytes\n", sizeof(struct record)));

	CClassifier cf;
	struct record rec;
	size_t size,totalRead=0;
	FILE* fp = ACE_OS::fopen(filename.c_str(), ACE_TEXT("rb"));
	while (1){
		size = ACE_OS::fread(&rec, 1, sizeof(struct record), fp);
		if (size != sizeof(struct record)) break;
		totalRead += size;
		if (cf.addEvt(rec.device, rec.event.time.tv_sec, rec.event.time.tv_usec, rec.event.type, rec.event.code, rec.event.value) < 0) {
			ACE_DEBUG((LM_ERROR, "[%T] error in add event\n"));
			break;
		}
	}
	ACE_OS::fclose(fp);
	ACE_DEBUG((LM_INFO, "[%T] %dbytes read from %s\n",totalRead,filename.c_str()));

	ACE_DEBUG((LM_INFO, "[%T] end\n"));
	ACE_RETURN(0);
}