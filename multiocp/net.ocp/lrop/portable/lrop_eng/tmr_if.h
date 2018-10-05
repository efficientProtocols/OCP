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
 * File name: tmr_if.h (Timer Interface)
 *
 * Description: Timer definition.
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

/*
 * RCS Revision: $Id: tmr_if.h,v 1.8 1996/09/09 10:29:35 kamran Exp $
 */

#ifndef _TMR_IF_H_	/*{*/
#define _TMR_IF_H_

#include "tmr.h"
#include "invoke.h"

typedef long TmrDuration;


typedef struct TmrQuHead {
	struct TmrData	*first;
	struct TmrData	*last;
} TmrQuHead;

#if 0
/* Where is the right place for this, tmr.h?? */
typedef Int SeqNu;
#define ALL_SEQ ((SeqNu) -1)
#endif

typedef struct TmrData {
    struct TmrData *next;
    struct TmrData *prev;
    struct InvokeInfo *invoke;
    Int name;
#if 0
    SeqNu seqNu;
#endif
    Int data;
} TmrData;

/* NOTYET, Needs ordering and cleanup */
typedef enum tmr_Name {
    lrop_allTimer = -1,

    lrop_invokePduRetranTimer =	1,
    lrop_resultPduRetranTimer =	2,
    lrop_lastTimer = 3,
#if 0
    EX_lrop_lastTimer =	7,
#endif
    lrop_inactivityTimer = 4,
    lrop_refNuTimer = 5,
    lrop_ackTimer = 6,
    lrop_perfNoResponseTimer = 7
} tmr_Name;

LCL_XTRN LgUns lrop_timeStamp;

#ifdef LINT_ARGS	/* Arg. checking enabled */

/*
extern Void lrop_tmrSetValues(InvokeInfo *);
extern Void lrop_tmrUpdate (InvokeInfo *, int);
extern Int  lrop_timerRsp  (Ptr);
extern SuccFail lrop_tmrCreate (InvokeInfo *, Int, LgInt, Int);
extern Void lrop_tmrCancel (InvokeInfo *, Int);
*/
SuccFail lrop_tmrCreate();
Void lrop_tmrSetValues();
Void lrop_tmrUpdate();
Void lrop_tmrCancel();

#else

SuccFail lrop_tmrCreate();
Void lrop_tmrSetValues();
Void lrop_tmrUpdate();
Void lrop_tmrCancel();

#endif /* LINT_ARGS */

#endif	/*}*/
