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

#ifndef TSIMTSYTESTHELPER_H
#define TSIMTSYTESTHELPER_H


#include "ttsytesthelperbase.h"
#include "tnetworktsytesthelper.h"

class TSimTsyTestHelper : public TTsyTestHelperBase
	{
public:
	TSimTsyTestHelper(CCTSYIntegrationTestSuiteStepBase& aTestStep);

	void WaitForMobilePhoneNotifyUSimApplicationsInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TInt& aCount, 
				TInt aExpectedCount,
				RMobilePhone::TAID &aActiveAID,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyUSimAppsSelectionModeChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TUSimSelectionMode& aUSimSelectionMode, 
				RMobilePhone::TUSimSelectionMode aExpectedUSimSelectionMode,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyUUSSettingChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TMobilePhoneUUSSetting& aSetting, 
				RMobilePhone::TMobilePhoneUUSSetting aExpectedSetting,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyIccMessageWaitingIndicatorsChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneMessageWaitingV1> &aMobilePhoneMessageWaitingV1,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyImsAuthorizationInfoChange( 
				TEtelRequestBase& aRequestStatus,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifySmartCardApplicationInfoChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				RMobilePhone::TAID& aAID, 
				RMobilePhone::TAID& aExpectedAID,
				RMobilePhone::TSmartCardApplicationEvent &aEvent,
				TInt aWantedStatus );

	void WaitForMobilePhoneNotifyMailboxNumbersChange( 
				RMobilePhone& aMobilePhone,
				TEtelRequestBase& aRequestStatus,
				TCmpBase<RMobilePhone::TMobilePhoneVoicemailIdsV3> &aMobilePhoneVoicemailIdsV3,
				TInt aWantedStatus );

	void WaitForMobilePhoneStoreNotifyStoreEvent( 
				RMobilePhoneStore& aMobilePhoneStore,
				TEtelRequestBase& aRequestStatus,
				TUint32& aEvent, 
				TUint32 aExpectedEvent,
				TInt &aIndex,
				TInt aWantedStatus );

	void WaitForMobileSmartCardEapNotifyEapMethodAccessStatusChange( 
				RMobileSmartCardEap& aMobileSmartCardEap,
				TEtelRequestBase& aRequestStatus,
				RMobileSmartCardEap::TEapMethodAccessStatus& aEapMethodStatus, 
				RMobileSmartCardEap::TEapMethodAccessStatus aExpectedEapMethodStatus,
				TInt aWantedStatus );

	template<typename T>
	TInt GetNumberOfUsedEntriesInStoreL(	
			RMobilePhoneStore& aPhoneStore,			
			const TDesC& aStoreName,
			T& aInfoPkg,
			RMobilePhone& aPhone,
			TInt& aUsedEntries1, 
			TInt& aUsedEntries2)  
	/**
	 * Uses two methods of getting the number of entries in the ICC SMS/phonebook store.
	 * 
	 * @param aPhoneStore reference to phone store object for GetInfo API
	 * @param aStoreName const reference to TDesC holding the name of the store, for which information is required 
	 * @param aInfoPkg type TMobilePhoneStoreInfoV1 or one of its derived classes' package types to hold phone store information
	 * @param aPhone reference to mobile phone object for GetPhoneStoreInfo API
	 * @param aUsedEntries1 reference of TInt type to hold iUsedEntries returned by GetInfo API
 	 * @param aUsedEntries2 reference of TInt type to hold iUsedEntries returned by GetPhoneStoreInfo API
	 * 
	 * @return KErrNone if one of the get info APIs are supported, KErrNotSupported if neither
	 * are supported.
	 */
		{	// Get number of entries using RMobilePhoneStore::GetInfo
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
;
	
	template<typename T>
	TInt AssertNumberOfUsedEntriesCorrectL(
			RMobilePhoneStore& aPhoneStore,
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
	 * @param aPhoneStore reference to phone store object for GetInfo API
	 * @param aStoreName const reference to TDesC holding the name of the store, for which information is required 
	 * @param aInfoPkg type TMobilePhoneStoreInfoV1 or one of its derived classes' package types to hold phone store information
	 * @param aPhone reference to mobile phone object for GetPhoneStoreInfo API
	 * @param aExpectedGetInfoEntries holding value for comparision with iUsedEntries returned by GetInfo API
 	 * @param aExpectedGetPhoneStoreInfoEntries holding value for comparision with iUsedEntries returned by GetPhoneStoreInfo API
	 * 
	 * @return KErrNone if one of the get info APIs are supported, KErrNotSupported if neither
	 * are supported.
	 */
		{
		// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is equal to aExpectedGetInfoEntries
		TExtEtelRequestStatus getInfoStatus (aPhoneStore, EMobilePhoneStoreGetInfo);	
		CleanupStack::PushL(getInfoStatus);
		aPhoneStore.GetInfo(getInfoStatus,aInfoPkg);
		ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::GetInfo, Timed out"));

		if (getInfoStatus.Int() == KErrNone)
			{
			ASSERT_EQUALS(getInfoStatus.Int(), KErrNone, _L("RMobilePhoneStore::GetInfo returned an error"));
			ASSERT_EQUALS((aInfoPkg().iUsedEntries),aExpectedGetInfoEntries,  _L("RMobilePhoneStore::GetInfo Unexpected number of iUsedEntries returned" ));
			}
		
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is equal to aExpectedGetPhoneStoreInfoEntries
		TExtEtelRequestStatus getInfoStatus2 (aPhoneStore, EMobilePhoneStoreGetInfo);
		CleanupStack::PushL(getInfoStatus2);	
		aPhone.GetPhoneStoreInfo(getInfoStatus2,aInfoPkg, aStoreName);
		ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus2, ETimeMedium), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out"));

		if (getInfoStatus2.Int() == KErrNone)
			{
			ASSERT_EQUALS(getInfoStatus2.Int(), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo returned an error"));
			ASSERT_EQUALS((aInfoPkg().iUsedEntries ),aExpectedGetPhoneStoreInfoEntries,  _L("RMobilePhone::GetPhoneStoreInfo Unexpected number of iUsedEntries returned" ));
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
	template<class INFO>
	TInt EnsureEmptyStoreSpaceL(RMobilePhoneStore& aPhoneStore,			
			const TDesC& aStoreName,
			INFO &aInfo,
			RMobilePhone& aPhone)
	/**
	 * Check the number of used entries in the store, and if all the entries are used, erase the last one.
	 * 
	 * @param aPhoneStore reference to phone store object for GetInfo API
	 * @param aStoreName const reference to TDesC holding the name of the store, for which information is required 
	 * @param aInfo type TMobilePhoneStoreInfoV1 or one of its derived classes' package types to hold phone store information
	 * @param aPhone reference to mobile phone object for GetPhoneStoreInfo API
	 * 
	 * @return an index to an empty entry in the store; negative number if fails.
	 */
		{
		TPckg<INFO> infoPckg(aInfo);
		TInt ret=0;
		// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is equal to aExpectedGetInfoEntries
		TExtEtelRequestStatus getInfoStatus (aPhoneStore, EMobilePhoneStoreGetInfo);	
		CleanupStack::PushL(getInfoStatus);
		TExtEtelRequestStatus getInfoStatus2 (aPhoneStore, EMobilePhoneStoreGetInfo);
		CleanupStack::PushL(getInfoStatus2);	
		aPhoneStore.GetInfo(getInfoStatus,infoPckg);
		ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::GetInfo, Timed out"));
		if (getInfoStatus.Int() != KErrNone)
			{
			aPhone.GetPhoneStoreInfo(getInfoStatus2,infoPckg, aStoreName);
			ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus2, ETimeMedium), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out"));
			if (getInfoStatus2.Int() != KErrNone)
				{
				ret=-1;
				}
			}
		if(ret!=-1) //Delete the last entry
			{
			TExtEtelRequestStatus deleteStatus(aPhoneStore, EMobilePhoneStoreDelete);
			CleanupStack::PushL (deleteStatus);
			aPhoneStore.Delete (deleteStatus, aInfo.iUsedEntries);
			ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(deleteStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::Delete timed out"));
			ret=aInfo.iUsedEntries;
			
			// check that the entry was deleted
			aPhoneStore.GetInfo(getInfoStatus,infoPckg);
			ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhoneStore::GetInfo, Timed out"));
			if(getInfoStatus.Int() != KErrNone)
				{
				aPhone.GetPhoneStoreInfo(getInfoStatus2,infoPckg, aStoreName);
				ASSERT_EQUALS(iTestStep.WaitForRequestWithTimeOut(getInfoStatus2, ETimeMedium), KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed out"));
				if (getInfoStatus2.Int() != KErrNone)
					{
					ret=-1;
					}
				}
			if(aInfo.iUsedEntries==aInfo.iTotalEntries)
				{
				ret =-1;
				}

			CleanupStack::PopAndDestroy(&deleteStatus);
			
			}
		CleanupStack::PopAndDestroy(2);
		return ret;
		}
	
	void VerifyPin2L(RMobilePhone& aMobilePhone, TNetworkTsyTestHelper& aNetworkTsyTestHelper);

	}; // class TSimTsyTestHelper 

#endif
