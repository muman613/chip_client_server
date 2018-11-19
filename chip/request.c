//-----------------------------------------------------------------------------
//	MODULE		:	request.c
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman
//	DATE		:	Mar 10, 2006
//	LAST MOD	:	$Date: 2006-07-28 02:05:41 $
//	DESCRIPTION	:	
//-----------------------------------------------------------------------------

#include "StdAfx.h"

#ifdef	ENABLE_REQUEST

#include <stdio.h>
#ifdef _WIN32
	#include <io.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>

#define ALLOW_OS_CODE 1
#include "llad/include/gbus.h"
#include "rmdef/rmdef.h"
#include "simulator.h"
#include "interface.h"
#include "chipinterf.h"
#include "msglog.h"
#include "chipdb.h"
#include "request.h"
#include "version.h"
#include "utils.h"

#define MAX_ENTRY	32
                  
struct request_entry	RequestEntryTable[MAX_ENTRY];

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void request_init(void) {
	int i;

	for (i = 0 ; i < MAX_ENTRY ; i++) {
    	memset(&RequestEntryTable[i], 0, sizeof(struct request_entry));
	}
	
	return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
	
static int find_free_request() {
	int i;
	for (i = 0 ; i < MAX_ENTRY ; i++) {
    	if (RequestEntryTable[i].request_flags == 0) {
        	return i;
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RMbool request_register(hardlib_command_t* cmd, int hSocket) {
	RMuint32		requestID	= find_free_request();
	RMbool			result		= FALSE;
	
//	log_message("request_register(%08lx, %ld)\n", cmd, hSocket);
//	log_message("Using request ID %ld\n", requestID);

    /* If a request is available, mark entry and allocate space for data */
    if (requestID != (RMuint32)-1) {
        RMuint32*		pData = 0L;
		RMuint32		dataCnt = cmd->nbytes / 4;
		RMuint32		i;
		        
     	RequestEntryTable[requestID].request_flags 		|= 0x80000000;
    	RequestEntryTable[requestID].request_data_size 	= dataCnt;

     	pData = malloc(cmd->nbytes);
		assert(pData != 0L);
		
      	recvall(hSocket, pData, cmd->nbytes, 0);

     	for (i = 0 ; i < dataCnt ; i++) {
			pData[i] = ntohl(pData[i]);
		}

		RequestEntryTable[requestID].request_data = pData;
				  
		result = TRUE; 
	}

	requestID = htonl(requestID);

    /* write the result back to the socket */
   	write(hSocket, &requestID, sizeof(RMuint32));
   
	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

RMbool request_unregister(hardlib_command_t* cmd, int hSocket) {
	RMuint32		requestID = cmd->address;

//	log_message("request_unregister(%08lx, %ld)\n", cmd, hSocket);

	if ((requestID >= 0) && (requestID <= MAX_ENTRY)) {

        if (RequestEntryTable[requestID].request_data != 0)
	        free(RequestEntryTable[requestID].request_data);

      	memset(&RequestEntryTable[requestID], 0, sizeof(struct request_entry));

      	return TRUE;
	}
	
	return FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
	
RMbool request_request(hardlib_command_t* cmd, int hSocket, struct gbus *pGBus) {
	RMuint32	requestID = cmd->address;
	RMuint32	regCount = 0,
				data = 0,
				x;
	RMuint32*	regData = 0L;
	RMuint32*	regList = 0L;

//	log_message("request_request() id = %ld\n", requestID);

	if ((requestID >= 0) &&
		(requestID <= MAX_ENTRY) &&
		(RequestEntryTable[requestID].request_flags & 0x80000000))
	{
		regCount = RequestEntryTable[requestID].request_data_size;

		regData = malloc(regCount * sizeof(RMuint32));
		regList = RequestEntryTable[requestID].request_data;

		data = htonl(regCount);

		/* send count of registers */
		sendall(hSocket, &data, sizeof(RMuint32), 0);

		for (x = 0 ; x < regCount ; x++) {
//			log_message("Reading 0x%08lx\n", regList[x]);
			regData[x] = htonl(gbus_read_uint32(pGBus, regList[x]));
		}

		sendall(hSocket, regData, regCount * sizeof(RMuint32), 0);

		free(regData);

		RequestEntryTable[requestID].request_count++;
	} else {
		log_message("Invalid request handle [%ld]!\n", requestID);
		sendall(hSocket, &data, sizeof(RMuint32), 0); /* reply with 0 registers */
	}
	
	return FALSE;
}

//-----------------------------------------------------------------------------

#endif	//	ENABLE_REQUEST
