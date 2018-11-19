#ifndef	__MSGLOG_H__
#define __MSGLOG_H__

void init_msglog(void);
void log_message(const char* sFmt, ...);
void close_msglog(void);

#ifdef	_WIN32
void MSWError(LPTSTR szFunc);
#endif

#endif	// __MSGLOG_H__
