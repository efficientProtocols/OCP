/*+
 * Description:
 *
-*/

/*
 * Author: Mohsen Banan.
 * History:
 *
 */

#ifdef RCS_VER	/*{*/
static char rcs[] = "$Id: upper.c,v 1.1.1.1 1995/05/25 07:09:17 mohsen Exp $";
#endif /*}*/

#include "estd.h"
#include "eh.h"
#include "tm.h"

#include "lrop.h"

LOCAL TM_ModuleCB *LROP_modCB;


PUBLIC SuccFail
LROP_init(void)
{
    if ( ! (LROP_modCB = TM_open("LROP_")) ) {
	EH_problem(ZPTR);
	return ( FAIL );
    }
}

PUBLIC LROP_SapDesc
LROP_sapBind(LSRO_SapSel sapSel, 
	     int (*invokeInd) (LROP_InvokeDesc invoke,
			       LSRO_SapSel 	locLSROSap,
			       LSRO_SapSel 	remLSROSap,
			       T_SapSel		*remTsap,
			       N_SapAddr		*remNsap,
			       LROP_OperationValue	operationValue,
			       LROP_EncodingType	encodingType,
			       DU_View		parameter),
	     int (*resultInd) (LROP_InvokeDesc invoke,
			       LROP_EncodingType	encodingType,
			       DU_View		parameter),
	     int (*errorInd) (LROP_InvokeDesc invoke,
			      LROP_EncodingType	encodingType,
			      DU_View 		parameter),
	     int (*failureInd) (LROP_InvokeDesc invoke,
				 LROP_FailureValue	failureValue))
{
}



/*<
 * Function:
 * Description:
 *
 * Arguments:
 *
 * Returns:
 *
 * 
>*/
PUBLIC SuccFail
LROP_sapUnBind(LSRO_SapSel sapSel)
{
}

PUBLIC LROP_InvokeDesc 
/* PUBLIC InvokeInfo * */
LROP_invokeReq(LSRO_SapSel locLSROSap,
	       LSRO_SapSel remLSROSap,
	       T_SapSel *remTsap,
	       N_SapAddr *remNsap,
	       LROP_OperationValue opValue,
	       LROP_EncodingType encodingType,
	       DU_View parameter)
{
    return ( (LROP_InvokeDesc) 0);
}


/*<
 * Function:
 * Description:
 *
 * Arguments:
 *
 * Returns:
 *
>*/
PUBLIC  Int
LROP_resultReq(LROP_InvokeDesc invokeDesc,
	       LROP_EncodingType encodingType, 
	       DU_View parameter)

{
}


/*<
 * Function:
 * Description:
 *
 * Arguments:
 *
 * Returns:
 *
>*/
PUBLIC  Int
LROP_errorReq(LROP_InvokeDesc invokeDesc,
	      LROP_EncodingType encodingType, 
	      DU_View parameter)

{
}











