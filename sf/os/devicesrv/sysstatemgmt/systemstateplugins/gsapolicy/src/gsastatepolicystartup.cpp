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

#include <barsc2.h>
#include <barsread2.h>
#include <e32property.h>
#include <e32uid.h>
#include <e32std.h>
#include <bautils.h>
#include <centralrepository.h>

#include <ssm/ssmcmd.hrh>
#include <ssm/ssmsubstates.hrh>
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
#include <ssm/ssmpatchableconstants.h>
#include <ssm/conditiontypes.hrh>
#endif

#include <ssm/ssmstatetransition.h>
#include <ssm/ssmcommandlistresourcereader.h>

#include "gsastatepolicystartup.h"
#include "ssmdebug.h"
#include "ssmpanic.h" 
#include "s32file.h" 
#include "ssmpolicypluginsprivatecrkeys.h"

/**
Panic used by Startup policy plug-in when resource reader is invalid.
Strings must not be longer than 16 characters or they will be truncated by User::Panic()
*/
_LIT(KPanicGsaStartupState, "StartupPolicy");

/**
 Start-up state policy resource file path format : "z:/private/<SID of SSM>/startup/<Value of KSystemStartupModeKey>/"
*/
_LIT(KCommandListPath, "z:\\private\\2000D75B\\startup\\%d\\");

/**
 Commandlist path to launch 'sysstart.exe' when a resource file for 'start-up' is not found.
 fallback to 'sysstart.exe' resource file path format : "z:/private/<SID of SSM>/startup/fallback/"
*/
_LIT(KFallbackCmdListPath, "z:\\private\\2000D75B\\startup\\fallback\\");

/**
Used to create an instance of MSsmStatePolicy class.

@return A pointer to an instance of MSsmStatePolicy
*/
EXPORT_C MSsmStatePolicy* CGsaStatePolicyStartup::NewL()
	{
	CGsaStatePolicyStartup* self = new (ELeave) CGsaStatePolicyStartup;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Gets the hardware reason for KSystemStartupModeKey and makes a RFs connection.
Creates Command list path and the resource reader for startup.

@leave One of the error value returned by 
	    RProperty::Get() 
	    RFs::Connect() 
	    RArray::AppendL() 
	    NewL()
@see RProperty::Get
*/
void CGsaStatePolicyStartup::ConstructL()
	{
	// Read the hardware reason
	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, iHardwareReason));
	User::LeaveIfError(iFs.Connect());
	
	// Add supported transitions from Startup 'ESsmStartup'
	iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmFail, KSsmAnySubState));
	iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmShutdown, KSsmAnySubState));
	iCurrentlySupportedTransitions.AppendL(TSsmState(ESsmShutdown, ESsmShutdownSubStateCritical));

	TFileName cmdListPath;
	GetCommandListPath(iHardwareReason, cmdListPath);
	DEBUGPRINT2(_L("Startup Policy : Startup command list path : %S"), &cmdListPath);

	// create resource reader
	iCommandListResourceReader = CSsmCommandListResourceReader::NewL(iFs, cmdListPath, *this);	
	}

/**
default CTOR
*/
CGsaStatePolicyStartup::CGsaStatePolicyStartup()
	{
	}

/**
DTOR
*/
CGsaStatePolicyStartup::~CGsaStatePolicyStartup()
	{
	delete iCommandListResourceReader;
	iSubStates.Close();
	iFs.Close();
	iCurrentlySupportedTransitions.Close();
	}

/**
Initializes command list resource reader.

@param aStatus to complete when the initialization operation has finished
@panic EInvalidResourceReader if the command list resource reader is invalid

@see MSsmStatePolicy::Initialize
*/
void CGsaStatePolicyStartup::Initialize(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, PanicNow(KPanicGsaStartupState, EInvalidResourceReader));
	
	// initialise command list resource reader.
	iCommandListResourceReader->Initialise(aStatus);
	}

/**
Cancels an asynchronous Initialize operation.

@see MSsmStatePolicy::InitializeCancel
*/
void CGsaStatePolicyStartup::InitializeCancel()
	{
	iCommandListResourceReader->InitialiseCancel();
	}

/** 
Deletes all resources and frees itself.

@see MSsmStatePolicy::Release
*/
void CGsaStatePolicyStartup::Release()
	{
	delete this;
	}

/** 
Determines if an incoming startup state transition request should be accepted or rejected.
Clients calling this API should posess 'ECapabilityPowerMgmt', else the API will return ENotAllowed.

@param aRequest Contains information about the new request
@param aCurrent Contains NULL or the first accepted but not yet completed transition request
@param aQueued Contains NULL or a second accepted but not yet started transition request
@param aMessage Message sent by SSM server, used to check if the client has 'ECapabilityPowerMgmt'

@return one of the TResponse value
@see MSsmStatePolicy::TransitionAllowed
@see MSsmStatePolicy::TResponse
*/
MSsmStatePolicy::TResponse CGsaStatePolicyStartup::TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, 
																TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage)
	{
	TResponse response = ENotAllowed;
	if (!aMessage.HasCapability(ECapabilityPowerMgmt))
		{
		DEBUGPRINT1(_L ("Startup Policy : Capability Check Failed."));
		return response;
		}

	//Check if the requested transition is supported from current state
	if(TransitionSupported(aRequest.State()))
		{
		if((NULL == aCurrent) && (NULL == aQueued))
			{
			// SsmServer is idle
			response = EDefinitelyAllowed;
			}
		else if((aRequest.State().MainState() == ESsmFail) || (aRequest.State().MainState() == ESsmShutdown))
			{
			// Going into failed state or shutdown state will override anything currently ongoing or queued
			response = EReplaceCurrentClearQueue;
			}
		}

#ifdef _DEBUG
	TSsmStateName name = aRequest.State().Name();
	if(ENotAllowed == response)
		{
		DEBUGPRINT3(_L("Startup Policy : Transition (Requested State: %S) is not allowed (Response: %d)."), &name, response);
		}
	else
		{
		DEBUGPRINT3(_L("Startup Policy : Transition (Requested State %S) is allowed (Response %d)."), &name, response);		
		}
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
void CGsaStatePolicyStartup::PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iCommandListResourceReader, PanicNow(KPanicGsaStartupState, EInvalidResourceReader));

	iSoftwareReason = aReason;

	//Let's start from the beginning if no specific minor state is selected
	iRequestedSubState = ((aState.SubState() == KSsmAnySubState) ? ESsmStartupSubStateCriticalStatic : aState.SubState());
	TSsmState publishState;
	publishState.Set(aState.MainState(), iRequestedSubState);

	const TInt commandListId = publishState.SubState();

	//Build the commandlist from resource
	iCommandListResourceReader->PrepareCommandList(commandListId, publishState, aStatus);
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
Cancels asynchronous PrepareCommandList operation.

@see MSsmStatePolicy::PrepareCommandListCancel
*/
void CGsaStatePolicyStartup::PrepareCommandListCancel()
	{
	iCommandListResourceReader->PrepareCommandListCancel();
	}

/**	
Return the command list once the  PrepareCommandList has completed.
Ownership of the returned command list is transferred to the caller.
@panic EInvalidResourceReader if the command list resource reader is invalid
@return The command list created during the preceding PrepareCommandList step
*/
CSsmCommandList* CGsaStatePolicyStartup::CommandList()
	{
	__ASSERT_DEBUG(iCommandListResourceReader , PanicNow(KPanicGsaStartupState, EInvalidResourceReader));

	return iCommandListResourceReader->GetCommandList();
	}

/**
Determines the next sub state transition.
@param aCurrentTransition Contains the last executed state
@param aReason Contains the reason as given by the request
@param aError Contains the completion code from the last executed sub-state transition
@param aSeverity Contains the severity of the failed command in case the sub-state transition ended with an error
@param aNextState The next System State to head for, if there is one
@panic EInvalidStartupstate if the current state is not startup
@return 	ETrue if aNextState contains another System State to head for, or 
		EFalse if there is no further transitions to do.
@see MSsmStatePolicy::GetNextState
*/
TBool CGsaStatePolicyStartup::GetNextState(TSsmState aCurrentTransition, TInt /*aReason*/, TInt aError, TInt /*aSeverity*/, TSsmState& aNextState)
	{
	__ASSERT_ALWAYS(aCurrentTransition.MainState() == ESsmStartup, PanicNow(KPanicGsaStartupState, EInvalidStartupState));

	if (KErrNone != aError)	// Handle CLE error here
		{
		if (iLaunchSysStart)	// 'sysstart.exe' was launched unsuccessfully so launch 'sysagt2srv.exe' and 'wserv.exe'
			{
			iLaunchSysStart = EFalse;
			iLaunchSysAgt2SrvAndWServ = ETrue;
			DEBUGPRINT2(_L("Startup Policy : sysstart.exe launched with error : %d"), aError);
			aNextState = TSsmState(ESsmStartup, ESsmStartupSubStateCriticalDynamic);
			return ETrue;
			}

#ifdef __WINS__	// on emulator
			{
			DEBUGPRINT2(_L("Startup Policy : CLE returned with (error : %d), Panic on Emulator"), aError);
			DEBUGPRINT1(_L("Startup Policy : Emulator (__WINS__) does not support a re-start, so Fail Policy is not invoked."));
			PanicNow(KPanicGsaStartupState, EEmulatorPowerOff);
			}
#else	// on hardware/device
			{
			TInt resetLimit = -1;
			TInt bootCount = -1;
			TRAPD( err, GetStartupCountAndMaxbootLimitL(bootCount, resetLimit) );
			if (bootCount < resetLimit && KErrNone == err)
				{
				aNextState = TSsmState(ESsmFail, ESsmFailSubStateRestart);
				}
			else	// Maximum allowed boot attempts has been made. Device needs a poweroff. Probable candidate for a reset/reflash.
				{
				aNextState = TSsmState(ESsmFail, ESsmFailSubStatePowerOff);
				}
	#ifdef _DEBUG
			TSsmStateName name = aNextState.Name();
			DEBUGPRINT3(_L("Startup Policy : CLE returned with (error : %d) so moving to Fail State : %S."), aError, &name);
	#endif
			return ETrue;
			}
#endif
		}
	else if(iLaunchSysStart || iLaunchSysAgt2SrvAndWServ)	// If either sysstart or sysagt2srv and wserv was launched and CLE did not return an error
		{
		if (iLaunchSysStart)
			{
			iLaunchSysStart = EFalse;
			DEBUGPRINT1(_L("Startup Policy : sysstart.exe launched successfully."));
			}
		if (iLaunchSysAgt2SrvAndWServ)
			{
			iLaunchSysAgt2SrvAndWServ = EFalse;
			DEBUGPRINT1(_L("Startup Policy : sysagt2srv.exe and wserv.exe launched successfully."));
			}
		aNextState = TSsmState(ESsmNormal, KSsmAnySubState);	// move to Normal state
		return ETrue;
		}
	else	// have to move one with the next substates in this state
		{
		// Get the sub states from resource reader only once
		if (!iSubStatesCount)
			{
			// Get sub states list from resource reader
			TRAPD(err, iCommandListResourceReader->GetCommandListIdsL(iSubStates));
			if (err)
				{
				DEBUGPRINT2(_L("Startup Policy : Command list ids prepared with error: %d"), err);
				}
			else
				{
				iSubStatesCount = iSubStates.Count();
				}
			}

		TInt index = iSubStates.Find(iRequestedSubState);

		if (KErrNotFound == index)
			{
			DEBUGPRINT2(_L("Startup Policy : SubState for transition not found: %d"), index);
			PanicNow(KPanicGsaStartupState, ESubStateIndexNotFound);
			}
		else if (index == (iSubStatesCount - 1))	// transition complete, move to Normal state
			{
			TInt retVal = EFalse;
			// moving to next state as the transition is completed for ESsmStartup
			if (iSubStatesCount && (iRequestedSubState == iSubStates[iSubStatesCount-1]))
				{
				aNextState = TSsmState(ESsmNormal, KSsmAnySubState);
				retVal = ETrue;
				}
			return retVal;
			}
		else		// there is a substate available for transition, moved ahead
			{
			iRequestedSubState = iSubStates[++index];
			aNextState = TSsmState(ESsmStartup, iRequestedSubState);
#ifdef _DEBUG
			TSsmStateName name = aNextState.Name();
			DEBUGPRINT2(_L("Startup Policy : Transition to next state is : %S"), &name);
#endif				
			return ETrue;
			}
		}
	return EFalse;
	} //lint !e1746 Suppress parameter 'aCurrentTransition' could be made const reference

/**
Callback used by CSsmCommandListResourceReader when a decision needs to be made
on whether to include a command in a command list or not.

@param aResourceFile Instance of CResourceFile
@param aResourceId Resource id of SSM_SYMBIAN_CONDITIONAL_INFORMATION struct for command
@return ETrue in case the command needs to be included in command list, else EFalse.

@see MSsmConditionalCallback::ConditionalCommandAllowedL
*/
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN 
TBool CGsaStatePolicyStartup::ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId)
#else
TBool CGsaStatePolicyStartup::ConditionalCommandAllowedL(CResourceFile& /*aResourceFile*/, TInt /*aResourceId*/)
#endif
	{
	TBool isAllowed = EFalse;        
#ifdef SYMBIAN_SSM_GRACEFUL_SHUTDOWN
   	HBufC8* buf = aResourceFile.AllocReadLC(aResourceId);
   	TResourceReader reader; 
	reader.SetBuffer(buf);
	//Read the type of the command from the resource file
	TUint16 type = reader.ReadUint16();
	CleanupStack::PopAndDestroy(buf);
	// check that the type is equal to "EGracefulShutdown"
	if(type == EGracefulShutdown)    
		{
		//Check whether SSM graceful shutdown is enabled or not
		if(IsSsmGracefulShutdown())   
			{
			isAllowed = ETrue;
			}
		}
#else
	// no commands use 'conditional_information' in Startup state command list.
	PanicNow(KPanicGsaStartupState, EConditionalInfoNotImplemented);
#endif
	return isAllowed;
	}

/*
Helper function to check whether requested transition is supported or not.
*/
TBool CGsaStatePolicyStartup::TransitionSupported(const TSsmState& aRequestedState) const
	{
	return (iCurrentlySupportedTransitions.Find(aRequestedState) > KErrNotFound);
	}

/*
Helper function to create command list path for start-up.
Implements fallback mechanism to launch 'sysstart.exe' if static command list for 'start-up' state is not found.
This temporary implementation is required during migration from existing 'sysstart' to 'ssma start-up'.
*/
void CGsaStatePolicyStartup::GetCommandListPath(TUint aBootMode, TDes& aCmdListPath)
	{
	aCmdListPath.Format(KCommandListPath, aBootMode);
	TBool found = BaflUtils::FolderExists(iFs, aCmdListPath);

	// This Fallback mechanism is used internally during migration from 'sysstart' to 'ssma start-up' and is not required otherwise.
	if (!found)
		{
		aCmdListPath.Copy(KFallbackCmdListPath());
		iLaunchSysStart = ETrue;	// launch 'sysstart.exe'
		}
	}

/*
Helper function to get the startup boot count and maximum boot attempts limit 
from the central repository
*/
void CGsaStatePolicyStartup::GetStartupCountAndMaxbootLimitL(TInt &aBootcount, TInt &aResetLimit)
	{
	CRepository* repository = CRepository::NewLC( KCRUidSsmStartupPolicy );
	User::LeaveIfError( repository->Get( KSsmStartupErrorResetLimit, aResetLimit ) );
	DEBUGPRINT2( _L("Reset limit is %d"), aResetLimit );
	User::LeaveIfError( repository->Get( KSsmStartupErrorResetCounter, aBootcount ) );
	DEBUGPRINT2( _L("Reset count is %d"), aBootcount );
	CleanupStack::Pop(repository);
	}
