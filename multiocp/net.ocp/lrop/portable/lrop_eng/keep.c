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
 * File: keep.c
 *
 * Function:
 *   lrop_pduSeqInit(PduSeq *p)    
 *   lrop_relPdu(InvokeInfo *invokeInfo, PduSeq *pduSeq)
 *   lrop_relAllPdu(InvokeInfo *invokeInfo)
 *   lrop_pduRetrieve(InvokeInfo *invokeInfo, PduSeq *pduSeq)
 *   lrop_pduKeep(InvokeInfo *invokeInfo, PduSeq *pduSeq, DU_View du)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: keep.c,v 1.19 1996/10/25 00:38:06 kamran Exp $";
#endif /*}*/

#include "estd.h"
#include "eh.h"
#include "local.h"
#include "extfuncs.h"


/*<
 * Function:    lrop_pduInit
 *
 * Description: Initialize PDU queue. 
 *
 * Arguments:   Pointer to DU view.
 *
 * Returns:     0 on successful completion, -1 otherwise.
 *
>*/

SuccFail
lrop_pduSeqInit(PduSeq *p)    
{
    if (p) {
       QU_INIT(p);            
       return (SUCCESS);
    }
    return (FAIL);
}


/*<
 * Function:    lrop_relPdu
 *
 * Description: The lrop_relPdu deletes pdu data in the allocated buffers.
 *
 * Arguments:   Invoke information, PDU.
 *
 * Returns:     0 on successful completion, -1 on unsuccessful completion.
 *
>*/

SuccFail
lrop_relPdu(InvokeInfo *invokeInfo, PduSeq *pduSeq)
{
    DU_View sdu;
#if 0	/* NOTYET */
    DU_View nextsdu;
#endif

#ifndef FAST
    if (invokeInfo == 0  ||  pduSeq == 0  ||  pduSeq->first == 0) {	
	EH_problem("lrop_relPdu: called with invalid argument\n");
	return (FAIL);
    }
#endif

#if 0		/* NOTYET: The real problem is not masking interrupts at
			   queue insert time. This should be removed when
			   signal-based fix is verified */

    for (sdu = QU_FIRST(pduSeq); 
	 ! QU_EQUAL(sdu, pduSeq); sdu = nextsdu) {
	nextsdu = QU_NEXT(sdu);
	if (sdu == nextsdu) {
	    EH_problem("lrop_relPdu: DU already released!\n");
	    return (FAIL);
	}
	QU_REMOVE(sdu);
#ifdef FUTURE
	_timeStamp = _duTimeGet(sdu);	
#endif
	TM_TRACE((DU_modCB, DU_MALLOC, "lrop_relPdu: DU_free: sdu=0x%lx\n",
		 sdu));

	DU_free(sdu);
    }
#else
	sdu = QU_FIRST(pduSeq);
        if (! QU_EQUAL(sdu, (DU_View)pduSeq)) {
	    QU_REMOVE(sdu);
       	    if (sdu) {
	    	TM_TRACE((DU_modCB, DU_MALLOC, 
			 "lrop_relPdu: DU_free: sdu=0x%lx\n", sdu));
	    	DU_free(sdu);
	    }
	}
#endif
    return (SUCCESS);
}



/*<
 * Function:    lrop_relAllPdu
 *
 * Description: Deletes all data in all allocated buffers.
 *
 * Arguments:   Invoke information.
 *
 * Returns:     0 on successful completion, -1 on unsuccessful completion.
 *
>*/

SuccFail
lrop_relAllPdu(InvokeInfo *invokeInfo)
{
    Int retVal = (SUCCESS);

#ifndef FAST
    if (invokeInfo == 0) {	
	EH_problem("lrop_relAllPdu: called with invalid argument\n");
	return (FAIL);
    }
#endif

    if (lrop_relPdu(invokeInfo, &invokeInfo->invokePduSeq) == FAIL) {
	EH_problem("lrop_relAllPdu: lrop_relPdu failed\n");
	retVal = FAIL;
    }
    if (lrop_relPdu(invokeInfo, &invokeInfo->resultPduSeq) == FAIL) {
	EH_problem("lrop_relAllPdu: lrop_relPdu failed\n");
	retVal = FAIL;
    }

    return retVal;
}


/*<
 * Function:    lrop_pduRetrieve
 *
 * Description:
 *   Right now with just one PDU type to be retrieved, the SEQ_ always only
 *   has one entry. If there was to be more, then the PUD type or some
 *   other indication must be passed to the function.
 *
 * Arguments: Invoke info, PDU seq.
 *
 * Returns:   Pointer to Data Unit view.
 *
>*/

DU_View 
lrop_pduRetrieve(InvokeInfo *invokeInfo, PduSeq *pduSeq)
{
    DU_View sdu, du;

    for (sdu = QU_FIRST(pduSeq); ! QU_EQUAL(sdu, pduSeq); sdu = QU_NEXT(sdu)) {
	du = DU_link(sdu);
	return (du);
    }
    return ((DU_View) NULL);
}


/*<
 * Function:    lrop_pduKeep
 *
 * Description: Store PDU (for future retrieval and checking against the future
 *              PDUs for duplication).
 *
 * Arguments:   Invoke info, pdu.
 *
 * Returns:     0 if successful, -1 if unsuccessful.
 *
>*/

Void
lrop_pduKeep(InvokeInfo *invokeInfo, PduSeq *pduSeq, DU_View du)
{
    QU_INSERT(du, pduSeq);

#ifdef FUTURE
    if (lrop_duTimeSet(du, TMR_getFreeCnt()) == FAIL) {
    	return (FAIL);
    } else {
    	return (SUCCESS);
    }
#endif
}

