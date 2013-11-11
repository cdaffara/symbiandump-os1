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

#include "tsupplementaltsytesthelper.h" 



/**
* Constructor
*/	
TSupplementalTsyTestHelper::TSupplementalTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobilePhone::NotifyCCBSRecall
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCCBSRecall( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCCBSEntryV1> &aMobilePhoneCCBSEntryV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCCBSRecall did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneCCBSEntryV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneCCBSEntryV1Pckg pkgParam( aMobilePhoneCCBSEntryV1.GetValue() );
		aMobilePhone.NotifyCCBSRecall(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCCBSRecall Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneCCBSEntryV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCCBSRecall Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCCBSRecall
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCCBSRecall( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCCBSEntryV2> &aMobilePhoneCCBSEntryV2,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCCBSRecall did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneCCBSEntryV2.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneCCBSEntryV2Pckg pkgParam( aMobilePhoneCCBSEntryV2.GetValue() );
		aMobilePhone.NotifyCCBSRecall(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCCBSRecall Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneCCBSEntryV2.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCCBSRecall Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCCBSStatusChange
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCCBSStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneCCBSStatus& aCcbsStatus, 
				RMobilePhone::TMobilePhoneCCBSStatus aExpectedCcbsStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCCBSStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCcbsStatus != aExpectedCcbsStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyCCBSStatusChange(aRequestStatus, aCcbsStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCCBSStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aCcbsStatus, aExpectedCcbsStatus, 
				_L("RMobilePhone::NotifyCCBSStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCallBarringStatusChange
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCallBarringStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneCBCondition& aCondition, 
				RMobilePhone::TMobilePhoneCBCondition aExpectedCondition,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCondition != aExpectedCondition )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyCallBarringStatusChange(aRequestStatus, aCondition);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCallBarringStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aCondition, aExpectedCondition, 
				_L("RMobilePhone::NotifyCallBarringStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCallForwardingActive
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCallForwardingActive( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobileService& aServiceGroup, 
				RMobilePhone::TMobileService aExpectedServiceGroup,
				RMobilePhone::TMobilePhoneCFActive &aActiveType,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCallForwardingActive did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aServiceGroup != aExpectedServiceGroup )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyCallForwardingActive(aRequestStatus, aServiceGroup, aActiveType);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCallForwardingActive Wrong completion status"))	
	ASSERT_EQUALS(aServiceGroup, aExpectedServiceGroup, 
				_L("RMobilePhone::NotifyCallForwardingActive Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCallForwardingStatusChange
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCallForwardingStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneCFCondition& aCondition, 
				RMobilePhone::TMobilePhoneCFCondition aExpectedCondition,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCallForwardingStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCondition != aExpectedCondition )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyCallForwardingStatusChange(aRequestStatus, aCondition);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCallForwardingStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aCondition, aExpectedCondition, 
				_L("RMobilePhone::NotifyCallForwardingStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCallServiceCapsChange
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCallServiceCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCallServiceCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobilePhone.NotifyCallServiceCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobilePhone::NotifyCallServiceCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobilePhone::NotifyCallServiceCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCallWaitingStatusChange
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCallWaitingStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneCWInfoEntryV1> &aMobilePhoneCWInfoEntryV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCallWaitingStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneCWInfoEntryV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneCWInfoEntryV1Pckg pkgParam( aMobilePhoneCWInfoEntryV1.GetValue() );
		aMobilePhone.NotifyCallWaitingStatusChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCallWaitingStatusChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneCWInfoEntryV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCallWaitingStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCipheringIndicatorStatus
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyCipheringIndicatorStatus( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobileCallCipheringIndicator& aIndicatorStatus, 
				RMobilePhone::TMobileCallCipheringIndicator aExpectedIndicatorStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCipheringIndicatorStatus did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aIndicatorStatus != aExpectedIndicatorStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyCipheringIndicatorStatus(aRequestStatus, aIndicatorStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCipheringIndicatorStatus Wrong completion status"))	
	ASSERT_EQUALS(aIndicatorStatus, aExpectedIndicatorStatus, 
				_L("RMobilePhone::NotifyCipheringIndicatorStatus Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyIdentityServiceStatus
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyIdentityServiceStatus( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneIdService& aService, 
				RMobilePhone::TMobilePhoneIdService aExpectedService,
				RMobilePhone::TMobilePhoneIdServiceStatus &aStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyIdentityServiceStatus did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aService != aExpectedService )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyIdentityServiceStatus(aRequestStatus, aService, aStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyIdentityServiceStatus Wrong completion status"))	
	ASSERT_EQUALS(aService, aExpectedService, 
				_L("RMobilePhone::NotifyIdentityServiceStatus Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyIdentitySuppressionRejected
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifyIdentitySuppressionRejected( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyIdentitySuppressionRejected did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyIdentitySuppressionRejected Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifySendNetworkServiceRequest
*/	
void TSupplementalTsyTestHelper::WaitForMobilePhoneNotifySendNetworkServiceRequest( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNotifySendSSOperation &aOperation,
				TCmpBase<RMobilePhone::TMobilePhoneSendSSRequestV3> &aMobilePhoneSendSSRequestV3,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifySendNetworkServiceRequest did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneSendSSRequestV3.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneSendSSRequestV3Pckg pkgParam( aMobilePhoneSendSSRequestV3.GetValue() );
		aMobilePhone.NotifySendNetworkServiceRequest(aRequestStatus, aOperation, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifySendNetworkServiceRequest Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneSendSSRequestV3.IsEqual(ELogError), 
				_L("RMobilePhone::NotifySendNetworkServiceRequest Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileUssdMessaging::NotifyNetworkRelease
*/	
void TSupplementalTsyTestHelper::WaitForMobileUssdMessagingNotifyNetworkRelease( 
				RMobileUssdMessaging& aMobileUssdMessaging,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileUssdMessaging::TMobileUssdAttributesV1> &aMobileUssdAttributesV1,
				TDes8 &aMsgAttributes,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileUssdMessaging::NotifyNetworkRelease did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobileUssdAttributesV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pkgParam( aMobileUssdAttributesV1.GetValue() );
		aMobileUssdMessaging.NotifyNetworkRelease(aRequestStatus, pkgParam, aMsgAttributes);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileUssdMessaging::NotifyNetworkRelease Wrong completion status"))	
	ASSERT_TRUE(aMobileUssdAttributesV1.IsEqual(ELogError), 
				_L("RMobileUssdMessaging::NotifyNetworkRelease Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
