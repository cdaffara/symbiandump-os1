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

#include "tsmsmessagingtsytesthelper.h" 



/**
* Constructor
*/	
TSmsMessagingTsyTestHelper::TSmsMessagingTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobileSmsMessaging::NotifyMoSmsBearerChange
*/	
void TSmsMessagingTsyTestHelper::WaitForMobileSmsMessagingNotifyMoSmsBearerChange( 
				RMobileSmsMessaging& aMobileSmsMessaging,
				TEtelRequestBase& aRequestStatus,
				RMobileSmsMessaging::TMobileSmsBearer& aBearer, 
				RMobileSmsMessaging::TMobileSmsBearer aExpectedBearer,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileSmsMessaging::NotifyMoSmsBearerChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aBearer != aExpectedBearer )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileSmsMessaging.NotifyMoSmsBearerChange(aRequestStatus, aBearer);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileSmsMessaging::NotifyMoSmsBearerChange Wrong completion status"))	
	ASSERT_EQUALS(aBearer, aExpectedBearer, 
				_L("RMobileSmsMessaging::NotifyMoSmsBearerChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileSmsMessaging::NotifyReceiveModeChange
*/	
void TSmsMessagingTsyTestHelper::WaitForMobileSmsMessagingNotifyReceiveModeChange( 
				RMobileSmsMessaging& aMobileSmsMessaging,
				TEtelRequestBase& aRequestStatus,
				RMobileSmsMessaging::TMobileSmsReceiveMode& aReceiveMode, 
				RMobileSmsMessaging::TMobileSmsReceiveMode aExpectedReceiveMode,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileSmsMessaging::NotifyReceiveModeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aReceiveMode != aExpectedReceiveMode )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileSmsMessaging.NotifyReceiveModeChange(aRequestStatus, aReceiveMode);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileSmsMessaging::NotifyReceiveModeChange Wrong completion status"))	
	ASSERT_EQUALS(aReceiveMode, aExpectedReceiveMode, 
				_L("RMobileSmsMessaging::NotifyReceiveModeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileSmsMessaging::NotifySmspListChange
*/	
void TSmsMessagingTsyTestHelper::WaitForMobileSmsMessagingNotifySmspListChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileSmsMessaging::NotifySmspListChange did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileSmsMessaging::NotifySmspListChange Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
