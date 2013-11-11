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

#include "tphonetsytesthelper.h" 
#include <ctsy/rmmcustomapi.h>

/**
* Constructor
*/	
TPhoneTsyTestHelper::TPhoneTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep)
			: TTsyTestHelperBase( aTestStep )
	{
	}
/**
* Notify reset for RMobilePhone::NotifyCostCapsChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyCostCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyCostCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobilePhone.NotifyCostCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobilePhone::NotifyCostCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobilePhone::NotifyCostCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyIccAccessCapsChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyIccAccessCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyIccAccessCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobilePhone.NotifyIccAccessCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobilePhone::NotifyIccAccessCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobilePhone::NotifyIccAccessCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifySecurityCapsChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifySecurityCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifySecurityCapsChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  
							&& ( ((aCaps & aWantedCaps) != aWantedCaps)
								 || ((aCaps & aUnwantedCaps) != 0) ) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.	
		aMobilePhone.NotifySecurityCapsChange(aRequestStatus, aCaps);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, 
				_L("RMobilePhone::NotifySecurityCapsChange Wrong completion status"))	
	ASSERT_BITS_SET(aCaps, aWantedCaps, aUnwantedCaps, 
				_L("RMobilePhone::NotifySecurityCapsChange Wrong caps"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifySecurityEvent
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifySecurityEvent( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneSecurityEvent& aEvent, 
				RMobilePhone::TMobilePhoneSecurityEvent aExpectedEvent,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifySecurityEvent did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aEvent != aExpectedEvent )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifySecurityEvent(aRequestStatus, aEvent);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifySecurityEvent Wrong completion status"))	
	ASSERT_EQUALS(aEvent, aExpectedEvent, 
				_L("RMobilePhone::NotifySecurityEvent Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifySignalStrengthChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifySignalStrengthChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TInt32& aSignalStrength, 
				TInt32 aExpectedSignalStrength,
				TInt8 &aBar,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifySignalStrengthChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aSignalStrength != aExpectedSignalStrength )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifySignalStrengthChange(aRequestStatus, aSignalStrength, aBar);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifySignalStrengthChange Wrong completion status"))	
	ASSERT_EQUALS(aSignalStrength, aExpectedSignalStrength, 
				_L("RMobilePhone::NotifySignalStrengthChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyLockInfoChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyLockInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneLock &aLock,
				RMobilePhone::TMobilePhoneLock aExpectedLock,
				TCmpBase<RMobilePhone::TMobilePhoneLockInfoV1> &aMobilePhoneLockInfoV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeMedium);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyLockInfoChange did not complete"))
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone	)
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		DEBUG_PRINTF3(_L("requestStatus: %d, aLock:%d, "),aRequestStatus.Int(),aLock)
		DEBUG_PRINTF4(_L("setting : %d, status:%d, isEqual : %d"),aMobilePhoneLockInfoV1.GetValue().iSetting,aMobilePhoneLockInfoV1.GetValue().iStatus, aMobilePhoneLockInfoV1.IsEqual(ENoLogError));
		if( aLock == aExpectedLock && aMobilePhoneLockInfoV1.IsEqual(ENoLogError)  )
			{
			break;
			}
		RMobilePhone::TMobilePhoneLockInfoV1Pckg pkgParam( aMobilePhoneLockInfoV1.GetValue() );
		aMobilePhone.NotifyLockInfoChange(aRequestStatus, aLock, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeLong);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyLockInfoChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneLockInfoV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyLockInfoChange Wrong lock information was returned"))	
	ASSERT_EQUALS(aLock, aExpectedLock, 
		_L("RMobilePhone::NotifyLockInfoChange Wrong lock was returned"))
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyNITZInfoChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyNITZInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNITZ& aNITZInfo, 
				RMobilePhone::TMobilePhoneNITZ& aExpectedNITZInfo,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNITZInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& (0 == memcmp(&aNITZInfo, &aExpectedNITZInfo, sizeof(aNITZInfo))) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyNITZInfoChange(aRequestStatus, aNITZInfo);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyNITZInfoChange Wrong completion status"))	
	ASSERT_TRUE((0 == memcmp(&aNITZInfo, &aExpectedNITZInfo, sizeof(aNITZInfo))), 
				_L("RMobilePhone::NotifyNITZInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyNetworkSelectionSettingChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyNetworkSelectionSettingChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkSelectionV1> &aMobilePhoneNetworkSelectionV1,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyNetworkSelectionSettingChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneNetworkSelectionV1.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg pkgParam( aMobilePhoneNetworkSelectionV1.GetValue() );
		aMobilePhone.NotifyNetworkSelectionSettingChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyNetworkSelectionSettingChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneNetworkSelectionV1.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyNetworkSelectionSettingChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifySecurityCodeInfoChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifySecurityCodeInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneSecurityCode &aSecurityCode,
				TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> &aMobilePhoneSecurityCodeInfoV5,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifySecurityCodeInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMobilePhoneSecurityCodeInfoV5.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg pkgParam( aMobilePhoneSecurityCodeInfoV5.GetValue() );
		aMobilePhone.NotifySecurityCodeInfoChange(aRequestStatus, aSecurityCode, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifySecurityCodeInfoChange Wrong completion status"))	
	ASSERT_TRUE(aMobilePhoneSecurityCodeInfoV5.IsEqual(ELogError), 
				_L("RMobilePhone::NotifySecurityCodeInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMmsConfig
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyMmsConfig( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMmsConnParams& aType, 
				RMobilePhone::TMmsConnParams aExpectedType,
				TDes8 &aConnectivity,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMmsConfig did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aType != aExpectedType )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyMmsConfig(aRequestStatus, aType, aConnectivity);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMmsConfig Wrong completion status"))	
	ASSERT_EQUALS(aType, aExpectedType, 
				_L("RMobilePhone::NotifyMmsConfig Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyMmsUpdate
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyMmsUpdate( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMmsNotificationV3> &aMmsNotificationV3,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyMmsUpdate did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aMmsNotificationV3.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.		
		RMobilePhone::TMmsNotificationV3Pckg pkgParam( aMmsNotificationV3.GetValue() );
		aMobilePhone.NotifyMmsUpdate(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}

	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyMmsUpdate Wrong completion status"))	
	ASSERT_TRUE(aMmsNotificationV3.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyMmsUpdate Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyBatteryInfoChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyBatteryInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneBatteryInfoV1> &aPhoneBatteryInfoComp,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyBatteryInfoChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& !aPhoneBatteryInfoComp.IsEqual(ENoLogError) )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		RMobilePhone::TMobilePhoneBatteryInfoV1 pkgParam(aPhoneBatteryInfoComp.GetValue());
		aMobilePhone.NotifyBatteryInfoChange(aRequestStatus, pkgParam);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyBatteryInfoChange Wrong completion status"))	
	ASSERT_TRUE(aPhoneBatteryInfoComp.IsEqual(ELogError), 
				_L("RMobilePhone::NotifyBatteryInfoChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

		
/**
* Notify reset for RMobilePhone::NotifyDefaultPrivacyChange
*/	
void TPhoneTsyTestHelper::WaitForMobilePhoneNotifyDefaultPrivacyChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhonePrivacy& aSetting, 
				RMobilePhone::TMobilePhonePrivacy aExpectedSetting,
				TInt aWantedStatus )

	{
	// Wait for the request to complete	
	TInt err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::NotifyDefaultPrivacyChange did not complete"))
	
	while (	err == KErrNone && aRequestStatus.Int() == KErrNone  	
							&& aSetting != aExpectedSetting )
		{
		// Request has completed with incorrect result. Consume any outstanding
		// Repost notification until timeout or we get the right result.
		
		aMobilePhone.NotifyDefaultPrivacyChange(aRequestStatus, aSetting);
		err = iTestStep.WaitForRequestWithTimeOut(aRequestStatus, ETimeShort);
		}
		
	ASSERT_EQUALS(aRequestStatus.Int(), aWantedStatus, _L("RMobilePhone::NotifyDefaultPrivacyChange Wrong completion status"))	
	ASSERT_EQUALS(aSetting, aExpectedSetting, 
				_L("RMobilePhone::NotifyDefaultPrivacyChange Wrong result"))	
	
	// Cancel request if it is still pending	
	if (aRequestStatus.Int() == KRequestPending)	
		{
		aRequestStatus.Cancel();
		}
	}

void TPhoneTsyTestHelper::WaitForPhoneBookStoreCacheReadyL( 
						RMmCustomAPI& aMmCustomAPI,
						TPhoneBookStoreId aPhoneBookId )
/**
 * helper function to check if the phone book store cache is ready, if not wait till ready 
 * 
 * @param aMmCustomAPI a reference to RMmCustomAPI session
 * @param aPhoneBookId a TPhoneBookStoreId  type parameter holding the phonebook type adn,fdn etc
 *
 * @return void
 */
	{
	if(aPhoneBookId != KIccAdnPhoneBook && aPhoneBookId != KIccFdnPhoneBook)
		{
		return;
		}
	TName phoneBookName;
	if(aPhoneBookId  == KIccAdnPhoneBook)
		{
		phoneBookName = KETelIccAdnPhoneBook;
		}
	else if(aPhoneBookId  == KIccFdnPhoneBook)
		{
		phoneBookName = KETelIccFdnPhoneBook;
		}

	// check if phonebook is cached already
	TExtEtelRequestStatus getPndCacheStatusStatus(aMmCustomAPI,ECustomGetPndCacheStatusIPC);
	RMmCustomAPI::TPndCacheStatus pndCacheStatus;
	aMmCustomAPI.GetPndCacheStatus(getPndCacheStatusStatus,pndCacheStatus,phoneBookName);
	TInt err = iTestStep.WaitForRequestWithTimeOut(getPndCacheStatusStatus, ETimeMedium);
	CHECK_EQUALS_L(err, KErrNone, _L("RMmCustomAPI::GetPndCacheStatus did not complete"))
	CHECK_EQUALS_L(getPndCacheStatusStatus.Int(), KErrNone, _L("RMmCustomAPI::GetPndCacheStatus wrong completion status"))	
	if( pndCacheStatus == RMmCustomAPI::ECacheReady)
		{
		return;
		}

	//wait for notification
	TExtEtelRequestStatus notifyPndCacheReadyStatus(aMmCustomAPI,ECustomNotifyPndCacheReadyIPC);
	aMmCustomAPI.NotifyPndCacheReady(notifyPndCacheReadyStatus,phoneBookName);
	err = iTestStep.WaitForRequestWithTimeOut(notifyPndCacheReadyStatus, ETimeLong);	
	CHECK_EQUALS_L(err, KErrNone, _L("RMmCustomAPI::NotifyPndCacheReady did not complete"))
	CHECK_EQUALS_L(notifyPndCacheReadyStatus.Int(), KErrNone, _L("RMmCustomAPI::NotifyPndCacheReady wrong completion status"))	
	
	aMmCustomAPI.GetPndCacheStatus(getPndCacheStatusStatus,pndCacheStatus,phoneBookName);
	err = iTestStep.WaitForRequestWithTimeOut(getPndCacheStatusStatus, ETimeMedium);
	CHECK_EQUALS_L(err, KErrNone, _L("RMmCustomAPI::GetPndCacheStatus did not complete"))
	CHECK_EQUALS_L(getPndCacheStatusStatus.Int(), KErrNone, _L("RMmCustomAPI::GetPndCacheStatus wrong completion status"))	
	CHECK_EQUALS_L(pndCacheStatus, RMmCustomAPI::ECacheReady,_L("RMmCustomAPI::GetPndCacheStatus returned incorrect status"))
	
	}

void TPhoneTsyTestHelper::BlockPinL(RMobilePhone& aMobilePhone,RMobilePhone::TMobilePhoneSecurityCode aPhoneSecurityCode)
/**
 * member function for blocking pin1/pin2 returing void
 * 
 * @param aMobilePhone a reference to RMobilePhone object
 * @param aPhoneSecurityCode holds the pin type to be bolcked
 *
 * @return void
 */
	{
	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 getSecurityCodeInfo;
	do
		{
		TExtEtelRequestStatus getSecurityCodeInfoStatus(aMobilePhone, EMobilePhoneGetSecurityCodeInfo);
		CleanupStack::PushL( getSecurityCodeInfoStatus );		
		RMobilePhone::TMobilePhoneSecurityCodeInfoV5Pckg getSecurityCodeInfoPckg( getSecurityCodeInfo );
		aMobilePhone.GetSecurityCodeInfo(getSecurityCodeInfoStatus, aPhoneSecurityCode, getSecurityCodeInfoPckg);
		ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getSecurityCodeInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo timed out"));
		ASSERT_EQUALS(getSecurityCodeInfoStatus.Int(), KErrNone, _L("RMobilePhone::GetSecurityCodeInfo returned an error"));
	
		TExtEtelRequestStatus verifySecurityCodeStatus(aMobilePhone, EMobilePhoneVerifySecurityCode);
		CleanupStack::PushL( verifySecurityCodeStatus );
		TPtrC wrongPin = _L("abcdef");
		RMobilePhone::TMobilePassword dummyUnblockCode; 
		aMobilePhone.VerifySecurityCode( verifySecurityCodeStatus, aPhoneSecurityCode, 
							wrongPin, dummyUnblockCode );
		ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(verifySecurityCodeStatus, ETimeLong), 
				KErrNone, _L("RMobilePhone::VerifySecurityCode timed-out"));
		ASSERT_TRUE( verifySecurityCodeStatus.Int() != KErrNone, _L("RMobilePhone::VerifySecurityCode returned KErrNone"));
			
		CleanupStack::PopAndDestroy(2,&getSecurityCodeInfoStatus);
				
		} while (getSecurityCodeInfo.iRemainingEntryAttempts != 0);	

	}



void TPhoneTsyTestHelper::ChangePhoneLockSettingL(RMobilePhone& aMobilePhone,
		RMobilePhone::TMobilePhoneLock aLock,RMobilePhone::TMobilePhoneLockSetting aLockSetting, 
		TDesC& aPasswd, TInt aErrorCode)
/**
 * member function for changing the lock setting returing void
 * 
 * @param aMobilePhone a reference to RMobilePhone object
 * @param aLock holding the lock information for which the setting needs to be changed
 * @param aLockSetting holding the state to be set for the lock
 * @param aPasswd holding the password to be provided while changing lock setting
 * @param aErrorCode the error code that we expect to recieve

 * @return void
 */
	{

	//post notification
	TExtEtelRequestStatus notifySecurityEventStatus(aMobilePhone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL( notifySecurityEventStatus );
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	aMobilePhone.NotifySecurityEvent(notifySecurityEventStatus, securityEvent);

    TExtEtelRequestStatus setLockSettingStatus(aMobilePhone, EMobilePhoneSetLockSetting);
	CleanupStack::PushL(setLockSettingStatus);
	aMobilePhone.SetLockSetting(setLockSettingStatus, aLock, aLockSetting);

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin1;
	RMobilePhone::TMobilePhoneSecurityEvent pinRequired = RMobilePhone::EPin1Required;
	RMobilePhone::TMobilePhoneSecurityEvent pinVerified = RMobilePhone::EPin1Verified;
	
	if(aLock == RMobilePhone::ELockICC)
		{
		secCode = RMobilePhone::ESecurityCodePin1;
		pinRequired = RMobilePhone::EPin1Required;
		pinVerified = RMobilePhone::EPin1Verified;
		}
	else if(aLock == RMobilePhone::ELockPin2)
		{
		secCode = RMobilePhone::ESecurityCodePin2;
		pinRequired = RMobilePhone::EPin2Required;
		pinVerified = RMobilePhone::EPin2Verified;
		}
	else if(aLock == RMobilePhone::ELockPhoneDevice)
		{		
		secCode = RMobilePhone::ESecurityCodePhonePassword;
		pinRequired = RMobilePhone::EPhonePasswordRequired;
		pinVerified = RMobilePhone::EPhonePasswordVerified;
		}
	else
		{
		ASSERT_EQUALS(1,0,_L("RMobilePhone::TMobilePhoneLock of this type not supported"));
		}
	
	TExtEtelRequestStatus verifySecurityCodeStatus(aMobilePhone, EMobilePhoneVerifySecurityCode);
	CleanupStack::PushL( verifySecurityCodeStatus );
	WaitForMobilePhoneNotifySecurityEvent(aMobilePhone,notifySecurityEventStatus,
			securityEvent,
			pinRequired,
			KErrNone);

	RMobilePhone::TMobilePassword dummyUnblockCode; 

	if( pinRequired == securityEvent )
		{
			
		aMobilePhone.NotifySecurityEvent(notifySecurityEventStatus, securityEvent);
		aMobilePhone.VerifySecurityCode( verifySecurityCodeStatus, secCode, 
			aPasswd, dummyUnblockCode );
		if(aErrorCode == KErrNone)
			{
			ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(verifySecurityCodeStatus, ETimeLong),
					KErrNone, _L("RMobilePhone::VerifySecurityCode timed-out"));
			ASSERT_EQUALS( verifySecurityCodeStatus.Int(), KErrNone, _L("RMobilePhone::VerifySecurityCode returned with an error"));
			WaitForMobilePhoneNotifySecurityEvent(aMobilePhone,notifySecurityEventStatus,
					securityEvent,
					pinVerified,
					KErrNone);
			}
		}
	ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(setLockSettingStatus, ETimeLong), KErrNone, _L("RMobilePhone::SetLockSetting timed out"));
	ASSERT_EQUALS(setLockSettingStatus.Int(), aErrorCode, _L("RMobilePhone::SetLockSetting did not returned as expected"));


	// notifySecurityEventStatus
	// setLockSettingStatus
	// verifySecurityCodeStatus
	CleanupStack::PopAndDestroy(3,&notifySecurityEventStatus);
	
	}

/**
 * Set FDN setting on or off (i.e. enable or disable FDN for the phone)
 * @param aPhone RMobilePhone session
 * @param aPIN2Password PIN2 password
 * @param aFdnSetting Either EFdnSetOn or EFdnSetOff
 * @return KErrNone or an error code from RMobilePhone::SetFdnSetting
 */
TInt TPhoneTsyTestHelper::SetPhoneFdnSettingL( RMobilePhone &aPhone, 
								RMobilePhone::TMobilePassword& aPIN2Password,
								RMobilePhone::TMobilePhoneFdnSetting aFdnSetting )
	{
	// Set up the security notification
	TExtEtelRequestStatus reqNotifySecEvent(aPhone, EMobilePhoneNotifySecurityEvent);
	CleanupStack::PushL(reqNotifySecEvent);
	RMobilePhone::TMobilePhoneSecurityEvent securityEvent;
	
	aPhone.NotifySecurityEvent(reqNotifySecEvent, securityEvent);

	// Call RMobilePhone::SetFdnSetting with RMobilePhone::EFdnSetOn
	TExtEtelRequestStatus reqSetFdnStatus( aPhone, EMobilePhoneSetFdnSetting);
	CleanupStack::PushL(reqSetFdnStatus);
	
	aPhone.SetFdnSetting(reqSetFdnStatus, aFdnSetting);

	// We don't test for this request completing here, as we expect it to remain pending while the PIN2
	// verify completes
	
	// ===  Check security events complete and verify PIN2 ===
	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Required
	WaitForMobilePhoneNotifySecurityEvent(aPhone,reqNotifySecEvent,
			securityEvent,
			RMobilePhone::EPin2Required,
			KErrNone);
	// Just for the log
	ASSERT_EQUALS(securityEvent,RMobilePhone::EPin2Required, 
							_L("RMobilePhone::NotifySecurityEvent returned an unexpected result") );

	// re-launch notification
	aPhone.NotifySecurityEvent(reqNotifySecEvent, securityEvent);
	
	// Verify PIN2 security code with RMobilePhone::VerifySecurityCode with RMobilePhone::ESecurityCodePin2 
	TExtEtelRequestStatus reqVerifySecCode(aPhone, EMobilePhoneVerifySecurityCode);
	CleanupStack::PushL(reqVerifySecCode);
	RMobilePhone::TMobilePassword dummyUnblockCode;
	DEBUG_PRINTF3(_L("REREREEEE %d %S"),__LINE__,&aPIN2Password);
	aPhone.VerifySecurityCode(reqVerifySecCode, RMobilePhone::ESecurityCodePin2, 
									aPIN2Password, dummyUnblockCode);
							
	TInt err = iTestStep.WaitForRequestWithTimeOut(reqVerifySecCode,ETimeMedium);
	DEBUG_PRINTF2(_L("REREREEEE %d"),__LINE__);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::VerifySecurityCode timed out"));
	ASSERT_EQUALS(reqVerifySecCode.Int(), KErrNone, _L("RMobilePhone::VerifySecurityCode returned an error"));

	// Check RMobilePhone::NotifySecurityEvent completes with RMobilePhone::EPin2Verified
	WaitForMobilePhoneNotifySecurityEvent(aPhone,reqNotifySecEvent,
			securityEvent,
			RMobilePhone::EPin2Verified,
			KErrNone);

	// Just for the log
	ASSERT_EQUALS(securityEvent,RMobilePhone::EPin2Verified, 
							_L("RMobilePhone::NotifySecurityEvent returned an unexpected result") );

	// Now wait for the FDN setting to complete
	err = iTestStep.WaitForRequestWithTimeOut(reqSetFdnStatus,ETimeMedium);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetFdnSetting timed out"));

	TInt result = reqSetFdnStatus.Int();
	
	// reqVerifySecCode
	// reqSetFdnStatus
	// reqNotifySecEvent
	CleanupStack::PopAndDestroy(3,&reqNotifySecEvent);
		
	return result;
	
	}



TFdnCleanupDisable::TFdnCleanupDisable(TPhoneTsyTestHelper &aBase,
		RMobilePhone &aPhone,
        RMobilePhone::TMobilePassword& aPIN2Password):iBase(aBase),iPhone(aPhone),iPIN2Password(aPIN2Password)
/*
 * Constructor
 * @param aBase reference to the helper.
 * @param aPhone reference to a phone session
 * @param aPIN2Password a reference to the pin2 password
 *
 */
    {
    }

void TFdnCleanupDisable::DoCleanupL(TAny* aPtr)
/*
 * This function is a static clean up function to be saved in the cleanup stack. Its disable the FDN
 *
 * @param aPtr a pointer to a TBackupFdnDisable instance.
 *
 */	
	{
	TFdnCleanupDisable* me = static_cast<TFdnCleanupDisable*>(aPtr);
	me->CleanupL();
	}

TFdnCleanupDisable::operator TCleanupItem()
/*
 * This operator create a TCleanupItem to save a pointer to the DoCleanup static function and this in the cleanup stack
 *
 */	
    {
	return TCleanupItem(DoCleanupL,this);
    }


void TFdnCleanupDisable::CleanupL()
/*
 * This function disable the FDN
 *
 */	
    {
    iBase.SetPhoneFdnSettingL(iPhone,iPIN2Password,RMobilePhone::EFdnSetOff);
	}
