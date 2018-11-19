//-----------------------------------------------------------------------------
//	MODULE		:	gbus_stubs.c
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman
//	DATE		:	Nov 3, 2005
//	LAST MOD	:	$Date: 2005-11-02 20:53:33 $
//	DESCRIPTION	:	stub out all required gbus functions
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include "gbus_stubs.h"

struct gbus *gbus_open(struct llad *h) {
	return (struct gbus*)0;
}

void gbus_close(struct gbus *h) {
}

struct llad *llad_open(RMascii *device) {
	return (struct llad*)0;
}

void llad_close(struct llad *h) {
}

RMuint32 gbus_read_uint32(struct gbus *h, RMuint32 byte_address) {
	return 0L;
}

void gbus_write_uint32(struct gbus *h, RMuint32 byte_address, RMuint32 data) {
}

void gbus_read_data8 (struct gbus *h, RMuint32 byte_address, RMuint8  *data, RMuint32 count) {
}

void gbus_write_data8 (struct gbus *h, RMuint32 byte_address, RMuint8  *data, RMuint32 count) {
}
