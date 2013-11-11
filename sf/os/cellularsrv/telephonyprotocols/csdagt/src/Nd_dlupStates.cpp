// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Dial Up States
// 
//

/**
 @file Nd_dlupstates.cpp 
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "Nd_dlupStatesTraces.h"
#endif

#include "Nd_dlupStates.h"
#include <comms-infras/eventlogger.h>
#include <csdprog.h>
#include "ND_DBACC.H"


CDlUpInit* CDlUpInit::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Init State

2 phased constructor for CDlUpInit, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpInit object.
*/
	{
	CDlUpInit* r=new(ELeave) CDlUpInit(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpInit::CDlUpInit(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpInit, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpInit::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{}

CDlUpInit::~CDlUpInit()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpInit::StartState()
/**
Starts dialup init state.
If connection is reconnect and used script is not NULL, delete script.
Call JumpToRunl() with KErrNone.
*/
	{
	if (((iNdEnv->BaseEnv())->IsReconnect()) && (iNdEnv->Script()!=NULL))
			{
			iNdEnv->DeleteScript();
			}
	JumpToRunl(KErrNone);
	}

CAgentStateBase* CDlUpInit::NextStateL(TBool aContinue)
/**
Changes to from init state to next state.
If connection is not continued, create disconnect state.
Else create dialling state.

@exception Leaves if NewL() leaves.
@return a new CDlUpDisconnect or CDlUpDialling object.
*/
	{
	if(!aContinue)
		{
		return CDlUpDisconnect::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDlUpDialling::NewL(*iSMObserver,*iNdEnv);
		}
	}

void CDlUpInit::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDlUpInit::RunL()
/**
Dial up init completed.
Call CompleteState() with KErrNone.
*/
	{
	__ASSERT_DEBUG(iStatus==KErrNone, User::Invariant());
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

CDlUpDialling* CDlUpDialling::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Dialling State

2 phased constructor for CDlUpDialling, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpDialling object.
*/
	{
	CDlUpDialling* r=new(ELeave) CDlUpDialling(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpDialling::CDlUpDialling(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpDialling, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpDialling::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{}

CDlUpDialling::~CDlUpDialling()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpDialling::StartState()
/**
Starts dialup dialling state.
Call UpdateProgress() with EStartingDialling and KErrNone.
If reconnect, trap and start reconnect by calling StartReconnectL() from ETel server processor.
Else trap and start dial up by calling StartDialUpL() from ETel server processor.
If trap error, call JumpToRunl() with the trap error and return.
Else set state active.
*/
	{
	TInt err=KErrNone;
	iSMObserver->UpdateProgress(ECsdStartingDialling,KErrNone);
	// Either start reconnection or dial up
	if ((iNdEnv->BaseEnv())->IsReconnect())
		{
		TRAP(err,(iNdEnv->TelServPrc())->StartReconnectL(*this));
		}	
	else
		{
		TRAP(err,(iNdEnv->TelServPrc())->StartDialUpL(*this));
		}
	// JumpToRunl with leave error if ETel server processor calls leave.
	if(err!=KErrNone)
		{
		JumpToRunl(err);
		return;
		}
	// Else continue connection, set state active and wait for completition.
	else
		{
		iStatus=KRequestPending;
		SetActive();
		}
	}

CAgentStateBase* CDlUpDialling::NextStateL(TBool aContinue)
/**
Changes to from dialling state to next state.
If connection is not continued or iStatus is not KErrNone, create Disconnect state.
Else create Open Data Port (BCA), Scan Script (non-BCA) state.

@exception Leaves if NewL() leaves.
@return a new CDlUpDisconnect or CDlUpOpenDataPort (BCA) / CDlUpScanScript (non-BCA) object.
*/
	{ 
	if((!aContinue)||(iStatus!=KErrNone))
		{
		return CDlUpDisconnect::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
		if (iNdEnv->UseScript())
			{			
		return CDlUpOpenDataPort::NewL(*iSMObserver, *iNdEnv);
			}
		else
			{
			return CDlUpOpen::NewL(*iSMObserver,*iNdEnv);
			}			
#else
		return CDlUpOpenDataPort::NewL(*iSMObserver, *iNdEnv);

#endif
		}
	}

void CDlUpDialling::TelFunctionComplete(TInt aError)
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CDlUpDialling::DoCancel()
/**
Cancels active requests.
*/
	{
	// if a log event creation is ongoing, we want to let it go and the log event should reflect the reality
	// a call has been made and custumer will be charged for it
	//iNdEnv->Logger()->Cancel();
	if((iNdEnv->TelServPrc())!=NULL)
		(iNdEnv->TelServPrc())->Cancel();
	if (iStatus == KRequestPending)
		{
		TelFunctionComplete(KErrCancel);
		}
	}

void CDlUpDialling::RunL()
/**
Dialling completed (should not complete if connection cancelled).
If there is an error then signal it by calling ConnectionComplete() with EFinishedDialling and iStatus.Int().
Call UpdateProgress() with ECsdFinishedDialling and KErrNone.
Call PreventConnectionRetries() to stop new connections.
Call CompleteState() with KErrNone.
Dialling is always asynchronous, so should not complete if cancelled.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPDIALLING_RUNL_1, "NetDial:\tCompleted Phase 'Dialling' with Error %d" ,iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdFinishedDialling,iStatus.Int());
		return;
		}
	iSMObserver->UpdateProgress(ECsdFinishedDialling,KErrNone);
	iSMObserver->PreventConnectionRetries();
	// request asynch creation of logevent and go to next state
	TBuf<KCommsDbSvrMaxFieldLength> remoteParty;
	(iNdEnv->NetDialDb())->GetRemoteParty(remoteParty);
	TDialString telNum;
	TRAPD(ret, (iNdEnv->NetDialDb())->DoDiallingResolutionL(telNum,EForDisplay));
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	iNdEnv->GetUseScriptL();
	if (!iNdEnv->UseScript())
		{
		// We need to do this stuff now because it will not be done later in the login script setup
		RCall::TCommPort commPort;
		iNdEnv->TelServPrc()->GetCommPortL(commPort); //This updates Nifman's record and is needed by ppp
 		iSMObserver->ServiceStarted();
		}
#endif // SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT		
	// JumpToRunl if error
	if(ret==KErrNone)
		{
		// no logging if problem in finding telephone number
		iNdEnv->Logger()->LogDataAddEvent(R_LOG_CON_CONNECTED, remoteParty,R_LOG_DIR_OUT,telNum,KLogDataEventTypeUid);
		}
	// go to next state without waiting for logevent creation being completed.
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}
TInt CDlUpDialling::RunError(TInt aError)
/**
	Handles errors
*/
	{
	iSMObserver->ConnectionComplete(ECsdScannedScript,aError);
	return KErrNone;
	}

CDlUpScanScript* CDlUpScanScript::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Scan Script State

2 phased constructor for CDlUpScanScript, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpScanScript object.
*/
	{
	CDlUpScanScript* r=new(ELeave) CDlUpScanScript(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpScanScript::CDlUpScanScript(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpDialling, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpScanScript::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{}

CDlUpScanScript::~CDlUpScanScript()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpScanScript::StartState()
/**
Starts Dialup Scan Script state, scans "login" script for READ command.
Call UpdateProgress() with ECsdScanningScript and KErrNone.
Setup script by calling SetUpScriptL().
If SetUpScriptL leaves, call JumpToRunl() with leave error.
If script is used, start scannin script and set state active.
Else call JumpToRunl() with KErrNone.
*/
	{
	if (iNdEnv->UseScript())
		{
		(iNdEnv->Script())->Scan(*this);
		iStatus=KRequestPending;
		SetActive();
		}
	else
		{
		JumpToRunl(KErrNone);
		}
	}

CAgentStateBase* CDlUpScanScript::NextStateL(TBool aContinue)
/**
Changes to from Scan Script state to next state.
If connection is not continued or iStatus is not KErrNone, create Close Data Port (BCA) / Hangup (non-BCA) state.
Else create get login info state.

@exception Leaves if NewL() leaves.
@return a new CDlUpCloseDataPort (BCA) / CDlUpHangUp (non-BCA) or CDlUpGetLoginInfo object.
*/
	{ 
	if((!aContinue)||(iStatus!=KErrNone))
		{
		return CDlUpCloseDataPort::NewL(*iSMObserver,*iNdEnv, iStatus.Int());
		}
	else
		{
		return CDlUpGetLoginInfo::NewL(*iSMObserver,*iNdEnv);
		}
	}

void CDlUpScanScript::DoCancel()
/**
Cancels active requests.
*/
	{
	if (iNdEnv->Script()!=NULL)
		{
		(iNdEnv->Script())->Cancel();
		}
	if (iStatus == KRequestPending)
		{
		ScriptFunctionComplete(KErrCancel);
		}
	}

void CDlUpScanScript::RunL()
/**
Scan completed  (should not complete if connection cancelled).
If there is an error then signal it by calling ConnectionComplete() with EScannedScript and iStatus.Int().
Call UpdateProgress() with ECsdScannedScript and KErrNone.
Call CompleteState() with KErrNone.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPSCANSCRIPT_RUNL_1, "NetDial:\tCompleted Phase 'Scan Script' with Error %d", iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdScannedScript,iStatus.Int());
		return;
		}
	iSMObserver->UpdateProgress(ECsdScannedScript,KErrNone);
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

void CDlUpScanScript::ScriptFunctionComplete(TInt aError)
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

CDlUpGetLoginInfo* CDlUpGetLoginInfo::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Get Login Info State

2 phased constructor for CDlUpGetLoginInfo, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpGetLoginInfo object.
*/
	{
	CDlUpGetLoginInfo* r=new(ELeave) CDlUpGetLoginInfo(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpGetLoginInfo::CDlUpGetLoginInfo(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CGetLoginInfo(aSMObserver,aNdEnv)
/**
Private constructor for CDlUpGetLoginInfo, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpGetLoginInfo::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDlUpGetLoginInfo::~CDlUpGetLoginInfo()
/**
Destructor.

Cancel active request.
*/
	{
	}

CAgentStateBase* CDlUpGetLoginInfo::NextStateL(TBool aContinue)
/**
Changes to from get login info state to next state.

If connection is not continued or iStatus is not KErrNone, create Hangup state.
Else create login state.

@exception Leaves if NewL() leaves.
@return a new CDlUpCloseDataPort (BCA) / CDlUpHangUp (non-BCA) or CDlUpLogin object.
*/
	{ 
	if((!aContinue)||(iStatus!=KErrNone))
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPGETLOGININFO_NEXTSTATEL_1, "NetDial:\tGetLoginInfo state cancelling (aContinue %d, iStatus %d)",aContinue, iStatus.Int());

		TInt err = iStatus.Int();
		if (!aContinue && err == KErrNone)
			{
			// Ensure that CDlUpCloseDataPort receives an error in the case where aContinue==EFalse and iStatus==KErrNone.
			// If not, CDlUpCloseDataPort will carry on as if we are doing a normal startup, advance to CDlUpOpen, and cause
			// a NIFMAN panic (ConnectComplete(), state is EDisconnecting).  Note that the aContinue flag does not seem to be
			// propagated between states, so even though it may be EFalse here (indicating a connection cancellation), in
			// CDlUpCloseDataPort it seems to revert back to ETrue.
			err = KErrCancel;
			}
		return CDlUpCloseDataPort::NewL(*iSMObserver,*iNdEnv, err);
		}
	else //succesful run of state
		{
		return CDlUpLogin::NewL(*iSMObserver,*iNdEnv);
		}
	}

CDlUpLogin* CDlUpLogin::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Login State

2 phased constructor for CDlUpLogin, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpLogin object.
*/
	{
	CDlUpLogin* r=new(ELeave) CDlUpLogin(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpLogin::CDlUpLogin(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpLogin, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpLogin::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDlUpLogin::~CDlUpLogin()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpLogin::StartState()
/**
Starts dialup call login state.
Call UpdateProgress() with ECsdStartingLogIn and KErrNone.
Call DoStartStateL() and trap possible leave.
If DoStartStateL leaves, call JumpToRunl() with leave error.
*/
	{
	iSMObserver->UpdateProgress(ECsdStartingLogIn,KErrNone);
	// This call executes the actual script.
	TRAPD(ret,DoStartStateL());
	if (ret!=KErrNone)
		JumpToRunl(ret);
	}

void CDlUpLogin::DoStartStateL()
/**
If login script is valid then execute it.
If script is used, call ExecuteL() and set state active.
Else call JumpToRunl() with KErrNone.
*/
	{
	if (iNdEnv->UseScript())
		{
		(iNdEnv->Script())->ExecuteL(*this);
		iStatus=KRequestPending;
		SetActive();
		}
	else
		{
		JumpToRunl(KErrNone);
		}
	}

CAgentStateBase* CDlUpLogin::NextStateL(TBool aContinue)
/**
Changes to from login state to next state.
If connection is not continued or iStatus is not KErrNone, create hangup state.
Else create open state (non-BCA).  Create close data port state (BCA).

@exception Leaves if NewL() leaves.
@return a new CDlUpCloseDataPort (BCA), CDlUpHangUp or CDlUpOpen (non-BCA) object.
*/
	{ 
	(void)aContinue;
	return CDlUpCloseDataPort::NewL(*iSMObserver,*iNdEnv, iStatus.Int());
	}

void CDlUpLogin::ScriptFunctionComplete(TInt aError)
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CDlUpLogin::DoCancel()
/**
Cancels active requests.
*/
	{
	if(iNdEnv->Script()!=NULL)
		(iNdEnv->Script())->Cancel();
	if (iStatus == KRequestPending)
		{
		ScriptFunctionComplete(KErrCancel);
		}
	}

void CDlUpLogin::RunL()
/**
Login completed  (should not complete if connection cancelled).
If there is an error then signal it by calling ConnectionComplete() with ECsdFinishedLogIn and iStatus.Int().
Call UpdateProgress() with EScannedScript and KErrNone.
Call CompleteState() with KErrNone.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPLOGIN_RUNL_1, _ "NetDial:\tCompleted Phase 'Login' with Error %d", iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdFinishedLogIn,iStatus.Int());
		return;
		}
	
	iSMObserver->UpdateProgress(ECsdFinishedLogIn,KErrNone);
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

CDlUpOpen* CDlUpOpen::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Open State

2 phased constructor for CDlUpOpen, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpOpen object.
*/
	{
	CDlUpOpen* r=new(ELeave) CDlUpOpen(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpOpen::CDlUpOpen(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CNdCallBackStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpOpen, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpOpen::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	CNdCallBackStateBase::ConstructL();
	}

CDlUpOpen::~CDlUpOpen()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpOpen::StartState()
/**
Starts dialup open state.
Call JumpToRunl() with KErrNone.
*/
	{
	JumpToRunl(KErrNone);
	}

CAgentStateBase* CDlUpOpen::NextStateL(TBool /*aContinue*/)
/**
Changes to from open state to next state.
If call back is used, create dial in init state.
else if connection is reconnect, create dial up init state.
Else create hangup state.

@exception Leaves if NewL() leaves.
@return a new CDlInInit, CDlUpInit or CDlUpHangUp object.
*/
	{
	if((iNdEnv->BaseEnv())->CallBack())
		{
		return CDlInInit::NewL(*iSMObserver,*iNdEnv);
		}
	else if((iNdEnv->BaseEnv())->IsReconnect())
		{
		return CDlUpInit::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDlUpHangUp::NewL(*iSMObserver,*iNdEnv);
		}
	}
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
void CDlUpOpen::TelFunctionComplete(TInt aError)
	{
	iTelEvent = ETrue;
	JumpToRunl(aError);
	}
#endif

void CDlUpOpen::DoCancel()
/**
Cancels active requests.
*/
	{
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
	if(NULL != iNdEnv->TelServPrc())
		{
		iNdEnv->TelServPrc()->Cancel();			
		}	
#endif
//	(iNdEnv->BaseEnv())->CompleteState(KErrCancel);
	}

void CDlUpOpen::RunL()
/**
Open completed.
Call UpdateProgress() with ECsdConnectionOpen and KErrNone.
If script is used, close it.
Call ConnectionComplete() with ECsdConnectionOpen and KErrNone.
*/
	{
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	if (iTelEvent)
		{
		iTelEvent = EFalse;		
		iSMObserver->Notification(EAgentToNifEventTypeDisableConnection, 
			reinterpret_cast<TAny*>(KErrDisconnected));
		return;
		}	
#endif					
	__ASSERT_DEBUG(iStatus==KErrNone,User::Invariant());
	iSMObserver->UpdateProgress(ECsdConnectionOpen,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPOPEN_RUNL_1, "NetDial:\tConnection Open");
	if (iNdEnv->Script()!=NULL)
		iNdEnv->Script()->CloseScript();
	iSMObserver->ConnectionComplete(ECsdConnectionOpen,KErrNone);
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
	iNdEnv->TelServPrc()->ListenForStatusChange(*this);
#endif	
	}

CDlUpHangUp* CDlUpHangUp::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up HangUp State

2 phased constructor for CDlUpHangUp, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpHangUp object.
*/
	{
	CDlUpHangUp* r=new(ELeave) CDlUpHangUp(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpHangUp::CDlUpHangUp(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CNdCallBackStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpHangUp, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpHangUp::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	CNdCallBackStateBase::ConstructL();
	}

CDlUpHangUp::~CDlUpHangUp()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpHangUp::StartState()
/**
Starts dialup hangup state.
Call UpdateProgress() with ECsdStartingHangUp and KErrNone.
If script is used, clean up script.
If dataport is loaned, recover it by calling ReturnCommPortL() from ETel server processor.
If ReturnCommPortL leaves, call JumpToRunl() with the leave error and return.
If call is active, call StartHangUpAfterDialOut() from ETel server processor and set state active.
Else call JumpToRunl() with KErrNone.
*/
	{
	iSMObserver->UpdateProgress(ECsdStartingHangUp,KErrNone);
	
	if (iNdEnv->Script()!=NULL)
		{
		(iNdEnv->Script())->CleanupScript();
		}

	if (iNdEnv->TelServPrc()->CommPortLoaned())
		{
		// returns ownership of the comm port to ETEL
		TRAPD(err,(iNdEnv->TelServPrc())->ReturnCommPortL());
		if (err!=KErrNone)
			{
			JumpToRunl(err);
			return;
			}
		}

	if (iNdEnv->TelServPrc()->CallActive())
		{
		(iNdEnv->TelServPrc())->StartHangUpAfterDialOut(*this);
		iStatus=KRequestPending;
		SetActive();
		}
	else
		JumpToRunl(KErrNone);
	}

CAgentStateBase* CDlUpHangUp::NextStateL(TBool /*aContinue*/)
/**
Changes to from hangup state to next state.
Next State will always be Disconnect regardless of implementation results

@exception Leaves if NewL() leaves.
@return a new CDlUpDisconnect object.
*/
	{
	return CDlUpDisconnect::NewL(*iSMObserver,*iNdEnv);
	}

void CDlUpHangUp::TelFunctionComplete(TInt aError)
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CDlUpHangUp::DoCancel()
/**
Cancels active requests.
*/
	{
	(iNdEnv->TelServPrc())->Cancel();
	if (iStatus == KRequestPending)
		{
		TelFunctionComplete(KErrCancel);
		}
	}

void CDlUpHangUp::RunL()
/**
Hangup completed.
If there is an error then log it, otherwise advance to closed phase.
Else call UpdateProgress() with ECsdFinishedHangUp and KErrNone.
Call CompleteState() with iStatus.Int().
*/
	{	
	//update the log object
	iNdEnv->Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTED, KLogDataEventTypeUid);
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPHANGUP_RUNL_1,"NetDial:\tCompleted Phase 'Hang Up' with Error %d", iStatus.Int());
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPHANGUP_RUNL_2, "NetDial:\tDisconnection Error %d", iStatus.Int());
		}
	else
		{
		iSMObserver->UpdateProgress(ECsdFinishedHangUp,KErrNone);
		}
	(iNdEnv->BaseEnv())->CompleteState(iStatus.Int());
	}

CDlUpDisconnect* CDlUpDisconnect::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Disconnect State
*/
	{
	CDlUpDisconnect* r=new(ELeave) CDlUpDisconnect(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpDisconnect::CDlUpDisconnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CNdCallBackStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpDisconnect, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpDisconnect::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	CNdCallBackStateBase::ConstructL();
	}

CDlUpDisconnect::~CDlUpDisconnect()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpDisconnect::StartState()
/**
Starts dialup disconnect state.
If script is used, clean up script.
If ETel server processor is used, close all ETel call objects.
Call JumpToRunl() with KErrNone.
*/
	{
	if ((iNdEnv->Script())!=NULL)
		{
		(iNdEnv->Script())->CleanupScript();
		}
	if ((iNdEnv->TelServPrc())!=NULL)
		{
		(iNdEnv->TelServPrc())->CloseCall();
		}
	// We need to make sure that logging is finished before closing everything.
	// forward iStatus to the logger, which will be responsible to complete iStatus when it is finished.
	iStatus = KRequestPending;
	SetActive();
	iNdEnv->Logger()->LogDataNotifyLastEventUpdate(&iStatus);
	}

CAgentStateBase* CDlUpDisconnect::NextStateL(TBool /*aContinue*/)
/**
Changes to from disconnect state to next state.
Next State will always be Disconnect regardless of implementation results.

@exception Leaves if NewL() leaves.
@return a new CDlUpDisconnect object.
*/
	{
	return CDlUpDisconnect::NewL(*iSMObserver,*iNdEnv);
	}

void CDlUpDisconnect::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDlUpDisconnect::RunL()
/**
Disconnect completed.
Call DisconnectComplete().
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPDISCONNECT_RUNL_1, _"NetDial:\tDisconnect Complete");

	iSMObserver->DisconnectComplete();
	}

CDlUpOpenDataPort* CDlUpOpenDataPort::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Dial Up Open Data Port State
*/
	{
	CDlUpOpenDataPort* r=new(ELeave) CDlUpOpenDataPort(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpOpenDataPort::CDlUpOpenDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlUpOpenDataPort, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpOpenDataPort::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	}

CDlUpOpenDataPort::~CDlUpOpenDataPort()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpOpenDataPort::StartState()
/**
Starts Dial Up Open Data Port state.
Set up script and initiate creation of channel.
Call JumpToRunl() with KErrNone.
*/
	{
	ASSERT(iNdEnv);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPOPENDATAPORT_STARTSTATE_1,"NetDial:\tOpening Data Port");

	iSMObserver->UpdateProgress(ECsdScanningScript,KErrNone);
	TRAPD(ret,iNdEnv->SetUpScriptL());
	if (ret!=KErrNone)
		{
		JumpToRunl(ret);
		return;
		}

	iNdEnv->Script()->CreateChannel(iStatus);
	iStatus = KRequestPending;
	SetActive();
	}

CAgentStateBase* CDlUpOpenDataPort::NextStateL(TBool aContinue)
/**
Changes to from Open Data Port state to next state.
Next State will be Hangup on errors or connection start cancellation, else Scan Script.

@exception Leaves if NewL() leaves.
@return a new CDlUpHangUp or CDlUpOpenDataPort object.
*/
	{
	if((!aContinue)||(iStatus!=KErrNone))
		{
		return CDlUpHangUp::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDlUpScanScript::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDlUpOpenDataPort::DoCancel()
/**
Cancels active requests.
*/
	{
	iNdEnv->Script()->CancelCreateChannel();
	}

void CDlUpOpenDataPort::RunL()
/**
Dial Up Open Data Port completed.
Call CompleteState() with KErrNone.
*/
	{
	if (iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPOPENDATAPORT_RUNL_1, "NetDial:\tCompleted Phase 'Open Data Port' with Error %d", iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdScannedScript,iStatus.Int());
		return;
		}

	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

CDlUpCloseDataPort* CDlUpCloseDataPort::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError)
/**
Dial Up Close Data Port State
*/
	{
	CDlUpCloseDataPort* r=new(ELeave) CDlUpCloseDataPort(aSMObserver,aNdEnv, aError);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpCloseDataPort::CDlUpCloseDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError)
	: CAgentStateBase(aSMObserver), iNdEnv(&aNdEnv), iError(aError)
/**
Private constructor for CDlUpCloseDataPort, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlUpCloseDataPort::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	}

CDlUpCloseDataPort::~CDlUpCloseDataPort()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlUpCloseDataPort::StartState()
/**
Starts Dial Up Close Data Port state.
Initiate shutdown of data port
*/
	{
	ASSERT(iNdEnv);
	ASSERT (iNdEnv->Script());

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPCLOSEDATAPORT_STARTSTATE_1, "NetDial:\tClosing Data Port");

	iNdEnv->Script()->ShutdownChannel(iStatus);
	iStatus = KRequestPending;
	SetActive();
	}

CAgentStateBase* CDlUpCloseDataPort::NextStateL(TBool aContinue)
/**
Changes to from Close Data Port state to next state.

@exception Leaves if NewL() leaves.
@return a new CDlUpCloseDataPort object.
*/
	{
	if((!aContinue) || (iError != KErrNone) || (iStatus.Int() != KErrNone))
		{
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPCLOSEDATAPORT_NEXTSTATEL_1, "NetDial:\tCloseDataPort state cancelling (iError %d, aContinue %d, iStatus %d)", iError, aContinue, iStatus.Int());
		return CDlUpHangUp::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDlUpOpen::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDlUpCloseDataPort::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDlUpCloseDataPort::RunL()
/**
Close Data Port completed.
Call CompleteState() with KErrNone.
*/
	{
	// Note: we assume that if iError != KErrNone then ConnectionComplete() has already been called by
	// the failing state (otherwise NIFMAN will panic due to the lack of a ConnectComplete() in response
	// to its earlier Connect()).
	if (iStatus!=KErrNone)
		{

		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPCLOSEDATAPORT_RUNL_1, "NetDial:\tCompleted Phase 'Close Data Port' with Error %d", iStatus.Int());
		if (iError != KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPCLOSEDATAPORT_RUNL_2, "Saved error = %d", iError);
			}
		else
			{
			// Only do the ConnectionComplete() due to errors in this state if a previous state has not already done
			// so (i.e. iError == KErrNone) otherwise NIFMAN will panic.
			iSMObserver->ConnectionComplete(iStatus.Int());
			return;
			}
		}
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

