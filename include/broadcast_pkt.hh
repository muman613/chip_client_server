//-----------------------------------------------------------------------------
//	MODULE		:	broadcast_pkt.h
//	PROJECT		:	EM86XX Debugger
//	PROGRAMMER	:	Michael Uman, Damien Vincent
//	DATE		:	Jul 14, 2005
//	LAST MOD	:	$Date: 2005-11-17 20:48:33 $
//	COPYRIGHT	:	(C) 2005 Sigma Designs, Inc.
//	DESCRIPTION	:	Define broadcast packet format.
//-----------------------------------------------------------------------------

#ifndef	__BROADCAST_PKT_H__
#define __BROADCAST_PKT_H__

#define BROADCAST_SOCKET		5080
#define REPLY_SOCKET			(BROADCAST_SOCKET + 1)

//typedef unsigned short int UINT16;

struct BROADCAST_PKT {
	char			sys_hostname[40];
	char			sys_username[40];

	char			tgt_hostname[40];
	unsigned short	tgt_version;
	unsigned short	tgt_maxchipid;
	unsigned short	tgt_port;
};

#endif	// __BROADCAST_PKT_H__
