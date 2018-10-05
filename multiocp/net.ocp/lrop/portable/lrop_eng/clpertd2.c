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
 * File name: clperftd.c (ConnectionLess Performer Transition Diagram)
 *
 * Description: Connection Less Performer Transition Diagram
 *
 * Functions:
 *  PUBLIC FSM_TransDiagram *
 *  lrop_2CLPerformerTransDiag (void)
 *
 *  lsfsm_e   Invoked when ENTERING a state.
 *  lsfsm_x   Invoked when EXITING a state.
 *  lsfsm_a   Invoked as ACTION of a transition.
 *  lsfsm_t   TRANSITION specificatin.
 *  lsfsm_s   STATE specification.
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: clpertd2.c,v 1.6 1996/12/06 21:40:50 kamran Exp $";
#endif /*}*/

#include  "estd.h" 
#include  "eh.h"
#include  "queue.h"
#include  "lropfsm.h"
#include  "fsm.h"
#include  "fsmtrans.h"

#include "target.h"
#include "local.h"
#include "extfuncs.h"

extern FSM_State FSM_sError;

extern FSM_State lsfsm_s2CLPerformerStart;
extern FSM_State lsfsm_s2InvokePduReceived;
extern FSM_State lsfsm_s2ResultPduRetrans;
extern FSM_State lsfsm_s2PerformerRefWait;

extern Int lsfsm_ePass(Void *machine, Void *userData, FSM_EventId evtId);
extern Int lsfsm_xPass(Void *machine,Void *userData, FSM_EventId evtId);

STATIC FSM_Trans	lsfsm_t2CLPerformerStart[]=
{ 
    {
	lsfsm_EvtPduInvoke,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer01,
	&lsfsm_s2InvokePduReceived,
	"P:InvokePdu"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2badEventIgnore,
	&lsfsm_s2CLPerformerStart,
	"I:BadEventIgnored"
    }
};


PUBLIC FSM_State lsfsm_s2CLPerformerStart = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_t2CLPerformerStart,
    "CL2-PerformerStart-State"};


STATIC FSM_Trans	lsfsm_tInvokePduReceived[]=
{ 
    {
	lsfsm_EvtResultReq,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer03,
	&lsfsm_s2ResultPduRetrans,
	"U:ResulReq"
    },
    {
	lsfsm_EvtPduInvoke,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer02,
	&lsfsm_s2InvokePduReceived,
	"P:DuplicateInvokePdu"
    },
    {
	FSM_EvtInternal,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer04 /*NOTYET */,
	&lsfsm_s2CLPerformerStart,
	"I:Failure"
    },
    {
	lsfsm_EvtPerfNoResponseTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer04,
	&lsfsm_s2CLPerformerStart,
	"I:Failure"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2badEventIgnore,
	&lsfsm_s2InvokePduReceived,
	"I:BadEventIgnored"
    }
};


PUBLIC FSM_State lsfsm_s2InvokePduReceived = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tInvokePduReceived,
    "CL2-InvokePduReceived-State"};

STATIC FSM_Trans	lsfsm_tResultPduRetrans[]=
{ 
    {
	lsfsm_EvtInactivityTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer06,
	&lsfsm_s2PerformerRefWait,
	"T:LastRetranResultPdu"
    },
    {
	lsfsm_EvtPduInvoke,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer05,
	&lsfsm_s2ResultPduRetrans,
	"P:DuplicateInvokePdu"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2badEventIgnore,
	&lsfsm_s2ResultPduRetrans,
	"I:BadEventIgnored"
    }
};


PUBLIC FSM_State lsfsm_s2ResultPduRetrans = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tResultPduRetrans,
    "CL2-ResultPduRetrans-State"};

/* This is needed to prevent late and duplicate InvokePdus from causing trouble */
STATIC FSM_Trans	lsfsm_tPerformerRefWait[]=
{ 
    {
	lsfsm_EvtRefNuTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer08,
	&lsfsm_s2CLPerformerStart,
	"T:PerformerRefWait"
    },
    {
	lsfsm_EvtPduInvoke,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2clPerformer07,
	&lsfsm_s2PerformerRefWait,
	"P:DuplicateInvokePdu"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_2badEventIgnore,
	&lsfsm_s2PerformerRefWait,
	"I:BadEventIgnored"
     }
};


PUBLIC FSM_State lsfsm_s2PerformerRefWait = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tPerformerRefWait,
    "CL2-PerformerRefWait-State"};



/* global variables */


/*<
 * Function: lrop_2CLPerformerTransDiag (void)
 *
 * Description: Create the transition diagram of Connectionless performer 
 *              and put it in start state.
 *
 * Arguments: None.
 *
 * Returns: Pointer to Finite State Machine of the Transition Diagram.
 *
 * 
>*/

PUBLIC FSM_TransDiagram *
lrop_2CLPerformerTransDiag (void)
{
    static FSM_TransDiagram * app;
    static Bool virgin = TRUE;

    if (!virgin) {
        return app;
    }

    virgin = FALSE;

    if ( ! (app = (FSM_TransDiagram *)FSM_TRANSDIAG_create("2CLPerformerTransDiag", &lsfsm_s2CLPerformerStart)) ) {
	EH_problem("lrop_2CLPerformerTransDiag: FSM_TRANSDIAG_create failed");
	app = (FSM_TransDiagram *) 0;
    }
    return app;
}

