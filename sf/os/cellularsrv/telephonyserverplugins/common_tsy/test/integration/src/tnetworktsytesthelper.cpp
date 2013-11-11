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

#include "tnetworktsytesthelper.h" 

#include "cctsyinidata.h"


/**
* Constructor
*/	
TNetworkTsyTestHelper::TNetworkTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobilePhone::NotifyModeChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyModeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode, 
				RMobilePhone::TMobilePhoneNetworkMode aExpectedNetworkMode,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyModeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aNetworkMode != aExpectedNetworkMode )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyModeChange(aRequestStatus, aNetworkMode);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyModeChange Wrong completion status"))	
	ASSERT_EQUALS(aNetworkMode, aExpectedNetworkMode, 
				_L("RMobilePhone::NotifyModeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyNetworkInvScanChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyNetworkInvScanChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneInvestigationScan& aSetting, 
				RMobilePhone::TMobilePhoneInvestigationScan aExpectedSetting,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNetworkInvScanChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aSetting != aExpectedSetting )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyNetworkInvScanChange(aRequestStatus, aSetting);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyNetworkInvScanChange Wrong completion status"))	
	ASSERT_EQUALS(aSetting, aExpectedSetting, 
				_L("RMobilePhone::NotifyNetworkInvScanChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyNetworkInvScanEvent
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyNetworkInvScanEvent( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneInvestigationScanEvent& aEvent, 
				RMobilePhone::TMobilePhoneInvestigationScanEvent aExpectedEvent,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNetworkInvScanEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEvent != aExpectedEvent )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyNetworkInvScanEvent(aRequestStatus, aEvent);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyNetworkInvScanEvent Wrong completion status"))	
	ASSERT_EQUALS(aEvent, aExpectedEvent, 
				_L("RMobilePhone::NotifyNetworkInvScanEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyNetworkRegistrationStatusChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyNetworkRegistrationStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneRegistrationStatus& aStatus, 
				RMobilePhone::TMobilePhoneRegistrationStatus aExpectedStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aStatus != aExpectedStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyNetworkRegistrationStatusChange(aRequestStatus, aStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aStatus, aExpectedStatus, 
				_L("RMobilePhone::NotifyNetworkRegistrationStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyNetworkSecurityLevelChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyNetworkSecurityLevelChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNetworkSecurity& aSecurity, 
				RMobilePhone::TMobilePhoneNetworkSecurity aExpectedSecurity,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNetworkSecurityLevelChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aSecurity != aExpectedSecurity )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyNetworkSecurityLevelChange(aRequestStatus, aSecurity);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyNetworkSecurityLevelChange Wrong completion status"))	
	ASSERT_EQUALS(aSecurity, aExpectedSecurity, 
				_L("RMobilePhone::NotifyNetworkSecurityLevelChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyPreferredNetworksListChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyPreferredNetworksListChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyPreferredNetworksListChange did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyPreferredNetworksListChange Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCurrentNetworkChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV1> &aMobilePhoneNetworkInfoV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkInfoV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg pkgParam( aMobilePhoneNetworkInfoV1.GetValue() );
		aMobilePhone.NotifyCurrentNetworkChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCurrentNetworkChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkInfoV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCurrentNetworkChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCurrentNetworkChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV2> &aMobilePhoneNetworkInfoV2,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkInfoV2.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg pkgParam( aMobilePhoneNetworkInfoV2.GetValue() );
		aMobilePhone.NotifyCurrentNetworkChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCurrentNetworkChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkInfoV2.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCurrentNetworkChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCurrentNetworkChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV5> &aMobilePhoneNetworkInfoV5,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkInfoV5.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkInfoV5Pckg pkgParam( aMobilePhoneNetworkInfoV5.GetValue() );
		aMobilePhone.NotifyCurrentNetworkChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCurrentNetworkChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkInfoV5.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCurrentNetworkChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCurrentNetworkChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV1> &aMobilePhoneNetworkInfoV1,
				RMobilePhone::TMobilePhoneLocationAreaV1 &aArea,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkInfoV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkInfoV1Pckg pkgParam( aMobilePhoneNetworkInfoV1.GetValue() );
		aMobilePhone.NotifyCurrentNetworkChange(aRequestStatus, pkgParam, aArea);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCurrentNetworkChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkInfoV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCurrentNetworkChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCurrentNetworkChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV2> &aMobilePhoneNetworkInfoV2,
				RMobilePhone::TMobilePhoneLocationAreaV1 &aArea,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkInfoV2.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg pkgParam( aMobilePhoneNetworkInfoV2.GetValue() );
		aMobilePhone.NotifyCurrentNetworkChange(aRequestStatus, pkgParam, aArea);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCurrentNetworkChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkInfoV2.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCurrentNetworkChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyCurrentNetworkChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyCurrentNetworkChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkInfoV5> &aMobilePhoneNetworkInfoV5,
				RMobilePhone::TMobilePhoneLocationAreaV1 &aArea,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkInfoV5.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkInfoV5Pckg pkgParam( aMobilePhoneNetworkInfoV5.GetValue() );
		aMobilePhone.NotifyCurrentNetworkChange(aRequestStatus, pkgParam, aArea);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyCurrentNetworkChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkInfoV5.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyCurrentNetworkChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyAPNListChanged
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyAPNListChanged( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyAPNListChanged did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyAPNListChanged Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyAPNControlListServiceStatusChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyAPNControlListServiceStatusChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus, 
				RMobilePhone::TAPNControlListServiceStatus aExpectedAPNControlListServiceStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyAPNControlListServiceStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aAPNControlListServiceStatus != aExpectedAPNControlListServiceStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyAPNControlListServiceStatusChange(aRequestStatus, aAPNControlListServiceStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyAPNControlListServiceStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aAPNControlListServiceStatus, aExpectedAPNControlListServiceStatus, 
				_L("RMobilePhone::NotifyAPNControlListServiceStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}


/**
* Notify reset for RMobilePhone::NotifyNetworkSelectionSettingChange
*/	
void TNetworkTsyTestHelper::WaitForMobilePhoneNotifyNetworkSelectionSettingChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNetworkSelectionV1 &aNetSel,
				RMobilePhone::TMobilePhoneSelectionMethod aDesiredMethod,
				TInt aWantedStatus )
	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNetworkSelectionSettingChange timed out"))
	
	while (	err == KErrNone && 
			aRequestStatus.Int() == KErrNone  &&
			aNetSel.iMethod != aDesiredMethod )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg pkg( aNetSel );
		aMobilePhone.NotifyNetworkSelectionSettingChange(aRequestStatus, pkg);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
						_L("RMobilePhone::NotifyNetworkSelectionSettingChange Wrong completion status"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
		
/**
 * Get the current network info 
 * @param aPhone RMobilePhone object
 * @param aPhoneNetwork Packaged descriptor carrying a TMobilePhoneNetworkInfoV1 object or derived class
 * @param aArea current location of the phone (see RMobilePhone::GetCUrrentNetwork)
 */
 TInt TNetworkTsyTestHelper::GetCurrentNetworkL( RMobilePhone& aPhone, RMobilePhone::TMobilePhoneNetworkInfoV1Pckg& aPhoneNetwork, 
 									RMobilePhone::TMobilePhoneLocationAreaV1& aArea,TOperatorName& aOperatorName )
 	{
 	
 	TExtEtelRequestStatus reqStatus(aPhone, EMobilePhoneGetCurrentNetwork);
 	CleanupStack::PushL(reqStatus);
 	
 	aPhone.GetCurrentNetwork( reqStatus, aPhoneNetwork, aArea );
 	
 	TInt reqResult = iTestStep.WaitForRequestWithTimeOut(reqStatus, ETimeLong);
 		
 	if(KErrNone == reqResult)
 		{
 		reqResult = reqStatus.Int();
 		}
 	
 	TPtrC ptrNetworkName;
 	if( aPhoneNetwork().iShortName.Size() > 0 )
 	    {
        ptrNetworkName.Set(aPhoneNetwork().iShortName.Ptr(), aPhoneNetwork().iShortName.Length());
 	    }
 	else
 	    {
        ptrNetworkName.Set(aPhoneNetwork().iDisplayTag.Ptr(), aPhoneNetwork().iDisplayTag.Length()); 	
 	    }
 	DEBUG_PRINTF2(_L("Current network is %S"), &ptrNetworkName);
 	
    if( ptrNetworkName.Find(_L("O2")) >=0 )
        {
        aOperatorName = EOperatorO2;
        }
    else if ( ptrNetworkName.Find(_L("voda")) >=0 )
        {
        aOperatorName = EOperatorVodafone;
        }
    else if ( ptrNetworkName.Find(_L("Orange")) >=0 )
        {
        aOperatorName = EOperatorOrange;
        }
    else if ( ptrNetworkName.Find(_L("Elisa")) >=0 )
        {
        aOperatorName = EOperatorElisa;
        }
    else if ( ptrNetworkName.Find(_L("DNA")) >=0 )
        {
        aOperatorName = EOperatorDNA;
        }
    else if ( ptrNetworkName.Find(_L("dna")) >=0 )
        {
        aOperatorName = EOperatorDNA;
        }
    else if ( ptrNetworkName.Find(_L("SONERA")) >=0 )
        {
        aOperatorName = EOperatorSonera;
        }
    else if ( ptrNetworkName.Find(_L("T-Mobile")) >=0 
            ||  ptrNetworkName.Find(_L("One2One")) >=0
            ||  ptrNetworkName.Find(_L("TMO UK")) >=0)
        {
        aOperatorName = EOperatorTMobile;
        }
    else if( (ptrNetworkName.Find(_L("01")) >=0) || 
            (ptrNetworkName.Find(_L("ANITE")) >=0) )
        {
        aOperatorName = EOperatorAnite;
        }
    else if ( ptrNetworkName.Find(_L("NTN")) >=0 
            ||  ptrNetworkName.Find(_L("NOKIA")) >=0)
        {
        aOperatorName = EOperatorNTN;
        }
    else
        {
        aOperatorName = EOperatorUnknown;
        }
    
 	CleanupStack::PopAndDestroy(1);

 	return reqResult;
 	}

/** 
* Determine the network registration status
* @param aPhone RMobilePhone object
* @param aRegistrationStatus - (out) returned phone registration status
* @return KErrNone if registered on home network, KErrNotReady if not on home network. 
*/
TInt TNetworkTsyTestHelper::GetNetworkRegistrationStatusL( RMobilePhone& aPhone, 
							RMobilePhone::TMobilePhoneRegistrationStatus &aRegistrationStatus )
	{
	TExtEtelRequestStatus reqStatus(aPhone, EMobilePhoneGetNetworkRegistrationStatus);
	CleanupStack::PushL(reqStatus);

	aPhone.GetNetworkRegistrationStatus(reqStatus, aRegistrationStatus);
	TInt reqResult = iTestStep.WaitForRequestWithTimeOut(reqStatus, ETimeLong );	

	if(KErrNone == reqResult)
		{
		reqResult = reqStatus.Int();
		}

	CleanupStack::PopAndDestroy(1);
	
	return reqResult;
	}
	
	
	
	
/** 
* Determine the network registration status and if we are not on the network, wait
* @param aPhone RMobilePhone object
* @return KErrNone if registered on home network, KErrNotReady if not on home network. 
* Other error codes as from RMobilePhone::GetNetworkRegistrationStatus
*/
TInt TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL( RMobilePhone& aPhone )
	{

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	TInt err = GetNetworkRegistrationStatusL(aPhone, registrationStatus);
	
	if( KErrNone != err )
		{
		return err;
		}
	
	if( registrationStatus == RMobilePhone::ERegisteredOnHomeNetwork )
		{
		return KErrNone;
		}
		
	TExtEtelRequestStatus reqStatus(aPhone, EMobilePhoneNotifyNetworkRegistrationStatusChange );
	CleanupStack::PushL(reqStatus);
	
	aPhone.NotifyNetworkRegistrationStatusChange(reqStatus, registrationStatus);
	 		
	WaitForMobilePhoneNotifyNetworkRegistrationStatusChange( aPhone, 
									reqStatus, registrationStatus,
									RMobilePhone::ERegisteredOnHomeNetwork, KErrNone );

	// If the wait has timed out, we check again to see if we missed the notification									
	if( KErrNone == err )
		{
		err = reqStatus.Int();		
		}

	if( KErrNone != err )
		{
		err = GetNetworkRegistrationStatusL(aPhone, registrationStatus);
		}		
								
	if( KErrNone == err )
		{
		err = (registrationStatus == RMobilePhone::ERegisteredOnHomeNetwork) ? KErrNone : KErrNotReady;
		}
									
	CleanupStack::PopAndDestroy(1);
	
	return err;
	
	}

void TNetworkTsyTestHelper::CheckPhoneConnectedToAniteL(RMobilePhone &aPhone)
	{
/*
 * @param aPhone - Handle to the subsession of a phone
 * 
 * This function checks to see if we are connected to the Anite Network Simulator.  
 * It leaves if we connected to any other network.  It does this because we don't want to 
 * actually connect to an operator answering emergency calls and then just hangup on them.
 * People can be fined for making prank calls to emergency numbers, and we don't want that.
 */
	RMobilePhone::TMobilePhoneNetworkInfoV1 netInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = GetCurrentNetworkL( aPhone, netInfoPkg, area, operatorName );
	CHECK_EQUALS_L( err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get network id"));
	CHECK_EQUALS_L( operatorName, EOperatorAnite, _L("RMobilePhone::GetCurrentNetwork did not return Anite.  Simulation tests should be tested using the Anite network simulator."));
	}

void TNetworkTsyTestHelper::GetNetworkPasswordsSectionL(RMobilePhone& aMobilePhone, const TDesC*& aNetworkSection)
/**
 * member function for obtaining the network section of the INI file to be read returning void
 *
 * @param aMobilePhone a reference to RMobilePhone object
 * @param aNetworkSection a reference to a pointer to hold the network section in INI file

 * @return void
 */

	{
	   static TBool firstTime = ETrue;
	   if(firstTime) // register to network only once
		   {
	       TInt ret = WaitForRegisteredOnHomeNetworkL(aMobilePhone);
	       ASSERT_EQUALS(ret, KErrNone,
	                _L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	       firstTime = EFalse;
	       }
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = GetCurrentNetworkL( aMobilePhone, infopckg, area, operatorName );	
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	switch ( operatorName )
		{
		case EOperatorO2:
			aNetworkSection = &KIniSectionO2Passwords;
			break;
		case EOperatorVodafone:
			aNetworkSection = &KIniSectionVodafonePasswords;
			break;
		case EOperatorOrange:
			aNetworkSection = &KIniSectionOrangePasswords;
			break;
		case EOperatorTMobile:
			aNetworkSection = &KIniSectionTMobilePasswords;
			break;
		case EOperatorAnite:
			aNetworkSection = &KIniSectionAnitePasswords;
			break;
		case EOperatorNTN:
            aNetworkSection = &KIniSectionNTNPasswords;
            break;
		case EOperatorUnknown:
		default:
		    CHECK_TRUE_L( EFail, _L("Failed to identify current network"));
			break;
		}
}
