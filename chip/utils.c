
#include "StdAfx.h"
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

#define ALLOW_OS_CODE 1
#include "llad/include/gbus.h"
#include "rmdef/rmdef.h"
#include "simulator.h"
#include "interface.h"
#include "chipinterf.h"
#include "msglog.h"
#include "utils.h"

int sendall (int s, const void *msg, size_t len, int flags) {
	int total = 0;

	while (total < (int) len) {
		/* Receive some bytes */
		int res = send (s, (char *)msg + total, len - total, flags);

		/* Check for an error */
		if (res <= 0) {
			log_message("No data has been sent\n");
			return -1;
		}

		/* Update the number of received bytes */
		total += res;
	}

	return total;
}



int recvall (int s, void *buf, size_t len, int flags) {
	int total = 0;

	while (total < (int) len) {
		/* Receive some bytes */
		int res = recv (s, (char *)buf + total, len - total, flags);

		/* Check for an error */
		if (res <= 0) {
			log_message("No data has been received\n");
			return -1;
		}

		/* Update the number of received bytes */
		total += res;
	}

	return total;
}
