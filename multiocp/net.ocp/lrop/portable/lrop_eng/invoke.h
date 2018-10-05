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
 * File name: invoke.h
 *
 * Description:
 *
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 * 
 */

/*
 * RCS Revision: $Id: invoke.h,v 1.17 1996/09/10 17:26:16 kamran Exp $
 */

#ifndef _INVOKE_H_	/*{*/
#define _INVOKE_H_

#include "addr.h"
#include "queue.h"
#include "du.h"

#include "tmr_if.h"

#include "lrop.h"
#include "target.h"

typedef struct PduSeq {
     DU_View first;
     DU_View last;
} PduSeq;

/*
 *  The  invoke structure.
 */

typedef struct InvokeInfoSeq {
    struct InvokeInfo *first;
    struct InvokeInfo *last;
} InvokeInfoSeq;

typedef struct InvokeInfo {
    struct InvokeInfo *next;		
    struct InvokeInfo *prev;
    
    struct FSM_TransDiagram *transDiag;    /* Code/Program to be run by this machine */
    struct FSM_State	*curState;
    Bool		performingNotInvoking;
		
    struct SapInfo	*locSap;	/* local  user */
    
    LROP_UserInvokeRef  userInvokeRef;  /* LSROS User Invoke Reference */
    LSRO_SapSel		remLsroSapSel;	/* Remote Address */
    T_SapSel		remTsapSel;
    N_SapAddr 		remNsapAddr;	/* remote network address */

    LROP_OperationValue	operationValue;
    LROP_EncodingType	encodingType;
    LROP_ErrorValue	errorValue;
    Int 		failureReason;
    Int 		invokeRefNu;
 
#ifndef FIXED_TIMERS
    TmrDuration		maxLocAckDelay;	/* local acknowledge time */
    TmrDuration		maxRemAckDelay;	/* remote acknowledge time */
    TmrDuration		roundTripDelay;	/* current round trip delay average */
#endif

    TmrDuration		retransTimeout;	/* retransmission timer interval */

    TmrDuration		refKeepTime;
    TmrDuration		rwait; 
    TmrDuration		inactivityDelay; 
    TmrDuration		perfNoResponse;

    PduSeq		invokePduSeq;
    PduSeq		resultPduSeq;

    short		nuOfRetrans;		/* max retransmission count */

    Bool		isResultNotError;

    TmrQuHead	tmrQu;

    QU_Element refMap;

} InvokeInfo;

#define lrop_getNsapAddr(elem) ((N_SapAddr *)((unsigned long int)elem-(unsigned long int)nSapAddrOffset))

#define lrop_getInvokeInfoAddr(elem) ((InvokeInfo *)((unsigned long int)elem-(unsigned long int)invokeInfoOffset))

Void (*lrop_freeLopsInvoke)(InvokeInfo *);

#include "lrop_sap.h"

#ifdef LINT_ARGS /*{*/

extern void lrop_invokeInit (Int); 
extern InvokeInfo *lrop_invokeInfoCreate (); 
extern void lrop_invokeInfoFree (InvokeInfo *); 
extern void lrop_invokeInfoInit (InvokeInfo *); 
extern Int lrop_refInit (Int); 
extern short int lrop_refNew (InvokeInfo *); 
extern SuccFail lrop_refKeep (Int refNu, InvokeInfo *); 
extern InvokeInfo *lrop_refToPerformer(short unsigned int refNu, N_SapAddr *nSapAddr);
extern InvokeInfo *lrop_refToInvoker(short unsigned int refNu, N_SapAddr *nSapAddr);
extern void lrop_refFree (InvokeInfo *);
extern void LOPS_freeInvoke (InvokeInfo *);

#else

extern void lrop_invokeInit (); 
extern InvokeInfo *lrop_invokeInfoCreate ();
extern void lrop_invokeInfoFree (); 
extern void lrop_invokeInfoInit (); 
extern void lrop_refInit (); 
extern short int lrop_refNew (); 
extern SuccFail lrop_refKeep (); 
extern InvokeInfo *lrop_refToInvoker ();
extern InvokeInfo *lrop_refToPerformer ();
extern void lrop_refFree ();
extern void LOPS_freeInvoke ();

#endif

#ifdef __STDC__
#else
#endif

#endif	/*}*/
