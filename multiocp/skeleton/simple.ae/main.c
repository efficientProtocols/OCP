/*
 *  This file is part of the Open C Platform (OCP) Library. OCP is a
 *  portable library for development of Data Communication Applications.
 * 
 *  Copyright (C) 1995 Neda Communications, Inc.
 * 	Prepared by Mohsen Banan (mohsen@neda.com)
 * 
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.  This library is
 *  distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 *  License for more details.  You should have received a copy of the GNU
 *  Library General Public License along with this library; if not, write
 *  to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139,
 *  USA.
 * 
 */

/*+
 * File name: main.c
 *
 * Description: Provider "main" program, and other G_ module functions.
 *
 * Functions:
 *   Int main(int argc, char **argv)
 *   G_heartBeat(void)
 *   G_exit(Int unUsed)
 *   G_init(void)
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: main.c,v 1.1.1.1 1998/01/30 01:13:13 mohsen Exp $";
#endif /*}*/


#include "estd.h"
/* #include "tm.h" */
/* #include "eh.h" */
#include "getopt.h"
#include "relid.h"
#include "cpr.h"

#include "target.h"
#include "dos.h"
#include "string.h"
#include "ae_power.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "memory.h"
#include "ae_init.h"          /* A-Engine initializations */
#include "aecomprt.h"

pComPortHandle pCom;

void OpenCom3(void)
{
	pCom = ComPortOpen(COM3, "Com Port 3");
	ComPortCtrl(pCom, BAUD_19200, Enable, CL_Eight, PT_None);
	
}

static char dbgBuffer[256];
int dbgPrint(const char *format, ... )
{
	va_list	args;
	va_start( args, format );
	vsprintf(dbgBuffer, format, args );
	va_end( args );
	ComPortPuts(pCom, dbgBuffer);
	return 0;
}


void InitSystem(void);

static ledd = 0;


/*<
 * Function:    main()
 *
 * Description: Provider main program.
 *
 * Arguments:   argc, argv.
 *
 * Returns:     0 on successful execution, other than 0 on unsuccessful.
 * 
>*/

void
main(int argc, char **argv)
{
    int c;
    extern char *optarg;
    extern int optind;
    char errbuf[1024];
    Char *copyrightNotice;
    String relidNotice;


    InitSystem();

    OpenCom3();
	

    /* 
     * the first thing a program does print out the version number.
     */

    if ( ! (relidNotice = RELID_getRelidNotice()) ) {
      /* EH_problem("main: get relid failed"); */
	exit(1);
    }
	
    dbgPrint("%s\n\n", relidNotice);
   

    /* 
     * the first thing a program does--check copyright and 
     * print out the version number.
     */

    if ( ! (copyrightNotice = CPR_getCopyrightNotice()) ) {
      /* EH_problem("main: get copyright failed"); */
	exit(1);
    }
	
    dbgPrint("%s\n\n", copyrightNotice);

    while ( TRUE ) {
	;
    }

}
