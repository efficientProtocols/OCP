/*
 *  Copyright (C) 1995,1996  AT&T Wireless Services, Inc. All rights reserved.
 *  Copyright (C) 1995,1996  Neda Communications, Inc. All rights reserved.
 * 
 *  This software is furnished under a license and use duplication,
 *  disclosure and all other uses are restricted to the rights specified
 *  in the written license between the licensee and copyright holders.
 * 
 */

#include "estd.h"
#include "target.h"
#include "getopt.h"
#include "config.h"
#include "lsro.h"
#include "tm.h"
#include "eh.h"

extern int udpSapSel;
extern int nuOfSaps;
extern int nuOfInvokes;
extern int invokePduSize;
extern int retransmitCount;

extern struct TimerValue {
    Int	    acknowledgment;
    Int	    roundtrip;
    Int	    retransmit;
    Int	    maxNSDULifeTime;
} timerValue; 

char errbuf[1024];

#if 0
extern ParamBoundary paramBoundary[]; 
#endif

#define FIRST_PARAM 0		/* move it and change it */
#define LAST_PARAM  10 		/* move it and change it */

Int
LSRO_setConf(Int param, Int value)
{
    if (param > LAST_PARAM  ||  param < FIRST_PARAM) {
	return -1;  /* INVALID_PARAM */
    }

#if 0
    if (value < paramBoundary[param].min  ||
	value > paramBoundary[param].max) {
	return -2;  /* INVALID_VALUE */
    }
#endif
	    
    switch (param) {
	case LSRO_PortNumber:
	    udpSapSel = value;
	    break;
	case LSRO_MaxSAPs:
	    nuOfSaps = value;
	    break;
	case LSRO_MaxInvokes:
	    nuOfInvokes = value;
	    break;
/*	case LSRO_MaxReference: */ /* Not yet */
	case LSRO_PduSize:
	    invokePduSize = value;
	    break;
	case LSRO_Acknowledgement:
	    timerValue.acknowledgment = value;
	    break;
	case LSRO_Roundtrip:
	    timerValue.roundtrip = value;
	    break;
	case LSRO_Retransmit:
	    timerValue.retransmit = value;
	    break;
	case LSRO_MaxNSDULifeTime:
	    timerValue.maxNSDULifeTime = value;
	    break;
	case LSRO_RetransmitCount:
	    retransmitCount = value;
    }

    return 0;
}

Int
LSRO_getConf(Int param, Int *value)
{
    if (param > LAST_PARAM  ||  param < FIRST_PARAM) {
	return -1;  /* INVALID_PARAM */
    }

    switch (param) {
	case LSRO_PortNumber:
	    *value = udpSapSel;
	     break;
	case LSRO_MaxSAPs:
	    *value = nuOfSaps;
	     break;
	case LSRO_MaxInvokes:
	    *value = nuOfInvokes;
	     break;
/*	case LSRO_MaxReference: NOT YET
	    *value = ; */
	case LSRO_PduSize:
	    *value = invokePduSize;
	     break;
	case LSRO_Acknowledgement:
	    *value = timerValue.acknowledgment;
	     break;
	case LSRO_Roundtrip:
	    *value = timerValue.roundtrip;
	     break;
	case LSRO_Retransmit:
	    *value = timerValue.retransmit;
	     break;
	case LSRO_MaxNSDULifeTime:
	    *value = timerValue.maxNSDULifeTime;
	     break;
	case LSRO_RetransmitCount:
	    *value = retransmitCount;
	     break;
    }

    return 0;
}



#ifdef TEST
main()
{
    setConf();
}
#endif
