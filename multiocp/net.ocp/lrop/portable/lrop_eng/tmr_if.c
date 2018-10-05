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
 * File: tmr_if.c
 *
 * Description: Timer interface.
 *
 * Functions:
 *   lrop_tmrSetValues(InvokeInfo *invoke)
 *   lrop_tmrUpdate(InvokeInfo *invoke, int sample)
 *   lrop_timerRsp(Ptr data)
 *   lrop_tmrCreate(InvokeInfo *invoke, Int name, LgInt time, Int data)
 *   lrop_tmrCancel(InvokeInfo *invoke, Int name)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: tmr_if.c,v 1.23 1997/01/14 19:14:22 kamran Exp $";
#endif /*}*/

#include "estd.h"
#include "eh.h"

#include "invoke.h"
#include "lropfsm.h"
#include "fsm.h"

#include "local.h"
#include "extfuncs.h"

LOCAL LgUns lrop_timeStamp;


/* Initial timer values */

#define ACKNOWLEDGEMENT		1000	/* Acknowledgement delay timer */
#define ROUNDTRIP		5000	/* Initial estimate of roundtrip dly */
#define RETRANSMIT		10000	/* Initial retransmission timer */
#define MAX_NSDU_LIFETIME	15000	/* Estimate of max NSDU lifetime */

#define RETRANSMIT_COUNT	4	/* Retransmission count */

struct TimerValue {
    Int	    acknowledgment;
    Int	    roundtrip;
    Int	    retransmit;
    Int	    maxNSDULifeTime;
} timerValue = 
{
    ACKNOWLEDGEMENT,
    ROUNDTRIP,
    RETRANSMIT,
    MAX_NSDU_LIFETIME
};

int retransmitCount = RETRANSMIT_COUNT;


/*<
 * Function: lrop_tmrSetValues
 *
 * Description: Setup the initial estimate of all timer values.
 *
 * Arguments: Invokation information struct
 *
 * Returns: None.
 *
>*/

LOCAL Void
lrop_tmrSetValues(InvokeInfo *invoke)
{
#if 0
#ifdef TM_ENABLED
	static int virginv = 1;
	static int virginf = 1;
#endif
#endif

    invoke->retransTimeout  = timerValue.retransmit + timerValue.roundtrip;
    invoke->rwait           = 
            2 * (timerValue.acknowledgment + timerValue.retransmit);
    invoke->nuOfRetrans     =  retransmitCount;
    invoke->refKeepTime     = (2 * timerValue.maxNSDULifeTime) + 
                              invoke->nuOfRetrans * timerValue.retransmit;
    invoke->inactivityDelay = 4 * timerValue.roundtrip;
    invoke->perfNoResponse  = (2 * timerValue.maxNSDULifeTime) + 
			      (invoke->nuOfRetrans + 1) * 
    			      (timerValue.retransmit + timerValue.roundtrip);
#if 0
#ifndef FIXED_TIMERS
#ifdef TM_ENABLED
	if (virginv) {
    	    TM_TRACE((LROP_modCB, TM_ENTER, "VARIABLE TIMERS in use\n"));
	    virginv = 0;
	}
#endif
    invoke->roundTripDelay  = (ROUNDTRIP);
    invoke->maxLocAckDelay  = (ACKNOWLEDGEMENT);
    invoke->retransTimeout  = (RETRANSMIT) + invoke->maxRemAckDelay;

    invoke->rwait           = 2 * invoke->retransTimeout;
    invoke->nuOfRetrans     = RETRANSMIT_COUNT;
    invoke->refKeepTime     = (2 * MAX_NSDU_LIFETIME) + 
                              invoke->nuOfRetrans * invoke->retransTimeout;
    invoke->inactivityDelay = 4 * invoke->roundTripDelay;
#else
#ifdef TM_ENABLED
	if (virginf) {
    	    TM_TRACE((LROP_modCB, TM_ENTER, "FIXED TIMERS in use\n"));
	    virginf = 0;
	}
#endif
    invoke->retransTimeout  = (RETRANSMIT) + (ROUNDTRIP);
    invoke->rwait           = 2 * ((ACKNOWLEDGEMENT) + (RETRANSMIT));
    invoke->nuOfRetrans     = RETRANSMIT_COUNT;
    invoke->refKeepTime     = (2 * MAX_NSDU_LIFETIME) + 
                              invoke->nuOfRetrans * (RETRANSMIT);
    invoke->inactivityDelay = 4 * ROUNDTRIP;
    invoke->perfNoResponse  = (2 * MAX_NSDU_LIFETIME) + 
			      invoke->nuOfRetrans * 
			      ((RETRANSMIT) + (ROUNDTRIP));
#endif
#endif

}


/*<
 * Function:    lrop_tmrUpdate
 *
 * Description: Update timers (dynamic) based on a new estimate of the 
 *              average round-trip delay.
 *
 * Arguments:   Invoke info, sample time.
 *
 * Returns:     None.
 *
>*/

LOCAL Void
lrop_tmrUpdate(InvokeInfo *invoke, int sample)
{
#define N	2
#define K	4

#ifndef FIXED_TIMERS

    /*
     * I do not believe there are networks which has a zero (or less) transit
     * delay time. 
     */

    if (sample <= 0)
	sample = 1;

    /* Calculate new estimate of average roundtrip delay */

    invoke->roundTripDelay += (sample - invoke->roundTripDelay) >> N;

    /* Update timer values based on latest estimate of roundtrip delay */

    invoke->retransTimeout = invoke->maxRemAckDelay + K * invoke->roundTripDelay;
    invoke->refKeepTime    = (2 * MAX_NSDU_LIFETIME) + 
                             invoke->nuOfRetrans * invoke->retransTimeout;
    invoke->rwait          = 4 * invoke->retransTimeout;

#endif
}


/*<
 * Function:    lrop_timerRsp
 *
 * Description: S_TIMER.indication event.
 *              This routine is performed whenever a timer has expired.
 *
 * Arguments:   Timer data.
 *
 * Returns:     
 *
>*/

STATIC Int
lrop_timerRsp(Ptr data)
{
    TmrData *tmrData;

    tmrData = (TmrData *)data;

    FSM_evtInfo.tmrInfo.name  = tmrData->name;
    FSM_evtInfo.tmrInfo.datum = tmrData->data;

    QU_REMOVE(tmrData);

    switch ( FSM_evtInfo.tmrInfo.name ) {
      case lrop_invokePduRetranTimer:
	if (FSM_runMachine((Void *)tmrData->invoke,  
			   lsfsm_EvtInvokePduRetranTimer) == FAIL) {
	    return FAIL;
	}
	break;

    case lrop_resultPduRetranTimer:
	if (FSM_runMachine((Void *)tmrData->invoke,  
			   lsfsm_EvtResultPduRetranTimer) == FAIL) {
	    return FAIL;
	}
	break;

    case lrop_refNuTimer:
	if (FSM_runMachine((Void *)tmrData->invoke,  
			   lsfsm_EvtRefNuTimer) == FAIL) {
	    return FAIL;
	}
	break;


    case lrop_lastTimer:
	if (FSM_runMachine((Void *)tmrData->invoke,  
			   lsfsm_EvtLastTimer) == FAIL) {
	    return FAIL;
	}
	break;

    case lrop_inactivityTimer:
	if (FSM_runMachine((Void *)tmrData->invoke,  
			   lsfsm_EvtInactivityTimer) == FAIL) {
	    return FAIL;
	}
	break;

    case lrop_perfNoResponseTimer:
	if (FSM_runMachine((Void *)tmrData->invoke,  
			   lsfsm_EvtPerfNoResponseTimer) == FAIL) {
	    return FAIL;
	}
	break;


    default:
	EH_problem("lrop_timerRsp: invalid timer name");
        return (FAIL);
    }

    return (SUCCESS);

} /* lrop_timerRsp() */


/*<
 * Function:    lrop_tmrCreate
 *
 * Description: Create a timer.
 *
 * Arguments:   Invoke info, timer name, time, data.
 *
 * Returns:     None.
 *
>*/

SuccFail
lrop_tmrCreate(InvokeInfo *invoke, Int name, LgInt time, Int data)
{
    TmrData *tmrData;
    TMR_Desc tmrDesc;

    if ((tmrDesc = TMR_create(time, lrop_timerRsp)) == NULL) {
	return (FAIL);
    }

    tmrData = (TmrData *) TMR_getData(tmrDesc);
    tmrData->invoke = invoke;
    tmrData->name   = name;
    tmrData->data   = data;

    QU_INIT(tmrData);
    QU_INSERT(&invoke->tmrQu, tmrData);

    return (SUCCESS);

} /* lrop_tmrCreate() */


/*<
 * Function:    lrop_tmrCancel
 *
 * Description: Cancel timer. A specific timer or all timer associated with
 *              an invokation can be cancelled.
 *
 * Arguments:   Invoke info, timer name.
 *
 * Returns:     None.
 *
>*/

LOCAL Void
lrop_tmrCancel(InvokeInfo *invoke, Int name)
{
    TmrData *tmrData;
    TmrData *next;
    TMR_Desc tmrDesc;
    
    if ( name == lrop_allTimer ) {
	/* All Timers associated with this Invoke should be canceled */

	while (! QU_EQUAL((tmrData = QU_FIRST(&invoke->tmrQu)), 
			   &invoke->tmrQu)) {
	    QU_REMOVE(tmrData);
	    tmrDesc = TMR_getDesc((Ptr)tmrData);
	    TMR_cancel(tmrDesc);
	}
    } else {
	for (tmrData = QU_FIRST(&invoke->tmrQu);
	     ! QU_EQUAL(tmrData, &invoke->tmrQu); 
	     tmrData = next) {
	    next = tmrData->next;
	    if (tmrData->name == name) {
		QU_REMOVE(tmrData);
		tmrDesc = TMR_getDesc((Ptr)tmrData);
		TMR_cancel(tmrDesc);
	    } 
	}
    }

} /* lrop_tmrCancel() */
