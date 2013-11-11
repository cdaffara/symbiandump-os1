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

#include "tcallcontroltsytesthelper.h" 
#include "cctsytestlogging.h"



/**
* Constructor
*/	
TCallControlTsyTestHelper::TCallControlTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RCall::NotifyCapsChange
*/	
void TCallControlTsyTestHelper::WaitForCallNotifyCapsChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				RCall::TCaps& aCaps, 
				RCall::TCaps aWantedCaps,
				RCall::TCaps aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RCall::NotifyCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps.iFlags & aWantedCaps.iFlags) != aWantedCaps.iFlags)
								 || ((aCaps.iFlags & aUnwantedCaps.iFlags) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aCall.NotifyCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RCall::NotifyCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps.iFlags, aWantedCaps.iFlags, aUnwantedCaps.iFlags, 
				_L("RCall::NotifyCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RCall::NotifyCallDurationChange
*/	
void TCallControlTsyTestHelper::WaitForCallNotifyCallDurationChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				TTimeIntervalSeconds& aTime, 
				TTimeIntervalSeconds aExpectedTime,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RCall::NotifyCallDurationChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aTime != aExpectedTime )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aCall.NotifyCallDurationChange(aRequestStatus, aTime);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RCall::NotifyCallDurationChange Wrong completion status"))	
	ASSERT_EQUALS(aTime.Int(), aExpectedTime.Int(), 
				_L("RCall::NotifyCallDurationChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RCall::NotifyHookChange
*/	
void TCallControlTsyTestHelper::WaitForCallNotifyHookChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				RCall::THookStatus& aHookStatus, 
				RCall::THookStatus aExpectedHookStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RCall::NotifyHookChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aHookStatus != aExpectedHookStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aCall.NotifyHookChange(aRequestStatus, aHookStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RCall::NotifyHookChange Wrong completion status"))	
	ASSERT_EQUALS(aHookStatus, aExpectedHookStatus, 
				_L("RCall::NotifyHookChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RCall::NotifyStatusChange
*/	
void TCallControlTsyTestHelper::WaitForCallNotifyStatusChange( 
				RCall& aCall,
				TEtelRequestBase& aRequestStatus,
				RCall::TStatus& aCallStatus, 
				RCall::TStatus& aExpectedCallStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RCall::NotifyStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCallStatus != aExpectedCallStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aCall.NotifyStatusChange(aRequestStatus, aCallStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RCall::NotifyStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aCallStatus, aExpectedCallStatus, 
				_L("RCall::NotifyStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RLine::NotifyCapsChange
*/	
void TCallControlTsyTestHelper::WaitForLineNotifyCapsChange( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				RLine::TCaps& aCaps, 
				RLine::TCaps aWantedCaps,
				RLine::TCaps aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RLine::NotifyCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps.iFlags & aWantedCaps.iFlags) != aWantedCaps.iFlags)
								 || ((aCaps.iFlags & aUnwantedCaps.iFlags) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aLine.NotifyCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RLine::NotifyCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps.iFlags, aWantedCaps.iFlags, aUnwantedCaps.iFlags, 
				_L("RLine::NotifyCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RLine::NotifyCallAdded
*/	
void TCallControlTsyTestHelper::WaitForLineNotifyCallAdded( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				TName& aName, 
				TName aExpectedName,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RLine::NotifyCallAdded did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aName != aExpectedName )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aLine.NotifyCallAdded(aRequestStatus, aName);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RLine::NotifyCallAdded Wrong completion status"))	
	ASSERT_EQUALS_DES16(aName, aExpectedName, 
				_L("RLine::NotifyCallAdded Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RLine::NotifyHookChange
*/	
void TCallControlTsyTestHelper::WaitForLineNotifyHookChange( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				RCall::THookStatus& aHookStatus, 
				RCall::THookStatus aExpectedHookStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RLine::NotifyHookChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aHookStatus != aExpectedHookStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aLine.NotifyHookChange(aRequestStatus, aHookStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RLine::NotifyHookChange Wrong completion status"))	
	ASSERT_EQUALS(aHookStatus, aExpectedHookStatus, 
				_L("RLine::NotifyHookChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RLine::NotifyStatusChange
*/	
void TCallControlTsyTestHelper::WaitForLineNotifyStatusChange( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				RCall::TStatus& aLineStatus, 
				RCall::TStatus& aExpectedLineStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RLine::NotifyStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aLineStatus != aExpectedLineStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aLine.NotifyStatusChange(aRequestStatus, aLineStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RLine::NotifyStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aLineStatus, aExpectedLineStatus, 
				_L("RLine::NotifyStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RLine::NotifyIncomingCall
*/	
void TCallControlTsyTestHelper::WaitForLineNotifyIncomingCall( 
				RLine& aLine,
				TEtelRequestBase& aRequestStatus,
				TName& aName, 
				TName aExpectedName,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RLine::NotifyIncomingCall did not complete"));
	
	DEBUG_PRINTF2(_L("Name returned is: %S"), &aName);
		
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aName != aExpectedName )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aLine.NotifyIncomingCall(aRequestStatus, aName);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RLine::NotifyIncomingCall Wrong completion status"))	
	ASSERT_EQUALS_DES16(aName, aExpectedName, 
				_L("RLine::NotifyIncomingCall Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPhone::NotifyCapsChange
*/	
void TCallControlTsyTestHelper::WaitForPhoneNotifyCapsChange( 
				RPhone& aPhone,
				TEtelRequestBase& aRequestStatus,
				RPhone::TCaps& aCaps, 
				RPhone::TCaps aWantedCaps,
				RPhone::TCaps aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPhone::NotifyCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps.iFlags & aWantedCaps.iFlags) != aWantedCaps.iFlags)
								 || ((aCaps.iFlags & aUnwantedCaps.iFlags) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aPhone.NotifyCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RPhone::NotifyCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps.iFlags, aWantedCaps.iFlags, aUnwantedCaps.iFlags, 
				_L("RPhone::NotifyCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPhone::NotifyModemDetected
*/	
void TCallControlTsyTestHelper::WaitForPhoneNotifyModemDetected( 
				RPhone& aPhone,
				TEtelRequestBase& aRequestStatus,
				RPhone::TModemDetection& aDetection, 
				RPhone::TModemDetection aExpectedDetection,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPhone::NotifyModemDetected did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aDetection != aExpectedDetection )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPhone.NotifyModemDetected(aRequestStatus, aDetection);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPhone::NotifyModemDetected Wrong completion status"))	
	ASSERT_EQUALS(aDetection, aExpectedDetection, 
				_L("RPhone::NotifyModemDetected Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyAlternatingCallSwitch
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyAlternatingCallSwitch( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyAlternatingCallSwitch did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyAlternatingCallSwitch Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyAudioToneEvent
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyAudioToneEvent( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TAudioToneV3> &aAudioToneV3,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyAudioToneEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aAudioToneV3.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileCall::TAudioToneV3Pckg pkgParam( aAudioToneV3.GetValue() );
		aMobileCall.NotifyAudioToneEvent(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyAudioToneEvent Wrong completion status"))	
	ASSERT_TRUE(aAudioToneV3.IsEqual(ELogError), 
				_L("RMobileCall::NotifyAudioToneEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyCallEvent
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyCallEvent( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallEvent& aEvent, 
				RMobileCall::TMobileCallEvent aExpectedEvent,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyCallEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEvent != aExpectedEvent )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileCall.NotifyCallEvent(aRequestStatus, aEvent);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyCallEvent Wrong completion status"))	
	ASSERT_EQUALS(aEvent, aExpectedEvent, 
				_L("RMobileCall::NotifyCallEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyHscsdInfoChange
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyHscsdInfoChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TMobileCallHscsdInfoV1> &aMobileCallHscsdInfoV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyHscsdInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobileCallHscsdInfoV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileCall::TMobileCallHscsdInfoV1Pckg pkgParam( aMobileCallHscsdInfoV1.GetValue() );
		aMobileCall.NotifyHscsdInfoChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyHscsdInfoChange Wrong completion status"))	
	ASSERT_TRUE(aMobileCallHscsdInfoV1.IsEqual(ELogError), 
				_L("RMobileCall::NotifyHscsdInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyMobileCallCapsChange
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyMobileCallCapsChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpRMobileCallTMobileCallCaps &aMobileCallCapsV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyMobileCallCapsChange did not complete"))

	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobileCallCapsV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileCall::TMobileCallCapsV1Pckg pkgParam( aMobileCallCapsV1.GetValue() );
		aMobileCall.NotifyMobileCallCapsChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyMobileCallCapsChange Wrong completion status"))	
	ASSERT_TRUE(aMobileCallCapsV1.IsEqual(ELogError), 
				_L("RMobileCall::NotifyMobileCallCapsChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyMobileCallStatusChange
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyMobileCallStatusChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallStatus& aStatus, 
				RMobileCall::TMobileCallStatus aExpectedStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyMobileCallStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aStatus != aExpectedStatus )
		{

		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileCall.NotifyMobileCallStatusChange(aRequestStatus, aStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeLong);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyMobileCallStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aStatus, aExpectedStatus, 
				_L("RMobileCall::NotifyMobileCallStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyMobileDataCallCapsChange
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyMobileDataCallCapsChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TMobileCallDataCapsV1> &aMobileCallDataCapsV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyMobileDataCallCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobileCallDataCapsV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileCall::TMobileCallDataCapsV1Pckg pkgParam( aMobileCallDataCapsV1.GetValue() );
		aMobileCall.NotifyMobileDataCallCapsChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyMobileDataCallCapsChange Wrong completion status"))	
	ASSERT_TRUE(aMobileCallDataCapsV1.IsEqual(ELogError), 
				_L("RMobileCall::NotifyMobileDataCallCapsChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyUUSCapsChange
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyUUSCapsChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyUUSCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobileCall.NotifyUUSCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobileCall::NotifyUUSCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobileCall::NotifyUUSCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyVoiceFallback
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyVoiceFallback( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TName& aCallName, 
				TName aExpectedCallName,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyVoiceFallback did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCallName != aExpectedCallName )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileCall.NotifyVoiceFallback(aRequestStatus, aCallName);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyVoiceFallback Wrong completion status"))	
	ASSERT_EQUALS_DES16(aCallName, aExpectedCallName, 
				_L("RMobileCall::NotifyVoiceFallback Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyPrivacyConfirmation
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyPrivacyConfirmation( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhonePrivacy& aPrivacySetting, 
				RMobilePhone::TMobilePhonePrivacy aExpectedPrivacySetting,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyPrivacyConfirmation did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aPrivacySetting != aExpectedPrivacySetting )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileCall.NotifyPrivacyConfirmation(aRequestStatus, aPrivacySetting);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyPrivacyConfirmation Wrong completion status"))	
	ASSERT_EQUALS(aPrivacySetting, aExpectedPrivacySetting, 
				_L("RMobileCall::NotifyPrivacyConfirmation Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyTrafficChannelConfirmation
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyTrafficChannelConfirmation( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallTch& aTchType, 
				RMobileCall::TMobileCallTch aExpectedTchType,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyTrafficChannelConfirmation did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aTchType != aExpectedTchType )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileCall.NotifyTrafficChannelConfirmation(aRequestStatus, aTchType);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyTrafficChannelConfirmation Wrong completion status"))	
	ASSERT_EQUALS(aTchType, aExpectedTchType, 
				_L("RMobileCall::NotifyTrafficChannelConfirmation Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileCall::NotifyRemotePartyInfoChange
*/	
void TCallControlTsyTestHelper::WaitForMobileCallNotifyRemotePartyInfoChange( 
				RMobileCall& aMobileCall,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileCall::TMobileCallRemotePartyInfoV1> &aMobileCallRemotePartyInfoV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileCall::NotifyRemotePartyInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobileCallRemotePartyInfoV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileCall::TMobileCallRemotePartyInfoV1Pckg pkgParam( aMobileCallRemotePartyInfoV1.GetValue() );
		aMobileCall.NotifyRemotePartyInfoChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileCall::NotifyRemotePartyInfoChange Wrong completion status"))	
	ASSERT_TRUE(aMobileCallRemotePartyInfoV1.IsEqual(ELogError), 
				_L("RMobileCall::NotifyRemotePartyInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}


		
/**
* Notify reset for RMobileConferenceCall::NotifyCapsChange
*/	
void TCallControlTsyTestHelper::WaitForMobileConferenceCallNotifyCapsChange( 
				RMobileConferenceCall& aMobileConferenceCall,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	
	TEST_FRAMEWORK_LOG2(_L("RMobileConferenceCall::NotifyCapsChange looking for caps %08X"),
			aWantedCaps	);
	
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileConferenceCall::NotifyCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		TEST_FRAMEWORK_LOG2(_L("TCallControlTsyTestHelper::WaitForMobileConferenceCallNotifyCapsChange got caps %08X"),
				aCaps );
		
		aMobileConferenceCall.NotifyCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobileConferenceCall::NotifyCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobileConferenceCall::NotifyCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileConferenceCall::NotifyConferenceEvent
*/	
void TCallControlTsyTestHelper::WaitForMobileConferenceCallNotifyConferenceEvent( 
				RMobileConferenceCall& aMobileConferenceCall,
				TEtelRequestBase& aRequestStatus,
				RMobileConferenceCall::TMobileConferenceEvent& aEvent, 
				RMobileConferenceCall::TMobileConferenceEvent aExpectedEvent,
				TName &aCallName,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileConferenceCall::NotifyConferenceEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEvent != aExpectedEvent )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		TEST_FRAMEWORK_LOG4(_L("TCallControlTsyTestHelper::WaitForMobileConferenceCallNotifyConferenceEvent looking for %d got %d name %S"),
									aExpectedEvent, aEvent, &aCallName );
				
		aMobileConferenceCall.NotifyConferenceEvent(aRequestStatus, aEvent, aCallName);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileConferenceCall::NotifyConferenceEvent Wrong completion status"))	
	ASSERT_EQUALS(aEvent, aExpectedEvent, 
				_L("RMobileConferenceCall::NotifyConferenceEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileConferenceCall::NotifyConferenceStatusChange
*/	
void TCallControlTsyTestHelper::WaitForMobileConferenceCallNotifyConferenceStatusChange( 
				RMobileConferenceCall& aMobileConferenceCall,
				TEtelRequestBase& aRequestStatus,
				RMobileConferenceCall::TMobileConferenceStatus& aStatus, 
				RMobileConferenceCall::TMobileConferenceStatus aExpectedStatus,
				TInt aWantedStatus )

	{
	TEST_FRAMEWORK_LOG2(_L("RMobileConferenceCall::NotifyConferenceStatusChange looking for %d"),
			aExpectedStatus	);
	
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileConferenceCall::NotifyConferenceStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aStatus != aExpectedStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		TEST_FRAMEWORK_LOG2(_L("RMobileConferenceCall::NotifyConferenceStatusChange intermediate %d"),
								aStatus	);
		aMobileConferenceCall.NotifyConferenceStatusChange(aRequestStatus, aStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileConferenceCall::NotifyConferenceStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aStatus, aExpectedStatus, 
				_L("RMobileConferenceCall::NotifyConferenceStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileLine::NotifyMobileLineStatusChange
*/	
void TCallControlTsyTestHelper::WaitForMobileLineNotifyMobileLineStatusChange( 
				RMobileLine& aMobileLine,
				TEtelRequestBase& aRequestStatus,
				RMobileCall::TMobileCallStatus& aStatus, 
				RMobileCall::TMobileCallStatus aExpectedStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileLine::NotifyMobileLineStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aStatus != aExpectedStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileLine.NotifyMobileLineStatusChange(aRequestStatus, aStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileLine::NotifyMobileLineStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aStatus, aExpectedStatus, 
				_L("RMobileLine::NotifyMobileLineStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyALSLineChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyALSLineChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneALSLine& aALSLine, 
				RMobilePhone::TMobilePhoneALSLine aExpectedALSLine,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyALSLineChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aALSLine != aExpectedALSLine )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyALSLineChange(aRequestStatus, aALSLine);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyALSLineChange Wrong completion status"))	
	ASSERT_EQUALS(aALSLine, aExpectedALSLine, 
				_L("RMobilePhone::NotifyALSLineChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyAlternatingCallCapsChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyAlternatingCallCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyAlternatingCallCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobilePhone.NotifyAlternatingCallCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobilePhone::NotifyAlternatingCallCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobilePhone::NotifyAlternatingCallCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyAlternatingCallModeChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyAlternatingCallModeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneAlternatingCallMode& aMode, 
				RMobilePhone::TMobilePhoneAlternatingCallMode aExpectedMode,
				RMobilePhone::TMobileService &aFirstService,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyAlternatingCallModeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aMode != aExpectedMode )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyAlternatingCallModeChange(aRequestStatus, aMode, aFirstService);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyAlternatingCallModeChange Wrong completion status"))	
	ASSERT_EQUALS(aMode, aExpectedMode, 
				_L("RMobilePhone::NotifyAlternatingCallModeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyDTMFCapsChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyDTMFCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyDTMFCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobilePhone.NotifyDTMFCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobilePhone::NotifyDTMFCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobilePhone::NotifyDTMFCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyDTMFEvent
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyDTMFEvent( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneDTMFEvent& aEvent, 
				RMobilePhone::TMobilePhoneDTMFEvent& aExpectedEvent,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyDTMFEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEvent != aExpectedEvent )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyDTMFEvent(aRequestStatus, aEvent);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyDTMFEvent Wrong completion status"))	
	ASSERT_EQUALS(aEvent, aExpectedEvent, 
				_L("RMobilePhone::NotifyDTMFEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyFdnStatusChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyFdnStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneFdnStatus& aFdnStatus, 
				RMobilePhone::TMobilePhoneFdnStatus aExpectedFdnStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyFdnStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aFdnStatus != aExpectedFdnStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyFdnStatusChange(aRequestStatus, aFdnStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyFdnStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aFdnStatus, aExpectedFdnStatus, 
				_L("RMobilePhone::NotifyFdnStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyIncomingCallTypeChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyIncomingCallTypeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneIncomingCallType &aCallType,
				TCmpBase<RMobileCall::TMobileDataCallParamsV1> &aMobileDataCallParamsV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyIncomingCallTypeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobileDataCallParamsV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileCall::TMobileDataCallParamsV1Pckg pkgParam( aMobileDataCallParamsV1.GetValue() );
		aMobilePhone.NotifyIncomingCallTypeChange(aRequestStatus, aCallType, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyIncomingCallTypeChange Wrong completion status"))	
	ASSERT_TRUE(aMobileDataCallParamsV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyIncomingCallTypeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyIndicatorChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyIndicatorChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aIndicator, 
				TUint32 aExpectedIndicator,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyIndicatorChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aIndicator != aExpectedIndicator )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyIndicatorChange(aRequestStatus, aIndicator);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyIndicatorChange Wrong completion status"))	
	ASSERT_EQUALS(aIndicator, aExpectedIndicator, 
				_L("RMobilePhone::NotifyIndicatorChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMessageWaiting
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyMessageWaiting( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TInt& aCount, 
				TInt aExpectedCount,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMessageWaiting did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCount != aExpectedCount )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyMessageWaiting(aRequestStatus, aCount);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMessageWaiting Wrong completion status"))	
	ASSERT_EQUALS(aCount, aExpectedCount, 
				_L("RMobilePhone::NotifyMessageWaiting Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMulticallIndicatorChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyMulticallIndicatorChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneMulticallIndicator& aMulticallIndicator, 
				RMobilePhone::TMobilePhoneMulticallIndicator aExpectedMulticallIndicator,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMulticallIndicatorChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aMulticallIndicator != aExpectedMulticallIndicator )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyMulticallIndicatorChange(aRequestStatus, aMulticallIndicator);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMulticallIndicatorChange Wrong completion status"))	
	ASSERT_EQUALS(aMulticallIndicator, aExpectedMulticallIndicator, 
				_L("RMobilePhone::NotifyMulticallIndicatorChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMulticallParamsChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyMulticallParamsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneMulticallSettingsV1> &aMobilePhoneMulticallSettingsV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMulticallParamsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneMulticallSettingsV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneMulticallSettingsV1Pckg pkgParam( aMobilePhoneMulticallSettingsV1.GetValue() );
		aMobilePhone.NotifyMulticallParamsChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMulticallParamsChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneMulticallSettingsV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyMulticallParamsChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMultimediaCallPreferenceChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyMultimediaCallPreferenceChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneMultimediaSettings& aMmSettings, 
				RMobilePhone::TMobilePhoneMultimediaSettings aExpectedMmSettings,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMultimediaCallPreferenceChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aMmSettings != aExpectedMmSettings )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyMultimediaCallPreferenceChange(aRequestStatus, aMmSettings);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMultimediaCallPreferenceChange Wrong completion status"))	
	ASSERT_EQUALS(aMmSettings, aExpectedMmSettings, 
				_L("RMobilePhone::NotifyMultimediaCallPreferenceChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyStopInDTMFString
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyStopInDTMFString( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyStopInDTMFString did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyStopInDTMFString Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyAirTimeDurationChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyAirTimeDurationChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TTimeIntervalSeconds& aTime, 
				TTimeIntervalSeconds aExpectedTime,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyAirTimeDurationChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aTime != aExpectedTime )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyAirTimeDurationChange(aRequestStatus, aTime);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyAirTimeDurationChange Wrong completion status"))	
	ASSERT_EQUALS(aTime.Int(), aExpectedTime.Int(), 
				_L("RMobilePhone::NotifyAirTimeDurationChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCostInfoChange
*/	
void TCallControlTsyTestHelper::WaitForMobilePhoneNotifyCostInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCostInfoV1> &aMobilePhoneCostInfoV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCostInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneCostInfoV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneCostInfoV1Pckg pkgParam( aMobilePhoneCostInfoV1.GetValue() );
		aMobilePhone.NotifyCostInfoChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCostInfoChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneCostInfoV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCostInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

/**
* Check the valid variables returned by RMobileCall::GetMobileCallInfo including the phone number
*/	
void TCallControlTsyTestHelper::CheckForValidCallInfo(RMobileCall::TMobileCallInfoV1& aCallInfo, TPtrC aNumber)	
{
	CheckForValidCallInfo(aCallInfo);
	if(aCallInfo.iValid & RMobileCall::KCallDialledParty)
		{
		ASSERT_TRUE( aCallInfo.iDialledParty.iTelNumber == aNumber, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallId to true, but set iCallId to an invalid number"))
		}	
}

/**
* Check the valid variables returned by RMobileCall::GetMobileCallInfo, when phone number is unknown
*/	
void TCallControlTsyTestHelper::CheckForValidCallInfo(RMobileCall::TMobileCallInfoV1& aCallInfo, TInt aRequiredExitCode, TBool aIsCallConnected)	
{
	if(aCallInfo.iValid & RMobileCall::KCallStartTime)
		{
		TTime invalidTime = 0;
		TTime startTime(aCallInfo.iStartTime);
		if(aIsCallConnected)
		    {
            ASSERT_TRUE( startTime > invalidTime, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallStartTime to true, but set iStartTime to an invalid number"));
		    }
		else
		    {
		    ASSERT_EQUALS( startTime.Int64(), invalidTime.Int64(), _L("RMobileCall::GetMobileCallInfo set bitmask for KCallStartTime to true, but set iStartTime to an invalid invalid time"));
		    }
		}
	
	if(aCallInfo.iValid & RMobileCall::KCallDuration)
		{
		TTimeIntervalSeconds invalidTime = 0;
		TTimeIntervalSeconds callDuration(aCallInfo.iDuration);
		if(aIsCallConnected)
		    {
		    ASSERT_TRUE( callDuration > invalidTime, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallDuration to true, but set iDuration to an invalid number"));
		    }
		else
		    {
		    // Note: This value contains previous call duration since it is not connected, can be 0 or bigger than 0. 
            ASSERT_TRUE( callDuration >= invalidTime, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallDuration to true, but set iDuration to an invalid number"));
		    }
		}

	if(aCallInfo.iValid & RMobileCall::KCallId)
		{
		if(aIsCallConnected)
		    {
            ASSERT_TRUE( aCallInfo.iCallId > -1, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallId to true, but set iCallId to an invalid number"))
		    }
		else
		    {
            ASSERT_EQUALS( aCallInfo.iCallId, -1, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallId to true, but set iCallId to an invalid number"))
		    }
		}

	if(aCallInfo.iValid & RMobileCall::KCallRemoteParty)
		{
		ASSERT_TRUE( aCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Length() > 0, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallRemoteParty to true, but set iRemoteParty.iRemoteNumber.iTelNumber.Length() to an invalid length"))
		}

	if(aCallInfo.iValid & RMobileCall::KCallExitCode)
		{
        // Note: iExitCode contains previous call result if it is not connected. 
		ASSERT_EQUALS( aCallInfo.iExitCode, aRequiredExitCode, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallExitCode to true, but did not set iExitCode to that expected."))
		}

	if(aCallInfo.iValid & RMobileCall::KCallEmergency)
		{
		ASSERT_EQUALS( aCallInfo.iEmergency, (TBool)EFalse, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallEmergency to true, but did not set iEmergency to EFalse, even though this is not an emergency number"))
		}				

	if(aCallInfo.iValid & RMobileCall::KCallForwarded)
		{
		ASSERT_EQUALS( aCallInfo.iForwarded, (TBool)EFalse, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallForwarded to true, but did not set iForwarded to EFalse, even though this is not an Forwarded call"))
		}		

	if(aCallInfo.iValid & RMobileCall::KCallPrivacy)
		{
		ASSERT_TRUE( aCallInfo.iPrivacy != RMobilePhone::EPrivacyUnspecified, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallPrivacy to true, but did not set to an expected number"))
		}	

	if(aCallInfo.iValid & RMobileCall::KCallTch)
		{
		ASSERT_TRUE( (aCallInfo.iTch != RMobileCall::ETchUnknown) , _L("RMobileCall::GetMobileCallInfo set bitmask for KCallTch to true, but iTch is still unknown"))
		}	

    // CTSY initialises iAlternatingCall to EAlternatingModeUnspecified, which should be updated from LTSY.
    // ( IPC is EMobileCallGetMobileCallInfo and method is CompleteNotifyMobileCallInfoChange().
	if(aCallInfo.iValid & RMobileCall::KCallAlternating)
		{
		if(aIsCallConnected)
		    {
		    // Because LTSY is not updating this correctly, check is changed from !=EAlternatingModeUnspecified to ==EAlternatingModeUnspecified
		    ASSERT_EQUALS(aCallInfo.iAlternatingCall, RMobilePhone::EAlternatingModeUnspecified, _L("RMobileCall::GetMobileCallInfo set bitmask for KCallAlternating to true, but iAlternatingCall is not EAlternatingModeUnspecified"))
		    }
		else
		    {
		    ASSERT_EQUALS(aCallInfo.iAlternatingCall, RMobilePhone::EAlternatingModeUnspecified , _L("RMobileCall::GetMobileCallInfo set bitmask for KCallAlternating to true, but iAlternatingCall is not initialized EAlternatingModeUnspecified"));
		    }
		}

}

