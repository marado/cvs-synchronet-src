/* nodedefs.h */

/* Synchronet node information structure and constant definitions */

/* $Id$ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2000 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This program is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU General Public License				*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU General Public License for more details: gpl.txt or			*
 * http://www.fsf.org/copyleft/gpl.html										*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

/************************************************************/
/* Constants, macros, and typedefs for use ONLY with SBBS	*/
/************************************************************/

#ifndef _NODEDEFS_H
#define _NODEDEFS_H

#include "gen_defs.h"

enum {                              /* Node Status */
     NODE_WFC                       /* Waiting for Call */
    ,NODE_LOGON                     /* at logon prompt */
    ,NODE_NEWUSER                   /* New user applying */
    ,NODE_INUSE                     /* In Use */
    ,NODE_QUIET                     /* In Use - quiet mode */
    ,NODE_OFFLINE                   /* Offline */
    ,NODE_NETTING                   /* Networking */
    ,NODE_EVENT_WAITING             /* Waiting for all nodes to be inactive */
    ,NODE_EVENT_RUNNING             /* Running an external event */
    ,NODE_EVENT_LIMBO               /* Allowing another node to run an event */
    };

                                    /* Bit values for node.misc */
#define NODE_ANON   (1<<0)          /* Anonymous User */
#define NODE_LOCK   (1<<1)          /* Locked for sysops only */
#define NODE_INTR   (1<<2)          /* Interrupted - hang up */
#define NODE_MSGW   (1<<3)          /* Message is waiting (old way) */
#define NODE_POFF   (1<<4)          /* Page disabled */
#define NODE_AOFF   (1<<5)          /* Activity Alert disabled */
#define NODE_UDAT   (1<<6)          /* User data has been updated */
#define NODE_RRUN   (1<<7)          /* Re-run this node when log off */
#define NODE_EVENT  (1<<8)          /* Must run node event after log off */
#define NODE_DOWN   (1<<9)          /* Down this node after logoff */
#define NODE_RPCHT  (1<<10)         /* Reset private chat */
#define NODE_NMSG   (1<<11)         /* Node message waiting (new way) */
#define NODE_EXT    (1<<12)         /* Extended info on node action */
#define NODE_LCHAT	(1<<13)			/* Being pulled into local chat */

enum {                              /* Node Action */
     NODE_MAIN                      /* Main Prompt */
    ,NODE_RMSG                      /* Reading Messages */
    ,NODE_RMAL                      /* Reading Mail */
    ,NODE_SMAL                      /* Sending Mail */
    ,NODE_RTXT                      /* Reading G-Files */
    ,NODE_RSML                      /* Reading Sent Mail */
    ,NODE_PMSG                      /* Posting Message */
    ,NODE_AMSG                      /* Auto-message */
    ,NODE_XTRN                      /* Running External Program */
    ,NODE_DFLT                      /* Main Defaults Section */
    ,NODE_XFER                      /* Transfer Prompt */
    ,NODE_DLNG                      /* Downloading File */
    ,NODE_ULNG                      /* Uploading File */
    ,NODE_BXFR                      /* Bidirectional Transfer */
    ,NODE_LFIL                      /* Listing Files */
    ,NODE_LOGN                      /* Logging on */
    ,NODE_LCHT                      /* In Local Chat with Sysop */
    ,NODE_MCHT                      /* In Multi-Chat with Other Nodes */
    ,NODE_GCHT                      /* In Local Chat with Guru */
    ,NODE_CHAT                      /* In Chat Section */
    ,NODE_SYSP                      /* Sysop Activity */
    ,NODE_TQWK                      /* Transferring QWK packet */
    ,NODE_PCHT                      /* In Private Chat */
    ,NODE_PAGE                      /* Paging another node for Private Chat */
    ,NODE_RFSD                      /* Retrieving file from seq dev (aux=dev)*/

	,NODE_LAST_ACTION				/* Must be last */
    };

#ifdef _WIN32	/* necessary for compatibility with SBBS v2 */
#pragma pack(push)
#pragma pack(1)
#endif

typedef struct {						/* Node information kept in NODE.DAB */
    uchar   status,                     /* Current Status of Node */
            errors,                     /* Number of Critical Errors */
            action;                     /* Action User is doing on Node */
    ushort  useron,                     /* User on Node */
            connection,                 /* Connection rate of Node */
            misc,                       /* Miscellaneous bits for node */
            aux;                        /* Auxillary word for node */
    ulong   extaux;                     /* Extended aux dword for node */
            } node_t;

#ifdef _WIN32
#pragma pack(pop)		/* original packing */
#endif

#endif /* Don't add anything after this line */
