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
// Dial In States
// 
//

/**
 @file Nd_dlinstates.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "Nd_dlinStatesTraces.h"
#endif

#include "ND_DLIN.H"
#include "Nd_dlinStates.h"
#include <comms-infras/eventlogger.h>
#include <csdprog.h>
#include "ND_DBACC.H"
#include "Nd_CallBack.h"

const TInt KDefaultCallBackTimeMicroSec=90000000;		//< 1.5 minutes

CDlInInit* CDlInInit::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Initial State Definition

2 phased constructor for CDlInInit, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlInInit object.
*/
	{
	CDlInInit* r=new(ELeave) CDlInInit(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlInInit::~CDlInInit()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

CDlInInit::CDlInInit(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlInInit, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlInInit::ConstructL()
/**
Instantiate Member variable.
*/
	{}

void CDlInInit::StartState()
/**
Starts dial in init state.
Call JumpToRunl() with KErrNone.
*/
	{
	JumpToRunl(KErrNone);
	}

CAgentStateBase* CDlInInit::NextStateL(TBool aContinue)
/**
Changes to from init state to next state.
If connection is not continued and iStatus is not KErrNone, create disconnect state.
Else create answering state.

@exception Leaves if NewL() leaves.
@return a new CDlInDisconnect or CDlInAnswering object.
*/
	{
	if(!aContinue||(iStatus!=KErrNone))
		{
		return CDlInDisconnect::NewL(*iSMObserver, *iNdEnv);
		}
	else
		{
		return CDlInAnswering::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDlInInit::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDlInInit::RunL()
/**
Dial in init completed.
Call CompleteState() with iStatus.Int().
*/
	{
	__ASSERT_DEBUG(iStatus==KErrNone, User::Invariant());
	(iNdEnv->BaseEnv())->CompleteState(iStatus.Int());
	}

CDlInAnswering* CDlInAnswering::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Answering State Definition

2 phased constructor for CDlInAnswering, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlInAnswering object.
*/
	{
	CDlInAnswering* r=new(ELeave) CDlInAnswering(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlInAnswering::~CDlInAnswering()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

CDlInAnswering::CDlInAnswering(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CNdCallBackStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlInAnswering, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlInAnswering::ConstructL()
/**
Instantiate Member variable.
Call CNdCallBackStateBase::ConstructL().
*/
	{
	CNdCallBackStateBase::ConstructL();
	}

void CDlInAnswering::StartState()
/**
Starts dial in answering state.
Call UpdateProgress() with ECsdStartingAnswer and KErrNone.
If call back is used, get call back timeout and start call back timer.
Call WaitForIncomingCallL() from ETel server processor.
If WaitForIncomingCallL() leaves, trap it and call JumpToRunl() with the leave error.
Else set state active.
*/
	{
	iSMObserver->UpdateProgress(ECsdStartingAnswer,KErrNone);

	if ((iNdEnv->BaseEnv())->CallBack())
		{
		TUint32 temp;
		TInt ret=(iNdEnv->NetDialDb())->GetCallbackTimeout(temp);
		TTimeIntervalMicroSeconds32 timer;
		if (ret!=KErrNone)
			timer=KDefaultCallBackTimeMicroSec;
		else
			timer=temp;
		iCallBackTimer->Start(timer);
		}

	TRAPD(err,(iNdEnv->TelServPrc())->WaitForIncomingCallL(*this));
	if(err!=KErrNone)
		{
		JumpToRunl(err);
		return;
		}
	else
		{
		iStatus=KRequestPending;
		SetActive();
		}
	}

CAgentStateBase* CDlInAnswering::NextStateL(TBool aContinue)
/**
Changes to from answering state to next state.
If connection is not continued and iStatus is not KErrNone, create hangup state.
Else if connection is not accepted, create answering state.
Else create log call start state.

@exception Leaves if NewL() leaves.
@return a new CDlInHangUp, CDlInAnswering or CDlInLogCallStart object.
*/
	{
	if(!aContinue||(iStatus!=KErrNone))
		{
		return CDlInHangUp::NewL(*iSMObserver, *iNdEnv);
		}
	else if (!iConnectionAccepted)
		{
		return CDlInAnswering::NewL(*iSMObserver, *iNdEnv);
		}
	else //succesful run of state
		{
		TBuf<KCommsDbSvrMaxFieldLength> remoteParty;
		iNdEnv->NetDialDb()->GetRemoteParty(remoteParty);
		iNdEnv->Logger()->LogDataAddEvent(R_LOG_CON_CONNECTED, remoteParty,R_LOG_DIR_IN,KNullDesC, KLogDataEventTypeUid);
		return CDlInOpen::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDlInAnswering::TelFunctionComplete(TInt aError)
/**
Dial in answer TelFunctionComplete.
Complete state with aError.

@param aError identifies the error code for completition.
*/
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CDlInAnswering::RunL()
/**
Dial in answer completed (should not complete if connection cancelled).
If there is an error then signal it, otherwise advance phase.
Answer is always asynchronous, so should never complete when cancelled
*/
	{
	if ((NULL != iNdEnv->TelServPrc()) && (iStatus==KErrTimedOut))	// timer went off before call received
		(iNdEnv->TelServPrc())->Cancel();
	if (iCallBackTimer!=NULL)
		iCallBackTimer->Cancel();
	
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLINANSWERING_RUNL_1, "NetDial:\tCompleted Phase Answering with Error %d", iStatus.Int());
		}
	else
		{
		TInt err=KErrNone;
		iConnectionAccepted=ETrue;
		if (!(iNdEnv->BaseEnv())->CallBack())
			err=iSMObserver->IncomingConnectionReceived();		
		if (err==KErrNone)
			{
			iSMObserver->UpdateProgress(ECsdAnswered,KErrNone);
			iSMObserver->ServiceStarted();
			}
		else
			{
			iConnectionAccepted=EFalse;
			}
		}
	(iNdEnv->BaseEnv())->CompleteState(iStatus.Int());
	}

void CDlInAnswering::DoCancel()
/**
Cancels active requests.
*/
	{	

	if ((iNdEnv->TelServPrc())!=NULL)
		(iNdEnv->TelServPrc())->Cancel();
	iCallBackTimer->Cancel();
	if(iStatus==KRequestPending)
		{
		TelFunctionComplete(KErrCancel);
		}
	}

CDlInOpen* CDlInOpen::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Open State Definition

2 phased constructor for CDlInOpen, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlInOpen object.
*/
	{
	CDlInOpen* r=new(ELeave) CDlInOpen(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlInOpen::CDlInOpen(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlInOpen, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

CDlInOpen::~CDlInOpen()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlInOpen::ConstructL()
/**
Instantiate Member variable.
*/
	{}

void CDlInOpen::StartState()
/**
Starts dial in open state.
Request COMM-port from ETEL, trap possible leave.
Call JumpToRunl with error code from requesting COMM-port.
*/
	{
	RCall::TCommPort commPort;
	TRAPD(err,(iNdEnv->TelServPrc())->GetCommPortL(commPort));
	JumpToRunl(err);
	}

CAgentStateBase* CDlInOpen::NextStateL(TBool aContinue)
/**
Changes to from log call start state to next state.
HangUp will be the next state whether or not a client has requested a connection cancel
unless connection is lost on a callback connection. In which case a reconnection
must be attempted, therefore return to dial up SM and redial.
If connection is continued, create dialup init state.
Else create hangup state.

@exception Leaves if NewL() leaves.
@return a new CDlUpInit or CDlInHangUp object.
*/
	{
	if(aContinue)
		{
		// Redial the server to get it to do callback again
		return CDlUpInit::NewL(*iSMObserver,*iNdEnv);
		}
	else
		{
		// HangUp will be the next state whether or not a client has requested a connection cancel
		return CDlInHangUp::NewL(*iSMObserver, *iNdEnv);
		}
	}

void CDlInOpen::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDlInOpen::RunL()
/**
Dial in open start completed.
Call ConnectionComplete() with EConnectionOpen and iStatus.Int().
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLINOPEN_RUNL_1, "NetDial:\tConnection Open");
	iSMObserver->ConnectionComplete(ECsdConnectionOpen,iStatus.Int());
	}

CDlInDisconnect* CDlInDisconnect::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Disconnect State Definition

2 phased constructor for CDlInDisconnect, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlInDisconnect object.
*/
	{
	CDlInDisconnect* r=new(ELeave) CDlInDisconnect(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlInDisconnect::CDlInDisconnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlInDisconnect, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlInDisconnect::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDlInDisconnect::~CDlInDisconnect()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlInDisconnect::StartState()
/**
Starts dial in disconnect state.
If ETel server processor is used, close it and possible opened objects.
Call JumpToRunl with KErrNone.
*/
	{
	if ((iNdEnv->TelServPrc())!=NULL)
		(iNdEnv->TelServPrc())->CloseCall();
	
	// We need to make sure that logging is finished before closing everything.
	// forward iStatus to the logger, which will be responsible to complete iStatus when it is finished.
	iStatus = KRequestPending;
	iNdEnv->Logger()->LogDataNotifyLastEventUpdate(&iStatus);
	SetActive();
	}

CAgentStateBase* CDlInDisconnect::NextStateL(TBool /*aContinue*/)
/**
Returns the same disconnect state.

@exception Leaves if NewL() leaves.
@return a new CDlInDisconnect object.
*/
	{
	return CDlInDisconnect::NewL(*iSMObserver, *iNdEnv);
	}

void CDlInDisconnect::DoCancel()
/**
Cancels active requests.
*/
	{
	}

void CDlInDisconnect::RunL()
/**
Dial in disconnect completed.
Call DisconnectComplete().
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLINDISCONNECT_RUNL_1, "NetDial:\tDisconnect Complete");
	
	iSMObserver->DisconnectComplete();
	}

CDlInHangUp* CDlInHangUp::NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
/**
Hangup State Definition

2 phased constructor for CDlInHangUp, first phase.

@param aSMObserver a reference to state machine observer.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlInHangUp object.
*/
	{
	CDlInHangUp* r=new(ELeave) CDlInHangUp(aSMObserver, aNdEnv);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlInHangUp::CDlInHangUp(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv)
	: CAgentStateBase(aSMObserver),iNdEnv(&aNdEnv)
/**
Private constructor for CDlInHangUp, used in the first phase of construction.

@param aSMObserver a reference to the database accessor.
@param aNdEnv a reference to the Netdial which defines a set of utility functions.
*/
	{}

void CDlInHangUp::ConstructL()
/**
Instantiate Member variable.
*/
	{}

CDlInHangUp::~CDlInHangUp()
/**
Destructor.
Cancel active request.
*/
	{
	Cancel();
	}

void CDlInHangUp::StartState()
/**
Starts dial in hangup state.
Call UpdateProgress with ECsdStartingHangUp and KErrNone.
If COMM-port is loaned, return port to ETEL.
If returning of COMM-port leaves, trap error and call JumpToRunl with error.
If call is active, start hangup and set state active.
Else call JumpToRunl() with KErrNone.
*/
	{
	iSMObserver->UpdateProgress(ECsdStartingHangUp,KErrNone);

	if (iNdEnv->TelServPrc()->CommPortLoaned())
		{
		// returns ownership of the comm port to ETEL
		TRAPD(err,(iNdEnv->TelServPrc())->ReturnCommPortL());
		if(err!=KErrNone)
			{
			JumpToRunl(err);
			return;
			}
		}

	if (iNdEnv->TelServPrc()->CallActive())
		{
		(iNdEnv->TelServPrc())->StartHangUpAfterDialIn(*this);
		iStatus=KRequestPending;
		SetActive();
		}
	else
		JumpToRunl(KErrNone);
	}

CAgentStateBase* CDlInHangUp::NextStateL(TBool /*aContinue*/)
/**
Changes to from init state to next state.
Disconnect will be the next state whether or not a client has requested a connection cancel.
Create disconnect state.

@exception Leaves if NewL() leaves.
@return a new CDlInDisconnect object.
*/
	{
	return CDlInDisconnect::NewL(*iSMObserver, *iNdEnv);
	}

void CDlInHangUp::TelFunctionComplete(TInt aError)
/**
Dial in hangup TelFunctionComplete.
Complete state with aError.

@param aError identifies the error code for completition.
*/
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}

void CDlInHangUp::DoCancel()
/**
Cancels active requests.
*/
	{
	if((iNdEnv->TelServPrc())!=NULL)
		(iNdEnv->TelServPrc())->Cancel();
	if(iStatus==KRequestPending)
		{
		TelFunctionComplete(KErrCancel);
		}
	}

void CDlInHangUp::RunL()
/**
Dial in hangup completed.
If there is an error then signal it, otherwise advance phase 
Hangup is always asynchronous, so it should never complete when cancelled.
*/
	{
	//update the log object
	iNdEnv->Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTED, KLogDataEventTypeUid);
	if(iStatus!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLINHANGUP_RUNL_1, "NetDial:\tCompleted Phase Hanging Up with Error %d",iStatus.Int());
		}
	iSMObserver->UpdateProgress(ECsdFinishedHangUp,KErrNone);
	(iNdEnv->BaseEnv())->CompleteState(iStatus.Int());
	}
