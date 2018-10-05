/*
 * $Id: dbgput.c,v 1.1.1.1 1998/01/30 01:13:13 mohsen Exp $
 *
 * Z80 specific cut down string i/o rtns
 *
 * Functions:
 * dbgPutString -- Low level print of a string
 * dbgPutWord -- Prints hex word.
 *
 * Copyright Sierra Wireless Inc. 1994
 */

/* ---- Include Files ----------------------------------- */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "target.h"
#include "decl.h"
#include "dbg.h"
//#if (CPU == Z80)
//#include "z182.h"
//#endif
	
pComPortHandle pComDbg=NULL;

unsigned int dbgMode = 	DBGMODE_OFF;

// Initialize COM1
BOOL dbgInit(ComPort Port)
{
	unsigned int curdbg = dbgMode;

	dbgMode = DBGMODE_OFF;
	if (pComDbg != NULL)
		ComPortClose(pComDbg);

	// Serial port 1 Initialization 19200:8:N:1
	pComDbg = ComPortOpen(Port, "Debug Port");
	if (pComDbg)
	{
		dbgMode = curdbg;
		return (TRUE);
	}
	return (FALSE);
}



/* ---- Public Variable Declarations -------------------- */
/* ---- Private Constant and Type Declarations ---------- */
const char htoa[] = {'0','1','2','3','4','5','6','7','8','9',
							'a','b','c','d','e','f'};
/* ---- Private Variable Declarations ------------------- */
/* ---- Private Function Prototypes --------------------- */
/* ---- Functions --------------------------------------- */

/*------------------------------------------------------------
 * dbgPutString -- Low level print of a string
 *
 * Inputs:
 *  *str   -   ptr to string 
 *
 * Outputs:
 *  none    
 *
 * Returns:
 *
 * Notes:
 */
void
dbgPutString(const char *str)
{
	if	(dbgMode & DBGMODE_DBG)
	{
		while (*str != '\0')
		{
			if (*str == '\n')
			{
				/* Do \r before \n due to term */
				while(ComPortPutC(pComDbg, '\r') == 0);
			}
				while(ComPortPutC(pComDbg, *str) == 0);
			str++;
		}
	}
}

void dbgDump(char *str, unsigned char *data, int len)
{
	int i,j;
	char buffer[80];
	unsigned char c;
	i=0; 
		
	if	(dbgMode & DBGMODE_DUMP)
	{
		dbgPrint("%s", str);

		while(i<len)
		{
			memset(buffer, 32, 80);	
			buffer[79] = 0;	
			for(j=0; j< 16 && i<len; j++, i++)
			{
				c = data[i];
				buffer[(j*3)] = 	htoa[c >> 4];
				buffer[(j*3)+1] = 	htoa[c & 0x0f];
				if (c >=32 && c <=127)
					buffer[55+j] = c;
				else
					buffer[55+j] = '.';
			}
			dbgPrint("%s", buffer);
		}
	}
}
	
	
/*------------------------------------------------------------
 * dbgPutWord -- Prints hex word.
 *
 * Inputs:
 *  data  -   uword to print 
 *
 * Outputs:
 *  none
 *
 * Returns:
 *
 * Notes:
 */

void
dbgPutWord(UWORD data)
{
	char	str[5];
	BYTE	i;

	str[4] = '\0';
	for (i= 3; i >= 0; i--)
	{
		str[i] = htoa[data & 0x000f];
		data = data>>4;
	}
	dbgPutString(str);
}
/*
 * $Log: dbgput.c,v $
 * Revision 1.1.1.1  1998/01/30 01:13:13  mohsen
 * Imported sources
 *
 * Revision 1.4  1997/12/18 07:18:29  mohsen
 * Bug Fixes.
 *
 * Revision 1.3  1997/12/16 01:19:47  mohsen
 * Minor clean ups and mob+udp fixes.
 *
 * Revision 1.2  1997/12/08 23:32:43  mohsen
 * David's first cut of MP215 modem code.
 *
 * Revision 1.1.1.1  1997/11/22 01:17:54  mohsen
 * Imported sources
 *
 * Revision 1.2  1997/11/20 02:26:18  mohsen
 * David's Baud rate fix.
 *
 * Revision 1.1.1.1  1997/11/13 03:14:22  mohsen
 * Imported sources
 *
 * Revision 1.3  1997/11/11 02:30:09  mohsen
 * Dave's new Com Driver Interface.
 *
 * Revision 1.2  1997/10/30 06:15:41  mohsen
 * David's new work.
 *
 * Revision 1.1  1997/10/15 22:52:56  mohsen
 * After Reorg and clean up
 *
 * Revision 2.1  1995/07/11  22:00:26  lukas
 * Bump revision to 2.1 for CDPD R1.1
 *
 * Revision 1.1  1994/05/31  18:29:18  overton
 * Initial commit, dbgPutString/dbgPutWord allow simple i/o without overhead
 * of formatter.
 *
 */

