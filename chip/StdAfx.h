#ifndef	__STDAFX_H__
#define __STDAFX_H__

#define ENABLE_REQUEST		1

#include <stdio.h>
#include <stdlib.h>

#ifdef	_WIN32
	#include <windows.h>
//	#include <winsock2.h>
	#define snprintf _snprintf
	#define vsnprintf _vsnprintf

	#define	socket_close(fh) closesocket(fh)
	typedef size_t socklen_t;

#else
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <sys/wait.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <popt.h>
	#include "daemon.h"

	#define socket_close(fh) close(fh)
	#define TEXT(x) (x)

#endif

#include "chipinterf.h"
#include "chipdb.h"
#include "interface.h"
#include "version.h"
#include "msglog.h"

#endif	// __STDAFX_H__

