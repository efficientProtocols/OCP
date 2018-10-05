/*
 * Copyright (C) 1995,1996  AT&T Wireless Services, Inc. All rights reserved.
 * Copyright (C) 1995,1996  Neda Communications, Inc. All rights reserved.
 *
 * This software is furnished under a license and use, duplication,
 * disclosure and all other uses are restricted to the rights specified
 * in the written license between the licensee and copyright holders.
 *
*/

#include "lropfsm.h"
#include "fsm.h"
#include "fsmtrans.h"
#include "lrop.h"

#ifdef LINT_ARGS	/* Arg. checking enabled */

extern SuccFail 
lrop_relPdu(InvokeInfo *invokeInfo, PduSeq *pduSeq);
extern SuccFail 
lrop_relAllPdu(InvokeInfo *invokeInfo);
extern DU_View 
lrop_pduRetrieve(InvokeInfo *invokeInfo, PduSeq *pduSeq);
extern Void 
lrop_pduKeep(InvokeInfo *invokeInfo, PduSeq *pduSeq, DU_View dt);
extern DU_View 
lrop_invokePdu (InvokeInfo *invoke, LSRO_SapSel remLsroSapSel, 
	        Int invokeRefNu, LROP_OperationValue operationValue, 
		LROP_EncodingType encodingType, DU_View data);
extern DU_View lrop_resultPdu (InvokeInfo *invoke, Int invokeRefNu, LSRO_SapSel locLsroSapSel, LSRO_SapSel remLsroSapSel, LROP_EncodingType encodingType, DU_View data);
extern DU_View lrop_errorPdu (InvokeInfo *invoke, Int invokeRefNu, LSRO_SapSel locLsroSapSel, LSRO_SapSel remLsroSapSel, LROP_EncodingType encodingType, LROP_ErrorValue errorValue, DU_View data);
extern DU_View lrop_ackPdu (InvokeInfo *invoke, Int invokeRefNu);
extern Void lrop_freeInvoke (InvokeInfo *);
extern Void lrop_relInvoke(InvokeInfo *invoke);

extern Int lrop_resultInd (InvokeInfo *invoke, 
			   LROP_UserInvokeRef userInvokeRef,
			   LROP_EncodingType encodingType, 
			   DU_View parameter);
extern Int lrop_errorInd (InvokeInfo *invoke, 
			  LROP_UserInvokeRef userInvokeRef,
			  LROP_EncodingType encodingType, 
			  LROP_ErrorValue errorValue, DU_View parameter);
extern Int lrop_failureInd (InvokeInfo *invoke, 
			    LROP_UserInvokeRef userInvokeRef,
			    LROP_FailureValue failureValue);

extern SuccFail lrop_pduSeqInit(PduSeq *p);

extern PUBLIC FSM_TransDiagram *lrop_CLInvokerTransDiag (void);
extern PUBLIC FSM_TransDiagram *lrop_CLPerformerTransDiag (void);
extern PUBLIC FSM_TransDiagram *lrop_2CLInvokerTransDiag (void);
extern PUBLIC FSM_TransDiagram *lrop_2CLPerformerTransDiag (void);
extern LOCAL Void tm_pduPr(tm_ModInfo *modInfo, TM_Mask mask, char *str, DU_View du, int maxLog);

extern Int lrop_invokeInd (InvokeInfo *invoke, LSRO_SapSel locLSROSap, 
			   LSRO_SapSel remLSROSap, T_SapSel *remTsap, 
			   N_SapAddr *remNsap, 
			   LROP_OperationValue operationValue, 
			   LROP_EncodingType encodingType, DU_View parameter);
extern Int 
lrop_resultCnf(InvokeInfo *invoke, LROP_UserInvokeRef userInvokeRef);
extern Int 
lrop_errorCnf(InvokeInfo *invoke, LROP_UserInvokeRef userInvokeRef);

extern Int tr_badEventIgnore(Void *machine, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker01 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker02 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker03 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker04 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker05 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker06 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker07 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker08 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker09 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clInvoker10 (Void *invoke, Void *userData, FSM_EventId evtId);

extern Int tr_clPerformer01 (Void *invoke, Void *userData, FSM_EventId evtId); 
extern Int tr_clPerformer02 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer03 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer04 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer05 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer06 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer07 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer08 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer09 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer10 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer11 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_clPerformer12 (Void *invoke, Void *userData, FSM_EventId evtId);

extern Int tr_2badEventIgnore(Void *machine, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker01 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker02 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker03 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker04 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker05 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker06 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker07 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker08 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker09 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clInvoker10 (Void *invoke, Void *userData, FSM_EventId evtId);

extern Int tr_2clPerformer01 (Void *invoke, Void *userData, FSM_EventId evtId); 
extern Int tr_2clPerformer02 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer03 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer04 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer05 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer06 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer07 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer08 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer09 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer10 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer11 (Void *invoke, Void *userData, FSM_EventId evtId);
extern Int tr_2clPerformer12 (Void *invoke, Void *userData, FSM_EventId evtId);

extern LOCAL Int lower_dataInd(T_SapSel *remTsapSel,
	      		       N_SapAddr *remNsapAddr,
	      		       T_SapSel *locTsapSel,
	      		       N_SapAddr *locNsapAddr,
	      		       DU_View data);

extern SuccFail lrop_sendFailurePdu(T_SapSel *tSapSel, N_SapAddr *naddr, 
			     	    short unsigned int invokeRefNu, 
			     	    unsigned char reason);
extern int getConf();
extern SuccFail lrop_authenticateIP(N_SapAddr *remNsap);

#else

extern SuccFail lrop_relPdu();
extern SuccFail lrop_relAllPdu();
extern DU_View lrop_pduRetrieve();
extern Void lrop_pduKeep();
extern DU_View lrop_invokePdu ();
extern DU_View lrop_resultPdu ();
extern DU_View lrop_ackPdu ();
extern Int lrop_resultInd ();
extern Int lrop_errorInd ();
extern Int lrop_failureInd ();
extern Void LROP_freeInvoke ();
extern Void lrop_relInvoke();

extern SuccFail lrop_pduSeqInit();

extern PUBLIC FSM_TransDiagram *lrop_CLInvokerTransDiag ();
extern PUBLIC FSM_TransDiagram *lrop_CLPerformerTransDiag ();
extern PUBLIC FSM_TransDiagram *lrop_2CLInvokerTransDiag ();
extern PUBLIC FSM_TransDiagram *lrop_2CLPerformerTransDiag ();
extern void tm_pduPr ();

extern Int lrop_invokeInd ();
extern Int lrop_resultCnf ();
extern Int lrop_errorCnf (); 

extern Int tr_badEventIgnore();
extern Int tr_clInvoker01 ();
extern Int tr_clInvoker02 ();
extern Int tr_clInvoker03 ();
extern Int tr_clInvoker04 ();
extern Int tr_clInvoker05 ();
extern Int tr_clInvoker06 ();
extern Int tr_clInvoker07 ();
extern Int tr_clInvoker08 ();
extern Int tr_clInvoker09 ();
extern Int tr_clInvoker10 ();

extern Int tr_badEventIgnore();
extern Int tr_clPerformer01 (); 
extern Int tr_clPerformer02 ();
extern Int tr_clPerformer03 ();
extern Int tr_clPerformer04 ();
extern Int tr_clPerformer05 ();
extern Int tr_clPerformer06 ();
extern Int tr_clPerformer07 ();
extern Int tr_clPerformer08 ();
extern Int tr_clPerformer09 ();
extern Int tr_clPerformer10 ();
extern Int tr_clPerformer11 ();
extern Int tr_clPerformer12 ();

extern Int tr_2badEventIgnore();
extern Int tr_2clInvoker01 ();
extern Int tr_2clInvoker02 ();
extern Int tr_2clInvoker03 ();
extern Int tr_2clInvoker04 ();
extern Int tr_2clInvoker05 ();
extern Int tr_2clInvoker06 ();
extern Int tr_2clInvoker07 ();
extern Int tr_2clInvoker08 ();
extern Int tr_2clInvoker09 ();
extern Int tr_2clInvoker10 ();

extern Int tr_2clPerformer01 ();
extern Int tr_2clPerformer02 ();
extern Int tr_2clPerformer03 ();
extern Int tr_2clPerformer04 ();
extern Int tr_2clPerformer05 ();
extern Int tr_2clPerformer06 ();
extern Int tr_2clPerformer07 ();
extern Int tr_2clPerformer08 ();
extern Int tr_2clPerformer09 ();
extern Int tr_2clPerformer10 ();
extern Int tr_2clPerformer11 ();
extern Int tr_2clPerformer12 ();

extern LOCAL Int lower_dataInd();
extern SuccFail lrop_sendFailurePdu();
extern int getConf();
extern SuccFail lrop_authenticateIP();

#endif
