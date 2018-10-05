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
 * File: perfact2.c
 *
 * Description:
 *	This file contains the actions to be taken at each step 
 *      of the LSROS transition Diagrams (performer side).
 *
 * Functions:
 *   tr_2clPerformer01(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer02(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer03(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer04(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer05(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer06(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer07(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_2clPerformer08(Void *machine, Void *userData, FSM_EventId evtId)
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: perfact2.c,v 1.14 1996/11/09 00:24:15 fletch Exp $";
#endif /*}*/

#include "estd.h"
#include "eh.h"

#include "invoke.h"
#include "lropfsm.h"
#include "layernm.h"
#include "lropdu.h"
#include "udp_if.h"

#include "target.h"

#include "sch.h"

#include "extfuncs.h"
#include "local.h"

extern Pdu lrop_pdu;

extern Void (*lrop_freeLopsInvoke)(InvokeInfo *);

/*
 *
 *  EVENT:
 *	LSRO-INVOKE-PDU received.
 *
 *  CURRENT STATE:
 *	CL Performer Start
 *
 *  ACTION:
 *	Issue LSROS-INVOKE.indication primitive.
 *
 *  RESULTING STATE:
 *	Invoke PDU Received.
 */

Int
tr_2clPerformer01(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (lrop_pdu.pdutype != INVOKE_PDU ) {
	EH_problem("tr_2clPerformer01: PDU is not invoke PDU");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer01: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    invoke->remLsroSapSel = lrop_pdu.fromLSROSap;
    invoke->remTsapSel    = *lrop_pdu.fromTsapSel;
    invoke->remNsapAddr   = *lrop_pdu.fromNsapAddr;
    invoke->encodingType  = lrop_pdu.encodingType;
    invoke->invokeRefNu   = lrop_pdu.invokeRefNu;


    lrop_tmrSetValues(invoke);

    if (lrop_invokeInd(invoke,
		   	invoke->locSap->sapSel,
		   	invoke->remLsroSapSel,
		   	&invoke->remTsapSel,
		   	&invoke->remNsapAddr,
		   	lrop_pdu.operationValue,
		   	invoke->encodingType,
		   	(DU_View) lrop_pdu.data) == FAIL) {

        TM_TRACE((DU_modCB, DU_MALLOC, 
	         "tr_2clPerformer01: DU_free: lrop_pdu.data=0x%lx\n", 
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
	return (FAIL);
    }

    if (lrop_tmrCreate(invoke, lrop_perfNoResponseTimer, 
		       invoke->perfNoResponse, 0) == FAIL) {
	EH_problem("tr_2clPerformer01: lrop_tmrCreate failed\n");
	return (FAIL);
    }

    NM_incCounter(NM_LSROS, &lrop_completeOperationCounter, 1);	

    return (SUCCESS);
}


/*
 *
 *  EVENT:
 *	LSROS-RESULT.request or LSROS-ERROR-request received.
 *
 *  CURRENT STATE:
 *	Invoke PDU Received.
 *
 *  ACTION:
 *	Add invoke reference number to the active list.
 *	Transmit LSRO-RESULT-PDU or LSRO-ERROR-PDU.
 *	Set LSRO-RESULT-PDU or LSRO-ERROR-PDU retransmission timer.
 *
 *  RESULTING STATE:
 *
 */

Int
tr_2clPerformer03(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = SUCCESS;
    DU_View du;

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifdef FUTURE
    if ( DU_size(FSM_evtInfo.resultReq.data) > RESULT_REQ_SIZE) {
	EH_problem("tr_2clPerformer03: Result Request too long");
	return (FAIL);
    }
#endif

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer03: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    /* To be used for resultCnf */
    invoke->isResultNotError = FSM_evtInfo.resultReq.isResultNotError;

    invoke->encodingType  = FSM_evtInfo.resultReq.encodingType;
    invoke->userInvokeRef = FSM_evtInfo.resultReq.userInvokeRef;

    if (lrop_refKeep(invoke->invokeRefNu, invoke) == FAIL) {
	EH_problem("tr_2clPerformer03: lrop_refKeep failed\n");
	return (FAIL);
    }

    lrop_tmrSetValues(invoke);

    if (invoke->isResultNotError) {
    	if ((du = lrop_resultPdu(invoke, 
			     	invoke->invokeRefNu, 
			     	invoke->locSap->sapSel,
			     	invoke->remLsroSapSel,
			     	FSM_evtInfo.resultReq.encodingType,
			     	FSM_evtInfo.resultReq.parameter)) == NULL) {
	    EH_problem("tr_2clPerformer03: lrop_resultPdu failed\n");
	    return (FAIL);
    	} 
    } else {
    	if ((du = lrop_errorPdu(invoke, 
			     	invoke->invokeRefNu, 
			     	invoke->locSap->sapSel,
			     	invoke->remLsroSapSel,
			     	FSM_evtInfo.errorReq.encodingType,
			     	FSM_evtInfo.errorReq.errorValue,
			     	FSM_evtInfo.errorReq.parameter)) == NULL) {
	    EH_problem("tr_2clPerformer03: lrop_errorPdu failed\n");
	    return (FAIL);
    	}
   }

#ifdef FUTURE
    if (lrop_pduKeep(invoke, &invoke->resultPduSeq, du) == FAIL) {
	EH_problem("tr_2clPerformer03: lrop_pduKeep failed\n");
        DU_free(du); 
	return (FAIL);
    }
#else
    lrop_pduKeep(invoke, &invoke->resultPduSeq, du);
#endif

    if ((du = lrop_pduRetrieve(invoke, &invoke->resultPduSeq)) == NULL) {
        EH_problem("tr_2clPerformer03: PDU can not be retrieved for retransmission\n");
        return (FAIL);
    }
    if (UDP_dataReq(lrop_udpSapDesc,
		    &(invoke->remTsapSel),
		    &(invoke->remNsapAddr),
		    du) 
        == FAIL) {
	retVal = FAIL;
    }

/*
    TM_TRACE((DU_modCB, DU_MALLOC, 
	     "tr_2clPerformer03: DU_free: du=0x%lx\n", du));
    DU_free(du); 
*/
    if (lrop_tmrCreate(invoke, lrop_inactivityTimer, 
		       invoke->inactivityDelay, 0) == FAIL) {
        EH_problem("tr_2clPerformer03: lrop_tmrCreate failed\n");
	return (FAIL);
    }
    lrop_tmrCancel(invoke, lrop_perfNoResponseTimer);

    return retVal;

} /* tr_2clPerformer03() */

/*
 *
 *  EVENT:
 *	Duplicate LSRO-INVOKE-PDU received.
 *
 *  CURRENT STATE:
 *	Invoke PDU Received.
 *
 *  ACTION:
 *	No action (ignore the duplicate LSRO-INVOKE-PDU).
 *
 *  RESULTING STATE:
 *	Stay in "Inovke PDU Received".
 *
 */

Int
tr_2clPerformer02(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if ( lrop_pdu.pdutype != INVOKE_PDU ) {
	EH_problem("tr_2clPerformer02: PDU is not invoke PDU");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer02: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    if (lrop_pdu.data != (DU_View)0) {

        TM_TRACE((DU_modCB, DU_MALLOC, 
	         "tr_2clPerformer02: DU_free: lrop_pdu.data=0x%lx\n", 
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
    }

    return (SUCCESS);

} /* tr_2clPerformer02() */

/*
 *
 *  EVENT:
 *	Duplicate LSRO-INVOKE-PDU received.
 *
 *  CURRENT STATE:
 *	Result PDU Retransmit
 *
 *  ACTION:
 *	Retransmit LSRO-RESULT-PDU or LSRO-ERROR PDU
 *
 *  RESULTING STATE:
 *	Stay in Result-PDU-Retransmit state.
 * 
 */

Int
tr_2clPerformer05(Void *machine, Void *userData, FSM_EventId evtId)
{
    DU_View du;

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifdef FUTURE
    if (lrop_pdu.pdutype != INVOKE_PDU ) {
	EH_problem("tr_2clPerformer05: PDU is not invoke PDU");
	return (FAIL);
    }
#endif

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer05: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    if (lrop_pdu.data != (DU_View)0) {

	TM_TRACE((DU_modCB, DU_MALLOC, 
                 "tr_2clPerformer05: DU_free: lrop_pdu.data=0x%lx\n",
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
    }

    if ((du = lrop_pduRetrieve(invoke, &invoke->resultPduSeq)) == NULL) {
        EH_problem("tr_2clPerformer05: PDU can not be retrieved "
		   "for retransmission\n");
        return (FAIL);
    }
    if (UDP_dataReq(lrop_udpSapDesc,
		    &(invoke->remTsapSel),
		    &(invoke->remNsapAddr),
		    du) 
	== FAIL) {

        TM_TRACE((DU_modCB, DU_MALLOC, 
		 "tr_2clPerformer05: DU_free: du=0x%lx\n", du));

	DU_free(du);
	return FAIL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "tr_2clPerformer05: DU_free: du=0x%lx\n", du));

    DU_free(du);

    lrop_tmrCancel(invoke, lrop_resultPduRetranTimer);

    if (lrop_tmrCreate(invoke, lrop_resultPduRetranTimer, 
		       invoke->retransTimeout, 0) == FAIL) {
        EH_problem("tr_2clPerformer05: lrop_tmrCreate failed\n");
	return (FAIL);
    }

    return (SUCCESS);
}


/*
 *
 *  EVENT:
 *	Duplicate Invoke PDU received.
 *
 *  CURRENT STATE:
 *	Performer RefNu Wait.
 *
 *  ACTION:
 *	Reset Invoke Reference Number timer.
 *
 *  RESULTING STATE:
 *	Stay in Performer RefNu Wait.
 * 
 */

Int
tr_2clPerformer07(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (lrop_pdu.pdutype != INVOKE_PDU) {
	EH_problem("tr_2clPerformer07: PDU is not Invoke");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer07: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    if (lrop_pdu.data != (DU_View)0) {

	TM_TRACE((DU_modCB, DU_MALLOC, 
		 "tr_2clPerformer07: DU_free: lrop_pdu.data=0x%lx\n",
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
    }

    lrop_tmrCancel(invoke, lrop_refNuTimer);

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	EH_problem("tr_2clPerformer07: lrop_tmrCreate failed\n");
	return FAIL;
    }

    return (SUCCESS);

} /* tr_2clPerformer07() */


/*
 *
 *  EVENT:
 *	Internal failure (including User not responding)
 *
 *  CURRENT STATE:
 *	Inoke PDU Received.
 *
 *  ACTION:
 *	Send LSRO-FAILURE-PDU.
 *
 *  RESULTING STATE:
 *	CL Performer Start.
 * 
 */

Int
tr_2clPerformer04(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = SUCCESS;

#ifdef TM_ENABLED
    static char taskN[100] = "Release invoke: ";
#endif

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer04: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    invoke->failureReason = LSRO_FailureLocResource;

    if (lrop_failureInd(invoke, 
			invoke->userInvokeRef,
			invoke->failureReason) == FAIL) {
	EH_problem("tr_2clPerformer04: lrop_failureInd failed\n");
	retVal = FAIL;
    }

    if (lrop_sendFailurePdu(&invoke->remTsapSel, &invoke->remNsapAddr, 
		        invoke->invokeRefNu, 
			LSRO_FailureUserNotResponding) == FAIL) { 
						/* it can be local resource too */
	EH_problem("tr_2clPerformer04: lrop_sendFailurePdu failed\n");
	retVal = FAIL;
    }

    lrop_tmrCancel(invoke, lrop_allTimer);

    if (lrop_relPdu(invoke, &invoke->invokePduSeq) == FAIL) {
	EH_problem("tr_2clPerformer04: lrop_relPdu failed\n");
	retVal = FAIL;
    }

#if 0
    lrop_refFree(invoke);
    lrop_freeLopsInvoke(invoke);
    lrop_invokeInfoFree(invoke);
#endif
#ifdef TM_ENABLED
        strcpy(taskN, "Release invoke: ");
    	SCH_submit ((Void *)lrop_relInvoke, 
		           (Ptr) invoke, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	SCH_submit ((Void *)lrop_relInvoke, 
		           (Ptr) invoke, 
		           SCH_PSEUDO_EVENT);
#endif

    TM_TRACE((DU_modCB, DU_MALLOC, 
	     "tr_2clPerformer04: DU_free: lrop_pdu.data=0x%lx\n", lrop_pdu.data));

#if 0  /*** check this ***/
    DU_free(lrop_pdu.data);
#endif

    NM_incCounter(NM_LSROS, &lrop_completeOperationCounter, -1);

    return retVal;

} /* tr_2clPerformer04() */


/*
 *
 *  EVENT:
 *	Inactivity Timer.
 *
 *  CURRENT STATE:
 *	Result PDU Retransmit
 *
 *  ACTION:
 *	Issue LSROS-RESULT.confirm or LSRO-ERROR.confirm.
 *	Initialize invoke reference number timer.
 *
 *  RESULTING STATE:
 *	Performer RefNu Wait.
 * 
 */

Int
tr_2clPerformer06(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = (SUCCESS);

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    lrop_tmrCancel(invoke, lrop_allTimer);

    if (lrop_relPdu(invoke, &invoke->resultPduSeq) == FAIL) {
	EH_problem("tr_2clPerformer06: lrop_relPdu failed\n");
	retVal = FAIL;
    }

    if (invoke->isResultNotError) {
	retVal = lrop_resultCnf(invoke, invoke->userInvokeRef);
    } else {
	retVal = lrop_errorCnf(invoke, invoke->userInvokeRef);
    }	

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	EH_problem("tr_2clPerformer06: lrop_tmrCreate failed\n");
	/* Release refNu */
  	return FAIL;  
    }

    return retVal;

} /* tr_2clPerformer06() */


/*
 *
 *  EVENT:
 *	Reference number timer expired.
 *
 *  CURRENT STATE:
 *	Performer RefNu Wait.
 *
 *  ACTION:
 *	Release the invoke reference number.
 *
 *  RESULTING STATE:
 *	CL Performer Start.
 * 
 */

Int
tr_2clPerformer08(Void *machine, Void *userData, FSM_EventId evtId)
{
#ifdef TM_ENABLED
    static char taskN[100] = "Release invoke: ";
#endif

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (FSM_evtInfo.tmrInfo.name != lrop_refNuTimer) {
	EH_problem("tr_2clPerformer08: Event is not reference number timer");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_2clPerformer08: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif
    
#if 0
    lrop_refFree(invoke);
    lrop_freeLopsInvoke(invoke);
    lrop_invokeInfoFree(invoke);
#endif

#ifdef TM_ENABLED
        strcpy(taskN, "Release invoke: ");
    	return SCH_submit ((Void *)lrop_relInvoke, 
		           (Ptr) invoke, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_relInvoke, 
		           (Ptr) invoke, 
		           SCH_PSEUDO_EVENT);
#endif


    return (SUCCESS);
}

