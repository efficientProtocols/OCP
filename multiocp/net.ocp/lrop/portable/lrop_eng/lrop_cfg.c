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
 * File: lropcfg.c
 *
 * Description: This module contains all initialization routines.
 *              LROP_init: Initialize protocol engine.
 *
 * Functions:
 *   LROP_init(Int udpSapSel, Int nuOfSaps, Int nuOfInvokes, Int invokePduSize)
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: lrop_cfg.c,v 1.12 1997/01/01 00:04:12 kamran Exp $";
#endif /*}*/

#include "estd.h"
#include "eh.h"
#include "nm.h"
#include "layernm.h"


#include "tm.h"
#include "inetaddr.h"
#include "lrop_cfg.h"
#include "udp_if.h"
#include "fsm.h"
#include "fsmtrans.h"

#include "local.h"
#include "extfuncs.h"
#include "target.h"

extern Void emptyFunctionHandler(InvokeInfo * pInvokeInfo);

/*----- Global Var Definition -----*/

Int udpSapSel = LROP_K_UdpSapSel;

LOCAL TM_ModuleCB *LROP_modCB;

Int nuOfSaps    = LROP_SAPS;
Int nuOfInvokes = LROP_INVOKES;
Int invokePduSize = INVOKE_PDU_SIZE;

UDP_SapDesc lrop_udpSapDesc;

#ifdef AUTHENTICATE_DOMAIN
N_SapAddr authorizedDomains[] = {
  {4, {198, 62, 92, 0}}, 
  {0, {0, 0, 0, 0}}
};
#endif



/*<
 * Function:    LROP_init
 *
 * Description: Initialize protocol engine
 *
 * Arguments:   None.
 *
 * Returns:     0 if successful, -1 if unsuccessful.
 *
>*/

PUBLIC SuccFail
LROP_init(Int udpSapSel, Int nuOfSaps, Int nuOfInvokes, Int invokePduSize)
{
    static Bool virgin = TRUE;
    
    if ( !virgin ) {
	return 0;
    }
    virgin = FALSE;

#ifdef TM_ENABLED
    TM_INIT();

    if (TM_OPEN(LROP_modCB, "LROP_") == NULL) {
    	EH_problem("LROP_init: TM_open LROP_ failed");
	return ( FAIL );
    }
#endif

    lrop_invokeInit(nuOfInvokes);
    lrop_sapInit(nuOfSaps);

    FSM_TRANSDIAG_init();
    FSM_init();

    UDP_init(MAX_SAPS);

    /* Make all local reference numbers assignable  */
    lrop_refInit(NREFS);

#ifdef FUTURE
    lrop_completeOperationCounter.count = 0;	
#endif

    {
	static T_SapSel locTsapSel;

	INET_portNuToTsapSel(udpSapSel, &locTsapSel);

	if ((lrop_udpSapDesc = UDP_sapBind(&locTsapSel, lower_dataInd)) 
	    == NULL) {
	    return (FAIL);
	}
    }

    lrop_freeLopsInvoke = emptyFunctionHandler;

    return ( SUCCESS );
}


/*<
 * Function:    emptyFunctionHandler
 *
 * Description: Empty Function handler
 *
 * Arguments:	Invoke info structure.
 *
 * Returns: 	None.
 *
>*/

Void
emptyFunctionHandler(InvokeInfo * pInvokeInfo)
{
#if 0
    TM_TRACE((LROP_modCB, TM_ENTER, 
    	     "WARNING: Function handler is empty (Don't worry)\n"));
#endif
}

#ifdef AUTHENTICATE_DOMAIN
 
/*<
 * Function:    lrop_authenticateIP
 *
 * Description: Authenticate the IP address
 *
 * Arguments:	IP address
 *
 * Returns: 	0 if authenticated, -1 otherwise.
 *
>*/

SuccFail
lrop_authenticateIP(N_SapAddr *remNsap) 
{
    extern N_SapAddr authorizedDomains[];
    int i;

    for (i = 0; authorizedDomains[i].len; i++) {
    	if (!SAP_nSapDomainCmp(remNsap, &authorizedDomains[i])) {
	    break;
    	}
    }

    if (!authorizedDomains[i].len) {
	EH_fatal("\nProgram is talking to unauthorized network address\n");
	return -1;
    }

    return 0;

} /* lrop_authenticateIP() */
#endif
