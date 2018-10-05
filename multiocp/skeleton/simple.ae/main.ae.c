/*
 * $Id: main.ae.c,v 1.1.1.1 1998/01/30 01:13:13 mohsen Exp $
 *
 * A-Engine 215 CSCDPD modem code
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
#include "task.h"
#include "ini.h"
#include "snlink.h"
#include "os.h"
#include "tmr.h"
#include "upper.h"
#include "version.h"

void InitSystem(void);

/* Memory pool.  Doing it this way because thats how they did it
 * in the "Write TSR Now" book.  Malloc seems to work but who am I
 * to argue with the author
 */
#define MEM_POOL_SIZE	(18500)
static char memArray[MEM_POOL_SIZE];

#define HOST_MSG_SIG 1 
lstHEAD HostRxList;						/* transmission list */
msgMBOX HostMbox;
osTASK HostRxTask;

static tmrTIMER tmrLed;
static ledd = 0;


/*------------------------------------------------------------
 * void HostRxComplete(UWORD lengthTransfered, UBYTE  * ipBufferPtr)
 *
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 */
void HostRxComplete(UWORD lengthTransfered, UBYTE  * ipBufferPtr)
{
	dbgPrint("Host RX Complete Len = %d",lengthTransfered); 
	upRxComplete(ipBufferPtr, lengthTransfered);

}

/*------------------------------------------------------------
 * void HostGetHostBuffer(UWORD ipLength, UBYTE  ** ipBufferPtr)
 *
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 */
void HostGetHostBuffer(UWORD ipLength, UBYTE  ** ipBufferPtr)
{
	dbgPrint("Host RX get buffer");
	*ipBufferPtr = upGetBuffer(ipLength);
}	


/*------------------------------------------------------------
 * void HostSendPkt(UBYTE *ipData, UWORD ipDataLen)
 *
 *
 * Inputs:
 *
 * Outputs:
 *
 * Returns:
 *
 */
void HostSendPkt(UBYTE *ipData, UWORD ipDataLen)
{
	HostSendPacketToSn(ipData, ipDataLen);
}


/*------------------------------------------------------------
 * void LedTimer(void)
 *
 *  LED Task - Blink LED
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
void LedTimer(void)
{
	ledd=~ledd;
	AeLed(ledd);
	tmrStart(&tmrLed, tmrMsec2Tick(500));
}




/*------------------------------------------------------------
 * HostTaskMain -- Host Task Main function
 *
 * Inputs:
 *  none
 *
 * Outputs:
 *  none
 *
 * Returns:
 *  none
 *
 * Notes:       Just does initialization and returns
 */
void HostTaskMain(void)
{

	// Init task procedure
	osInitTask(&HostRxTask, "HOST", HostReceiveFromSNTaskProc, HOST_MSG_SIG);

	// Init mailbox to receive incoming packet messages from "sn"
	msgInitMbox(&HostMbox, "HOST", &HostRxTask, HOST_MSG_SIG);

	// 	Setup and Start LED blink
	tmrSetup(&tmrLed, (tmrFXN) LedTimer, 0, 0);
	tmrStart(&tmrLed, tmrMsec2Tick(500));
}



/*------------------------------------------------------------
 * pdInitAll --     Initialize the complete System. Install as packet driver,
 *                      set up timer and int28 routines, initialize variables
 *
 * Inputs:
 *  none
 *
 * Outputs:
 *  none
 *
 * Returns: --      True - everything ok, False- error
 *
 *
 */
static void pdInitAll(void)
{
	char far *memPtr;
	size_t memSize = MEM_POOL_SIZE;

	lstInit(&HostRxList);

	/* Initialize Public variables  */
	memPtr = memArray;
	memset(memArray, '*', sizeof (memArray));

	if (taskInit((void *)memPtr, memSize))
	{
		HostTaskMain();
		logInit(NULL);
		//tmrSetup(&tmrWdog, (tmrFXN) Watchdog, 0, 0);

		/* Second Round initialization */
		taskInit2();
		
		//tmrStart(&tmrWdog, Sec2Tick(60));
	}
}


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
void main()
{
	pComPortHandle pComMdm;
	int i;
	int sz = sizeof(hstATCFG);

	// init a-engine and timer tick
	InitSystem();


	if (sz != 150)
	{
		while(1)
		{
		}
	}

	// setup initialization data
	UpperInit();

	dbgPrint("Sierra Wireless CS-CDPD driver");
	dbgPrint("%s", I5_PRODUCTID);


#if (COMCFG == COM_MP215)
	// turn on modem
	MODEM_ON;

	dbgPrint("Waiting for Modem to power up...");
	
	// MB -- NOTYET, how can we tell when the modem is on other 
	// than the way we are doing it.

	osSleep(3000);
	
	pComMdm = ComPortOpen(MODEM_PORT, "Modem (SLIP) Port");
	
	dbgPrint("Waiting for CTS");

	ComPortSetDTR(pComMdm, Enable);

	while(ComPortGetCts(pComMdm) == Disable);

	ComPortClose(pComMdm);
	
	dbgPrint("Modem Up!!");
#endif

	// Initialize system
	pdInitAll();
	
	// run forever
	UpperRun();
}
/*
 * $Log: main.ae.c,v $
 * Revision 1.1.1.1  1998/01/30 01:13:13  mohsen
 * Imported sources
 *
 * Revision 1.4  1997/12/29 09:37:06  mohsen
 * Mohsen's general cleanups.
 *
 * Revision 1.3  1997/12/19 23:10:49  mohsen
 * David's Major Fixes for timers and bugs.
 *
 * Revision 1.2  1997/12/18 06:50:16  mohsen
 * Bug Fixes.
 *
 * Revision 1.1  1997/12/08 23:14:39  mohsen
 * David's first cut of MP215 modem code.
 *
 * Revision 1.1.1.1  1997/11/22 01:17:58  mohsen
 * Imported sources
 *
 */
