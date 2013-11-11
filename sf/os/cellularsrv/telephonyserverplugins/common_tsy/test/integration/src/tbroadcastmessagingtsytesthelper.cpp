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

#include "tbroadcastmessagingtsytesthelper.h" 



/**
* Constructor
*/	
TBroadcastMessagingTsyTestHelper::TBroadcastMessagingTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobileBroadcastMessaging::NotifyBroadcastIdListChange
*/	
void TBroadcastMessagingTsyTestHelper::WaitForMobileBroadcastMessagingNotifyBroadcastIdListChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileBroadcastMessaging::NotifyBroadcastIdListChange did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileBroadcastMessaging::NotifyBroadcastIdListChange Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileBroadcastMessaging::NotifyFilterSettingChange
*/	
void TBroadcastMessagingTsyTestHelper::WaitForMobileBroadcastMessagingNotifyFilterSettingChange( 
				RMobileBroadcastMessaging& aMobileBroadcastMessaging,
				TEtelRequestBase& aRequestStatus,
				RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter& aSetting, 
				RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter aExpectedSetting,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileBroadcastMessaging::NotifyFilterSettingChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aSetting != aExpectedSetting )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileBroadcastMessaging.NotifyFilterSettingChange(aRequestStatus, aSetting);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileBroadcastMessaging::NotifyFilterSettingChange Wrong completion status"))	
	ASSERT_EQUALS(aSetting, aExpectedSetting, 
				_L("RMobileBroadcastMessaging::NotifyFilterSettingChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileBroadcastMessaging::NotifyLanguageFilterChange
*/	
void TBroadcastMessagingTsyTestHelper::WaitForMobileBroadcastMessagingNotifyLanguageFilterChange( 
				RMobileBroadcastMessaging& aMobileBroadcastMessaging,
				TEtelRequestBase& aRequestStatus,
				TDes16& aLangFilter, 
				TDes16 aExpectedLangFilter,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileBroadcastMessaging::NotifyLanguageFilterChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aLangFilter != aExpectedLangFilter )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileBroadcastMessaging.NotifyLanguageFilterChange(aRequestStatus, aLangFilter);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileBroadcastMessaging::NotifyLanguageFilterChange Wrong completion status"))	
	ASSERT_EQUALS_DES16(aLangFilter, aExpectedLangFilter, 
				_L("RMobileBroadcastMessaging::NotifyLanguageFilterChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
