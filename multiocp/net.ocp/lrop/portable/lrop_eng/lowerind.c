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
 * File: lowerind.c
 *
 * Description: This file contains the interface event routines between
 *              this and the lower layer.
 *
 * Functions:   lower_dataInd(T_SapSel *remTsapSel,
 *  	                      N_SapAddr *remNsapAddr,
 *	                      T_SapSel *locTsapSel,
 *	                      N_SapAddr *locNsapAddr,
 *	                      DU_View data)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: lowerind.c,v 1.16 1997/01/15 19:48:32 kamran Exp $";
#endif /*}*/

#include "eh.h"
#include "lropdu.h"

#include "local.h"
#include "extfuncs.h"

extern Pdu lrop_pdu;
extern InvokeInfo *lrop_inPdu (DU_View bp); 


/*<
 * Function:    lower_dataInd
 *
 * Description: This function is performed from the lower layer to indicate 
 *              the arrival of a SDU.
 *
 * Arguments:   Transport SAP, network SAP, data unit.
 *
 * Returns:     	
 *
>*/

LOCAL Int
lower_dataInd(T_SapSel *remTsapSel,
	      N_SapAddr *remNsapAddr,
	      T_SapSel *locTsapSel,
	      N_SapAddr *locNsapAddr,
	      DU_View data)
{
    InvokeInfo *invoke;
    Int gotVal;

    /* Save network address of source and destination */

    lrop_pdu.fromTsapSel  = remTsapSel;
    lrop_pdu.fromNsapAddr = remNsapAddr;

    lrop_pdu.toTsapSel    = locTsapSel;
    lrop_pdu.toNsapAddr   = locNsapAddr;

    if ((invoke = lrop_inPdu(data)) == (InvokeInfo *) NULL) {
	TM_TRACE((LROP_modCB, TM_ENTER, 
    		 "lower_dataInd: SAP not active or received PDU can "
		 "not be associated with operation\n"));

/* NOTYET: send failure PDU? */

	if (data) {
	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lower_dataInd: DU_free: data=0x%lx\n", data));
	    DU_free(data);
	}
    	return (FAIL);
    }
	
    switch ( lrop_pdu.pdutype ) {
    case INVOKE_PDU:
	gotVal = FSM_runMachine((Void *)invoke,  lsfsm_EvtPduInvoke);
	break;

    case RESULT_PDU:
    case ERROR_PDU:
	gotVal = FSM_runMachine((Void *)invoke,  lsfsm_EvtPduResult);
	break;

    case ACK_PDU:
 	gotVal = FSM_runMachine((Void *)invoke,  lsfsm_EvtPduAck);
	break;

    case FAILURE_PDU:
        gotVal = FSM_runMachine((Void *)invoke,  lsfsm_EvtPduFailure);
	break;

    default:
	EH_problem("lower_dataInd: Invalid PDU type");
	return (FAIL);
    }

    if (gotVal < 0) {
	TM_TRACE((LROP_modCB, TM_ENTER, 
                 "lower_dataInd: FSM_runMachine returned negative value: %d\n",
	         gotVal));

        if (lrop_pdu.data != (DU_View) NULL) {

	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lower_dataInd: DU_free: lrop_pdu.data=0x%lx\n",
		     lrop_pdu.data));

	    DU_free(lrop_pdu.data);
	}
    }

    return (SUCCESS);

} /* lower_dataInd() */
