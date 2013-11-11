// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/startupdomainpskeys.h>
#include <ssm/ssmpatchableconstants.h>

#include "gsastatepolicynormal.h"
#include "ssmdebug.h"
#include "ssmpanic.h"

/**
Panic used by Normal policy plug-in when resource reader is invalid.
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
*/
_LIT(KPanicGsaNormalState, "NormalStatePolicy");

/**
 Normal state policy resource file path. "z:/private/<SID of SSM>/normal/"
*/ 
_LIT(KCommandListPath, "z:\\private\\2000D75B\\normal\\");

/**
Used to create an instance of MSsmStatePolicy class.

@return A pointer to an instance of MSsmStatePolicy
*/
EXPORT_C MSsmStatePolicy* CGsaStatePolicyNormal::NewL()
	{
	CGsaStatePolicyNormal* self = new (ELeave) CGsaStatePolicyNormal;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
 
 /**
default CTOR
*/
CGsaStatePolicyNormal::CGsaStatePolicyNormal()
	{
	}

/**
DTOR
*/
CGsaStatePolicyNormal::~CGsaStatePolicyNormal()
	{
	delete iCommandListResourceReader;
	delete iEvaluator;
	iCurrentlySupportedTransitions.Close();
	iFs.Close();	
	}

/**
Makes a RFs connection and maintains a list of supported transition states from normal state.

@leave One of the error value returned by RFs::Connect() or RArray::AppendL()
*/
void CGsaStatePolicyNormal::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	
	if (IsSsmGracefulOffline())
        {
        // Used to evaluate conditions in command lists.
        iEvaluator = CConditionEvaluate::NewL();
        //Substates ESsmNormalRfOnSubState and ESsmNormalRfOffSubState are the valid transition from Normal State
        iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmNormal, ESsmNormalRfOnSubState));
        iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmNormal, ESsmNormalRfOffSubState));
        }	
	// Add supported transitions from Normal 'ESsmNormal'
	iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmShutdown, KSsmAnySubState));
	iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmFail, KSsmAnySubState));
	iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmShutdown, ESsmShutdownSubStateCritical));

	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, KCommandListPath(), *this);
	}

/**
Initializes command list resource reader.

@param aStatus to complete when the initialization operation has finished
@panic EInvalidResourceReader if the command list resource reader is invalid

@see MSsmStatePolicy::Initialize
*/
void CGsaStatePolicyNormal::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, PanicNow(KPanicGsaNormalState, EInvalidResourceReader));

	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

/**
Cancels an asynchronous Initialize operation.

@see MSsmStatePolicy::InitializeCancel
*/
void CGsaStatePolicyNormal::InitializeCancel()
	{
	iCommandListResourceReader->InitialiseCancel();
	}

/** 
Deletes all resources and frees itself.

@see MSsmStatePolicy::Release
*/
void CGsaStatePolicyNormal::Release()
	{
	delete this;
	}

/** 
Determines if an incoming normal state transition request should be accepted or rejected.
Transition to ESsmFail and ESsmShutdown state is allowed from ESsmNormal state. All other requests are rejected.
Clients calling this API should posess 'ECapabilityPowerMgmt', else the API will return ENotAllowed.

@param aRequest Contains information about the new request
@param aCurrent Contains NULL or the first accepted but not yet completed transition request
@param aQueued Contains NULL or a second accepted but not yet started transition request
@param aMessage Message sent by SSM server, used to check if the client has 'ECapabilityPowerMgmt'

@return one of the TResponse value
@see MSsmStatePolicy::TransitionAllowed
@see MSsmStatePolicy::TResponse
*/
MSsmStatePolicy::TResponse CGsaStatePolicyNormal::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, 
															TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	TResponse response = ENotAllowed;
	if (!aMessage.HasCapability(ECapabilityPowerMgmt))
		{
		return response;
		}

	//Check if the requested transition is supported from current state
	if (TransitionSupported(aRequest.State()))
		{		
		//Transition is allowed when there is no current and queued transitions.
		//or allow substate(ESsmNormalRfOnSubState and ESsmNormalRfOffSubState)transition only when KSsmGracefulOffline is enabled and queue is empty.
		if (((NULL == aCurrent) && (NULL == aQueued)) || (IsSsmGracefulOffline() && (NULL == aQueued)))
			{
			response = EDefinitelyAllowed;
			}
		else if((aRequest.State().MainState() == ESsmFail) || (aRequest.State().MainState() == ESsmShutdown))
			{
			// Going into fail/shutdown state will override anything currently ongoing or queued
			response = EReplaceCurrentClearQueue;
			}
		}

#ifdef _DEBUG
	TSsmStateName name = aRequest.State().Name();
	DEBUGPRINT3(_L("Normal Policy : Transition (Requested State: %S) is allowed with (Response: %d)."), &name, response);
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
void CGsaStatePolicyNormal::PrepareCommandList(TSsmState aState, TInt /*aReason*/, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG( iCommandListResourceReader , PanicNow(KPanicGsaNormalState, EInvalidResourceReader));

	//Let's start from the beginning if no specific sub state is selected
	const TUint16 substate = aState.SubState();
	const TSsmState state(aState.MainState(), (substate==KSsmAnySubState) ? ESsmNormalSubState : substate);
	const TInt commandListId = state.SubState();

	//Build the commandlist from resource
	iCommandListResourceReader->PrepareCommandList(commandListId, state, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
Cancels asynchronous PrepareCommandList operation.

@see MSsmStatePolicy::PrepareCommandListCancel
*/
void CGsaStatePolicyNormal::PrepareCommandListCancel()
	{
	iCommandListResourceReader->PrepareCommandListCancel();
	}

/**
Return the command list once the PrepareCommandList has completed.
Ownership of the returned command list is transferred to the caller.
@panic EInvalidResourceReader if the command list resource reader is invalid
@return The command list created during the preceding PrepareCommandList step
*/
CSsmCommandList* CGsaStatePolicyNormal::CommandList()
	{
	__ASSERT_DEBUG( iCommandListResourceReader , PanicNow(KPanicGsaNormalState, EInvalidResourceReader));

	return iCommandListResourceReader->GetCommandList();
	}

/**
Determines the next sub state transition.
@param aCurrentTransition Contains the last executed state
@param aReason Contains the reason as given by the request
@param aError Contains the completion code from the last executed sub-state transition
@param aSeverity Contains the severity of the failed command in case the sub-state transition ended with an error
@param aNextState The next System State to head for, if there is one
@panic EInvalidNormalState if the current state is not Normal
@return 	ETrue if aNextState contains another System State to head for, or 
		EFalse if there is no further transitions to do.
@see MSsmStatePolicy::GetNextState
*/
TBool CGsaStatePolicyNormal::GetNextState(TSsmState aCurrentTransition, TInt /*aReason*/, TInt aError, TInt /*aSeverity*/, TSsmState& aNextState)
	{
	__ASSERT_ALWAYS(aCurrentTransition.MainState() == ESsmNormal, PanicNow(KPanicGsaNormalState, EInvalidNormalState));
	DEBUGPRINT2(_L("Normal Policy : CLE returned with error : %d after execution of Normal command list."), aError);

	if (KErrNone != aError)
		{
		aNextState = TSsmState(ESsmFail, KSsmAnySubState);
#ifdef _DEBUG
		TSsmStateName name = aNextState.Name();
		DEBUGPRINT3(_L("Normal Policy : CLE returned with (error : %d) so moving to Fail State : %S."), aError, &name);
#endif
		return ETrue;
		}
	
	//Boot into RF ON/OFF substate when patchable constant KSsmGracefulOffline is enabled and the state
	//transition is requested from Startup to Normal with a substate request "KSsmAnySubState" .
	//This will be executed only once in the bootup.
 	if (IsSsmGracefulOffline() && (aCurrentTransition.SubState() == KSsmAnySubState))
       {               
        TInt offlineMode = EBootIntoOnlineModeUninitialized;
        TInt simStatus = ESimUsable;
        if ((RProperty::Get(KPSUidStartup, KStartupBootIntoOffline, offlineMode) == KErrNone) && (RProperty::Get(KPSUidStartup, KPSSimStatus, simStatus ) == KErrNone))
            {                
            //Boot into RF OFF substate if KStartupBootIntoOffline=EBootIntoOfflineMode or KPSSimStatus!=SimUsable
            //or else boot in to RF ON substate
            TUint16 nextSubstate;
            nextSubstate = (offlineMode == EBootIntoOfflineMode || !(simStatus == ESimUsable)) ? ESsmNormalRfOffSubState : ESsmNormalRfOnSubState;
            aNextState = TSsmState(ESsmNormal, nextSubstate);
#ifdef _DEBUG
            TSsmStateName name = aNextState.Name();
            DEBUGPRINT2(_L("Normal Policy : Transition to next state is : %S"), &name);
#endif       
            return ETrue;
            }
        else
            {
            //Move to Fail state if P&S keys KStartupBootIntoOffline and KPSUidStartup are not defined.
            aNextState = TSsmState(ESsmFail, KSsmAnySubState);
            #ifdef _DEBUG
                    TSsmStateName name = aNextState.Name();
                    DEBUGPRINT3(_L("Normal Policy : P&S key undefined returned with (error : %d) so moving to Fail State : %S."), aError, &name);
            #endif
            return ETrue;
            }
        }	
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
TBool CGsaStatePolicyNormal::ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId)
	{
	TBool ret = EFalse;
	if (IsSsmGracefulOffline())
	    {
	    //conditions are evaluated when patchable constant KSsmGracefulOffline is enabled
	    ret = iEvaluator->EvaluateL(aResourceFile, aResourceId);
	    }
	else
	    {
        // no commands use 'conditional_information' in Normal state command list.
        PanicNow(KPanicGsaNormalState, EConditionalInfoNotImplemented);
        }
	return ret;
	}

/*
Helper function to check whether requested transition is supported or not.
@param aRequestedState Requested transition
@return 	ETrue if transition is supported
		EFalse if transition is not supported
*/
TBool CGsaStatePolicyNormal::TransitionSupported(const TSsmState& aRequestedState) const
	{
	return (iCurrentlySupportedTransitions.Find(aRequestedState) > KErrNotFound);
	}
