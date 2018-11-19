//-----------------------------------------------------------------------------
//	MODULE		:	chipdb.c
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman
//	DATE		:	Jul 18, 2005
//	LAST MOD	:	$Date: 2006-03-22 02:53:44 $
//	DESCRIPTION	:	Maintain table of gbus and llad pointers
//-----------------------------------------------------------------------------

#define ALLOW_OS_CODE 1

#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chipdb.h"
#include "msglog.h"

//#define	DEBUG_CHIPDB		1

DB_ENTRY	chip_db[MAX_ID];
static unsigned short	max_id = 0;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int open_chip_db(void) {
	int 		result = FALSE;

#ifdef	_WIN32

	max_id = 1;
	result = TRUE;
	chip_db[0].pgbus = (struct gbus*)-1;

#else	// _WIN32

	int			index;
	RMuint32 	ui32;

#ifdef	DEBUG_CHIPDB
	fprintf(stderr, "open_chip_db()\n");
#endif

	memset(chip_db, 0, sizeof(chip_db));
	
	for (index = 0 ; index < MAX_ID ; index++) {
		char sDeviceID[16];
		struct llad*	pllad;
		struct gbus*	pgbus;
		char*			pDesc = 0L;

		snprintf(sDeviceID, 16, "%d", index);
		pllad = llad_open (sDeviceID);

		if (pllad == NULL) {
//			printf("No llad device %s\n", sDeviceID);
			break;
		}

		/* get config options */

		pDesc = malloc(38);
		memset(pDesc, 0, 38);
		llad_get_config(pllad, pDesc, 38);
		
		log_message("INFO: %s\n", pDesc);
		
		pgbus = gbus_open (pllad);

		if (pgbus == NULL) {
//			printf("No gbus for llad 0x%08x\n", (int)pllad);
			break;
		}

//		log_message("INFO: pGBus = 0x%08x\n", pgbus);
		
		/* Set up the chip */

		if (IS_FLAG_INIT(option.uFlags)) {
			log_message("INFO : Initializing chip...\n");
			chip_init (pgbus);
		}

		/* Chip initialized ? */
		ui32 = gbus_read_uint32 (pgbus, 0x10000 + 0x3C);

		if (ui32 == 0) {
			log_message("INFO : Warning, system speed not initialized or incorrect.\n");
			log_message("INFO : You should run em8600_dram_init first.\n");
		}

		/* Display some info */

		chip_displayinfo (pgbus);

		chip_db[index].pllad	= pllad;
		chip_db[index].pgbus 	= pgbus;
		chip_db[index].pdesc	= pDesc;
	}

	max_id = index;
	result = TRUE;

#endif	// _WIN32

	log_message("INFO : %d boards in chip database\n", max_id);

	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int close_chip_db(void) {
	int result = FALSE;

#ifdef	_WIN32

	result = TRUE;

#else	// _WIN32

	int		index;

#ifdef	DEBUG_CHIPDB
	fprintf(stderr, "close_chip_db()\n");
#endif

	for (index = 0; index < max_id ; index++) {
		gbus_close(chip_db[index].pgbus);
		llad_close(chip_db[index].pllad);
		free(chip_db[index].pdesc);
		
		chip_db[index].pgbus = 0;
		chip_db[index].pllad = 0;
		chip_db[index].pdesc = 0;
	}

#endif	// _WIN32

	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

unsigned short get_max_chip(void) {
	int result = max_id;

	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct gbus* get_gbus(int index) {
	struct gbus*	pGBus = 0;

	if (index <= max_id) 
		pGBus = chip_db[index].pgbus;
	
	return pGBus;
}

//-----------------------------------------------------------------------------
