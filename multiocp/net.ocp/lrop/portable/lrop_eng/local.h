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
 * File name: local.h
 *
 * Description:
 *
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 * 
 */

/*
 * RCS Revision: $Id: local.h,v 1.11 1996/09/09 10:29:16 kamran Exp $
 */


#ifndef _LOCAL_H_	/*{*/
#define _LOCAL_H_

#include "estd.h"
#include "du.h"
#include "tm.h"

#define TM_LROPFSM  TM_BIT1
#define DU_MALLOC   TM_BIT10

#define TM_PDUIN    TM_BIT5
#define TM_PDUOUT   TM_BIT6

#define MAX_LOG     80

LCL_XTRN TM_ModuleCB *LROP_modCB;
LCL_XTRN TM_ModuleCB *DU_modCB;

extern DU_Pool *G_duMainPool;

#endif	/*}*/


