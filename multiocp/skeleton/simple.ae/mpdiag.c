/*
 * $Id: mpdiag.c,v 1.1.1.1 1998/01/30 01:13:13 mohsen Exp $
 *
 * A-Engine MP215 CSCDPD modem diagnotics code
 *
 * Copyright Sierra Wireless Inc., 1996. All rights reserved.
 *
 * 
 * Functions:
 *  
 *
 */

#include "target.h"
#include "dos.h"
#include "string.h"
#include "ae_power.h"


/*------------------------------------------------------------
 * void main()
 *
 *
 *
 * Inputs:
 *  none
 *
 * Outputs:
 *  none
 *
 *
 *
 */
void mp215Diag()
{
#ifndef FAST
    {
	/* Check for overrun and other errors on Com2
	 */
	extern int AeCom2ErrorDetected;
	if (AeCom2ErrorDetected) {
	    dbgPrint("ERR: AeCom2ErrorDetected=0x%x", AeCom2ErrorDetected);
	    AeCom2Status();
	    AeCom2ErrorDetected = 0;
	} 
    }	
#endif /* FAST */
}

/*
 * $Log: mpdiag.c,v $
 * Revision 1.1.1.1  1998/01/30 01:13:13  mohsen
 * Imported sources
 *
 * Revision 1.2  1997/12/31 09:02:41  mohsen
 * Com2 bug fixes.
 *
 * Revision 1.1  1997/12/29 09:37:35  mohsen
 * Mohsen's general cleanups.
 *
 *
 */
