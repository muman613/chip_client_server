#ifndef SIMULATOR_H
#define SIMULATOR_H

/* Load a number of bytes from dm, pm or dram : return an array of bytes (big endian) */
#define COMMAND_LOADPM   0x0001  // Deprecated
#define COMMAND_LOADDM   0x0002  // Deprecated
#define COMMAND_LOADDRAM 0x0004  // Deprecated
#define COMMAND_READMEM  0x0008
#define	COMMAND_GBR32	 0x0009  // gbus_read_uint32
#define COMMAND_GBR16	 0x000A  // gbus_read_uint16
#define COMMAND_GBR8	 0x000B	 // gbus_read_uint8

/* Put a number of bytes into dm, pm or dram */
#define COMMAND_SAVEPM   0x0010  // Deprecated
#define COMMAND_SAVEDM   0x0020  // Deprecated
#define COMMAND_SAVEDRAM 0x0040  // Deprecated
#define COMMAND_WRITEMEM 0x0080
#define COMMAND_GBW32	 0x0090	 // gbus_write_uint32
#define COMMAND_GBW16    0x00A0  // gbus_write_uint16
#define COMMAND_GBW8	 0x00B0	 // gbus_write_uint8 

/* Make a host interruption */
#define COMMAND_BREAK 0x0100

/* Clear memory command */
#define COMMAMD_CLEARMEM 0x1000

/* Start, Reset or Stop mambo */
#define COMMAND_START 0x0200
#define COMMAND_RESET 0x0400
#define COMMAND_STOP  0x0800

/* Return the identity of the target */
#define COMMAND_IDENTIFY 0x2000
#define COMMAND_VERSION	 0x2001		// ONLY SUPPORTED RISCDSPD
#define COMMAND_INFO	 0x2002		// ONLY SUPPORTED RISCDSPD

#define COMMAND_OVLTABLE	0x3000

/* Return the number of cycles already done by the CPU */
#define COMMAND_CYCLES      0x4000
#define COMMAND_CYCLES2     0x4001
#define COMMAND_RESETCYCLES 0x8000

#define COMMAND_LAUNCH		0xA000


/* Return the status of the CPU */
#define COMMAND_GETSTATUS  0x10000

/* Express request interface */
#define	COMMAND_REGISTER	0x20000
#define COMMAND_UNREGISTER	0x20010
#define	COMMAND_REQUEST		0x20020

/* Plugin request interface */
#define	COMMAND_PLUGINQUERY			0x30000
#define	COMMAND_PLUGINLISTPARMS		0x30010
#define COMMAND_PLUGINSETPARM		0x30020
#define COMMAND_PLUGINGETPARM		0x30030
#define COMMAND_PLUGINRUN			0x30040

#define	COMMAND_EXIT				0x80000

/**
 * Structure used for sending request packet
 */
 
typedef struct {
	/* The chip number to send the command */
	RMuint32 chip;
	/* The command (see below) */
	RMuint32 command;
	/* The address in bytes */
	RMuint32 address;
	/* The number of bytes to load or to store */
	RMuint32 nbytes;
} hardlib_command_t;


#endif


/* End of simulator.h */
