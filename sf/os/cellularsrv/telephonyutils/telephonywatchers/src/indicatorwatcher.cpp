// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// User includes


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "indicatorwatcherTraces.h"
#endif

#include "indicatorwatcher.h"

// System includes
#include <sacls.h>

 #include <commsdat.h>

//
// ------> Global exports
//


//
// ------> CIndicatorWatcher (source)
//

CIndicatorWatcher::CIndicatorWatcher()
:	CPhoneWatcher()
	{
	}

CIndicatorWatcher* CIndicatorWatcher::NewL(TAny* /*aWatcherParams*/)
	{
	CIndicatorWatcher* self= new (ELeave) CIndicatorWatcher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CIndicatorWatcher::ConstructL()
	{
	CWatcherBase::ConstructL();
	iCallStateWatcher = new (ELeave) CCallStateWatcher(Phone());
	
	/* Attach to properties for minimum access time */
	User::LeaveIfError(iCurrentCallProperty.Attach(KUidSystemCategory, KUidCurrentCall.iUid)); 
	User::LeaveIfError(iChargerStatusProperty.Attach(KUidSystemCategory, KUidChargerStatus.iUid)); 
	User::LeaveIfError(iNetworkStatusProperty.Attach(KUidSystemCategory, KUidNetworkStatus.iUid)); 
	}

CIndicatorWatcher::~CIndicatorWatcher()
	{
	Cancel();
	delete iCallStateWatcher;
	iCurrentCallProperty.Close();
	iChargerStatusProperty.Close();
	iNetworkStatusProperty.Close();
	}

//
//
//

void CIndicatorWatcher::HandlePhoneStateEventL(TInt aCompletionCode)
	{
	switch(IndicatorState())
		{
	case EIndicatorNotYetInitialised:
	case EIndicatorRequestInitialIndicator:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINDICATORWATCHER_HANDLEPHONESTATEEVENTL_1, "IndicatorWatcher : Requesting initial indicator values");
		Phone().GetIndicator(iStatus, iIndicatorInfo);
		IndicatorState() = EIndicatorWaitingForInitialIndicator;
		SetActive();
		break;

	case EIndicatorWaitingForInitialIndicator:
		IndicatorState() = EIndicatorIssuingIndicatorChangeNotification;
		HandleIndicatorUpdateL(aCompletionCode);
		break;

	case EIndicatorIssuingIndicatorChangeNotification:
		HandleIndicatorUpdateL(aCompletionCode);
		break;

	default:
		__ASSERT_DEBUG(0, IndicatorPanic(EUnexpectedState));
		}
	}

void CIndicatorWatcher::HandleCancel()
	{
	if	(Phone().SubSessionHandle() == KNullHandle)
		return;

	if	(IndicatorState() == EIndicatorWaitingForInitialIndicator)
		Phone().CancelAsyncRequest(EMobilePhoneGetIndicator);
	else if (IndicatorState() == EIndicatorIssuingIndicatorChangeNotification)
		{
		Phone().CancelAsyncRequest(EMobilePhoneNotifyIndicatorChange);
		if (iCallStateWatcher)
			iCallStateWatcher->Cancel();
		}
	}

void CIndicatorWatcher::ReleasePhoneResources()
//
//	Called by the phone watcher base class. Release any telephony related
//	resources and reset and state.
//
	{
	// This is only called within RunL and therefore we can't be active
	__ASSERT_DEBUG(!IsActive(), IndicatorPanic(EUnexpectedActiveState));

	// Reset state
	iState = EIndicatorNotYetInitialised;
	}


//
//
//

void CIndicatorWatcher::HandleIndicatorUpdateL(TInt aCompletionCode)
	{
#ifdef OST_TRACE_COMPILER_IN_USE
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINDICATORWATCHER_HANDLEINDICATORUPDATEL_1, "IndicatorWatcher : Handling phone state change with request result (%d)", aCompletionCode);
#else
	(void) aCompletionCode;
#endif

#ifdef WATCHER_TESTING
	// TESTING: 
	{	
	// Get a random number which controls what state we update...
	TInt index;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory,KUidTestProp_CallStateChange.iUid, index));
	switch(index)
		{
	default:
	case 0: // CALL STATE
		{
		TSACurrentCall state = static_cast<TSACurrentCall>(WHelpers::Rand(ESACallNone, ESACallAlternating, TheSeed));
		User::LeaveIfError(iCurrentCallProperty.Set(state));
		break;
		}

	case 1: // BATTERY CHARGER
		{
		TSAChargerStatus state = static_cast<TSAChargerStatus>(WHelpers::Rand(ESAChargerConnected, ESAChargerNotCharging, TheSeed));
		User::LeaveIfError(iChargerStatusProperty.Set(state));
		break;
		}

	case 2: // NETWORK AVAILABILITY
		{
		TSANetworkStatus state = static_cast<TSANetworkStatus>(WHelpers::Rand(ESANetworkAvailable, ESANetworkUnAvailable, TheSeed));
		User::LeaveIfError(iNetworkStatusProperty.Set(state));
		break;
		}
		}

	SuspendFor(10); // seconds
	return;
	}
#else
	if	(aCompletionCode < KErrNone)
		{
		// Indicate we don't know what the indicator status is
		User::LeaveIfError(iCurrentCallProperty.Set(KErrUnknown));
		User::LeaveIfError(iChargerStatusProperty.Set(KErrUnknown));
		User::LeaveIfError(iNetworkStatusProperty.Set(KErrUnknown));

		if	(aCompletionCode == KErrNotSupported)
			{
			// If the TSY returns 'Not supported' then it isn't 
			// worth re-sending the request, so give up gracefully.
			SetDisabled(_L("IndicatorNotifier : TSY returned not supported (%d)"), aCompletionCode);
			}
		else if	(aCompletionCode == KErrCancel)
			{
			// Indicator watcher was cancelled
			SetDisabled(_L("IndicatorNotifier : TSY has cancelled request (%d)"), aCompletionCode);
			}
		else if	(aCompletionCode == KWatcherBaseModemNotDetected)
			{
			// We should release all telephony related resources until the
			// phone is available again.
			Cancel();
			Reset();

			// The modem / phone cannot be found. Wait until it becomes available again...
			WaitForPhoneToPowerUpL();
			}
		else if	(ErrorCountIncrement() >= KErrorRetryCount)
			{
			// We've already tried as many times as possible. Shut ourselves down forever.
			// This watcher will be restarted when the machine is rebooted.
			SetDisabled(_L("IndicatorNotifier : Max retries reached or exceeded. Shutting down until reboot."), 0);
			}
		else
			SuspendFor(KErrorRetryPausePeriod);
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINDICATORWATCHER_HANDLEINDICATORUPDATEL_2, "IndicatorWatcher : Processing successful indicator event");
	
		// Update charger status if there has been a change
		{
		TInt chargerState;
		User::LeaveIfError(iChargerStatusProperty.Get(chargerState));
		
		TInt newChargerState=KErrUnknown;

		if (iIndicatorInfo & RMobilePhone::KIndChargerConnected)
			newChargerState=ESAChargerConnected;
		else
			newChargerState=ESAChargerDisconnected;
	
		if (newChargerState!=chargerState)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINDICATORWATCHER_HANDLEINDICATORUPDATEL_3, "IndicatorWatcher : New Charger State %d", newChargerState);
			User::LeaveIfError(iChargerStatusProperty.Set(newChargerState));
			}
		}

		// Update network available status if there has been a change
		{
		TInt networkState;
		User::LeaveIfError(iNetworkStatusProperty.Get(networkState));		
		TInt newNetworkState=KErrUnknown;

		if (iIndicatorInfo & RMobilePhone::KIndNetworkAvailable)
			newNetworkState=ESANetworkAvailable;
		else
			newNetworkState=ESANetworkUnAvailable;
	
		if (newNetworkState!=networkState)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINDICATORWATCHER_HANDLEINDICATORUPDATEL_4, "IndicatorWatcher : New Network State %d", newNetworkState);
			User::LeaveIfError(iNetworkStatusProperty.Set(newNetworkState));		
			}
		}

		// Update call-in-progress status if there has been a change
		{
		TInt callState;
		User::LeaveIfError(iCurrentCallProperty.Get(callState));
		TInt newCallState=KErrUnknown;

		if ((iIndicatorInfo & RMobilePhone::KIndCallInProgress) && 
			(callState <= ESACallNone))
			{
			newCallState=iCallStateWatcher->StartCallStateMonitor();
			}
		else if (!(iIndicatorInfo & RMobilePhone::KIndCallInProgress))
			{
			iCallStateWatcher->Cancel();
			newCallState=ESACallNone;
			}
	
		if (newCallState!=callState)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CINDICATORWATCHER_HANDLEINDICATORUPDATEL_5, "IndicatorWatcher : New Call State %d", newCallState);
			User::LeaveIfError(iCurrentCallProperty.Set(newCallState));
			}
		}

		// Issue another request
		Phone().NotifyIndicatorChange(iStatus, iIndicatorInfo);
		SetActive();
		}
#endif
	}

//
//
//
//

CCallStateWatcher::CCallStateWatcher(RMobilePhone& aPhone, TInt aPriority)
:	CActive(aPriority), iPhone(aPhone)
	{
	CActiveScheduler::Add(this);
	}

CCallStateWatcher::~CCallStateWatcher()
	{
	Cancel();
	}

TInt CCallStateWatcher::StartCallStateMonitor()
	{
	TInt state=KErrUnknown;
	TInt lineCount = 0;

	// Find the line and call to open permanently
	TInt error = iPhone.EnumerateLines(lineCount);
	if	(error)
		return state;

	for(TInt i = 0; i<lineCount; i++)
		{
		RPhone::TLineInfo lineInfo;
		error = iPhone.GetLineInfo(i, lineInfo);
		if (error)
			break; // and return state unknown

		error = iLine.Open(iPhone,lineInfo.iName);
		if (error)
			break; // and return state unknown
		
		TInt callCount=0;
		error = iLine.EnumerateCall(callCount);
		if (error)
			{
			iLine.Close();
			break;
			}

// There may be more than one call - but this watcher only supports
// monitoring first call found
		RLine::TCallInfo callInfo;
		TBool found=EFalse;
		for(TInt i=0;i<callCount;i++)
			{
			error = iLine.GetCallInfo(i,callInfo);
			if((error==KErrNone)&&
			   (callInfo.iStatus!=RCall::EStatusIdle)&&
			   (callInfo.iStatus!=RCall::EStatusUnknown))
				{
				found=ETrue;
				break;
				}
			}

		if(found)
			{
			error = iCall.OpenExistingCall(iLine, callInfo.iCallName);
			if (error)
				{
				// Found the line to monitor but can't open call
				iLine.Close();
				return state;
				}

			state = GetCallState();

			if (state!=KErrUnknown)
				{
				// Now monitor for further state changes
				iCall.NotifyMobileCallStatusChange(iStatus,iCallState);
				SetActive();
				break;
				}
			else
				{
				iCall.Close();
				iLine.Close();
				return state;
				}

			} // end of if (callCount>0)
		else
			{
			// Close this line and look at next one
			iLine.Close();
			}

		} // end of for loop

	return state;
	}


void CCallStateWatcher::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		TInt callState;
		User::LeaveIfError(RProperty::Get(KUidSystemCategory, KUidCurrentCall.iUid, callState));
		
		TInt newCallState=KErrUnknown;

		newCallState=GetCallState();

		if (newCallState!=callState)
			User::LeaveIfError(RProperty::Set(KUidSystemCategory,KUidCurrentCall.iUid,newCallState));
			
		if (newCallState != KErrUnknown)
			{
			// Now monitor for further state changes
			iCall.NotifyMobileCallStatusChange(iStatus,iCallState);
			SetActive();
			}
		else
			{
			iCall.Close();
			iLine.Close();
			}
		}
	else
		{
		iCall.Close();
		iLine.Close();
		}
	}


void CCallStateWatcher::DoCancel()
	{
	iCall.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);
	iCall.Close();
	iLine.Close();
	}

TInt CCallStateWatcher::RunError(TInt /*aError*/)
	{
	// Release resources
	iCall.Close();
	iLine.Close();
	return KErrNone;
	}

TInt CCallStateWatcher::GetCallState()
	{
	TInt state=KErrUnknown;

	RMobileCall::TMobileCallInfoV1 info;
	RMobileCall::TMobileCallInfoV1Pckg infoPckg(info);

	TInt error = iCall.GetMobileCallInfo(infoPckg);
	if	(error)
		{
		// Found the call to monitor but can't get information
		return state;
		}

	switch (info.iStatus)
		{
	case RMobileCall::EStatusRinging:
		state = ESACallRinging;
		break;
	case RMobileCall::EStatusDialling:
		state = ESACallDialling;
		break;
	case RMobileCall::EStatusConnecting:
		state = ESACallAlerting;
		break;
	case RMobileCall::EStatusAnswering:
		state = ESACallAnswering;
		break;
	case RMobileCall::EStatusDisconnecting:
	case RMobileCall::EStatusDisconnectingWithInband:
		state = ESACallDisconnecting;
		break;
	case RMobileCall::EStatusConnected:
	case RMobileCall::EStatusHold:
	case RMobileCall::EStatusReconnectPending:
		{
		// Call is connected, so update with type of connected call
		if((info.iValid & RMobileCall::KCallAlternating)&&
		   (info.iAlternatingCall != RMobilePhone::EAlternatingModeSingle))
			state = ESACallAlternating;
		else
			{
			switch (info.iService)
				{
			case RMobilePhone::EVoiceService:
			case RMobilePhone::EAuxVoiceService:
				state = ESACallVoice;
				break;
			case RMobilePhone::ECircuitDataService:
				state = ESACallData;
				break;
			case RMobilePhone::EFaxService:
				state = ESACallFax;
				break;
			default:
				// Not a valid call service
				break;
				}
			}
		}
		break;
	default:
		// must be idle state - or special idle state
		state = ESACallNone;
		break;
		}

	return state;
	}

//
//
//

//
// Panic Function
//
void CIndicatorWatcher::IndicatorPanic(TWatcherPanic aPanicNumber)
	{
	_LIT(panicText,"Indicator Watcher");
	User::Panic(panicText,aPanicNumber);
	}







