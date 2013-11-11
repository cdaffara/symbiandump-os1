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

#include <ssm/ssmsubstates.hrh>

#include "ssmdebug.h"

#include "saastateadaptation.h"
#include "clayerpanic.h"
#include "startupadaptationadapter.h"


/*
 * Creates a new object associated with the passed in CStartupAdaptationAdapter
 * 
 * @internalComponent
 */
CSaaStateAdaptation* CSaaStateAdaptation::NewL(CStartupAdaptationAdapter* aAdapter)
	{
	CSaaStateAdaptation* self = new (ELeave) CSaaStateAdaptation(aAdapter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*
 * Destructor for this object
 * 
 * @internalComponent
 */
CSaaStateAdaptation::~CSaaStateAdaptation()
	{
	iEventQueue.Close();
	}

/*
 * Decrements the reference count for this object, deleting it if necessary
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::Release()
	{
	// This MClass is owned by the singleton CStartupAdaptationAdapter class so
	// release should do nothing.
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EGlobalStateChange);
	// Map TSsmState to TGlobalState
	iGlobalStatePckg() = MapToStartupAdaptationState(aState);
	
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}	
	}  //lint !e1746 Suppress parameter 'aState' could be made const reference

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::RequestCoopSysSelfTest(TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EExecuteSelftests);
	// No parameters to set to pass in
	
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EExecuteReset);
	// Set the reason package contents
	iResetReasonPckg() = static_cast<StartupAdaptation::TResetReason>(aReason);
	
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::RequestCoopSysPerformShutdownActions(TInt /*aReason*/, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EExecuteShutdown);
	// No parameters to set to pass in
	
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::RequestCoopSysPerformRfsActions(TSsmRfsType aRfsType, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EExecuteDOSRfs);
	// Set up the reason
	switch(aRfsType)
		{
		case ESsmShallowRfs:
			iRfsReasonPckg() = StartupAdaptation::ENormalRFS;
			break;
		case ESsmDeepRfs:
			iRfsReasonPckg() = StartupAdaptation::EDeepRFS;
			break;
		default:
			// Assume the reason is a custom one
			iRfsReasonPckg() = static_cast<StartupAdaptation::TRFSReason>(aRfsType);
			break;
		}
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::RequestCancel()
	{
	CancelRequest();
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::NotifyCoopSysEvent(TDes8& aEvent, TRequestStatus& aStatus)
	{
	if(iEventStatus != NULL)
		{
		DEBUGPRINT1A("SAA - Multiple notify coop sys event requests detected, completing with KErrInUse");
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	if(iEventQueue.Count() != 0)
		{
		// Complete immediately with an event from the queue
		DEBUGPRINT1A("SAA - Completing notify coop sys event immediately with queued event");
		// Read value from array
		TPckgBuf<TSsmCoopSysEventType> pckgBuf(iEventQueue[0]);
		// Remove value from array
		iEventQueue.Remove(0);
		// Copy descriptor package across
		aEvent.Copy(pckgBuf);
		// Complete status
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrNone);		
		}
	else
		{
		// No events in queue so wait
		aStatus = KRequestPending;
		iEventStatus = &aStatus;
		iEventOutputBuffer = &aEvent;
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::NotifyCancel()
	{
	if(iEventStatus != NULL)
		{
		User::RequestComplete(iEventStatus, KErrCancel);
		iEventStatus = NULL;
		}
	// Clear buffer
	iEventOutputBuffer = NULL;
	}

/*
 * Constructs a new state adaptation object and associates it with aAdapter
 * 
 * @internalComponent
 */
CSaaStateAdaptation::CSaaStateAdaptation(CStartupAdaptationAdapter* aAdapter)
: CAdaptationBase(aAdapter)
	{
	
	}

/**
 * Number of events to pre-allocate in the event queue
 * 
 * @internalComponent
 */
const TInt KEventQueueReserveValue = 8;

/*
 * Second phase of construction 
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::ConstructL()
	{
	// Preallocate event queue some memory
	iEventQueue.ReserveL(KEventQueueReserveValue);
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
void CSaaStateAdaptation::RequestComplete(const StartupAdaptation::TCommand __DEBUG_ONLY(aCommandId), TDesC8& aRetPckg)
	{
	DEBUGPRINT3A("SAA - Response received from adaptation with commandId: %d, expecting %d", aCommandId, CommandId());
	__ASSERT_DEBUG(aCommandId == CommandId(), CLAYER_PANIC(ECLayerUnexpectedCommandResponse));
	switch(CommandId())
		{
		case StartupAdaptation::EGlobalStateChange: // fall through
		case StartupAdaptation::EExecuteSelftests: // fall through
		case StartupAdaptation::EExecuteShutdown: // fall through
		case StartupAdaptation::EExecuteReset: // fall through
		case StartupAdaptation::EExecuteDOSRfs:
			{
				StartupAdaptation::TResponsePckg responsePckg;
				responsePckg.Copy(aRetPckg);
				CompleteRequestStatus(responsePckg());
				break;
			}
		default:
			// Return an error to the client
			CompleteRequestStatus(KErrArgument);
			break;
		}
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
TDesC8* CSaaStateAdaptation::ParameterPckg()
	{
	TDesC8* ptr = NULL;
	switch(CommandId())
		{
		case StartupAdaptation::EGlobalStateChange:
			ptr = &iGlobalStatePckg;
			break;
		case StartupAdaptation::EExecuteSelftests:
			ptr = &iNullBuf;
			break;
		case StartupAdaptation::EExecuteShutdown:
			ptr = &iNullBuf;
			break;
		case StartupAdaptation::EExecuteDOSRfs:
			ptr = &iRfsReasonPckg;
			break;
		case StartupAdaptation::EExecuteReset:
			ptr = &iResetReasonPckg;
			break;
		default:
			ptr = NULL;
			break;
		}
	return ptr;
	}

/**
 * Maps a TSsmState object into the associated StartupAdaptation::TGlobalState value.
 * 
 * Uses the following mappings:
 * 
 * ESWStateStartingUiServices = TSsmState(ESsmStartup, ESWStateStartingUiServices - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical); 
 * ESWStateStartingCriticalApps = TSsmState(ESsmStartup, ESWStateStartingCriticalApps - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);  
 * ESWStateSelfTestOK = TSsmState(ESsmStartup, ESWStateSelfTestOK - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * // States for the security check phase.
 * ESWStateSecurityCheck = TSsmState(ESsmStartup, ESWStateSecurityCheck - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateCriticalPhaseOK = TSsmState(ESsmStartup, ESWStateCriticalPhaseOK - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateEmergencyCallsOnly = TSsmState(ESsmStartup, ESWStateEmergencyCallsOnly - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * // Terminal states defined by the boot mode (and some other variables such as offline mode).
 * ESWStateTest = TSsmState(ESsmStartup, ESWStateTest - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateCharging = TSsmState(ESsmStartup, ESWStateCharging - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateAlarm = TSsmState(ESsmStartup, ESWStateAlarm - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateNormalRfOn = TSsmState(ESsmNormal, ESWStateNormalRfOn - ESWStateStartingUiServices + ESsmStartupSubStateCriticalStatic);
 * ESWStateNormalRfOff = TSsmState(ESsmNormal, ESWStateNormalRfOff - ESWStateStartingUiServices + ESsmStartupSubStateCriticalStatic);
 * ESWStateNormalBTSap = TSsmState(ESsmNormal, ESWStateNormalBTSap - ESWStateStartingUiServices + ESsmStartupSubStateCriticalStatic);
 * // States for notifying adaptation about a terminal state change.
 * ESWStateAlarmToCharging = TSsmState(ESsmStartup, ESWStateAlarmToCharging - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateChargingToAlarm = TSsmState(ESsmStartup, ESWStateChargingToAlarm - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateChargingToNormal = TSsmState(ESsmStartup, ESWStateChargingToNormal - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * ESWStateAlarmToNormal = TSsmState(ESsmStartup, ESWStateAlarmToNormal - ESWStateStartingUiServices + ESsmStartupSubStateNonCritical);
 * // Shutdown-related states.
 * ESWStateShuttingDown = TSsmState(ESsmShutdown, KSsmAnySubState);
 * // Error states during critical phase.
 * ESWStateFatalStartupError = TSsmState(ESsmFail, KSsmAnySubState);
 * 
 * Returns 0 on invalid TSsmState
 * 
 * @internalComponent
 */
StartupAdaptation::TGlobalState CSaaStateAdaptation::MapToStartupAdaptationState(TSsmState aState)
	{
	using namespace StartupAdaptation;
	
	// Calculate the difference from the substate to the value of TGlobalState values
	// This uses ESsmStartupSubStateNonCritical to map to the correct part of the staged
	// start-up architecture and therefore to map the states as described in the comment
	// for this method.
	const TInt KStartupSubStateAdjustment = static_cast<TInt>(ESWStateStartingUiServices) 
											- static_cast<TInt>(ESsmStartupSubStateNonCritical);
	// Adjust the substate value
	const TInt adjustedStartupSubStateValue = aState.SubState() + KStartupSubStateAdjustment;
	
	// Calculate the difference from the substate to the value of TGlobalState values
	// This uses ESsmStartupSubStateCriticalStatic to map to the correct states as described
	// in the comment for this method.
	const TInt KNormalSubStateAdjustment = static_cast<TInt>(ESWStateStartingUiServices) 
	 										- static_cast<TInt>(ESsmStartupSubStateCriticalStatic);

	// Adjust the substate value
	const TInt adjustedNormalSubStateValue = aState.SubState() + KNormalSubStateAdjustment;
	
	TGlobalState ret = static_cast<TGlobalState>(0);
	
	if(aState.MainState() == ESsmStartup)
		{
		ret = static_cast<TGlobalState>(adjustedStartupSubStateValue);
		}
	else if(aState.MainState() == ESsmNormal)
		{
		ret = static_cast<TGlobalState>(adjustedNormalSubStateValue);
		}
	else if(aState.MainState() == ESsmShutdown)
		{
		ret = ESWStateShuttingDown;
		}
	else if(aState.MainState() == ESsmFail)
		{
		ret = ESWStateFatalStartupError;
		}
	
	// Validate the return value is correct
	switch(aState.MainState())
		{
		case ESsmStartup:
			if(ret < ESWStateStartingUiServices || 
					(ret > ESWStateAlarm && ret < ESWStateAlarmToCharging) || 
					ret > ESWStateAlarmToNormal)
				{
				CLAYER_PANIC(ECLayerInvalidSubState);
				}
			break;
		case ESsmNormal:
			if(ret < ESWStateNormalRfOn || ret > ESWStateNormalBTSap)
				{
				CLAYER_PANIC(ECLayerInvalidSubState);
				}
			break;
		case ESsmShutdown:
			if(aState.SubState() != KSsmAnySubState)
				{
				CLAYER_PANIC(ECLayerInvalidSubState);
				}
			break;
		case ESsmFail:
			if(aState.SubState() != KSsmAnySubState)
				{
				CLAYER_PANIC(ECLayerInvalidSubState);
				}
			break;
		default:
			// Do nothing
			break;
		}
	return ret;
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
 * Processes the event passed in and distributes to waiting client or queues event.
 * 
 * @internalComponent
 */
void CSaaStateAdaptation::ProcessEventL(TSsmCoopSysEventType aEventType)
	{
	DEBUGPRINT2A("SAA - State adaptation processing event with type: %d", aEventType);
	if(iEventStatus == NULL)
		{
		// Need to queue event as nothing is waiting to be notified
		iEventQueue.AppendL(aEventType);
		}
	else
		{
		// Client waiting for event
		__ASSERT_DEBUG(iEventOutputBuffer != NULL, CLAYER_PANIC(ECLayerNullStateEventBuffer));
		TPckgBuf<TSsmCoopSysEventType> pckgBuf(aEventType);
		// Copy event across
		iEventOutputBuffer->Copy(pckgBuf);
		// Complete status
		User::RequestComplete(iEventStatus, KErrNone);
		// Clear event status and buffer
		iEventOutputBuffer = NULL;
		iEventStatus = NULL;
		}
	}
