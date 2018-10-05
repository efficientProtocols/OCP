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
 * File: pdupr.c
 *
 * Description: Print PDU.
 *
 * Functions: tm_pduPr (char *str, DU_View du, char *p, int maxLog)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: pdupr.c,v 1.12 1996/09/09 10:29:30 kamran Exp $";
#endif /*}*/

#include "estd.h"
#include "invoke.h"
#include "du.h"
#include "lropfsm.h"
#include "nm.h"
#include "layernm.h"
#include "byteordr.h"
#include "lropdu.h"

#include "local.h"

#ifdef TM_ENABLED
static char *typeStr[5] = {
    	"inv",		/* 0x00 */
	"res",		/* 0x01 */
	"err",		/* 0x02 */
	"ack",		/* 0x03 */
	"fai",		/* 0x04 */
};
#endif


/*<
 * Function:    tm_pduPr
 *
 * Description: Print PDU contents.
 *
 * Arguments:   Message string, pointer to data unit, print addr, length.
 *
 * Returns:     None.
 *
>*/

#ifdef TM_ENABLED
LOCAL Void
tm_pduPr(tm_ModInfo *modInfo, TM_Mask mask, char *str, DU_View du, int maxLog)
{
    extern FILE *tmFile;
    extern logPdu();

    char *duData, *duDataStart;
    Int  duSize;
    unsigned char c;
    Int   length;
    Bool  pduCut = 0;
    LgInt lastLogAddr;

    if ( !(modInfo->mask & mask) ) {
	return;
    }

    TM_TRACE((LROP_modCB, TM_ENTER,
	    "tm_pduPr:  type=(%s): duAddr=%lu  dataAddr=%lu  DUsize=%d\n",
	    str, TM_prAddr((Ptr) du), TM_prAddr((Ptr) DU_data(du)), DU_size(du)));

    /* PCI, Byte1 , PDU-TYPE + remLsroSapSel or encodingType ... */

    fprintf(tmFile, "------------------------------------------------------------------------\n");

    duDataStart = duData = DU_data(du);
    duSize = DU_size(du);

    BO_get1(c, duData);

    switch (c & 0x07) {

    case INVOKE_PDU:

	fprintf (tmFile, "%s %s  ", str, typeStr[c & 0x07]);
	/* PCI, BYTE-1 */
	fprintf (tmFile, "Rem=%d  ",  (c & 0xF0) >> 4);	
	fprintf (tmFile, "Loc=%d  ", ((c & 0xF0) >> 4) - 1);

	/* PCI Byte-2 */
	BO_get1(c, duData);
	fprintf (tmFile, "Ref=%u  ", c);	

	/* PCI, Byte-3 , operationValue + EncodingType */
	BO_get1(c, duData);  

	fprintf (tmFile, "OpVal=%d  ", c & 0x3F);
	fprintf (tmFile, "Encod=%d\n", (c & 0xC0) >> 6);

	break;

    case RESULT_PDU:
	fprintf (tmFile, "%s %s  ", str, typeStr[c & 0x07]);
	/* PCI, BYTE-1 */
	fprintf (tmFile, "Encod=%d  ", (c & 0xC0) >> 6);

	/* PCI Byte-2 */
	BO_get1(c, duData);
	fprintf (tmFile, "RefNo=%u\n", c);	

	break;

    case ERROR_PDU:

	fprintf (tmFile, "%s %s  ", str, typeStr[c & 0x07]);
	/* PCI, BYTE-1 */
	fprintf (tmFile, "\nEncod=%d  ", (c & 0xC0) >> 6);

	/* PCI Byte-2 */
	BO_get1(c, duData);
	fprintf (tmFile, "RefNo=%u  ", c);	

	/* PCI Byte-3 */
	BO_get1(c, duData);
	fprintf (tmFile, "ErrVal=%u\n", c);	

	break;

    case ACK_PDU:

	fprintf (tmFile, "%s %s  ", str, typeStr[c & 0x07]);
	/* PCI Byte-2 */
	BO_get1(c, duData);
	fprintf (tmFile, "RefNo=%u\n", c);	

        return;

    case FAILURE_PDU:

	fprintf (tmFile, "%s %s  ", str, typeStr[c & 0x07]);
	BO_get1(c, duData);
	fprintf (tmFile, "RefNo=%u\n", c);	

	break;

    default:
	fprintf (tmFile, "Unknown PDU type %s\n", str );
	break;
    }

    if ( (length = duSize) > maxLog ) {
	length = maxLog;
	pduCut = 1;
    }

    lastLogAddr = TM_hexDump(LROP_modCB, TM_PDUIN | TM_PDUOUT, 
                             str, DU_data(du), length-1);

    if ( pduCut ) {
	fprintf (tmFile, "%05ld: ...hex dump is cut...\n", lastLogAddr);
    }

} /* tm_pduPr() */
#endif

