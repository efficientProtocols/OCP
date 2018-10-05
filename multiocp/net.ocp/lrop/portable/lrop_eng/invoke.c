/*
 *  Copyright (C) 1995,1996  AT&T Wireless Services, Inc. All rights reserved.
 *  Copyright (C) 1995,1996  Neda Communications, Inc. All rights reserved.
 * 
 *  This software is furnished under a license and use, duplication,
 *  disclosure and all other uses are restricted to the rights specified
 *  in the written license between the licensee and copyright holders.
 * 
 */

/*+
 * File: invoke.c
 *
 * Description: Reference number and invoke ID initialization and handling.
 *
 * Functions:
 *    lrop_invokeInit(Int nuOfInvokes)
 *    lrop_invokeInfoCreate(SapInfo *sap)
 *    lrop_invokeInfoFree(InvokeInfo *invokeInfo)
 *    lrop_invokeInfoInit(InvokeInfo *invoke)
 *    lrop_refInit(Int)
 *    lrop_refNew(InvokeInfo *invoke)
 *    lrop_refKeep(Int refNu, InvokeInfo *invoke)
 *    lrop_refToPerformer(short unsigned int refNu)
 *    lrop_refToInvoker(short unsigned int refNu)
 *    lrop_refFree(InvokeInfo *invoke)
 *    lrop_freeLopsInvoke(InvokeInfo *invoke)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: invoke.c,v 1.36 1996/12/23 10:42:39 kamran Exp $";
#endif /*}*/

#include "eh.h"
#include "estd.h"
#include "queue.h"
#include "seq.h"
#include "tm.h"
#include "sf.h"
#include "sap.h"

#include "lrop_sap.h"
#include "invoke.h"
#include "lropfsm.h"
#include "fsm.h"
#include "nm.h"
#include "layernm.h"

#include "target.h"

#include "extfuncs.h"
#include "local.h"

STATIC InvokeInfoSeq invokeInfoSeq;
STATIC SEQ_PoolDesc invokeInfoPool;

unsigned int nSapAddrOffset;	/* Offset of NSAP address from refMap (refNuMap)*/
    				/* &invokeInfo.refMap - &invokeInfo.remNsapAddr */
unsigned int invokeInfoOffset;  /* Offset of start of invokeInfo from refMap */
    				/* &invokeInfo.refMap - &invokeInfo */

Int totalRefNumbers = 0;

QU_Head *refToInvokerMap;
QU_Head *refToPerformerMap;

/*
 *  Map for quick translations between an Invoke-Ref-Number and
 *  the correspondent InvokeInfo structure.
 *
 *  refbase contains the first local reference number which is in use.
 *  This number is incremented by NREFS each time the LSROS layer
 *  is initialized, which will guarantee that we will not reuse the
 *  same reference number too soon.
 */

/* NOTYET -- All signed and unsigned comparisions should be reviewed */

Int refBase 	       = 0;		/* Reference Number Base */
unsigned short newRef  = 0;		/* New Reference Number  */


/*<
 * Function:    lrop_invokeInit
 *
 * Description: Create and initialize invoke ID pool and initialize
 *
 * Arguments:   Number of invoke IDs
 *
 * Returns:     None.
 *
>*/

LOCAL Void
lrop_invokeInit(Int nuOfInvokes)
{
    static Bool virgin = TRUE;
    InvokeInfo invokeInfo;

    if ( !virgin ) {
	return;
    }
    virgin = FALSE;

    if ((invokeInfoPool = SEQ_poolCreate(sizeof(*invokeInfoSeq.first), 0)) 
	== NULL) {
	return;
    }

    QU_INIT(&invokeInfoSeq);

    /* Calulate offsets from the head of the reference number mapping queue */

    nSapAddrOffset   =  (unsigned int)&invokeInfo.refMap - 
			(unsigned int)&invokeInfo.remNsapAddr;
    invokeInfoOffset =  (unsigned int)&invokeInfo.refMap - 
			(unsigned int)&invokeInfo;

} /* lrop_invokeInit() */



/*<
 * Function:    lrop_invokeInfoCreate
 *
 * Description: Create an invokation information entity.
 *
 * Arguments:   SAP.
 *
 * Returns:     Pointer to invoke information struct.
 *
>*/

LOCAL InvokeInfo *
lrop_invokeInfoCreate(SapInfo *sap)
{
    InvokeInfo *invokeInfo;

    if ( ! (invokeInfo = (InvokeInfo *) SEQ_elemObtain(invokeInfoPool)) ) {
	/* NM_incCounter(NM_LSROS, &lrop_noInvokeInfosCounter, 1); */
	EH_problem("lrop_invokeInfoCreate: SEQ_elemObtain failed\n");
	return ((InvokeInfo *) 0);
    }

    QU_MOVE(invokeInfo, &sap->invokeSeq);

    invokeInfo->locSap = sap;

    lrop_invokeInfoInit(invokeInfo);

    return (invokeInfo);

} /* lrop_invokeInfoCreate() */



/*<
 * Function:    lrop_invokeInfoFree
 *
 * Description: Release and invoke information entity.
 *
 * Arguments:   Pointer to invoke info struct.
 *
 * Returns:     None.
 *              If input parameter is a NULL pointer, no action is performed.
 *              If input parameter is an invalid pointer, undefined results.
 *
>*/

LOCAL Void
lrop_invokeInfoFree(InvokeInfo *invokeInfo)
{
    DU_View du;
#if 0 /* NOTYET */
    DU_View duNext;
#endif
    PduSeq *pduSeq;

    if (invokeInfo == (InvokeInfo *)0) {
	return;
    }

/*    SF_memRelease(invokeInfo->curState);  */
 
#if 0		/* NOTYET: The real problem is not masking interrupts at
			   queue insert time. This should be removed when
			   signal-based fix is verified */

   for (pduSeq = &invokeInfo->invokePduSeq, 
	du = (DU_View)QU_FIRST(pduSeq); 
        ! QU_EQUAL(du, (DU_View)pduSeq);
	du = duNext) {
	duNext = QU_NEXT(du);
	QU_REMOVE(du);

	if (du) {
	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lrop_invokeInfoFree: DU_free: du=0x%lx\n", du));
  	    DU_free (du);
	}
   }

   for (pduSeq = &invokeInfo->resultPduSeq, 
	du = (DU_View)QU_FIRST(pduSeq); 
        ! QU_EQUAL(du, (DU_View)pduSeq);
	du = duNext) {

	duNext = QU_NEXT(du);
	QU_REMOVE(du);

	if (du) {
	    TM_TRACE((DU_modCB, DU_MALLOC, 
                 "lrop_invokeInfoFree: DU_free: du=0x%lx\n", du));
  	    DU_free (du);
	}
   }
#else
   pduSeq = &invokeInfo->invokePduSeq;
   du = (DU_View)QU_FIRST(pduSeq); 
   if (! QU_EQUAL(du, (DU_View)pduSeq)) {
	QU_REMOVE(du);
	if (du) {
	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lrop_invokeInfoFree: DU_free: du=0x%lx\n", du));
  	    DU_free (du);
	}
   }

   pduSeq = &invokeInfo->resultPduSeq;
   du = (DU_View)QU_FIRST(pduSeq); 
   if (! QU_EQUAL(du, (DU_View)pduSeq)) {
   	QU_REMOVE(du);
       	if (du) {
	    TM_TRACE((DU_modCB, DU_MALLOC, 
                "lrop_invokeInfoFree: DU_free: du=0x%lx\n", du));
      	    DU_free (du);
 	}
   }
#endif

    QU_remove((QU_Element *)invokeInfo);
    SEQ_elemRelease(invokeInfoPool, invokeInfo);
}


/*<
 * Function:    lrop_invokeInfoInit
 *
 * Description: This procedure initializes an InvokeInfo before it is used.
 *
 * Arguments:   Pointer to invoke info.
 *
 * Returns:     None.
 *
>*/

LOCAL Void
lrop_invokeInfoInit(InvokeInfo *invoke)
{
    invoke->remNsapAddr.len = 0;	/* reset remote NSAP address length */
    invoke->remTsapSel.len  = 0;	/* reset remote TSAP selector length */

    QU_init((QU_Element *)&invoke->tmrQu);      /* Initialize Timer Queue */
    QU_init((QU_Element *)&invoke->refMap);     /* Initialize reference no map */

    lrop_pduSeqInit(&invoke->invokePduSeq);  /* Initialize invoke PDU sequence */
    lrop_pduSeqInit(&invoke->resultPduSeq);  /* Initialize result PDU sequence */
}

/*<
 * Function:    lrop_refInit
 *
 * Description: Initialize reference number tables.
 *
 * Arguments:   Total number of reference numbers.
 *
 * Returns:     0 if successfule, a negative error value otherwise.
 *
>*/

LOCAL Int
lrop_refInit(Int totalRefs)
{
    Int i;
    static Bool virgin = TRUE;
#ifdef TIME_AVAILABLE
    OS_ZuluDateTime *pDateTime;
    OS_Uint32 *pJulianDate;
#endif

    if (!virgin) {
    	TM_TRACE((LROP_modCB, TM_ENTER, 
		 "WARNING: lrop_refInit: Function is called more than once\n"));
	return -2;
    }

    virgin = FALSE;

    if (totalRefs <= 0) {
	EH_problem("lrop_refInit: Invalid Total Reference Numbers\n");
	return FAIL;
    }
    
    totalRefNumbers = totalRefs;

    refBase = -totalRefNumbers;
#ifdef TIME_AVAILABLE
    newRef = OS_timeMinSec();
    newRef = newRef % 900;
    newRef = newRef / 4;
#else
    newRef  =  totalRefNumbers;	
#endif

    if ((refToInvokerMap = 
	(QU_Head *)SF_memGet((totalRefNumbers + 1)*sizeof(QU_Head))) == 0) {
	EH_problem("lrop_refInit: Memory allocation failed for refToInvokerMap\n");
	return FAIL;
    } 
    if ((refToPerformerMap = 
	(QU_Head *)SF_memGet((totalRefNumbers + 1)*sizeof(QU_Head))) == 0) {
	EH_problem("lrop_refInit: Memory allocation failed for refToPerformerMap\n");
	return FAIL;
    } 

    for (i = 0; i <= totalRefNumbers; i++) {
	QU_INIT(&refToInvokerMap[i]);
	QU_INIT(&refToPerformerMap[i]);
    }
    refBase += totalRefNumbers;

    return (SUCCESS);

} /* lrop_refInit() */


/*<
 * Function:    lrop_refNew
 *
 * Description: Return an unused reference number
 *
 * Arguments:   Invoke info.
 *
 * Returns:     Reference number (a positive number) if successful, a negative
 *              error number if unsuccessful.
 *
>*/

LOCAL short 
lrop_refNew(InvokeInfo *invoke)
{
    Int i;
    QU_Element *elem;

    /* RefNew only applies to the invoker */
    if ( invoke->performingNotInvoking ) {
	EH_problem("lrop_refNew: called by performer\n");
        return (FAIL);
    }

    for (i = 0; i < totalRefNumbers; i++) { 
	if (++newRef >= totalRefNumbers) {
	    newRef = 0;
	}
       	for (elem = QU_FIRST(&refToInvokerMap[newRef]); 
	     ! QU_EQUAL(elem, &refToInvokerMap[newRef]); elem = QU_NEXT(elem) ) {
	    if (N_sapAddrCmp(&invoke->remNsapAddr, lrop_getNsapAddr(elem)) == 0) {
	    	break;
	    }
	}
	if (QU_EQUAL(elem, &refToInvokerMap[newRef])) {
            QU_INSERT(&refToInvokerMap[newRef], &invoke->refMap); 
	    return (newRef + refBase);
        }
    }

    EH_problem("lrop_refNew: Invoker Out of Reference numbers\n");
    return -1;    /*** err.h  ***/

} /* lrop_refNew() */


/*<
 * Function:    Associate the given invocation with the given reference number.
 *		and register the ref number as used.
 *
 * Description: This primitive returns a reference number to be used in
 *              identifying a connection. ????
 *
 * Arguments:   Reference number, pointer to invoke structure.
 *
 * Returns:	0 is successful, -1 otherwise
 *
>*/

LOCAL SuccFail
lrop_refKeep(Int refNu, InvokeInfo *invoke)
{
    QU_Element *elem;

    /* RefKeep only applies to the performer */
    if ( invoke->performingNotInvoking == FALSE ) {
	EH_problem("lrop_refKeep: called by performer\n");
	return ( FAIL );
    }

    if (refNu >= refBase && refNu < (refBase + totalRefNumbers)) {
       	for (elem = QU_FIRST(&refToPerformerMap[refNu]); 
	     ! QU_EQUAL(elem, &refToPerformerMap[refNu]); elem = QU_NEXT(elem) ) {
	    if (N_sapAddrCmp(&invoke->remNsapAddr, lrop_getNsapAddr(elem)) == 0) {
 		EH_problem("lrop_refKeep: Reference number already in use\n");
		return ( FAIL );
	    }
	}
       	QU_INSERT(&refToPerformerMap[refNu], &invoke->refMap); 
	return (SUCCESS);

    } else {
	EH_problem("lrop_refKeep: invalid reference number\n");
    	TM_TRACE((LROP_modCB, TM_ENTER, 
		 "lrop_refKeep: Invalid REF NO=%d\n", refNu));
	return ( FAIL );
    }

} /* lrop_refKeep() */



/*<
 * Function:    lrop_refToPerformer
 *
 * Description: Get a reference number and return the corresponding invoke ID.
 *
 * Arguments:   Reference number.
 *
 * Returns:     Inovke info for valid reference number, 0 otherwise.
 *
>*/

LOCAL InvokeInfo *
lrop_refToPerformer(short unsigned int refNu, N_SapAddr *nSapAddr)
{
    QU_Element *elem;
    /* only applies to the performer */

    if (refNu >= refBase && refNu < (refBase + totalRefNumbers)) {
       	for (elem = QU_FIRST(&refToPerformerMap[refNu]); 
	     ! QU_EQUAL(elem, &refToPerformerMap[refNu]); elem = QU_NEXT(elem) ) {
	    if (N_sapAddrCmp(lrop_getNsapAddr(elem), nSapAddr) == 0) {
		return (InvokeInfo *)lrop_getInvokeInfoAddr(elem);
	    }
	}
 	EH_problem("lrop_refKeep: No operation can be associated with the given reference number\n");
	return (InvokeInfo *)0;

    } else {
	EH_problem("lrop_refKeep: invalid reference number\n");
	return (InvokeInfo *)0;
    }

} /* lrop_refToPerformer() */


/*<
 * Function:    lrop_refToInvoker
 *
 * Description: Get reference number and return the corresponding invoke info.
 *
 * Arguments:   Reference number.
 *
 * Returns:     Inovke info for valid reference number, 0 otherwise.
 *
>*/

LOCAL InvokeInfo *
lrop_refToInvoker(short unsigned int refNu, N_SapAddr *nSapAddr)
{
    QU_Element *elem;
    /* only applies to the invoker */

    if (refNu >= refBase && refNu < (refBase + totalRefNumbers)) {
       	for (elem = QU_FIRST(&refToInvokerMap[refNu]); 
	     ! QU_EQUAL(elem, &refToInvokerMap[refNu]); elem = QU_NEXT(elem) ) {
	    if (N_sapAddrCmp(lrop_getNsapAddr(elem), nSapAddr) == 0) {
		return (InvokeInfo *)lrop_getInvokeInfoAddr(elem);
	    }
	}
 	EH_problem("lrop_refToInvoker: No operation can be associated with the given reference number\n");
	return (InvokeInfo *)0;

    } else {
	EH_problem("refToInvoker: invalid reference number\n");
	return (InvokeInfo *)0;
    }

} /* lrop_refToInvoker() */


/*<
 * Function:    lrop_refFree
 *
 * Description: Release reference number.
 *
 * Arguments:   Invoke info.
 *
 * Returns:     None.
 *
>*/

LOCAL Void
lrop_refFree(InvokeInfo *invoke)
{
   QU_REMOVE(&invoke->refMap);
   QU_INIT(&invoke->refMap);
}


/*<
 * Function:    LROP_freeLopsInvoke
 *
 * Description: Set call back function to release LOPS layer entities 
 *              when an invocation is done in LROP layer.
 *
 * Arguments:   Call back function.
 *
 * Returns:     None.
 *
>*/

Void
LROP_freeLopsInvoke(Void (*lops_freeInvoke)(InvokeInfo *))
{
   lrop_freeLopsInvoke = lops_freeInvoke;
}


/*<
 * Function:    lrop_relInvoke
 *
 * Description: Release invoke structure
 *
 * Arguments:   Pointer to invoke structure
 *
 * Returns:     None.
 *
>*/

Void
lrop_relInvoke(InvokeInfo *invoke)
{
   lrop_refFree(invoke); 		/* Free reference number */
   lrop_freeLopsInvoke(invoke); 	/* Free LOPS entries     */
   lrop_invokeInfoFree(invoke); 	/* Free invoke entries   */
}

