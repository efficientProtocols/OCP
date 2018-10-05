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
 * File: pduin.c
 *
 * Description: This file contains the code for analyzing incoming PDU headers.
 *
 * Functions:
 *   SuccFail lrop_sendFailurePdu(N_SapAddr *naddr, 
 *                                short unsigned int invokeRefNu, 
 *                                unsigned char reason)
 *   lrop_getInvoke(struct Pdu *pdu)
 *   lrop_inPdu(DU_View du)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: pduin.c,v 1.23 1996/09/20 23:46:43 kamran Exp $";
#endif /*}*/


#include "estd.h"
#include "eh.h"
#include "addr.h"
#include "invoke.h"
#include "lropdu.h"

#include "du.h"
#include "byteordr.h"

#include "lropfsm.h"
#include "nm.h"
#include "layernm.h"

#include "target.h"
#include "sap.h"
#include "udp_if.h"
#include "lrop_sap.h"

#include "local.h"
#include "extfuncs.h"


extern Counter lrop_badAddrCounter;
extern Counter lrop_opRefusedCounter;
extern FSM_TransDiagram *lrop_CLPerformerTransDiag(void);

LOCAL Pdu lrop_pdu;


/*<
 * Function:    lrop_sendFailurePdu
 *
 * Description: Send a FAILURE PDU to destination reference.
 *
 * Arguments:   Network SAP, invoke reference number, failure reason.
 *
 * Returns:     0 if successful, -1 if unsuccessful.
 *
>*/

SuccFail 
lrop_sendFailurePdu(T_SapSel *tSapSel, N_SapAddr *naddr, 
			     short unsigned int invokeRefNu, 
			     unsigned char reason)
{
#ifdef FUTURE
    union BO_Swap swap;
#endif
    register unsigned char *p;
    unsigned char *pdustart;
    DU_View du;
    int len;

    len = 3;

    TM_TRACE((LROP_modCB, TM_ENTER,
	    "lrop_sendFailurePdu: (rcv %u PDU) refNu=%u reason=%u\n",
	    lrop_pdu.pdutype & 0xff, invokeRefNu, reason & 0xff));

    if ( (du = DU_alloc(G_duMainPool, 0)) == 0 ) {
	EH_problem("lrop_sendFailurePdu: DU_alloc failed\n");	
	return FAIL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, 
             "lrop_sendFailurePdu: DU_alloc: du=0x%lx\n", du));

    DU_prepend(du, len);
    p = DU_data(du);
    pdustart = p;

    BO_put1(p, FAILURE_PDU);
    BO_put1(p, invokeRefNu);
    BO_put1(p, reason);
/*
    len = p - pdustart - 1;
    *pdustart = len;
*/
/*    (*LROP_lowDesc.dataReq)(LROP_SAP, naddr, lrop_pdu.qos, du); */
    if (UDP_dataReq(lrop_udpSapDesc, tSapSel ,naddr, du) == FAIL) {
	if (du) {

	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lrop_sendFailurePdu: DU_free: du=0x%lx\n", du));

	    DU_free(du);
	}
	return (FAIL);
    }

#if FUTURE
    NM_incCounter(NM_LSROS, &lrop_pduSentCounter, 1);
#endif

#ifdef TM_ENABLED
    tm_pduPr(LROP_modCB, TM_PDUIN, "->", du, MAX_LOG);
#endif

    if (du) {

        TM_TRACE((DU_modCB, DU_MALLOC, 
                 "lrop_sendFailurePdu: DU_free: du=0x%lx\n", du));

        DU_free(du);
    }

    return (SUCCESS);

} /* lrop_sendFailurePdu */


/*<
 * Function:    lrop_outOfInvokeId
 *
 * Description: LOPS is out of invoke id. Send a FAILURE PDU.
 *
 * Arguments:   invoke info, Network SAP, failure reason.
 *
 * Returns:     O if successful, -1 otherwise.
 *
>*/
SuccFail
lrop_outOfInvokeId(InvokeInfo *invok, T_SapSel *tSapSel, 
			    N_SapAddr *naddr, 
			    unsigned char reason)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_outOfInvokeId: reason=%x\n", reason));
    return lrop_sendFailurePdu(tSapSel, naddr, invok->invokeRefNu, reason);
}



/*<
 * Function:    lrop_getInvoke
 *
 * Description: 
 *  Select an InvokeInfo for an incoming PDU.
 *  Search list of InvokeInfos and find one that matches
 *  the SAP-IDs in case of INVOKE-PDU; and InvokeRefNu
 *  for all other types of PDUs.
 *
 * Arguments: PDU.
 *
 * Returns:   Invoke info.
 *
>*/

STATIC InvokeInfo *
lrop_getInvoke(struct Pdu *pdu)
{
    register InvokeInfo *invokeInfo;
    SapInfo *sap;

    /* Process INVOKE PDUs.  */

    if (pdu->pdutype == INVOKE_PDU) {

	/*
	 * If the performer SAP is not activated, send a FAILURE PDU
	 */

	if ((sap = lrop_getSapInfo(pdu->toLSROSap)) == (SapInfo *) NULL) {
    	    TM_TRACE((LROP_modCB, TM_ENTER, 
	     	     "lrop_getInvoke: getSapInfo Failed\n"));
	    lrop_sendFailurePdu(pdu->fromTsapSel, pdu->fromNsapAddr, 
			        pdu->invokeRefNu, 2);
              /* user not responding but it's really a baddAddress */
	    NM_incCounter(NM_LSROS, &lrop_opRefusedCounter, 1);
	    NM_incCounter(NM_LSROS, &lrop_badAddrCounter, 1);
	    return ((InvokeInfo *) NULL);
	}

	/*
	 * Check for duplicated INVOKE PDU. An INVOKE PDU is a duplicate if
	 * destination reference and UDP-SAP address matches. 
	 */

	for (invokeInfo = sap->invokeSeq.first; 
             invokeInfo != (InvokeInfo *) &sap->invokeSeq;
	     invokeInfo = invokeInfo->next) {
	    if (invokeInfo->invokeRefNu == pdu->invokeRefNu &&
		N_sapAddrCmp(&invokeInfo->remNsapAddr, pdu->fromNsapAddr)==0) {
		return (invokeInfo);
	    }
	}

	/*
	 * Non-duplicate INVOKE PDU is received. Select an available InvokeInfo
	 * in order to generate a LROP-Invoke.Ind event. 
	 */

	if ((invokeInfo = lrop_invokeInfoCreate(sap)) != (InvokeInfo *) NULL) {

	    /* Select CL-Performer Transition-Diagram for invoke (machine) */
	    if (invokeInfo->locSap->functionalUnit == LSRO_2Way) {
	    	invokeInfo->transDiag = lrop_2CLPerformerTransDiag();
	    } else {
	    	invokeInfo->transDiag = lrop_CLPerformerTransDiag();
	    }
	    invokeInfo->curState              = invokeInfo->transDiag->state;
	    invokeInfo->performingNotInvoking = TRUE;

	    return (invokeInfo);
	}

	/*
	 * We are out of resources.
	 */

    	TM_TRACE((LROP_modCB, TM_ENTER, 
	         "lrop_getInvoke: Out of resources\n"));

	if (lrop_sendFailurePdu(pdu->fromTsapSel, pdu->fromNsapAddr, 
			        pdu->invokeRefNu, 1) == FAIL) {
	    return ((InvokeInfo *) NULL);	/* differentiate this case */
	}

	return ((InvokeInfo *) NULL);
    }

    /*
     * Given the refNu find me the invokeInfo.
     */

    switch (pdu->pdutype) {

    case INVOKE_PDU:
    case ACK_PDU:
	if ((invokeInfo = lrop_refToPerformer(pdu->invokeRefNu, pdu->fromNsapAddr))
	    == (InvokeInfo *)NULL) {
	    EH_problem("lrop_getInvoke: no invokeInfo found for ACK RefNu\n");
	    return ((InvokeInfo *) NULL);
	}
	return (invokeInfo);

    case RESULT_PDU:
	if ((invokeInfo = lrop_refToInvoker(pdu->invokeRefNu, pdu->fromNsapAddr)) 
	    == (InvokeInfo *)NULL) {
	    EH_problem("lrop_getInvoke: no invokeInfo found for RESULT RefNu\n");
	    lrop_sendFailurePdu(pdu->fromTsapSel, pdu->fromNsapAddr, 
				pdu->invokeRefNu, 
				1 /* NOTYET mismatched_reference */);
	    return ((InvokeInfo *) NULL);
	}
	return (invokeInfo);

    case ERROR_PDU:
	if ((invokeInfo = lrop_refToInvoker(pdu->invokeRefNu, pdu->fromNsapAddr)) 
	    == (InvokeInfo *)NULL) {
	    EH_problem("lrop_getInvoke: no invokeInfo found for RESULT RefNu\n");
	    lrop_sendFailurePdu(pdu->fromTsapSel, pdu->fromNsapAddr, 
				pdu->invokeRefNu, 
				1 /* NOTYET mismatched_reference */);
	    return ((InvokeInfo *) NULL);
	}
        invokeInfo->errorValue = pdu->errorValue;
	return (invokeInfo);

    case FAILURE_PDU:
	if ((invokeInfo = lrop_refToInvoker(pdu->invokeRefNu, pdu->fromNsapAddr)) 
	    == (InvokeInfo *)NULL ) {

	    EH_problem("lrop_getInvoke: no invokeInfo found for Failure RefNu\n");
	    return ((InvokeInfo *) NULL);
	}

 	if (invokeInfo->invokeRefNu != pdu->invokeRefNu) {
	    EH_problem("lrop_getInvoke: invokeInfo found but RefNu doesn't match\n");
	    return ((InvokeInfo *) NULL);
	}

        invokeInfo->failureReason = pdu->failureValue;
	return (invokeInfo);

    default:
	EH_problem("lrop_getInvoke: Unknown PDU type");
	return ((InvokeInfo *) NULL);
    }

} /* lrop_getInvoke() */


/*<
 * Function:    lrop_inPdu
 *
 * Description: 
 *   Read the PDU header on an incoming PDU. 
 *   Construct the PDU structure and strip the header from the data buffer. 
 *   If there is no data in the buffer then free it.
 *
 * Arguments: PDU
 *
 * Returns:   Invoke info.
 *
>*/

LOCAL
InvokeInfo *
lrop_inPdu(DU_View du)
{
    register unsigned char *p;
    register struct Pdu    *pdu;
    InvokeInfo    *invoke;
    unsigned char *pstart;
    unsigned char c;
    int  count;
    Int  released;

    pdu = &lrop_pdu;
    pdu->data    = (DU_View) NULL;
    pdu->pdutype = 0;
    pdu->badpdu  = FALSE;

    invoke = (InvokeInfo *) NULL;

    p      = DU_data(du);
    pstart = p;
    count  = DU_size(du);

    if (count == 0) {
	released = 0;

        TM_TRACE((LROP_modCB, DU_MALLOC, "lrop_inPdu: DU_free: du=0x%lx\n", du));

	DU_free(du);
	return ((InvokeInfo *) NULL);
    }

    /* This SDU does contain something. */

#ifdef TM_ENABLED
    tm_pduPr( LROP_modCB, TM_PDUIN, "<-", du, MAX_LOG);
#endif


    /* PCI, Byte1 , PDU-TYPE + remLsroSapSel or encodingType ... */
    BO_get1(c, p);
    pdu->pdutype = (c & 0x07);

    /* Validate PDU type */

    if (pdu->pdutype > 5 ) {
	goto badpdu;
    }

    /* Setup default parameters */

    /* PDU type dependent processing */

    switch (pdu->pdutype) {

    case INVOKE_PDU:

	/* PCI, BYTE-1 */
	pdu->toLSROSap   = (c & 0xF0) >> 4;	
	pdu->fromLSROSap = pdu->toLSROSap - 1;  /* From is the Invoker, To is the Performer */				
	/* PCI Byte-2 */
	BO_get1(pdu->invokeRefNu, p);

	/* PCI, Byte-3 , operationValue + EncodingType */
	BO_get1(c, p);  

	pdu->operationValue = c & 0x3F;
	pdu->encodingType   = (c & 0xC0) >> 6;

	break;

    case RESULT_PDU:

	/* PCI, BYTE-1 */
	pdu->encodingType = (c & 0xC0) >> 6;	

	/* PCI Byte-2 */
	BO_get1(pdu->invokeRefNu, p);

	break;

    case ERROR_PDU:

	/* PCI, BYTE-1 */
	pdu->encodingType = (c & 0xC0) >> 6;	

	/* PCI Byte-2 */
	BO_get1(pdu->invokeRefNu, p);

	/* PCI Byte-3 */
	BO_get1(pdu->errorValue, p);

	break;

    case ACK_PDU:

	/* PCI Byte-2 */
	BO_get1(pdu->invokeRefNu, p);

	if ((invoke = lrop_getInvoke(pdu)) == (InvokeInfo *) NULL)
	    goto ignore;

	break;

    case FAILURE_PDU:

	/* PCI, BYTE-2 */
	BO_get1(pdu->invokeRefNu, p);

	/* PCI Byte-3 */
	BO_get1(pdu->failureValue, p);

	if ((invoke = lrop_getInvoke(pdu)) == (InvokeInfo *) NULL)
	    goto ignore;
	break;

    default:
	EH_problem("lrop_inPdu: Unknown PDU type");
	goto ignore;
    }

    /* Remove The PCI */

    DU_strip(du, p - pstart);
    count = DU_size(du);

    /* NOTYET */
    /* Verify size limits here */

    if (count == 0 && pdu->pdutype == ACK_PDU) {

        TM_TRACE((LROP_modCB, DU_MALLOC, "lrop_inPdu: DU_free: du=0x%lx\n", du));

	DU_free(du);
	du = (DU_View) NULL;
	released = 1;
    }

    pdu->data = du;


    /*
     * If the received PDU has not already been associated with its
     * InvokeInfo, do the association now. 
     */

    if (invoke == (InvokeInfo *) NULL) {
	if ((invoke = lrop_getInvoke(pdu)) == (InvokeInfo *) NULL) {
	    goto ignore;
	}
    }

    return (invoke);

    /*
     * PDU has been received which could not be associated with an operation
     * We will ignore this PDU. 
     */

ignore:
    if (du != (DU_View)0 ) {

    	TM_TRACE((LROP_modCB, TM_ENTER, 
		 "lrop_inPdu: PDU ignored du=0x%lx\n", du));

        TM_TRACE((LROP_modCB, DU_MALLOC, 
	     	 "lrop_inPdu: DU_free: du=0x%lx refcnt=%d\n", 
	         du, du->bufInfo->refcnt));

	DU_free(du);
    }

    TM_TRACE((LROP_modCB, TM_PDUIN,
             "lrop_inPdu:          PDU received and ignored, type=%x\n", 
             pdu->pdutype & 0xff));

    return ((InvokeInfo *) NULL);

    /*
     * Badly formatted PDU received. Free the buffer and reset blocking
     * parameters. It will also be counted a a bad PDU for NM purposes. 
     */

badpdu:
    if (du != (DU_View)0 ) {

    	TM_TRACE((LROP_modCB, DU_MALLOC, "lrop_inPdu: DU_free: du=0x%lx\n", du));

	DU_free(du);
    }
    /* NOTYET     NM_incCounter(NM_LSROS, &lrop_invalidPduCounter, 1); */

    TM_TRACE((LROP_modCB, TM_PDUIN,
             "lrop_inPdu:          Bad PDU received type=%x\n", 
             pdu->pdutype & 0xff));

    return ((InvokeInfo *) NULL);

} /* lrop_inPdu(DU_View du) */
