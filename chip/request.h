#ifndef	__REQUEST_H__
#define	__REQUEST_H__

#include "simulator.h"

void   request_init(void);
RMbool request_register(hardlib_command_t* cmd, int hSocket);
RMbool request_unregister(hardlib_command_t* cmd, int hSocket);
RMbool request_request(hardlib_command_t* cmd, int hSocket, struct gbus* pGBus);

struct request_entry {
	RMuint32		request_flags;
	RMuint32		request_count;
    RMuint32		request_data_size;
	void*			request_data;
};

#endif
