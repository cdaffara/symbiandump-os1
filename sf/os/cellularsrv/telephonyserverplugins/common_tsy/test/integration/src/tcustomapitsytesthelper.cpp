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

#include "tcustomapitsytesthelper.h" 



/**
* Constructor
*/	
TCustomApiTsyTestHelper::TCustomApiTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMmCustomAPI::NotifyAlsBlockedChanged
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyAlsBlockedChanged( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TGetAlsBlockStatus& aBlockStatus, 
				RMmCustomAPI::TGetAlsBlockStatus aExpectedBlockStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyAlsBlockedChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aBlockStatus != aExpectedBlockStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyAlsBlockedChanged(aRequestStatus, aBlockStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyAlsBlockedChanged Wrong completion status"))	
	ASSERT_EQUALS(aBlockStatus, aExpectedBlockStatus, 
				_L("RMmCustomAPI::NotifyAlsBlockedChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyCellInfoChange
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyCellInfoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TMmCellInfo> &aMmCellInfo,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyCellInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMmCellInfo.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMmCustomAPI::TMmCellInfoPckg pkgParam( aMmCellInfo.GetValue() );
		aMmCustomAPI.NotifyCellInfoChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyCellInfoChange Wrong completion status"))	
	ASSERT_TRUE(aMmCellInfo.IsEqual(ELogError), 
				_L("RMmCustomAPI::NotifyCellInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyCipheringInfoChange
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyCipheringInfoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TCipheringInfo>& aCipheringInfo,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyCipheringInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aCipheringInfo.IsEqual(ENoLogError))
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyCipheringInfoChange(aRequestStatus, aCipheringInfo.GetValue());
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyCipheringInfoChange Wrong completion status"))	
	ASSERT_TRUE(aCipheringInfo.IsEqual(ELogError), 
				_L("RMmCustomAPI::NotifyCipheringInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyDtmfEvent
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyDtmfEvent( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TDtmfInfo>& aInfo, 
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyDtmfEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aInfo.IsEqual(ENoLogError))
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyDtmfEvent(aRequestStatus, aInfo.GetValue());
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyDtmfEvent Wrong completion status"))	
	ASSERT_TRUE(aInfo.IsEqual(ELogError), 
				_L("RMmCustomAPI::NotifyDtmfEvent Wrong result"));	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyEGprsInfoChange
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyEGprsInfoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TGprsInformationType> &aGprsInformationType,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyEGprsInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aGprsInformationType.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		TPckg<RMmCustomAPI::TGprsInformationType> pkgParam( aGprsInformationType.GetValue() );
		aMmCustomAPI.NotifyEGprsInfoChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyEGprsInfoChange Wrong completion status"))	
	ASSERT_TRUE(aGprsInformationType.IsEqual(ELogError), 
				_L("RMmCustomAPI::NotifyEGprsInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyHSxPAStatus
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyHSxPAStatus( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::THSxPAStatus& aHSxPAStatus, 
				RMmCustomAPI::THSxPAStatus aExpectedHSxPAStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyHSxPAStatus did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aHSxPAStatus != aExpectedHSxPAStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyHSxPAStatus(aRequestStatus, aHSxPAStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyHSxPAStatus Wrong completion status"))	
	ASSERT_EQUALS(aHSxPAStatus, aExpectedHSxPAStatus, 
				_L("RMmCustomAPI::NotifyHSxPAStatus Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyIccCallForwardingStatusChange
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyIccCallForwardingStatusChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TCFIndicators> &aCFIndicators,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyIccCallForwardingStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aCFIndicators.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMmCustomAPI::TCFIndicatorsPckg pkgParam( aCFIndicators.GetValue() );
		aMmCustomAPI.NotifyIccCallForwardingStatusChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyIccCallForwardingStatusChange Wrong completion status"))	
	ASSERT_TRUE(aCFIndicators.IsEqual(ELogError), 
				_L("RMmCustomAPI::NotifyIccCallForwardingStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyNetworkConnectionFailure
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyNetworkConnectionFailure( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyNetworkConnectionFailure did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyNetworkConnectionFailure Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyNSPSStatus
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyNSPSStatus( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TNspsStatus& aNspsStatus, 
				RMmCustomAPI::TNspsStatus aExpectedNspsStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyNSPSStatus did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aNspsStatus != aExpectedNspsStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyNSPSStatus(aRequestStatus, aNspsStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyNSPSStatus Wrong completion status"))	
	ASSERT_EQUALS(aNspsStatus, aExpectedNspsStatus, 
				_L("RMmCustomAPI::NotifyNSPSStatus Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyPndCacheReady
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyPndCacheReady( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TName& aPndName, 
				TName aExpectedPndName,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyPndCacheReady did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aPndName != aExpectedPndName )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyPndCacheReady(aRequestStatus, aPndName);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyPndCacheReady Wrong completion status"))	
	ASSERT_EQUALS_DES16(aPndName, aExpectedPndName, 
				_L("RMmCustomAPI::NotifyPndCacheReady Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyProgrammableOperatorLogoChange
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyProgrammableOperatorLogoChange( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TOperatorId>& aOperatorId, 
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyProgrammableOperatorLogoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aOperatorId.IsEqual(ENoLogError))
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyProgrammableOperatorLogoChange(aRequestStatus, aOperatorId.GetValue());
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyProgrammableOperatorLogoChange Wrong completion status"))	
	ASSERT_TRUE(aOperatorId.IsEqual(ELogError),
				_L("RMmCustomAPI::NotifyProgrammableOperatorLogoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifyRauEvent
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifyRauEvent( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TRauEventStatus& aEventStatus, 
				RMmCustomAPI::TRauEventStatus aExpectedEventStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifyRauEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEventStatus != aExpectedEventStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifyRauEvent(aRequestStatus, aEventStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifyRauEvent Wrong completion status"))	
	ASSERT_EQUALS(aEventStatus, aExpectedEventStatus, 
				_L("RMmCustomAPI::NotifyRauEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifySimCardStatus
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifySimCardStatus( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				RMmCustomAPI::TSIMCardStatus& aCardStatus, 
				RMmCustomAPI::TSIMCardStatus aExpectedCardStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifySimCardStatus did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCardStatus != aExpectedCardStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifySimCardStatus(aRequestStatus, aCardStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifySimCardStatus Wrong completion status"))	
	ASSERT_EQUALS(aCardStatus, aExpectedCardStatus, 
				_L("RMmCustomAPI::NotifySimCardStatus Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMmCustomAPI::NotifySsNetworkEvent
*/	
void TCustomApiTsyTestHelper::WaitForMmCustomAPINotifySsNetworkEvent( 
				RMmCustomAPI& aMmCustomAPI,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMmCustomAPI::TSsTypeAndMode>& aSsTypeAndMode, 
				RMmCustomAPI::TSsInfo &aSsInfo,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMmCustomAPI::NotifySsNetworkEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aSsTypeAndMode.IsEqual(ENoLogError))
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMmCustomAPI.NotifySsNetworkEvent(aRequestStatus, aSsTypeAndMode.GetValue(), aSsInfo);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMmCustomAPI::NotifySsNetworkEvent Wrong completion status"))	
	ASSERT_TRUE(aSsTypeAndMode.IsEqual(ELogError),
				_L("RMmCustomAPI::NotifySsNetworkEvent Wrong result"));
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
