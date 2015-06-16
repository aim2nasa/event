#ifndef __COMMON_H__
#define __COMMON_H__

#define EVENT_RECORD_INIT       (0xffabc000)
#define EVENT_RECORD_START      (0xffabc001)
#define EVENT_RECORD_DATA       (0xffabc002)
#define EVENT_RECORD_STOP       (0xffabc003)
#define TERMINATE_CLIENT        (0xffabcffe)
#define TERMINATE_SERVER        (0xffabcfff)

//error codes
#define OK                      (0x00000000)
#define ERROR_RECV_INT          (0xe0000001)
#define ERROR_RECV_INT_SIZE     (0xe0000002)
#define ERROR_EVTREC_NO_DEV     (0xe0000003)
#define ERROR_EVTREC_DEVOPEN    (0xe0000004)

#endif
