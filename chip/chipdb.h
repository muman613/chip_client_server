//-----------------------------------------------------------------------------
//	MODULE		:	chipdb.h
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman, Damien Vincent
//	DATE		:	Jul 18, 2005
//	LAST MOD	:	$Date: 2006-03-22 02:53:44 $
//	DESCRIPTION	:	Maintain table of gbus and llad pointers
//-----------------------------------------------------------------------------

#ifndef	__CHIPDB_H__
#define __CHIPDB_H__

#include "chipinterf.h"

#define	MAX_ID		8

typedef struct _db_entry {
	struct gbus*	pgbus;
	struct llad*	pllad;
	char*			pdesc;
} DB_ENTRY;

extern DB_ENTRY		chip_db[];

int 			open_chip_db(void);
int 			close_chip_db(void);
unsigned short 	get_max_chip(void);
struct gbus* 	get_gbus(int index);

#endif	// __CHIPDB_H__
