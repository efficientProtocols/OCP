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
 * File: perfact.c
 *
 * Description:
 *	This file contains the actions to be taken at each step 
 *      of the LSROS transition Diagrams (performer side).
 *
 * Functions:
 *   tr_clPerformer01(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer02(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer03(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer04(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer05(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer06(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer07(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer08(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer09(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer10(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer11(Void *machine, Void *userData, FSM_EventId evtId)
 *   tr_clPerformer12(Void *machine, Void *userData, FSM_EventId evtId)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: perfact.c,v 1.32 1997/03/14 04:45:55 kamran Exp $";
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

Bool dropResultConfirm = FALSE;

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
tr_clPerformer01(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (lrop_pdu.pdutype != INVOKE_PDU ) {
	EH_problem("tr_clPerformer01: PDU is not invoke PDU\n");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer01: Called with invalid invoke pointer\n");
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

	EH_problem("tr_clPerformer01: lrop_invokeInd failed\n");
        TM_TRACE((DU_modCB, DU_MALLOC, 
	         "tr_clPerformer01: DU_free: lrop_pdu.data=0x%lx\n", 
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);

        lrop_sendFailurePdu(&invoke->remTsapSel, &invoke->remNsapAddr, 
			    invoke->invokeRefNu, LSRO_FailureUserNotResponding);
	return (FAIL);
    }

#ifndef NO_UPSHELL
    if (lrop_tmrCreate(invoke, lrop_perfNoResponseTimer, 
		       invoke->perfNoResponse, 0) == FAIL) {
	EH_problem("tr_clPerformer01: lrop_tmrCreate failed\n");
	return (FAIL);
    }
#endif

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
tr_clPerformer02(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = SUCCESS;
    DU_View du;

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifdef FUTURE
    if ( DU_size(FSM_evtInfo.resultReq.data) > RESULT_REQ_SIZE) {
	EH_problem("tr_clPerformer02: Result Request too long\n");
	return (FAIL);
    }
#endif

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer02: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    /* To be used for resultCnf */
    invoke->isResultNotError = FSM_evtInfo.resultReq.isResultNotError;

    invoke->encodingType  = FSM_evtInfo.resultReq.encodingType;
    invoke->userInvokeRef = FSM_evtInfo.resultReq.userInvokeRef;

    if (lrop_refKeep(invoke->invokeRefNu, invoke) == FAIL) {
	EH_problem("tr_clPerformer02: lrop_refKeep failed\n");
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
	    EH_problem("tr_clPerformer02: lrop_resultPdu failed\n");
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
	    EH_problem("tr_clPerformer02: lrop_errorPdu failed\n");
	    return (FAIL);
    	}
    }

#ifdef FUTURE
    if (lrop_pduKeep(invoke, &invoke->resultPduSeq, du) == FAIL) {
	EH_problem("tr_clPerformer02: lrop_pduKeep failed\n");
        DU_free(du); 
	return (FAIL);
    }
#else
    lrop_pduKeep(invoke, &invoke->resultPduSeq, du);
#endif

    if ((du = lrop_pduRetrieve(invoke, &invoke->resultPduSeq)) == NULL) {
        EH_problem("tr_clPerformer02: PDU can not be retrieved "
		   "for retransmission\n");
        DU_free(du); 
        return (FAIL);
    }
    if (UDP_dataReq(lrop_udpSapDesc,
		    &(invoke->remTsapSel),
		    &(invoke->remNsapAddr),
		    du) 
        == FAIL) {
	EH_problem("tr_clPerformer02: UDP_dataReq failed\n");
	retVal = FAIL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "tr_clPerformer02: DU_free: du=0x%lx\n", du));

    DU_free(du); 

    if (lrop_tmrCreate(invoke, lrop_resultPduRetranTimer, 
		       invoke->retransTimeout, 0) == FAIL) {
	EH_problem("tr_clPerformer02: lrop_tmrCreate failed\n");
	return (FAIL);
    }
    lrop_tmrCancel(invoke, lrop_perfNoResponseTimer);

    return retVal;

} /* tr_clPerformer02() */

/*
 *  
 *  EVENT:
 *	LSRO-ACK-PDU received.
 *
 *  CURRENT STATE:
 *	ACK-PDU Wait.
 *
 *  ACTION:
 *	Initialize invoke refernce number timer.
 *	Issue LSROS-RESULT.confirm or LSROS-ERROR.confirm.
 *
 *  RESULTING STATE:
 *	Performer RefNu Wait.
 *
 */

Int
tr_clPerformer03(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = SUCCESS;

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifdef FUTURE
    if ( lrop_pdu.pdutype != ACK_PDU ) {
	EH_problem("tr_clPerformer03: PDU is not ACK PDU\n");
	return (FAIL);
    }
#endif

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer03: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    lrop_tmrCancel(invoke, lrop_allTimer);

    if (lrop_relPdu(invoke, &invoke->resultPduSeq) == FAIL) {
	EH_problem("tr_clPerformer03: lrop_relPdu failed\n");
	retVal = FAIL;
    }

/*** REMOVE (begin) ***/
if (dropResultConfirm)	{		/* this is used for testing only */

    invoke->failureReason = LSRO_FailureTransmission;

    if (lrop_failureInd(invoke, 
			invoke->userInvokeRef,
			invoke->failureReason) == FAIL) {
	EH_problem("tr_clPerformer09: lrop_failureInd failed\n");
	retVal = FAIL;
    }

    lrop_tmrCancel(invoke, lrop_allTimer);

    if (lrop_relPdu(invoke, &invoke->resultPduSeq) == FAIL) {
	EH_problem("tr_clPerformer09: lrop_relPdu failed\n");
	retVal = FAIL;
    }

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	/* Release refNu */
	EH_problem("tr_clPerformer09: lrop_tmrCreate failed\n");
  	return FAIL;  
    }
    return retVal;
}
/*** REMOVE (end) ***/

    if (invoke->isResultNotError) {
	retVal = lrop_resultCnf(invoke, invoke->userInvokeRef);
    } else {
	retVal = lrop_errorCnf(invoke, invoke->userInvokeRef);
    }	

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	/* Release refNu */
	EH_problem("tr_clPerformer03: lrop_tmrCreate failed\n");
  	return FAIL;  
    }

    return retVal;

} /* tr_clPerformer03() */

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
tr_clPerformer04(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if ( lrop_pdu.pdutype != INVOKE_PDU ) {
	EH_problem("tr_clPerformer04: PDU is not invoke PDU\n");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer04: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    TM_TRACE((LROP_modCB, TM_PDUIN, 
	     "<- (DUPLICATE) Invoke PDU while waiting for LSROS user (Performer machine)"));

    if (lrop_pdu.data != (DU_View)0) {

        TM_TRACE((DU_modCB, DU_MALLOC, 
	         "tr_clPerformer04: DU_free: lrop_pdu.data=0x%lx\n", 
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
    }

    return (SUCCESS);
}

/*
 *  
 *  EVENT:
 *	LSRO-RESULT-PDU or LSRO-ERROR-PDU retransmission timer expired.
 *
 *  CURRENT STATE:
 *	ACK-PDU Wait.
 *
 *  ACTION:
 *	Retransmit LSRO-RESULT-PDU or LSRO-ERROR-PDU while number of 
 *	  retransmissions is less than maximum number of retransmissions.
 *	Increment the number of retransmissions timer.
 *
 *  RESULTING STATE:
 *	Stay in ACK-PDU Wait.
 *
 */

Int
tr_clPerformer05(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = SUCCESS;
    DU_View du;

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifdef FUTURE
    if (FSM_evtInfo.tmrInfo.name != lrop_resultPduRetranTimer) {
	EH_problem("tr_clPerformer05: Event is not Result PDU Retrans Timer\n");
	return (FAIL);
    }
#endif

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer05: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    if (FSM_evtInfo.tmrInfo.datum >= invoke->nuOfRetrans) {
#if 0
        if ((du = lrop_pduRetrieve(invoke, &invoke->resultPduSeq)) 
	    != (DU_View)0) {
            DU_free(du); 
	}
#endif
	if (lrop_tmrCreate(invoke, lrop_lastTimer, invoke->rwait, 0) == FAIL) {
	    EH_problem("tr_clPerformer05: lrop_tmrCreate failed\n");
	    return (FAIL);
	} else {
            return (SUCCESS);
	}
    } else {
	if ((du = lrop_pduRetrieve(invoke, &invoke->resultPduSeq)) == NULL) {
	    EH_problem("tr_clPerformer05: PDU can not be retrieved for retransmission\n");
	    return (FAIL);
        }
	if (UDP_dataReq(lrop_udpSapDesc,
		        &(invoke->remTsapSel),
		        &(invoke->remNsapAddr),
		        du)
	    == FAIL) {
	    EH_problem("tr_clPerformer05: UDP_dataReq failed\n");
	    retVal = FAIL;
    	}

        TM_TRACE((LROP_modCB, TM_PDUIN, 
	         "\n-> (RETRANSMIT) Result PDU while waiting for ACK and retrans timer went off (Performer)"));

        TM_TRACE((DU_modCB, DU_MALLOC, 
		 "tr_clPerformer05: DU_free: du=0x%lx\n", du));

	DU_free(du);

	if (lrop_tmrCreate(invoke,
		       lrop_resultPduRetranTimer, 
		       invoke->retransTimeout,
		       FSM_evtInfo.tmrInfo.datum + 1) == FAIL) {
	    EH_problem("tr_clPerformer05: lrop_tmrCreate failed\n");
	    return (FAIL);
	}

	if (retVal == SUCCESS) {
	    NM_incCounter(NM_LSROS, &lrop_pduRetranCounter, 1);
	}
    }

    return retVal;

} /* tr_clPerformer05() */


/*
 *
 *  EVENT:
 *	Duplicate LSRO-INVOKE-PDU received.
 *
 *  CURRENT STATE:
 *	ACK-PDU Wait.
 *
 *  ACTION:
 *	Send result PDU.
 *
 *  RESULTING STATE:
 *	Stay in ACK-PDU Wait state.
 * 
 */

Int
tr_clPerformer06(Void *machine, Void *userData, FSM_EventId evtId)
{
    DU_View du;

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifdef FUTURE
    if (lrop_pdu.pdutype != INVOKE_PDU ) {
	EH_problem("tr_clPerformer06: PDU is not invoke PDU\n");
	return (FAIL);
    }
#endif

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer06: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    TM_TRACE((LROP_modCB, TM_PDUIN, 
	     "<- (DUPLICATE) Invoke PDU while waiting for ACK (Performer machine)"));

    if (lrop_pdu.data != (DU_View)0) {

	TM_TRACE((DU_modCB, DU_MALLOC, 
                 "tr_clPerformer06: DU_free: lrop_pdu.data=0x%lx\n",
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
    }

    if ((du = lrop_pduRetrieve(invoke, &invoke->resultPduSeq)) == NULL) {
        EH_problem("tr_clPerformer06: PDU can not be retrieved for retransmission\n");
        return (FAIL);
    }
    if (UDP_dataReq(lrop_udpSapDesc,
		    &(invoke->remTsapSel),
		    &(invoke->remNsapAddr),
		    du) 
	== FAIL) {

	EH_problem("tr_clPerformer06: UDP_dataReq failed\n");
        TM_TRACE((DU_modCB, DU_MALLOC, 
		 "tr_clPerformer06: DU_free: du=0x%lx\n", du));

	DU_free(du);
	return FAIL;
    }

    TM_TRACE((LROP_modCB, TM_PDUIN, 
	     "\n-> (RETRANSMIT) Result PDU because duplicate invoke PDU received (Performer)"));

    TM_TRACE((DU_modCB, DU_MALLOC, "tr_clPerformer06: DU_free: du=0x%lx\n", du));

    DU_free(du);

    lrop_tmrCancel(invoke, lrop_resultPduRetranTimer);

    if (lrop_tmrCreate(invoke, lrop_resultPduRetranTimer, 
		       invoke->retransTimeout, 0) == FAIL) {
	EH_problem("tr_clPerformer06: lrop_tmrCreate failed\n");
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
tr_clPerformer07(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (lrop_pdu.pdutype != INVOKE_PDU) {
	EH_problem("tr_clPerformer07: PDU is not Invoke\n");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer07: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    if (lrop_pdu.data != (DU_View)0) {

	TM_TRACE((DU_modCB, DU_MALLOC, 
		 "tr_clPerformer07: DU_free: lrop_pdu.data=0x%lx\n",
		 lrop_pdu.data));

	DU_free(lrop_pdu.data);
    }

    lrop_tmrCancel(invoke, lrop_refNuTimer);

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	EH_problem("tr_clPerformer07: lrop_tmrCreate failed\n");
	return FAIL;
    }

    return (SUCCESS);
}


/*
 *
 *  EVENT:
 *	Internal failure.
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
tr_clPerformer08(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = SUCCESS;

#ifdef TM_ENABLED
    static char taskN[100] = "Release invoke: ";
#endif

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer08: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    invoke->failureReason = LSRO_FailureLocResource;

    if (lrop_failureInd(invoke, 
			invoke->userInvokeRef, 
			invoke->failureReason) == FAIL) {
	EH_problem("tr_clPerformer08: lrop_failureInd failed\n");
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
	EH_problem("tr_clPerformer08: lrop_relPdu failed\n");
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
	     "tr_clPerformer08: DU_free: lrop_pdu.data=0x%lx\n", lrop_pdu.data));

#if 0   /* check this */
    DU_free(lrop_pdu.data);
#endif

    NM_incCounter(NM_LSROS, &lrop_completeOperationCounter, -1);

    return retVal;

}


/*
 *
 *  EVENT:
 *	Last timer.
 *
 *  CURRENT STATE:
 *	ACK-PDU Wait.
 *
 *  ACTION:
 *	Issue LSROS-FAILURE.indication.
 *	Initialize invoke reference number timer.
 *
 *  RESULTING STATE:
 *	Performer RefNu Wait.
 * 
 */

Int
tr_clPerformer09(Void *machine, Void *userData, FSM_EventId evtId)
{
    Int retVal = (SUCCESS);

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    invoke->failureReason = LSRO_FailureTransmission;

    if (lrop_failureInd(invoke, 
			invoke->userInvokeRef,
			invoke->failureReason) == FAIL) {
	EH_problem("tr_clPerformer09: lrop_failureInd failed\n");
	retVal = FAIL;
    }

    lrop_tmrCancel(invoke, lrop_allTimer);

    if (lrop_relPdu(invoke, &invoke->resultPduSeq) == FAIL) {
	EH_problem("tr_clPerformer09: lrop_relPdu failed\n");
	retVal = FAIL;
    }

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	/* Release refNu */
	EH_problem("tr_clPerformer09: lrop_tmrCreate failed\n");
  	return FAIL;  
    }

    return retVal;
}


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
tr_clPerformer10(Void *machine, Void *userData, FSM_EventId evtId)
{
#ifdef TM_ENABLED
    static char taskN[100] = "Release invoke: ";
#endif

    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (FSM_evtInfo.tmrInfo.name != lrop_refNuTimer) {
	EH_problem("tr_clPerformer10: Event is not reference number timer\n");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer10: Called with invalid invoke pointer\n");
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


/*
 *
 *  EVENT:
 *	Duplicate LSRO-ACK-PDU received.
 *
 *  CURRENT STATE:
 *	Performer RefNu Wait.
 *
 *  ACTION:
 *	Reset Invoke Reference Number timer.
 *
 *  RESULTING STATE:
 *	Stay in Performer RefNu Wait state.
 * 
 */

Int
tr_clPerformer11(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    if (lrop_pdu.pdutype != ACK_PDU) {
	EH_problem("tr_clPerformer11: PDU is not Ack\n");
	return (FAIL);
    }

#ifndef FAST
    if (invoke == 0) {
	EH_problem("tr_clPerformer11: Called with invalid invoke pointer\n");
	return (FAIL);
    }
#endif

    if (lrop_pdu.data != (DU_View)0) {
	DU_free(lrop_pdu.data);

	TM_TRACE((DU_modCB, DU_MALLOC, "tr_clPerformer11: DU_free: du=0x%lx\n",
		 lrop_pdu.data));
    }

    lrop_tmrCancel(invoke, lrop_refNuTimer);

    if (lrop_tmrCreate(invoke, lrop_refNuTimer, invoke->refKeepTime, 0) 
	== FAIL) {
	EH_problem("tr_clPerformer11: lrop_tmrCreate failed\n");
	return FAIL;
    }

    return (SUCCESS);
}


/*
 *
 *  EVENT:
 *	Hold-on ACK request.
 *
 *  CURRENT STATE:
 *	Invoke PDU Received.
 *
 *  ACTION:
 *	Send Hold-on LSRO-ACK-PDU.
 *
 *  RESULTING STATE:
 *	Stay in Invoke PDU Received.
 * 
 */

Int
tr_clPerformer12(Void *machine, Void *userData, FSM_EventId evtId)
{
    InvokeInfo *invoke;
    invoke = (InvokeInfo *)machine;

    EH_problem("tr_clPerformer12: Hold-on ack is not implemented yet\n");

    /* NOTYET */
/*
    User said hang on while I perform
*/
	return (SUCCESS);
}
