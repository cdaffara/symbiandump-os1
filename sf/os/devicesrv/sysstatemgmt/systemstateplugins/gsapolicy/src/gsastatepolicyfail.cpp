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

#include <barsread2.h>
#include <e32property.h>
#include <e32uid.h>

#include <ssm/ssmcmd.hrh>
#include <ssm/ssmsubstates.hrh>
#include <ssm/ssmcommandlistresourcereader.h>

#include "gsastatepolicyfail.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
Panic used by Fail policy plug-in when resource reader is invalid.
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
*/
_LIT(KPanicGsaFailState, "FailStatePolicy");

/**
 Fail state policy resource file path. "z:/private/<SID of SSM>/fail/"
*/
_LIT(KCommandListPath, "z:\\private\\2000D75B\\fail\\");

/**
Used to create an instance of MSsmStatePolicy class.

@return A pointer to an instance of MSsmStatePolicy
*/
EXPORT_C MSsmStatePolicy* CGsaStatePolicyFail::NewL()
	{
	CGsaStatePolicyFail* self = new (ELeave) CGsaStatePolicyFail;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
 /**
default CTOR
*/
CGsaStatePolicyFail::CGsaStatePolicyFail()
	{
	}

/**
DTOR
*/
CGsaStatePolicyFail::~CGsaStatePolicyFail()
	{
	delete iCommandListResourceReader;
	iFs.Close();	
	}

/**
Makes a RFs connection.
Creates Command list path and the resource reader.

@leave One of the error value returned by RFs::Connect() or NewL()
@see RFs::Connect()
*/
void CGsaStatePolicyFail::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());

	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

/**
Initializes command list resource reader.

@param aStatus to complete when the initialization operation has finished

@panic EInvalidResourceReader if the command list resource reader is invalid

@see MSsmStatePolicy::Initialize
*/
void CGsaStatePolicyFail::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, PanicNow(KPanicGsaFailState, EInvalidResourceReader));

	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

/**
Cancels an asynchronous Initialize operation.

@see MSsmStatePolicy::InitializeCancel
*/
void CGsaStatePolicyFail::InitializeCancel()
	{
	iCommandListResourceReader->InitialiseCancel();
	}

/** 
Deletes all resources and frees itself.

@see MSsmStatePolicy::Release
*/
void CGsaStatePolicyFail::Release()
	{
	delete this;
	}

/** 
No transition is allowed from Fail state. Requests for transition to any state is returned as ENotAllowed.

@param aRequest Contains information about the new request
@param aCurrent Contains NULL or the first accepted but not yet completed transition request
@param aQueued Contains NULL or a second accepted but not yet started transition request
@param aMessage Message sent by SSM server

@return always ENotAllowed
@see MSsmStatePolicy::TransitionAllowed
*/
MSsmStatePolicy::TResponse CGsaStatePolicyFail::TransitionAllowed(const TSsmStateTransition& /*aRequest*/, TSsmStateTransition const* /*aCurrent*/, 
															TSsmStateTransition const* /*aQueued*/, const RMessagePtr2& /*aMessage*/)
	{
	return ENotAllowed;
	}

/** 
Create the command list associated with a sub state transition.

@param aState Contains the state and substate that identifies the command list to create
@param aReason Contains the reason as given by the request
@param aStatus to complete when the operation has finished
@panic EInvalidResourceReader if the command list resource reader is invalid
@see MSsmStatePolicy::PrepareCommandList
*/ 
void CGsaStatePolicyFail::PrepareCommandList(TSsmState aState, TInt /*aReason*/, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG( iCommandListResourceReader , PanicNow(KPanicGsaFailState, EInvalidResourceReader));

	//Let's start from the beginning if no specific sub state is selected
	const TUint16 substate = aState.SubState();
	const TSsmState state(aState.MainState(), (substate==KSsmAnySubState) ? ESsmFailSubStateRestart : substate);
	const TInt commandListId = state.SubState();

	//Build the commandlist from resource
	iCommandListResourceReader->PrepareCommandList(commandListId, state, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
Cancels asynchronous PrepareCommandList operation.

@see MSsmStatePolicy::PrepareCommandListCancel
*/
void CGsaStatePolicyFail::PrepareCommandListCancel()
	{
	iCommandListResourceReader->PrepareCommandListCancel();
	}

/**
Return the command list once the PrepareCommandList has completed.
Ownership of the returned command list is transferred to the caller.
@panic EInvalidResourceReader if the command list resource reader is invalid
@return The command list created during the preceding PrepareCommandList step
*/
CSsmCommandList* CGsaStatePolicyFail::CommandList()
	{
	__ASSERT_DEBUG( iCommandListResourceReader , PanicNow(KPanicGsaFailState, EInvalidResourceReader));

	return iCommandListResourceReader->GetCommandList();
	}

/**
Determines the next sub state transition.
Will never reach GetNextState() if transition to Fail is sucessful as the device would be shutdown/restarted. 
Only case when GetNextState is executed is due to a failure in executing Fail transition.
This would cause a panic and thereby causing a device restart.

@param aCurrentTransition Contains the last executed state
@param aReason Contains the reason as given by the request
@param aError Contains the completion code from the last executed sub-state transition
@param aSeverity Contains the severity of the failed command in case the sub-state transition ended with an error
@param aNextState The next System State to head for, if there is one
@panic EInvalidFailState if the main state is not Fail
@panic ERestartDevice if the value returned by CLE is null
@return 	ETrue if aNextState contains another System State to head for, or 
		EFalse if there is no further transitions to do.
@see MSsmStatePolicy::GetNextState
*/
TBool CGsaStatePolicyFail::GetNextState(TSsmState aCurrentTransition, TInt /*aReason*/, TInt aError, TInt /*aSeverity*/, TSsmState& /*aNextState*/)
	{
	DEBUGPRINT2(_L("Fail Policy : CLE returned completion code : %d after executing the Fail command list."), aError);
	__ASSERT_ALWAYS(aCurrentTransition.MainState() == ESsmFail, PanicNow(KPanicGsaFailState, EInvalidFailState));
	__ASSERT_ALWAYS((KErrNone == aError), PanicNow(KPanicGsaFailState, ERestartDevice));


	return EFalse;	// no more substates to execute

	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

/**
Callback used by CSsmCommandListResourceReader when a decision needs to be made
on whether to include a command in a command list or not.

@param aResourceFile Instance of CResourceFile
@param aResourceId Resource id of SSM_SYMBIAN_CONDITIONAL_INFORMATION struct for command
@return ETrue in case the command needs to be included in command list, else EFalse.

@see MSsmConditionalCallback::ConditionalCommandAllowedL
*/
TBool CGsaStatePolicyFail::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt /*aResourceId*/)
	{
	// no commands use 'conditional_information' in Fail state command list.
	PanicNow(KPanicGsaFailState, EConditionalInfoNotImplemented);
	return EFalse; // keeping compiler happy
	}
