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
 * File name: lrop_sap.h (Service Access Point)
 *
 * Description: LROP Service Access Point.
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

/*
 * RCS Revision: $Id: lrop_sap.h,v 1.6 1996/09/13 02:30:56 kamran Exp $
 */

#ifndef _LROP_SAP_H_	/*{*/
#define _LROP_SAP_H_

#include "lsro.h"
#include "lrop.h"
#include "invoke.h"
#include "local.h"

/*
 *  Information required for support of communication
 *  with multiple Service Users through the SAP abstraction.
 */
typedef struct SapInfoSeq {
    struct SapInfo *first;
    struct SapInfo *last;
} SapInfoSeq;

typedef struct SapInfo {
    struct SapInfo	*next;		/* next SAP structure */
    struct SapInfo	*prev;		/* previous SAP structure */
    LSRO_SapSel	sapSel;			/* SAP-ID for this user */
    LSRO_FunctionalUnit	functionalUnit;	/* Functional Unit for this SAP */
    InvokeInfoSeq	invokeSeq;	/* associated invokations */

    int (*invokeInd) (
		      LSRO_SapSel 	locLSROSap,
		      LSRO_SapSel 	remLSROSap,
		      T_SapSel		*remTsap,
		      N_SapAddr		*remNsap,
		      LROP_InvokeDesc invoke,
		      LROP_OperationValue	operationValue,
		      LROP_EncodingType	encodingType,
		      DU_View		parameter);
    int (*resultInd) (LROP_InvokeDesc invoke,
		      LROP_UserInvokeRef userInvokeRef,
		      LROP_EncodingType	encodingType,
		      DU_View		parameter);
    int (*errorInd) (LROP_InvokeDesc invoke,
		     LROP_UserInvokeRef userInvokeRef,
		     LROP_EncodingType	encodingType,
		     LROP_ErrorValue	errorValue,
		     DU_View 		parameter);
    int (*resultCnf) (LROP_InvokeDesc invoke,
		      LROP_UserInvokeRef userInvokeRef);
    int (*errorCnf) (LROP_InvokeDesc invoke,
		     LROP_UserInvokeRef userInvokeRef);
    int (*failureInd) (LROP_InvokeDesc invoke,
		       LROP_UserInvokeRef userInvokeRef,
		       LROP_FailureValue	failureValue);
} SapInfo;

/* This really belongs to lrop.h */
/* typedef Void *LROP_SapDesc; */

#ifdef LINT_ARGS /*{*/

extern void lrop_sapInit (Int nuOfSaps); 

extern Int
LROP_sapBind(LROP_SapDesc *sapDesc, LSRO_SapSel sapSel, 
	     LSRO_FunctionalUnit functionalUnit,		
	     int (*invokeInd) (LSRO_SapSel 	locLSROSap,
			       LSRO_SapSel 	remLSROSap,
			       T_SapSel		*remTsap,
			       N_SapAddr		*remNsap,
			       LROP_InvokeDesc invoke,
			       LROP_OperationValue	operationValue,
			       LROP_EncodingType	encodingType,
			       DU_View		parameter),
	     int (*resultInd) (LROP_InvokeDesc invoke,
			       LROP_UserInvokeRef userInvokeRef,
			       LROP_EncodingType	encodingType,
			       DU_View		parameter),
	     int (*errorInd) (LROP_InvokeDesc invoke,
			      LROP_UserInvokeRef userInvokeRef,
			      LROP_EncodingType	encodingType,
			      LROP_ErrorValue   errorValue,
			      DU_View 		parameter),
	     int (*resultCnf) (LROP_InvokeDesc invoke,
			      LROP_UserInvokeRef userInvokeRef),
	     int (*errorCnf) (LROP_InvokeDesc invoke,
			      LROP_UserInvokeRef userInvokeRef),
	     int (*failureInd) (LROP_InvokeDesc invoke,
			        LROP_UserInvokeRef userInvokeRef,
				LROP_FailureValue	failureValue));
extern SapInfo  *lrop_getSapInfo (LSRO_SapSel);
extern SuccFail LROP_sapUnBind  (LSRO_SapSel sapSel); 
extern SapInfo  *lrop_getSapInfo (LSRO_SapSel);
extern SapInfo  *lrop_getSapInfo (LSRO_SapSel sapSel); 

#else

extern void lrop_sapInit (); 
extern Int LROP_sapBind ();
extern SapInfo  *lrop_getSapInfo (LSRO_SapSel);
extern SuccFail LROP_sapUnBind  (LSRO_SapSel sapSel); 
extern SapInfo  *lrop_getSapInfo (LSRO_SapSel);
extern SapInfo  *lrop_getSapInfo (LSRO_SapSel sapSel); 

#endif


#ifdef __STDC__
#else
#endif


#endif
	
