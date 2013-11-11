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

#include "tsimtsytesthelper.h" 
#include "cctsyinidata.h"

/**
* Constructor
*/	
TSimTsyTestHelper::TSimTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobilePhone::NotifyUSimApplicationsInfoChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifyUSimApplicationsInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TInt& aCount, 
				TInt aExpectedCount,
				RMobilePhone::TAID &aActiveAID,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyUSimApplicationsInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aCount != aExpectedCount )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyUSimApplicationsInfoChange(aRequestStatus, aCount, aActiveAID);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyUSimApplicationsInfoChange Wrong completion status"))	
	ASSERT_EQUALS(aCount, aExpectedCount, 
				_L("RMobilePhone::NotifyUSimApplicationsInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyUSimAppsSelectionModeChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifyUSimAppsSelectionModeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TUSimSelectionMode& aUSimSelectionMode, 
				RMobilePhone::TUSimSelectionMode aExpectedUSimSelectionMode,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyUSimAppsSelectionModeChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aUSimSelectionMode != aExpectedUSimSelectionMode )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyUSimAppsSelectionModeChange(aRequestStatus, aUSimSelectionMode);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyUSimAppsSelectionModeChange Wrong completion status"))	
	ASSERT_EQUALS(aUSimSelectionMode, aExpectedUSimSelectionMode, 
				_L("RMobilePhone::NotifyUSimAppsSelectionModeChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyUUSSettingChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifyUUSSettingChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneUUSSetting& aSetting, 
				RMobilePhone::TMobilePhoneUUSSetting aExpectedSetting,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyUUSSettingChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aSetting != aExpectedSetting )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyUUSSettingChange(aRequestStatus, aSetting);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyUUSSettingChange Wrong completion status"))	
	ASSERT_EQUALS(aSetting, aExpectedSetting, 
				_L("RMobilePhone::NotifyUUSSettingChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyIccMessageWaitingIndicatorsChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifyIccMessageWaitingIndicatorsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneMessageWaitingV1> &aMobilePhoneMessageWaitingV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneMessageWaitingV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneMessageWaitingV1Pckg pkgParam( aMobilePhoneMessageWaitingV1.GetValue() );
		aMobilePhone.NotifyIccMessageWaitingIndicatorsChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneMessageWaitingV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyIccMessageWaitingIndicatorsChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyImsAuthorizationInfoChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifyImsAuthorizationInfoChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyImsAuthorizationInfoChange did not complete"))
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyImsAuthorizationInfoChange Wrong completion status"))		
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifySmartCardApplicationInfoChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifySmartCardApplicationInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TAID& aAID, 
				RMobilePhone::TAID& aExpectedAID,
				RMobilePhone::TSmartCardApplicationEvent &aEvent,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifySmartCardApplicationInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aAID != aExpectedAID )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifySmartCardApplicationInfoChange(aRequestStatus, aAID, aEvent);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifySmartCardApplicationInfoChange Wrong completion status"))	
	ASSERT_EQUALS_DES8(aAID, aExpectedAID, 
				_L("RMobilePhone::NotifySmartCardApplicationInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMailboxNumbersChange
*/	
void TSimTsyTestHelper::WaitForMobilePhoneNotifyMailboxNumbersChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneVoicemailIdsV3> &aMobilePhoneVoicemailIdsV3,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMailboxNumbersChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneVoicemailIdsV3.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg pkgParam( aMobilePhoneVoicemailIdsV3.GetValue() );
		aMobilePhone.NotifyMailboxNumbersChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMailboxNumbersChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneVoicemailIdsV3.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyMailboxNumbersChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhoneStore::NotifyStoreEvent
*/	
void TSimTsyTestHelper::WaitForMobilePhoneStoreNotifyStoreEvent( 
				RMobilePhoneStore& aMobilePhoneStore,
				TEtelRequestBase& aRequestStatus,
				TUint32& aEvent, 
				TUint32 aExpectedEvent,
				TInt &aIndex,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhoneStore::NotifyStoreEvent did not complete"))

	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEvent != aExpectedEvent )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhoneStore.NotifyStoreEvent(aRequestStatus, aEvent, aIndex);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhoneStore::NotifyStoreEvent Wrong completion status"))	
	ASSERT_EQUALS(aEvent, aExpectedEvent, 
				_L("RMobilePhoneStore::NotifyStoreEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobileSmartCardEap::NotifyEapMethodAccessStatusChange
*/	
void TSimTsyTestHelper::WaitForMobileSmartCardEapNotifyEapMethodAccessStatusChange( 
				RMobileSmartCardEap& aMobileSmartCardEap,
				TEtelRequestBase& aRequestStatus,
				RMobileSmartCardEap::TEapMethodAccessStatus& aEapMethodStatus, 
				RMobileSmartCardEap::TEapMethodAccessStatus aExpectedEapMethodStatus,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobileSmartCardEap::NotifyEapMethodAccessStatusChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEapMethodStatus != aExpectedEapMethodStatus )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobileSmartCardEap.NotifyEapMethodAccessStatusChange(aRequestStatus, aEapMethodStatus);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobileSmartCardEap::NotifyEapMethodAccessStatusChange Wrong completion status"))	
	ASSERT_EQUALS(aEapMethodStatus, aExpectedEapMethodStatus, 
				_L("RMobileSmartCardEap::NotifyEapMethodAccessStatusChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}
#if 0
template<typename T>		
TInt TSimTsyTestHelper::GetNumberOfUsedEntriesInStoreL(	
		RMobilePhoneStore& aPhoneStore, 
		const TDesC& aStoreName,
		T& aInfoPkg,
		RMobilePhone& aPhone,
		TInt& aUsedEntries1, 
		TInt& aUsedEntries2)
/**
 * Uses two methods of getting the number of entries in the ICC SMS store.
 * 
 * @param aSmsStore SMS store subsession
 * @param aGetInfoError Error returned by RMobilePhoneStore::GetInfo. If this is  
 * KErrNone, aUsedEntries is valid.
 * @param aUsedEntries Number of used entries as returned by RMobilePhoneStore::GetInfo
 * @param aGetPhoneStoreInfoError Error returned by RMobilePhone::GetPhoneStoreInfo. If this is
 * KErrNone, this indicates that aUsedEntries2 is valid.
 * @param aUsedEntriews2 Number of used entries as returned by RMobilePhone::GetPhoneStoreInfo
 * 
 * @return KErrNone if one of the get info APIs are supported, KErrNotSupported if neither
 * are supported.
 */
	{
	// Get number of entries using RMobilePhoneStore::GetInfo
	TExtEtelRequestStatus getInfoStatus (aPhoneStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	aPhoneStore.GetInfo(getInfoStatus,aInfoPkg);
	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone,_L("RMobileSmsStore::GetInfo timed out"));
	if (getInfoStatus.Int() == KErrNone)
		{
		aUsedEntries1 = aInfoPkg().iUsedEntries;
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhoneStore::GetInfo returned with error = %d"), getInfoStatus.Int());
		}

	// Get number of entries using RMobilePhone::GetPhoneStoreInfo
	TExtEtelRequestStatus getInfoStatus2 (aPhoneStore,EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus2);	
	aPhone.GetPhoneStoreInfo(getInfoStatus2,aInfoPkg, aStoreName);
	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus2, ETimeMedium), KErrNone,_L("RMobilePhone::GetPhoneStoreInfo timed out"));
	if (getInfoStatus2.Int() == KErrNone)
		{
		aUsedEntries2 = aInfoPkg().iUsedEntries;
		}
	else
		{
		DEBUG_PRINTF2(_L("RMobilePhone::GetPhoneStoreInfo returned with error = %d"), getInfoStatus2.Int());
		}
	
	if (getInfoStatus.Int() == KErrNone || getInfoStatus2.Int() == KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		return KErrNone;
		}
	else
		{
		CleanupStack::PopAndDestroy(2);
		return KErrNotSupported;
		}
	}


TInt TSimTsyTestHelper::AssertNumberOfUsedEntriesCorrectL
						(RMobilePhoneStore& aPhoneStore, 
						const TDesC& aStoreName,
						T& aInfoPkg,
						RMobilePhone& aPhone,
						TInt aExpectedGetInfoEntries, 
						TInt aExpectedGetPhoneStoreInfoEntries)
/**
 * Call RMobilePhone::GetPhoneStoreInfo and RMobilePhoneStore::GetInfo  and checks
 * that the number of used entries in the phone book is the expected value if the
 * API is supported.
 * 
 * @param aPhoneStore Reference to phone store subsession.
 * @param aStoreName Store name from Etelmm
 * @param aExpectedGetInfoEntries A expected number of entries to be returned by 
 * RMobilePhoneStore::GetInfo
 * @param aExpectedGetPhoneStoreInfoEntries A expected number of entries to be
 * returned by RMobilePhone::GetPhoneStoreInfo
 * 
 * @return KErrNone if RMobilePhone::GetPhoneStoreInfo or RMobilePhoneStore::GetInfo
 * completed with KErrNone. KErrNotSupported otherwise.
 */
	{
	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is equal to aExpectedGetInfoEntries
	TExtEtelRequestStatus getInfoStatus (aPhoneStore, EMobilePhoneStoreGetInfo);	
	CleanupStack::PushL(getInfoStatus);
	aPhoneStore.GetInfo(getInfoStatus,aInfoPkg);
	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::GetInfo, Timed out"))	

	if (getInfoStatus.Int() == KErrNone)
		{
		ASSERT_EQUALS(getInfoStatus.Int(), KErrNone, _L("RMobilePhoneStore::GetInfo returned an error"))			
		ASSERT_EQUALS(aExpectedGetInfoEntries, (aInfoPkg().iUsedEntries), _L("RMobilePhoneStore::GetInfo Unexpected number of iUsedEntries returned" ))
		}
	
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is equal to aExpectedGetPhoneStoreInfoEntries
	TExtEtelRequestStatus getInfoStatus2 (aPhoneStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(getInfoStatus2);	
	aPhone.GetPhoneStoreInfo(getInfoStatus2,aInfoPkg, aStoreName);
	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus2, ETimeMedium), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out"))

	if (getInfoStatus2.Int() == KErrNone)
		{
		ASSERT_EQUALS(getInfoStatus2.Int(), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo returned an error"))			
		ASSERT_EQUALS(aExpectedGetPhoneStoreInfoEntries, (aInfoPkg().iUsedEntries ), _L("RMobilePhone::GetPhoneStoreInfo Unexpected number of iUsedEntries returned" ))
		}
	
	if (getInfoStatus.Int() == KErrNone || getInfoStatus2.Int() == KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		return KErrNone;
		}
	else
		{
		CleanupStack::PopAndDestroy(2);
		return KErrNotSupported;
		}
	}
#endif

void TSimTsyTestHelper::VerifyPin2L(RMobilePhone& aMobilePhone, TNetworkTsyTestHelper& aNetworkTsyTestHelper)
/**
 * member function for enabling pin2 for FDN phone book
 * @param aMobilePhone reference to RMobilePhone
 * @param aNetworkTsyTestHelper reference to TNetworkTsyTestHelper
 *
 * @return void.
 */
	{

	const TDesC* networkSection = NULL;
	aNetworkTsyTestHelper.GetNetworkPasswordsSectionL(aMobilePhone,networkSection);
	
	// Get the password from the ini file
	TPtrC password; 
	CHECK_TRUE_L(iTestStep.GetStringFromConfig(*networkSection, KIniPin2, password) != EFalse, 
			_L("GetStringFromConfig returned with an error"));

	TExtEtelRequestStatus notifySecurityEventStatus(aMobilePhone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL( notifySecurityEventStatus );
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	aMobilePhone.NotifySecurityEvent(notifySecurityEventStatus, securityEvent);

	TExtEtelRequestStatus verifySecurityCodeStatus(aMobilePhone, EMobilePhoneVerifySecurityCode);
	CleanupStack::PushL( verifySecurityCodeStatus );
	RMobilePhone::TMobilePassword dummyUnblockCode; 
	
	aMobilePhone.VerifySecurityCode( verifySecurityCodeStatus, RMobilePhone::ESecurityCodePin2, 
								password, dummyUnblockCode );
							
	CHECK_EQUALS_L(iTestStep.WaitForRequestWithTimeOut(verifySecurityCodeStatus, ETimeMedium), 
			KErrNone, _L("RMobilePhone::VerifySecurityCode timed-out"));
	CHECK_EQUALS_L( verifySecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::VerifySecurityCode returned with an error"));
	
	TInt err = iTestStep.WaitForRequestWithTimeOut(notifySecurityEventStatus, ETimeShort);	
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::NotifySecurityEvent did not complete"))
	
	while (	err == KErrNone && notifySecurityEventStatus.Int() == KErrNone  	
							&& securityEvent != RMobilePhone::EPin2Verified )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifySecurityEvent(notifySecurityEventStatus, securityEvent);
		err = iTestStep.WaitForRequestWithTimeOut(notifySecurityEventStatus, ETimeShort);
		}
		
	CHECK_EQUALS_L(notifySecurityEventStatus.Int(), KErrNone, _L("RMobilePhone::NotifySecurityEvent Wrong completion status"))	
	CHECK_EQUALS_L(securityEvent, RMobilePhone::EPin2Verified, 
				_L("RMobilePhone::NotifySecurityEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (notifySecurityEventStatus.Int() == KRequestPending)	
		{
		notifySecurityEventStatus.Cancel();
		}
	
	CleanupStack::PopAndDestroy(2,&notifySecurityEventStatus);

	}
