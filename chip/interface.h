#ifndef INTERFACE_HH
#define INTERFACE_HH

typedef unsigned short  UINT16;
int interface_Listen(UINT16 port);
void kill_interface(UINT16 port);

/**
 * TEMP.
 */
#define dramctrl_mapping 		0x10000000
#define dspmem_mapping_tdmx		0x140000	/* Transport Demux block */
#define dspmem_mapping_aud0   	0x180000	/* Audio block #0 */
#define dspmem_mapping_aud1		0x1a0000	/* Audio block #1 */
#define dspmem_mapping_vid0		0x100000	/* MPEG block #0 */
#define dspmem_mapping_vid1		0x120000	/* MPEG block #1 */

#define ioblock_mapping_video0	0x80000		/* Video block #0 */
#define ioblock_mapping_video1	0x90000		/* Video block #1 */
#define ioblock_mapping_demux0	0xA0000		/* Demux block #0 */
#define ioblock_mapping_audio0  0xC0000		/* Audio chip #0 */
#define ioblock_mapping_audio1  0xD0000		/* Audio chip #0 */

#define OFFSET_DM        0x10000	/* Offset in bytes */
#define OFFSET_RESETVEC  0x0FFFC	/* Offset in bytes */


#define	NAMED_SOCKET	"/tmp/chipinterf"

#endif

/* End of interface.h */
