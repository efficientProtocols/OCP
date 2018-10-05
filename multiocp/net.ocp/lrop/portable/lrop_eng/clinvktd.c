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
 * File name: clinvktd.c (Connectionless Invoker Transition Diagram)
 *
 * Description: Connectionless Invoker Transition Diagram
 *
 * Functions:
 *   lrop_CLInvokerTransDiag (void)
 *   lsfsm_eError (Void *machine, FSM_EventId evtId)
 *   tr_badEventIgnore(Void *machine, Void *userData, FSM_EventId evtId)
 *   lsfsm_ePass(Void *machine, Void *userData, FSM_EventId evtId)
 *   lsfsm_xPass(Void *machine, Void *userData, SM_EventId evtId)
 *
 *   lsfsm_e   Invoked when ENTERING a state.
 *   lsfsm_x   Invoked when EXITING a state.
 *   lsfsm_a   Invoked as ACTION of a transition.
 *   lsfsm_t   TRANSITION specificatin.
 *   lsfsm_s   STATE specification.
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: clinvktd.c,v 1.16 1997/03/14 04:48:43 kamran Exp $";
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

extern FSM_State lsfsm_sCLInvokerStart;
extern FSM_State lsfsm_sInvokePduSent;
extern FSM_State lsfsm_sAckPduSent;
extern FSM_State lsfsm_sInvokerRefWait;

extern Int lsfsm_ePass(Void *machine, Void *userData, FSM_EventId evtId);
extern Int lsfsm_xPass(Void *machine,Void *userData, FSM_EventId evtId);

#ifdef FUTURE
extern FSM_State lsfsm_sError;

LOCAL FSM_Trans	lsfsm_tError[]=
{
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	(Int(*)())0,
	&FSM_sError
    }
};

PUBLIC FSM_State lsfsm_sError = {
	lsfsm_eError,
	(Int(*)()) 0,
	lsfsm_tError,
        "Error-State"};
#endif

STATIC FSM_Trans	lsfsm_tCLInvokerStart[]=
{ 
    {
	lsfsm_EvtInvokeReq,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker01,
	&lsfsm_sInvokePduSent,
	"U:Invoke.Req"
    }, 
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_badEventIgnore,
	&lsfsm_sCLInvokerStart,
	"I:BadEventIgnored"
    }
};

PUBLIC FSM_State lsfsm_sCLInvokerStart = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tCLInvokerStart,
    "CL-InvokerStart-State"};

STATIC FSM_Trans	lsfsm_tInvokePduSent[]=
{ 
    {
	lsfsm_EvtInvokePduRetranTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker02,
	&lsfsm_sInvokePduSent,
	"T:RetranInvokePdu"
    },
    {
	lsfsm_EvtLastTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker03,
	&lsfsm_sInvokerRefWait,
	"T:LastRetran"
    },
    {
	lsfsm_EvtPduResult,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker04,
	&lsfsm_sAckPduSent,
	"P:ResultPdu"
    },
    {
	lsfsm_EvtPduFailure,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker05,
	&lsfsm_sInvokerRefWait,
	"P:FailurePdu"
    },
    {
	lsfsm_EvtPduAck,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker06,
	&lsfsm_sInvokePduSent,
	"P:AckPdu"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_badEventIgnore,
	&lsfsm_sInvokePduSent,
	"I:BadEventIgnored"
    }
};

PUBLIC FSM_State lsfsm_sInvokePduSent = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tInvokePduSent,
    "CL-InvokePduSent-State"};

STATIC FSM_Trans	lsfsm_tAckPduSent[]=
{ 
    {
	lsfsm_EvtPduResult,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker07,
	&lsfsm_sAckPduSent,
	"P:DuplicateResultPdu"
    },
    {
	lsfsm_EvtInactivityTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker10,
	&lsfsm_sInvokerRefWait,
	"T:Inactivity"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_badEventIgnore,
	&lsfsm_sAckPduSent,
	"I:BadEventIgnored"
    }
};

PUBLIC FSM_State lsfsm_sAckPduSent = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tAckPduSent,
    "CL-AckPduSent-State"};

STATIC FSM_Trans	lsfsm_tInvokerRefWait[]=
{ 
    {
	lsfsm_EvtRefNuTimer,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker08,
	&lsfsm_sCLInvokerStart,
	"T:InvokerRefWait"
    },
    {
	lsfsm_EvtPduResult,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_clInvoker09,
	&lsfsm_sInvokerRefWait,
	"P:DuplicateResultPdu"
    },
    {
	FSM_EvtDefault,
	(Bool(*)(FSM_Machine *, FSM_UserData *, FSM_EventId))0,
	tr_badEventIgnore,
	&lsfsm_sInvokerRefWait,
	"I:BadEventIgnored"
    }
};


PUBLIC FSM_State lsfsm_sInvokerRefWait = {
    lsfsm_ePass,
    lsfsm_xPass,
    lsfsm_tInvokerRefWait,
    "CL-InvokerRefWait-State"};

char *eventName[] = {
    "FSM_EvtDefault",
    "FSM_EvtInternal",
    "lsfsm_EvtInvokeReq",
    "lsfsm_EvtErrorReq",
    "lsfsm_EvtResultReq",
    "lsfsm_EvtPduInputInd",
    "lsfsm_EvtPduInvoke",
    "lsfsm_EvtPduResult",
    "lsfsm_EvtPduAck",
    "lsfsm_EvtPduFailure",
    "lsfsm_EvtTimerInd",
    "lsfsm_EvtInvokePduRetranTimer",
    "lsfsm_EvtResultPduRetranTimer",
    "lsfsm_EvtRefNuTimer",
    "lsfsm_EvtInactivityTimer",
    "lsfsm_EvtLastTimer",
    "lsfsm_EvtPerfNoResponseTimer"
};


/*<
 * Function:    lrop_CLInvokerTransDiag
 *
 * Description: Protcol Engine: Create Connectionless Invoker Transition 
 *              Diagram.
 *
 * Arguments:   None.
 *
 * Returns:     Pointer to state machine diagram if successful, otherwise 0.
 *
 * 
>*/

PUBLIC FSM_TransDiagram *
lrop_CLInvokerTransDiag (void)
{
    static FSM_TransDiagram * app;
    static Bool virgin = TRUE;

    if (!virgin) {
        return app;
    }

    virgin = FALSE;

    if ( ! (app = (FSM_TransDiagram*)
		  FSM_TRANSDIAG_create("CLInvokerTransDiag", 
					&lsfsm_sCLInvokerStart)) ) {
	EH_problem("lrop_CLInvokerTransDiag: FSM_TRANSDIAG_create failed");
	app = (FSM_TransDiagram *) 0;
    }
    return app;
}


/*<
 * Function:    lsfsm_eError
 *
 * Description: Error function
 *
 * Arguments:   State machine, event id.
 *
 * Returns:     0 if completed successfule, -1 otherwise.
 * 
>*/
Int
lsfsm_eError (Void *machine, Void *userData, FSM_EventId evtId)
{
    EH_problem("lsfsm_eError: FSM \"error\" fucntion is called");
    return (SUCCESS); 
}


/*<
 * Function:    tr_badEventIgnore
 *
 * Description: Bad event is ignored.
 *
 * Arguments:   State machine, event id.
 *
 * Returns:     0 if completed successfule, -1 otherwise.
 * 
>*/

Int
tr_badEventIgnore(Void *machine, Void *userData, FSM_EventId evtId)
{
    char buf[256];

    sprintf (buf, "tr_badEventIgnore: Bad Event Ignored: 0x%lx, %s\n", 
	     evtId, eventName[evtId]);
    EH_problem(buf);

    return (SUCCESS); 
}


/*<
 * Function:    lsfsm_ePass
 *
 * Description: Invoked when entering a state.
 *
 * Arguments:   State machine, event id.
 *
 * Returns:     0 if completed successfule, -1 otherwise.
 * 
>*/

Int
lsfsm_ePass(Void *machine, Void *userData, FSM_EventId evtId)
{
    TM_TRACE((LROP_modCB, TM_ENTER,
	"lsfsm_ePass:         machine=0x%lx  evtId=0x%x\n", machine, evtId));

    return (SUCCESS); 
}



/*<
 * Function:    lsfsm_xPass
 *
 * Description: Invoked when exiting a state.
 *
 * Arguments:   State machine, event id.
 *
 * Returns:     0 if completed successfule, -1 otherwise.
 * 
>*/

Int
lsfsm_xPass(Void *machine,Void *userData, FSM_EventId evtId)
{
    TM_TRACE((LROP_modCB, TM_ENTER,
	"lsfsm_xPass:         machine=0x%lx  evtId=0x%x\n", machine, evtId));

    return (SUCCESS); 
}

