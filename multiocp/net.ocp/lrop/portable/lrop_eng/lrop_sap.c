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
 * File: sap.c
 *
 * Description: SAP processing.
 *
 * Functions:
 *   lrop_sapInit(Int nuOfSaps)
 *   LROP_sapBind(LROP_SapSel sapSel, ...)
 *   LROP_sapUnbind(LROP_SapSel sapSel)
 *   lrop_getSapInfo(LROP_SapSel sapSel)
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: lrop_sap.c,v 1.9 1997/01/11 00:49:19 kamran Exp $";
#endif /*}*/

#include  "target.h"   /* NOTYET */
#include  "estd.h"
#include  "eh.h"
#include  "queue.h"
#include  "seq.h"
#include  "tm.h"
#include  "lsro.h"
#include  "invoke.h"
#include  "lrop_cfg.h"

#include "lrop_sap.h"
#include "local.h"

STATIC SapInfoSeq   sapInfoSeq;
STATIC SEQ_PoolDesc sapInfoPool;

LOCAL TM_ModDesc lrop_tmDesc;

extern InvokeInfo *refToInvokerMap[NREFS + 1];
extern InvokeInfo *refToPerformerMap[NREFS + 1];
extern unsigned short refBase;
extern unsigned short newRef;	

#define LROP_SapSel LSRO_SapSel


/*<
 * Function:    lrop_sapInit
 * 
 * Description: Initialize the LROP_ module.
 *	        Create a Tracing Module.
 *
 * Arguments:   Number of SAP's.
 *
 * Returns:     None.
 * 
>*/

PUBLIC Void
lrop_sapInit(Int nuOfSaps)
{
    static Bool virgin = TRUE;

    if ( !virgin ) {
	return;
    }
    virgin = FALSE;

    sapInfoPool = SEQ_poolCreate(sizeof(*sapInfoSeq.first), 0);
    QU_INIT(&sapInfoSeq);
}



/*<
 * Function: LROP_sapBind
 *
 * Description: Bind an Address to a Service User.
 *
 * Arguments: SAP selector, SAP descriptor (outgoing argument), action
 *            functions.
 *
 * Returns:   0 if successful, a negative error value if unsuccessful.
 * 
>*/

PUBLIC Int
LROP_sapBind(LROP_SapDesc *sapDesc, LROP_SapSel sapSel, 
	     LROP_FunctionalUnit functionalUnit,
	     int (*invokeInd) (LROP_SapSel 	   locLROPSap,
			       LROP_SapSel 	   remLROPSap,
			       T_SapSel		   *remTsap,
			       N_SapAddr	   *remNsap,
			       LROP_InvokeDesc     invoke,
			       LROP_OperationValue operationValue,
			       LROP_EncodingType   encodingType,
			       DU_View		   parameter),
	     int (*resultInd) (LROP_InvokeDesc   invoke,
			       LROP_UserInvokeRef  userInvokeRef,
			       LROP_EncodingType encodingType,
			       DU_View		 parameter),
	     int (*errorInd) (LROP_InvokeDesc   invoke,
			      LROP_UserInvokeRef  userInvokeRef,
			      LROP_EncodingType	encodingType,
			      LROP_ErrorValue   errorValue,
			      DU_View 		parameter),
	     int (*resultCnf) (LROP_InvokeDesc  invoke,
			       LROP_UserInvokeRef  userInvokeRef),
	     int (*errorCnf) (LROP_InvokeDesc   invoke,
			      LROP_UserInvokeRef  userInvokeRef),
	     int (*failureInd) (LROP_InvokeDesc   invoke,
			        LROP_UserInvokeRef  userInvokeRef,
				LROP_FailureValue failureValue))
{
    SapInfo *sapInfo;

    /*
     * If no SAP-ID specified, we will only allow one TS user which will
     * match the null SAP-ID. 
     */
    if (sapSel == 0 ) {
	EH_problem("LROP_sapBind: Bad Sap");
	return  -3; 
    }

    if ( sapSel > LSRO_MAX_SAP_NO ) { /* no need to check min because unsigned */
	EH_problem("LROP_sapBind: Invalid Sap number\n");
    	TM_TRACE((LROP_modCB, TM_ENTER, 
		 "               Accepted range: %d to %d\n",
		 LSRO_MIN_SAP_NO, LSRO_MAX_SAP_NO));
	return  -6; 
    }

    if ( ! (sapInfo = (SapInfo *) SEQ_elemObtain(sapInfoPool)) ) {
	EH_problem("LROP_sapBind: SEQ_elemObtain failed");
	return -2;
    }

    /* If the SAP-ID is already in use, the function fails.  */
    if (lrop_getSapInfo(sapSel) != (SapInfo *) 0) {
	EH_problem("LROP_sapBind: Sap in use");
	return -4; 
    }

    if (functionalUnit != LSRO_3Way  &&  functionalUnit != LSRO_2Way) {
	EH_problem("LROP_sapBind: Invalid functional unit. It should be 2-way or 3-way\n");
	return -5; 
    }

    /* So the SAP can be created */

    sapInfo->sapSel = sapSel;
    sapInfo->functionalUnit = functionalUnit;

    sapInfo->invokeInd  = invokeInd;
    sapInfo->resultInd  = resultInd;
    sapInfo->errorInd   = errorInd;
    sapInfo->resultCnf  = resultCnf;
    sapInfo->errorCnf   = errorCnf;
    sapInfo->failureInd = failureInd;

    QU_INIT(sapInfo);
    QU_INSERT(&sapInfoSeq, sapInfo);

    QU_INIT(&sapInfo->invokeSeq);

    TM_TRACE((LROP_modCB, TM_ENTER,
	    "LROP_sapBind (before return) sapSel=%d  functionalUnit=%d\n", 
	    sapSel, functionalUnit));

#ifndef NO_UPSHELL
    TM_TRACE((LROP_modCB, TM_ENTER, 
    	     "***** lrop_engine compiled WITHOUT NO_UPSHELL for one process call back *****\n"));
    *sapDesc = sapInfo;
#else
    TM_TRACE((LROP_modCB, TM_ENTER, 
    	     "***** lrop_engine compiled WITH NO_UPSHELL for one process call back *****\n"));
    *sapDesc = (LROP_SapDesc) (unsigned long)sapSel;
#endif

    return 0;
}



/*<
 * Function:    LROP_sapUnbind
 *
 * Description: Unbind a service user.
 *
 * Arguments:   SAP selector.
 *
 * Returns:     0 if successful, a negative error value if unsuccessful.
 * 
>*/

PUBLIC SuccFail
LROP_sapUnbind(LROP_SapSel sapSel)
{
    SapInfo *sap;

    TM_TRACE((LROP_modCB, TM_ENTER,
	    "LROP_sapUnbind (Entry) sapSel=%d\n", sapSel));

    if ((sap = lrop_getSapInfo(sapSel)) == (SapInfo *) 0) {
        TM_TRACE((LROP_modCB, TM_ENTER,
	         "LROP_sapUnbind: Sap not active (%d)!\n", sapSel));
	return ( FAIL ); /*	return -2;   *** err.h ***/
    }

    /*
     * We refuse to unregister a SAP-ID which is in use.
     */

    if (! QU_EQUAL(QU_FIRST(&sap->invokeSeq), &sap->invokeSeq)) {
#if 0
	EH_problem("LROP_sapUnbind: SAP has active invokations");
	return ( FAIL ); /*	return -3;   *** err.h ***/
#endif
 	Int refNu;
	InvokeInfo *invokeInfo;
	InvokeInfo *invokeInfoNext;

#if 0		/* NOTYET: The real problem is not masking interrupts at
			   queue insert time. This should be removed when
			   signal-based fix is verified */

	for (invokeInfo = sap->invokeSeq.first; 
             invokeInfo != (InvokeInfo *) &sap->invokeSeq;) {
	    if ( invokeInfo->performingNotInvoking == TRUE ) {
            	for (refNu = 0; refNu < NREFS; refNu++) {
		    if (invokeInfo == refToPerformerMap[refNu]) {
		    	refToPerformerMap[refNu] = (InvokeInfo *)0;
		    }
	    	}
	    } else {
            	for (refNu = 0; refNu < NREFS; refNu++) {
		    if (invokeInfo == refToInvokerMap[refNu]) {
			refToInvokerMap[refNu] = (InvokeInfo *)0;
		    }
	    	}
	    }
#else
	invokeInfo = sap->invokeSeq.first; 
	if ( invokeInfo->performingNotInvoking == TRUE ) {
            for (refNu = 0; refNu < NREFS; refNu++) {
	    	if (invokeInfo == refToPerformerMap[refNu]) {
		    refToPerformerMap[refNu] = (InvokeInfo *)0;
		}
	    }
	} else {
            for (refNu = 0; refNu < NREFS; refNu++) {
	    	if (invokeInfo == refToInvokerMap[refNu]) {
		    refToInvokerMap[refNu] = (InvokeInfo *)0;
		}
	    }
	}
#endif

            lrop_tmrCancel(invokeInfo, lrop_allTimer);

	    invokeInfoNext = invokeInfo->next;

	    lrop_refFree(invokeInfo);
	    lrop_freeLopsInvoke(invokeInfo); 
	    lrop_invokeInfoFree(invokeInfo);
		
	    invokeInfo = invokeInfoNext;
#if 0
	}
#endif
    	QU_INIT(&sap->invokeSeq);
    }

    /* Free SAP table entry  */

    QU_REMOVE(sap);
    SEQ_elemRelease(sapInfoPool, sap);

    TM_TRACE((LROP_modCB, TM_ENTER,
	    "lrop_sapUnbind (exit) sapSel=%d\n", sapSel));

    return ( SUCCESS );
}



/*<
 * Function:    lrop_getSapInfo
 *
 * Description: Get SAP information. Find entry in local service user table.
 *
 * Arguments:   SAP selector.
 *
 * Returns:     Pointer to SAP information struct if successful, NULL if 
 *              unsuccessful.
 *
>*/

LOCAL SapInfo *
lrop_getSapInfo(LSRO_SapSel sapSel)
{
    SapInfo *sap;

    for (sap = QU_FIRST(&sapInfoSeq); 
	 ! QU_EQUAL(sap, &sapInfoSeq);
	 sap = QU_NEXT(sap)) {

	if (!sap) {
	    EH_problem("lrop_getSapInfo: sap queue pointing to zero\n")
    	    return ((SapInfo *) 0);
	}

	if (sapSel == sap->sapSel) {
	    return (sap);
	}
    }

    TM_TRACE((LROP_modCB, TM_ENTER,
	    "lrop_getSapInfo: No sapInfo found for sapSel(%d)\n", sapSel));
#ifdef TM_ENABLED
    for (sap = QU_FIRST(&sapInfoSeq); 
     	 ! QU_EQUAL(sap, &sapInfoSeq);
	 sap = QU_NEXT(sap)) {
    	TM_TRACE((LROP_modCB, TM_ENTER,
                 "lrop_getSapInfo: Active SAP: %d\n", sap->sapSel));
    	}
#endif

    return ((SapInfo *) 0);
}


