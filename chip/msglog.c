#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef	_WIN32
#else
	#include <syslog.h>
#endif

#include "chipinterf.h"
#include "msglog.h"

void init_msglog(void) {

#ifdef	_WIN32

#else
	if (IS_FLAG_DAEMON(option.uFlags)) {
		openlog("chipinterf", LOG_NDELAY, LOG_USER);
	}
#endif

	return;
}

void close_msglog(void) {

#ifdef	_WIN32

#else
	if (IS_FLAG_DAEMON(option.uFlags)) {
		closelog();
	}
#endif

	return;
}

void log_message(const char* sFmt, ...) {
	static char	dbgBuffer[256];
	va_list		valist;

	va_start(valist,sFmt);
	vsnprintf(dbgBuffer, 256, sFmt, valist);

#ifdef	_WIN32
	OutputDebugString(dbgBuffer);
#else
	if (IS_FLAG_DAEMON(option.uFlags)) {
		syslog(LOG_INFO, dbgBuffer);
	} else {
		fprintf(stderr, dbgBuffer);
	}
#endif
	
	return;	
}

#ifdef	_WIN32

void MSWError(LPTSTR lpszFunction) { 
//	TCHAR szBuf[80]; 
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

//	wsprintf(szBuf, 
//		"%s failed with error %d: %s", 
//		lpszFunction, dw, lpMsgBuf); 

	log_message("%s failed with error %d: %s", 
				lpszFunction, dw, lpMsgBuf); 

	//	MessageBox(NULL, szBuf, "Error", MB_OK); 

	LocalFree(lpMsgBuf);
//	ExitProcess(dw); 
}
#endif
