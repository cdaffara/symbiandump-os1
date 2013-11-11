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

#include "tpacketservicetsytesthelper.h" 
#include "cctsyinidata.h"

#include "pcktcs.h"

/**
* Constructor
*/	
TPacketServiceTsyTestHelper::TPacketServiceTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RPacketContext::NotifyConnectionSpeedChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketContextNotifyConnectionSpeedChange( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TUint& aRate, 
				TUint aExpectedRate,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketContext::NotifyConnectionSpeedChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aRate != aExpectedRate )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketContext.NotifyConnectionSpeedChange(aRequestStatus, aRate);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketContext::NotifyConnectionSpeedChange Wrong completion status"))	
	ASSERT_EQUALS(aRate, aExpectedRate, 
				_L("RPacketContext::NotifyConnectionSpeedChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketContext::NotifyStatusChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketContextNotifyStatusChange( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				RPacketContext::TContextStatus& aContextStatus, 
				RPacketContext::TContextStatus aExpectedContextStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketContext::NotifyStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aContextStatus != aExpectedContextStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		DEBUG_PRINTF3(_L("State %d Expected %d waiting for correct state"), aContextStatus, aExpectedContextStatus);

		if (RPacketContext::EStatusActive == aContextStatus)
			{
			// === NTN Failure ===
			ERR_PRINTF1(_L("<font color=Orange>$NTNKnownFailure: Context Activation success (EStatusActive = 3)</font>"));
			// Nothing else to wait for now!
			// Deactivate the context with RPacketContext::Deactivate
			TExtEtelRequestStatus contextDeactivateStatus(aPacketContext, EPacketContextDeactivate);
			aPacketContext.Deactivate(contextDeactivateStatus);
			User::Leave(iTestStep.WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong));
			}

		aPacketContext.NotifyStatusChange(aRequestStatus, aContextStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketContext::NotifyStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aContextStatus, aExpectedContextStatus, 
				_L("RPacketContext::NotifyStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketContext::NotifyConfigChanged
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketContextNotifyConfigChanged( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfigGPRS> &aContextConfigGPRS,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketContext::NotifyConfigChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aContextConfigGPRS.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketContext::TContextConfigGPRSPckg pkgParam( aContextConfigGPRS.GetValue() );
		aPacketContext.NotifyConfigChanged(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketContext::NotifyConfigChanged Wrong completion status"))	
	ASSERT_TRUE(aContextConfigGPRS.IsEqual(ELogError), 
				_L("RPacketContext::NotifyConfigChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketContext::NotifyConfigChanged
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketContextNotifyConfigChanged( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfigR99_R4> &aContextConfigR99_R4,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketContext::NotifyConfigChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aContextConfigR99_R4.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketContext::TContextConfigR99_R4Pckg pkgParam( aContextConfigR99_R4.GetValue() );
		aPacketContext.NotifyConfigChanged(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketContext::NotifyConfigChanged Wrong completion status"))	
	ASSERT_TRUE(aContextConfigR99_R4.IsEqual(ELogError), 
				_L("RPacketContext::NotifyConfigChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketContext::NotifyConfigChanged
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketContextNotifyConfigChanged( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfig_R5> &aContextConfig_R5,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketContext::NotifyConfigChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aContextConfig_R5.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketContext::TContextConfig_R5Pckg pkgParam( aContextConfig_R5.GetValue() );
		aPacketContext.NotifyConfigChanged(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketContext::NotifyConfigChanged Wrong completion status"))	
	ASSERT_TRUE(aContextConfig_R5.IsEqual(ELogError), 
				_L("RPacketContext::NotifyConfigChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketContext::NotifyDataTransferred
*/	
void TPacketServiceTsyTestHelper::WaitForPacketContextNotifyDataTransferred( 
				RPacketContext& aPacketContext,
				TEtelRequestBase& aRequestStatus,
				RPacketContext::TDataVolume& aVolume, 
				RPacketContext::TDataVolume& aExpectedVolume,
				TUint &aRcvdGranularity,
				TUint &aSentGranularity,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketContext::NotifyDataTransferred did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& (0 == memcmp(&aVolume, &aExpectedVolume, sizeof(aVolume))) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketContext.NotifyDataTransferred(aRequestStatus, aVolume, aRcvdGranularity,aSentGranularity);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketContext::NotifyDataTransferred Wrong completion status"))	
	ASSERT_TRUE((0 == memcmp(&aVolume, &aExpectedVolume, sizeof(aVolume))), 
				_L("RPacketContext::NotifyDataTransferred Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketQoS::NotifyProfileChanged
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketQoSNotifyProfileChanged( 
				RPacketQoS& aPacketQoS,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketQoS::TQoSGPRSNegotiated> &aQoSGPRSNegotiated,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketQoS::NotifyProfileChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aQoSGPRSNegotiated.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketQoS::TQoSGPRSNegotiatedPckg pkgParam( aQoSGPRSNegotiated.GetValue() );
		aPacketQoS.NotifyProfileChanged(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketQoS::NotifyProfileChanged Wrong completion status"))	
	ASSERT_TRUE(aQoSGPRSNegotiated.IsEqual(ELogError), 
				_L("RPacketQoS::NotifyProfileChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketQoS::NotifyProfileChanged
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketQoSNotifyProfileChanged( 
				RPacketQoS& aPacketQoS,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketQoS::TQoSR99_R4Negotiated> &aQoSR99_R4Negotiated,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketQoS::NotifyProfileChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aQoSR99_R4Negotiated.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketQoS::TQoSR99_R4NegotiatedPckg pkgParam( aQoSR99_R4Negotiated.GetValue() );
		aPacketQoS.NotifyProfileChanged(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketQoS::NotifyProfileChanged Wrong completion status"))	
	ASSERT_TRUE(aQoSR99_R4Negotiated.IsEqual(ELogError), 
				_L("RPacketQoS::NotifyProfileChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketQoS::NotifyProfileChanged
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketQoSNotifyProfileChanged( 
				RPacketQoS& aPacketQoS,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketQoS::TQoSR5Negotiated> &aQoSR5Negotiated,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketQoS::NotifyProfileChanged did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aQoSR5Negotiated.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketQoS::TQoSR5NegotiatedPckg pkgParam( aQoSR5Negotiated.GetValue() );
		aPacketQoS.NotifyProfileChanged(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketQoS::NotifyProfileChanged Wrong completion status"))	
	ASSERT_TRUE(aQoSR5Negotiated.IsEqual(ELogError), 
				_L("RPacketQoS::NotifyProfileChanged Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketService::NotifyAttachModeChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyAttachModeChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TAttachMode& aMode, 
				RPacketService::TAttachMode aExpectedMode,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyAttachModeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aMode != aExpectedMode )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketService.NotifyAttachModeChange(aRequestStatus, aMode);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyAttachModeChange Wrong completion status"))	
	ASSERT_EQUALS(aMode, aExpectedMode, 
				_L("RPacketService::NotifyAttachModeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketService::NotifyChangeOfNtwkRegStatus
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyChangeOfNtwkRegStatus( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TRegistrationStatus& aRegistrationStatus, 
				RPacketService::TRegistrationStatus aExpectedRegistrationStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyChangeOfNtwkRegStatus did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aRegistrationStatus != aExpectedRegistrationStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketService.NotifyChangeOfNtwkRegStatus(aRequestStatus, aRegistrationStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyChangeOfNtwkRegStatus Wrong completion status"))	
	ASSERT_EQUALS(aRegistrationStatus, aExpectedRegistrationStatus, 
				_L("RPacketService::NotifyChangeOfNtwkRegStatus Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketService::NotifyContextActivationRequested
*/
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyContextActivationRequested( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TPckg<RPacketContext::TContextConfigGPRS> &aContextConfigGPRSPkg,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyContextActivationRequested did not complete"))

	while (	err == KErrNone && aRequestStatus.Int() == KErrNone )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		aPacketService.NotifyContextActivationRequested(aRequestStatus, aContextConfigGPRSPkg);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyContextActivationRequested Wrong completion status"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

/**
* Notify reset for RPacketService::NotifyContextActivationRequested
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyContextActivationRequested( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfigR99_R4> &aContextConfigR99_R4,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyContextActivationRequested did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aContextConfigR99_R4.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketContext::TContextConfigR99_R4Pckg pkgParam( aContextConfigR99_R4.GetValue() );
		aPacketService.NotifyContextActivationRequested(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyContextActivationRequested Wrong completion status"))	
	ASSERT_TRUE(aContextConfigR99_R4.IsEqual(ELogError), 
				_L("RPacketService::NotifyContextActivationRequested Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketService::NotifyContextActivationRequested
*/	
/*DOES NOT COMPILE
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyContextActivationRequested( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RPacketContext::TContextConfig_R5> &aContextConfig_R5,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyContextActivationRequested did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aContextConfig_R5.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RPacketContext::TContextConfig_R5Pckg pkgParam( aContextConfig_R5.GetValue() );
		aPacketService.NotifyContextActivationRequested(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyContextActivationRequested Wrong completion status"))	
	ASSERT_TRUE(aContextConfig_R5.IsEqual(ELogError), 
				_L("RPacketService::NotifyContextActivationRequested Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
*/		
/**
* Notify reset for RPacketService::NotifyContextAdded
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyContextAdded( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				TDes& aContextId, 
				TDes aExpectedContextId,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyContextAdded did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aContextId != aExpectedContextId )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketService.NotifyContextAdded(aRequestStatus, aContextId);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyContextAdded Wrong completion status"))	
	ASSERT_EQUALS_DES16(aContextId, aExpectedContextId, 
				_L("RPacketService::NotifyContextAdded Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketService::NotifyDynamicCapsChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyDynamicCapsChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TDynamicCapsFlags& aCaps, 
				RPacketService::TDynamicCapsFlags aWantedCaps,
				RPacketService::TDynamicCapsFlags aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyDynamicCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aPacketService.NotifyDynamicCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RPacketService::NotifyDynamicCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RPacketService::NotifyDynamicCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketService::NotifyMSClassChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyMSClassChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TMSClass& aNewClass, 
				RPacketService::TMSClass aExpectedNewClass,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyMSClassChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aNewClass != aExpectedNewClass )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketService.NotifyMSClassChange(aRequestStatus, aNewClass);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyMSClassChange Wrong completion status"))	
	ASSERT_EQUALS(aNewClass, aExpectedNewClass, 
				_L("RPacketService::NotifyMSClassChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketService::NotifyReleaseModeChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyReleaseModeChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TPacketReleaseMode& aReleaseMode, 
				RPacketService::TPacketReleaseMode aExpectedReleaseMode,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyReleaseModeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aReleaseMode != aExpectedReleaseMode )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketService.NotifyReleaseModeChange(aRequestStatus, aReleaseMode);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyReleaseModeChange Wrong completion status"))	
	ASSERT_EQUALS(aReleaseMode, aExpectedReleaseMode, 
				_L("RPacketService::NotifyReleaseModeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RPacketService::NotifyStatusChange
*/	
void TPacketServiceTsyTestHelper::WaitForPacketServiceNotifyStatusChange( 
				RPacketService& aPacketService,
				TEtelRequestBase& aRequestStatus,
				RPacketService::TStatus& aPacketStatus, 
				RPacketService::TStatus aExpectedPacketStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeVeryLong);	
	ASSERT_EQUALS(err, KErrNone, _L("RPacketService::NotifyStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aPacketStatus != aExpectedPacketStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aPacketService.NotifyStatusChange(aRequestStatus, aPacketStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RPacketService::NotifyStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aPacketStatus, aExpectedPacketStatus, 
				_L("RPacketService::NotifyStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Gets the GPRS info for the supplied network by reading from the data ini file
*/
void TPacketServiceTsyTestHelper::GetGprsSettings(RMobilePhone::TMobilePhoneNetworkInfoV1 aInfo, TPtrC& aAccessPoint, TPtrC& aUser, TPtrC& aPassword,TUint aNumber)
{
    TBuf<200> section;
    TPtrC ptrNetworkName;
    if( aInfo.iShortName.Size() > 0 )
        {
        ptrNetworkName.Set(aInfo.iShortName.Ptr(), aInfo.iShortName.Length());
        }
    else
        {
        ptrNetworkName.Set(aInfo.iDisplayTag.Ptr(), aInfo.iDisplayTag.Length());    
        }
    DEBUG_PRINTF2(_L("Current network is %S"), &ptrNetworkName);
    if(ptrNetworkName.Find(_L("voda")) != KErrNotFound)	//vodafone
		{
        section.Copy(KIniSectionVodafoneGprs);
		DEBUG_PRINTF1(_L("Retrieving Vodafone GPRS settings"));
		}
	else if(ptrNetworkName.Find(_L("O2"))	!= KErrNotFound)//O2
		{
		DEBUG_PRINTF1(_L("Retrieving O2 GPRS settings"));
        section.Copy(KIniSectionO2Gprs);
		}
	else if(ptrNetworkName.Find(_L("Orange"))!= KErrNotFound)	//Orange
		{
		DEBUG_PRINTF1(_L("Retrieving Orange GPRS settings"));
        section.Copy(KIniSectionOrangeGprs);
		}
	else if((ptrNetworkName.Find(_L("one2one")) != KErrNotFound)||(ptrNetworkName.Find(_L("T-Mobile"))	!= KErrNotFound) || (ptrNetworkName.Find(_L("TMO UK")) != KErrNotFound))//TMobile
		{
		DEBUG_PRINTF1(_L("Retrieving TMobile GPRS settings"));
        section.Copy(KIniSectionTMobileGprs);
		}
	else if(ptrNetworkName.Find(_L("Elisa"))!= KErrNotFound)	//Elisa
		{
		DEBUG_PRINTF1(_L("Retrieving Elisa GPRS settings"));
        section.Copy(KIniSectionElisaGprs);
		}
	else if(ptrNetworkName.Find(_L("dna"))!= KErrNotFound)	//DNA
		{
		DEBUG_PRINTF1(_L("Retrieving DNA GPRS settings"));
        section.Copy(KIniSectionDNAGprs);
		}
	else if(ptrNetworkName.Find(_L("SONERA"))!= KErrNotFound)	//SONERA
		{
		DEBUG_PRINTF1(_L("Retrieving Sonera GPRS settings"));
        section.Copy(KIniSectionSoneraGprs);

		}
	else if(ptrNetworkName.Find(_L("3 UK")) != KErrNotFound)  // Three
		{
		DEBUG_PRINTF1(_L("Retrieving Three GPRS settings"));
        section.Copy(KIniSectionThreeGprs);
        }
	else if(ptrNetworkName.Find(_L("NTN")) != KErrNotFound)   // NTN
        {
		section.Copy(KIniSectionNTNGprs);
		DEBUG_PRINTF1(_L("Retrieving NTN GPRS settings"));
        }
	else if(ptrNetworkName.Find(_L("NOKIA")) != KErrNotFound)   // NTN
        {
		section.Copy(KIniSectionNTNGprs);
		DEBUG_PRINTF1(_L("Retrieving NTN GPRS settings"));
        }
	else if( (ptrNetworkName.Find(_L("01")) >=0) || 
			(ptrNetworkName.Find(_L("ANITE")) >=0) ) // Anite
		{
		DEBUG_PRINTF1(_L("Retrieving Anite GPRS settings"));
        section.Copy(KIniSectionVodafoneGprs); // Using vodafone GRPS settings, since Anite should accept any kind
		}
	else
		{
		DEBUG_PRINTF2(_L("Unable to retrieve network name \"%S\". Using defaults"),&(ptrNetworkName));
        section.Copy(KIniSectionDefaultGprs);
		}
    if(aNumber!=1)
        {
        TBuf<200> temp;
        temp.Copy(section);
        section.Format(_L("%S %d"),&temp,aNumber);
        }
    iTestStep.GetStringFromConfig(section, KIniApnName, aAccessPoint);
    iTestStep.GetStringFromConfig(section, KIniUsername, aUser);
    iTestStep.GetStringFromConfig(section, KIniPassword, aPassword);
}

void TPacketServiceTsyTestHelper::ActivatePrimaryPacketContextL(RMobilePhone& aMobilePhone, 
                                                                RPacketContext& aPacketContext,
                                                                TInt aNum)
/**
 * Activates a primary packet context with a GPRS config.
 * 
 * @param aMobilePhone Reference to an RMobilePhone subsession.
 * @param aPacketService Reference to an RPacketService
 *                       subsession.
 * @param aPacketContext Reference to an RPacketContext
 *                       subsession representing the primary
 *                       packet context.
 * @param aNum           The number of the context.
 */
    {
   	// Get network info from phone
	TExtEtelRequestStatus getNetworkStatus(aMobilePhone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getNetworkStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	aMobilePhone.GetCurrentNetwork(getNetworkStatus, infopckg);

	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getNetworkStatus, ETimeLong),
				  						    KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getNetworkStatus.Int(),
				  KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Currenlty there are three context configs types: TContextConfigGPRS, TContextConfigCDMA, TContextConfigR99_R4
	// A suitable config should be chosen accoriding to the baseband's capability.
	
	// Get the context config using network info
	// Using Gprs config for now...
	RPacketContext::TContextConfigGPRS config;
	TPtrC apn, user, pwd;
	GetGprsSettings(info, apn, user, pwd, aNum);
	config.iAccessPointName.Copy(apn);
	config.iProtocolConfigOption.iAuthInfo.iUsername.Copy(user);
	config.iProtocolConfigOption.iAuthInfo.iPassword.Copy(pwd);
	TPckg<RPacketContext::TContextConfigGPRS> configPk(config);

	// Set the context.
	TExtEtelRequestStatus setConfigStatus(aPacketContext, EPacketContextSetConfig);	
	CleanupStack::PushL(setConfigStatus);
	aPacketContext.SetConfig(setConfigStatus, configPk);
	
	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(setConfigStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::SetConfig timed out"));
	ASSERT_EQUALS(setConfigStatus.Int(), KErrNone,
					_L("RPacketContext::SetConfig returned with error status."));

	// ===  Activate packet context  ===

	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(aPacketContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	aPacketContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);

	// Activate packet context with RPacketContext::Activate
	TExtEtelRequestStatus contextActivateStatus(aPacketContext, EPacketContextActivate);
	CleanupStack::PushL(contextActivateStatus);
	aPacketContext.Activate(contextActivateStatus);

	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(contextActivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Activate timed out."));
	ASSERT_EQUALS(contextActivateStatus.Int(), KErrNone,
					_L("RPacketContext::Activate returned with error status."));

	// Check RPacketContext::NotifyStatusChange completes with EStatusActive
    RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusActive;
	WaitForPacketContextNotifyStatusChange(
				aPacketContext,
				notifyContextStChStatus,
				packetContextStatus, 
				expectedPacketContextStatus,
				KErrNone);
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, _L("Packet Context's status expected to be EStatusActive but is not."));

    CleanupStack::PopAndDestroy(4, &getNetworkStatus);
    }

