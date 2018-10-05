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
 * File name: layernm.C
 *
 * Description: Network management interface.
 *
 * Functions:
 *   SuccFail lrop_nm_incr(int layer, register Counter *counter, int increment)
 *   SuccFail lrop_nm_event(int layer, int eventid)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: layernm.c,v 1.10 1996/09/09 10:29:13 kamran Exp $";
#endif /*}*/

#include "local.h"
#include "estd.h"
#include "extfuncs.h"
#include "nm.h"

/*  LROP_ counters  */

Counter lrop_pduRetranCounter;

Counter lrop_completeOperationCounter;
Counter lrop_protocolErrorsCounter;

Counter lrop_pduSentCounter;
Counter lrop_invokePduRetranCounter;
Counter lrop_badAddrCounter;
Counter lrop_opRefusedCounter;


#ifdef MSDOS
/* 
 * call this from ginit to force globals to link 
 */
void
lrop_force_msdos_link(void)
{
}
#endif

/*
 *  Stubs when NM_ module is not linked in.
 */

#ifdef NM


/*<
 * Function:    lrop_nm_incr
 *
 * Description: Increment a counter and check against the threshold
 *
 * Arguments:   Layer, counter, increment value
 *
 * Returns:     0 if successful, -1 if unsuccessful. 
 *
>*/

SuccFail lrop_nm_incr(int layer, Counter *counter, int increment)
{
    counter->count += increment;
    if (counter->threshold && counter->count >= counter->threshold->count) {
	counter->count = 0;
	return lrop_nm_event(layer, counter->event);
    }
    return (SUCCESS); 
}


/*<
 * Function:    lrop_nm_event
 *
 * Description: Process an event
 *
 * Arguments:   Layer, event identifier
 *
 * Returns:     0 if successful, -1 if unsuccessful. 
 *
>*/

SuccFail lrop_nm_event(int layer, int eventid)
{
    return (SUCCESS); 
}
#endif







