/*
 * Copyright (C) 1995,1996  AT&T Wireless Services, Inc. All rights reserved.
 * Copyright (C) 1995,1996  Neda Communications, Inc. All rights reserved.
 *
 * This software is furnished under a license and use, duplication,
 * disclosure and all other uses are restricted to the rights specified
 * in the written license between the licensee and copyright holders.
 *
*/
/*+
 * File name: target.h
 *
 * Description: Target machine specific definitions
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 * 
 */

/*
 * RCS Revision: $Id: target.h,v 1.25 1996/11/21 19:24:41 kamran Exp $
 */

#ifndef _TARGET_H_	/*{*/
#define _TARGET_H_

#include "oe.h"
#include "du.h"

/* 
 * System Wide Buffer and Timer Specifications.
 */

#define FIXED_TIMERS

/*
 * Critical Section Protection Mechanism.
 */
#ifndef ENTER
# define ENTER();
#endif

#ifndef LEAVE
# define LEAVE();
#endif

/*
 * Mode of Interprocess communication to be used
 * IMQ_SYSV, IMQ_SIMU, IMQ_BSD
 */

/*
 * Clock period in miliseconds
 */
#define IMQ_BSD

#define CLOCK_PERIOD		1000	/* Clock period in miliseconds */
#define NUMBER_OF_TIMERS	0	/* No preallocated timers */

#define MAX_SCH_TASKS		100	/* Maximum number of SCH tasks */

#define MAX_SAPS		4	/* # of service access points */

#define MAXBFSZ			1528	/* maxPdu=1500; leave some extra */
#define BUFFERS			20	/* number of buffers */
#define VIEWS			40	/* number of views */

/*
 * UDP point of control and observation conditional compilation
 *
 * #define UDP_PC_ turns on the PDU inhibit feature
 * #define UDP_PO_ enables PDU logging
 *
 */
/*#define UDP_PC_*/
#define UDP_PO_
#define NO_UPSHELL

#endif	/*}*/
