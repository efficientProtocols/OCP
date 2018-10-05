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
 * File: pduout.c
 *
 * Description:
 *   LROP-PDU building routines.
 *   Each function in this module takes as arguments all needed parameters, 
 *   allocates a DU_ if needed, prepends and builds the PDU header and
 *   returns a pointer to the DU_ containing a finished PDU.
 *
 * Functions:
 *   lrop_invokePdu
 *   lrop_resultPdu
 *   lrop_failurePdu
 *   lrop_ackPdu
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: pduout.c,v 1.15 1996/09/10 17:26:17 kamran Exp $";
#endif /*}*/

#include "estd.h"
#include "eh.h"
#include "invoke.h"
#include "lropdu.h"

#include "lropfsm.h"
#include "nm.h"
#include "layernm.h"
#include "byteordr.h"

#include "target.h"

#include "local.h"
#include "extfuncs.h"


/*<
 * Function:    lrop_invokePdu
 *
 * Description: Build invoke PDU.
 *
 * Arguments:   SAP selector, invoke reference number, operation value,
 *              encoding type, data.
 *
 * Returns:     Pointer to PDU.
 *
>*/

LOCAL DU_View 
lrop_invokePdu(InvokeInfo *invoke,
	       LSRO_SapSel		remLsroSapSel,	/* Remote Address */
	       Int 			invokeRefNu,
	       LROP_OperationValue	operationValue,
	       LROP_EncodingType	encodingType,
	       DU_View 			data)
{
#ifdef FUTURE
    union BO_Swap swap;
#endif
    register unsigned char *p;
    unsigned char *pdustart;
    DU_View du;


#define INVOKE_PCI_LEN 3

    if ( (du = (data == (DU_View) NULL) ? DU_alloc(G_duMainPool, 0) : data) 
	  == 0) {
	EH_problem("lrop_invokePdu: DU_alloc failed\n");	
	return NULL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "lrop_invokePdu: DU_alloc: du=0x%lx\n", du));

    DU_prepend(du, INVOKE_PCI_LEN);
    p = DU_data(du);                   /* p = du->addr; */
    pdustart = p;

    /* PCI, Byte1 , PDU-TYPE + remLsroSapSel */
    {
	Byte	pciByte1;
	
		/* Easier to debug but less efficient */
	pciByte1 = (remLsroSapSel << 4) | INVOKE_PDU; 
	BO_put1(p, pciByte1);
    }

    /* PCI, Byte-2 , InvokeRefNu */
    BO_put1(p, invokeRefNu);

    /* PCI, Byte-3 , operationValue + EncodingType */
    {
	Byte	pciByte3;
	
	pciByte3 = (encodingType << 6) | operationValue;
	BO_put1(p, pciByte3);
    }

#ifdef TM_ENABLED
    tm_pduPr(LROP_modCB, TM_PDUOUT, "->", du, MAX_LOG);
#endif

    NM_incCounter(NM_LSROS, &lrop_pduSentCounter, 1);
    return (du);
}


/*<
 * Function:    lrop_resultPdu
 *
 * Description: Build result or error PDU.
 *
 * Arguments:   Invoke info, invoke reference number, local SAP selector, 
 *              remote SAP selectore, encoding type, data.  
 *
 * Returns:     Pointer to PDU.
>*/

LOCAL DU_View 
lrop_resultPdu(InvokeInfo *invoke,
	       Int invokeRefNu,
	       LSRO_SapSel		locLsroSapSel,	
	       LSRO_SapSel		remLsroSapSel,	
	       LROP_EncodingType	encodingType,
	       DU_View data)
{
#ifdef FUTURE
    union BO_Swap swap;
#endif
    register unsigned char *p;
    unsigned char *pdustart;
    DU_View du;

#define RESULT_PCI_LEN 2
 
    if ( (du = (data == (DU_View) NULL) ? DU_alloc(G_duMainPool, 0) : data) 
	 == 0) {
	EH_problem("lrop_resultPdu: DU_alloc failed\n");	
	return NULL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "lrop_resultPdu: DU_alloc: du=0x%lx\n", du));

    DU_prepend(du, RESULT_PCI_LEN);
    p = DU_data(du);
    pdustart = p;

    /* PCI, Byte1 , PDU-TYPE + encodingType */
    {
	Byte	pciByte1;
	
    	pciByte1 = (encodingType << 6) | RESULT_PDU; 

	BO_put1(p, pciByte1);
    }

    /* PCI, Byte-2 , InvokeRefNu */
    BO_put1(p, invokeRefNu);

#ifdef TM_ENABLED
    tm_pduPr(LROP_modCB, TM_PDUOUT, "->", du, MAX_LOG);
#endif

    NM_incCounter(NM_LSROS, &lrop_pduSentCounter, 1);
    return (du);
}


/*<
 * Function:    lrop_errorPdu
 *
 * Description: Build error PDU.
 *
 * Arguments:   Invoke info, invoke reference number, local SAP selector, 
 *              remote SAP selectore, encoding type, error value, data.  
 *
 * Returns:     Pointer to PDU.
>*/

LOCAL DU_View 
lrop_errorPdu(InvokeInfo *invoke,
	      Int invokeRefNu,
	      LSRO_SapSel	locLsroSapSel,	
	      LSRO_SapSel	remLsroSapSel,	
	      LROP_EncodingType	encodingType,
	      LROP_ErrorValue	errorValue,
	      DU_View data)
{
#ifdef FUTURE
    union BO_Swap swap;
#endif
    register unsigned char *p;
    unsigned char *pdustart;
    DU_View du;

#define ERROR_PCI_LEN 3
 
    if ( (du = (data == (DU_View) NULL) ? DU_alloc(G_duMainPool, 0) : data) 
	 == 0) {
	EH_problem("lrop_errorPdu: DU_alloc failed\n");	
	return NULL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "lrop_errorPdu: DU_alloc: du=0x%lx\n", du));

    DU_prepend(du, ERROR_PCI_LEN);
    p = DU_data(du);
    pdustart = p;

    /* PCI, Byte1 , PDU-TYPE + encodingType */
    {
	Byte	pciByte1;
	
    	pciByte1 = (encodingType << 6) | ERROR_PDU; 

	BO_put1(p, pciByte1);
    }

    /* PCI, Byte-2 , InvokeRefNu */
    BO_put1(p, invokeRefNu);

    /* PCI, Byte-3 , ErrorValue */
    BO_put1(p, errorValue);

#ifdef TM_ENABLED
    tm_pduPr(LROP_modCB, TM_PDUOUT, "->", du, MAX_LOG);
#endif

    NM_incCounter(NM_LSROS, &lrop_pduSentCounter, 1);
    return (du);
}



/*<
 * Function:    lrop_failurePdu
 *
 * Description: Build failure PDU.
 *
 * Arguments:   Invoke info, invoke reference number, failure reason, data.  
 *
 * Returns:     Pointer to PDU.
 *
>*/

DU_View 
lrop_failurePdu(InvokeInfo *invoke, short unsigned int invokeRefNu, 
		unsigned char reason, DU_View data)
{
#ifdef FUTURE
    union BO_Swap swap;
#endif
    register unsigned char *p;
    unsigned char *pdustart;
    DU_View du;
    int     len;

    len = 3; /* NOTYET */

    if ( (du = (data == (DU_View) NULL) ? DU_alloc(G_duMainPool, 0) : data) 
	 == 0) {
	EH_problem("lrop_failurePdu: DU_alloc failed\n");	
	return NULL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "lrop_failurePdu: DU_alloc: du=0x%lx\n", du));

    DU_prepend(du, len);
    p = DU_data(du);
    pdustart = p;

/*    BO_put1(p, 0); */
    BO_put1(p, FAILURE_PDU);
    BO_put1(p, invokeRefNu);
    BO_put1(p, reason);

/*    len = p - pdustart - 1;
    *pdustart = len;
*/

#ifdef TM_ENABLED
    tm_pduPr(LROP_modCB, TM_PDUOUT, "->", du, MAX_LOG);
#endif

    NM_incCounter(NM_LSROS, &lrop_pduSentCounter, 1);
    return (du);
}


/*<
 * Function:    lrop_ackPdu
 *
 * Description: Build ACK PDU.
 *
 * Arguments:   Invoke info, invoke reference number.  
 *
 * Returns:     Pointer to PDU.
 *
>*/

DU_View 
lrop_ackPdu(InvokeInfo *invoke,
	    Int invokeRefNu)
{
#ifdef FUTURE
    union BO_Swap swap;
#endif
    register unsigned char *p;
    unsigned char *pdustart;
    DU_View du;

#define ACK_PCI_LEN 2

    if ((du = DU_alloc(G_duMainPool, ACK_PCI_LEN)) == 0) {
	EH_problem("lrop_ackPdu: DU_alloc failed\n");	
	return NULL;
    }

    TM_TRACE((DU_modCB, DU_MALLOC, "lrop_ackPdu: DU_alloc: du=0x%lx\n", du));

    p = DU_data(du);
    pdustart = p;


    /* PCI, Byte1 , PDU-TYPE  */
    {
	Byte	pciByte1;
	
	pciByte1 =  ACK_PDU; 
	BO_put1(p, pciByte1);
    }

    /* PCI, Byte-2 , InvokeRefNu */
    BO_put1(p, invokeRefNu);

#ifdef TM_ENABLED
    tm_pduPr(LROP_modCB, TM_PDUOUT, "->", du, MAX_LOG);
#endif

    NM_incCounter(NM_LSROS, &lrop_pduSentCounter, 1);
    return (du);
}
