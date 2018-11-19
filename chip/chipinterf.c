//-----------------------------------------------------------------------------
//	MODULE		:	chipinterf.c
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman, Damien Vincent
//	DATE		:	Jul 14, 2005
//	LAST MOD	:	$Date: 2008-11-10 21:43:39 $
//	DESCRIPTION	:	Provide TCP/IP port for connection to debugger.
//-----------------------------------------------------------------------------

#define ALLOW_OS_CODE	1

#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef	_WIN32

#else
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <popt.h>
#endif


#include "chipinterf.h"
#include "interface.h"
#include "daemon.h"
#include "version.h"
#include "msglog.h"
#include "chipdb.h"
#include "request.h"

#define DRAM_CONTROLLER0_REGS 	0x30000
#define ENABLE_SIG_HANDLER		1

t_options	option = {
	5000,			// Port
	0,				// Flags
	0,
	0,
};

int nDebugFlags = 0;

#ifdef	_WIN32

SERVICE_STATUS			ServiceStatus;
SERVICE_STATUS_HANDLE	hStatus;

void WINAPI chipinterface_main(int argc, char* argv[]);
void WINAPI chipinterface_handler(DWORD dwControl);

#else	// _WIN32

struct poptOption poptOptions[] = {
	{ "port", 	'p', POPT_ARG_INT, &option.nPort, 0, "Port to listen", "Port # (5000)" },
//	{ "chip", 	'c', POPT_ARG_INT, &option.nChipID, 0, "Chip to open", "Chip # (0)" },
	{ "debug", 	'd', POPT_ARG_INT, &nDebugFlags, 0, "Debug flags", "(1) debug read (2) debug write" },
	{ "cmd",	'e', POPT_ARG_STRING, &option.szCmd, 0, "Command to launch", },
	{ "init",	'i', POPT_ARG_NONE, NULL, 'i', "Initialize hardware", },
	{ "daemon",	'D', POPT_ARG_NONE, NULL, 'D', "Run as daemon", },
	{ "version", 0, POPT_ARG_NONE, NULL, 'V', "Display version #", },

	{ "kill",	'k', POPT_ARG_NONE, NULL, 'k', "Kill running interface", },

	POPT_AUTOHELP
	POPT_TABLEEND
};

void signal_handler(int signum);

#endif	// _WIN32

RMuint64 RMGetTimeInMicroSeconds (void) {
#ifdef	_WIN32
	return (RMuint64)0;
#else	// _WIN32
	static RMbool firsttimehere = TRUE;
	static struct timeval timeorigin;

	struct timeval now;

	// the first time, set the origin.
	if (firsttimehere) {
		gettimeofday (&timeorigin, NULL);
		firsttimehere = FALSE;
	}

	gettimeofday (&now, NULL);

	return (RMuint64) (((RMint64) now.tv_sec -
	                    (RMint64) timeorigin.tv_sec) * 1000000LL +
	                   ((RMint64) now.tv_usec - (RMint64) timeorigin.tv_usec));
#endif	// _WIN32
}


RMuint32 GetNbCyclesPerSec (struct gbus * pgbus) {
	RMuint64 ui64InitTime;
	RMuint32 ui32InitReg, ui32EndReg;

	ui32InitReg = gbus_read_uint32 (pgbus, 0x10000 + 0x40);

	ui64InitTime = RMGetTimeInMicroSeconds ();

	// Test only 1/2second
	while ((RMGetTimeInMicroSeconds () - ui64InitTime) < 500000)
		;

	ui32EndReg = gbus_read_uint32 (pgbus, 0x10000 + 0x40);

	// Return value for 1 full second
	return (ui32EndReg - ui32InitReg) * 2;
}



void chip_checkmem (struct gbus *pgbus) {
	unsigned int i;
	unsigned int baseaddr = 0x10060000;

	for (i = 0; i < 128; i++)
		gbus_write_uint32 (pgbus, baseaddr + 4 * i, 0x12345678);

	for (i = 0; i < 128; i++)
	{
		unsigned int tmp = gbus_read_uint32 (pgbus, baseaddr + 4 * i);
		;
		if (tmp != 0x12345678)
			log_message("Error at %d : 0x%X  0x%X\n", i, tmp, i);
	}
}


void
chip_displayinfo (struct gbus *pgbus)
{
	/* System frequency */
	log_message("INFO : System frequency is %d MHz\n",
	         (int) ((GetNbCyclesPerSec (pgbus) + 1000000) / 1000000L));

	/* Check memory */
//	chip_checkmem (pgbus);
//	log_message("INFO : Memory check done\n");
}


void
chip_init (struct gbus *pgbus)
{
	/* Set up DRAM controler 0 */
	gbus_write_uint32 (pgbus, DRAM_CONTROLLER0_REGS + 0xFFFC, 3);
	gbus_write_uint32 (pgbus, DRAM_CONTROLLER0_REGS + 0xFFFC, 2);
	gbus_write_uint32 (pgbus, DRAM_CONTROLLER0_REGS + 0x0000, 0xe34000b8);	/* 0xE33000BC */
	gbus_write_uint32 (pgbus, DRAM_CONTROLLER0_REGS + 0x0004, 0x00093333);	/* 0x63333    */
	gbus_write_uint32 (pgbus, DRAM_CONTROLLER0_REGS + 0xFFFC, 0);
}

#ifndef	_WIN32
void parseArguments(int argc, char** argv) {
	poptContext	con;
	char		rc;

	con = poptGetContext("chipinterf", argc, (const char**)argv, poptOptions, 0);

	while ((rc = poptGetNextOpt(con)) != (char)-1) {
//		printf("rc = %d\n", (int)rc);

		switch (rc) {
			case 'V':
				displayVersionNumber();
				poptFreeContext(con);
				exit(0);
				
			case 'D':
				option.uFlags |= FLAG_DAEMON;
				break;
				
			case 'i':
				option.uFlags |= FLAG_INIT;
				break;
			case 'k':
				option.uFlags |= FLAG_KILL;
				break;
		}
	}

#ifdef	_DEBUG
	printf( "Port = %d\n", option.nPort);
#endif

	poptFreeContext(con);
	
	option.uFlags |= (nDebugFlags & 0x00000003);
	return;
}
#endif

#ifdef	_WIN32

#ifdef WINDOWS_SERVICE

void main(void) {
	SERVICE_TABLE_ENTRY		ServiceTable[2];

	init_msglog();

	ServiceTable[0].lpServiceName = "rmchipinterface";
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)chipinterface_main;

	ServiceTable[1].lpServiceName = 0L;
	ServiceTable[1].lpServiceProc = 0L;

	if (!StartServiceCtrlDispatcher(ServiceTable)) {
		log_message("ERROR: Unable to start service!\n");
	}
	
	log_message("OK\n");

	close_msglog();

	return;
}

void WINAPI chipinterface_main(int argc, char* argv[]) {

	log_message("INFO: chipinterface_main()\n");

	ServiceStatus.dwServiceType        = SERVICE_WIN32; 
	ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
	ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | 
											SERVICE_ACCEPT_PAUSE_CONTINUE; 
	ServiceStatus.dwWin32ExitCode      = 0; 
	ServiceStatus.dwServiceSpecificExitCode = 0; 
	ServiceStatus.dwCheckPoint         = 0; 
	ServiceStatus.dwWaitHint           = 0; 

	hStatus = RegisterServiceCtrlHandler("rmchipinterface", chipinterface_handler);
	
	if (hStatus == (SERVICE_STATUS_HANDLE)0L) {
		log_message("ERROR: RegisterServiceCtrlHandler returned %ld\n", GetLastError());
	}

	ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
	ServiceStatus.dwCheckPoint         = 0; 
	ServiceStatus.dwWaitHint           = 0; 

	if (!SetServiceStatus (hStatus, &ServiceStatus)) { 
		DWORD status;

		status = GetLastError(); 
		log_message("ERROR: SetServiceStatus returned %d\n", status);
	} 

	open_chip_db();

	while (!0) {
		log_message("-- loop\n");
		Sleep(1000);
	}

	return;
}

void WINAPI chipinterface_handler(DWORD dwControl) {
	log_message("chipinterface_handler(%ld)\n", dwControl);

	switch (dwControl) {
    case SERVICE_CONTROL_STOP: 
        // Do whatever it takes to stop here. 
		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
		ServiceStatus.dwCheckPoint    = 0; 
		ServiceStatus.dwWaitHint      = 0; 

		if (!SetServiceStatus (hStatus, &ServiceStatus)) { 
			DWORD status = GetLastError();
			log_message("ERROR: SetServiceStatus returned %ld\n", status);
		} 

		log_message("INFO: Stopping rmchip service...\n");
		return; 

	default:
		break;
	}

}
#else	// WINDOWS_SERVICE

int main(int argc, char* argv[]) {
	WSADATA		wsaData = {0};

	WSAStartup(MAKEWORD(2,2), &wsaData);

	init_msglog();	/* Initialize logging systems. */


	log_message("chipinterf V%d.%d-%0d [%s]\n",
		MAJOR_VERSION,
		MINOR_VERSION,
		RELEASE_NUMBER,
		RELEASE_DATE);

/* Open the device /dev/mum{id} */;

	if (open_chip_db() != TRUE) {
		log_message("ERROR: Unable to open chip database!\n");
	}

//	option.pGBus = get_gbus(0);
		
	interface_Listen (option.nPort);

	log_message("INFO : Shutting down chip interface.\n");
	
	close_chip_db();
	close_msglog();		// Release the message logging facility...
	
	WSACleanup();

	return 0;
}

#endif	// WINDOWS_SERVICE

#else	// WIN32

int main(int argc, char* argv[]) {

	parseArguments(argc, argv);

	if (IS_FLAG_KILL(option.uFlags)) {
		kill_interface(option.nPort);
		return 0;
	}

#ifdef	ENABLE_SIG_HANDLER
	signal(SIGINT, signal_handler);
#endif

//	--	If DAEMON flag set, cause application to detach from parent process

	if (IS_FLAG_DAEMON(option.uFlags)) {
		daemon(0,0);
	}

	init_msglog();	/* Initialize logging systems. */
#ifdef ENABLE_REQUEST
	request_init();
#endif

	log_message("chipinterf V%d.%d-%0d [%s]\n",
		MAJOR_VERSION,
		MINOR_VERSION,
		RELEASE_NUMBER,
		RELEASE_DATE);
			
/* Open the device /dev/mum{id} */;

	if (open_chip_db() != TRUE) {
		log_message("ERROR: Unable to open chip database!\n");
	}

//	option.pGBus = get_gbus(0);
		

	interface_Listen (option.nPort);

	log_message("INFO : Shutting down chip interface.\n");
	
//	close_chip_db();
	close_msglog();		// Release the message logging facility...
	
	return 0;
}

void signal_handler(int signum) {
//	fprintf(stderr, "INTERRUPT HANDLER!\n");

	log_message("INFO : Shutting down chip interface.\n");

	close_chip_db();
	close_msglog();
	
	exit(0);
}

#endif	// _WIN32

void displayVersionNumber(void) {
	printf("Version %d.%d-%0d [%s]\n",
		MAJOR_VERSION,
		MINOR_VERSION,
		RELEASE_NUMBER,
		RELEASE_DATE);
	return;
}


/* End of chipinterf.c */
