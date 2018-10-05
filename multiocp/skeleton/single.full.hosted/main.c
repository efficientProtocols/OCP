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
 * Description: "main" program, and other G_ module functions.
 *
 * Features:
 *           getopt
 *           RELID_ 
 *           CPR_
 *           TM_ 
 *
 *           LOG_
 *           EH_
 * 
 *           CONFIG_
 * 
 *           LIC_
 *
 * Functions:
 *   Int main(int argc, char **argv)
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
static char rcs[] = "$Id: main.c,v 1.3 1998/02/26 06:05:43 mohsen Exp $";
#endif /*}*/

#ifndef TM_ENABLED
#define TM_ENABLED
#endif


#include "estd.h"

/* #include "eh.h" */
#include "getopt.h"
#include "relid.h"
#include "cpr.h"
#include "log.h"
#include "tm.h"

static char *usage = "usage: %s [-V]\n";

Void G_init(), SSP_init(), SSP_action();


PUBLIC TM_ModDesc G_tmDesc;		/* Global Module Trace descriptor */


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
    Int c;
    extern char *optarg;
    extern int optind;
    STATIC Char errbuf[1024];
    Char *copyrightNotice;
    String relidNotice;


    G_init();
    LOG_init();


    optind = 0;			/* reset command line processing by getopt */

    while ((c = getopt(argc, argv, "T:t:V")) != EOF) {
	switch ( c ) {
	case 'T':
	case 't':
	    TM_setUp(optarg);
	    break;

	case 'V':
	    /* Verify that the copyright notice is authentic and print it */
	    {
		if ( ! (copyrightNotice = CPR_getCopyrightNeda()) ) {
		  /*  EH_problem("main: get copyright failed"); */
		    exit(1);
		}
		LOG_event("%s\n", copyrightNotice);
		exit(0);
	    }
	    break;

	default :
	    sprintf(errbuf, usage, argv[0]);
	    /* EH_fatal(errbuf); */
	}
    }


    /* 
     * the first thing a program does print out the version number.
     */

    if ( ! (relidNotice = RELID_getRelidNotice()) ) {
      /* EH_problem("main: get relid failed"); */
	exit(1);
    }
	
    LOG_event("%s\n", argv[0]);
    LOG_event("%s\n\n", relidNotice);
   

    /* 
     * the first thing a program does--check copyright and 
     * print out the version number.
     */

    if ( ! (copyrightNotice = CPR_getCopyrightNotice()) ) {
      /* EH_problem("main: get copyright failed"); */
	exit(1);
    }
	
    LOG_event("%s\n\n", copyrightNotice);

    
    TM_trace(G_tmDesc, TM_ENTER,
	     "G_ can be used by orphand modules\n");

    SSP_init();
    TM_validate();
    SSP_action();
    exit(0);
}


/*<
 * Function:    
 *
 * Description: 
 *
 * Arguments:
 *
 * Returns: 
 *
>*/

Void G_init()
{
    TM_init();
    G_tmDesc = TM_open("G_");
}

/*
 * SSP_ MODULE
 */

TM_ModDesc SSP_tmDesc;


/*<
 * Function:    
 *
 * Description: 
 *
 * Arguments:
 *
 * Returns: 
 *
>*/

Void SSP_init()
{
    SSP_tmDesc = TM_open("SSP_");
}


/*<
 * Function:    
 *
 * Description: 
 *
 * Arguments:
 *
 * Returns: 
 *
>*/

Void SSP_action()
{
    static Char *someData = "SOME DATA";

#ifdef TM_ENABLED
    TM_trace(SSP_tmDesc, TM_ENTER,
	     "Action, someData=%s\n", TM_prAddr(someData));
    TM_hexDump(SSP_tmDesc, TM_ENTER,
	       "someData", someData, strlen(someData)+1);
#endif

#ifdef TM_ENABLED
    if (TM_query(SSP_tmDesc, TM_BIT1)) {
	LOG_event("%s This way we can extend TM_ capabilities\n",
	       TM_here());
    }		 
#endif
}

