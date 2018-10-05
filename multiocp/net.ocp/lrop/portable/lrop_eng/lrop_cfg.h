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
 * File name: lrop_cfg.h
 *
 * Description: Configuration file of LSRO Protocol Engine.
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 * 
 */

/*
 * RCS Revision: $Id: lrop_cfg.h,v 1.10 1996/10/14 18:25:47 kamran Exp $
 */

#ifndef _LROP_CFG_H_	/*{*/
#define _LROP_CFG_H_

extern Int udpSapSel;
extern Int invokePduSize;

#define LROP_K_UdpSapSel 2002
#define LROP_SAPS        200
#define LROP_INVOKES     200

#define NREFS 256  

#define INVOKE_PDU_SIZE  1500

/*
#ifdef MTS_COMPLETE
#ifdef UA_COMPILATION_2WAY_ONLY
#ifdef UA_COMPILATION_3WAY_ONLY
#ifdef UA_COMPILATION_2WAY_AND_3WAY
*/

#endif	/*}*/
