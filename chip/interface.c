//-----------------------------------------------------------------------------
//	MODULE		:	interface.c
//	PROJECT		:	chipinterf
//	PROGRAMMER	:	Michael Uman, Damien Vincent
//	DATE		:	Jul 14, 2005
//	LAST MOD	:	$Date: 2008-11-26 20:57:04 $
//	DESCRIPTION	:	Handle TCP/IP interface.
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include <stdio.h>
#ifdef _WIN32
	#include <io.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define ALLOW_OS_CODE 1
#include "llad/include/gbus.h"
#include "rmdef/rmdef.h"
#include "simulator.h"
#include "interface.h"
#include "chipinterf.h"
#include "msglog.h"
#include "chipdb.h"
#include "request.h"
#include "utils.h"
#include "version.h"

//#define ENABLE_COMMSOCK		1

//	--	Build options...

#if (EM86XX_MODE != EM86XX_MODEID_STANDALONE)

#define NO_BLOCK			1			// Enable non-blocking code
#ifndef	_WIN32
	#define DO_FORK			1			// Enable forking server
#endif

#define ENABLE_BROWSER		1			// Enable resp to Debugger broadcast

#else	// (EM86XX_MODE == EM86XX_MODE_STANDALONE)

#define NO_BLOCK			1			// Enable non-blocking code
#define ENABLE_BROWSER		1			// Enable resp to Debugger broadcast

#endif	// (EM86XX_MODE == EM86XX_MODE_STANDALONE)

#ifdef	ENABLE_BROWSER
	#include "../include/broadcast_pkt.hh"
#endif

/* String giving the identity of the target */
//const char identity[] = "RISCDSPC";
const char identity[] = "RISCDSPD";

static void interface_ReadMem (int hSocket, struct gbus *pGBus,
                               unsigned int byteaddress, unsigned int nbytes);
static void interface_WriteMem (int hSocket, struct gbus *pGBus,
                                unsigned int byteaddress,
                                unsigned int nbytes);

//static void interface_gbus_read_uint32(int hSocket, struct gbus* pGBus, unsigned int byteaddress);


//static int recvall (int s, void *buf, size_t len, int flags);
//static int sendall (int s, const void *msg, size_t len, int flags);

void 	launch_command(void);
void 	addChild(int pid, int socket);
void 	releaseChild(int pid);
int 	findChild(int pid, int* socket);
int		create_named_socket(void);

#define MAX_PROCESS		4
#define MAX_CONNECTIONS	1		// Total connections to IP Socket

typedef struct _tagProcessTable {
	int		pid;
	int		socket;
} PROCESS_TABLE;

PROCESS_TABLE		pTable[MAX_PROCESS];

extern t_options option;
unsigned int	create_server_socket(UINT16 port);

#ifdef	ENABLE_BROWSER

//	--	Chip interface browser support function prototypes...
unsigned int 	create_broadcast_socket(void);
void 	handle_broadcast(int hBroadcastSocket);
void 	send_reply(struct sockaddr_in* server, struct BROADCAST_PKT* pkt);

#ifdef	DEBUG_BROADCAST
void 	display_broadcast_info(struct sockaddr_in* server, struct BROADCAST_PKT* pkt);
#endif

#endif	// ENABLE_BROWSER

#if 0
static void interface_gbus_read_uint32(int hSocket, struct gbus* pGBus, unsigned int byteaddress) {
	RMuint32		data;

	if (IS_DEBUG_READ(option.uFlags))
		log_message("interface_gbus_read_uint32(0x%08lx)\n", byteaddress);

	data = htonl(gbus_read_uint32(pGBus, byteaddress));
	sendall (hSocket, &data, sizeof(RMuint32), 0);

	return;
}
#endif

static void interface_ReadMem (	int hSocket,
								struct gbus *pGBus,
								unsigned int byteaddress,
								unsigned int nbytes)
{
	unsigned int i;

	unsigned int ndword = (nbytes >> 2) + 2;
	unsigned int *mem =
	    (unsigned int *) malloc (ndword * sizeof (unsigned int));
	unsigned char *bytemem = (unsigned char *) mem;

	unsigned int addressaligned = (byteaddress >> 2) << 2;
	unsigned int shift = byteaddress & 3;

#ifdef	DEBUG_READMEM
	fprintf(stderr, "rm ndword %d mem 0x%08x aligned 0x%08x shift %d ",
					ndword, (unsigned int)mem, addressaligned, shift);
#endif

	if (IS_DEBUG_READ(option.uFlags))
		log_message("interface_ReadMem(0x%08X, %d)\n", byteaddress, nbytes);

	if (byteaddress < 0x10000000) {
		// Read n bytes + conversion from host to network (big endian)
		for (i = 0; i < ndword; i++)
			mem[i] = htonl (gbus_read_uint32 (pGBus, addressaligned + 4 * i));
	} else {
		gbus_read_data8 (pGBus, byteaddress, (RMuint8 *) (bytemem + shift),
		                 nbytes);
	}

#ifdef	DEBUG_READMEM
	for (i = 0 ; i < nbytes ; i++) {
		fprintf(stderr, "0x%02x ",bytemem[i]);
	}
	fprintf(stderr, "\n");
#endif

	// Send the array to the network
	sendall (hSocket, (char *) (bytemem + shift), nbytes, 0);

	// Free memory
	free (mem);
}


static void interface_WriteMem (int hSocket,
								struct gbus *pGBus, 
								unsigned int byteaddress,
								unsigned int nbytes)
{
	unsigned int tmp, i;
	unsigned int err;

	unsigned int ndword = (nbytes >> 2) + 2;
	unsigned int *mem =
	    (unsigned int *) malloc (ndword * sizeof (unsigned int));
	unsigned char *bytemem = (unsigned char *) mem;

	unsigned int addressaligned = (byteaddress >> 2) << 2;
	unsigned int shift = byteaddress & 3;
	unsigned int shift2 = (byteaddress + nbytes) & 3;

//	if (option.uFlags & FLAG_DEBUG_WRITE)
	if (IS_DEBUG_WRITE(option.uFlags)) {
		log_message("interface_WriteMem(0x%08X, %d)\n", byteaddress, nbytes);
		log_message("addressaligned = 0x%08lx shift = %ld shift2 = %ld\n", addressaligned, shift, shift2);
	}

	// Initialize the output array
	for (i = 0; i < ndword; i++)
		mem[i] = 0;

	// Read n bytes from the network (Leave shift bytes at the begining for alignement purpose)
	err = recvall (hSocket, (char *) (bytemem + shift), nbytes, 0);
	if (err != nbytes)
	{
		log_message(
		         "INTERFACE(x): Error : didn't receive enough bytes : requested=%u bytes   read=%u bytes",
		         nbytes, err);
	}

	if (byteaddress < 0x10000000)
	{
		// Adjust the 1st dword (adjust the 1st dword with shifted bytes read from memory)
		tmp = gbus_read_uint32 (pGBus, addressaligned);
		for (i = 0; i < shift; i++)
			bytemem[i] = (tmp << (i * 8)) >> 24;

		// Adjust the last dword
		// tmp = gbus_read_uint32(addressaligned+((shift+nbytes)>>2));
		tmp = gbus_read_uint32 (pGBus, byteaddress + nbytes - shift2);
		for (i = shift2; i < 4; i++)
			bytemem[shift + nbytes + (i - shift2)] = (tmp << (i * 8)) >> 24;

		// Convert from network (big endian) to host + Write the array in memory
		for (i = 0; i < (shift + nbytes + ((4 - shift2) & 3)) >> 2; i++)
			gbus_write_uint32 (pGBus, addressaligned + 4 * i, ntohl (mem[i]));
	} else
	{
		gbus_write_data8 (pGBus, byteaddress, (RMuint8 *) (bytemem + shift),
		                  nbytes);
	}

	// Free memory
	free (mem);
}

int interface_Listen (UINT16 port) {
	/* Variables to establish the connection with the chip */
	struct gbus *pGBus;

	/* Socket */
	unsigned int 		hSocket, hServerSocket;
#ifdef	ENABLE_COMMSOCK
	unsigned int		hCommSocket;
#endif
	unsigned int 		hBroadcastSocket = (unsigned int)-1;
//	struct sockaddr_in 	sockaddr;
	struct sockaddr_in	client;
//	int					clientLen;
	socklen_t			clientLen;
	
	/* Variables to handle the command */
	hardlib_command_t command;

	/* Statistics */
	unsigned int totalbytecount;
	unsigned int numcycles;
	unsigned int cpustatus[2];

#ifdef	DO_FORK
	int childPID = -1;
#endif
	//	int count;

#ifndef	_WIN32
	/* remove lock file */
	unlink("/tmp/chipinterf.lock");
#endif
	
	/* Initialize process table */

	memset(&pTable, 0, sizeof(pTable));

#ifdef	ENABLE_COMMSOCK
	hCommSocket = create_named_socket();
#endif

	/* Create TCP/IP sockets. */

	hServerSocket = create_server_socket(port);

	if (hServerSocket == (unsigned int)-1) {
		log_message("Unable to create server socket!\n");
		return -1;
	}

#ifdef	ENABLE_BROWSER

	hBroadcastSocket = create_broadcast_socket();

	if (hBroadcastSocket == (unsigned int)-1) {
		log_message("Unable to create broadcast socket!\n");
		socket_close(hServerSocket);
		return -1;
	}

#ifdef	_DEBUG
	fprintf(stderr, "hCommSocket = %d\nhServerSocket = %d\nhBroadcastSocket = %d\n",
		hCommSocket, hServerSocket, hBroadcastSocket);
#endif

#endif	// ENABLE_BROWSER

	while (1) {
		log_message("Waiting for connection (Port %d)...\n", option.nPort);
		
#ifndef NO_BLOCK
		
		clientLen = sizeof(client);
		hSocket = accept (hServerSocket, (struct sockaddr*)&client, &clientLen);
		log_message("Connection received from [%s]!\n",
				inet_ntoa(client.sin_addr));
		
#else	// NO_BLOCK

		hSocket = -1;
		while (hSocket == (unsigned int)-1) {
			fd_set			readSet;
			int				num;
			struct timeval 	tv;
			int				max_select;
#ifdef	DO_FORK
			int				id;
#endif
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			
			FD_ZERO(&readSet);
#ifdef	ENABLE_COMMSOCK
			FD_SET(hCommSocket, &readSet);
#endif
			FD_SET(hServerSocket, &readSet);

#ifdef	ENABLE_BROWSER
			FD_SET(hBroadcastSocket, &readSet);
			max_select = hBroadcastSocket + 1;
#else
			max_select = hServerSocket + 1;
#endif

			/* Wait for connection to socket */
//			fprintf(stderr, "Waiting for signal!\n");
			num = select(max_select, &readSet, NULL, NULL, &tv);	
//			fprintf(stderr, "num = %d!\n", num);

			if (FD_ISSET(hServerSocket, &readSet)) {
				clientLen = sizeof(client);
				hSocket = accept(hServerSocket, (struct sockaddr*)&client, &clientLen);
#ifdef	_WIN32
				if (hSocket == -1) {
					MSWError("accept");	
				}
#endif
				log_message("Connection received from [%s]!\n",
					inet_ntoa(client.sin_addr));
#ifdef	ENABLE_BROWSER
			} else if (FD_ISSET(hBroadcastSocket, &readSet)) {
				handle_broadcast(hBroadcastSocket);
#endif
#ifdef ENABLE_COMMSOCK
			} else if (FD_ISSET(hCommSocket, &readSet)) {
				char ch;

				read(hCommSocket, &ch, 1);
				
				log_message("shutting down!\n");
				goto shutdown;
#endif
			} else {

				/* check if a child process has completed */

#ifdef	DO_FORK
#ifdef	_WIN32

#else
				id = waitpid(-1, NULL, WNOHANG);

				if (id > 0) {
					int socket;

					log_message("Child %d exited...\n", id);

					/* check if process is a server process */

					if (findChild(id, &socket)) {
						close(socket);
						releaseChild(id);
					}
				}
#endif	// _WIN32
#endif	// DO_FORK
			}
			
		}

#endif	// NO_BLOCK

//#ifdef	_WIN32

//#else	// _WIN32

#ifdef	DO_FORK
		if ((childPID = fork()) == 0) {
#endif

//		fprintf(stderr, "Starting main loop!\n");
		
		/****************
		 * Command loop *
		 ****************/

		while (recvall
		        (hSocket, (char *) (&command), sizeof (hardlib_command_t),
		         0) == sizeof (hardlib_command_t)) {

			/* Read a command */
			totalbytecount += sizeof (hardlib_command_t);

			/* Convert to host integers */
			command.chip 	= ntohl (command.chip);
			command.command = ntohl (command.command);
			command.address = ntohl (command.address);
			command.nbytes 	= ntohl (command.nbytes);

//			log_message("COMMAND   : chip %ld Command %08lx Address 0x%08lx nBytes %ld\n",
//				command.chip, command.command, command.address, command.nbytes);

			pGBus = get_gbus(command.chip);	// get gbus interface...

			if (pGBus == 0) {
				log_message("ERROR: Invalid chip id!\n");
				break;
			}
			
//			log_message("Command received [0x%08X]\n", command.command);

			/* Handle the command */
			switch (command.command) {
			case COMMAND_READMEM:
				interface_ReadMem (hSocket, pGBus, command.address,
				                   command.nbytes);
				totalbytecount += command.nbytes;
				break;
			case COMMAND_WRITEMEM:
				interface_WriteMem (hSocket, pGBus, command.address,
				                    command.nbytes);
				totalbytecount += command.nbytes;
				break;
		
#if 0
			case COMMAND_GBR32:
				interface_gbus_read_uint32(hSocket, pGBus, command.address);
				totalbytecount += sizeof(RMuint32);
				break;

			case COMMAND_GBR16:
			case COMMAND_GBR8:
				break;

			case COMMAND_GBW32:
			case COMMAND_GBW16:
			case COMMAND_GBW8:
				break;
#endif

			case COMMAND_BREAK:
				log_message("INTERFACE(%d): Host interruption\n", port);
				/* break audio block #0 */
				gbus_write_uint32 (pGBus,
				                   dspmem_mapping_aud0 + OFFSET_DM + (0x3E8A << 2),
				                   0x101);
				/* break audio block #1 */
				gbus_write_uint32 (pGBus,
				                   dspmem_mapping_aud1 + OFFSET_DM + (0x3E8A << 2),
				                   0x101);
				/* break transport demux block */
				gbus_write_uint32 (pGBus,
				                   dspmem_mapping_tdmx + OFFSET_DM + (0x2F0A << 2),
				                   0x101);
				/* break MPEG block #0 */
				gbus_write_uint32 (pGBus,
								   dspmem_mapping_vid0 + OFFSET_DM + (0xFFA << 2),
								   0x101);
				/* break MPEG block #1 */
				gbus_write_uint32 (pGBus,
								   dspmem_mapping_vid1 + OFFSET_DM + (0xFFA << 2),
								   0x101);
				break;

			case COMMAND_START:
				log_message("INTERFACE(%d): Starting the Risc\n", port);
				gbus_write_uint32 (pGBus, ioblock_mapping_video0 + OFFSET_RESETVEC, 0);
				gbus_write_uint32 (pGBus, ioblock_mapping_audio0 + OFFSET_RESETVEC, 0);
				gbus_write_uint32 (pGBus, ioblock_mapping_video1 + OFFSET_RESETVEC, 0);
				gbus_write_uint32 (pGBus, ioblock_mapping_audio1 + OFFSET_RESETVEC, 0);
				break;

			case COMMAND_RESET:
				log_message("INTERFACE(%d): Reseting the Risc\n", port);
				gbus_write_uint32 (pGBus, ioblock_mapping_video0 + OFFSET_RESETVEC, 3);
				gbus_write_uint32 (pGBus, ioblock_mapping_audio0 + OFFSET_RESETVEC, 3);
				gbus_write_uint32 (pGBus, ioblock_mapping_video1 + OFFSET_RESETVEC, 3);
				gbus_write_uint32 (pGBus, ioblock_mapping_audio1 + OFFSET_RESETVEC, 3);
				break;
			
			case COMMAND_STOP:
				log_message("INTERFACE(%d): Stopping the Risc\n", port);
				gbus_write_uint32 (pGBus, ioblock_mapping_video0 + OFFSET_RESETVEC, 1);
				gbus_write_uint32 (pGBus, ioblock_mapping_audio0 + OFFSET_RESETVEC, 1);
				gbus_write_uint32 (pGBus, ioblock_mapping_video1 + OFFSET_RESETVEC, 1);
				gbus_write_uint32 (pGBus, ioblock_mapping_audio1 + OFFSET_RESETVEC, 1);
				break;
			
			case COMMAMD_CLEARMEM:
				break;
			
			case COMMAND_IDENTIFY:
				send(hSocket, identity, 8, 0);
				totalbytecount += 8;
				break;

			case COMMAND_VERSION:
                {
					char	sVersion[16];

					memset(sVersion, 0, 16);
					sprintf(sVersion, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION,RELEASE_NUMBER);
					send(hSocket, sVersion, 16, 0);
					totalbytecount += 16;
				}
				break;

			case COMMAND_INFO:
				{
					int			x;
					RMuint32	numBoards = htonl(get_max_chip());

					send(hSocket, &numBoards, 4, 0);

					for (x = 0 ; x < get_max_chip() ; x++) {
						send(hSocket, chip_db[x].pdesc, 37, 0);
					}
				}
				break;
			case COMMAND_CYCLES:
				numcycles = htonl (0);
				send (hSocket, (char *) (&numcycles), 4, 0);
				totalbytecount += 4;
				break;

			case COMMAND_CYCLES2:
				numcycles = htonl (0);
//				write (hSocket, (char *) (&numcycles), 4);
				send (hSocket, (char *) (&numcycles), 4, 0);
				totalbytecount += 4;
				break;

			case COMMAND_RESETCYCLES:
				break;

			case COMMAND_GETSTATUS:
				cpustatus[0] = htonl (0);
				cpustatus[1] = htonl (0);
//				write (hSocket, (char *) (cpustatus), 8);
				send( hSocket, (char *)(cpustatus), 8, 0);
				totalbytecount += 8;
				break;

			case COMMAND_LAUNCH:
				launch_command();
				break;

#ifdef	ENABLE_REQUEST				
			case COMMAND_REGISTER:
				request_register(&command, hSocket);
				break;

			case COMMAND_UNREGISTER:
				request_unregister(&command, hSocket);
				break;

			case COMMAND_REQUEST:
				request_request(&command, hSocket, pGBus);
				break;
#endif
					
			default:
				log_message("Unknown command : 0x%X\n", command.command);
			}

		}

//		log_message("Closing socket %d\n", hSocket);

		close (hSocket);
#ifdef	DO_FORK
		_exit(0);
	} else {
		addChild(childPID, hSocket);
	}
#endif	// DO_FORK

//#endif	// _WIN32
	}

shutdown:

	socket_close(hServerSocket);
#ifdef	ENABLE_COMMSOCK
	socket_close(hCommSocket);
#endif

	if (hBroadcastSocket != (unsigned int)-1) {
		socket_close(hBroadcastSocket);
		hBroadcastSocket = -1;
	}

	unlink(NAMED_SOCKET);

	return 0;
}

void launch_command(void) {

#ifdef	_WIN32

//	NOT SUPPORTED ON WINDOWS PLATFORM

#else	// _WIN32
	struct stat buf;

//	If no command specified, just return...

	if (option.szCmd == NULL) 
		return;
	
	
//	Check if lock file exists...

	if (stat("/tmp/chipinterf.lock", &buf) != 0) {

		log_message("INTERFACE(%d): Launching application...\n", option.nPort);

		if ((option.child_pid = fork()) == 0) {
			int 	result = 0;
			int 	fd;
			char	cmdLine[256];

//	Construct command using template : xterm -e {szCmd}

			strncpy(cmdLine, "xterm -e ", 256);
			strncat(cmdLine, option.szCmd, 256);

			fd = open("/tmp/chipinterf.lock", O_WRONLY|O_CREAT);
			close(fd);

//	Execute the command...

			result = system(cmdLine);
			
			unlink("/tmp/chipinterf.lock");
	
#ifdef	_DEBUG
			log_message("Result = %d\n", result);
#endif

			_exit(0);
		} else {
			log_message("Child process pid = %d\n", option.child_pid);
		}
	} else {
	//	int res = 0;
		log_message("-- application already running [%d]!\n", option.child_pid);
#if 0
		res = kill((option.child_pid + 1), SIGKILL);
		if (res == 0) {
			log_message("process killed!\n");
//			waitpid(option.child_pid + 1, NULL, 0);
//			option.child_pid = 0;

			sleep(1);
			launch_command();
		}
#endif
	}

#endif	// _WIN32

	return;
}

/*
 * 	child process tracking functions
 */

void addChild(int pid, int socket) {
	int i;

	for (i = 0 ; i < MAX_PROCESS ; i++) {
		if (pTable[i].pid == 0) {
			pTable[i].pid 		= pid;
			pTable[i].socket 	= socket;
			break;
		}
	}

	return;
}

int findChild(int pid, int* socket) {
	int i;

	for (i = 0 ; i < MAX_PROCESS ; i++) {
		if (pTable[i].pid == pid) {
			if (socket != NULL)
				*socket = pTable[i].socket;
			return 1;
		}
	}

	return 0;
}

void releaseChild(int pid) {
	int i;

	for (i = 0 ; i < MAX_PROCESS ; i++) {
		if (pTable[i].pid == pid) {
			pTable[i].pid = 0;
			pTable[i].socket = 0;
			break;
		}
	}

	return;
}

unsigned int create_server_socket(UINT16 port) {
	unsigned int 		hServerSocket;
	struct sockaddr_in 	sockaddr;

	/* Wait for a connection on the port */
	sockaddr.sin_addr.s_addr 	= INADDR_ANY;
	sockaddr.sin_port 			= htons(port);
	sockaddr.sin_family 		= AF_INET;

	hServerSocket = socket (AF_INET, SOCK_STREAM, 0);

	if (hServerSocket == (unsigned int)-1) {
#ifdef	_WIN32
		log_message("Socket error : WSAGetLastError = %d", WSAGetLastError());
#else
		log_message("Socket error\n");
#endif
		return -1;
	}

	if (bind(hServerSocket, (struct sockaddr *) &sockaddr, sizeof (sockaddr)) == -1) {
		log_message("Bind error!\nUsually occurs when another instance is running...\n");
		return -1;
	}
	
	if (listen (hServerSocket, MAX_CONNECTIONS) == -1) {
		log_message("Listen error\n");
		return -1;
	}

	return hServerSocket;
}

#ifdef	ENABLE_BROWSER

unsigned int create_broadcast_socket(void) {
	struct sockaddr_in		sin = {0};
	int 	hBroadcastSocket = -1;
	int 	res = 0;
	int		opt = 1;
	
	hBroadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);

//	--	Set socket as a broadcast socket...
	
	res = setsockopt(hBroadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(int));

	if (res == -1) {
		log_message("ERROR: Unable to set socket option BROADCAST!\n");
		socket_close(hBroadcastSocket);
		return -1;
	}

//	--	Set socket to reuse address if needed...

	res = setsockopt(hBroadcastSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(int));
	if (res == -1) {
		log_message("ERROR: Unable to set socket option REUSEADDR!\n");
		socket_close(hBroadcastSocket);
		return -1;
	}
	
//	printf("Waiting for broadcasts...\n");


	sin.sin_family 		= AF_INET;
	sin.sin_addr.s_addr = 0;//INADDR_BROADCAST; //0;
	sin.sin_port		= htons(BROADCAST_SOCKET);

	res = bind(hBroadcastSocket, (struct sockaddr*)&sin, sizeof(sin));

	if (res == -1) {
#ifdef	_WIN32
		MSWError("BIND");
#else
		log_message("BIND : %d - %d %s\n", res, errno, strerror(errno));
#endif
		socket_close(hBroadcastSocket);
		return -1;
	}

	return hBroadcastSocket;
}

void handle_broadcast(int hBroadcastSocket) {
	struct sockaddr_in		serverIP;
	socklen_t				ipLen = sizeof(serverIP);
	struct BROADCAST_PKT	packet;
	int						res;

//	log_message(">> handle_broadcast()\n");

	res = recvfrom(hBroadcastSocket, (char*)&packet, sizeof(packet), 0,
			(struct sockaddr*)&serverIP, &ipLen);

#ifdef	DEBUG_BROADCAST
	display_broadcast_info(&serverIP, &packet);
#endif	// DEBUG_BROADCAST

	send_reply(&serverIP, &packet);
}

#ifdef	DEBUG_BROADCAST
void display_broadcast_info(struct sockaddr_in* server, struct BROADCAST_PKT* pkt) {
	printf("inet addr = %s\n", inet_ntoa(server->sin_addr));
	printf("uname     = %s\n", pkt->sys_hostname);
	printf("user name = %s\n", pkt->sys_username);

	return;
}
#endif

void send_reply(struct sockaddr_in* server, struct BROADCAST_PKT* pkt) {
	int s, sock;
	struct sockaddr_in		sin = {0};

//	log_message(">> Send reply!\n");
	
	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (s == -1) {
		printf("ERROR: Unable to create socket!\n");
		return;
	}

	memcpy(&sin, server, sizeof(struct sockaddr_in));
		
	sin.sin_family 		= AF_INET;
	sin.sin_port		= htons(REPLY_SOCKET);
		
	sock = connect(s, (struct sockaddr*)&sin,  sizeof(sin));

	if (sock == -1) {
		log_message("ERROR: Unable to connect to remote socket!\n");
	} else {
//		log_message("-- CONNECTED --\n");

//	--	Fill in target information in packet...

		gethostname(pkt->tgt_hostname, 40);
//		pkt->tgt_chipid		= htons(option.nChipID);
		pkt->tgt_maxchipid	= htons(get_max_chip());
		pkt->tgt_port		= htons(option.nPort);
		pkt->tgt_version	= htons(((MAJOR_VERSION & 0xff) << 8) | (MINOR_VERSION & 0xff));

//		log_message("HOSTNAME = %s", pkt->tgt_hostname);

//	--	Send packet back to host...
		
//		write(s, (char *)pkt, sizeof(struct BROADCAST_PKT));
		send(s, (const char*)pkt, sizeof(struct BROADCAST_PKT), 0);
	}

	socket_close(s);
}

#endif	// ENABLE_BROWSER
#include <sys/un.h>

int create_named_socket(void) {
	int	hSock = 0;
	struct sockaddr_un myname;
	
	fprintf(stderr,"create_named_socket()\n");
	
	unlink(NAMED_SOCKET);
	
	hSock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (hSock < 0) {
		fprintf(stderr, "ERROR: Unable to create socket!\n");
		return -1;
	}
	myname.sun_family = AF_UNIX;
	strcpy(myname.sun_path, "/tmp/chipinterf");
	
	if (bind(hSock, (struct sockaddr*)&myname, sizeof(struct sockaddr_un))) {
		fprintf(stderr, "ERROR: unable to bind socket!\n");
		return -1;
	}
	
//	printf("socket = %d\n", hSock);
	return hSock;
}

/*
 * Send to shutdown socket.
 */
 
void kill_interface(UINT16 port) {
	int	hSock = 0;
	struct sockaddr_un myname;
	int res;
	char	ch = 'A';
		
	hSock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (hSock < 0) {
		log_message("ERROR: Unable to create socket!\n");
		return;
	}
	
	myname.sun_family = AF_UNIX;
	strcpy(myname.sun_path, NAMED_SOCKET);
	
	res = connect(hSock, (struct sockaddr*)&myname, sizeof(myname));
	if (res == -1) {
		log_message("No interfaces are running...\n");
		return;
	}
		
//	printf("Connected!\n");
	write(hSock, &ch, 1);
	printf("Interface closed!\n");
	close(hSock);
	return;
}
/* End of interface.c */

