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
 * File: lropfsm.h
 *
 * Description:
 *	This File contains the definititions related to the Finite State 
 *      Machine of LSROS engine. Represented by the Module Identifer  FSM_.
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

/*
 * RCS Revision: $Id: lropfsm.h,v 1.9 1996/09/13 02:30:56 kamran Exp $
 */

#ifndef _LROPFSM_H
#define _LROPFSM_H

#include "tm.h"
#include "addr.h"

#include "invoke.h"
#include "lrop.h"

    /* User Action Primitives */
#define lsfsm_EvtInvokeReq		2
#define lsfsm_EvtErrorReq		3
#define lsfsm_EvtResultReq		4

#define lsfsm_EvtPduInputInd		5
    /* In PDU Indications */
#define    lsfsm_EvtPduInvoke		6
#define    lsfsm_EvtPduResult		7
#define    lsfsm_EvtPduAck		8
#define    lsfsm_EvtPduFailure		9
    
#define    lsfsm_EvtTimerInd		10
    /* Timer Indications */
#define    lsfsm_EvtInvokePduRetranTimer	11
#define    lsfsm_EvtResultPduRetranTimer	12
#define    lsfsm_EvtRefNuTimer		13
#define    lsfsm_EvtInactivityTimer	14
#define    lsfsm_EvtLastTimer		15
#define    lsfsm_EvtPerfNoResponseTimer	16

typedef union FSM_EventInfo {  
    struct InvokeReq {
	LROP_UserInvokeRef userInvokeRef;
	LSRO_SapSel remLSROSap;
	T_SapSel *remTsap;
	N_SapAddr *remNsap;
        LROP_OperationValue opValue;
	LROP_EncodingType encodingType;
	DU_View  parameter;
    } invokeReq;

    struct ResultReq {
	Bool	isResultNotError;
	LROP_UserInvokeRef userInvokeRef;
	LROP_EncodingType encodingType;
	DU_View    	  parameter;
    } resultReq;

    struct ErrorReq {
	Bool	isResultNotError;
	LROP_UserInvokeRef userInvokeRef;
	LROP_EncodingType encodingType;
	LROP_ErrorValue   errorValue;
	DU_View  	  parameter;
    } errorReq;

    struct InternalInfo {
	Int	expiredTimerName;
    } internalInfo;
    struct TmrInfo {
	Int	name;
	long 	subscript;
	Int 	datum;
    } tmrInfo;
} FSM_EventInfo;

/* LCL_XTRN FSM_EventInfo FSM_evtInfo; */     /* Global data for all FSM_ handlers */
FSM_EventInfo FSM_evtInfo;     /* Global data for all FSM_ handlers */

/*
typedef InvokeInfo FSM_Machine;
*/
#endif
