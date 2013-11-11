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

#include "tlocationservicestsytesthelper.h" 



/**
* Constructor
*/	
TLocationServicesTsyTestHelper::TLocationServicesTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobileLocationServices::NotifyMtlr
*/	
void TLocationServicesTsyTestHelper::WaitForMobileLocationServicesNotifyMtlr( 
				RMobileLocationServices& aMobileLocationServices,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileLocationServices::TNotifyMtlrV7> &aNotifyMtlrV7,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileLocationServices::NotifyMtlr did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aNotifyMtlrV7.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileLocationServices::TNotifyMtlrV7Pckg pkgParam( aNotifyMtlrV7.GetValue() );
		aMobileLocationServices.NotifyMtlr(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileLocationServices::NotifyMtlr Wrong completion status"))	
	ASSERT_TRUE(aNotifyMtlrV7.IsEqual(ELogError), 
				_L("RMobileLocationServices::NotifyMtlr Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileLocationServices::NotifyMeasurementControl
*/	
void TLocationServicesTsyTestHelper::WaitForMobileLocationServicesNotifyMeasurementControl( 
				RMobileLocationServices& aMobileLocationServices,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobileLocationServices::TMeasurementControlV7> &aMeasurementControlV7,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileLocationServices::NotifyMeasurementControl did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMeasurementControlV7.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobileLocationServices::TMeasurementControlV7Pckg pkgParam( aMeasurementControlV7.GetValue() );
		aMobileLocationServices.NotifyMeasurementControl(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileLocationServices::NotifyMeasurementControl Wrong completion status"))	
	ASSERT_TRUE(aMeasurementControlV7.IsEqual(ELogError), 
				_L("RMobileLocationServices::NotifyMeasurementControl Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
