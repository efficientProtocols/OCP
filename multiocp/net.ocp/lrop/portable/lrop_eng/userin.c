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
 * File: userin.c
 *
 * Description:
 *   This file contains the action primitive interface between
 *   the LSROS layer and the user of the LSROS services.
 *
 * Functions:
 *   LROP_invokeReq
 *   LROP_errorReq
 *   LROP_resultReq
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: userin.c,v 1.29 1997/01/01 00:03:38 kamran Exp $";
#endif /*}*/

#include "eh.h"
#include "estd.h"
#include "lrop_sap.h"
#include "lropfsm.h"
#include "fsm.h"
#include "lrop_cfg.h"

#include "lsro.h"
#include "lrop.h"

#include "local.h" 
#include "extfuncs.h" 


/*<
 * Function:    LROP_invokeReq
 *
 * Description: LSROS-INVOKE.request
 *
 * Arguments:   Invoke descriptor, Local SAP selector, remote SAP selector, 
 *		Transport SAP selector, Network SAP selector, operation value, 
 *		encoding type, parameter.
 *
 * Returns:     0 if successful, -1 otherwise
 *
>*/

PUBLIC Int
LROP_invokeReq(LROP_InvokeDesc 	   *invokeDesc,
	       LROP_UserInvokeRef  userInvokeRef,
	       LROP_SapSel 	   locLSROSap,
	       LROP_SapSel 	   remLSROSap,
	       T_SapSel 	   *remTsap,
	       N_SapAddr 	   *remNsap,
	       LROP_OperationValue opValue,
	       LROP_EncodingType   encodingType,
	       DU_View 	 	   parameter)
{
    InvokeInfo *invoke;
    Int gotVal;
    SapInfo *sapInfo;
    DU_View lropView;

    TM_TRACE((LROP_modCB, TM_ENTER, "LROP_invokeReq entered\n"));

#ifdef AUTHENTICATE_DOMAIN
    if (lrop_authenticateIP(remNsap)) {
	EH_problem("\nProgram is talking to unauthorized network address\n");
	return (FAIL);
    }
#endif

    if ((sapInfo = lrop_getSapInfo(locLSROSap)) == (SapInfo *) NULL) {
	EH_problem("LROP_invokeReq: lrop_getSapInfo failed");
	return (FAIL);
    }

    if ((invoke = lrop_invokeInfoCreate(sapInfo)) == (InvokeInfo *) NULL) {
	EH_problem("LROP_invokeReq: lrop_invokeInfoCreate failed");
	return (FAIL);
    }

    TM_TRACE((LROP_modCB, TM_ENTER, "LROP_invokeReq: invokeInfo=0x%lx\n", 
	     invoke));

    if (parameter) {
        if (DU_size(parameter) > invokePduSize) {
            EH_problem("LROP_invokeReq: We shouldn't be here. Large data not supported by connectionless engine\n");
	    return (FAIL);
	}
    }

    FSM_evtInfo.invokeReq.userInvokeRef= userInvokeRef;
    FSM_evtInfo.invokeReq.remLSROSap   = remLSROSap;
    FSM_evtInfo.invokeReq.remTsap      = remTsap;
    FSM_evtInfo.invokeReq.remNsap      = remNsap;
    FSM_evtInfo.invokeReq.opValue      = opValue;
    FSM_evtInfo.invokeReq.encodingType = encodingType;
    FSM_evtInfo.invokeReq.parameter    = lropView = DU_link(parameter);

    /* Select CL-Invoker Transition-Diagram for this invoke (machine) */

    if (sapInfo->functionalUnit == LSRO_3Way) {
    	invoke->transDiag = lrop_CLInvokerTransDiag();

    } else if (sapInfo->functionalUnit == LSRO_2Way) {
    	invoke->transDiag = lrop_2CLInvokerTransDiag();

    } else {
        EH_problem("LROP_invokeReq: Invalid functional unit assigned to sap\n");
/*        DU_free(lropView); */
	return (FAIL);
    }

    invoke->curState  = invoke->transDiag->state;
    invoke->performingNotInvoking = FALSE;

    if ((gotVal = FSM_runMachine((Void *)invoke, lsfsm_EvtInvokeReq)) < 0) {
	EH_problem("LROP_invokeReq: FSM_runMachine failed");
/*        DU_free(lropView); */
	return (FAIL);
    }

    *invokeDesc = (LROP_InvokeDesc *)invoke;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	      "LROP_invokeReq: Before exit: invokeDesc=0x%lx\n",
	      (unsigned long)invokeDesc));

/*    DU_free(lropView); */

    return (SUCCESS);
}


/*<
 * Function:    LROP_resultReq
 *
 * Description: LSROS RESULT or ERROR request
 *
 * Arguments:   Type (result or error), invoke descriptor, encoding type, 
 *              parameter.
 *
 * Returns:     0 if successful, -1 if unsuccessful.  (?)
 *
>*/

PUBLIC  Int
LROP_resultReq(LROP_InvokeDesc invokeDesc,
	       LROP_UserInvokeRef userInvokeRef,
	       LROP_EncodingType encodingType, 
	       DU_View parameter)

{
    int gotVal;
    DU_View duLink = NULL;

    TM_TRACE((LROP_modCB, TM_ENTER, "LROP_resultReq\n"));

    if (parameter) {
    	duLink = DU_link(parameter);
    }

    FSM_evtInfo.resultReq.isResultNotError = 1;
    FSM_evtInfo.resultReq.userInvokeRef    = userInvokeRef;
    FSM_evtInfo.resultReq.encodingType     = encodingType;
    FSM_evtInfo.resultReq.parameter        = duLink;

    if ((gotVal = FSM_runMachine((Void *)invokeDesc, lsfsm_EvtResultReq)) < 0) {
	if (duLink != (DU_View)0) {

	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lrop_resultReq: DU_free: parameter=0x%x\n", duLink));

	    DU_free(duLink);
	}
    }
    return (gotVal);
}


/*<
 * Function:    LROP_errorReq
 *
 * Description: LSROS-ERROR.request.
 *
 * Arguments:   Invoke descriptor, encoding type, parameter.
 *
 * Returns:     0 if successful, -1 if unsuccessful.
 *
>*/

PUBLIC  Int
LROP_errorReq(LROP_InvokeDesc invokeDesc,
	      LROP_UserInvokeRef userInvokeRef,
	      LROP_EncodingType encodingType, 
	      LROP_ErrorValue errorValue, 
	      DU_View parameter)

{
    int gotVal;
    DU_View duLink = NULL;

    TM_TRACE((LROP_modCB, TM_ENTER, "LROP_errorReq\n"));

    if (parameter) {
    	duLink = DU_link(parameter);
    }

    FSM_evtInfo.resultReq.isResultNotError = 0;
    FSM_evtInfo.resultReq.userInvokeRef    = userInvokeRef;
    FSM_evtInfo.errorReq.encodingType      = encodingType;
    FSM_evtInfo.errorReq.errorValue        = errorValue;
    FSM_evtInfo.errorReq.parameter         = duLink;

    if ((gotVal = FSM_runMachine((Void *)invokeDesc, lsfsm_EvtResultReq)) < 0) {
	if (duLink != (DU_View)0) {

	    TM_TRACE((DU_modCB, DU_MALLOC, 
                     "lrop_errorReq: DU_free: parameter=0x%x\n",
		     parameter));

	    DU_free(parameter);
	}
    }
    return (gotVal);
}


