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
// SIGNALSTRENGHWATCHER.CPP
// 
//

// User includes


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "signalstrengthwatcherTraces.h"
#endif

#include "signalstrengthwatcher.h"

// System includes
#include <sacls.h>

 #include <commsdat.h>

// In dBm
const TInt32 KSignalStrengthHigh = -51;
const TInt32 KSignalStrengthMedium = -77;
const TInt32 KSignalStrengthNone = -113;
const TInt8  KSignalZeroBars = 0;
const TInt8  KSignalThreeBars = 3;
const TInt8  KSignalFiveBars = 5;

//
// ------> Global exports
//


//
// ------> CSignalStrengthWatcher (source)
//

CSignalStrengthWatcher::CSignalStrengthWatcher()
:	CPhoneWatcher()
	{
	}

CSignalStrengthWatcher* CSignalStrengthWatcher::NewL(TAny* /*aWatcherParams*/)
	{
	CSignalStrengthWatcher* self= new (ELeave) CSignalStrengthWatcher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}
	
void CSignalStrengthWatcher::ConstructL()
{
	CWatcherBase::ConstructL();
		
	User::LeaveIfError(iNetworkStrengthProperty.Attach(KUidSystemCategory, KUidNetworkStrength.iUid));
}

CSignalStrengthWatcher::~CSignalStrengthWatcher()
	{
	Cancel();
	iNetworkStrengthProperty.Close();
	}

//
//
//

void CSignalStrengthWatcher::HandlePhoneStateEventL(TInt aCompletionCode)
	{
	switch(SignalStrengthState())
		{
	case ESignalStrengthNotYetInitialised:
	case ESignalStrengthStateRequestInitialSignalStrength:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLEPHONESTATEEVENTL_1, "SignalStrengthWatcher : Requesting initial signal strength");
		Phone().GetSignalStrength(iStatus, iSignalStrength, iSignalBars);
		SignalStrengthState() = ESignalStrengthWaitingForInitialSignalStrength;
		SetActive();
		break;

	case ESignalStrengthWaitingForInitialSignalStrength:
		SignalStrengthState() = ESignalStrengthStateIssuingSignalChangeNotificationRequest;
		HandleSignalStrengthUpdateL(aCompletionCode);
		break;

	case ESignalStrengthStateIssuingSignalChangeNotificationRequest:
		HandleSignalStrengthUpdateL(aCompletionCode);
		break;

	default:
		__ASSERT_DEBUG(0, SignalPanic(EUnexpectedState));
		}
	}

void CSignalStrengthWatcher::HandleCancel()
	{
	if	(Phone().SubSessionHandle() == KNullHandle)
		return;

	if	(SignalStrengthState() == ESignalStrengthWaitingForInitialSignalStrength)
		Phone().CancelAsyncRequest(EMobilePhoneGetSignalStrength);
	else if (SignalStrengthState() == ESignalStrengthStateIssuingSignalChangeNotificationRequest)
		Phone().CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
	}

void CSignalStrengthWatcher::ReleasePhoneResources()
//
//	Called by the phone watcher base class. Release any telephony related
//	resources and reset and state.
//
	{
	// This is only called within RunL and therefore we can't be active
	__ASSERT_DEBUG(!IsActive(), SignalPanic(EUnexpectedActiveState));

	// Reset state
	iState = ESignalStrengthNotYetInitialised;
	}


//
//
//

void CSignalStrengthWatcher::HandleSignalStrengthUpdateL(TInt aCompletionCode)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLESIGNALSTRENGTHUPDATEL_1, "SignalStrengthNotifier : Handling signal strength update");
	if	(aCompletionCode < KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLESIGNALSTRENGTHUPDATEL_2, "SignalStrengthNotifier : Handling request error (%d)", aCompletionCode);

		// Indicate we don't know what the signal strength is
		TInt ret = iNetworkStrengthProperty.Set(ESANetworkStrengthUnknown);
		if (!(ret == KErrNone || ret == KErrNotFound))
			User::Leave(ret);

		if	(aCompletionCode == KErrNotSupported)
			{
			// If the TSY returns 'Not supported' then it isn't 
			// worth re-sending the request, so give up gracefully.
			SetDisabled(_L("SignalStrengthNotifier : TSY returned not supported (%d)"), aCompletionCode);
			}
		else if	(aCompletionCode == KErrCancel)
			{
			// Signal Strength watcher was cancelled
			SetDisabled(_L("SignalStrengthNotifier : TSY has cancelled request (%d)"), aCompletionCode);
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
			SetDisabled(_L("SignalStrengthNotifier : Max retries reached or exceeded. Shutting down until reboot."), 0);
			}
		else
			{
			// Doing this means we will asynchronously request the signal strength 
			// so that we can update to a real value again.
			SignalStrengthState() = ESignalStrengthNotYetInitialised;
			SuspendFor(KErrorRetryPausePeriod);
			}
		}
	else
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLESIGNALSTRENGTHUPDATEL_3, "SignalStrengthNotifier : Signal strength is: %d", (TInt) iSignalStrength);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLESIGNALSTRENGTHUPDATEL_4, "SignalStrengthNotifier : Signal bars is: %d", iSignalBars);

		// What is the signal strength now?
		TInt newState = ESANetworkStrengthUnknown;
		// iSignalStrength is in dBm - hence use of negative numbers
		if	((iSignalStrength == KSignalStrengthNone) || (iSignalBars == KSignalZeroBars))
			newState = ESANetworkStrengthNone;
		else if ((iSignalStrength < KSignalStrengthMedium) || (iSignalBars < KSignalThreeBars))
			newState = ESANetworkStrengthLow;
		else if ((iSignalStrength < KSignalStrengthHigh) || (iSignalBars < KSignalFiveBars))
			newState = ESANetworkStrengthMedium;
		else
			newState = ESANetworkStrengthHigh;

		// Update properties
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLESIGNALSTRENGTHUPDATEL_5, "SignalStrengthNotifier : Informing properties of signal strength change");
		TInt ret = iNetworkStrengthProperty.Set(newState);
		if (!(ret == KErrNone || ret == KErrNotFound))
			User::Leave(ret);
				
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIGNALSTRENGTHWATCHER_HANDLESIGNALSTRENGTHUPDATEL_6, "SignalStrengthNotifier : Requesting signal strength change notification");
		Phone().NotifySignalStrengthChange(iStatus, iSignalStrength, iSignalBars);
		SetActive();
		}
	}


//
//
//

//
// Panic Function
//
void CSignalStrengthWatcher::SignalPanic(TWatcherPanic aPanicNumber)
	{
	_LIT(panicText,"Signal Strength Watcher");
	User::Panic(panicText,aPanicNumber);
	}










