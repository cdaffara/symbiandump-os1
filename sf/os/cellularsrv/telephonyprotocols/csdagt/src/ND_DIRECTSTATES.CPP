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
// Direct Connection States
// 
//

/**
 @file Nd_directstates.cpp 
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ND_DIRECTSTATESTraces.h"
#endif

#include "ND_DIRCT.H"
#include "ND_SCR.H"
#include "ND_DBACC.H"
#include "ND_STD.H"
#include "ND_DIRECTSTATES.H"

CDirCtInit* CDirCtInit::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Initial State
2 phased constructor for CDirCtInit, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtInit* r=new(ELeave) CDirCtInit(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtInit::CDirCtInit(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtInit, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtInit::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtInit::~CDirCtInit()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtInit::StartState()
/**
Starts direct connection init state.
If connection is reconnect and used script is not NULL, reconfigure and cancel COMM-port and delete script.
Call JumpToRunl with KErrNone.
*/
	{
	JumpToRunl(KErrNone);
	}

CAgentStateBase* CDirCtInit::NextStateL(TBool aContinue)
/**
Changes to from init state to next state.
If connection is not continued, create disconnect state.
Else create scan script state.

@exception Leaves if NewL() leaves.
@return a new CDirCtDisconnect or CDirCtScanScript object.
*/
	{
	if(!aContinue)
		{
		return CDirCtDisconnect::NewL(*iSMObserver,*iNdEnv);
		}
	else  //Usual SM route. If no connection cancel next state scans the script
		{
		if (((iNdEnv->BaseEnv())->IsReconnect()) && (iNdEnv->Script()!=NULL))
			{
			return CDirCtReConfigureCommPort::NewL(*iSMObserver,*iNdEnv,ETrue);
			}
		else
			{
			return CDirCtOpenDataPort::NewL(*iSMObserver,*iNdEnv);
			}
		}
	}

void CDirCtInit::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDirCtInit::RunL()
/**
Direct connection init completed.
Call CompleteState() with KErrNone.
*/
	{
	__ASSERT_DEBUG(iStatus==KErrNone, User::Invariant());	// should just have come from JumpToRunl(KErrNone)
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

CDirCtScanScript* CDirCtScanScript::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Scan Script State

2 phased constructor for CDirCtScanScript, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtScanScript object.
*/
	{
	CDirCtScanScript* r=new(ELeave) CDirCtScanScript(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtScanScript::CDirCtScanScript(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv) 
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtScanScript, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtScanScript::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtScanScript::~CDirCtScanScript()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtScanScript::StartState()
/**
Scan "login" script for READ command.
Call UpdateProgress() with EScanningScript and KErrNone.
Call SetUpScriptL() to setup the script. If call leaves, trap it
and call JumpToRunl().
If script should be used, start scanning script and set state active.
Else call JumpToRunl().
*/
	{
	iSMObserver->UpdateProgress(ECsdScanningScript,KErrNone);

	if (iNdEnv->UseScript())
		{
		iStatus=KRequestPending;
		SetActive();
		iNdEnv->Script()->Scan(*this);
		}
	else
		{
		JumpToRunl(KErrNone);
		}
	}

CAgentStateBase* CDirCtScanScript::NextStateL(TBool aContinue)
/**
Changes to from scan script state to next state.
If connection is not continued or iStatus is not KErrNone, create disconnect state.
Else create get login info state.

@param aContinue indicates whether connection should continue.
@exception Leaves if NewL() leaves.
@return a new CDirCtDisconnect or CDirCtGetLoginInfo object.
*/
	{ 
	if((!aContinue)||(iStatus!=KErrNone))
		{
		return CDirCtCloseDataPort::NewL(*iSMObserver,*iNdEnv,iStatus.Int());
		}
	else //succesful run of state
		{
		return CDirCtGetLoginInfo::NewL(*iSMObserver,*iNdEnv);
		}
	}

void CDirCtScanScript::DoCancel()
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

void CDirCtScanScript::RunL()
/**
Scan completed (should not complete if connection cancelled).
If there is an error then signal it, otherwise advance the phase to connect.
Call ServiceStarted() if not a reconnect.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTSCANSCRIPT_RUNL_1,"NetDial:\tCompleted Phase Scan Script with Error %d", iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdScannedScript,iStatus.Int());
		return;
		}

	iSMObserver->UpdateProgress(ECsdScannedScript,iStatus.Int());
	if (!(iNdEnv->BaseEnv())->IsReconnect())
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTSCANSCRIPT_RUNL_2, "NetDial:\tService Started");
		}
	else
		{			
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTSCANSCRIPT_RUNL_3,"NetDial:\tService Started On Reconnection");
		}
	//iSMObserver->ServiceStarted();
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

void CDirCtScanScript::ScriptFunctionComplete(TInt aError)
/**
Script complete. Complete request.
*/
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

CDirCtGetLoginInfo* CDirCtGetLoginInfo::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Get Login Info State

2 phased constructor for CDirCtGetLoginInfo, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtGetLoginInfo* r=new(ELeave) CDirCtGetLoginInfo(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtGetLoginInfo::CDirCtGetLoginInfo(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CGetLoginInfo(aSMObserver,aNdEnv)
/**
Private constructor for CDirCtGetLoginInfo, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtGetLoginInfo::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtGetLoginInfo::~CDirCtGetLoginInfo()
/**
Destructor.
*/
	{
	}

CAgentStateBase* CDirCtGetLoginInfo::NextStateL(TBool aContinue)
/**
Changes to from get login info state to next state.
If connection is not continued or iStatus is not KErrNone, create hangup state.
Else create connect state.

@param aContinue indicates whether connection should continue.
@exception Leaves if NewL() leaves.
@return a new CDirCtHangUp or CDirCtConnect object.
*/
	{ 
	if((!aContinue)||(iStatus!=KErrNone))
		{
		return CDirCtHangUp::NewL(*iSMObserver,*iNdEnv);
		}
	else //succesful run of state
		{
		return CDirCtConfigureCommPort::NewL(*iSMObserver,*iNdEnv);
		}
	}

CDirCtConnect* CDirCtConnect::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Connect State

2 phased constructor for CDirCtConnect, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtConnect* r=new(ELeave) CDirCtConnect(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtConnect::CDirCtConnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv) 
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtConnect, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtConnect::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtConnect::~CDirCtConnect()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtConnect::StartState()
/**
Starts direct connection connect state.
Call UpdateProgress() with ECsdStartingConnect and KErrNone.
Configure comm port and execute script.
*/
	{
	TRAPD(ret,DoStartStateL());
	if (ret!=KErrNone)
		JumpToRunl(ret);
	}

CAgentStateBase* CDirCtConnect::NextStateL(TBool aContinue)
/**
Changes to from connect state to next state.
If connection is not continued or iStatus is not KErrNone, create hangup state.
Else create open state.

@param aContinue indicates whether connection should continue.
@exception Leaves if NewL() leaves.
@return a new CDirCtHangUp or CDirCtOpen object.
*/
	{
	if((!aContinue)||(iStatus!=KErrNone))
		{
		return CDirCtHangUp::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDirCtOpen::NewL(*iSMObserver,*iNdEnv);
		}
	}

void CDirCtConnect::DoCancel()
/**
Cancels active requests and calls ScriptFunctionComplete() with KErrCancel.
*/
	{
	if (iNdEnv->Script()!=NULL)
		{
		(iNdEnv->Script())->Cancel();
		}
	if (!IsActive())
		SetActive();
	ScriptFunctionComplete(KErrCancel);
	}

void CDirCtConnect::RunL()
/**
Connect completed (should not complete if connection cancelled).
If there is an error then signal it.
Call UpdateProgress() with ECsdFinishedConnect and KErrNone.
Call PreventConnectionRetries().
Complete connect state with KErrNone.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCONNECT_RUNL_1, "NetDial:\tCompleted Phase Execute Script with Error %d", iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdFinishedConnect,iStatus.Int());
		return;
		}
	
	iSMObserver->UpdateProgress(ECsdFinishedConnect,KErrNone);
	iSMObserver->PreventConnectionRetries();
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

void CDirCtConnect::ScriptFunctionComplete(TInt aError)
/**
Completes direct connect state with aError parameter.

@param aError is error code to be used with request completition.
*/
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CDirCtConnect::DoStartStateL()
/**
Executes script if used.  If script is not used, calls JumpToRunl() with KErrNone.

@exception Leaves if ExecuteL() leaves.
*/
	{
	if (iNdEnv->UseScript())
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCONNECT_DOSTARTSTATEL_1, "NetDial:\tExecuting Script");

		(iNdEnv->Script())->ExecuteL(*this);
		iStatus=KRequestPending;
		SetActive();
		}
	else
		{
		JumpToRunl(KErrNone);
		}
	}

CDirCtOpen* CDirCtOpen::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Open State

2 phased constructor for CDirCtOpen, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtOpen* r=new(ELeave) CDirCtOpen(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtOpen::CDirCtOpen(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv) 
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtOpen, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtOpen::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtOpen::~CDirCtOpen()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtOpen::StartState()
/**
Starts direct connection open state.
Calls JumpToRunl. When connection completed, tell NifMan.
*/
	{
	JumpToRunl(KErrNone);
	}

CAgentStateBase* CDirCtOpen::NextStateL(TBool /*aContinue*/)
/**
Jumps to direct init or hangup state.
If there should be reconnection, create init state.
Else create hangup state.

@param aContinue not used.
@exception Leaves if NewL() leaves.
@return a new CDirCtInit or CDirCtHangUp object.
*/
	{
	if((iNdEnv->BaseEnv())->IsReconnect())
		{
		return CDirCtInit::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDirCtHangUp::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDirCtOpen::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDirCtOpen::RunL()
/**
Direct connection open completed.
Call UpdateProgress() with ECsdConnectionOpen and KErrNone.
If script is used, close script.
*/
	{
	__ASSERT_DEBUG(iStatus==KErrNone,User::Invariant());
	iSMObserver->UpdateProgress(ECsdConnectionOpen,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTOPEN_RUNL_1, "NetDial:\tConnection Open");
	if (iNdEnv->Script()!=NULL)
		iNdEnv->Script()->CloseScript();
	iSMObserver->ConnectionComplete(ECsdConnectionOpen,KErrNone);
	}

CDirCtHangUp* CDirCtHangUp::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection HangUp State

2 phased constructor for CDirCtHangUp, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtHangUp* r=new(ELeave) CDirCtHangUp(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}
 
CDirCtHangUp::CDirCtHangUp(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtHangUp, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

CDirCtHangUp::~CDirCtHangUp()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtHangUp::ConstructL()
/**
Instantiate Member variable.
*/
	{}

void CDirCtHangUp::StartState()
/**
Starts direct connection hangup state.
Call UpdateProgress() with ECsdStartingHangUp and KErrNone.
Hang-up - just drop DTR.
Calls JumpToRunl.
Only need this to be asynchronous for PPP else we could do a synchronous disconnect.
*/
	{
	iSMObserver->UpdateProgress(ECsdStartingHangUp,KErrNone);
	iStatus = KRequestPending;
	SetActive();
	(iNdEnv->Script())->DropDTR(&iStatus);
	}

CAgentStateBase* CDirCtHangUp::NextStateL(TBool /*aContinue*/)
/**
Next state from hangup will always be CDirCtDisconnect

@exception Leaves if NewL() leaves.
@return a new CDirCtDisconnect object.
*/
	{
	return CDirCtReConfigureCommPort::NewL(*iSMObserver,*iNdEnv,EFalse);
	}

void CDirCtHangUp::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDirCtHangUp::RunL()
/**
Direct connection hangup completed.
Call UpdateProgress() with ECsdFinishedHangUp and KErrNone.
Complete hangup state.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTHANGUP_RUNL_1,"NetDial:\tCompleted Phase Hang Up with Error %d", iStatus.Int());
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTHANGUP_RUNL_2, "NetDial:\tDisconnection Error %d", iStatus.Int());
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTHANGUP_RUNL_3,"Hang Up");
	iSMObserver->UpdateProgress(ECsdFinishedHangUp,iStatus.Int());
	(iNdEnv->BaseEnv())->CompleteState(iStatus.Int());
	}

CDirCtDisconnect* CDirCtDisconnect::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Disconnect State

2 phased constructor for CDirCtDisconnect, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtDisconnect* r=new(ELeave) CDirCtDisconnect(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtDisconnect::CDirCtDisconnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv) 
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtDisconnect, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

CDirCtDisconnect::~CDirCtDisconnect()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtDisconnect::ConstructL()
/**
Instantiate Member variable.
*/
	{}

void CDirCtDisconnect::StartState()
/**
Starts direct connection disconnect state.
If script is used, clean up script.
Calls JumpToRunl().
*/
	{
	if ((iNdEnv->Script())!=NULL)
		(iNdEnv->Script())->CleanupScript();
	JumpToRunl(KErrNone);
	}

CAgentStateBase* CDirCtDisconnect::NextStateL(TBool /*aContinue*/)
/**
Returns the same disconnect state.

@exception Leaves if NewL() leaves.
@return a new CDirCtDisconnect object.
*/
	{
	return CDirCtDisconnect::NewL(*iSMObserver,*iNdEnv);
	}

void CDirCtDisconnect::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDirCtDisconnect::RunL()
/**
Direct connection disconnect completed.
If script is used, call ReConfigureAndCancelCommPort().
Call DisconnectComplete().
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTDISCONNECT_RUNL_1, "NetDial:\tDisconnect Complete");

	iSMObserver->DisconnectComplete();
	}

CDirCtOpenDataPort* CDirCtOpenDataPort::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Open Data Port State

@exception Leaves if NewL() leaves.
@return a new CDirCtOpenDataPort object.
*/
	{
	CDirCtOpenDataPort* r=new(ELeave) CDirCtOpenDataPort(aSMObserver,aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtOpenDataPort::CDirCtOpenDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtOpenDataPort, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtOpenDataPort::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	}

CDirCtOpenDataPort::~CDirCtOpenDataPort()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtOpenDataPort::StartState()
/**
Starts direct connection open data port state.
If script is used, clean up script.
Initiate creation of data port channel.
Call JumpToRunl() with KErrNone.
*/
	{
	ASSERT(iNdEnv);
	TRAPD(ret,iNdEnv->SetUpScriptL());
	if (ret!=KErrNone)
		{
		JumpToRunl(ret);
		return;
		}

	iStatus = KRequestPending;
	SetActive();
	iNdEnv->Script()->CreateChannel(iStatus);
	}

CAgentStateBase* CDirCtOpenDataPort::NextStateL(TBool aContinue)
/**
Changes to to next state.  Next state depends on whether open data
port was successful and whether connection start has been cancelled
or not (aContinue).
*/
	{
	if (iStatus != KErrNone)
		{
		// Open unsuccessful
		return CDirCtDisconnect::NewL(*iSMObserver,*iNdEnv);
		}
	else
	if (!aContinue)
		{
		// Open successful, but connect shouldn't continue.
		return CDirCtCloseDataPort::NewL(*iSMObserver,*iNdEnv, KErrNone);
		}
	else
		{
		// Open successful, connect should continue
		return CDirCtScanScript::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDirCtOpenDataPort::DoCancel()
/**
Cancels active requests.
*/
	{
	iNdEnv->Script()->CancelCreateChannel();
	}

void CDirCtOpenDataPort::RunL()
/**
Open data port completed.
Call CompleteState() with any error.
*/
	{
	(iNdEnv->BaseEnv())->CompleteState(iStatus.Int());
	}

//
// CDirCtConfigureCommPort
//

CDirCtConfigureCommPort* CDirCtConfigureCommPort::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Direct Connection Configure Comm Port State

2 phased constructor for CDirCtConfigureCommPort, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtInit object.
*/
	{
	CDirCtConfigureCommPort* r=new(ELeave) CDirCtConfigureCommPort(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtConfigureCommPort::CDirCtConfigureCommPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv) 
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDirCtConfigureCommPort, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtConfigureCommPort::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtConfigureCommPort::~CDirCtConfigureCommPort()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtConfigureCommPort::StartState()
/**
Starts direct connection configure comm port state.
Call UpdateProgress() with ECsdStartingConnect and KErrNone.
Initiate asynchronous configuration of data port.
*/
	{
	iSMObserver->UpdateProgress(ECsdStartingConnect,KErrNone);
	TRAPD(ret,DoStartStateL());
	if (ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCONFIGURECOMMPORT_STARTSTATE_1, "NetDial:\tError %d Configuring Comm Port", ret);
		JumpToRunl(ret);
		}
	}

CAgentStateBase* CDirCtConfigureCommPort::NextStateL(TBool aContinue)
/**
Changes to from configure comm port state to next state.
If connection is not continued or iStatus is not KErrNone, create hangup state.
Else create connect state.

@param aContinue indicates whether connection should continue.
@exception Leaves if NewL() leaves.
@return a new CDirCtHangUp or CDirCtConnect object.
*/
	{
	if((!aContinue) || (iStatus!=KErrNone))
		{
		return CDirCtHangUp::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		return CDirCtConnect::NewL(*iSMObserver,*iNdEnv);
		}
	}

void CDirCtConfigureCommPort::DoCancel()
/**
Cancels active requests
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCONFIGURECOMMPORT_DOCANCEL_1,"NetDial:\tCancel Configure Comm Port");


	(iNdEnv->Script())->CancelConfigureCommPort();
	}

void CDirCtConfigureCommPort::RunL()
/**
Configure comm port completed (should not complete if connection cancelled).
If there is an error then close script and signal it.
Complete configure comm port state with KErrNone.
*/
	{
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCONFIGURECOMMPORT_RUNL_1, "NetDial:\tCompleted Phase Configuring Comm Port with Error %d", iStatus.Int());
		if (iNdEnv->Script()!=NULL)
			iNdEnv->Script()->CloseScript();
		iSMObserver->ConnectionComplete(ECsdFinishedConnect,iStatus.Int());
		return;
		}
	
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

void CDirCtConfigureCommPort::DoStartStateL()
/**
Retrieves Comm Port configuration parameters from CommsDat and initiates configuration.

@exception Leaves if GetCommConfigForDirectConnectL() leaves.
*/
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCONFIGURECOMMPORT_DOSTARTSTATEL_1,"NetDial:\tConfiguring Comm Port");

	(iNdEnv->NetDialDb())->GetCommConfigForDirectConnectL(iConfigPckg);

	iStatus = KRequestPending;
	SetActive();
	(iNdEnv->Script())->ConfigureCommPort(iStatus, iConfigPckg);
	}

//
// CDirCtReConfigureCommPort
//

CDirCtReConfigureCommPort* CDirCtReConfigureCommPort::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TBool aReConnect)
/**
Direct Connection ReConfigure Comm Port State
2 phased constructor for CDirCtReConfigureCommPort, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtReConfigureCommPort object.
*/
	{
	CDirCtReConfigureCommPort* r=new(ELeave) CDirCtReConfigureCommPort(aSMObserver, aNdEnv, aReConnect);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtReConfigureCommPort::CDirCtReConfigureCommPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TBool aReConnect)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv),iReConnect(aReConnect)
/**
Private constructor for CDirCtReConfigureCommPort, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtReConfigureCommPort::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDirCtReConfigureCommPort::~CDirCtReConfigureCommPort()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtReConfigureCommPort::StartState()
/**
Starts direct connection reconfigure comm port state.
Reconfigure port to allow signals to be dropped.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTRECONFIGURECOMMPORT_STARTSTATE_1, "NetDial:\tReconfiguring Comm Port");
	iStatus = KRequestPending;
	SetActive();
	// Reconfigure the Comm Port to allow the signals to be set.
	(iNdEnv->Script())->ReConfigureAndCancelCommPort(iStatus);
	}

CAgentStateBase* CDirCtReConfigureCommPort::NextStateL(TBool aContinue)
/**
Changes to from reconfigure comm port state to next state.
If not a reconnection, create close data port state.
Else create disconnect or open data port state depending on whether
connection startup has been cancelled or not.

@exception Leaves if NewL() leaves.
@return a new CDirCtDisconnect, CDirCtOpenDataPort or CDirCtCloseDataPort object.
*/
	{
	if (iReConnect)
		{
		if(!aContinue)
			{
			return CDirCtDisconnect::NewL(*iSMObserver,*iNdEnv);
			}
		else
			{
			return CDirCtOpenDataPort::NewL(*iSMObserver,*iNdEnv);
			}
		}
	else
		{
		// Don't treat errors in this state as fatal.  KErrCompletion is a (simple but hacky) way to prevent
		// CDirCtCloseDataPort from issuing a ConnectionComplete() if it fails (we have already issued
		// a ConnectionComplete(), and a second one will cause a NIFMAN panic).
		return CDirCtCloseDataPort::NewL(*iSMObserver,*iNdEnv,KErrCompletion);
		}
	}

void CDirCtReConfigureCommPort::DoCancel()
/**
Cancels active requests.
*/
	{
	// Used to cancel both ReConfigureAndCancelCommPort() and DropSignals()
	if (iNdEnv->Script())
		{
		(iNdEnv->Script())->CancelConfigureCommPort();
		}
	}

void CDirCtReConfigureCommPort::RunL()
/**
Direct connection reconfigure comm port completed.
Call CompleteState() with KErrNone.
*/
	{
	// Note: we aren't acting on errors here as it may be worth continuing even if we can't configure the
	// Comm Port or drop the control signals.  The configuration here is only to allow the control signals
	// to drop, not to set up the final Comm Port properly.  The final Comm Port configuration will take
	// place in CDirCtConfigureCommPort.
	if (!iDropping)
		{
		// ReConfigureAndCancelCommPort() has completed to allow us to drop the Comm Port signals.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTRECONFIGURECOMMPORT_RUNL_1, "NetDial:\tDropping Signals");

		iDropping = ETrue;
		iStatus = KRequestPending;
		SetActive();
		(iNdEnv->Script())->DropSignals(iStatus);
		return;			// don't advance to the next state before when DropSignals() completes
		}
	else
		{
		// DropSignals() has completed, so delete the script (for reconnects) and move to the next state.
		if (iReConnect)
			{
			iNdEnv->DeleteScript();
			}
		}
	(iNdEnv->BaseEnv())->CompleteState(KErrNone);
	}

//
// CDirCtCloseDataPort
//

CDirCtCloseDataPort* CDirCtCloseDataPort::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError)
/**
Direct Connection Close Data Port State
*/
	{
	CDirCtCloseDataPort* r=new(ELeave) CDirCtCloseDataPort(aSMObserver,aNdEnv, aError);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtCloseDataPort::CDirCtCloseDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv),iError(aError)
/**
Private constructor for CDirCtCloseDataPort, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDirCtCloseDataPort::ConstructL()
/**
Instantiate Member variable.
Call base class CNdCallBackStateBase::ConstructL().

@exception Leaves if CNdCallBackStateBase::ConstructL() leaves, or not enough memory is available.
*/
	{
	}

CDirCtCloseDataPort::~CDirCtCloseDataPort()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDirCtCloseDataPort::StartState()
/**
Starts direct connection close data port state.
Initiate shutdown of data port.
Call JumpToRunl() with KErrNone.
*/
	{
	ASSERT(iNdEnv);
	ASSERT (iNdEnv->Script());
	iStatus = KRequestPending;
	SetActive();
	iNdEnv->Script()->ShutdownChannel(iStatus);
	}

CAgentStateBase* CDirCtCloseDataPort::NextStateL(TBool /*aContinue*/)
/**
Changes to from close data port state to next state.
Next State will always be Disconnect regardless of implementation results.

@exception Leaves if NewL() leaves.
@return a new CDirCtCloseDataPort object.
*/
	{
	return CDirCtDisconnect::NewL(*iSMObserver,*iNdEnv);
	}

void CDirCtCloseDataPort::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDirCtCloseDataPort::RunL()
/**
Direct Connection close data port completed.
Call CompleteState() with KErrNone.
*/
	{
	// Note: we assume that if iError != KErrNone then ConnectionComplete(error) or CompleteState(error) has
	// already been called by the failing state (otherwise NIFMAN will panic due to the lack of a ConnectComplete()
	// in response to its earlier Connect()).  Consequently, we can't call either method with an error argument
	// for a second time (otherwise NIFMAN will panic due to too many ConnectComplete() responses).
	if (iStatus!=KErrNone)
	    {
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCLOSEDATAPORT_RUNL_1,  "NetDial:\tCompleted Phase Close Data Port with Error %d", iStatus.Int());
		if (iError != KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDIRCTCLOSEDATAPORT_RUNL_2,"Saved error = %d", iError);
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

