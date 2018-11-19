//-----------------------------------------------------------------------------
//	MODULE		:	chipinterf.h
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman, Damien Vincent
//	DATE		:	Jul 14, 2005
//	LAST MOD	:	$Date: 2007-11-13 03:28:28 $
//	DESCRIPTION	:	Define preprocessor symbols and structures for chip
//					interface.
//-----------------------------------------------------------------------------

#ifndef	__CHIPINTERF_H__
#define __CHIPINTERF_H__


#define ALLOW_OS_CODE 1
#include "rmdef/rmdef.h"
#include "llad/include/gbus.h"

#define FLAG_DEBUG_READ		0x0001
#define FLAG_DEBUG_WRITE	0x0002
#define	FLAG_INIT			0x0004
#define FLAG_DAEMON			0x0008
#define FLAG_KILL			0x0010

#define IS_DEBUG_READ(x)			((x & FLAG_DEBUG_READ) != 0)
#define IS_DEBUG_WRITE(x)			((x & FLAG_DEBUG_WRITE) != 0)
#define IS_FLAG_INIT(x)				((x & FLAG_INIT) != 0)
#define IS_FLAG_DAEMON(x)			((x & FLAG_DAEMON) != 0)
#define IS_FLAG_KILL(x)				((x & FLAG_KILL) != 0)

typedef struct {
	unsigned short	nPort;
	unsigned short	uFlags;
	const char*		szCmd;
	int				child_pid;
} t_options;


extern t_options option;

/** Return the time in microseconds */
RMuint64 RMGetTimeInMicroSeconds (void);
/** Return the speed of the chip */
RMuint32 GetNbCyclesPerSec (struct gbus *pgbus);

void chip_init (struct gbus *pgbus);
void chip_checkmem (struct gbus *pgbus);
void chip_displayinfo (struct gbus *pgbus);

void parseArguments(int argc, char** argv);
void displayVersionNumber(void);

#ifdef	_WIN32
int main_msw(int argc, char* argv[]);
#else
int main_linux(int argc, char* argv[]);
#endif

#endif	// __CHIPINTERF_H__


