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
 * File:        userout.c
 *
 * Description: This file contains the event primitive interface between
 *              the LSROS layer and the user of the LSROS services.
 *
 * Functions:
 *   lrop_invokeInd()
 *   lrop_resultInd()
 *   lrop_errorInd()
 *   lrop_resultCnf()
 *   lrop_errorCnf()
 *   lrop_failureInd()
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: userout.c,v 1.14 1997/01/01 00:03:38 kamran Exp $";
#endif /*}*/

#include "estd.h"
#include "invoke.h"
#include "lrop_sap.h"

#include "lropfsm.h"
#include "nm.h"
#include "sf.h"
#include "eh.h"
#include "sch.h"
#include "layernm.h"

#include "local.h"
#include "extfuncs.h"

typedef struct InvokeIndArg
{
    InvokeInfo 		*invoke;
    LSRO_SapSel		locLSROSap;
    LSRO_SapSel 	remLSROSap;
    T_SapSel		*remTsap;
    N_SapAddr		*remNsap;
    LROP_OperationValue	operationValue;
    LROP_EncodingType	encodingType;
    DU_View		parameter;
} InvokeIndArg;

typedef struct ResultIndArg
{
    InvokeInfo 	  	*invoke;
    LROP_UserInvokeRef 	userInvokeRef;
    LROP_EncodingType  	encodingType;
    DU_View		parameter;
} ResultIndArg;

typedef struct ErrorIndArg
{
    InvokeInfo 		*invoke;
    LROP_UserInvokeRef 	userInvokeRef;
    LROP_EncodingType   encodingType;
    LROP_ErrorValue	errorValue;
    DU_View 		parameter;
} ErrorIndArg;

typedef struct ResultConfArg
{
    InvokeInfo *invoke;
    LROP_UserInvokeRef 	userInvokeRef;
} ResultConfArg;

typedef struct ErrorConfArg
{
    InvokeInfo *invoke;
    LROP_UserInvokeRef 	userInvokeRef;
} ErrorConfArg;

typedef struct FailureIndArg
{
    InvokeInfo        *invoke;
    LROP_UserInvokeRef 	userInvokeRef;
    LROP_FailureValue failureValue;
} FailureIndArg;

Int lrop_scheduledInvokeInd(InvokeIndArg *arg);
Int lrop_scheduledResultInd(ResultIndArg *arg);
Int lrop_scheduledErrorInd(ErrorIndArg *arg);
Int lrop_scheduledResultCnf(ResultConfArg *arg);
Int lrop_scheduledErrorCnf(ErrorConfArg *arg);
Int lrop_scheduledFailureInd(FailureIndArg *arg);


/*<
 * Function:    lrop_invokeInd
 *
 * Description: LSRO-INVOKE.indication
 *
 * Arguments:   Invoke info, SAP selector, Transport SAP selector, Network
 *              SAP selector, operation value, encoding type, parameter.
 *
 * Returns:     
 *
>*/

LOCAL Int 
lrop_invokeInd(InvokeInfo 	   *invoke,
	       LSRO_SapSel 	   locLSROSap,
	       LSRO_SapSel 	   remLSROSap,
	       T_SapSel		   *remTsap,
	       N_SapAddr	   *remNsap,
	       LROP_OperationValue operationValue,
	       LROP_EncodingType   encodingType,
	       DU_View		   parameter)
{
    InvokeIndArg *arg;

#ifdef TM_ENABLED
    char taskN[100] = "Invoke Indication: ";
#endif

#ifdef AUTHENTICATE_DOMAIN
    if (lrop_authenticateIP(remNsap)) {
	EH_problem("\nProgram is talking to unauthorized network address\n");
	return (FAIL);
    }
#endif

    if ((arg = SF_memGet(sizeof(InvokeIndArg)))) {
    } else {
	EH_problem("Memory allocation failed");
	return (FAIL);
    } 

    arg->invoke         = invoke;
    arg->locLSROSap     = locLSROSap;
    arg->remLSROSap     = remLSROSap;
    arg->remTsap        = remTsap;
    arg->remNsap        = remNsap;
    arg->operationValue = operationValue;
    arg->encodingType   = encodingType;
    arg->parameter      = parameter;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_invokeInd: Invoke.indication scheduled: "
	     "refNu=%ld", 
	     arg->invoke));

#ifdef TM_ENABLED
        strcpy(taskN, "Invoke Indication: ");
    	return SCH_submit ((Void *)lrop_scheduledInvokeInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_scheduledInvokeInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT);
#endif
}


/*<
 * Function:    lrop_scheduledInvokeInd
 *
 * Description: Call scheduled invoke indication (call-back function)
 *
 * Arguments:	Invoke inducation info.
 *
 * Returns: 
 *
>*/

LOCAL Int 
lrop_scheduledInvokeInd(InvokeIndArg *arg)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_scheduledInvokeInd: Invoke.indication call-back function "
	     "called: refNu=%ld\n", arg->invoke));

    (*(arg->invoke->locSap->invokeInd)) (
				    arg->locLSROSap,
				    arg->remLSROSap,
				    arg->remTsap,
				    arg->remNsap,
				    (LROP_InvokeDesc) arg->invoke,
				    arg->operationValue,
				    arg->encodingType,
				    arg->parameter);
    DU_free(arg->parameter);
    SF_memRelease(arg);
    return 0;
}


/*<
 * Function:    lrop_resultInd
 *
 * Description: LSROS-RESULT.indication.
 *
 * Arguments:   Invoke info, encoding type, parameter.
 *
 * Returns:     
 *
>*/

LOCAL Int 
lrop_resultInd(InvokeInfo 	  *invoke,
	       LROP_UserInvokeRef userInvokeRef,
	       LROP_EncodingType  encodingType,
	       DU_View		  parameter)
{
    ResultIndArg *arg;

#ifdef TM_ENABLED
    char taskN[100] = "Result Indication: ";
#endif

    if ((arg = SF_memGet(sizeof(ResultIndArg)))) {
    } else {
	EH_problem("Memory allocation failed");
	return (FAIL);
    } 

    arg->invoke        = invoke;
    arg->userInvokeRef = userInvokeRef;
    arg->encodingType  = encodingType;
    arg->parameter     = parameter;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_resultInd: Result.indication scheduled: "
	     "refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

#ifdef TM_ENABLED
        strcpy(taskN, "Result Indication: ");
    	return SCH_submit ((Void *)lrop_scheduledResultInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_scheduledResultInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT);
#endif

}


/*<
 * Function:    lrop_scheduledResultInd
 *
 * Description: Call scheduled result indication function.
 *
 * Arguments:	Result indication parameters.
 *
 * Returns: 
 *
>*/

Int
lrop_scheduledResultInd(ResultIndArg *arg)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_scheduledResultInd: Result.indication call-back function "
	     "called: refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

    (*arg->invoke->locSap->resultInd) ((LROP_InvokeDesc) arg->invoke, 
	       				arg->userInvokeRef,
				        arg->encodingType, 
					arg->parameter);
    DU_free(arg->parameter);
    SF_memRelease(arg);
    return 0;
}


/*<
 * Function:    lrop_errorInd
 *
 * Description: LSROS-ERROR.indication event.
 *
 * Arguments:   Invoke info, encoding type, parameter.
 *
 * Returns:     
 *
>*/

LOCAL Int 
lrop_errorInd(InvokeInfo *invoke,
	      LROP_UserInvokeRef userInvokeRef,
	      LROP_EncodingType  encodingType,
	      LROP_ErrorValue	 errorValue,
	      DU_View 		 parameter)
{
    ErrorIndArg *arg;

#ifdef TM_ENABLED
    char taskN[100] = "Error Indication: ";
#endif

    if ((arg = SF_memGet(sizeof(ErrorIndArg)))) {
    } else {
	EH_problem("Memory allocation failed");
	return (FAIL);
    } 

    arg->invoke        = invoke;
    arg->userInvokeRef = userInvokeRef;
    arg->encodingType  = encodingType;
    arg->errorValue    = errorValue;
    arg->parameter     = parameter;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_errorInd: Error.indication scheduled: "
	     "refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

#ifdef TM_ENABLED
        strcpy(taskN, "Error Indication: ");
    	return SCH_submit ((Void *)lrop_scheduledErrorInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_scheduledErrorInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT);
#endif

}


/*<
 * Function:    lrop_scheduledErrorInd
 *
 * Description: Call scheduled error indication function.
 *
 * Arguments:	Error indication parameters.
 *
 * Returns:
 *
>*/

Int
lrop_scheduledErrorInd(ErrorIndArg *arg)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_scheduledErrorInd: Error.indication call-back function "
	     "called: refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

    (*arg->invoke->locSap->errorInd) ((LROP_InvokeDesc) arg->invoke, 
				      arg->userInvokeRef,
				      arg->errorValue, 
				      arg->encodingType, 
				      arg->parameter);

    DU_free(arg->parameter);
    SF_memRelease(arg);
    return 0;
}


/*<
 * Function:    lrop_resultCnf
 *
 * Description: LSROS-RESULT.confirm event.
 *
 * Arguments:   Invoke info.
 *
 * Returns:
 *
>*/

LOCAL Int 
lrop_resultCnf(InvokeInfo *invoke,
	       LROP_UserInvokeRef userInvokeRef)
{
    ResultConfArg *arg;

#ifdef TM_ENABLED
    char taskN[100] = "Result Confirm: ";
#endif

    if ((arg = SF_memGet(sizeof(ResultConfArg)))) {
    } else {
	EH_problem("Memory allocation failed");
	return (FAIL);
    } 

    arg->invoke        = invoke;
    arg->userInvokeRef = userInvokeRef;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_resultCnf: Result.confirmation scheduled: "
	     "refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

#ifdef TM_ENABLED
        strcpy(taskN, "Result Confirmation: ");
    	return SCH_submit ((Void *)lrop_scheduledResultCnf, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_scheduledResultCnf, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT);
#endif
}


/*<
 * Function:    lrop_scheduledResultCnf
 *
 * Description: Call scheduled result confirm
 *
 * Arguments:	Result confirm parameters.
 *
 * Returns: 
 *
>*/

Int
lrop_scheduledResultCnf(ResultConfArg *arg)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_scheduledResultCnf: Result.confirmation call-back function "
	     "called: refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

    (*arg->invoke->locSap->resultCnf) ((LROP_InvokeDesc) arg->invoke,
				        arg->userInvokeRef);

    SF_memRelease(arg);
    return 0;
}


/*<
 * Function:    lrop_errorCnf
 *
 * Description: LSROS-ERROR.indication event.
 *
 * Arguments:   Invoke info.
 *
 * Returns:
 *
>*/

LOCAL Int 
lrop_errorCnf(InvokeInfo *invoke, LROP_UserInvokeRef userInvokeRef)
{
    ErrorConfArg *arg;

#ifdef TM_ENABLED
    char taskN[100] = "Error Confirm: ";
#endif

    if ((arg = SF_memGet(sizeof(ErrorConfArg)))) {
    } else {
	EH_problem("Memory allocation failed");
	return (FAIL);
    } 

    arg->invoke        = invoke;
    arg->userInvokeRef = userInvokeRef;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_errorCnf: Error.confirmation scheduled: "
	     "refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

#ifdef TM_ENABLED
        strcpy(taskN, "Error Confirmation: ");
    	return SCH_submit ((Void *)lrop_scheduledErrorCnf, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_scheduledErrorCnf, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT);
#endif
}


/*<
 * Function:    lrop_scheduledErrorCnf
 *
 * Description: Call scheduled error confirm call-back function
 *
 * Arguments:	Error confirm parameters.
 *
 * Returns: 
 *
>*/

Int
lrop_scheduledErrorCnf(ErrorConfArg *arg)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_scheduledErrorCnf: Error.confirmation call-back function "
	     "called: refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

    (*arg->invoke->locSap->errorCnf) ((LROP_InvokeDesc) arg->invoke,
				      arg->userInvokeRef);
    SF_memRelease(arg);
    return 0;
}


/*<
 * Function:    lrop_failureInd
 *
 * Description: LSROS-FAILURE.indication event.
 *
 * Arguments:   Invoke info, failure reason.
 *
 * Returns:     
 *
>*/
LOCAL Int 
lrop_failureInd(InvokeInfo        *invoke,
	        LROP_UserInvokeRef userInvokeRef,
		LROP_FailureValue failureValue)
{
    FailureIndArg *arg;

#ifdef TM_ENABLED
    char taskN[100] = "Failure Indication: ";
#endif

    if ((arg = SF_memGet(sizeof(FailureIndArg)))) {
    } else {
	EH_problem("Memory allocation failed");
	return (FAIL);
    } 

    arg->invoke        = invoke;
    arg->userInvokeRef = userInvokeRef;
    arg->failureValue  = failureValue;

    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_failureInd: Failure.indication scheduled: "
	     "refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

#ifdef TM_ENABLED
        strcpy(taskN, "Failure Indication: ");
    	return SCH_submit ((Void *)lrop_scheduledFailureInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT, 
		           (String) strcat(taskN, TM_here()));
#else
    	return SCH_submit ((Void *)lrop_scheduledFailureInd, 
		           (Ptr) arg, 
		           SCH_PSEUDO_EVENT);
#endif
}


/*<
 * Function:    lrop_scheduledFailureInd
 *
 * Description: Call scheduled failure indication call-back function
 *
 * Arguments:	Failure indication paramenters.
 *
 * Returns: 
 *
>*/

Int
lrop_scheduledFailureInd(FailureIndArg *arg)
{
    TM_TRACE((LROP_modCB, TM_ENTER, 
	     "lrop_scheduledFailureInd: Failure.indication call-back function "
	     "called: refNu=%ld, userInvokeRef=%ld", 
	     arg->invoke, arg->userInvokeRef));

    (*arg->invoke->locSap->failureInd) ((LROP_InvokeDesc) arg->invoke, 
				        arg->userInvokeRef,
					arg->failureValue);

    SF_memRelease(arg);
    return 0;
}
