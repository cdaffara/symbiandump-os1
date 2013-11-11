// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <barsc2.h>
#include <barsread2.h>
#include <e32property.h>
#include <e32uid.h>
#include <e32power.h>

#include <ssm/ssmcmd.hrh>
#include <ssm/ssmsubstates.hrh>

#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlistresourcereader.h>

#include "gsastatepolicyshutdown.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
Panic used by Shutdown policy plug-in when resource reader is invalid.
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
*/
_LIT(KPanicGsaShutdownState, "ShutdownStatePolicy");

/**
 Shutdown state policy resource file path. "z:/private/<SID of SSM>/shutdown/"
*/
_LIT(KCommandListPath, "z:\\private\\2000D75B\\shutdown\\");

/**
Used to create an instance of MSsmStatePolicy class.

@return A pointer to an instance of MSsmStatePolicy
*/
EXPORT_C MSsmStatePolicy* CGsaStatePolicyShutdown::NewL()
	{
	CGsaStatePolicyShutdown* self = new (ELeave) CGsaStatePolicyShutdown;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Makes a RFs connection and maintains a list of supported transition states from shutdown state.

@leave One of the error value returned by RProperty::Get() or RFs::Connect() or RArray::AppendL()
@see RProperty::Get
*/
void CGsaStatePolicyShutdown::ConstructL()
	{
	// Read the hardware reason
	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, iHardwareReason));
	User::LeaveIfError(iFs.Connect());

	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

/**
default CTOR
*/
CGsaStatePolicyShutdown::CGsaStatePolicyShutdown()
	{
	}

/**
DTOR
*/
CGsaStatePolicyShutdown::~CGsaStatePolicyShutdown()
	{
	delete iCommandListResourceReader;
	iSubStates.Close();
	iFs.Close();
	}

/**
Initializes command list resource reader.

@param aStatus to complete when the initialization operation has finished
@panic EInvalidResourceReader if the command list resource reader is invalid

@see MSsmStatePolicy::Initialize
*/
void CGsaStatePolicyShutdown::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, PanicNow(KPanicGsaShutdownState, EInvalidResourceReader));

	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

/**
Cancels an asynchronous Initialize operation.

@see MSsmStatePolicy::InitializeCancel
*/
void CGsaStatePolicyShutdown::InitializeCancel()
	{
	iCommandListResourceReader->InitialiseCancel();
	}

/** 
Deletes all resources and frees itself.

@see MSsmStatePolicy::Release
*/
void CGsaStatePolicyShutdown::Release()
	{
	delete this;
	}

/** 
Determines if an incoming shutdown state transition request should be accepted or rejected.
Only transition to ESsmFail state is allowed from ESsmShutdown state. All other requests are rejected.
Clients calling this API should posess 'ECapabilityPowerMgmt', else the API will return ENotAllowed.

@param aRequest Contains information about the new request
@param aCurrent Contains NULL or the first accepted but not yet completed transition request
@param aQueued Contains NULL or a second accepted but not yet started transition request
@param aMessage Message sent by SSM server, used to check if the client has 'ECapabilityPowerMgmt'

@return one of the TResponse value
@see MSsmStatePolicy::TransitionAllowed
@see MSsmStatePolicy::TResponse
*/
MSsmStatePolicy::TResponse CGsaStatePolicyShutdown::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* /*aCurrent*/, 
																TSsmStateTransition const* /*aQueued*/, const RMessagePtr2& aMessage)
	{
	TResponse response = ENotAllowed;

	if (!aMessage.HasCapability(ECapabilityPowerMgmt))
		{
		return response;
		}

	if (aRequest.State().MainState() == ESsmFail)
		{
		// Going into failed state will override anything currently ongoing or queued
		response = EReplaceCurrentClearQueue;
		}
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
	if(aRequest.State().MainState() == ESsmShutdown)
		{
		response = EDefinitelyAllowed;
		}
#endif	//SYMBIAN_SSM_GRACEFUL_SHUTDOWN

#ifdef _DEBUG
	TSsmStateName name = aRequest.State().Name();
	DEBUGPRINT3(_L("Shutdown Policy : Transition to state %S. allowed with response %d"), &name, response);	
#endif
	return response;
	}

/** 
Create the command list associated with a sub state transition.

@param aState Contains the state and substate that identifies the command list to create
@param aReason Contains the reason as given by the request
@param aStatus to complete when the operation has finished
@panic EInvalidResourceReader if the command list resource reader is invalid
@see MSsmStatePolicy::PrepareCommandList
*/ 
void CGsaStatePolicyShutdown::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG( iCommandListResourceReader , PanicNow(KPanicGsaShutdownState, EInvalidResourceReader));

	iSoftwareReason = aReason;

	//Let's start from the beginning if no specific minor state is selected
	iRequestedSubState = ((aState.SubState()==KSsmAnySubState) ? ESsmShutdownSubStateCritical : aState.SubState());
	iPublishState.Set(aState.MainState(), ((iRequestedSubState==KSsmAnySubState) ? ESsmShutdownSubStateCritical : iRequestedSubState));
	const TInt commandListId = iPublishState.SubState();

	//Build the commandlist from resource
	iCommandListResourceReader->PrepareCommandList(commandListId, iPublishState, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
Cancels asynchronous PrepareCommandList operation.

@see MSsmStatePolicy::PrepareCommandListCancel
*/
void CGsaStatePolicyShutdown::PrepareCommandListCancel()
	{
	iCommandListResourceReader->PrepareCommandListCancel();
	}

/**
Return the command list once the PrepareCommandList has completed.
Ownership of the returned command list is transferred to the caller.
@panic EInvalidResourceReader if the command list resource reader is invalid
@return The command list created during the preceding PrepareCommandList step
*/
CSsmCommandList* CGsaStatePolicyShutdown::CommandList()
	{
	__ASSERT_DEBUG( iCommandListResourceReader , PanicNow(KPanicGsaShutdownState, EInvalidResourceReader));

	return iCommandListResourceReader->GetCommandList();
	}

/**
Determines the next sub state transition.
@param aCurrentTransition Contains the last executed state
@param aReason Contains the reason as given by the request
@param aError Contains the completion code from the last executed sub-state transition
@param aSeverity Contains the severity of the failed command in case the sub-state transition ended with an error
@param aNextState The next System State to head for, if there is one
@panic EInvalidMainState if the current state is not shutdown
@return 	ETrue if aNextState contains another System State to head for, or 
		EFalse if there is no further transitions to do.
@see MSsmStatePolicy::GetNextState
*/
TBool CGsaStatePolicyShutdown::GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt /*aSeverity*/, TSsmState& aNextState)
	{
	__ASSERT_ALWAYS(aCurrentTransition.MainState() == ESsmShutdown, PanicNow(KPanicGsaShutdownState, EInvalidShutdownState));	
	DEBUGPRINT2(_L("Shutdown Policy : CLE returned with error : %d after execution of Shutdown command list."), aError);

	if (KErrNone != aError)
		{
		aNextState = TSsmState(ESsmFail, KSsmAnySubState);
#ifdef _DEBUG
		TSsmStateName name = aNextState.Name();
		DEBUGPRINT3(_L("Shutdown Policy : CLE returned with (error : %d) so moving to Fail State : %S."), aError, &name);
#endif		
		return ETrue;
		}

	// Get the sub states from resource reader only once
	if (!iSubStatesCount)
		{
		// Get sub states list from resource reader
		TRAPD(err, iCommandListResourceReader->GetCommandListIdsL(iSubStates));
		if (err)
			{
			DEBUGPRINT2(_L("Shutdown Policy : Command list ids prepared with error: %d"), err);
			}
		else
			{
			iSubStatesCount = iSubStates.Count();
			}
		}

	TInt index = iSubStates.Find(iRequestedSubState);

	if ((index < 0) || (index >= (iSubStatesCount - 1)))
		{
		return EFalse;
		}

	// ESsmShutdownSubStatePowerOff is transition to EPwStandby. So further transition should not be allowed.
	// ESsmShutdownSubStateReStart is transition to EPwRestart.
	// On successful restart (EPwRestart) control will not reach here again.
	if (iRequestedSubState == ESsmShutdownSubStatePowerOff)
		{
		return EFalse;
		}
	else if (ESsmShutdownSubStateNonCritical != iRequestedSubState)
		{ 
		iRequestedSubState = iSubStates[++index];
		aNextState = TSsmState(ESsmShutdown, iRequestedSubState);
		}
	else
		{
		TUint16 subState = (EPwRestart == aReason) ? ESsmShutdownSubStateReStart : ESsmShutdownSubStatePowerOff;
		aNextState = TSsmState(ESsmShutdown, subState);
		}
#ifdef _DEBUG
	TSsmStateName name = aNextState.Name();
	DEBUGPRINT2(_L("Shutdown Policy : Transition to next state is : %S"), &name);
#endif
	return ETrue;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

/**
Callback used by CSsmCommandListResourceReader when a decision needs to be made
on whether to include a command in a command list or not.

@param aResourceFile Instance of CResourceFile
@param aResourceId Resource id of SSM_SYMBIAN_CONDITIONAL_INFORMATION struct for command
@return ETrue in case the command needs to be included in command list, else EFalse.

@see MSsmConditionalCallback::ConditionalCommandAllowedL
*/
TBool CGsaStatePolicyShutdown::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt /*aResourceId*/)
	{
	// no commands use 'conditional_information' in Shutdown state command list.
	PanicNow(KPanicGsaShutdownState, EConditionalInfoNotImplemented);
	return EFalse;	// avoid a compiler warning.
	}
