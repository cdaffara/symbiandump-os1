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

#ifndef TPHONETSYTESTHELPER_H
#define TPHONETSYTESTHELPER_H


#include "ttsytesthelperbase.h"
	
class TPhoneTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TPhoneTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobilePhoneNotifyCostCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyIccAccessCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifySecurityCapsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TUint32& aCaps, 
				TUint32 aWantedCaps,
				TUint32 aUnwantedCaps,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifySecurityEvent( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneSecurityEvent& aEvent, 
				RMobilePhone::TMobilePhoneSecurityEvent aExpectedEvent,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifySignalStrengthChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TInt32& aSignalStrength, 
				TInt32 aExpectedSignalStrength,
				TInt8 &aBar,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyLockInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneLock &aLock,
				RMobilePhone::TMobilePhoneLock aExpectedLock,
				TCmpBase<RMobilePhone::TMobilePhoneLockInfoV1> &aMobilePhoneLockInfoV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyNITZInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneNITZ& aNITZInfo, 
				RMobilePhone::TMobilePhoneNITZ& aExpectedNITZInfo,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyNetworkSelectionSettingChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneNetworkSelectionV1> &aMobilePhoneNetworkSelectionV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifySecurityCodeInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneSecurityCode &aSecurityCode,
				TCmpBase<RMobilePhone::TMobilePhoneSecurityCodeInfoV5> &aMobilePhoneSecurityCodeInfoV5,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMmsConfig( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMmsConnParams& aType, 
				RMobilePhone::TMmsConnParams aExpectedType,
				TDes8 &aConnectivity,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMmsUpdate( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMmsNotificationV3> &aMmsNotificationV3,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyBatteryInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneBatteryInfoV1> &aPhoneBatteryInfoComp,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyDefaultPrivacyChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhonePrivacy& aSetting, 
				RMobilePhone::TMobilePhonePrivacy aExpectedSetting,
				TInt aWantedStatus );
	
	void WaitForPhoneBookStoreCacheReadyL( 
						RMmCustomAPI& aMmCustomAPI,
						TPhoneBookStoreId aPhoneBookId );
	void BlockPinL(RMobilePhone& aMobilePhone,RMobilePhone::TMobilePhoneSecurityCode aPhoneSecurityCode);
	void ChangePhoneLockSettingL(RMobilePhone& aMobilePhone, RMobilePhone::TMobilePhoneLock aLock,
							RMobilePhone::TMobilePhoneLockSetting aLockSetting, TDesC& aPasswd, TInt aErrorCode = KErrNone);
	TInt SetPhoneFdnSettingL( RMobilePhone &aPhone, 
								RMobilePhone::TMobilePassword& aPIN2Password,
								RMobilePhone::TMobilePhoneFdnSetting aFdnSetting );
	
	}; // class TPhoneTsyTestHelper 

class TFdnCleanupDisable
	{
public:
	TFdnCleanupDisable(TPhoneTsyTestHelper &aBase,
			           RMobilePhone &aPhone,
			           RMobilePhone::TMobilePassword& aPIN2Password);
	operator TCleanupItem();

private:
	TPhoneTsyTestHelper &iBase;
	RMobilePhone &iPhone;
	RMobilePhone::TMobilePassword &iPIN2Password;

	static void DoCleanupL(TAny* aPtr);
	void CleanupL();
	};


#endif
