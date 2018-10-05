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
 * File name: layernm.h (Network Management)
 *
 * Description: Counter defintion 
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 * 
 */

/*
 * RCS Revision: $Id: layernm.h,v 1.5 1996/09/09 10:29:15 kamran Exp $
 */

#ifndef _LSROSNM_H_	/*{*/
#define _LSROSNM_H_

#include "nm.h"

extern Counter lrop_pduRetranCounter;
extern Counter lrop_completeOperationCounter;
extern Counter lrop_protocolErrorsCounter;

extern Counter lrop_pduSentCounter;
extern Counter lrop_invokePduRetranCounter;
extern Counter lrop_badAddrCounter;
extern Counter lrop_opRefusedCounter;

#ifdef LINT_ARGS	/* Arg. checking enabled */

extern int lrop_nm_incr (int layer, register Counter *counter, int increment); 
extern int lrop_nm_event (int layer, int eventid); 

#else

extern int lrop_nm_incr ();
extern int lrop_nm_event ();

#endif /* LINT_ARGS */

#endif	/*}*/
