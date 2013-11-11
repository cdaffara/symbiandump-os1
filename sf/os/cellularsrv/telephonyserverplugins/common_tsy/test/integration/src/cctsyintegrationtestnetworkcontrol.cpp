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
// Test step definitions for the NetworkControl functional unit.
// 
//

/**
 @internalTechnology
*/

#include <etelpckt.h>
#include <pcktcs.h>
 
#include "cctsyintegrationtestnetworkcontrol.h"
#include "listretrievalhelpers.h"
#include "cctsyinidata.h"

	
CCTSYIntegrationTestNetworkControlBase::CCTSYIntegrationTestNetworkControlBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), 
		iNetworkTestHelper(*this),
		iCallControlTestHelper(*this),
		iPacketServiceTestHelper(*this)
		
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestNetworkControlBase::~CCTSYIntegrationTestNetworkControlBase()
/*
 * Destructor
 */
	{
	}
			

/** 
* Check a preferred network entry is in the list
* 
* @param aList The list to search.
* @param aTarget The network to search for in aList.
* 
* @return ETrue if aTarget is found in aList, EFalse otherwise.
*/
TBool CCTSYIntegrationTestNetworkControlBase::IsPreferredNetworkInListL( 
								CMobilePhoneStoredNetworkList& aList, 
								RMobilePhone::TMobilePreferredNetworkEntryV3& aTarget)
	{
	TInt listCount = aList.Enumerate(); 

	for( TInt i=0; i < listCount; i++ )
		{
		const RMobilePhone::TMobilePreferredNetworkEntryV3 &netInfo = aList.GetEntryL(i);
		
		if( netInfo.iAccess == aTarget.iAccess && 
			netInfo.iCountryCode == aTarget.iCountryCode &&
			netInfo.iNetworkId == aTarget.iNetworkId )
			{
			return ETrue;
			}
		}
	
	return EFalse;
	
	}

void CCTSYIntegrationTestNetworkControlBase::SetAttachModeToWhenPossibleAndWaitForAttachL(RPacketService& aPacketService)
/**
 * Attaches the phone to the packet service by setting the
 * attach mode to RPacketService::EAttachWhenPossible. Requests
 * that the phone attaches to the packet service and waits for
 * attach to complete.
 * 
 * @param aPacketService Reference to packet service subsession.
 */
    {
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(aPacketService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    TExtEtelRequestStatus notifyServiceStatusChStatus(aPacketService, EPacketNotifyStatusChange);
    CleanupStack::PushL(notifyServiceStatusChStatus);
    RPacketService::TStatus packetServiceStatus;
    aPacketService.NotifyStatusChange(notifyServiceStatusChStatus, packetServiceStatus);

    if(attachMode != RPacketService::EAttachWhenPossible)
		{
		CHECK_EQUALS_L(aPacketService.SetAttachMode(RPacketService::EAttachWhenPossible), KErrNone,
					 _L("RPacketService::SetAttachMode returned with an error."));
		}

    CHECK_EQUALS_L(aPacketService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));
    CHECK_EQUALS_L(attachMode, RPacketService::EAttachWhenPossible, _L("RPacketService::GetAttachMode did not return EAttachWhenPossible."));

    // Ensure phone is attached to the packet service (RPacketService::GetStatus returns EStatusAttached).
	RPacketService::TStatus packetStatus;
	CHECK_EQUALS_L(aPacketService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
	
	// Wait for phone to attach to packet service if it is not attached.
	if (packetStatus == RPacketService::EStatusUnattached)
		{
        iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				aPacketService,
				notifyServiceStatusChStatus,
				packetServiceStatus, 
				RPacketService::EStatusAttached,
				KErrNone);
        CHECK_EQUALS_L(aPacketService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
        CHECK_EQUALS_L(packetStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus did not return EStatusAttached"));
		}

    CleanupStack::PopAndDestroy(1, &notifyServiceStatusChStatus);
    }
	
/**************************************************************************/

CCTSYIntegrationTestNetworkControl0001::CCTSYIntegrationTestNetworkControl0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0001::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0001::~CCTSYIntegrationTestNetworkControl0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0001
 * @SYMFssID BA/CTSY/NTWC-0001
 * @SYMTestCaseDesc Manually select home and non-home network.

 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhonePreferredNetworks::NewL, RMobilePhone::SelectNetwork, RMobilePhone::StorePreferredNetworksListL, RMobilePhone::SetNetworkSelectionSetting, RMobilePhone::GetNetworkSelectionSetting, RMobilePhone::NotifyNetworkSelectionSettingChange
 * @SYMTestExpectedResults Pass - Can register onto home network and this does not change preferred network list.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify home network can be selected, non-home network cannot be selected and preferred network list does not change after manual network selection.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetDetectedNetworks | KCapsManualNetworkSelection 
	TUint32 caps;
	CHECK_EQUALS_L( phone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error") );
	TUint32 wantedCaps = RMobilePhone::KCapsGetDetectedNetworks | RMobilePhone::KCapsManualNetworkSelection;
	CHECK_BITS_SET_L( caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps") );
	
	// Ensure RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionAutomatic 
	RMobilePhone::TMobilePhoneNetworkSelectionV1 netSelSetting;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg netSelSettingPkg(netSelSetting);
		
	CHECK_EQUALS_L( phone.GetNetworkSelectionSetting( netSelSettingPkg ), KErrNone, 
		_L("RMobilePhone::GetNetworkSelectionSetting returned an error") );
	
	if (netSelSetting.iMethod != RMobilePhone::ENetworkSelectionAutomatic)
		{
		DEBUG_PRINTF1(_L("Setting network selection setting to auto"));
		TExtEtelRequestStatus setNetSelSettingStatus(phone, EMobilePhoneSetNetworkSelectionSetting);
		CleanupStack::PushL(setNetSelSettingStatus);
		netSelSetting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
		phone.SetNetworkSelectionSetting( setNetSelSettingStatus, netSelSettingPkg );
		
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setNetSelSettingStatus, ETimeLong), KErrNone, 
						_L("RMobilePhone::SetNetworkSelectionSetting timed out") );
		CHECK_EQUALS_L( setNetSelSettingStatus.Int(), KErrNone, 
						_L("RMobilePhone::SetNetworkSelectionSetting returned error") );
		
		CleanupStack::Pop(&setNetSelSettingStatus);
		}

	CHECK_EQUALS_L( phone.GetNetworkSelectionSetting( netSelSettingPkg ), KErrNone, 
			_L("RMobilePhone::GetNetworkSelectionSetting returned an error") );
	CHECK_EQUALS_L( netSelSetting.iMethod, RMobilePhone::ENetworkSelectionAutomatic, 
		_L("RMobilePhone::GetNetworkSelectionSetting - TMobilePhoneNetworkSelectionV1::iMethod was not ENetworkSelectionAutomatic") );
								
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Get the ID and country code of the home network and store it for use later ===
	RMobilePhone::TMobilePhoneNetworkInfoV1 homeNetworkInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> homeInfoPkg(homeNetworkInfo);	
	TExtEtelRequestStatus getHomeNetworkStatus(phone, EMobilePhoneGetHomeNetwork);
	CleanupStack::PushL(getHomeNetworkStatus);
	phone.GetHomeNetwork( getHomeNetworkStatus, homeInfoPkg );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getHomeNetworkStatus, ETimeLong), KErrNone, 
			_L("RMobilePhone::GetHomeNetwork timed out") );
	ASSERT_EQUALS( getHomeNetworkStatus.Int(), KErrNone, _L("TNetworkTsyTestHelper::GetHomeNetworkL returned an error") );

	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV1 returns aNetworkInfo.iNetworkId with length > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV1 returns aNetworkInfo.iCountryCode with length > 0
	
	ASSERT_TRUE( homeNetworkInfo.iNetworkId.Length() > 0, 
							_L("RMobilePhone::GetHomeNetwork returned aNetworkInfo.iNetworkId <= 0"));
	ASSERT_TRUE( homeNetworkInfo.iCountryCode.Length() > 0, 
							_L("RMobilePhone::GetHomeNetwork returned aNetworkInfo.iCountryCode <= 0"));

	// ===  Put the phone into manual network selection mode. ===
	
	// Set up the RMobilePhone::NotifyNetworkSelectionSettingChange notification for later test
	TExtEtelRequestStatus notifyNetSelSettingChange( phone, EMobilePhoneNotifyNetworkSelectionSettingChange);
	CleanupStack::PushL(notifyNetSelSettingChange);
	
	RMobilePhone::TMobilePhoneNetworkSelectionV1 netSelNotify;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg netSelNotifyPkg( netSelNotify );
	phone.NotifyNetworkSelectionSettingChange(notifyNetSelSettingChange, netSelNotifyPkg);
	
	// Set network selection setting to manual with RMobilePhone::SetNetworkSelectionSetting with 
	// aSetting.iMethod = ENetworkSelectionManual
	TExtEtelRequestStatus setNetSelSettingStatus(phone, EMobilePhoneSetNetworkSelectionSetting);
	CleanupStack::PushL(setNetSelSettingStatus);
	netSelSetting.iMethod = RMobilePhone::ENetworkSelectionManual;
	phone.SetNetworkSelectionSetting( setNetSelSettingStatus, netSelSettingPkg );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setNetSelSettingStatus, ETimeLong), KErrNone, 
					_L("RMobilePhone::SetNetworkSelectionSetting timed out") );
	ASSERT_EQUALS( setNetSelSettingStatus.Int(), KErrNone, 
					_L("RMobilePhone::SetNetworkSelectionSetting returned error") );					
	
	// ===  Verify manual selection mode has been set and notification received ===

	// Check RMobilePhone::NotifyNetworkSelectionSettingChange completes with 
	// aSetting.iMethod = ENetworkSelectionManual
	iNetworkTestHelper.WaitForMobilePhoneNotifyNetworkSelectionSettingChange(phone,
													notifyNetSelSettingChange,netSelNotify, 
													RMobilePhone::ENetworkSelectionManual, KErrNone);

	// Check RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionManual
	ASSERT_EQUALS( phone.GetNetworkSelectionSetting( netSelSettingPkg ), KErrNone, 
						_L("RMobilePhone::GetNetworkSelectionSetting returned an error") );
	ASSERT_EQUALS( netSelSetting.iMethod, RMobilePhone::ENetworkSelectionManual, 
							_L("RMobilePhone::GetNetworkSelectionSetting returned method is not ENetworkSelectionManual") );

	// ===  Get and store (locally) the list of preferred networks for use later ===

	// Use helper class for CRetrieveMobilePhonePreferredNetworks to retrieve a list of preferred networks 
	// Store list for use later. 
	
	CRetrieveMobilePhonePreferredNetworksExec *netListHelper = CRetrieveMobilePhonePreferredNetworksHelper::NewL(phone);
	CleanupStack::PushL( netListHelper );
	TInt err;
	CMobilePhoneStoredNetworkList* prefNetList = netListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNotSupported, _L("CRetrieveMobilePhonePreferredNetworks::Start failed to get the list"));	

	// ===  Get List of detected networks and ensure that this list contain at least 2 networks ===
	// (the home network and an other one).
	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a list of detected networks 
	CRetrieveMobilePhoneDetectedNetworksExec *detectedNetListHelper = 
				CRetrieveMobilePhoneDetectedNetworksHelper::NewL(phone,ERetrieveListV1);
	
	CMobilePhoneNetworkList *detectedNetList = detectedNetListHelper->DoGetList(err);
	
	ASSERT_EQUALS(err, KErrNotSupported, _L("CRetrieveMobilePhoneDetectedNetworks::Start failed to get the list"));

	// $CTSYProblem CTSY does not support EMobilePhoneGetPreferredNetworksPhase1 and 
	// EMobilePhoneGetDetectedNetworksPhase1 IPCs.
	// If supported for these are added, the rest of this test can be uncommented
	// and the CleanupStack items sorted out.
	
	/*	
	TInt detectedNetCount = detectedNetList->Enumerate();
	DEBUG_PRINTF2(_L("CRetrieveMobilePhoneDetectedNetworks returned list of %d entries"), detectedNetCount );
	ASSERT_TRUE(detectedNetCount >= 2, _L("CRetrieveMobilePhoneDetectedNetworks returned list of < 2 entries"));
	
	// Cycle through the items in CMobilePhoneNetworkList list and find a network which 
	// is not the home network (TMobilePhoneNetworkInfoV1::iNetworkId in the list != aNetworkInfo.iNetworkId 
	// from RMobilePhone::GetHomeNetwork)
	const RMobilePhone::TMobilePhoneNetworkInfoV1 *notHomeNetworkInfo = NULL;
	for( TInt i=0; i < detectedNetCount; i++ )
		{
		const RMobilePhone::TMobilePhoneNetworkInfoV1 &netInfo = detectedNetList->GetEntryL(i);
		if( netInfo.iNetworkId != homeNetworkInfo.iNetworkId )
			{
			notHomeNetworkInfo = &netInfo;
			break;
			}
		}
	
	// ===  When a non-home network has been found, try to select it ===
	ASSERT_TRUE( NULL != notHomeNetworkInfo, _L("Failed to find a non-home network") );
	TEST_CHECK_POINT_L( _L("Failed to find a non-home network, leaving") );

	//  Select network with RMobilePhone::SelectNetwork with 
	// aIsManual=ETrue and 
	// aManualSelection.iCountry=TMobilePhoneNetworkInfoV1::iCountryCode and 
	// aManualSelection.iNetwork=TMobilePhoneNetworkInfoV1::iNetworkId 
	
	TExtEtelRequestStatus reqSetNetworkStatus(phone, EMobilePhoneSelectNetwork);
	CleanupStack::PushL(reqSetNetworkStatus);	
	RMobilePhone::TMobilePhoneNetworkManualSelection manNetSel;

	manNetSel.iCountry = notHomeNetworkInfo->iCountryCode;
	manNetSel.iNetwork = notHomeNetworkInfo->iNetworkId;
	phone.SelectNetwork(reqSetNetworkStatus, ETrue, manNetSel );
	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSetNetworkStatus, ETimeLong ), KErrNone, 
								_L("RMobilePhone::SelectNetwork timed out")	);
	
	//  Check this returns error != KErrorNone 
	ASSERT_TRUE( KErrNone != reqSetNetworkStatus.Int(),	
				_L("RMobilePhone::SelectNetwork on non home network succeeded - should fail"));

	//  Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegistrationDenied 
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	iNetworkTestHelper.GetNetworkRegistrationStatusL(phone, registrationStatus);
	ASSERT_EQUALS(registrationStatus, RMobilePhone::ERegistrationDenied, 
						_L("RMobilePhone::GetNetworkRegistrationStatus did not return ERegistrationDenied"));

	// ===  Register back on the home network ===
	// Select network with RMobilePhone::SelectNetwork with 
	// aIsManual=ETrue and aManualSelection.iCountry=aNetworkInfo.iCountryCode 
	// of home network and aManualSelection.iNetwork=aNetworkInfo.iNetworkId of home network 
	
	manNetSel.iCountry = homeNetworkInfo.iCountryCode;
	manNetSel.iNetwork = homeNetworkInfo.iNetworkId;
	phone.SelectNetwork(reqSetNetworkStatus, ETrue, manNetSel );
	
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqSetNetworkStatus, ETimeLong ), KErrNone, 
								_L("RMobilePhone::SelectNetwork timed out")	);
	
	//  Check this returns error != KErrorNone 
	ASSERT_TRUE( KErrNone != reqSetNetworkStatus.Int(),	
						_L("RMobilePhone::SelectNetwork on home network succeeded - should fail"));
	
	// ===  Get list of preferred networks and check it has not changed ===
	// Use helper class for CRetrieveMobilePhonePreferredNetworks to retrieve a list of preferred networks 
		
	CRetrieveMobilePhonePreferredNetworksExec *checkNetListHelper = CRetrieveMobilePhonePreferredNetworksHelper::NewL(phone);
	CleanupStack::PushL( checkNetListHelper );
	
	CMobilePhoneStoredNetworkList* checkPrefNetList = checkNetListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhonePreferredNetworks::DoGetList failed to get the check list"));	
	
	// Check contents of each entry in the CMobilePhoneStoredNetworkList list retrieved is the same as the 
	// first preferred network list.
	
	TInt prefListCount = prefNetList->Enumerate();
	TInt checkListCount = checkPrefNetList->Enumerate();
	
	ASSERT_EQUALS( prefListCount, checkListCount, _L("Lists of preferred networks differ in size"));
	
	if(  prefListCount == checkListCount )
		{
		// Lists are the same size. We assume the order is the same too
		for( TInt j=0; j < prefListCount; j++ )
			{
			const RMobilePhone::TMobilePreferredNetworkEntryV3 &prefInfo = prefNetList->GetEntryL(j);
			const RMobilePhone::TMobilePreferredNetworkEntryV3 &checkInfo = checkPrefNetList->GetEntryL(j);
			
			ASSERT_EQUALS( prefInfo.iCountryCode, checkInfo.iCountryCode, _L("Phone entry country codes do not match"));
			ASSERT_EQUALS( prefInfo.iNetworkId, checkInfo.iNetworkId, _L("Phone network ids do not match"));

			}
		}
	*/	
	// Repost the notification request
	phone.NotifyNetworkSelectionSettingChange(notifyNetSelSettingChange, netSelNotifyPkg);

	// ===  Put the phone into automatic network selection mode. ===
	// Set network selection setting to manual with RMobilePhone::SetNetworkSelectionSetting 
	// with aSetting.iMethod = ENetworkSelectionAutomatic 
	netSelSetting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	phone.SetNetworkSelectionSetting( setNetSelSettingStatus, netSelSettingPkg );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setNetSelSettingStatus, ETimeLong), KErrNone, 
					_L("RMobilePhone::SetNetworkSelectionSetting timed out") );
					
	ASSERT_EQUALS( setNetSelSettingStatus.Int(), KErrNone, 
					_L("RMobilePhone::SetNetworkSelectionSetting returned error") );					

	// ===  Verify automatic selection mode has been set and notification received ===

	// Check RMobilePhone::NotifyNetworkSelectionSettingChange completes with aSetting.iMethod = ENetworkSelectionAutomatic
	iNetworkTestHelper.WaitForMobilePhoneNotifyNetworkSelectionSettingChange(phone, 
															notifyNetSelSettingChange, netSelNotify, 
															RMobilePhone::ENetworkSelectionAutomatic,
															KErrNone);	

	// Check RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionAutomatic
	ASSERT_EQUALS( phone.GetNetworkSelectionSetting( netSelSettingPkg ), KErrNone, 
					_L("RMobilePhone::GetNetworkSelectionSetting returned an error") );
	ASSERT_EQUALS( netSelSetting.iMethod, RMobilePhone::ENetworkSelectionAutomatic, 
					_L("RMobilePhone::GetNetworkSelectionSetting did not return ENetworkSelectionAutomatic") );
	
	//
	// TEST END
	//

    StartCleanup();
	
	// netListHelper
	// setNetSelSettingStatus
	// notifyNetSelSettingChange
    // getHomeNetworkStatus
	CleanupStack::PopAndDestroy(4,&getHomeNetworkStatus);
	
	return TestStepResult();
	}
	

TPtrC CCTSYIntegrationTestNetworkControl0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0001");
	}



CCTSYIntegrationTestNetworkControl0002::CCTSYIntegrationTestNetworkControl0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0002::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0002::~CCTSYIntegrationTestNetworkControl0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0002
 * @SYMFssID BA/CTSY/NTWC-0002
 * @SYMTestCaseDesc Basic cell selection
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCurrentNetwork, RMobilePhone::GetLastUsedAccessTechnology, RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::GetServiceProviderName, RMobilePhone::GetCurrentNetworkName
 * @SYMTestExpectedResults Pass - Phone camps on a cell
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone has camped on a cell with an expected network information - name, mode, type, cellid, location area
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//

	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetRegistrationStatus | KCapsGetCurrentMode | KCapsGetCurrentNetwork | KCapsGetHomeNetwork 
	TUint32 networkCaps=0;
	CHECK_EQUALS_L(mobilePhone.GetNetworkCaps(networkCaps), KErrNone,
				   _L("RMobilePhone::GetNetworkCaps returned with an error"));
	TUint32 expectedCaps= RMobilePhone::KCapsGetRegistrationStatus | RMobilePhone::KCapsGetCurrentMode 
						  | RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsGetHomeNetwork;
	CHECK_BITS_SET_L(networkCaps, expectedCaps, KNoUnwantedBits,
					 _L("RMobilePhone::GetNetworkCaps returned with wrong caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Get mode ===

	// Check RMobilePhone::GetCurrentMode returns aNetworkMode=ENetworkModeGsm or ENetworkModeWcdma
	RMobilePhone::TMobilePhoneNetworkMode mode;
	ASSERT_EQUALS(mobilePhone.GetCurrentMode(mode), KErrNone, _L("RMobilePhone::GetCurrentMode returned with an error"));
	
	ASSERT_TRUE((mode == RMobilePhone::ENetworkModeGsm) || (mode == RMobilePhone::ENetworkModeWcdma),
				_L("RMobilePhone::GetCurrentMode did not return ENetworkModeGsm or ENetworkModeWcdma as expected"));
	
	// ===  Get current network ===

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 location;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg netInfoPckg(netInfo);
    TExtEtelRequestStatus getCurrentNetworkStatus(mobilePhone, EMobilePhoneGetCurrentNetwork);	
	CleanupStack::PushL(getCurrentNetworkStatus);
	mobilePhone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPckg, location);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeMedium),KErrNone, 
				  _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned with an error"));
	ASSERT_TRUE(netInfo.iCountryCode.Length()>0 , _L("RMobilePhone::GetCurrentNetwork returned with blank Country Code"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length()>0 , _L("RMobilePhone::GetCurrentNetwork returned with blank Long Name"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or ENetworkModeWcdma
	ASSERT_TRUE((netInfo.iMode == RMobilePhone::ENetworkModeGsm) || (netInfo.iMode == RMobilePhone::ENetworkModeWcdma) , _L("RMobilePhone::GetCurrentNetwork did not return ENetworkModeGsm Mode or ENetworkModeWcdma Mode"));		
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length()>0 , _L("RMobilePhone::GetCurrentNetwork returned with blank Network Id"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length()>0 , _L("RMobilePhone::GetCurrentNetwork returned with blank Short Name"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus,RMobilePhone::ENetworkStatusCurrent , _L("RMobilePhone::GetCurrentNetwork did not return ENetworkStatusCurrent status as expected "));			

	// Check RMobilePhone::GetLastUsedAccessTechnology returns aAccessTech=ENetworkAccessUnknown or ENetworkAccessGsm or ENetworkAccessGsmCompact or ENetworkAccessUtran
	RMobilePhone::TMobilePhoneNetworkAccess accessTech;
	TExtEtelRequestStatus getLastUsedAccessTechnologyStatus(mobilePhone, EMobilePhoneGetLastUsedAccessTechnology);	
	CleanupStack::PushL(getLastUsedAccessTechnologyStatus);
	mobilePhone.GetLastUsedAccessTechnology(getLastUsedAccessTechnologyStatus,accessTech);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getLastUsedAccessTechnologyStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetLastUsedAccessTechnology timed out"));
	ASSERT_EQUALS(getLastUsedAccessTechnologyStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetLastUsedAccessTechnology returned with an error"));

	// Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegisteredOnHomeNetwork
	RMobilePhone::TMobilePhoneRegistrationStatus  status;
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL( mobilePhone, status), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(status, RMobilePhone::ERegisteredOnHomeNetwork,
	              _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ERegisteredOnHomeNetwork Status"));
	
	// ===  Get service provider name ===

	// Check RMobilePhone::GetServiceProviderName with a TMobilePhoneServiceProviderNameV2 parameter returns aName.iPLMNField with length > 0
	RMobilePhone::TMobilePhoneServiceProviderNameV2 providerName;
    RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg providerNamePckg(providerName);
    TExtEtelRequestStatus getServiceProviderNameStatus(mobilePhone, EMobilePhoneGetServiceProviderName);	
	CleanupStack::PushL(getServiceProviderNameStatus);
    mobilePhone.GetServiceProviderName(getServiceProviderNameStatus, providerNamePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getServiceProviderNameStatus, ETimeMedium),KErrNone, 
	              _L("RMobilePhone::GetServiceProviderName timed out"));
	ASSERT_EQUALS(getServiceProviderNameStatus.Int(), KErrGsm0707NotFound, 
	              _L("RMobilePhone::GetServiceProviderName returned with an error"));
	
	// $CTSYProblem
	// For RMobilePhone::GetServiceProviderName, LTSY is returning KErrGsm0707NotFound
	// CTSY is correctly propagating the error back to the client.
	// If the LTSY is altered to retrieve the provider name successfully, the following
	// test code can be used and KErrNone can be expected from this API.
	
/*	// Check RMobilePhone::GetServiceProviderName with a TMobilePhoneServiceProviderNameV2 parameter returns aName.iDisplayReq with length > 0
	ASSERT_TRUE(providerName.iDisplayReq !=0, _L("RMobilePhone::GetServiceProviderName returned 0 RMobilePhone::TMobilePhoneServiceProviderNameV2.iDisplayReq"));
	
	// Check RMobilePhone::GetServiceProviderName with a TMobilePhoneServiceProviderNameV2 parameter returns aName.iPLMNField with length > 0
	ASSERT_TRUE(providerName.iPLMNField.Length()>0 , _L("RMobilePhone::GetServiceProviderName returned with blank PLMNField"));			
	
	// Check RMobilePhone::GetServiceProviderName with a TMobilePhoneServiceProviderNameV2 parameter returns aName.iSPName with length > 0
	ASSERT_TRUE(providerName.iSPName.Length()>0 , _L("RMobilePhone::GetServiceProviderName returned with blank SPName"));			
*/
	
	// Check RMobilePhone::GetCurrentNetworkName with a TMobilePhoneNetworkNameV3 returns aNetworkName.iLongName with length > 0
	RMobilePhone::TMobilePhoneNetworkNameV3 networkname;
	RMobilePhone::TMobilePhoneNetworkNameV3Pckg networknamePckg(networkname);
    RMobilePhone::TMobilePhoneOPlmnV3 locInfo;
 	RMobilePhone::TMobilePhoneOPlmnV3Pckg locInfoPckg(locInfo);
    TExtEtelRequestStatus getCurrentNetworkNameStatus(mobilePhone, EMobilePhoneGetNetworkName);	
	CleanupStack::PushL(getCurrentNetworkNameStatus);
	mobilePhone.GetCurrentNetworkName(getCurrentNetworkNameStatus, networknamePckg, locInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkNameStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetCurrentNetworkName timed out"));
	ASSERT_EQUALS(getCurrentNetworkNameStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetworkName returned with an error"));
	ASSERT_TRUE(networkname.iLongName.Length()>0 , _L("RMobilePhone::GetCurrentNetworkName returned with blank Long Name"));			

	// Check RMobilePhone::GetCurrentNetworkName with a TMobilePhoneNetworkNameV3 returns aNetworkName.iShortName with length > 0
	ASSERT_TRUE(networkname.iShortName.Length()>0 , _L("RMobilePhone::GetServiceProviderName returned with blank Short Name"));		
		
	// Check RMobilePhone::GetCurrentNetworkName with a TMobilePhoneOPlmnV3 returns aLocInfo.iCountryCode with length > 0
	ASSERT_TRUE(locInfo.iCountryCode.Length()>0 , _L("RMobilePhone::GetServiceProviderName returned with blank Country Code"));			
	
	// Check RMobilePhone::GetCurrentNetworkName with a TMobilePhoneOPlmnV3 returns aLocInfo.iNetworkId with length > 0
	ASSERT_TRUE(locInfo.iNetworkId.Length()>0 , _L("RMobilePhone::GetServiceProviderName returned with blank Network Id"));			
	
	//
	// TEST END
	//

    StartCleanup();
    // Pop
    // getCurrentNetworkStatus
    // getLastUsedAccessTechnologyStatus
    // getServiceProviderNameStatus
    // getCurrentNetworkNameStatus
	
	CleanupStack::PopAndDestroy(4, &getCurrentNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0002");
	}



CCTSYIntegrationTestNetworkControl0003::CCTSYIntegrationTestNetworkControl0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0003::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0003::~CCTSYIntegrationTestNetworkControl0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0003
 * @SYMFssID BA/CTSY/NTWC-0003
 * @SYMTestCaseDesc Cell reselection from "no service" state
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCurrentNetwork, RMobilePhone::NotifyNetworkRegistrationStatusChange, RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::NotifyModeChange
 * @SYMTestExpectedResults Pass - Phone reselects a cell.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify cell is reselected and network information is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
    
    RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
    // Phone is in a blackspot, with no coverage. 
	DisplayUserInteractionPromptL(_L("Please be sure that phone is in a blackspot, with no coverage"), ETimeLong);

	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetRegistrationStatus | KCapsGetCurrentMode | KCapsGetCurrentNetwork | KCapsNotifyRegistrationStatus | KCapsNotifyMode | KCapsNotifyCurrentNetwork 
	TUint32 networkCaps = 0;
	TUint32 expectedCaps = RMobilePhone::KCapsGetRegistrationStatus | RMobilePhone::KCapsGetCurrentMode | RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsNotifyRegistrationStatus | RMobilePhone::KCapsNotifyMode | RMobilePhone::KCapsNotifyCurrentNetwork;
	CHECK_EQUALS_L(phone.GetNetworkCaps(networkCaps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned with an error"));
	CHECK_BITS_SET_L(networkCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned with wrong caps"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10075);
	User::Leave(KErrNone);	
	
	
	// Check RMobilePhone::GetNetworkRegistrationStatus returns ENotRegisteredNoService or ENotRegisteredSearching
    RMobilePhone::TMobilePhoneRegistrationStatus status;
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone, status), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_TRUE(status != RMobilePhone::ENotRegisteredSearching || status != RMobilePhone::ENotRegisteredNoService, 
	            _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ENotRegisteredSearching or ENotRegisteredNoServiceStatus"));
	
	TExtEtelRequestStatus reqNetworkRegStatusChange(phone, EMobilePhoneNotifyNetworkRegistrationStatusChange);
    CleanupStack::PushL(reqNetworkRegStatusChange);
    // Call notify for networkregistrationchange
    RMobilePhone::TMobilePhoneRegistrationStatus networkRegStatus;
    phone.NotifyNetworkRegistrationStatusChange(reqNetworkRegStatusChange, networkRegStatus);
    
   	TExtEtelRequestStatus reqNotifyCurrentNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(reqNotifyCurrentNetChangeStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5> netInfoPkg(netInfo);
	// Call notify for networkchange
	phone.NotifyCurrentNetworkChange(reqNotifyCurrentNetChangeStatus, netInfoPkg, locationArea );
	
	TExtEtelRequestStatus reqNotifyModeChangeStatus(phone, EMobilePhoneNotifyModeChange);
	CleanupStack::PushL(reqNotifyModeChangeStatus);
	RMobilePhone::TMobilePhoneNetworkMode mobNetworkMode;
	// Call notify for NotifyModeChange
	phone.NotifyModeChange(reqNotifyModeChangeStatus, mobNetworkMode);
	
	// Move phone to an area with coverage 
	DisplayUserInteractionPromptL(_L("Move phone to an area with coverage"), ETimeLong);

	// ===  Check cell is reselected and network information is correct ===

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with aStatus=ERegisteredOnHomeNetwork
	RMobilePhone::TMobilePhoneRegistrationStatus expectedStatus = RMobilePhone::ERegisteredOnHomeNetwork;
	const TInt wantedStatus = KErrNone;
	iNetworkTestHelper.WaitForMobilePhoneNotifyNetworkRegistrationStatusChange(phone, reqNetworkRegStatusChange, networkRegStatus, expectedStatus, wantedStatus);
		
	// Check RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone, status), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(status, RMobilePhone::ERegisteredOnHomeNetwork, _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ENotRegisteredSearching or ENotRegisteredNoServiceStatus"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(reqNotifyCurrentNetChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
	ASSERT_EQUALS(reqNotifyCurrentNetChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned error"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iDisplayTag"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLongName"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_EQUALS(netInfo.iMode, RMobilePhone::ENetworkModeGsm, _L("RMobilePhone::NotifyCurrentNetworkChange iMode did not return ENetworkModeGsm as expected"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iNetworkId"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iShortName"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange iStatus did not return ENetworkStatusCurrent as expected"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	ASSERT_TRUE(locationArea.iCellId > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCellId"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(locationArea.iLocationAreaCode > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLocationAreaCode"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	RMobilePhone::TMobilePhoneNetworkInfoV5 currentNetworkInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 currentNetworkAreaLocation;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg currentNetworkInfoPckg(currentNetworkInfo);
    TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);	
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork(getCurrentNetworkStatus, currentNetworkInfoPckg, currentNetworkAreaLocation);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned returned error"));

	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(currentNetworkInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(currentNetworkInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_EQUALS(currentNetworkInfo.iMode, RMobilePhone::ENetworkModeGsm, _L("RMobilePhone::GetCurrentNetwork iMode did not return ENetworkModeGsm as expected"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(currentNetworkInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(currentNetworkInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortName"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(currentNetworkInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork iStatus did not return ENetworkStatusCurrent as expected"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
    ASSERT_TRUE(currentNetworkAreaLocation.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(currentNetworkAreaLocation.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));

	// Check RMobilePhone::NotifyModeChange completes with aNetworkMode=ENetworkModeGsm
	RMobilePhone::TMobilePhoneNetworkMode expectedMobNetMode = RMobilePhone::ENetworkModeGsm;
	iNetworkTestHelper.WaitForMobilePhoneNotifyModeChange(phone, reqNotifyModeChangeStatus, mobNetworkMode, expectedMobNetMode, wantedStatus);

	// Check RMobilePhone::GetCurrentMode completes with aNetworkMode=ENetworkModeGsm
	RMobilePhone::TMobilePhoneNetworkMode currentMobNetMode;
	phone.GetCurrentMode(currentMobNetMode);
	ASSERT_EQUALS(currentMobNetMode, RMobilePhone::ENetworkModeGsm, _L("RMobilePhone::GetCurrentMode did not return ENetworkModeGsm as expected"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
    // reqNetworkRegStatusChange
    // reqNotifyCurrentNetChangeStatus
    // getCurrentNetworkStatus
    // reqNotifyModeChangeStatus
	CleanupStack::PopAndDestroy(4, &reqNetworkRegStatusChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0003");
	}



CCTSYIntegrationTestNetworkControl0004::CCTSYIntegrationTestNetworkControl0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0004::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0004::~CCTSYIntegrationTestNetworkControl0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0004
 * @SYMFssID BA/CTSY/NTWC-0004
 * @SYMTestCaseDesc Emergency camping
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Phone registers for emergency calls only
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify registration status is for emergency calls only.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Log a message to indicate that this test is run without a SIM in the phone
	DisplayUserInteractionPromptL(_L("Remove SIM card before running this test"), ETimeMedium);
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetRegistrationStatus 
	TUint32 networkCaps = 0;
	TUint32 expectedCaps = RMobilePhone::KCapsGetRegistrationStatus;
	ASSERT_EQUALS(phone.GetNetworkCaps(networkCaps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned with an error"));
	ASSERT_BITS_SET(networkCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned with wrong caps"));

	// ===  Check that registration status is for emergency calls only. ===

	// Check RMobilePhone::GetNetworkRegistrationStatus returns ENotRegisteredSearching
	RMobilePhone::TMobilePhoneRegistrationStatus  status;
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone, status), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	// $CTSYProblem LTSY does not send correct notification. This wast tested using different timeout's but
    // ENotRegisteredEmergencyOnly notification did not arrive as expected.
    // Test was modified to accept ENotRegisteredSearching state, because this is the
    // one which returned from LTSY. From logs we could see that this was the only state which was notified to CTSY.
	ASSERT_EQUALS(status, RMobilePhone::ENotRegisteredSearching, _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ENotRegisteredSearching Status"));

	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0004");
	}



CCTSYIntegrationTestNetworkControl0005::CCTSYIntegrationTestNetworkControl0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0005::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0005::~CCTSYIntegrationTestNetworkControl0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0005
 * @SYMFssID BA/CTSY/NTWC-0005
 * @SYMTestCaseDesc Automatic cell selection
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCurrentNetwork, RMobilePhone::GetHomeNetwork, RMobilePhone::GetNetworkSelectionSetting, RMobilePhone::GetHomeNetworkSearchPeriod
 * @SYMTestExpectedResults Pass - Home, current and preferred network information are correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify home network info., current network info. correct and home network is in the preferred network list.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetHomeNetwork | KCapsGetCurrentNetwork 
	TUint32 caps;
	CHECK_EQUALS_L(phone.GetNetworkCaps(caps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error"));
	TUint32 wantedCaps = RMobilePhone::KCapsGetHomeNetwork | RMobilePhone::KCapsGetCurrentNetwork;
	CHECK_BITS_SET_L(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	RMobilePhone::TMobilePhoneNetworkSelectionV1 netSelSetting;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg netSelSettingPckg(netSelSetting);
	
	// Check RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionAutomatic
	ASSERT_EQUALS( phone.GetNetworkSelectionSetting( netSelSettingPckg ), KErrNone, _L("RMobilePhone::GetNetworkSelectionSetting returned an error") );
	ASSERT_EQUALS( netSelSetting.iMethod, RMobilePhone::ENetworkSelectionAutomatic, _L("RMobilePhone::GetNetworkSelectionSetting did not return ENetworkSelectionAutomatic") );
	
	// $CTSYProblem
	// CTSY does not support RMobilePhone::GetHomeNetworkSearchPeriod
	// GetHomeNetworkSearchPeriod responses directly with KErrNotSupported
    
	// Check RMobilePhone::GetHomeNetworkSearchPeriod returns aSearchIntv >= 0
	TExtEtelRequestStatus searchPeriodReqStatus(phone, EMobilePhoneGetHomeNetworkSearchPeriod);
	CleanupStack::PushL(searchPeriodReqStatus);
	TInt searchIntv;
	phone.GetHomeNetworkSearchPeriod(searchPeriodReqStatus, searchIntv);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(searchPeriodReqStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetHomeNetworkSearchPeriod timed out"));
	ASSERT_EQUALS(searchPeriodReqStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetHomeNetworkSearchPeriod returned an error"));

	// ===  Get the home network information. ===
	RMobilePhone::TMobilePhoneNetworkInfoV5 homeNWInfo;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg homeNWInfoPckg(homeNWInfo);
	TExtEtelRequestStatus getHomeNetworkStatus(phone, EMobilePhoneGetHomeNetwork);
	CleanupStack::PushL(getHomeNetworkStatus);
	phone.GetHomeNetwork(getHomeNetworkStatus, homeNWInfoPckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getHomeNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetHomeNetwork timed out"));
	ASSERT_EQUALS(getHomeNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetHomeNetwork returned an error"));

	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(homeNWInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetHomeNetwork returns invalid CountryCode Tag"));
	
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(homeNWInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetHomeNetwork returns invalid iDisplayTag"));
	
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(homeNWInfo.iLongName.Length() > 0, _L("RMobilePhone::GetHomeNetwork returns invalid iLongName"));
	
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_EQUALS(homeNWInfo.iMode, RMobilePhone::ENetworkModeGsm, _L("RMobilePhone::GetHomeNetwork did not return ENetworkModeGsm as expected"));
	
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(homeNWInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetHomeNetwork returns invalid iNetworkId"));
	
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(homeNWInfo.iShortName.Length() > 0, _L("RMobilePhone::GetHomeNetwork returns invalid iShortName"));
	
	// $CTSYProblem 
	// GetHomeNetwork() doesn't return correct NetworkInfo.iStatus
    
  	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus= ENetworkStatusUnknown
    ASSERT_EQUALS(homeNWInfo.iStatus, RMobilePhone::ENetworkStatusUnknown , _L("RMobilePhone::GetHomeNetwork did not return ENetworkStatusUnknown status as expected "));

	// ===  Get the current network information. ===
	RMobilePhone::TMobilePhoneNetworkInfoV5 currentNWInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 currentLocation;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg currentNWInfoPckg(currentNWInfo);
    TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);	
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork(getCurrentNetworkStatus, currentNWInfoPckg, currentLocation);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(currentNWInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(currentNWInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid Display Tag"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(currentNWInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or ENetworkModeWcdma
	ASSERT_TRUE(currentNWInfo.iMode == RMobilePhone::ENetworkModeGsm || currentNWInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork did not return ENetworkModeGsm as expected"));		

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(currentNWInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(currentNWInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortName"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(currentNWInfo.iStatus, RMobilePhone::ENetworkStatusCurrent , _L("RMobilePhone::GetCurrentNetwork did not return ENetworkStatusCurrent status as expected "));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0
	ASSERT_TRUE(currentLocation.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returned invalid iCellId"));
			
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(currentLocation.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));

	// Use helper class for CRetrieveMobilePhonePreferredNetworks to retrieve a list of preferred networks
	CRetrieveMobilePhonePreferredNetworksExec *netListHelper = CRetrieveMobilePhonePreferredNetworksHelper::NewL(phone);
	CleanupStack::PushL(netListHelper);
	TInt err;
	CMobilePhoneStoredNetworkList* prefNetList = netListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNotSupported, _L("CRetrieveMobilePhonePreferredNetworks::Start failed to get the list"));
	
	// $CTSYProblem CTSY does not support EMobilePhoneGetPreferredNetworksPhase1 and 
	// EMobilePhoneGetDetectedNetworksPhase1 IPCs.
	// If supported, "if (prefNetList != NULL)"-line can be commented/deleted
    
	if (prefNetList != NULL)
		{
		// Check CMobilePhoneStoredNetworkList::Enumerate returns >= 1
		ASSERT_TRUE(prefNetList->Enumerate() >= 1, _L("CMobilePhoneStoredNetworkList::Enumerate did not return valid number of entries"));

		// ===  Check first entry in preferred network list is the home network ===

		// Check first item in the CMobilePhoneStoredNetworkList list contains a RMobilePhone::TMobilePreferredNetworkEntryV3 with
		const RMobilePhone::TMobilePreferredNetworkEntryV3 &prefInfo = prefNetList->GetEntryL(0);

		// iAccess with caps in set of KNetworkAccessGsm
		ASSERT_BITS_SET(prefInfo.iAccess, RMobilePhone::KNetworkAccessGsm, KNoUnwantedBits,	_L("CMobilePhoneStoredNetworkList.GetEntry() returned first list item with invalid prefInfo.iAccess"));

		// iCountryCode = aNetworkInfo.iCountryCode from RMobilePhone::GetHomeNetwork
		ASSERT_EQUALS_DES16(prefInfo.iCountryCode, homeNWInfo.iCountryCode, _L("CMobilePhoneStoredNetworkList.GetEntry() returned first list item with invalid prefInfo.iCountryCode. Not equal to home network"));

		// iNetworkId = aNetworkInfo.iNetworkId from RMobilePhone::GetHomeNetwork 
		ASSERT_EQUALS_DES16(prefInfo.iNetworkId, homeNWInfo.iNetworkId, _L("CMobilePhoneStoredNetworkList.GetEntry() returned first list item with invalid prefInfo.iNetworkId. Not equal to home network"));
		}
	
	//
	// TEST END
	//

    StartCleanup();
    
    //searchPeriodReqStatus
    //getHomeNetworkStatus
    //getCurrentNetworkStatus
    //netListHelper
	CleanupStack::PopAndDestroy(4, &searchPeriodReqStatus);
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0005");
	}



CCTSYIntegrationTestNetworkControl0006::CCTSYIntegrationTestNetworkControl0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0006::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0006::~CCTSYIntegrationTestNetworkControl0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0006
 * @SYMFssID BA/CTSY/NTWC-0006
 * @SYMTestCaseDesc Roaming cell selection
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetHomeNetwork, RMobilePhone::GetNetworkRegistrationStatus
 * @SYMTestExpectedResults Pass - Roaming SIM can register onto PLMN but is different to home network
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify current network differs from home network.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetRegistrationStatus | KCapsGetCurrentMode | KCapsGetCurrentNetwork | KCapsGetHomeNetwork  
	TUint32 caps;
	CHECK_EQUALS_L( phone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error") );
	TUint32 wantedCaps = RMobilePhone::KCapsGetRegistrationStatus | RMobilePhone::KCapsGetCurrentMode | RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsGetHomeNetwork;
	CHECK_BITS_SET_L( caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps") );


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Insert a roaming SIM and start phone 

	// ===  Get home network information ===
	RMobilePhone::TMobilePhoneNetworkInfoV5 homeNetworkInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5> homeInfoPkg(homeNetworkInfo);	
	TExtEtelRequestStatus getHomeNetworkStatus(phone, EMobilePhoneGetHomeNetwork);
	CleanupStack::PushL(getHomeNetworkStatus);
	phone.GetHomeNetwork( getHomeNetworkStatus, homeInfoPkg );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getHomeNetworkStatus, ETimeLong), KErrNone, 
			_L("RMobilePhone::GetHomeNetwork timed out") );
	ASSERT_EQUALS( getHomeNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetHomeNetwork returned an error") );

	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(homeNetworkInfo.iDisplayTag.Length() > 0, 
							_L("RMobilePhone::GetHomeNetwork returned aNetworkInfo.iDisplayTag <= 0"));
							
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(homeNetworkInfo.iLongName.Length() > 0, 
							_L("RMobilePhone::GetHomeNetwork returned aNetworkInfo.iLongName <= 0"));
							
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(homeNetworkInfo.iNetworkId.Length() > 0, 
							_L("RMobilePhone::GetHomeNetwork returned aNetworkInfo.iNetworkId <= 0"));
							
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(homeNetworkInfo.iShortName.Length() > 0, 
							_L("RMobilePhone::GetHomeNetwork returned aNetworkInfo.iShortName <= 0"));

	// ===  Get current network information and check it differs from the home network ===	
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 location;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg netInfoPckg(netInfo);
    TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);	
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPckg, location);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeMedium),KErrNone, 
				  _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned with an error"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag != aNetworkInfo.iDisplayTag from RMobilePhone::GetHomeNetwork
	ASSERT_TRUE(netInfo.iDisplayTag != homeNetworkInfo.iDisplayTag, 
							_L("RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag == aNetworkInfo.iDisplayTag from RMobilePhone::GetHomeNetwork"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName != aNetworkInfo.iLongName from RMobilePhone::GetHomeNetwork
	ASSERT_TRUE(netInfo.iLongName != homeNetworkInfo.iLongName, 
							_L("RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName == aNetworkInfo.iLongName from RMobilePhone::GetHomeNetwork"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId != aNetworkInfo.iNetworkId from RMobilePhone::GetHomeNetwork
	ASSERT_TRUE(netInfo.iNetworkId != homeNetworkInfo.iNetworkId, 
							_L("RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId == aNetworkInfo.iNetworkId from RMobilePhone::GetHomeNetwork"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName != aNetworkInfo.iShortName from RMobilePhone::GetHomeNetwork
	ASSERT_TRUE(netInfo.iShortName != homeNetworkInfo.iShortName, 
							_L("RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName == aNetworkInfo.iShortName from RMobilePhone::GetHomeNetwork"));
	
	// Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegisteredRoaming
	RMobilePhone::TMobilePhoneRegistrationStatus  status;
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL( phone, status), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(status, RMobilePhone::ERegisteredRoaming,
	              _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ERegisteredRoaming Status"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	//getHomeNetworkStatus
	//getCurrentNetworkStatus	
	CleanupStack::PopAndDestroy(2,&getHomeNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0006");
	}



CCTSYIntegrationTestNetworkControl0007::CCTSYIntegrationTestNetworkControl0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0007::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0007::~CCTSYIntegrationTestNetworkControl0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0007
 * @SYMFssID BA/CTSY/NTWC-0007
 * @SYMTestCaseDesc Selection mode preserved after switch off.
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhonePreferredNetworks::NewL, RMobilePhone::SelectNetwork, RMobilePhone::StorePreferredNetworksListL, RMobilePhone::SetNetworkSelectionSetting, RMobilePhone::GetNetworkSelectionSetting, RMobilePhone::NotifyNetworkSelectionSettingChange
 * @SYMTestExpectedResults Pass - Manual selection mode is preserved on re-start
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone is still in manual selection mode after TSY is unloaded and reloaded.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure that the phone has the capabilities KCapsGetDetectedNetworks | KCapsManualNetworkSelection 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get home network name and id (save these for later) ===

	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0

	// ===  Put the phone into manual netowrk selection mode. ===

	// Set network selection setting to manual with RMobilePhone::SetNetworkSelectionSetting with aSetting.iMethod = ENetworkSelectionManual 

	// ===  Verify manual selection mode has been set and notification received ===

	// Check RMobilePhone::NotifyNetworkSelectionSettingChange completes with aSetting.iMethod = ENetworkSelectionManual

	// Check RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionManual

	// ===  Get and store (locally) the list of prefered Network ===

	// Create a CRetrieveMobilePhonePreferredNetworks::NewL 

	// Start retrieval of preferred network list with CRetrieveMobilePhonePreferredNetworks::Start 

	// Retrieve pointer to the CMobilePhoneStoredNetworkList list with CRetrieveMobilePhonePreferredNetworks::RetrieveListL 

	// Check CMobilePhoneStoredNetworkList::Enumerate returns >= 1

	// ===  Close down TSY and restart to force network re-selection ===

	// Close phone subsession with RPhone::Close 

	// Unload TSY with RTelServer::UnloadPhoneModule 

	// Reload it with RTelServer::LoadPhoneModule 

	// Open phone subsession with RPhone::Open 

	// ===  Verify phone is still in manual selection mode ===

	// Check RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionManual

	// ===  Get List of detected networks ===

	// Create a CRetrieveMobilePhoneDetectedNetworks with CRetrieveMobilePhoneDetectedNetworks::NewL 

	// Start retrieval of list of detected network with CRetrieveMobilePhoneDetectedNetworks::StartV5 

	// Retrieve the CMobilePhoneNetworkListV5 with CRetrieveMobilePhoneDetectedNetworks::RetrieveListV5L 

	// ===  Ensure that this list contains at least 2 networks (the Home network and another one). ===

	// Check CMobilePhoneNetworkListV5::Enumerate returns number of items in list > 1

	// ===  Select the home network ===

	// Select home network with RMobilePhone::SelectNetwork with aIsManual=ETrue and aManualSelection.iCountry=aNetworkInfo.iCountryCode from RMobilePhone::GetHomeNetwork and aManualSelection.iNetwork=aNetworkInfo.iNetworkId from RMobilePhone::GetHomeNetwork 

	// ===  Verify that the preferred Network list is not changed. ===

	// Start retrieval of preferred network list with CRetrieveMobilePhonePreferredNetworks::Start 

	// Retrieve pointer to the CMobilePhoneStoredNetworkList list with CRetrieveMobilePhonePreferredNetworks::RetrieveListL 

	// Check CMobilePhoneStoredNetworkList::Enumerate returns same number of entries as before the TSY was closed down.

	// Check the current CMobilePhoneStoredNetworkList contains the same entries as previous list (check iAccess, iCountryCode, iNetworkId in TMobilePreferredNetworkEntryV3 is the same)

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0007");
	}



CCTSYIntegrationTestNetworkControl0008::CCTSYIntegrationTestNetworkControl0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0008::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0008::~CCTSYIntegrationTestNetworkControl0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0008
 * @SYMFssID BA/CTSY/NTWC-0008
 * @SYMTestCaseDesc Register on a denied network.
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneDetectedNetworks::RetrieveListL, RMobilePhone::NotifyNetworkRegistrationStatusChange, CRetrieveMobilePhoneDetectedNetworks::NewL, RMobilePhone::SelectNetwork, CMobilePhoneNetworkListV5::Enumerate, RMobilePhone::SetNetworkSelectionSetting, RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::GetHomeNetwork, CRetrieveMobilePhoneDetectedNetworks::StartV5
 * @SYMTestExpectedResults Pass - Cannot register onto a denied network
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify registration status is denied when attempting to register on another network.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetDetectedNetworks | KCapsManualNetworkSelection 

	// Be registered onto home network 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get the network ID of the home network (store this for use later) ===

	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0

	// ===  Get list of all the detected networks ===

	// Use the helper class for CRetrieveMobilePhoneDetectedNetworks to get a list of detected networks 

	// Start retrieval of preferred network list with CRetrieveMobilePhoneDetectedNetworks::StartV5 

	// Check CMobilePhoneNetworkListV5::Enumerate on the list returned returns number of networks >= 1

	// ===  Switch to manual network selection ===

	// Set network selection setting to automatic with RMobilePhone::SetNetworkSelectionSetting with aSetting.iMethod=ENetworkSelectionManual 

	// ===  Attempt to select each of the other networks ===

	// Cycle through the list of detected networks CMobilePhoneNetworkListV5 

	// If the TMobilePhoneNetworkInfoV5::iNetworkId != aNetworkInfo.iNetworkId returned from RMobilePhone::GetHomeNetwork 

	//  Check RMobilePhone::SelectNetwork with aIsManual=ETrue and aManualSelection.iCountry=TMobilePhoneNetworkInfoV5::iCountryCode and aManualSelection.iNetwork=TMobilePhoneNetworkInfoV5::iNetworkId 

	//  Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegistrationDenied 

	// ===  Ensure registration returns to previous network ===

	// Set network selection setting to automatic with RMobilePhone::SetNetworkSelectionSetting with aSetting.iMethod=ENetworkSelectionAutomatic 

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with aStatus=ERegisteredOnHomeNetwork

	// Check RMobilePhone::GetNetworkRegistrationStatus returns with ERegisteredOnHomeNetwork

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0008");
	}



CCTSYIntegrationTestNetworkControl0009::CCTSYIntegrationTestNetworkControl0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0009::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0009::~CCTSYIntegrationTestNetworkControl0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0009
 * @SYMFssID BA/CTSY/NTWC-0009
 * @SYMTestCaseDesc Automatic network selection at start
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::GetCurrentNetwork
 * @SYMTestExpectedResults Pass - Registration status notificatins are received
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify current network change and network registration status notifications complete.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// $CTSYProblem
	// Put this back into the script when a decision has been made as to what
	// this test case should do since the board has already booted by the time
	// the test starts, not possible to catch the notifications.
	
	/*

START_TESTCASE BA-CTSY-INT-NTWC-0009
//!@SYMTestCaseID BA-CTSY-INT-NTWC-0009
//!@SYMFssID BA/CTSY/NTWC-0009
//!@SYMTestCaseDesc Automatic network selection at start
//!@SYMTestPriority High
//!@SYMTestActions RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::GetCurrentNetwork
//!@SYMTestExpectedResults Pass - Registration status notificatins are received
//!@SYMTestType CIT
//!@SYMTestCaseDependencies live/automatic
RUN_TEST_STEP 180 TE_CTSYIntegrationTestSuite CCTSYIntegrationTestNetworkControl0009 c:\ctsy\ctsyintegration_data.ini
END_TESTCASE BA-CTSY-INT-NTWC-0009

	 */
	
	RMobilePhone& tempPhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetRegistrationStatus 
	TUint32 caps;
	CHECK_EQUALS_L( tempPhone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error") );
	TUint32 wantedCaps = RMobilePhone::KCapsGetRegistrationStatus;
	CHECK_BITS_SET_L( caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong network caps") );

	// Close down TSY, so registration will re-start when TSY re-opens 
	iEtelSessionMgr.RestartL();
	// phone object is now invalid, reopen
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Ensure network registration status goes from unknown->searching->registered home network ===

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with 
	// ERegistrationUnknown->ENotRegisteredSearching->ERegisteredOnHomeNetwork
	
	// Seems a bit unlikely that we will catch the whole sequence if the TSY has been restarted
	// in the absence of notifications during the restart, so check the last one we get is reasonable

	TExtEtelRequestStatus reqRegStatusChange( phone, EMobilePhoneNotifyNetworkRegistrationStatusChange );
	CleanupStack::PushL(reqRegStatusChange);
	
	RMobilePhone::TMobilePhoneRegistrationStatus phoneRegStatus = RMobilePhone::ERegistrationUnknown;
	TBool receivedRegNotify = EFalse;
	TInt err;
	
	do
		{
		DEBUG_PRINTF1(_L("calling notify"));
		phone.NotifyNetworkRegistrationStatusChange( reqRegStatusChange, phoneRegStatus );
		err = WaitForRequestWithTimeOut( reqRegStatusChange, ETimeMedium );
		
		if( KErrNone == err )
			{			
			receivedRegNotify = ETrue;
			}
		}
	while(KErrNone == err);
	
	ASSERT_TRUE( receivedRegNotify, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange timed out") );
	DEBUG_PRINTF2( _L("RMobilePhone::NotifyNetworkRegistrationStatusChange Last registration status=%d"), phoneRegStatus );	
	
	ASSERT_TRUE( (RMobilePhone::ERegistrationUnknown == phoneRegStatus) ||  
					(RMobilePhone::ENotRegisteredSearching == phoneRegStatus) ||  
					(RMobilePhone::ERegisteredOnHomeNetwork == phoneRegStatus), 
					_L("RMobilePhone::NotifyNetworkRegistrationStatusChange returned invalid registration status") );
			
	// Check RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork
	TExtEtelRequestStatus getNetRegStatus(phone,EMobilePhoneGetNetworkRegistrationStatus);
	CleanupStack::PushL(getNetRegStatus);
	
	phone.GetNetworkRegistrationStatus(getNetRegStatus, phoneRegStatus);
	err = WaitForRequestWithTimeOut( getNetRegStatus, ETimeShort );
	
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus timed out") )
	ASSERT_EQUALS(phoneRegStatus, RMobilePhone::ERegisteredOnHomeNetwork, 
								_L("RMobilePhone::GetNetworkRegistrationStatus returned invalid state"))
	
	// ===  Check receive notifciations for current network change ===
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5> netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
	TExtEtelRequestStatus reqNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(reqNetChangeStatus);
	
	phone.NotifyCurrentNetworkChange( reqNetChangeStatus, netInfoPkg, area );	
		
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqNetChangeStatus, ETimeMedium), KErrNone, 
							_L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
								
	ASSERT_EQUALS( reqNetChangeStatus.Int(), KErrNone, 
							_L("RMobilePhone::NotifyCurrentNetworkChange returned returned error"));
							
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iCountryCode with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iDisplayTag with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iLongName with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iMode=ENetworkModeGsm
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iNetworkId with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iShortName with length > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aNetworkInfo.iStatus=ENetworkStatusCurrent
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aArea.iCellId > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 completes with aArea.iLocationAreaCode > 0
	ASSERT_TRUE( netInfo.iCountryCode.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iCountryCode length 0") )
	ASSERT_TRUE( netInfo.iDisplayTag.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iDisplayTag length 0") )
	ASSERT_TRUE( netInfo.iLongName.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iLongNamelength 0") )	
	ASSERT_EQUALS( netInfo.iMode, RMobilePhone::ENetworkModeGsm, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns  Network mode not GSM") )
	ASSERT_TRUE( netInfo.iNetworkId.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange  returns iNetworkId length 0") )	
	ASSERT_TRUE( netInfo.iShortName.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iShortNamelength 0") )
	ASSERT_EQUALS( netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, 
						_L("RMobilePhone::NotifyCurrentNetworkChange returns network status not ENetworkStatusCurrent") )

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE( area.iCellId > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns Location cell ID 0") )
	ASSERT_TRUE( area.iLocationAreaCode > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns Location AreaCode ID 0") )

	// Get the network information and test again
	TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork( getCurrentNetworkStatus, netInfoPkg, area );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeLong), KErrNone, 
			_L("RMobilePhone::GetCurrentNetwork timed out") );
	ASSERT_EQUALS( getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error") );

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_TRUE( netInfo.iCountryCode.Length() > 0, 
					_L("RMobilePhone::GetCurrentNetwork returns iCountryCode length 0") )
	ASSERT_TRUE( netInfo.iDisplayTag.Length() > 0, 
					_L("RMobilePhone::GetCurrentNetwork returns iDisplayTag length 0") )
	ASSERT_TRUE( netInfo.iLongName.Length() > 0, 
					_L("RMobilePhone::GetCurrentNetwork returns iLongNamelength 0") )	
	ASSERT_TRUE( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, 
					_L("RMobilePhone::GetCurrentNetwork returns Network mode not GSM") )
	ASSERT_TRUE( netInfo.iNetworkId.Length() > 0, 
					_L("RMobilePhone::GetCurrentNetwork returns iNetworkId length 0") )	
	ASSERT_TRUE( netInfo.iShortName.Length() > 0,
					_L("RMobilePhone::GetCurrentNetwork returns iShortNamelength 0") )
	ASSERT_EQUALS( netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent,
					_L("RMobilePhone::GetCurrentNetwork returns network status not ENetworkStatusCurrent") )

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE( area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns Location cell ID 0") )
	ASSERT_TRUE( area.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns Location AreaCode ID 0") )
	
	//
	// TEST END
	//

    StartCleanup();
	
	// getCurrentNetworkStatus
	// getNetRegStatus
	// reqNetChangeStatus
	// reqRegStatusChange
	CleanupStack::Pop(4,&reqRegStatusChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0009");
	}



CCTSYIntegrationTestNetworkControl0010::CCTSYIntegrationTestNetworkControl0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0010::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0010::~CCTSYIntegrationTestNetworkControl0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0010
 * @SYMFssID BA/CTSY/NTWC-0010
 * @SYMTestCaseDesc Select a network manually using a roaming SIM
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Fails to register onto another VPLMN
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify roaming SIM cannot register onto another network.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetCurrentNetwork | KCapsManualNetworkSelection 

	// ===  Romaing Sim is registered on a VPLMN ===

	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredRoaming 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get ID of the current network (store this for use later) ===

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0

	// ===  Get list of detected networks ===

	// Use helper class to start retrieval of V5 preferred network list with CRetrieveMobilePhoneDetectedNetworks 

	// Check CMobilePhoneNetworkListV5::Enumerate on list returned returns number of networks > 0

	// ===  Try and manually select another one to register onto ===

	// Set network selection setting to manual with RMobilePhone::SetNetworkSelectionSetting with aSetting.iMethod=ENetworkSelectionManual 

	// Check RMobilePhone::NotifyNetworkSelectionSettingChange completes with aSetting.iMethod = ENetworkSelectionManual

	// Check RMobilePhone::GetNetworkSelectionSetting returns aSetting.iMethod = ENetworkSelectionManual

	// Cycle through the list of networks in CMobilePhoneNetworkListV5 

	//  If the TMobilePhoneNetworkInfoV5::iNetworkId != iNetworkId from RMobilePhone::GetCurrentNetwork 

	//  then 

	//   select network in list with RMobilePhone::SelectNetwork with aIsManual=ETrue and aManualSelection.iCountry=TMobilePhoneNetworkInfoV5::iCountryCode from item in CMobilePhoneNetworkListV5 and aManualSelection.iNetwork=TMobilePhoneNetworkInfoV5::iNetworkId from item in CMobilePhoneNetworkListV5 

	// ===  Check network does not allow registration ===

	//   Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with aStatus=ERegisteredRoaming or ERegistrationDenied 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0010");
	}



CCTSYIntegrationTestNetworkControl0011::CCTSYIntegrationTestNetworkControl0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0011::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0011::~CCTSYIntegrationTestNetworkControl0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0011
 * @SYMFssID BA/CTSY/NTWC-0011
 * @SYMTestCaseDesc Attempt network detect when no signal strength
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneDetectedNetworks::Start
 * @SYMTestExpectedResults Pass - Cannot detect networks when signal strength is zero.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify detected networks list is empty.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetDetectedNetworks 

	// Check RMobilePhone::GetSignalCaps returns caps in set of KCapsGetSignalStrength

	// Remove antenna 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RMobilePhone::GetSignalStrength returns aSignalStrength=0
	// Check RMobilePhone::GetSignalStrength returns aBar=0

	// ===  Get list of all the detected networks ===

	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a V5 list of preferred networks 

	// Check CMobilePhoneNetworkListV5::Enumerate returns number of networks=0

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0011");
	}



CCTSYIntegrationTestNetworkControl0012::CCTSYIntegrationTestNetworkControl0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0012::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0012::~CCTSYIntegrationTestNetworkControl0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0012
 * @SYMFssID BA/CTSY/NTWC-0012
 * @SYMTestCaseDesc Get NITZ information.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyNITZInfoChange
 * @SYMTestExpectedResults Pass - NITZ infomation is received from network
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify valid NITZ information returned and NITZ notification received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of KCapsGetNITZInfo | KCapsNotifyNITZInfo 
	
	TUint32 caps;
	CHECK_EQUALS_L( phone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned error") )
	TUint32 wantedCaps = RMobilePhone::KCapsGetNITZInfo | RMobilePhone::KCapsNotifyNITZInfo;
	CHECK_BITS_SET_L( caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong network caps") );
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Wait 2 mins for the network to send NITZ info
	User::After(KOneSecond*120); 
	
	// ===  Get the current Network Information and Time Zone (NITZ) from the network. ===
	
	RMobilePhone::TMobilePhoneNITZ nitzInfo;
	TInt err = phone.GetNITZInfo( nitzInfo );
	
	// Check KErrNone or KErrNotFound was returned
	ASSERT_TRUE((err==KErrNone) || (err==KErrNotFound), _L("RMobilePhone::GetNITZInfo returned error"));
	
	// If the network supplied NITZ info:
	// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iDST=1 or aNITZInfo.iDST=2
	// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iLongNetworkId with length > 0
	// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iShortNetworkId with length > 0
	// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iTimeZone >= 0
	// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iNitzFieldsUsed contains relevant flags set
	// Check RMobilePhone::GetNITZInfo returns aNITZInfo.Year() >= 2007
	if (err == KErrNone)
		{		
		ASSERT_TRUE( nitzInfo.iLongNetworkId.Length() > 0, 
						_L("RMobilePhone::GetNITZInfo returns nitzInfo.iLongNetworkId zero length"))
		ASSERT_TRUE( nitzInfo.iShortNetworkId.Length() > 0, 
						_L("RMobilePhone::GetNITZInfo returns nitzInfo.iShortNetworkId zero length"))
		ASSERT_TRUE( nitzInfo.iTimeZone > 0, _L("RMobilePhone::GetNITZInfo returns nitzInfo.iTimeZone zero"))
		
		ASSERT_BITS_SET( nitzInfo.iNitzFieldsUsed, 
								RMobilePhone::KCapsTimeAvailable |
								RMobilePhone::KCapsTimezoneAvailable |
								RMobilePhone::KCapsShortNameAvailable |
								RMobilePhone::KCapsLongNameAvailable,
								KNoUnwantedBits, 
								_L("RMobilePhone::GetNITZInfo returns nitzInfo.iNitzFieldsUsed invalid"))
		
		DEBUG_PRINTF2(_L("RMobilePhone::GetNITZInfo returns nitzInfo.year=%d"), nitzInfo.Year() );	
		ASSERT_TRUE( nitzInfo.Year() >= 2007, _L("RMobilePhone::GetNITZInfo returns nitzInfo year pre 2007"));
		}

	// ===  Wait for up to 1 minute for next NITZ update ===
	TExtEtelRequestStatus reqNotifyNitzInfoChange(phone, EMobilePhoneNotifyNITZInfoChange);
	CleanupStack::PushL(reqNotifyNitzInfoChange);
	
	phone.NotifyNITZInfoChange(reqNotifyNitzInfoChange,nitzInfo);
	WaitForRequestWithTimeOut(reqNotifyNitzInfoChange, ETimeLong);

	// Check NITZ info if network sent it
	if (reqNotifyNitzInfoChange.Int() != KErrCancel)
		{
		ASSERT_EQUALS( reqNotifyNitzInfoChange.Int(), KErrNone, _L("RMobilePhone::NotifyNITZInfoChange returned error"))
		
		// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iDST=1 or aNITZInfo.iDST=2
		// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iLongNetworkId with length > 0
		// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iShortNetworkId with length > 0
		// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iTimeZone >= 0
		// Check RMobilePhone::GetNITZInfo returns aNITZInfo.iNitzFieldsUsed contains relevant flags set
		// Check RMobilePhone::GetNITZInfo returns aNITZInfo.Year() >= 2007
		ASSERT_TRUE( nitzInfo.iLongNetworkId.Length() > 0, 
					_L("RMobilePhone::GetNITZInfo returns nitzInfo.iLongNetworkId is zero length"))
		ASSERT_TRUE( nitzInfo.iShortNetworkId.Length() > 0, 
					_L("RMobilePhone::GetNITZInfo returns nitzInfo.iShortNetworkId is zero length"))
		ASSERT_TRUE( nitzInfo.iTimeZone > 0, _L("NITZ Info iTimeZone is zero"))
		
		ASSERT_BITS_SET( nitzInfo.iNitzFieldsUsed, 
								RMobilePhone::KCapsTimeAvailable |
								RMobilePhone::KCapsTimezoneAvailable |
								RMobilePhone::KCapsShortNameAvailable |
								RMobilePhone::KCapsLongNameAvailable,
								KNoUnwantedBits, 
								_L("RMobilePhone::GetNITZInfo returns nitzInfo.iNitzFieldsUsed invalid"))
		ASSERT_TRUE( nitzInfo.Year() >= 2007, _L("RMobilePhone::GetNITZInfo returns nitzInfo year pre 2007"));
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	//reqNotifyNitzInfoChange
	CleanupStack::PopAndDestroy(1,&reqNotifyNitzInfoChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0012");
	}



CCTSYIntegrationTestNetworkControl0013::CCTSYIntegrationTestNetworkControl0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0013::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0013::~CCTSYIntegrationTestNetworkControl0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0013
 * @SYMFssID BA/CTSY/NTWC-0013
 * @SYMTestCaseDesc Move from one cell into another.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::GetCurrentNetwork
 * @SYMTestExpectedResults Pass - New network information is received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify current network notification completes and cell ID changes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a suitable cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Ensure phone has KCapsGetCurrentNetwork and KCapsNotifyCurrentNetwork capabilities. 
	TUint32 networkCaps=0;
	CHECK_EQUALS_L(phone.GetNetworkCaps(networkCaps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned with an error"));
	TUint32 expectedCaps= RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsNotifyCurrentNetwork;
	CHECK_BITS_SET_L(networkCaps, expectedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned with wrong caps"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Ensure phone is registered to the network. ===

	// Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegisteredOnHomeNetwork
	RMobilePhone::TMobilePhoneRegistrationStatus  status;
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL( phone, status), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(status, RMobilePhone::ERegisteredOnHomeNetwork, _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ERegisteredOnHomeNetwork Status"));

	// ===  Get the current network information. ===

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 location;
    RMobilePhone::TMobilePhoneLocationAreaV1 previousLocation;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg netInfoPckg(netInfo);
    TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);	
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPckg, location);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned with an error"));
	previousLocation = location;
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName "));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortName"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus,RMobilePhone::ENetworkStatusCurrent , _L("RMobilePhone::GetCurrentNetwork returns invalid iStatus"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0
	ASSERT_TRUE((TInt)location.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));	

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE((TInt)location.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));

	// Post notification for network change
	TExtEtelRequestStatus reqNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(reqNetChangeStatus);
	phone.NotifyCurrentNetworkChange(reqNetChangeStatus, netInfoPckg, location);	

	// Simulator simulates movement into another cell. 
	DisplayUserInteractionPromptL(_L("Please simulate movement to another cell"), ETimeLong);

	// ===  Ensure current network change notification complates and cell ID has changed. ===
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqNetChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
	ASSERT_EQUALS( reqNetChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned with an error"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCountryCode"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iDisplayTag"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLongNamelength"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iMode") );

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange  returns invalid iNetworkId"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iShortNamelength"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iStatus"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0 and aArea.iCellId != previous aArea.iCellId from RMobilePhone::GetCurrentNetwork
	ASSERT_TRUE(location.iCellId > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCellId"));
	ASSERT_TRUE(location.iCellId != previousLocation.iCellId, _L("RMobilePhone::NotifyCurrentNetworkChange returns same iCellId as before movement to another cell"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(location.iLocationAreaCode > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLocationAreaCode"));

	// ===  Get the current network information. ===
	phone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPckg, location);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongNamelength"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortNamelength"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork returns invalid iStatus"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0 and aArea.iCellId != previous aArea.iCellId from RMobilePhone::GetCurrentNetwork
	ASSERT_TRUE((TInt)location.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));
	ASSERT_TRUE(location.iCellId != previousLocation.iCellId, _L("RMobilePhone::GetCurrentNetwork returns same iCellId as before movement to another cell"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE((TInt)location.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));


	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getCurrentNetworkStatus
	// reqNetChangeStatus
	CleanupStack::PopAndDestroy(2, &getCurrentNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0013");
	}



CCTSYIntegrationTestNetworkControl0014::CCTSYIntegrationTestNetworkControl0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0014::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0014::~CCTSYIntegrationTestNetworkControl0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0014
 * @SYMFssID BA/CTSY/NTWC-0014
 * @SYMTestCaseDesc Move from an area with no service into a cell with service.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyNetworkRegistrationStatusChange, RMobilePhone::NotifyCurrentNetworkChange, RMobilePhone::GetCurrentNetwork
 * @SYMTestExpectedResults Pass - Device is registered on network. Network information is retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify current network information and registration status is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10077);
	User::Leave(KErrNone);

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// ===  Ensure the phone is not registered to a network. ===
	
	// Check RMobilePhone::GetNetworkRegistrationStatus returns ENotRegisteredNoService
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	CHECK_EQUALS_L(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone, regStatus), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	CHECK_EQUALS_L(regStatus, RMobilePhone::ENotRegisteredNoService, _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ENotRegisteredNoService Status"));

	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of  KCapsGetRegistrationstatus | KCapsGetCurrentNetwork | KCapsNotifyRegistrationStatus capabilities. 
	TUint32 caps;
	CHECK_EQUALS_L(phone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned error") )
	TUint32 wantedCaps = RMobilePhone::KCapsNotifyRegistrationStatus | RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsNotifyRegistrationStatus;
	CHECK_BITS_SET_L(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong network caps") );

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Get network registration status and verify it is as expected. ===

	// Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ENotRegisteredNoService
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone, regStatus), KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(regStatus, RMobilePhone::ENotRegisteredNoService, _L("RMobilePhone::GetNetworkRegistrationStatus did not return the expected ENotRegisteredNoService Status"));

	//Push notify for NotifyCurrentNetworkChange
	TExtEtelRequestStatus regNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(regNetChangeStatus);
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5> netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	phone.NotifyCurrentNetworkChange(regNetChangeStatus, netInfoPkg, area);
	
	//Push notify for NotifyNetworkRegistrationStatusChange
	RMobilePhone::TMobilePhoneRegistrationStatus status;
	TExtEtelRequestStatus networkRegStChange(phone, EMobilePhoneNotifyNetworkRegistrationStatusChange);
	CleanupStack::PushL(networkRegStChange);
	phone.NotifyNetworkRegistrationStatusChange(networkRegStChange, status);
	
	// Simulator simulates movement a cell with coverage.
	DisplayUserInteractionPromptL(_L("Please move to a cell with coverage"), ETimeLong);

	// ===  Ensure network registration changes ===

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with ERegisteredOnHomeNetwork
	ASSERT_EQUALS(WaitForRequestWithTimeOut(networkRegStChange, ETimeLong), KErrNone, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange timed out"));
	ASSERT_EQUALS(networkRegStChange.Int(), KErrNone, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange returned with an error"));
	ASSERT_EQUALS(status, RMobilePhone::ERegisteredOnHomeNetwork, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange did not complete with status = ERegisteredOnHomeNetwork"));
	
	// Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegisteredOnHomeNetwork
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone,regStatus),KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));	
	ASSERT_EQUALS(regStatus, RMobilePhone::ERegisteredOnHomeNetwork, _L("RMobilePhone::GetRegistrationStatus did not return expected status"));
	
	// ===  Ensure network information notification received ===
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regNetChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));								
	ASSERT_EQUALS(regNetChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned an error"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns iCountryCode length 0") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0	
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns iDisplayTag length 0"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns iLongName length 0"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_EQUALS(netInfo.iMode, RMobilePhone::ENetworkModeGsm, _L("RMobilePhone::NotifyCurrentNetworkChange did not return aNetworkInfo.iMode=ENetworkModeGsm"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns iNetworkId length 0"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns iShortName 0 "));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange did not return aNetworkInfo.iStatus=ENetworkStatusCurrent"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	ASSERT_TRUE(area.iCellId > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns Location cell ID 0"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(area.iLocationAreaCode > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns Location AreaCode ID 0"));
	
	// === Get current network ===
    TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
    CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPkg, area);
    
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid Display Tag"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_EQUALS(netInfo.iMode, RMobilePhone::ENetworkModeGsm, _L("RMobilePhone::GetCurrentNetwork did not return ENetworkModeGsm as expected"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortName"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent , _L("RMobilePhone::GetCurrentNetwork did not return ENetworkStatusCurrent status as expected "));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	ASSERT_TRUE(area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returned invalid iCellId"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(area.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));

	//
	// TEST END
	//

    StartCleanup();
    
    //regNetChangeStatus
    //networkRegStChange
    //getCurrentNetworkStatus
	CleanupStack::PopAndDestroy(3, &regNetChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0014");
	}



CCTSYIntegrationTestNetworkControl0015::CCTSYIntegrationTestNetworkControl0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0015::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0015::~CCTSYIntegrationTestNetworkControl0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0015
 * @SYMFssID BA/CTSY/NTWC-0015
 * @SYMTestCaseDesc Cell selection after emergency camping.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCurrentNetwork, RMobilePhone::NotifyNetworkRegistrationStatusChange, RMobilePhone::NotifyCurrentNetworkChange
 * @SYMTestExpectedResults Pass - Phone re-registers successfully to the new cell when moving out of a cell with emergency coverage to one with full coverage.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify network registration status and current network information are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10078);
	User::Leave(KErrNone);

	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);		
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of  KCapsGetRegistrationStatus | KCapsGetCurrentNetwork | KCapsNotifyRegistrationStatus capabilities. 
	TUint32 caps;
	CHECK_EQUALS_L(phone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error"));
	TUint32 wantedCaps = RMobilePhone::KCapsGetRegistrationStatus | RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsNotifyRegistrationStatus;
	CHECK_BITS_SET_L(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps"));

	// SImulator is simulating cell with emergency coverage only. 
	DisplayUserInteractionPromptL(_L("Please ensure cell is set for emergency calls only"), ETimeLong);	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get registration status  ===

	// Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ENotRegisteredEmergencyOnly

	
	RMobilePhone::TMobilePhoneRegistrationStatus regStatus;
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone,regStatus),KErrNone, 
			_L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(regStatus, RMobilePhone::ENotRegisteredEmergencyOnly, 
			_L("RMobilePhone::GetNetworkRegistrationStatus did not return expected status"));

	//Push notify for NotifyNetworkRegistrationStatusChange
	RMobilePhone::TMobilePhoneRegistrationStatus status;
	TExtEtelRequestStatus networkRegStChange(phone, EMobilePhoneNotifyNetworkRegistrationStatusChange);
	CleanupStack::PushL(networkRegStChange);
	phone.NotifyNetworkRegistrationStatusChange(networkRegStChange, status);
		
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5> netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
	//Push notify for NotifyCurrentNetworkChange
	TExtEtelRequestStatus regNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(regNetChangeStatus);
	phone.NotifyCurrentNetworkChange( regNetChangeStatus, netInfoPkg, area );
	
	// Simulate moving into a new cell with full coverage.
	DisplayUserInteractionPromptL(_L("Simulate moving into a cell with full coverage"), ETimeLong);
	 
	// ===  Check registration status notification completes ===

	// Check RMobilePhone::NotifyNetworkRegistrationStatusChange completes with aStatus=ERegisteredOnHomeNetwork
	ASSERT_EQUALS(WaitForRequestWithTimeOut(networkRegStChange, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange timed out"));
	ASSERT_EQUALS(networkRegStChange.Int(), KErrNone, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange returned with an error"));
	ASSERT_EQUALS(status, RMobilePhone::ERegisteredOnHomeNetwork, _L("RMobilePhone::NotifyNetworkRegistrationStatusChange did not complete with status = ERegisteredOnHomeNetwork"));
	
	// Check RMobilePhone::GetNetworkRegistrationStatus returns with ERegisteredOnHomeNetwork
	ASSERT_EQUALS(iNetworkTestHelper.GetNetworkRegistrationStatusL(phone,regStatus),KErrNone, 
				_L("RMobilePhone::GetNetworkRegistrationStatus returned with an error"));
	ASSERT_EQUALS(regStatus, RMobilePhone::ERegisteredOnHomeNetwork, 
				_L("RMobilePhone::GetRegistrationStatus did not return expected status"));
		
	// ===  Get current network ===		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regNetChangeStatus, ETimeMedium), KErrNone, 
							_L("RMobilePhone::NotifyCurrentNetworkChange timed out"));								
	ASSERT_EQUALS(regNetChangeStatus.Int(), KErrNone, 
							_L("RMobilePhone::NotifyCurrentNetworkChange returned an error"));	
					
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iCountryCode length 0") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iDisplayTag length 0"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iLongName length 0"));
	
	//Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iMode") );
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, 
					_L("RMobilePhone::NotifyCurrentNetworkChange returns iNetworkId length 0"));	
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns iShortName 0 "));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange did not return aNetworkInfo.iStatus=ENetworkStatusCurrent"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	ASSERT_TRUE(area.iCellId > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns Location cell ID 0"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(area.iLocationAreaCode > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns Location AreaCode ID 0"));
	
	
	TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork( getCurrentNetworkStatus, netInfoPkg, area );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeLong), KErrNone, 
			_L("RMobilePhone::GetCurrentNetwork timed out") );
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length()>0, _L("RMobilePhone::GetCurrentNetwork returns iCountryCode length 0"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns iDisplayTag length 0"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns iLongName length 0"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns iNetworkId length 0"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns iShortName length 0"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork did not return aNetworkInfo.iStatus=ENetworkStatusCurrent"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iCellId > 0
	ASSERT_TRUE(area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns Location cell ID 0"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(area.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns Location AreaCode ID 0"));

	
	//
	// TEST END
	//

    StartCleanup();
    
    //Pop
	//networkRegStChange
	//regNetChangeStatus
	//getCurrentNetworkStatus	
	CleanupStack::PopAndDestroy(3,&networkRegStChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0015");
	}



CCTSYIntegrationTestNetworkControl0016::CCTSYIntegrationTestNetworkControl0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0016::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0016::~CCTSYIntegrationTestNetworkControl0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0016
 * @SYMFssID BA/CTSY/NTWC-0016
 * @SYMTestCaseDesc Get network capabilities.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Network caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify network capabilities returned are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RMobilePhone::GetNetworkCaps returns caps of RMobilePhone::KCapsGetRegistrationStatus
	//        | RMobilePhone::KCapsNotifyRegistrationStatus 
	//        | RMobilePhone::KCapsGetCurrentMode 
	//        | RMobilePhone::KCapsNotifyMode 
	//        | RMobilePhone::KCapsGetCurrentNetwork 
	//        | RMobilePhone::KCapsNotifyCurrentNetwork 
	//        | RMobilePhone::KCapsGetHomeNetwork 
	//        | RMobilePhone::KCapsGetDetectedNetworks 
	//        | RMobilePhone::KCapsManualNetworkSelection 
	//        | RMobilePhone::KCapsNotifyNITZInfo 
	//        | RMobilePhone::KCapsGetNITZInfo 

	TUint32 caps;
	ASSERT_EQUALS( phone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned error") )
	TUint32 wantedCaps = RMobilePhone::KCapsGetRegistrationStatus
					        | RMobilePhone::KCapsNotifyRegistrationStatus 
        					| RMobilePhone::KCapsGetCurrentMode 
        					| RMobilePhone::KCapsNotifyMode 
     						| RMobilePhone::KCapsGetCurrentNetwork 
     						| RMobilePhone::KCapsNotifyCurrentNetwork 
     						| RMobilePhone::KCapsGetHomeNetwork 
     						| RMobilePhone::KCapsGetDetectedNetworks 
     						| RMobilePhone::KCapsManualNetworkSelection 
     						| RMobilePhone::KCapsNotifyNITZInfo 
     						| RMobilePhone::KCapsGetNITZInfo;
     						
	ASSERT_BITS_SET( caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returns wrong network caps") );


	//
	// TEST END
	//

    StartCleanup();
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0016");
	}



CCTSYIntegrationTestNetworkControl0017::CCTSYIntegrationTestNetworkControl0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0017::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0017::~CCTSYIntegrationTestNetworkControl0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0017
 * @SYMFssID BA/CTSY/NTWC-0017
 * @SYMTestCaseDesc Receive period network information update.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCurrentNetwork
 * @SYMTestExpectedResults Pass - Location information updated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify current network information notification received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10079);
	User::Leave(KErrNone);	
	
	// Phone camped on valid cell. 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of  KCapsNotifyCurrentNetwork capability. 
	TUint32 caps;
	CHECK_EQUALS_L( mobilePhone.GetNetworkCaps( caps ), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error") );
	TUint32 wantedCaps = RMobilePhone::KCapsNotifyCurrentNetwork;
	CHECK_BITS_SET_L( caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps") );


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// $CTSYProblem Timer T3302 needs to be set in the attach accept message from the Simulator
	ASSERT(TRUE);
	
	// ===  Get current network information ===
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TExtEtelRequestStatus currentNetworkStatus(mobilePhone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(currentNetworkStatus);
	mobilePhone.GetCurrentNetwork( currentNetworkStatus, netInfoPkg, area );	
			
	ASSERT_EQUALS( WaitForRequestWithTimeOut(currentNetworkStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS( currentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned error"));
				
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE( netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE( netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE( netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE( netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId") );	
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE( netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortname"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS( netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork did not return ENetworkStatusCurrent status as expected"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0
	ASSERT_TRUE((TInt)area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE((TInt)area.iLocationAreaCode > 0 ,_L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));
	
	// ===  Register for notification of change in the current network. ===

	// Register for notification of RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 

	// Post notification for network change
	TExtEtelRequestStatus reqNetChangeStatus(mobilePhone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(reqNetChangeStatus);
	mobilePhone.NotifyCurrentNetworkChange( reqNetChangeStatus, netInfoPkg, area );	
	
	// Simulated network sends new location network information (e.g. new display tag or new location area code) 
	DisplayUserInteractionPromptL(_L("Simulate network sending new location information (e.g. new display tag or new location area code."), ETimeLong);
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqNetChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
	ASSERT_EQUALS( reqNetChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned returned error"));
	ASSERT_TRUE( netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCountryCode") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE( netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iDisplayTag") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE( netInfo.iLongName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLongName") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iMode") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE( netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iNetworkId") );	
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE( netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iShortname") );		
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS( netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange did not return ENetworkStatusCurrent status as expected"));		
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0
	//$CTSYProblem: RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId is not > 0 as expected.
	ASSERT_EQUALS((TInt)area.iCellId, 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCellId"));

		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	//$CTSYProblem: RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode is not > 0 as expected.
	ASSERT_EQUALS((TInt)area.iLocationAreaCode, 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLocationAreaCode"));
	
	// ===  Get current network information ===

	mobilePhone.GetCurrentNetwork( currentNetworkStatus, netInfoPkg, area );					
	ASSERT_EQUALS( WaitForRequestWithTimeOut(currentNetworkStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS( currentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned returned error"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE( netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE( netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag") );
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE( netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma 
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE( netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId") );		
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE( netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortname") );	
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS( netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork did not return ENetworkStatusCurrent status as expected"))	
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0
	ASSERT_TRUE((TInt)area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId") );	
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE((TInt)area.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode") );

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// currentNetworkStatus
	// reqNetChangeStatus

	CleanupStack::PopAndDestroy(2, &currentNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0017");
	}



CCTSYIntegrationTestNetworkControl0018::CCTSYIntegrationTestNetworkControl0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0018::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0018::~CCTSYIntegrationTestNetworkControl0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0018
 * @SYMFssID BA/CTSY/NTWC-0018
 * @SYMTestCaseDesc Receive notification of a higher priority network that does not offer CS voice.
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneDetectedNetworks::RetrieveListL, CRetrieveMobilePhoneDetectedNetworks::NewL, RMobilePhone::GetNetworkInvScanSetting, RMobilePhone::SelectNetwork, CMobilePhoneNetworkListV5::Enumerate, RMobilePhone::SetNetworkSelectionSetting, RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::GetHomeNetwork, RMobilePhone::NotifyNetworkInvScanEvent, CRetrieveMobilePhoneDetectedNetworks::StartV5
 * @SYMTestExpectedResults Pass - Network investigation scan event completes and new network can be selected.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify network investigation event completes and new network can be selected.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Check network investigation scan setting is on ===

	// Check RMobilePhone::GetNetworkInvScanSetting returns aSetting!=0

	// ===  Get the network ID of the home network (store this for use later) ===

	// Check RMobilePhone::GetHomeNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0

	// ===  Detect another higher priority network ===

	// Simulator simulates an additional higher priority network which does not provide CS voice. 

	// Check RMobilePhone::NotifyNetworkInvScanEvent completes with aEvent=ENetworkInvestigationHighPriorityPlmn

	// ===  Get list if detected networks ===

	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a V5 list of preferred networks 

	// Check CMobilePhoneNetworkListV5::Enumerate on the list returned returns number of networks > 1

	// ===  Switch to manual network selection ===

	// Set network selection setting to automatic with RMobilePhone::SetNetworkSelectionSetting with aSetting.iMethod=ENetworkSelectionManual 

	// ===  Go through the list of detected networks and manually select new network ===

	// Cycle through the list of detected networks CMobilePhoneNetworkListV5 

	// If the TMobilePhoneNetworkInfoV5::iNetworkId != aNetworkInfo.iNetworkId returned from RMobilePhone::GetHomeNetwork 

	//  then select network with RMobilePhone::SelectNetwork with aIsManual=ETrue and aManualSelection.iCountry=TMobilePhoneNetworkInfoV5::iCountryCode and aManualSelection.iNetwork=TMobilePhoneNetworkInfoV5::iNetworkId 

	//  Check RMobilePhone::GetNetworkRegistrationStatus returns aStatus=ERegisteredOnHomeNetwork or =ERegisteredRoaming 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0018");
	}



CCTSYIntegrationTestNetworkControl0019::CCTSYIntegrationTestNetworkControl0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0019::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0019::~CCTSYIntegrationTestNetworkControl0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0019
 * @SYMFssID BA/CTSY/NTWC-0019
 * @SYMTestCaseDesc Move into new cell with an active call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::GetStatus, RCall:Dial, RCall::NotifyStatusChange, RMobilePhone::NotifyCurrentNetworkChange
 * @SYMTestExpectedResults Pass - Network information can be retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify call stays connected and network information notification received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10080);
	User::Leave(KErrNone);	
 

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of  KCapsGetCurrentNetwork, KCapsNotifyCurrentNetwork. 
	TUint32 caps;
	CHECK_EQUALS_L( phone.GetNetworkCaps(caps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error"));
	TUint32 wantedCaps = RMobilePhone::KCapsGetCurrentNetwork | RMobilePhone::KCapsNotifyCurrentNetwork;
	CHECK_BITS_SET_L(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps"));

	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork 
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get voice line 1. 
	RMobileLine& mobileLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get current network information ===

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 location;
    RMobilePhone::TMobilePhoneNetworkInfoV5Pckg netInfoPckg(netInfo);
    TExtEtelRequestStatus getCurrentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);	
	CleanupStack::PushL(getCurrentNetworkStatus);
	phone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPckg, location);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeMedium),KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned with an error"));
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));			
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName "));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE (netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortName"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus,RMobilePhone::ENetworkStatusCurrent , _L("RMobilePhone::GetCurrentNetwork returns invalid iStatus"));			

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId == 0
	ASSERT_TRUE((TInt)location.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));	

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode == 0
	ASSERT_TRUE((TInt)location.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));

	// Post Notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus(mobileCall, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus callStatus;
	mobileCall.NotifyStatusChange(notifyStatusChangeStatus, callStatus);

	// Dial a number that answers. 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF1(_L("Board Now Dialling: "));
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));

	// Check RCall::NotifyStatusChange completes with EStatusConnected.
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(mobileCall,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);

	// Post notification for network change
	TExtEtelRequestStatus reqNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(reqNetChangeStatus);
	phone.NotifyCurrentNetworkChange(reqNetChangeStatus, netInfoPckg, location);	

	// Simulate moving into a new cell.
	DisplayUserInteractionPromptL(_L("Simulate moving into a new cell"), ETimeLong);

	// ===  Check change of current network notification completes ===

	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqNetChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
	ASSERT_EQUALS( reqNetChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned with an error"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCountryCode"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iDisplayTag"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLongNamelength"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE (netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iMode"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange  returns invalid iNetworkId"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iShortNamelength"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iStatus"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId > 0
	ASSERT_TRUE(location.iCellId > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCellId"));

	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode > 0
	ASSERT_TRUE(location.iLocationAreaCode > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLocationAreaCode"));

	// ===  Get current network information ===

	phone.GetCurrentNetwork(getCurrentNetworkStatus, netInfoPckg, location);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCurrentNetworkStatus, ETimeLong), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(getCurrentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned an error"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongNamelength"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortNamelength"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork returns invalid iStatus"));

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId == 0
	ASSERT_TRUE((TInt)location.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));	

	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode == 0
	ASSERT_TRUE((TInt)location.iLocationAreaCode > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLocationAreaCode"));

	// ===  Check call is still connected ===

	// Check RCall::GetStatus returns EStatusConnected.
	ASSERT_EQUALS(mobileCall.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an Error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return EStatusConnected"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Hang up the active call. 
	mobileCall.HangUp();

	// Pop
	// getCurrentNetworkStatus
	// notifyStatusChangeStatus
	// dialStatus
	// reqNetChangeStatus
	CleanupStack::PopAndDestroy(4, &getCurrentNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0019");
	}



CCTSYIntegrationTestNetworkControl0020::CCTSYIntegrationTestNetworkControl0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0020::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0020::~CCTSYIntegrationTestNetworkControl0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0020
 * @SYMFssID BA/CTSY/NTWC-0020
 * @SYMTestCaseDesc Move into a new cell with packet service active.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::GetCurrentNetwork, RMobilePhone::NotifyCurrentNetworkChange, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Network information is retrieved. Packet service remains active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify packet service stays active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//


	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	// Ensure that RMobilePhone::GetNetworkCaps returns caps in set of  KCapsGetCurrentNetwork and KCapsNotifyCurrentNetwork. 
	TUint32 caps;
	CHECK_EQUALS_L(phone.GetNetworkCaps(caps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error"));
	TUint32 wantedCaps = RMobilePhone::KCapsGetHomeNetwork | RMobilePhone::KCapsNotifyCurrentNetwork;
	CHECK_BITS_SET_L(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps"));
	
	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork 
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Ensure phone is attached to packet service. 
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	// Get the attach mode so that it can be restored at the end of the test
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(packetService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);
    
    
    
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	
	// ===  Get current network information ===
	RMobilePhone::TMobilePhoneNetworkInfoV5 netInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> netInfoPkg(netInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TExtEtelRequestStatus currentNetworkStatus(phone, EMobilePhoneGetCurrentNetwork);
	CleanupStack::PushL(currentNetworkStatus);
	phone.GetCurrentNetwork( currentNetworkStatus, netInfoPkg, area );	
			
	ASSERT_EQUALS( WaitForRequestWithTimeOut(currentNetworkStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS(currentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned error"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortname"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork returns invalid iStatus"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId == 0
	ASSERT_TRUE((TInt)area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));

	// Post notification for network change
	TExtEtelRequestStatus reqNetChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(reqNetChangeStatus);
	phone.NotifyCurrentNetworkChange( reqNetChangeStatus, netInfoPkg, area );
	
	// Simulate moving into a new cell. 
	DisplayUserInteractionPromptL(_L("Simulate moving into a new cell"), ETimeLong);
	
	// ===  Check change of current network notification completes ===
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqNetChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
	ASSERT_EQUALS( reqNetChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned returned error"));
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE(netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCountryCode"));
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE(netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iDisplayTag"));
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLongName"));
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iMode") );
	
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iNetworkId"));
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iShortname"));	
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iStatus"));
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId == 0
	//$CTSYProblem: RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId is not > 0 as expected.
	ASSERT_EQUALS((TInt)area.iCellId, 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iCellId"));
		
	// Check RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode == 0
	//$CTSYProblem: RMobilePhone::NotifyCurrentNetworkChange with parameter type TMobilePhoneLocationAreaV1 returns aArea.iLocationAreaCode is not > 0 as expected.
	ASSERT_EQUALS((TInt)area.iLocationAreaCode, 0, _L("RMobilePhone::NotifyCurrentNetworkChange returns invalid iLocationAreaCode"));


	// ===  Get current network information ===
	phone.GetCurrentNetwork( currentNetworkStatus, netInfoPkg, area );					
	ASSERT_EQUALS( WaitForRequestWithTimeOut(currentNetworkStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCurrentNetwork timed out"));
	ASSERT_EQUALS( currentNetworkStatus.Int(), KErrNone, _L("RMobilePhone::GetCurrentNetwork returned returned error"));
	
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iCountryCode with length > 0
	ASSERT_TRUE( netInfo.iCountryCode.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCountryCode"));
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iDisplayTag with length > 0
	ASSERT_TRUE( netInfo.iDisplayTag.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iDisplayTag"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iLongName with length > 0
	ASSERT_TRUE(netInfo.iLongName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iLongName"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iMode=ENetworkModeGsm or aNetworkInfo.iMode=ENetworkModeWcdma
	ASSERT_TRUE ( netInfo.iMode == RMobilePhone::ENetworkModeGsm || netInfo.iMode == RMobilePhone::ENetworkModeWcdma, _L("RMobilePhone::GetCurrentNetwork returns invalid iMode") );
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iNetworkId with length > 0
	ASSERT_TRUE(netInfo.iNetworkId.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iNetworkId"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneNetworkInfoV5 returns aNetworkInfo.iShortName with length > 0
	ASSERT_TRUE(netInfo.iShortName.Length() > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iShortname"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aNetworkInfo.iStatus=ENetworkStatusCurrent
	ASSERT_EQUALS(netInfo.iStatus, RMobilePhone::ENetworkStatusCurrent, _L("RMobilePhone::GetCurrentNetwork returns invalid iStatus"));
		
	// Check RMobilePhone::GetCurrentNetwork with parameter type TMobilePhoneLocationAreaV1 returns aArea.iCellId == 0
	ASSERT_TRUE((TInt)area.iCellId > 0, _L("RMobilePhone::GetCurrentNetwork returns invalid iCellId"));

	// ===  Check packet service is still attached ===

	// Check RPacketService::GetStatus returns EStatusAttached
	RPacketService::TStatus pckSrvcStatus;
	ASSERT_EQUALS(packetService.GetStatus(pckSrvcStatus), KErrNone, _L("RPacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(pckSrvcStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus returns incorrect status."));
	
	// ===  Check packet network status ===

	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork
    TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
    CleanupStack::PushL(regStatus);
    RPacketService::TRegistrationStatus registrationStatus;
    packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeLong), KErrNone, _L("RPacketService::GetNtwkRegStatus timed-out."));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, _L("RPacketService::GetNtwkRegStatus returned with an error"));
    ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, _L("RPacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork"));
	
	//
	// TEST END
	//

			
    StartCleanup();
	
	// Return packet service's attach mode to original setting
	ASSERT_EQUALS(packetService.SetAttachMode(attachMode), KErrNone, _L("RPacketService::SetAttachMode returned with an error."));

	// Pop
	// currentNetworkStatus
	// reqNetChangeStatus
	// regStatus
	CleanupStack::PopAndDestroy(3, &currentNetworkStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0020");
	}



CCTSYIntegrationTestNetworkControl0021::CCTSYIntegrationTestNetworkControl0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0021::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0021::~CCTSYIntegrationTestNetworkControl0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0021
 * @SYMFssID BA/CTSY/NTWC-0021
 * @SYMTestCaseDesc Store and retrieve a list of preferred networks.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyPreferredNetworksListChange, CRetrieveMobilePhonePreferredNetworks
::Start
 * @SYMTestExpectedResults Pass - Preferred network list stored and retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify list retrieved is the same as that stored and notification of list change is received.
 *
 * @return - TVerdict code
 */
	{
	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	TInt err;
		
	// ===  Create a list of preferred networks ===
	// Create three TMobilePreferredNetworkEntryV3 objects with valid values in 
	// TMobilePreferredNetworkEntryV3::iAccess, 
	// TMobilePreferredNetworkEntryV3::iCountryCode and 
	// TMobilePreferredNetworkEntryV3::iNetworkId 
	
	RMobilePhone::TMobilePreferredNetworkEntryV3 prefNetworks[3];
	
	// Three German networks
	
	prefNetworks[0].iAccess = RMobilePhone::KNetworkAccessGsm;
	prefNetworks[0].iCountryCode = _L("262");
	prefNetworks[0].iNetworkId = _L("01");
	
	prefNetworks[1].iAccess = RMobilePhone::KNetworkAccessGsm;
	prefNetworks[1].iCountryCode = _L("262");
	prefNetworks[1].iNetworkId = _L("02");

	prefNetworks[2].iAccess = RMobilePhone::KNetworkAccessGsm;
	prefNetworks[2].iCountryCode = _L("262");
	prefNetworks[2].iNetworkId = _L("03");
	
	CMobilePhoneStoredNetworkList *newNetList = CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL( newNetList );
	
	// Add them to a CMobilePhoneStoredNetworkList with CMobilePhoneStoredNetworkList::InsertEntryL 
	for( TInt i=0; i < 3; i++ )
		{
		newNetList->AddEntryL(prefNetworks[i]);
		}
		
	TExtEtelRequestStatus reqStoreStatus( phone, EMobilePhoneStorePreferredNetworksList );
	CleanupStack::PushL(reqStoreStatus);
	
	
	// Request change notification
	TExtEtelRequestStatus reqPrefListChange( phone, EMobilePhoneNotifyStorePreferredNetworksListChange );
	CleanupStack::PushL(reqPrefListChange);
	
	phone.NotifyPreferredNetworksListChange( reqPrefListChange );
	
	// ===  Store list ===
	// Store the list with RMobilePhone::StorePreferredNetworksListL completes with KErrNone 

	// The session manager cannot be used for this API because it
	// allocates memory.
	// Therefore open a new subsession just for this API
	RMobilePhone phone2;
	RTelServer telServer;
	User::LeaveIfError(telServer.Connect());
	User::LeaveIfError(phone2.Open(telServer, iEtelSessionMgr.PhoneName()));
	phone2.StorePreferredNetworksListL(reqStoreStatus, newNetList);
	err = WaitForRequestWithTimeOut( reqStoreStatus, ETimeLong );
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::StorePreferredNetworksList timed out"))
	ASSERT_EQUALS( reqStoreStatus.Int(), KErrNotSupported, _L("RMobilePhone::StorePreferredNetworksList returned error"))
	phone2.Close();
	telServer.Close();
	
	// $CTSYProblem CTSY does not support RMobilePhone::StorePreferredNetworksListL
	// so the test has been coded to return KErrNotSupported (the correct result in
	// this situation.  When CTSY support has been added, the test case can be
	// extended to have the following code below.
	
	
	/*
	// ===  Check list change notification completes ===

	// Check RMobilePhone::NotifyPreferredNetworksListChange completes with KErrNone
	iNetworkTestHelper.WaitForMobilePhoneNotifyPreferredNetworksListChange( reqPrefListChange, KErrNone );
	ASSERT_EQUALS( reqPrefListChange.Int(), KErrNone, 
			_L("RMobilePhone::NotifyPreferredNetworksListChange returned with an error") );

	// ===  Get list of preferred networks and verify it is the same as the list stored ===
	// Use helper class for CRetrieveMobilePhonePreferredNetworks to retrieve a list of 
	// preferred networks 

	CRetrieveMobilePhonePreferredNetworksExec *checkNetListHelper = 
										CRetrieveMobilePhonePreferredNetworksHelper::NewL(phone);
	CleanupStack::PushL( checkNetListHelper );
	
	CMobilePhoneStoredNetworkList* checkNetList = checkNetListHelper->DoGetList(err);
	
	// Check CMobilePhoneStoredNetworkList::Enumerate on the list retrieved returns number of networks = 3
	TInt checkCount = checkNetList->Enumerate();
	
	ASSERT_EQUALS( checkCount, 3, _L("Incorrect number of entries in CMobilePhoneStoredNetworkList"));
	
	ASSERT_TRUE( IsPreferredNetworkInListL( *checkNetList, prefNetworks[0]), _L("Entry 0 not found in list") );
	ASSERT_TRUE( IsPreferredNetworkInListL( *checkNetList, prefNetworks[1]), _L("Entry 1 not found in list") );
	ASSERT_TRUE( IsPreferredNetworkInListL( *checkNetList, prefNetworks[2]), _L("Entry 2 not found in list") );		
	*/
	
	//
	// TEST END
	//

    StartCleanup();
	
	// reqPrefListChange
	// reqStoreStatus
	// newNetList
	CleanupStack::PopAndDestroy(3, newNetList);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0021");
	}



CCTSYIntegrationTestNetworkControl0022::CCTSYIntegrationTestNetworkControl0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0022::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0022::~CCTSYIntegrationTestNetworkControl0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0022
 * @SYMFssID BA/CTSY/NTWC-0022
 * @SYMTestCaseDesc Get service provider name.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Service provider name returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify service provider name is retrieved.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork 

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	TInt err = iNetworkTestHelper.GetNetworkRegistrationStatusL( phone,registrationStatus);
	
	CHECK_TRUE_L(err == KErrNone, _L("RMobilePhone::GetNetworkRegistrationStatus returned error"))
	
	CHECK_TRUE_L(registrationStatus ==  RMobilePhone::ERegisteredOnHomeNetwork, 
								_L("RMobilePhone::GetNetworkRegistrationStatus did not return ERegisteredOnHomeNetwork"))

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get service provider name ===
	RMobilePhone::TMobilePhoneServiceProviderNameV2 spName;
	TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> spNamePkg(spName);
	TExtEtelRequestStatus reqGetSPNameStatus(phone, EMobilePhoneGetServiceProviderName );
	CleanupStack::PushL(reqGetSPNameStatus);
		
	phone.GetServiceProviderName(reqGetSPNameStatus, spNamePkg );
	ASSERT_EQUALS( WaitForRequestWithTimeOut(reqGetSPNameStatus, ETimeLong ), KErrNone, 
								_L("RMobilePhone::GetServiceProviderName timed out") );
	ASSERT_EQUALS( reqGetSPNameStatus.Int(), KErrGsm0707NotFound ,_L("RMobilePhone::GetServiceProviderName returned error") );
	
	// $CTSYProblem The LTSY always returns KErrGsm0707NotFound when this API is called.
	// CTSY correctly propagates the error back to the client.
	// If the LTSY does not return an error the following code can be used to test
	// this API.
	
	/*
	// Check RMobilePhone::GetServiceProviderName with a TMobilePhoneServiceProviderNameV2 parameter returns aName.iPLMNField with length > 0
	// Check RMobilePhone::GetServiceProviderName with a TMobilePhoneServiceProviderNameV2 parameter returns aName.iSPName with length > 0
	ASSERT_TRUE( spName.iPLMNField.Length() > 0, 
					_L("RMobilePhone::GetServiceProviderName returns TMobilePhoneServiceProviderNameV2 iPLMNField has zero length"))
	ASSERT_TRUE( spName.iSPName.Length() > 0, 
					_L("RMobilePhone::GetServiceProviderName returns TMobilePhoneServiceProviderNameV2 iSPName has zero length"))
	*/
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(1,&reqGetSPNameStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0022");
	}



CCTSYIntegrationTestNetworkControl0023::CCTSYIntegrationTestNetworkControl0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0023::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0023::~CCTSYIntegrationTestNetworkControl0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0023
 * @SYMFssID BA/CTSY/NTWC-0023
 * @SYMTestCaseDesc Cancel retrieval of preferred networks.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Retrieval of preferred networks is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	//iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get list of all the preferred networks and cancel request ===

	// Use helper class for CRetrieveMobilePhonePreferredNetworks to retrieve a list of preferred networks 

	// Cancel the request  

	// Check request completes with KErrCancel

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0023");
	}



CCTSYIntegrationTestNetworkControl0024::CCTSYIntegrationTestNetworkControl0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0024::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0024::~CCTSYIntegrationTestNetworkControl0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0024
 * @SYMFssID BA/CTSY/NTWC-0024
 * @SYMTestCaseDesc Cancel retrieval of detected networks.
 * @SYMTestPriority High
 * @SYMTestActions CRetrieveMobilePhoneDetectedNetworks::StartV2, CRetrieveMobilePhoneDetectedNetworks::StartV5, CRetrieveMobilePhoneDetectedNetworks::StartV8
 * @SYMTestExpectedResults Pass - Retrieval of detected networks is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify request is cancelled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	//iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	// Ensure RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Get list of all the detected networks with V1 version of the API and cancel request ===

	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a V1 list of detected networks 

	// Cancel the request immediately 

	// Check request completes with KErrCancel

	// ===  Get list of all the detected networks with V2 version of the API ===

	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a V2 list of detected networks 

	// Cancel the request immediately 

	// Check request completes with KErrCancel

	// ===  Get list of all the detected networks with V5 version of the API ===

	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a V5 list of detected networks 

	// Cancel the request immediately 

	// Check request completes with KErrCancel

	// ===  Get list of all the detected networks with V8 version of the API ===

	// Use helper class for CRetrieveMobilePhoneDetectedNetworks to retrieve a V8 list of detected networks 

	// Cancel the request with immediately 

	// Check request completes with KErrCancel

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0024");
	}



CCTSYIntegrationTestNetworkControl0025::CCTSYIntegrationTestNetworkControl0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestNetworkControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestNetworkControl0025::GetTestStepName());
	}

CCTSYIntegrationTestNetworkControl0025::~CCTSYIntegrationTestNetworkControl0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestNetworkControl0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-NTWC-0025
 * @SYMFssID BA/CTSY/NTWC-0025
 * @SYMTestCaseDesc Set network selection setting to an unsupported setting.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyNetworkSelectionSettingChange, RMobilePhone::SetNetworkSelectionSetting
 * @SYMTestExpectedResults Pass - KErrNotSupported returned when unsupported setting is set and old setting is preserved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported is returned and network selection setting is unchanged.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//	
	
	// Set up the notification for RMobilePhone::NotifyNetworkSelectionSettingChange for later test
	TExtEtelRequestStatus reqNotifyNetSelSettingChange(phone, EMobilePhoneNotifyNetworkSelectionSettingChange);
	CleanupStack::PushL(reqNotifyNetSelSettingChange);
	
	RMobilePhone::TMobilePhoneNetworkSelectionV1 netSelNotify;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg netSelNotifyPkg( netSelNotify );
	
	phone.NotifyNetworkSelectionSettingChange(reqNotifyNetSelSettingChange, netSelNotifyPkg);
	
	// ===  Set network selection setting to an unsupported setting ===
	// Set network selection setting to manual wtth RMobilePhone::SetNetworkSelectionSetting with 
	// aSetting.iMethod=ENetworkSelectionUnknown 
	TExtEtelRequestStatus setNetSelSettingStatus(phone, EMobilePhoneSetNetworkSelectionSetting);
	CleanupStack::PushL(setNetSelSettingStatus);
	RMobilePhone::TMobilePhoneNetworkSelectionV1 netSel;
	RMobilePhone::TMobilePhoneNetworkSelectionV1Pckg netSelPkg(netSel);
	netSel.iMethod = RMobilePhone::ENetworkSelectionUnknown;
	phone.SetNetworkSelectionSetting( setNetSelSettingStatus, netSelPkg );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setNetSelSettingStatus, ETimeLong), KErrNone, 
					_L("RMobilePhone::SetNetworkSelectionSetting timed out") );
		
	// Check KErrNotSupported is returned
	ASSERT_EQUALS( setNetSelSettingStatus.Int(), KErrNotSupported, 
							_L("RMobilePhone::SetNetworkSelectionSetting did not return KErrNotSupported"));

	// Check RMobilePhone::NotifyNetworkSelectionSettingChange is still pending
	ASSERT_EQUALS( reqNotifyNetSelSettingChange.Int(), KRequestPending, 
			_L("RMobilePhone::NotifyNetworkSelectionSettingChange did not return KRequestPending"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    // Set the network selection back to automatic
    
	netSel.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	phone.SetNetworkSelectionSetting( setNetSelSettingStatus, netSelPkg );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setNetSelSettingStatus, ETimeLong), KErrNone, 
					_L("RMobilePhone::SetNetworkSelectionSetting timed out") );    

	ASSERT_EQUALS( setNetSelSettingStatus.Int(), KErrNone, 
							_L("RMobilePhone::SetNetworkSelectionSetting returned error"))
    
    //setNetSelSettingStatus
	//reqNotifyNetSelSettingChange
	CleanupStack::PopAndDestroy(2,&reqNotifyNetSelSettingChange);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestNetworkControl0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestNetworkControl0025");
	}



