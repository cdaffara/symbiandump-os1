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
// The TEFUnit test suite for NetworkControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsynetworkcontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "mmretrieve.h"
#include "cctsyactiveretriever.h"
#include "utils.h"

const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)

CTestSuite* CCTsyNetworkControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStartV20001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStartV20002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStartV20004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStartV20005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStart0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNITZInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNITZInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSelectNetwork0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSelectNetwork0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSelectNetwork0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSelectNetwork0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetwork0004bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyModeChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyModeChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyModeChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetwork0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetwork0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetwork0002L)
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetwork0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetwork0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetwork0004bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNITZInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNITZInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNITZInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetworkName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetworkName0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCurrentNetworkName0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0002bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetServiceProviderName0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkInvScanSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkInvScanEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetHomeNetworkSearchPeriod0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkSelectionSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkSelectionSetting0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0002bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCurrentNetworkChange0004bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkInvScanChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSetNetworkSelectionSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSetNetworkSelectionSetting0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSetNetworkSelectionSetting0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSetNetworkSelectionSetting0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestSetNetworkSelectionSetting0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetLastUsedAccessTechnology0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStorePreferredNetworksListL0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkRegistrationStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkRegistrationStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkRegistrationStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetNetworkRegistrationStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyPreferredNetworksListChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkSelectionSettingChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkSelectionSettingChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkSelectionSettingChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkRegistrationStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkRegistrationStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyNetworkRegistrationStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestStartV50001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCellInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCellInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCellInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestGetCellInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCellInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCellInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCellInfoChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyNetworkControlFU, TestNotifyCellInfoChange0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-NTWC-RMPDNS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneDetectedNetworks::StartV2
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneDetectedNetworks::StartV2
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStartV20001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
		
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNotSupported);

	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 	
 	// --- prepare data for CompleteL ---
 	CMobilePhoneNetworkListV2* networkListV2 = CMobilePhoneNetworkListV2::NewL();
	CleanupStack::PushL(networkListV2);
	TMockLtsyData1<CMobilePhoneNetworkListV2*> listData(networkListV2);
	
 	RMobilePhone::TMobilePhoneNetworkInfoV2 infoV2;
	
	infoV2.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	infoV2.iMode        = RMobilePhone::ENetworkModeUnknown;
	infoV2.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	infoV2.iBandInfo    = RMobilePhone::EBandUnknown;	
	
	_LIT(KCountryCode, "1234");
	infoV2.iCountryCode = KCountryCode;

	_LIT(KCdmaSID, "12345678");
	infoV2.iCdmaSID     = KCdmaSID;

	_LIT(KAnalogSID, "12345678");
	infoV2.iAnalogSID   = KAnalogSID;

	_LIT(KNetworkId, "12345678");
	infoV2.iNetworkId   = KNetworkId;
	
	_LIT(KDisplayTag, "1234567890");
	infoV2.iDisplayTag  = KDisplayTag;
	
	_LIT(KShortName, "1234567890");
	infoV2.iShortName   = KShortName;

	_LIT(KLongName, "12345678901234567890");
	infoV2.iLongName    = KLongName;
	
	networkListV2->AddEntryL(infoV2);
	listData.SerialiseL(data); 
 	// --- -------------------------- ---

    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrGeneral, data);

	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrGeneral, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneDetectedNetworks::StartV2 when result is not cached.
 	//-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone, data);

	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();
	
	CMobilePhoneNetworkListV2* retrieveList = NULL;
	retrieveList = retrieveMobilePhoneDetectedNetworks->RetrieveListV2L();
	
	if ( retrieveList )
		{
		TInt index(0);
		RMobilePhone::TMobilePhoneNetworkInfoV2 retrieveInfo;
		
		CleanupStack::PushL(retrieveList);
		retrieveInfo = retrieveList->GetEntryL(index);

		ASSERT_EQUALS( RMobilePhone::ENetworkAccessUnknown, retrieveInfo.iAccess   );
		ASSERT_EQUALS( RMobilePhone::ENetworkModeUnknown  , retrieveInfo.iMode     );
		ASSERT_EQUALS( RMobilePhone::ENetworkStatusUnknown, retrieveInfo.iStatus   );
		ASSERT_EQUALS( RMobilePhone::EBandUnknown         , retrieveInfo.iBandInfo );
		
		ASSERT_EQUALS( 0, retrieveInfo.iCountryCode.Compare( KCountryCode ) );
		ASSERT_EQUALS( 0, retrieveInfo.iCdmaSID    .Compare( KCdmaSID     ) );
		ASSERT_EQUALS( 0, retrieveInfo.iAnalogSID  .Compare( KAnalogSID   ) );
		ASSERT_EQUALS( 0, retrieveInfo.iNetworkId  .Compare( KNetworkId   ) );
		ASSERT_EQUALS( 0, retrieveInfo.iDisplayTag .Compare( KDisplayTag  ) );
		ASSERT_EQUALS( 0, retrieveInfo.iShortName  .Compare( KShortName   ) );
		ASSERT_EQUALS( 0, retrieveInfo.iLongName   .Compare( KLongName    ) );
		
		CleanupStack::PopAndDestroy(1); // retrieveList
		}

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneDetectedNetworks::StartV2
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);	
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone, data);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // retrieveMobilePhoneDetectedNetworks, 
	                                // activeAuthorizationInfoRetriever,
	                                // networkListV2
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-RMPDNS-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrieveMobilePhoneDetectedNetworks::StartV2
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrieveMobilePhoneDetectedNetworks::StartV2
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStartV20002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	// --- prepare data for CompleteL ---
	CMobilePhoneNetworkListV2* networkListV2 = CMobilePhoneNetworkListV2::NewL();
	CleanupStack::PushL(networkListV2);
	TMockLtsyData1<CMobilePhoneNetworkListV2*> listData(networkListV2);
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 infoV2;
	
	infoV2.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	infoV2.iMode        = RMobilePhone::ENetworkModeUnknown;
	infoV2.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	infoV2.iBandInfo    = RMobilePhone::EBandUnknown;
	
	_LIT(KCountryCode, "1234");
	infoV2.iCountryCode = KCountryCode;

	_LIT(KCdmaSID, "12345678");
	infoV2.iCdmaSID     = KCdmaSID;

	_LIT(KAnalogSID, "12345678");
	infoV2.iAnalogSID   = KAnalogSID;

	_LIT(KNetworkId, "12345678");
	infoV2.iNetworkId   = KNetworkId;
	
	_LIT(KDisplayTag, "1234567890");
	infoV2.iDisplayTag  = KDisplayTag;
	
	_LIT(KShortName, "1234567890");
	infoV2.iShortName   = KShortName;

	_LIT(KLongName, "12345678901234567890");
	infoV2.iLongName    = KLongName;
	
	networkListV2->AddEntryL(infoV2);

    listData.SerialiseL(data);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request of
	// CRetrieveMobilePhoneDetectedNetworks::Cancel from LTSY->CTSY
 	//-------------------------------------------------------------------------
    
	iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksCancel, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksCancel, KErrGeneral);
    
	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	// status matchs KErrCancel because requests cancels in etel
	// before completion of cancel comes to CTSY
	// but completion of request in CTSY is awaited by etel
	// to release execution of the thread calling the Cancel
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrieveMobilePhoneDetectedNetworks::Cancel.
 	//-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksCancel, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksCancel, KErrNone);
    
	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: Successful completion request of
	// CRetrieveMobilePhoneDetectedNetworks::Cancel on 2nd phase of
	// CRetrieveMobilePhoneDetectedNetworks::StartV2.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone, data);

	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of CRetrieveMobilePhoneDetectedNetworks::Cancel()
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksCancel, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request of
	// CRetrieveMobilePhoneDetectedNetworks::Cancel to LTSY
 	//-------------------------------------------------------------------------
    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksCancel, KErrNotSupported);
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone, data);
    
	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());
	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // retrieveMobilePhoneDetectedNetworks, 
	                                // activeRetriever,
	                                // networkListV2
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-RMPDNS-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to CRetrieveMobilePhoneDetectedNetworks::StartV2
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to CRetrieveMobilePhoneDetectedNetworks::StartV2
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStartV20004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CMobilePhoneNetworkListV2* networkListV2 = CMobilePhoneNetworkListV2::NewL();
	CleanupStack::PushL(networkListV2);
	TMockLtsyData1<CMobilePhoneNetworkListV2*> listData(networkListV2);
	
	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks1 = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks1);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever1 = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks1);
	CleanupStack::PushL(activeRetriever1);
	scheduler.AddRetrieverL(*activeRetriever1);

	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks2 = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(phone2);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks2);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever2 = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks2);
	CleanupStack::PushL(activeRetriever2);
	scheduler.AddRetrieverL(*activeRetriever2);

	//-------------------------------------------------------------------------
	// Initialization infoV2
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 infoV2;
	
	infoV2.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	infoV2.iMode        = RMobilePhone::ENetworkModeUnknown;
	infoV2.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	infoV2.iBandInfo    = RMobilePhone::EBandUnknown;
	
	_LIT(KCountryCode, "1234");
	infoV2.iCountryCode = KCountryCode;

	_LIT(KCdmaSID, "12345678");
	infoV2.iCdmaSID     = KCdmaSID;

	_LIT(KAnalogSID, "12345678");
	infoV2.iAnalogSID   = KAnalogSID;

	_LIT(KNetworkId, "12345678");
	infoV2.iNetworkId   = KNetworkId;
	
	_LIT(KDisplayTag, "1234567890");
	infoV2.iDisplayTag  = KDisplayTag;
	
	_LIT(KShortName, "1234567890");
	infoV2.iShortName   = KShortName;

	_LIT(KLongName, "12345678901234567890");
	infoV2.iLongName    = KLongName;
	
	networkListV2->AddEntryL(infoV2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting CRetrieveMobilePhoneDetectedNetworks::StartV2
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);
    listData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone, data);

	retrieveMobilePhoneDetectedNetworks1->StartV2(activeRetriever1->Status());
	activeRetriever1->Activate();

	retrieveMobilePhoneDetectedNetworks2->StartV2(activeRetriever2->Status());
	activeRetriever2->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever1->iStatus.Int());
	ASSERT_EQUALS(KErrServerBusy, activeRetriever2->iStatus.Int());

	AssertMockLtsyStatusL();
 	
	CleanupStack::PopAndDestroy(5); // retrieveMobilePhoneDetectedNetworks1, 
	                                // retrieveMobilePhoneDetectedNetworks2,  
	                                // activeRetriever1,
	                                // activeRetriever2,
	                                // networkListV2
	CleanupStack::PopAndDestroy(2); // telServer2, phone2
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-RMPDNS-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneDetectedNetworks::StartV2 with timeout
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneDetectedNetworks::StartV2 and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStartV20005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of CRetrieveMobilePhoneDetectedNetworks::StartV2
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetDetectedNetworksV2Phase1, KErrNone);

	retrieveMobilePhoneDetectedNetworks->StartV2(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrTimedOut, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // retrieveMobilePhoneDetectedNetworks, activeRetriever
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(1); // this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-EMPDNS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneDetectedNetworks::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneDetectedNetworks::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	retrieveMobilePhoneDetectedNetworks->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // retrieveMobilePhoneDetectedNetworks, 
	                                // activeAuthorizationInfoRetriever
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-RMPDNS5-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrieveMobilePhoneDetectedNetworks::StartV5
@SYMTestPriority High
@SYMTestActions Invokes CRetrieveMobilePhoneDetectedNetworks::StartV5
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStartV50001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

	CRetrieveMobilePhoneDetectedNetworks* retrieveMobilePhoneDetectedNetworks = 
	                         CRetrieveMobilePhoneDetectedNetworks::NewL(iPhone);
	CleanupStack::PushL(retrieveMobilePhoneDetectedNetworks);
	
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = 
						CActiveRetriever::NewL(*retrieveMobilePhoneDetectedNetworks);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	retrieveMobilePhoneDetectedNetworks->StartV5(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNotSupported, activeRetriever->iStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // retrieveMobilePhoneDetectedNetworks, 
	                                // activeAuthorizationInfoRetriever
	CActiveScheduler::Install(NULL);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGNI-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNITZInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNITZInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNITZInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TInt res = KErrNone;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetNITZInfo when result is not cached.
 	//-------------------------------------------------------------------------
	
	RMobilePhone::TMobilePhoneNITZ NITZInfoReceive;
	res = iPhone.GetNITZInfo(NITZInfoReceive);
	ASSERT_EQUALS(KErrNotFound, res);

	TInt32	                                   nitzFieldsUsed = 0;
	TInt                                       timeZone       = 10;
	TInt	                                   DST            = 5;
	RMobilePhone::TMobilePhoneNetworkShortName shortNetworkId = _L("");
	RMobilePhone::TMobilePhoneNetworkLongName  longNetworkId  = _L("");

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	// change NITZ info
	RMobilePhone::TMobilePhoneNITZ NITZInfo(2007, EOctober, 18, 16,  0,  0, 0);
	RMobilePhone::TMobilePhoneNetworkShortName mobilePhoneNetworkShortName = _L("ShortName");
	NITZInfo.iShortNetworkId = mobilePhoneNetworkShortName;
	RMobilePhone::TMobilePhoneNetworkLongName mobilePhoneNetworkLongName = _L("LongName");
	NITZInfo.iLongNetworkId = mobilePhoneNetworkLongName;
	nitzFieldsUsed = 15;
	NITZInfo.iNitzFieldsUsed = nitzFieldsUsed;
	NITZInfo.iTimeZone = timeZone;
	NITZInfo.iDST = DST;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNITZ> NITZInfoData(NITZInfo);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	NITZInfoData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyNITZInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	res = iPhone.GetNITZInfo(NITZInfoReceive);
	ASSERT_EQUALS(KErrNone, res);

	ASSERT_EQUALS(nitzFieldsUsed             , NITZInfoReceive.iNitzFieldsUsed);
	ASSERT_EQUALS(mobilePhoneNetworkShortName, NITZInfoReceive.iShortNetworkId);
	ASSERT_EQUALS(mobilePhoneNetworkLongName , NITZInfoReceive.iLongNetworkId );
	ASSERT_EQUALS(timeZone                   , NITZInfoReceive.iTimeZone      );
	ASSERT_EQUALS(DST                        , NITZInfoReceive.iDST           );
			
 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	// change CurrentNITZ info
    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
    networkInfo.iMode = RMobilePhone::ENetworkModeTdcdma;
    RMobilePhone::TMobilePhoneNetworkShortName mobilePhoneNetworkCurrentShortName = _L("CShortName");
    networkInfo.iShortName = mobilePhoneNetworkCurrentShortName;
    RMobilePhone::TMobilePhoneNetworkLongName mobilePhoneNetworkCurrentLongName = _L("CLongName");
    networkInfo.iLongName = mobilePhoneNetworkCurrentLongName;
	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
	               RMobilePhone::TMobilePhoneLocationAreaV1> currentNITZInfoData(networkInfo, locationArea);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	data.Close();
	currentNITZInfoData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	res = iPhone.GetNITZInfo(NITZInfoReceive);
	ASSERT_EQUALS(KErrNone, res);

	ASSERT_EQUALS(mobilePhoneNetworkCurrentShortName, NITZInfoReceive.iShortNetworkId);
	ASSERT_EQUALS(mobilePhoneNetworkCurrentLongName , NITZInfoReceive.iLongNetworkId );

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
    
    RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfo2; 
	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV2, 
	               RMobilePhone::TMobilePhoneLocationAreaV1> currentNITZInfoData2(networkInfo2, locationArea);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	data.Close();
	currentNITZInfoData2.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

    networkInfo.iMode = RMobilePhone::ENetworkModeWcdma;
	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
	               RMobilePhone::TMobilePhoneLocationAreaV1> mockData2(networkInfo, locationArea);

	data.Close();
	mockData2.SerialiseL(data); 

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNotSupported, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGNI-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNITZInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNITZInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNITZInfo0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt res = KErrNone;
	
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::GetNITZInfo
 	//-------------------------------------------------------------------------

	// Note that this does not actually test anything, as KErrNotFound will be returned
	// in any case (see previous test case)
	// This test case is redundant 
	TDateTime dateTime;
	res = iPhone.GetNITZInfo(static_cast<RMobilePhone::TMobilePhoneNITZ&>(dateTime));
	ASSERT_EQUALS(KErrNotFound, res);

	CleanupStack::PopAndDestroy(2, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MSN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SelectNetwork
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SelectNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSelectNetwork0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

	TBool isManual = EFalse;
	RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection;
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("7");
	manualSelection.iCountry = countryCode;
	RMobilePhone::TMobilePhoneNetworkIdentity networkIdentity = _L("12345");
	manualSelection.iNetwork = networkIdentity;

	TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
		mockData2(isManual, manualSelection);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data, KErrNotSupported);

	iPhone.SelectNetwork(requestStatus, isManual, manualSelection);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneSelectNetwork, KErrGeneral);

	iPhone.SelectNetwork(requestStatus, isManual, manualSelection);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SelectNetwork when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneSelectNetwork, KErrNone);

	iPhone.SelectNetwork(requestStatus, isManual, manualSelection);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C2: for isManual = ETrue;
 	//-------------------------------------------------------------------------

	isManual = ETrue;
	TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
		mockData22(isManual, manualSelection);

	data.Close();
	mockData22.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneSelectNetwork, KErrNone);

	iPhone.SelectNetwork(requestStatus, isManual, manualSelection);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SelectNetwork
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);	
    iMockLTSY.CompleteL(EMobilePhoneSelectNetwork, KErrNone);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSN-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SelectNetwork
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SelectNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSelectNetwork0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	TBool isManual = EFalse;
	RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection;
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("7");
	manualSelection.iCountry = countryCode;
	RMobilePhone::TMobilePhoneNetworkIdentity networkIdentity = _L("12345");
	manualSelection.iNetwork = networkIdentity;

	TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
		mockData2(isManual, manualSelection);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SelectNetwork
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data, KErrNone);

	iPhone.SelectNetwork(requestStatus, isManual, manualSelection);	

    iMockLTSY.ExpectL(EMobilePhoneSelectNetworkCancel, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneSelectNetworkCancel, KErrNone);

    iPhone.CancelAsyncRequest(EMobilePhoneSelectNetwork);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SelectNetwork
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SelectNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSelectNetwork0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TBool isManual1 = EFalse;
	RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection1;
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode1 = _L("7");
	manualSelection1.iCountry = countryCode1;
	RMobilePhone::TMobilePhoneNetworkIdentity networkIdentity1 = _L("12345");
	manualSelection1.iNetwork = networkIdentity1;

	TBool isManual2 = ETrue;
	RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection2;
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode2 = _L("8");
	manualSelection2.iCountry = countryCode2;
	RMobilePhone::TMobilePhoneNetworkIdentity networkIdentity2 = _L("54321");
	manualSelection2.iNetwork = networkIdentity2;

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
		mockData21(isManual1, manualSelection1);

	TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
		mockData22(isManual2, manualSelection2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SelectNetwork
 	//-------------------------------------------------------------------------

	mockData21.SerialiseL(data1);
    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data1, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneSelectNetwork, KErrNone);
	iPhone.SelectNetwork(requestStatus1, isManual1, manualSelection1);

	mockData22.SerialiseL(data2);
    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data2, KErrNone);
    iMockLTSY.CompleteL(EMobilePhoneSelectNetwork, KErrNone);
	phone2.SelectNetwork(requestStatus2, isManual2, manualSelection2);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data1, data2, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSN-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SelectNetwork with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SelectNetwork and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSelectNetwork0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

	TBool isManual = EFalse;
	RMobilePhone::TMobilePhoneNetworkManualSelection manualSelection;
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("7");
	manualSelection.iCountry = countryCode;
	RMobilePhone::TMobilePhoneNetworkIdentity networkIdentity = _L("12345");
	manualSelection.iNetwork = networkIdentity;

	TMockLtsyData2<TBool, RMobilePhone::TMobilePhoneNetworkManualSelection> 
		mockData2(isManual, manualSelection);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SelectNetwork
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSelectNetwork, data, KErrNone);
	iPhone.SelectNetwork(requestStatus, isManual, manualSelection);	

	/*
	 * Test for timeout of RMobilePhone::SelectNetwork fails. 
	 */
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 260402);
	ASSERT_TRUE(EFalse);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGCM-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentMode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCurrentMode when result is not cached.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkMode networkMode;
	TInt result = iPhone.GetCurrentMode(networkMode);
	ASSERT_EQUALS(KErrNone, result);
	
	ASSERT_EQUALS(RMobilePhone::ENetworkModeUnknown, networkMode);

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetCurrentMode again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;

	networkMode = RMobilePhone::ENetworkModeAmps;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkMode> networkModeData(networkMode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkModeData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyModeChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	result = iPhone.GetCurrentMode(networkMode);
	ASSERT_EQUALS(KErrNone, result);
	
	ASSERT_EQUALS(RMobilePhone::ENetworkModeAmps, networkMode);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetHomeNetwork
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetHomeNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
			
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetHomeNetwork when result is not cached.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoV1Pckg = networkInfoV1;

	iPhone.GetHomeNetwork(requestStatus, networkInfoV1Pckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    // default values to compare with variables returned from GetHomeNetwork().
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("234");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("23499");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian mobile");
        
    ASSERT_EQUALS(RMobilePhone::ENetworkModeWcdma    , networkInfoV1.iMode       );
    ASSERT_EQUALS(RMobilePhone::ENetworkStatusCurrent, networkInfoV1.iStatus     );
    ASSERT_EQUALS(RMobilePhone::EBandUnknown         , networkInfoV1.iBandInfo   );
    ASSERT_EQUALS(countryCode                        , networkInfoV1.iCountryCode);
    ASSERT_EQUALS(cdmaSID                            , networkInfoV1.iCdmaSID    );
    ASSERT_EQUALS(analogSID                          , networkInfoV1.iAnalogSID  );
    ASSERT_EQUALS(networkId                          , networkInfoV1.iNetworkId  );
    ASSERT_EQUALS(displayTag                         , networkInfoV1.iDisplayTag );
    ASSERT_EQUALS(shortName                          , networkInfoV1.iShortName  );
    ASSERT_EQUALS(longName                           , networkInfoV1.iLongName   );

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfoV2;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV2 > networkInfoV2Pckg = networkInfoV2;

	iPhone.GetHomeNetwork(requestStatus, networkInfoV2Pckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(RMobilePhone::ENetworkAccessUtran, networkInfoV2.iAccess);

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	iPhone.GetHomeNetwork(requestStatus, networkInfoV5Pckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(1, networkInfoV5.iHsdpaAvailableIndicator);
    ASSERT_EQUALS(1, networkInfoV5.iEgprsAvailableIndicator);

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetHomeNetwork again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	
    /*
     * Expects to receive this data after call GetHomeNetwork()
     */
    countryCode = _L("567");
    cdmaSID     = _L("890");
    analogSID   = _L("123");
    networkId   = _L("56700");
    displayTag  = _L("symbian2");
    shortName   = _L("symbian2");
    longName    = _L("symbian2 mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1_input;
	networkInfoV1_input.iMode        = RMobilePhone::ENetworkModeCdma2000   ;
	networkInfoV1_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV1_input.iBandInfo    = RMobilePhone::E1900BandA             ;
	networkInfoV1_input.iCountryCode = countryCode;
	networkInfoV1_input.iCdmaSID     = cdmaSID    ;
	networkInfoV1_input.iAnalogSID   = analogSID  ;
	networkInfoV1_input.iNetworkId   = networkId  ;
	networkInfoV1_input.iDisplayTag  = displayTag ;
	networkInfoV1_input.iShortName   = shortName  ;
	networkInfoV1_input.iLongName    = longName   ;
	
	TMockLtsyData1< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoDataV1(networkInfoV1_input);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoDataV1.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1_output;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoV1Pckg_output = networkInfoV1_output;

	iPhone.GetHomeNetwork(requestStatus, networkInfoV1Pckg_output);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(networkInfoV1_input.iMode       , networkInfoV1_output.iMode       );
    ASSERT_EQUALS(networkInfoV1_input.iStatus     , networkInfoV1_output.iStatus     );
    ASSERT_EQUALS(networkInfoV1_input.iBandInfo   , networkInfoV1_output.iBandInfo   );
    ASSERT_EQUALS(networkInfoV1_input.iCountryCode, networkInfoV1_output.iCountryCode);
    ASSERT_EQUALS(networkInfoV1_input.iCdmaSID    , networkInfoV1_output.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV1_input.iAnalogSID  , networkInfoV1_output.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV1_input.iNetworkId  , networkInfoV1_output.iNetworkId  );
    ASSERT_EQUALS(networkInfoV1_input.iDisplayTag , networkInfoV1_output.iDisplayTag );
    ASSERT_EQUALS(networkInfoV1_input.iShortName  , networkInfoV1_output.iShortName  );
    ASSERT_EQUALS(networkInfoV1_input.iLongName   , networkInfoV1_output.iLongName   );

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0001a
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetHomeNetwork
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetHomeNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0001aL()
	{
	TRequestStatus requestStatus;

	// Open new client
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RMobilePhone phone;
	TInt ret = phone.Open(iTelServer, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone);
	
	ret = iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	
 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	phone.GetHomeNetwork(requestStatus, networkInfoV5Pckg);
	
    User::WaitForRequest(requestStatus);
 	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotFound, requestStatus.Int());
 
 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoV1Pckg = networkInfoV1;

	phone.GetHomeNetwork(requestStatus, networkInfoV1Pckg);
	
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfoV2;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV2 > networkInfoV2Pckg = networkInfoV2;

	phone.GetHomeNetwork(requestStatus, networkInfoV2Pckg);
	
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // phone, this
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetHomeNetwork (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetHomeNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
			
 	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RMobilePhone::GetHomeNetwork from the CTSY cache.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoV8Pckg = networkInfoV8;

	iPhone.GetHomeNetwork(requestStatus, networkInfoV8Pckg);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    // default values to compare with variables returned from GetHomeNetwork().
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("234");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("23499");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian mobile");
        
    ASSERT_EQUALS(RMobilePhone::ENetworkModeWcdma    , networkInfoV8.iMode       );
    ASSERT_EQUALS(RMobilePhone::ENetworkStatusCurrent, networkInfoV8.iStatus     );
    ASSERT_EQUALS(RMobilePhone::EBandUnknown         , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(countryCode                        , networkInfoV8.iCountryCode);
    ASSERT_EQUALS(cdmaSID                            , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(analogSID                          , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkId                          , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(displayTag                         , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(shortName                          , networkInfoV8.iShortName  );
    ASSERT_EQUALS(longName                           , networkInfoV8.iLongName   );
    ASSERT_EQUALS(RMobilePhone::ENetworkAccessUtran	 , networkInfoV8.iAccess     );
    ASSERT_TRUE(networkInfoV8.iHsdpaAvailableIndicator );
    ASSERT_TRUE(networkInfoV8.iEgprsAvailableIndicator );
    ASSERT_TRUE(networkInfoV8.iHsupaAvailableIndicator );
    
 	//-------------------------------------------------------------------------
	// TEST B: RMobilePhone::GetHomeNetwork again from the cache
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	
    countryCode = _L("567");
    cdmaSID     = _L("890");
    analogSID   = _L("123");
    networkId   = _L("56700");
    displayTag  = _L("symbian2");
    shortName   = _L("symbian2");
    longName    = _L("symbian2 mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8_input;
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeCdma2000;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV8_input.iBandInfo    = RMobilePhone::E1900BandA;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessGsm;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;
	
	TMockLtsyData1< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoDataV8(networkInfoV8_input);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoDataV8.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetHomeNetwork(requestStatus, networkInfoV8Pckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling RMobilePhone::GetHomeNetwork (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetHomeNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);
                   
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV8> networkInfoPckgV8(networkInfoV8);
    
    //-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetHomeNetwork
 	//-------------------------------------------------------------------------
    
    iPhone.GetHomeNetwork(requestStatus, networkInfoPckgV8);
    iPhone.CancelAsyncRequest(EMobilePhoneGetHomeNetwork);
    User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	//requestStatus should be KErrNone if CTSY completed GetHomeNetwork() 
	//request before cancel request arrived. requestStatus should be KErrCancel 
	//if CTSY received cancel request before before GetHomeNetwork() request
	//was completed.
	ASSERT_TRUE( ( requestStatus.Int() == KErrNone || requestStatus.Int() == KErrCancel ) );
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetHomeNetwork with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetHomeNetwork with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RMobilePhone::GetHomeNetwork
 	//-------------------------------------------------------------------------
 	
	TInt extensionId = 1;
	TPckg<TInt> networkInfoPckg = extensionId;

	iPhone.GetHomeNetwork(requestStatus, networkInfoPckg);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());
    
    AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetHomeNetwork
 	//-------------------------------------------------------------------------

	TBuf8<1> smallDes;

	iPhone.GetHomeNetwork(requestStatus, smallDes);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetHomeNetwork
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetHomeNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetHomeNetwork
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfoV2;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV2 > networkInfoV2Pckg = networkInfoV2;

	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	iPhone.GetHomeNetwork(requestStatus1, networkInfoV2Pckg);

	phone2.GetHomeNetwork(requestStatus2, networkInfoV5Pckg);
	
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    ASSERT_EQUALS(RMobilePhone::ENetworkAccessUtran, networkInfoV2.iAccess);

    ASSERT_EQUALS(1, networkInfoV5.iHsdpaAvailableIndicator);
    ASSERT_EQUALS(1, networkInfoV5.iEgprsAvailableIndicator);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGHN-0004b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetHomeNetwork (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetHomeNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetwork0004bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test multiple client requesting RMobilePhone::GetHomeNetwork
 	//-------------------------------------------------------------------------
	
	// default values to compare with variables returned from GetHomeNetwork().
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("234");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("23499");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian mobile");
    
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoV8Pckg = networkInfoV8;

	iPhone.GetHomeNetwork(requestStatus1, networkInfoV5Pckg);
	phone2.GetHomeNetwork(requestStatus2, networkInfoV8Pckg);
	
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    ASSERT_EQUALS(RMobilePhone::ENetworkModeWcdma     , networkInfoV5.iMode       );
    ASSERT_EQUALS(RMobilePhone::ENetworkStatusCurrent , networkInfoV5.iStatus     );
    ASSERT_EQUALS(RMobilePhone::EBandUnknown          , networkInfoV5.iBandInfo   );
    ASSERT_EQUALS(countryCode                         , networkInfoV5.iCountryCode);
    ASSERT_EQUALS(cdmaSID                             , networkInfoV5.iCdmaSID    );
    ASSERT_EQUALS(analogSID                           , networkInfoV5.iAnalogSID  );
    ASSERT_EQUALS(networkId                           , networkInfoV5.iNetworkId  );
    ASSERT_EQUALS(displayTag                          , networkInfoV5.iDisplayTag );
    ASSERT_EQUALS(shortName                           , networkInfoV5.iShortName  );
    ASSERT_EQUALS(longName                            , networkInfoV5.iLongName   );
    ASSERT_EQUALS(RMobilePhone::ENetworkAccessUtran   , networkInfoV5.iAccess     );
    ASSERT_TRUE(networkInfoV5.iHsdpaAvailableIndicator);
    ASSERT_TRUE(networkInfoV5.iEgprsAvailableIndicator);
	
	ASSERT_EQUALS(RMobilePhone::ENetworkModeWcdma     , networkInfoV8.iMode       );
    ASSERT_EQUALS(RMobilePhone::ENetworkStatusCurrent , networkInfoV8.iStatus     );
    ASSERT_EQUALS(RMobilePhone::EBandUnknown          , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(countryCode                         , networkInfoV8.iCountryCode);
    ASSERT_EQUALS(cdmaSID                             , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(analogSID                           , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkId                           , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(displayTag                          , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(shortName                           , networkInfoV8.iShortName  );
    ASSERT_EQUALS(longName                            , networkInfoV8.iLongName   );
    ASSERT_EQUALS(RMobilePhone::ENetworkAccessUtran   , networkInfoV8.iAccess     );
    ASSERT_TRUE(networkInfoV8.iHsdpaAvailableIndicator);
    ASSERT_TRUE(networkInfoV8.iEgprsAvailableIndicator);
    ASSERT_TRUE(networkInfoV8.iHsupaAvailableIndicator);
    
    AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-NTWC-MGNC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetNetworkCaps when result is not cached.
 	//-------------------------------------------------------------------------

	// hard coded value
	TUint32 networkCaps;
	iPhone.GetNetworkCaps(networkCaps);

	ASSERT_EQUALS((TUint32) RMobilePhone::KCapsGetRegistrationStatus
						  | RMobilePhone::KCapsNotifyRegistrationStatus
						  | RMobilePhone::KCapsGetCurrentMode
						  | RMobilePhone::KCapsNotifyMode
						  | RMobilePhone::KCapsGetCurrentNetwork
						  | RMobilePhone::KCapsNotifyCurrentNetwork
						  | RMobilePhone::KCapsGetHomeNetwork
						  | RMobilePhone::KCapsGetDetectedNetworks
						  | RMobilePhone::KCapsManualNetworkSelection
						  | RMobilePhone::KCapsNotifyNITZInfo
						  | RMobilePhone::KCapsGetNITZInfo              , networkCaps);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNMC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyModeChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyModeChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus; 
	TRequestStatus mockLtsyStatus;
	RMobilePhone::TMobilePhoneNetworkMode networkMode;
	RMobilePhone::TMobilePhoneNetworkMode networkModeComplete = RMobilePhone::ENetworkModeUnregistered;
	TMockLtsyData1< RMobilePhone::TMobilePhoneNetworkMode > networkModeData1(networkModeComplete);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyModeChange when result is not cached.
 	//-------------------------------------------------------------------------

	iPhone.NotifyModeChange(requestStatus, networkMode);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkModeData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyModeChange, KErrNone, data);
	
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(networkMode, networkModeComplete);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyModeChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	data.Close();
	networkModeData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyModeChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNMC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyModeChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyModeChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus; 
	TRequestStatus mockLtsyStatus;
	RMobilePhone::TMobilePhoneNetworkMode networkMode;
	RMobilePhone::TMobilePhoneNetworkMode networkModeComplete = RMobilePhone::ENetworkModeUnregistered;
	TMockLtsyData1< RMobilePhone::TMobilePhoneNetworkMode > networkModeData1(networkModeComplete);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyModeChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyModeChange(requestStatus, networkMode);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyModeChange);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	networkModeData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyModeChange, KErrNone, data, 10);

	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
 	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNMC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyModeChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyModeChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1; 
	TRequestStatus requestStatus2; 
	TRequestStatus mockLtsyStatus; 
	RMobilePhone::TMobilePhoneNetworkMode networkMode1;
	RMobilePhone::TMobilePhoneNetworkMode networkMode2;
	RMobilePhone::TMobilePhoneNetworkMode networkModeComplete = RMobilePhone::ENetworkModeUnregistered;
	TMockLtsyData1< RMobilePhone::TMobilePhoneNetworkMode > networkModeData1(networkModeComplete);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyModeChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyModeChange(requestStatus1, networkMode1);
	phone2.NotifyModeChange(requestStatus2, networkMode2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	networkModeData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyModeChange, KErrNone, data);

    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_EQUALS(networkMode1, networkModeComplete);
    ASSERT_EQUALS(networkMode2, networkModeComplete);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
/*
@SYMTestCaseID BA-CTSY-NTWC-MGCN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentNetwork
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetwork0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCurrentNetwork when result is not cached.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoV1Pckg = networkInfoV1;

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV1Pckg, area);
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("");
        
    ASSERT_EQUALS( RMobilePhone::ENetworkModeUnknown  , networkInfoV1.iMode       );
    ASSERT_EQUALS( RMobilePhone::ENetworkStatusUnknown, networkInfoV1.iStatus     );
    ASSERT_EQUALS( RMobilePhone::EBandUnknown         , networkInfoV1.iBandInfo   );
    ASSERT_EQUALS( countryCode                        , networkInfoV1.iCountryCode);
    ASSERT_EQUALS( cdmaSID                            , networkInfoV1.iCdmaSID    );
    ASSERT_EQUALS( analogSID                          , networkInfoV1.iAnalogSID  );
    ASSERT_EQUALS( networkId                          , networkInfoV1.iNetworkId  );
    ASSERT_EQUALS( displayTag                         , networkInfoV1.iDisplayTag );
    ASSERT_EQUALS( shortName                          , networkInfoV1.iShortName  );
    ASSERT_EQUALS( longName                           , networkInfoV1.iLongName   );

	TBool areaKnown        = EFalse;
	TUint locationAreaCode = 0;
	TUint cellId           = 0;

    ASSERT_EQUALS(areaKnown       , area.iAreaKnown       );
    ASSERT_EQUALS(locationAreaCode, area.iLocationAreaCode);
    ASSERT_EQUALS(cellId          , area.iCellId          );
	
 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfoV2;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV2 > networkInfoV2Pckg = networkInfoV2;

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV2Pckg, area);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(RMobilePhone::ENetworkAccessUnknown, networkInfoV2.iAccess);

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------
	
	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV5Pckg, area);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(0, networkInfoV5.iHsdpaAvailableIndicator);
    ASSERT_EQUALS(0, networkInfoV5.iEgprsAvailableIndicator);
	
 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetCurrentNetwork again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneLocationAreaV1 area2;
	
	areaKnown        = ETrue;
	locationAreaCode = 10;
	cellId           = 100;
	
	area2.iAreaKnown        = areaKnown;
	area2.iLocationAreaCode = locationAreaCode;
	area2.iCellId           = cellId;
	
    countryCode = _L("567");
    cdmaSID     = _L("890");
    analogSID   = _L("123");
    networkId   = _L("56700");
    displayTag  = _L("symbian2");
    shortName   = _L("symbian2");
    longName    = _L("symbian2 mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1Ptr;
	networkInfoV1Ptr.iMode        = RMobilePhone::ENetworkModeCdma2000   ;
	networkInfoV1Ptr.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV1Ptr.iBandInfo    = RMobilePhone::E1900BandA             ;
	networkInfoV1Ptr.iCountryCode = countryCode;
	networkInfoV1Ptr.iCdmaSID     = cdmaSID    ;
	networkInfoV1Ptr.iAnalogSID   = analogSID  ;
	networkInfoV1Ptr.iNetworkId   = networkId  ;
	networkInfoV1Ptr.iDisplayTag  = displayTag ;
	networkInfoV1Ptr.iShortName   = shortName  ;
	networkInfoV1Ptr.iLongName    = longName   ;

	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV1, 
	               RMobilePhone::TMobilePhoneLocationAreaV1> currentNetworkData2(networkInfoV1Ptr, area2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	currentNetworkData2.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV1Pckg, area);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(networkInfoV1Ptr.iMode       , networkInfoV1.iMode       );
    ASSERT_EQUALS(networkInfoV1Ptr.iStatus     , networkInfoV1.iStatus     );
    ASSERT_EQUALS(networkInfoV1Ptr.iBandInfo   , networkInfoV1.iBandInfo   );
    ASSERT_EQUALS(networkInfoV1Ptr.iCountryCode, networkInfoV1.iCountryCode);
    ASSERT_EQUALS(networkInfoV1Ptr.iCdmaSID    , networkInfoV1.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV1Ptr.iAnalogSID  , networkInfoV1.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV1Ptr.iNetworkId  , networkInfoV1.iNetworkId  );
    ASSERT_EQUALS(networkInfoV1Ptr.iDisplayTag , networkInfoV1.iDisplayTag );
    ASSERT_EQUALS(networkInfoV1Ptr.iShortName  , networkInfoV1.iShortName  );
    ASSERT_EQUALS(networkInfoV1Ptr.iLongName   , networkInfoV1.iLongName   );

    ASSERT_EQUALS(area.iAreaKnown       , area2.iAreaKnown       );
    ASSERT_EQUALS(area.iLocationAreaCode, area2.iLocationAreaCode);
    ASSERT_EQUALS(area.iCellId          , area2.iCellId          );

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	TRfStateInfo rfInfo = ERfsStateInfoInactive;
	TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
    data.Close();
    mockData1.SerialiseL(data);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV1Pckg, area);
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGsmOfflineOpNotAllowed, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/*
@SYMTestCaseID BA-CTSY-NTWC-MGCN-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentNetwork (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetwork0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoV8Pckg = networkInfoV8;

	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RMobilePhone::GetCurrentNetwork when result is not cached.
 	//-------------------------------------------------------------------------
	
	iPhone.GetCurrentNetwork(requestStatus, networkInfoV8Pckg, area);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("");
	
	ASSERT_EQUALS( RMobilePhone::ENetworkModeUnknown  , networkInfoV8.iMode       );
    ASSERT_EQUALS( RMobilePhone::ENetworkStatusUnknown, networkInfoV8.iStatus     );
    ASSERT_EQUALS( RMobilePhone::EBandUnknown         , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS( countryCode                        , networkInfoV8.iCountryCode);
    ASSERT_EQUALS( cdmaSID                            , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS( analogSID                          , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS( networkId                          , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS( displayTag                         , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS( shortName                          , networkInfoV8.iShortName  );
    ASSERT_EQUALS( longName                           , networkInfoV8.iLongName   );
    ASSERT_EQUALS( RMobilePhone::ENetworkAccessUnknown, networkInfoV8.iAccess     );
    ASSERT_TRUE( !networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_TRUE( !networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_TRUE( !networkInfoV8.iHsupaAvailableIndicator   );
    
    TBool areaKnown        = EFalse;
	TUint locationAreaCode = 0;
	TUint cellId           = 0;
	
	ASSERT_EQUALS(areaKnown       , area.iAreaKnown       );
    ASSERT_EQUALS(locationAreaCode, area.iLocationAreaCode);
    ASSERT_EQUALS(cellId          , area.iCellId          );
    
    //-------------------------------------------------------------------------
	// TEST B: RMobilePhone::GetCurrentNetwork again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
 	
	countryCode = _L("567");
    cdmaSID     = _L("890");
    analogSID   = _L("123");
    networkId   = _L("56700");
    displayTag  = _L("symbian2");
    shortName   = _L("symbian2");
    longName    = _L("symbian2 mobile");
    
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8_input;
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeCdma2000;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV8_input.iBandInfo    = RMobilePhone::E1900BandA;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessGsm;
	networkInfoV8_input.iHsdpaAvailableIndicator = ETrue;
	networkInfoV8_input.iEgprsAvailableIndicator = ETrue;
	networkInfoV8_input.iHsupaAvailableIndicator = ETrue;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area_input;
	
	areaKnown        = ETrue;
	locationAreaCode = 10;
	cellId           = 100;
	
	area_input.iAreaKnown        = areaKnown;
	area_input.iLocationAreaCode = locationAreaCode;
	area_input.iCellId           = cellId;
	
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV8, 
		RMobilePhone::TMobilePhoneLocationAreaV1> currentNetworkData(networkInfoV8_input, area_input);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	currentNetworkData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV8Pckg, area);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
	
	ASSERT_EQUALS(area_input.iAreaKnown       , area.iAreaKnown       );
    ASSERT_EQUALS(area_input.iLocationAreaCode, area.iLocationAreaCode);
    ASSERT_EQUALS(area_input.iCellId          , area.iCellId );       
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCurrentNetwork (no location)
 	//-------------------------------------------------------------------------
 	
 	countryCode = _L("");
    cdmaSID     = _L("");
    analogSID   = _L("");
    networkId   = _L("");
    displayTag  = _L("");
    shortName   = _L("");
    longName    = _L("");
    
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeUnknown;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	networkInfoV8_input.iBandInfo    = RMobilePhone::EBandUnknown;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;
 	
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV8, 
		RMobilePhone::TMobilePhoneLocationAreaV1> currentNetworkData2(networkInfoV8_input, area_input);
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	currentNetworkData2.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetwork(requestStatus, networkInfoV8Pckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	
    ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
    
    AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}
	
/*
@SYMTestCaseID BA-CTSY-NTWC-MGCN-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetCurrentNetwork (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetCurrentNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetwork0002L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);
                   
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV8> networkInfoPckgV8(networkInfoV8);

	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetCurrentNetwork
 	//-------------------------------------------------------------------------
    
    iPhone.GetCurrentNetwork(requestStatus, networkInfoPckgV8, area);
    iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
    User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	//requestStatus should be KErrNone if CTSY completed GetCurrentNetwork() 
	//request before cancel request arrived. requestStatus should be KErrCancel 
	//if CTSY received cancel request before before GetCurrentNetwork() request
	//was completed.
	ASSERT_TRUE( ( requestStatus.Int() == KErrNone || requestStatus.Int() == KErrCancel ) );
	
	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetCurrentNetwork (no location)
 	//-------------------------------------------------------------------------
    
    iPhone.GetCurrentNetwork(requestStatus, networkInfoPckgV8);
    iPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetworkNoLocation);
    User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	//requestStatus should be KErrNone if CTSY completed GetCurrentNetwork() 
	//request before cancel request arrived. requestStatus should be KErrCancel 
	//if CTSY received cancel request before before GetCurrentNetwork() request
	//was completed.
	ASSERT_TRUE( ( requestStatus.Int() == KErrNone || requestStatus.Int() == KErrCancel ) );
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}
	
/**
@SYMTestCaseID BA-CTSY-NTWC-MGCN-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentNetwork with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentNetwork with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetwork0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	

	RMobilePhone::TMobilePhoneLocationAreaV1 area;

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetCurrentNetwork
 	//-------------------------------------------------------------------------

	TBuf8<1> smallDes;

	iPhone.GetCurrentNetwork(requestStatus, smallDes, area);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::GetCurrentNetwork
 	//-------------------------------------------------------------------------

	TInt extensionId = 3000;
	TPckg< TInt > networkInfoPckg2 ( extensionId);

	iPhone.GetCurrentNetwork(requestStatus, networkInfoPckg2, area);
		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(1, this); 

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGCN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCurrentNetwork
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCurrentNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetwork0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RMobilePhone::TMobilePhoneLocationAreaV1 area1;
	RMobilePhone::TMobilePhoneLocationAreaV1 area2;

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV11;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoV1Pckg1 = networkInfoV11;

	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV12;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV1 > networkInfoV1Pckg2 = networkInfoV12;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetCurrentNetwork
 	//-------------------------------------------------------------------------

	iPhone.GetCurrentNetwork(requestStatus1, networkInfoV1Pckg1, area1);

	phone2.GetCurrentNetwork(requestStatus2, networkInfoV1Pckg2, area2);
    
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("");
        
	TBool areaKnown        = EFalse;
	TUint locationAreaCode = 0;
	TUint cellId           = 0;

    ASSERT_EQUALS(RMobilePhone::ENetworkModeUnknown  , networkInfoV11.iMode       );
    ASSERT_EQUALS(RMobilePhone::ENetworkStatusUnknown, networkInfoV11.iStatus     );
    ASSERT_EQUALS(RMobilePhone::EBandUnknown         , networkInfoV11.iBandInfo   );
    ASSERT_EQUALS(countryCode                        , networkInfoV11.iCountryCode);
    ASSERT_EQUALS(cdmaSID                            , networkInfoV11.iCdmaSID    );
    ASSERT_EQUALS(analogSID                          , networkInfoV11.iAnalogSID  );
    ASSERT_EQUALS(networkId                          , networkInfoV11.iNetworkId  );
    ASSERT_EQUALS(displayTag                         , networkInfoV11.iDisplayTag );
    ASSERT_EQUALS(shortName                          , networkInfoV11.iShortName  );
    ASSERT_EQUALS(longName                           , networkInfoV11.iLongName   );

    ASSERT_EQUALS(areaKnown       , area1.iAreaKnown       );
    ASSERT_EQUALS(locationAreaCode, area1.iLocationAreaCode);
    ASSERT_EQUALS(cellId          , area1.iCellId          );

    ASSERT_EQUALS(RMobilePhone::ENetworkModeUnknown  , networkInfoV12.iMode       );
    ASSERT_EQUALS(RMobilePhone::ENetworkStatusUnknown, networkInfoV12.iStatus     );
    ASSERT_EQUALS(RMobilePhone::EBandUnknown         , networkInfoV12.iBandInfo   );
    ASSERT_EQUALS(countryCode                        , networkInfoV12.iCountryCode);
    ASSERT_EQUALS(cdmaSID                            , networkInfoV12.iCdmaSID    );
    ASSERT_EQUALS(analogSID                          , networkInfoV12.iAnalogSID  );
    ASSERT_EQUALS(networkId                          , networkInfoV12.iNetworkId  );
    ASSERT_EQUALS(displayTag                         , networkInfoV12.iDisplayTag );
    ASSERT_EQUALS(shortName                          , networkInfoV12.iShortName  );
    ASSERT_EQUALS(longName                           , networkInfoV12.iLongName   );

    ASSERT_EQUALS(areaKnown       , area2.iAreaKnown       );
    ASSERT_EQUALS(locationAreaCode, area2.iLocationAreaCode);
    ASSERT_EQUALS(cellId          , area2.iCellId          );

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGCN-0004b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCurrentNetwork (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCurrentNetwork
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetwork0004bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// TEST A: Test multiple client requesting RMobilePhone::GetCurrentNetwork
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoV8Pckg = networkInfoV8;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area1;
	RMobilePhone::TMobilePhoneLocationAreaV1 area2;

	TBool areaKnown        = ETrue;
	TUint locationAreaCode = 100;
	TUint cellId           = 10;

	TRequestStatus mockLtsyStatus;
	
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("567");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("890");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("123");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("56700");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian2 mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8_input;
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeCdma2000;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV8_input.iBandInfo    = RMobilePhone::E1900BandA;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessGsm;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area_input;
	area_input.iAreaKnown = areaKnown;
	area_input.iLocationAreaCode = locationAreaCode;
	area_input.iCellId = cellId;
	
	TMockLtsyData2< RMobilePhone::TMobilePhoneNetworkInfoV8,
		RMobilePhone::TMobilePhoneLocationAreaV1 > networkInfoData(networkInfoV8_input, area_input );

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoData.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetwork(requestStatus1, networkInfoV5Pckg, area1);
	phone2.GetCurrentNetwork(requestStatus2, networkInfoV8Pckg, area2);
    
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV5.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV5.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV5.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV5.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV5.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV5.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV5.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV5.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV5.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV5.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV5.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV5.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV5.iEgprsAvailableIndicator   );

	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );

	//-------------------------------------------------------------------------
	// TEST B: Test multiple client requesting 
	// RMobilePhone::GetCurrentNetwork (no location)
 	//-------------------------------------------------------------------------

 	countryCode = _L("");
    cdmaSID     = _L("");
    analogSID   = _L("");
    networkId   = _L("");
    displayTag  = _L("");
    shortName   = _L("");
    longName    = _L("");
    
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeUnknown;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	networkInfoV8_input.iBandInfo    = RMobilePhone::EBandUnknown;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;

	TMockLtsyData2< RMobilePhone::TMobilePhoneNetworkInfoV8,
		RMobilePhone::TMobilePhoneLocationAreaV1 > networkInfoData2(networkInfoV8_input, area_input );

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoData2.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetwork(requestStatus1, networkInfoV5Pckg);
	phone2.GetCurrentNetwork(requestStatus2, networkInfoV8Pckg);

	User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV5.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV5.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV5.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV5.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV5.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV5.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV5.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV5.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV5.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV5.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV5.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV5.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV5.iEgprsAvailableIndicator   );

	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
    
    AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-NTWC-MNIC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyNITZInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNITZInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNITZInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	RMobilePhone::TMobilePhoneNITZ NITZInfo;

	TInt32	                                   nitzFieldsUsed = 10;
	TInt                                       timeZone       = 100;
	TInt	                                   DST            = 1000;
	RMobilePhone::TMobilePhoneNetworkShortName shortNetworkId = _L("short");
	RMobilePhone::TMobilePhoneNetworkLongName  longNetworkId  = _L("long");
	
	NITZInfo.iNitzFieldsUsed = nitzFieldsUsed;
	NITZInfo.iTimeZone       = timeZone      ;
	NITZInfo.iDST            = DST           ;
	NITZInfo.iShortNetworkId = shortNetworkId;
	NITZInfo.iLongNetworkId  = longNetworkId ;
	
	RMobilePhone::TMobilePhoneNITZ NITZInfoComplete;
	TMockLtsyData1< RMobilePhone::TMobilePhoneNITZ > NITZInfoData1( NITZInfoComplete );
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyNITZInfoChange when result is not cached.
 	//-------------------------------------------------------------------------

	iPhone.NotifyNITZInfoChange(requestStatus, NITZInfo);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	NITZInfoData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNITZInfoChange, KErrNone, data);

    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(NITZInfo.iNitzFieldsUsed, NITZInfoComplete.iNitzFieldsUsed);
    ASSERT_EQUALS(NITZInfo.iTimeZone      , NITZInfoComplete.iTimeZone      );
    ASSERT_EQUALS(NITZInfo.iDST           , NITZInfoComplete.iDST           );
    ASSERT_EQUALS(NITZInfo.iShortNetworkId, NITZInfoComplete.iShortNetworkId);
    ASSERT_EQUALS(NITZInfo.iLongNetworkId , NITZInfoComplete.iLongNetworkId );

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyNITZInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	data.Close();
	NITZInfoData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNITZInfoChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNIC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyNITZInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyNITZInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNITZInfoChange0002L()
	{

// This test should test cancellation of NotifyNITZInfoChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus; 
	TRequestStatus mockLtsyStatus;
	RMobilePhone::TMobilePhoneNITZ NITZInfo;
	RMobilePhone::TMobilePhoneNITZ NITZInfoComplete;
	TMockLtsyData1< RMobilePhone::TMobilePhoneNITZ > NITZInfoData1( NITZInfoComplete );

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyModeChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyNITZInfoChange(requestStatus, NITZInfo);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNITZInfoChange);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	NITZInfoData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNITZInfoChange, KErrNone, data);
	
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNIC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyNITZInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyNITZInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNITZInfoChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
	
	RMobilePhone::TMobilePhoneNITZ NITZInfo1;
	RMobilePhone::TMobilePhoneNITZ NITZInfo2;

	RMobilePhone::TMobilePhoneNITZ NITZInfoComplete;
	TMockLtsyData1< RMobilePhone::TMobilePhoneNITZ > NITZInfoData1( NITZInfoComplete );

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyNITZInfoChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyNITZInfoChange(requestStatus1, NITZInfo1);
	phone2.NotifyNITZInfoChange(requestStatus2, NITZInfo2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	NITZInfoData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNITZInfoChange, KErrNone, data);

    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    ASSERT_EQUALS(NITZInfo1.iNitzFieldsUsed, NITZInfoComplete.iNitzFieldsUsed);
    ASSERT_EQUALS(NITZInfo1.iTimeZone      , NITZInfoComplete.iTimeZone      );
    ASSERT_EQUALS(NITZInfo1.iDST           , NITZInfoComplete.iDST           );
    ASSERT_EQUALS(NITZInfo1.iShortNetworkId, NITZInfoComplete.iShortNetworkId);
    ASSERT_EQUALS(NITZInfo1.iLongNetworkId , NITZInfoComplete.iLongNetworkId );

    ASSERT_EQUALS(NITZInfo2.iNitzFieldsUsed, NITZInfoComplete.iNitzFieldsUsed);
    ASSERT_EQUALS(NITZInfo2.iTimeZone      , NITZInfoComplete.iTimeZone      );
    ASSERT_EQUALS(NITZInfo2.iDST           , NITZInfoComplete.iDST           );
    ASSERT_EQUALS(NITZInfo2.iShortNetworkId, NITZInfoComplete.iShortNetworkId);
    ASSERT_EQUALS(NITZInfo2.iLongNetworkId , NITZInfoComplete.iLongNetworkId );

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGCNN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentNetworkName
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentNetworkName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetworkName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCurrentNetworkName when result is not cached.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkNameV3 networkNameV3;
	TPckg< RMobilePhone::TMobilePhoneNetworkNameV3 > networkNameV3Pckg( networkNameV3 );
    
    RMobilePhone::TMobilePhoneOPlmnV3 OPlmnV3; 
	TPckg< RMobilePhone::TMobilePhoneOPlmnV3 > OPlmnV3Pckg( OPlmnV3 );
    
	iPhone.GetCurrentNetworkName(requestStatus, networkNameV3Pckg, OPlmnV3Pckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	RMobilePhone::TMobilePhoneNetworkLongName    longName   = _L("");
	RMobilePhone::TMobilePhoneNetworkShortName   shortName  = _L("");
	TBuf<RMobilePhone::KMaxNetworkNameFieldSize> otherNames = _L("");

	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode           = _L("");
	RMobilePhone::TMobilePhoneNetworkIdentity    networkId             = _L("");
	TUint32                                      PNNid                 = 0;
	TUint                                        firstLocationAreaCode = 0;
	TUint                                        lastLocationAreaCode  = 0;

    ASSERT_EQUALS(longName  , networkNameV3.iLongName  );
    ASSERT_EQUALS(shortName , networkNameV3.iShortName );
    ASSERT_EQUALS(otherNames, networkNameV3.iOtherNames);

    ASSERT_EQUALS(countryCode          , OPlmnV3.iCountryCode          );
    ASSERT_EQUALS(networkId            , OPlmnV3.iNetworkId            );
    ASSERT_EQUALS(PNNid                , OPlmnV3.iPNNid                );
    ASSERT_EQUALS(firstLocationAreaCode, OPlmnV3.iFirstLocationAreaCode);
    ASSERT_EQUALS(lastLocationAreaCode , OPlmnV3.iLastLocationAreaCode );

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetCurrentNetworkName again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneLocationAreaV1 area;

    countryCode = _L("567");
    networkId   = _L("56700");
    shortName   = _L("symbian2");
    longName    = _L("symbian2 mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 networkInfoV1Ptr;
	networkInfoV1Ptr.iCountryCode = countryCode;
	networkInfoV1Ptr.iNetworkId   = networkId  ;
	networkInfoV1Ptr.iShortName   = shortName  ;
	networkInfoV1Ptr.iLongName    = longName   ;

	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV1, 
	               RMobilePhone::TMobilePhoneLocationAreaV1> currentNetworkData2(networkInfoV1Ptr, area);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	currentNetworkData2.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetCurrentNetworkName(requestStatus, networkNameV3Pckg, OPlmnV3Pckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(networkInfoV1Ptr.iCountryCode, OPlmnV3.iCountryCode);
    ASSERT_EQUALS(networkInfoV1Ptr.iNetworkId  , OPlmnV3.iNetworkId  );
    ASSERT_EQUALS(networkInfoV1Ptr.iShortName  , networkNameV3.iShortName  );
    ASSERT_EQUALS(networkInfoV1Ptr.iLongName   , networkNameV3.iLongName   );

	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGCNN-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCurrentNetworkName with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCurrentNetworkName with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetworkName0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
		
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetCurrentNetworkName
 	//-------------------------------------------------------------------------

	TBuf8<1> wrongDes1;
	TBuf8<1> wrongDes2;

	iPhone.GetCurrentNetworkName(requestStatus, wrongDes1, wrongDes2);
	
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGCNN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCurrentNetworkName
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCurrentNetworkName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCurrentNetworkName0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetCurrentNetworkName
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkNameV3 networkNameV31;
	TPckg< RMobilePhone::TMobilePhoneNetworkNameV3 > networkNameV3Pckg1( networkNameV31 );

	RMobilePhone::TMobilePhoneNetworkNameV3 networkNameV32;
	TPckg< RMobilePhone::TMobilePhoneNetworkNameV3 > networkNameV3Pckg2( networkNameV32 );
    
    RMobilePhone::TMobilePhoneOPlmnV3 OPlmnV31; 
	TPckg< RMobilePhone::TMobilePhoneOPlmnV3 > OPlmnV3Pckg1( OPlmnV31 );

    RMobilePhone::TMobilePhoneOPlmnV3 OPlmnV32; 
	TPckg< RMobilePhone::TMobilePhoneOPlmnV3 > OPlmnV3Pckg2( OPlmnV32 );
    
	iPhone.GetCurrentNetworkName(requestStatus1, networkNameV3Pckg1, OPlmnV3Pckg1);

	phone2.GetCurrentNetworkName(requestStatus2, networkNameV3Pckg2, OPlmnV3Pckg2);
	
    User::WaitForRequest(requestStatus1);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	RMobilePhone::TMobilePhoneNetworkLongName    longName   = _L("");
	RMobilePhone::TMobilePhoneNetworkShortName   shortName  = _L("");
	TBuf<RMobilePhone::KMaxNetworkNameFieldSize> otherNames = _L("");

	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode           = _L("");
	RMobilePhone::TMobilePhoneNetworkIdentity    networkId             = _L("");
	TUint32                                      PNNid                 = 0;
	TUint                                        firstLocationAreaCode = 0;
	TUint                                        lastLocationAreaCode  = 0;

    ASSERT_EQUALS(longName  , networkNameV31.iLongName  );
    ASSERT_EQUALS(shortName , networkNameV31.iShortName );
    ASSERT_EQUALS(otherNames, networkNameV31.iOtherNames);

    ASSERT_EQUALS(countryCode          , OPlmnV31.iCountryCode          );
    ASSERT_EQUALS(networkId            , OPlmnV31.iNetworkId            );
    ASSERT_EQUALS(PNNid                , OPlmnV31.iPNNid                );
    ASSERT_EQUALS(firstLocationAreaCode, OPlmnV31.iFirstLocationAreaCode);
    ASSERT_EQUALS(lastLocationAreaCode , OPlmnV31.iLastLocationAreaCode );

    ASSERT_EQUALS(longName  , networkNameV32.iLongName  );
    ASSERT_EQUALS(shortName , networkNameV32.iShortName );
    ASSERT_EQUALS(otherNames, networkNameV32.iOtherNames);

    ASSERT_EQUALS(countryCode          , OPlmnV32.iCountryCode          );
    ASSERT_EQUALS(networkId            , OPlmnV32.iNetworkId            );
    ASSERT_EQUALS(PNNid                , OPlmnV32.iPNNid                );
    ASSERT_EQUALS(firstLocationAreaCode, OPlmnV32.iFirstLocationAreaCode);
    ASSERT_EQUALS(lastLocationAreaCode , OPlmnV32.iLastLocationAreaCode );

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceProviderName
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobilePhone::TMobilePhoneServiceProviderNameV2 nameComplete;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceProviderNameV2> nameCompleteData1(nameComplete);
	
	RMobilePhone::TMobilePhoneServiceProviderNameV2 name;
	TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg(name);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName, KErrNotSupported);
	
	iPhone.GetServiceProviderName(requestStatus, namePckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    nameCompleteData1.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
    iMockLTSY.CompleteL(EMobilePhoneGetServiceProviderName, KErrGeneral, data);
	
	iPhone.GetServiceProviderName(requestStatus, namePckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetServiceProviderName when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
    iMockLTSY.CompleteL(EMobilePhoneGetServiceProviderName, KErrNone, data);
	
	iPhone.GetServiceProviderName(requestStatus, namePckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	RMobilePhone::TDisplayRequirementsFlags	displayReq = 1;
	TBuf<RMobilePhone::KMaxSPNameSize>	    SPName     = _L("");
	TBuf<RMobilePhone::KMaxPLMNFieldSize>	PLMNField  = _L("");

    ASSERT_EQUALS(displayReq, name.iDisplayReq);
    ASSERT_EQUALS(SPName    , name.iSPName    );
    ASSERT_EQUALS(PLMNField , name.iPLMNField );

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetServiceProviderName again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	displayReq = 2;
    SPName     = _L("SPName");
	PLMNField  = _L("PLMNField");
	
	nameComplete.iDisplayReq = displayReq;
	nameComplete.iSPName     = SPName    ;
	nameComplete.iPLMNField  = PLMNField ;
	
    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
    data.Close();
    nameCompleteData1.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetServiceProviderName, KErrNone, data);
	
	iPhone.GetServiceProviderName(requestStatus, namePckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    ASSERT_EQUALS(displayReq, name.iDisplayReq);
    ASSERT_EQUALS(SPName    , name.iSPName    );
    ASSERT_EQUALS(PLMNField , name.iPLMNField );

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetServiceProviderName
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    data.Close();
    nameCompleteData1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceProviderName, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0001a
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test for coverage increasing
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0001aL()
	{
	TRequestStatus requestStatus;

	// Open new client
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RMobilePhone phone;
	TInt ret = phone.Open(iTelServer, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone);
	
	ret = iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	
	RMobilePhone::TMobilePhoneServiceProviderNameV2 name;
	TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg(name);

 	//-------------------------------------------------------------------------
	// for coverage increasing
 	//-------------------------------------------------------------------------

	phone.GetServiceProviderName(requestStatus, namePckg);
	
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // phone, this
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetServiceProviderName
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetServiceProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0002L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
 
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobilePhone::GetServiceProviderName
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
    
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneServiceProviderNameV2 name;
    TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg(name);
    
    iPhone.GetServiceProviderName(requestStatus, namePckg);

    iPhone.CancelAsyncRequest(EMobilePhoneGetServiceProviderName);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
    
    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(this); // this
    }

/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0002b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetServiceProviderName combined with a request
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetServiceProviderName, cancels the first request
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0002bL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);
    
    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //---------------------------------------------------------------------------------
    // Test cancelling of RMobilePhone::GetCustomerServiceProfile with multiple clients
    //---------------------------------------------------------------------------------
    
    // expect and complete the second request
    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
    
    RBuf8 data;
    CleanupClosePushL(data);

    RMobilePhone::TMobilePhoneServiceProviderNameV2 nameComplete;
    TMockLtsyData1<RMobilePhone::TMobilePhoneServiceProviderNameV2> nameCompleteData1(nameComplete);
    
    nameCompleteData1.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetServiceProviderName, KErrNone, data, 10);

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneServiceProviderNameV2 name;
    TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg(name);
    
    iPhone.GetServiceProviderName(requestStatus, namePckg);
 
    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneServiceProviderNameV2 name2;
    TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg2(name2);
    
    phone2.GetServiceProviderName(requestStatus2, namePckg2);
    
    // Cancel the first request
    iPhone.CancelAsyncRequest(EMobilePhoneGetServiceProviderName);
    
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
  
    RMobilePhone::TDisplayRequirementsFlags displayReq = 1;
    TBuf<RMobilePhone::KMaxSPNameSize>      SPName     = _L("");
    TBuf<RMobilePhone::KMaxPLMNFieldSize>   PLMNField  = _L("");

    ASSERT_EQUALS(displayReq, name2.iDisplayReq);
    ASSERT_EQUALS(SPName    , name2.iSPName    );
    ASSERT_EQUALS(PLMNField , name2.iPLMNField );
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(4, this); // data, telserver2, phone2, this
    }

/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceProviderName with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceProviderName with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetServiceProviderName
 	//-------------------------------------------------------------------------
	
	TBuf8<1> wrongDes;
	
	iPhone.GetServiceProviderName(requestStatus, wrongDes);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetServiceProviderName
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetServiceProviderName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	
	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneServiceProviderNameV2 nameComplete;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceProviderNameV2> nameCompleteData1(nameComplete);
	
	RMobilePhone::TMobilePhoneServiceProviderNameV2 name1;
	TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg1(name1);

	RMobilePhone::TMobilePhoneServiceProviderNameV2 name2;
	TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg2(name2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetServiceProviderName
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
    nameCompleteData1.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetServiceProviderName, KErrNone, data);
	
	iPhone.GetServiceProviderName(requestStatus1, namePckg1);

	phone2.GetServiceProviderName(requestStatus2, namePckg2);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	RMobilePhone::TDisplayRequirementsFlags	displayReq = 1;
	TBuf<RMobilePhone::KMaxSPNameSize>	    SPName     = _L("");
	TBuf<RMobilePhone::KMaxPLMNFieldSize>	PLMNField  = _L("");

    ASSERT_EQUALS(displayReq, name1.iDisplayReq);
    ASSERT_EQUALS(SPName    , name1.iSPName    );
    ASSERT_EQUALS(PLMNField , name1.iPLMNField );

    ASSERT_EQUALS(displayReq, name2.iDisplayReq);
    ASSERT_EQUALS(SPName    , name2.iSPName    );
    ASSERT_EQUALS(PLMNField , name2.iPLMNField );

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGSPN-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceProviderName with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceProviderName and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetServiceProviderName0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneServiceProviderNameV2 name;
	TPckg<RMobilePhone::TMobilePhoneServiceProviderNameV2> namePckg(name);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetServiceProviderName
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetServiceProviderName);
	
	iPhone.GetServiceProviderName(requestStatus, namePckg);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNISS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkInvScanSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkInvScanSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkInvScanSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt result;
	RMobilePhone::TMobilePhoneInvestigationScan setting;

	result = iPhone.GetNetworkInvScanSetting(setting);
	
	ASSERT_EQUALS(KErrNotSupported, result);

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNNISE-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyNetworkInvScanEvent
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkInvScanEvent
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkInvScanEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneInvestigationScanEvent event;
	
	iPhone.NotifyNetworkInvScanEvent(requestStatus, event);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGHNSP-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetHomeNetworkSearchPeriod
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetHomeNetworkSearchPeriod
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetHomeNetworkSearchPeriod0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TInt searchIntv;
	
	iPhone.GetHomeNetworkSearchPeriod(requestStatus, searchIntv);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGNSS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkSelectionSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkSelectionSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkSelectionSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TInt result;

	RMobilePhone::TMobilePhoneNetworkSelectionV1 setting;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingPckg(setting);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetNetworkSelectionSetting when result is not cached.
 	//-------------------------------------------------------------------------

	result = iPhone.GetNetworkSelectionSetting(settingPckg);
	
	ASSERT_EQUALS(KErrNone, result);

	RMobilePhone::TMobilePhoneSelectionMethod	method        = RMobilePhone::ENetworkSelectionUnknown;
	RMobilePhone::TMobilePhoneBandClass	    	bandClass     = RMobilePhone::ENetworkBandClassUnknown;
	RMobilePhone::TMobilePhoneOperation		    operationMode = RMobilePhone::ENetworkOperationUnknown;

	ASSERT_EQUALS(method       , setting.iMethod       );
	ASSERT_EQUALS(bandClass    , setting.iBandClass    );
	ASSERT_EQUALS(operationMode, setting.iOperationMode);

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetNetworkSelectionSetting again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	
	RMobilePhone::TMobilePhoneNetworkSelectionV1 settingComplete;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingData1(settingComplete);
	
	method = RMobilePhone::ENetworkSelectionAutomatic;
	
	settingComplete.iMethod = method;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	settingData1.SerialiseL(data); 
    iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSelectionSettingChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	result = iPhone.GetNetworkSelectionSetting(settingPckg);
	
	ASSERT_EQUALS(KErrNone, result);
	
	ASSERT_EQUALS(method       , setting.iMethod       );
	ASSERT_EQUALS(bandClass    , setting.iBandClass    );
	ASSERT_EQUALS(operationMode, setting.iOperationMode);

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGNSS-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkSelectionSetting with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkSelectionSetting with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkSelectionSetting0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TInt result;

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetNetworkSelectionSetting
 	//-------------------------------------------------------------------------

	TBuf8<1> wrongDes;
	
	result = iPhone.GetNetworkSelectionSetting(wrongDes);
	ASSERT_EQUALS(KErrArgument, result);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCurrentNetworkChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCurrentNetworkChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("567");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("890");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("123");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("56700");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian2 mobile");
	
    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoComplete;
	// V5
	networkInfoComplete.iEgprsAvailableIndicator = ETrue;
	networkInfoComplete.iHsdpaAvailableIndicator = EFalse;
	// V2
	networkInfoComplete.iAccess = RMobilePhone::ENetworkAccessGsm;
	// V1
	networkInfoComplete.iMode        = RMobilePhone::ENetworkModeCdma2000   ;
	networkInfoComplete.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoComplete.iBandInfo    = RMobilePhone::E1900BandA             ;
	networkInfoComplete.iCountryCode = countryCode;
	networkInfoComplete.iCdmaSID     = cdmaSID    ;
	networkInfoComplete.iAnalogSID   = analogSID  ;
	networkInfoComplete.iNetworkId   = networkId  ;
	networkInfoComplete.iDisplayTag  = displayTag ;
	networkInfoComplete.iShortName   = shortName  ;
	networkInfoComplete.iLongName    = longName   ;
	
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> mockData2(networkInfoComplete, locationArea);
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV2> networkInfoPckg(networkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCurrentNetworkChange when result is not cached.
 	//-------------------------------------------------------------------------

	iPhone.NotifyCurrentNetworkChange(requestStatus, networkInfoPckg, area);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(networkInfoComplete.iAccess     , networkInfo.iAccess);
	ASSERT_EQUALS(networkInfoComplete.iMode       , networkInfo.iMode       );
	ASSERT_EQUALS(networkInfoComplete.iStatus     , networkInfo.iStatus     );
	ASSERT_EQUALS(networkInfoComplete.iBandInfo   , networkInfo.iBandInfo   );
	ASSERT_EQUALS(networkInfoComplete.iCountryCode, networkInfo.iCountryCode);
	ASSERT_EQUALS(networkInfoComplete.iCdmaSID    , networkInfo.iCdmaSID    );
	ASSERT_EQUALS(networkInfoComplete.iAnalogSID  , networkInfo.iAnalogSID  );
	ASSERT_EQUALS(networkInfoComplete.iNetworkId  , networkInfo.iNetworkId  );
	ASSERT_EQUALS(networkInfoComplete.iDisplayTag , networkInfo.iDisplayTag );
	ASSERT_EQUALS(networkInfoComplete.iShortName  , networkInfo.iShortName  );
	ASSERT_EQUALS(networkInfoComplete.iLongName   , networkInfo.iLongName   );

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyCurrentNetworkChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCurrentNetworkChange (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCurrentNetworkChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0001bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------
    
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("567");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("890");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("123");
    RMobilePhone::TMobilePhoneNetworkIdentity	 networkId   = _L("56700");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkShortName 	 shortName   = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkLongName 	 longName    = _L("symbian2 mobile");
    
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8_input;
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeCdma2000;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV8_input.iBandInfo    = RMobilePhone::E1900BandA;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessGsm;
	networkInfoV8_input.iHsdpaAvailableIndicator = ETrue;
	networkInfoV8_input.iEgprsAvailableIndicator = ETrue;
	networkInfoV8_input.iHsupaAvailableIndicator = ETrue;
    
    TBool areaKnown        = ETrue;
	TUint locationAreaCode = 20;
	TUint cellId           = 200;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area_input;
	area_input.iAreaKnown        = areaKnown;
	area_input.iLocationAreaCode = locationAreaCode;
	area_input.iCellId           = cellId;
    
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV8, 
		RMobilePhone::TMobilePhoneLocationAreaV1> currentNetworkData(networkInfoV8_input, area_input);
    
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoV8Pckg = networkInfoV8;
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
    iPhone.NotifyCurrentNetworkChange(requestStatus, networkInfoV8Pckg, area);
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	currentNetworkData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
    
    ASSERT_EQUALS(area_input.iAreaKnown       , area.iAreaKnown       );
    ASSERT_EQUALS(area_input.iLocationAreaCode, area.iLocationAreaCode);
    ASSERT_EQUALS(area_input.iCellId          , area.iCellId          );
    
    //-------------------------------------------------------------------------
	// TEST B: Successful completion request of
	// RMobilePhone::NotifyCurrentNetworkChange (no location)
 	//-------------------------------------------------------------------------
    
    countryCode = _L("");
    cdmaSID     = _L("");
    analogSID   = _L("");
    networkId   = _L("");
    displayTag  = _L("");
    shortName   = _L("");
    longName    = _L("");
    
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeUnknown;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	networkInfoV8_input.iBandInfo    = RMobilePhone::EBandUnknown;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;
    
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV8, 
		RMobilePhone::TMobilePhoneLocationAreaV1> currentNetworkData2(networkInfoV8_input, area_input);
    
    iPhone.NotifyCurrentNetworkChange(requestStatus, networkInfoV8Pckg);
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	currentNetworkData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
    
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCurrentNetworkChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCurrentNetworkChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);

    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoComplete;
	networkInfoComplete.iAccess = RMobilePhone::ENetworkAccessGsm;
	networkInfoComplete.iEgprsAvailableIndicator = ETrue;
	networkInfoComplete.iHsdpaAvailableIndicator = EFalse;
	
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> mockData2(networkInfoComplete, locationArea);
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfo;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV2> networkInfoPckg(networkInfo);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyCurrentNetworkChange(requestStatus, networkInfoPckg, area);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	mockData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0002b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCurrentNetworkChange (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCurrentNetworkChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0002bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);
	
	TBool areaKnown        = ETrue;
	TUint locationAreaCode = 100;
	TUint cellId           = 10;

	RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("234");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("23499");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8_input;
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeCdma2000;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV8_input.iBandInfo    = RMobilePhone::E1900BandA;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessGsm;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area_input;
	area_input.iAreaKnown = areaKnown;
	area_input.iLocationAreaCode = locationAreaCode;
	area_input.iCellId = cellId;
	
	TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV8, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> networkInfoData(networkInfoV8_input, area_input);
                   
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV8> networkInfoPckgV8(networkInfoV8);

	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	
	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------
 	
 	iPhone.NotifyCurrentNetworkChange(requestStatus, networkInfoPckgV8, area);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCurrentNetworkChange (no location)
 	//-------------------------------------------------------------------------
	
	iPhone.NotifyCurrentNetworkChange(requestStatus, networkInfoPckgV8);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange);
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // data, this
	}
	
/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCurrentNetworkChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCurrentNetworkChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TBuf8<1> badDescriptor;
	
	iPhone.NotifyCurrentNetworkChange(requestStatus, badDescriptor, area);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
    
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------
    
	TInt index(KETelExtMultimodeV1);
	TPckg<TInt> pckg(index);
	
	iPhone.NotifyCurrentNetworkChange(requestStatus, pckg, area);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(this); // this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCurrentNetworkChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCurrentNetworkChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoComplete;
	networkInfoComplete.iAccess = RMobilePhone::ENetworkAccessGsm;
	networkInfoComplete.iEgprsAvailableIndicator = ETrue;
	networkInfoComplete.iHsdpaAvailableIndicator = EFalse;
	
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
    TMockLtsyData2<RMobilePhone::TMobilePhoneNetworkInfoV5, 
                   RMobilePhone::TMobilePhoneLocationAreaV1> mockData2(networkInfoComplete, locationArea);
	
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfo1;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV2> networkInfoPckg1(networkInfo1);
	RMobilePhone::TMobilePhoneNetworkInfoV2 networkInfo2;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV2> networkInfoPckg2(networkInfo2);
	RMobilePhone::TMobilePhoneLocationAreaV1 area1;
	RMobilePhone::TMobilePhoneLocationAreaV1 area2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyCurrentNetworkChange(requestStatus1, networkInfoPckg1, area1);
	phone2.NotifyCurrentNetworkChange(requestStatus2, networkInfoPckg2, area2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	ASSERT_EQUALS(networkInfoComplete.iAccess, networkInfo1.iAccess);
	ASSERT_EQUALS(networkInfoComplete.iAccess, networkInfo2.iAccess);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNCNC-0004b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCurrentNetworkChange (TMobilePhoneNetworkInfoV8)
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCurrentNetworkChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCurrentNetworkChange0004bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
	
	//-------------------------------------------------------------------------
	// TEST A: Test multiple client requesting 
	// RMobilePhone::NotifyCurrentNetworkChange
 	//-------------------------------------------------------------------------
    
    RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfoV5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfoV5Pckg = networkInfoV5;

	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV8 > networkInfoV8Pckg = networkInfoV8;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area1;
	RMobilePhone::TMobilePhoneLocationAreaV1 area2;
	
    TBool areaKnown        = ETrue;
	TUint locationAreaCode = 200;
	TUint cellId           = 20;
    
    RMobilePhone::TMobilePhoneNetworkCountryCode countryCode = _L("567");
    RMobilePhone::TMobilePhoneNetworkIdentity    cdmaSID     = _L("890");
    RMobilePhone::TMobilePhoneNetworkIdentity    analogSID   = _L("123");
    RMobilePhone::TMobilePhoneNetworkIdentity    networkId   = _L("56700");
    RMobilePhone::TMobilePhoneNetworkDisplayTag  displayTag  = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkShortName   shortName   = _L("symbian2");
    RMobilePhone::TMobilePhoneNetworkLongName    longName    = _L("symbian2 mobile");
	
	RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfoV8_input;
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeCdma2000;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusAvailable;
	networkInfoV8_input.iBandInfo    = RMobilePhone::E1900BandA;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessGsm;
	networkInfoV8_input.iHsdpaAvailableIndicator = ETrue;
	networkInfoV8_input.iEgprsAvailableIndicator = ETrue;
	networkInfoV8_input.iHsupaAvailableIndicator = ETrue;
	
	RMobilePhone::TMobilePhoneLocationAreaV1 area_input;
	area_input.iAreaKnown = areaKnown;
	area_input.iLocationAreaCode = locationAreaCode;
	area_input.iCellId = cellId;
	
	TMockLtsyData2< RMobilePhone::TMobilePhoneNetworkInfoV8,
		RMobilePhone::TMobilePhoneLocationAreaV1 > networkInfoData(networkInfoV8_input, area_input );
	
	iPhone.NotifyCurrentNetworkChange(requestStatus1, networkInfoV5Pckg, area1);
	phone2.NotifyCurrentNetworkChange(requestStatus2, networkInfoV8Pckg, area2);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV5.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV5.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV5.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV5.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV5.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV5.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV5.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV5.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV5.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV5.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV5.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV5.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV5.iEgprsAvailableIndicator   );

	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
	
	
    //-------------------------------------------------------------------------
	// TEST B: Test multiple client requesting 
	// RMobilePhone::NotifyCurrentNetworkChange (no location)
 	//-------------------------------------------------------------------------	
	
	countryCode = _L("");
    cdmaSID     = _L("");
    analogSID   = _L("");
    networkId   = _L("");
    displayTag  = _L("");
    shortName   = _L("");
    longName    = _L("");
    
	networkInfoV8_input.iMode        = RMobilePhone::ENetworkModeUnknown;
	networkInfoV8_input.iStatus      = RMobilePhone::ENetworkStatusUnknown;
	networkInfoV8_input.iBandInfo    = RMobilePhone::EBandUnknown;
	networkInfoV8_input.iCountryCode = countryCode;
	networkInfoV8_input.iCdmaSID     = cdmaSID;
	networkInfoV8_input.iAnalogSID   = analogSID;
	networkInfoV8_input.iNetworkId   = networkId;
	networkInfoV8_input.iDisplayTag  = displayTag;
	networkInfoV8_input.iShortName   = shortName;
	networkInfoV8_input.iLongName    = longName;
	networkInfoV8_input.iAccess      = RMobilePhone::ENetworkAccessUnknown;
	networkInfoV8_input.iHsdpaAvailableIndicator = EFalse;
	networkInfoV8_input.iEgprsAvailableIndicator = EFalse;
	networkInfoV8_input.iHsupaAvailableIndicator = EFalse;

	TMockLtsyData2< RMobilePhone::TMobilePhoneNetworkInfoV8,
		RMobilePhone::TMobilePhoneLocationAreaV1 > networkInfoData2(networkInfoV8_input, area_input );
	
	iPhone.NotifyCurrentNetworkChange(requestStatus1, networkInfoV5Pckg);
	phone2.NotifyCurrentNetworkChange(requestStatus2, networkInfoV8Pckg);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	networkInfoData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneNotifyCurrentNetworkChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV5.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV5.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV5.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV5.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV5.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV5.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV5.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV5.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV5.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV5.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV5.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV5.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV5.iEgprsAvailableIndicator   );

	ASSERT_EQUALS(networkInfoV8_input.iMode       , networkInfoV8.iMode       );
    ASSERT_EQUALS(networkInfoV8_input.iStatus     , networkInfoV8.iStatus     );
    ASSERT_EQUALS(networkInfoV8_input.iBandInfo   , networkInfoV8.iBandInfo   );
    ASSERT_EQUALS(networkInfoV8_input.iCountryCode, networkInfoV8.iCountryCode);
    ASSERT_EQUALS(networkInfoV8_input.iCdmaSID    , networkInfoV8.iCdmaSID    );
    ASSERT_EQUALS(networkInfoV8_input.iAnalogSID  , networkInfoV8.iAnalogSID  );
    ASSERT_EQUALS(networkInfoV8_input.iNetworkId  , networkInfoV8.iNetworkId  );
    ASSERT_EQUALS(networkInfoV8_input.iDisplayTag , networkInfoV8.iDisplayTag );
    ASSERT_EQUALS(networkInfoV8_input.iShortName  , networkInfoV8.iShortName  );
    ASSERT_EQUALS(networkInfoV8_input.iLongName   , networkInfoV8.iLongName   );
    ASSERT_EQUALS(networkInfoV8_input.iAccess     , networkInfoV8.iAccess     );
    ASSERT_EQUALS(networkInfoV8_input.iHsdpaAvailableIndicator, networkInfoV8.iHsdpaAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iEgprsAvailableIndicator, networkInfoV8.iEgprsAvailableIndicator   );
    ASSERT_EQUALS(networkInfoV8_input.iHsupaAvailableIndicator, networkInfoV8.iHsupaAvailableIndicator   );
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNNISC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyNetworkInvScanChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkInvScanChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkInvScanChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneInvestigationScan setting;

	iPhone.NotifyNetworkInvScanChange(requestStatus, setting);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSNSS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetNetworkSelectionSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetNetworkSelectionSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSetNetworkSelectionSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus; 
    TRequestStatus mockLtsyStatus;

	RMobilePhone::TMobilePhoneNetworkSelectionV1 setting;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingPckg(setting);

	setting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> mockData1(setting);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	mockData1.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data, KErrNotSupported);

	setting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	iPhone.SetNetworkSelectionSetting(requestStatus, settingPckg);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data);
    iMockLTSY.CompleteL(EMobilePhoneSetNetworkSelectionSetting, KErrGeneral);

	setting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	iPhone.SetNetworkSelectionSetting(requestStatus, settingPckg);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetNetworkSelectionSetting when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data);
    iMockLTSY.CompleteL(EMobilePhoneSetNetworkSelectionSetting, KErrNone);

	iPhone.SetNetworkSelectionSetting(requestStatus, settingPckg);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
 	// for coverage increasing
 	//-------------------------------------------------------------------------

	setting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> mockData12(setting);

	data.Close();
	mockData12.SerialiseL(data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSelectionSettingChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.SetNetworkSelectionSetting(requestStatus, settingPckg);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetNetworkSelectionSetting
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSetNetworkSelectionSetting, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MSNSS-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetNetworkSelectionSetting
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetNetworkSelectionSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSetNetworkSelectionSetting0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus; 

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 setting;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingPckg(setting);

	setting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> mockData1(setting);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetNetworkSelectionSetting
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	mockData1.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data);
    iMockLTSY.CompleteL(EMobilePhoneSetNetworkSelectionSetting, KErrNone, 10);
	
	iPhone.SetNetworkSelectionSetting(requestStatus, settingPckg);

    iPhone.CancelAsyncRequest(EMobilePhoneSetNetworkSelectionSetting);
		
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    
    //RMobilePhone::SetNetworkSelectionSetting request cannot be cancelled once it has been sent.  
    //This is the reason why KErrNone is returned. KErrNone means that original
    //RMobilePhone::SetNetworkSelectionSetting request was handled successfully.
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	
	CleanupStack::PopAndDestroy(2); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MSNSS-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetNetworkSelectionSetting with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetNetworkSelectionSetting with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSetNetworkSelectionSetting0003L()
	{
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    TRequestStatus requestStatus; 

    //-------------------------------------------------------------------------
    // Test B: Test passing wrong descriptor size to parameter in
    // RMobilePhone::GetCellInfo
    //-------------------------------------------------------------------------

    RMobilePhone::TMobilePhoneLocationAreaV1 setting;
    TPckg<RMobilePhone::TMobilePhoneLocationAreaV1> settingPckg(setting);
    
    TBuf8<1> smallDes;

    iPhone.SetNetworkSelectionSetting(requestStatus, smallDes);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(this); // this
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSNSS-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetNetworkSelectionSetting
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetNetworkSelectionSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSetNetworkSelectionSetting0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);

	TRequestStatus requestStatus1; 
	TRequestStatus requestStatus2; 

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 setting1;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingPckg1(setting1);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 setting2;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingPckg2(setting2);

	setting1.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	setting2.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> mockData11(setting1);
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> mockData12(setting2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetNetworkSelectionSetting
 	//-------------------------------------------------------------------------

	mockData11.SerialiseL(data1);
    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data1);
    iMockLTSY.CompleteL(EMobilePhoneSetNetworkSelectionSetting, KErrNone);

	iPhone.SetNetworkSelectionSetting(requestStatus1, settingPckg1);

	mockData12.SerialiseL(data2);
    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data2);
    iMockLTSY.CompleteL(EMobilePhoneSetNetworkSelectionSetting, KErrNone);

	phone2.SetNetworkSelectionSetting(requestStatus2, settingPckg2);
	
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data1, data2, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSNSS-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetNetworkSelectionSetting with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetNetworkSelectionSetting and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestSetNetworkSelectionSetting0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus; 

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 setting;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> settingPckg(setting);

	setting.iMethod = RMobilePhone::ENetworkSelectionAutomatic;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> mockData1(setting);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetNetworkSelectionSetting
 	//-------------------------------------------------------------------------

	/*
	 * Test for timeout of RMobilePhone::SetNetworkSelectionSetting fails - 
	 * (fuction CMmPhoneTsy::Complete, line 5399 in cmmphonetsy.cpp ).
	 */
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 261902);
	ASSERT_TRUE(EFalse);
	
	mockData1.SerialiseL(data);
    iMockLTSY.ExpectL(EMobilePhoneSetNetworkSelectionSetting, data);

	iPhone.SetNetworkSelectionSetting(requestStatus, settingPckg);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGLUAT-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetLastUsedAccessTechnology
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetLastUsedAccessTechnology
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetLastUsedAccessTechnology0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneNetworkAccess accessTech;
	
	iPhone.GetLastUsedAccessTechnology(requestStatus, accessTech);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MSPNL-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::StorePreferredNetworksListL
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::StorePreferredNetworksListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestStorePreferredNetworksListL0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	CMobilePhoneStoredNetworkList* list = CMobilePhoneStoredNetworkList::NewL();
	CleanupStack::PushL(list);
	
	iPhone.StorePreferredNetworksListL(requestStatus, list);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // data, this, list
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGNRS-0001
@SYMPREQ 1551
@SYMCR 1595
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkRegistrationStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkRegistrationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkRegistrationStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneRegistrationStatus status;
    
    // Initial test to verify that GetNetworkRegistrationStatus behaves 
	// correctly when the modem is not ready (i.e., before the LTSY sends
    // a EMmTsyBootNotifyModemStatusReadyIPC notification to CTSY)
    TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, err);

    iPhone.GetNetworkRegistrationStatus(requestStatus, status);
    
    // If the modem is not ready, the request should complete immediately
    // with KErrNone, and registration status should be "Unknown"
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobilePhone::ERegistrationUnknown, status);
	iPhone.Close();
	
	OpenPhoneL(); // whole phone bootup procedure for rest of tests
	
	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus, KErrNotSupported);

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
    iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, KErrGeneral);

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetNetworkRegistrationStatus when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
    iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, KErrNone);

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(RMobilePhone::ERegistrationUnknown, status);
	
	//-------------------------------------------------------------------------
	// TEST : Completion of request 
	// RMobilePhone::GetNetworkRegistrationStatus with extended error code.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
    iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound));

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmCCNetworkOutOfOrder, requestStatus.Int());
	
	// Check that basic error code is returned when error granuality set to EErrorBasic
	RTelServer::TErrorGranularity granularity(RTelServer::EErrorBasic);
	TInt ret = iTelServer.SetExtendedErrorGranularity(granularity);
	ASSERT_EQUALS(KErrNone, ret);

	iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound));

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetNetworkRegistrationStatus again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus = RMobilePhone::ENotRegisteredNoService;
	RMobilePhone::TMobilePhoneNetworkStatus networkStatus;

	TMockLtsyData2<RMobilePhone::TMobilePhoneRegistrationStatus, 
	               RMobilePhone::TMobilePhoneNetworkStatus> mockData2(registrationStatus, networkStatus);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	mockData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(status, registrationStatus);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetNetworkRegistrationStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MGNRS-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetNetworkRegistrationStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetNetworkRegistrationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkRegistrationStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus status;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetNetworkRegistrationStatus
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
    iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, KErrNone);

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);

    iPhone.CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus);
		
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(1); // this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGNRS-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetNetworkRegistrationStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetNetworkRegistrationStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkRegistrationStatus0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RMobilePhone::TMobilePhoneRegistrationStatus status1 = RMobilePhone::ERegistrationUnknown;
	RMobilePhone::TMobilePhoneRegistrationStatus status2 = RMobilePhone::ERegistrationUnknown;
	
	RMobilePhone::TMobilePhoneRegistrationStatus compRegistrationStatus = RMobilePhone::ERegisteredOnHomeNetwork;
	RMobilePhone::TMobilePhoneNetworkStatus compNetworkStatus = RMobilePhone::ENetworkStatusAvailable;
	TMockLtsyData2<RMobilePhone::TMobilePhoneRegistrationStatus, 
	               RMobilePhone::TMobilePhoneNetworkStatus> mockData2(compRegistrationStatus, compNetworkStatus);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetNetworkRegistrationStatus
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
    
	mockData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);	
    iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, KErrNone);

	iPhone.GetNetworkRegistrationStatus(requestStatus1, status1);

	phone2.GetNetworkRegistrationStatus(requestStatus2, status2);
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	ASSERT_EQUALS(RMobilePhone::ERegisteredOnHomeNetwork, status1);
	ASSERT_EQUALS(RMobilePhone::ERegisteredOnHomeNetwork, status2);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MGNRS-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetNetworkRegistrationStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetNetworkRegistrationStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetNetworkRegistrationStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneRegistrationStatus status;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetNetworkRegistrationStatus
 	//-------------------------------------------------------------------------

	/*
	 * Test for timeout of RMobilePhone::GetNetworkRegistrationStatus fails - 
	 * testexecute aborts test because of timeout
	 */
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 262203);
	ASSERT_TRUE(EFalse);

    iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);

	iPhone.GetNetworkRegistrationStatus(requestStatus, status);

    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this

	}
/**
@SYMTestCaseID BA-CTSY-NTWC-MNPNLC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyPreferredNetworksListChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyPreferredNetworksListChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyPreferredNetworksListChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	
	iPhone.NotifyPreferredNetworksListChange(requestStatus);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNNSSC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkSelectionSettingChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV1;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> pckgSelectionV1(selectionV1);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV1Complete;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> dataSelectionV1(selectionV1Complete);
		
	RMobilePhone::TMobilePhoneSelectionMethod method   = RMobilePhone::ENetworkSelectionAutomatic;
	RMobilePhone::TMobilePhoneBandClass bandClass     = RMobilePhone::ENetworkBandClassBOnly; 
	RMobilePhone::TMobilePhoneOperation operationMode = RMobilePhone::ENetworkOperationAnalogPreferred; 
	selectionV1Complete.iMethod        = method;
	selectionV1Complete.iBandClass     = bandClass;
	selectionV1Complete.iOperationMode = operationMode;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyNetworkSelectionSettingChange when result is not cached.
 	//-------------------------------------------------------------------------

	iPhone.NotifyNetworkSelectionSettingChange(requestStatus, pckgSelectionV1);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	dataSelectionV1.SerialiseL(data);	
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSelectionSettingChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(selectionV1Complete.iMethod           , selectionV1.iMethod       );
	// hard code value
	ASSERT_EQUALS(RMobilePhone::ENetworkBandClassUnknown, selectionV1.iBandClass    );
	// hard code value
	ASSERT_EQUALS(RMobilePhone::ENetworkOperationUnknown, selectionV1.iOperationMode);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyNetworkSelectionSettingChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSelectionSettingChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNNSSC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkSelectionSettingChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV1;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> pckgSelectionV1(selectionV1);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV1Complete;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> dataSelectionV1(selectionV1Complete);

	RMobilePhone::TMobilePhoneSelectionMethod method = RMobilePhone::ENetworkSelectionAutomatic;
	selectionV1Complete.iMethod = method;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyNetworkSelectionSettingChange
 	//-------------------------------------------------------------------------
 	
	iPhone.NotifyNetworkSelectionSettingChange(requestStatus, pckgSelectionV1);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkSelectionSettingChange);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	dataSelectionV1.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSelectionSettingChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-NTWC-MNNSSC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyNetworkSelectionSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkSelectionSettingChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV11;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> pckgSelectionV11(selectionV11);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV12;
	TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1> pckgSelectionV12(selectionV12);

	RMobilePhone::TMobilePhoneNetworkSelectionV1 selectionV1Complete;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkSelectionV1> dataSelectionV1(selectionV1Complete);

	RMobilePhone::TMobilePhoneSelectionMethod method = RMobilePhone::ENetworkSelectionAutomatic;
	selectionV1Complete.iMethod = method;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyNetworkSelectionSettingChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyNetworkSelectionSettingChange(requestStatus1, pckgSelectionV11);
	phone2.NotifyNetworkSelectionSettingChange(requestStatus2, pckgSelectionV12);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	dataSelectionV1.SerialiseL(data);	
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkSelectionSettingChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(selectionV1Complete.iMethod, selectionV11.iMethod);
	ASSERT_EQUALS(selectionV1Complete.iMethod, selectionV12.iMethod);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNNRSC-0001
@SYMPREQ 1551
@SYMCR 1595
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkRegistrationStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatusComplete;
	RMobilePhone::TMobilePhoneNetworkStatus networkStatusComplete = RMobilePhone::ENetworkStatusUnknown;;
	TMockLtsyData2<RMobilePhone::TMobilePhoneRegistrationStatus, RMobilePhone::TMobilePhoneNetworkStatus> 
		dataStatusComplete(registrationStatusComplete, networkStatusComplete);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyNetworkRegistrationStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

	RArray<RMobilePhone::TMobilePhoneRegistrationStatus> arrayRegistrationStatus;
	CleanupClosePushL(arrayRegistrationStatus);
	
	// see line 531 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ENotRegisteredNoService);
	// see line 532 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ENotRegisteredEmergencyOnly);
	// see line 533 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ENotRegisteredSearching);
	// see line 534 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ERegistrationDenied);
	// see line 544 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ERegisteredBusy);
	// see line 545 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ERegisteredOnHomeNetwork);
	// see line 546 in CMmPhoneGsmWcdmaExt.cpp
	arrayRegistrationStatus.AppendL(RMobilePhone::ERegisteredRoaming);

	for (TInt i = 0; i < arrayRegistrationStatus.Count(); i++)
		{
		registrationStatusComplete = arrayRegistrationStatus[i];

		iPhone.NotifyNetworkRegistrationStatusChange(requestStatus, registrationStatus);
		
		dataStatusComplete.SerialiseL(data);	
		iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);
		
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		ASSERT_EQUALS(KErrNone, requestStatus.Int());

		ASSERT_EQUALS(registrationStatus, registrationStatusComplete);
		}

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyNetworkRegistrationStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	dataStatusComplete.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    //-------------------------------------------------------------------------
    // TEST: Completion of request RMobilePhone::NotifyNetworkRegistrationStatusChange 
    // with extended error code
    //-------------------------------------------------------------------------
    registrationStatusComplete = RMobilePhone::ENotRegisteredNoService;
    
    iPhone.NotifyNetworkRegistrationStatusChange(requestStatus, registrationStatus);
    		
	dataStatusComplete.SerialiseL(data);	
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound), data);
    
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmCCNetworkOutOfOrder, requestStatus.Int());
	
	// Check that basic error code is returned when error granuality set to EErrorBasic
	RTelServer::TErrorGranularity granularity(RTelServer::EErrorBasic);
	TInt ret = iTelServer.SetExtendedErrorGranularity(granularity);
	ASSERT_EQUALS(KErrNone, ret);
	
	registrationStatusComplete = RMobilePhone::ENotRegisteredEmergencyOnly;
	
	iPhone.NotifyNetworkRegistrationStatusChange(requestStatus, registrationStatus);
	    		
	dataStatusComplete.SerialiseL(data);	
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound), data);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // data, this, arrayRegistrationStatus
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNNRSC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkRegistrationStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus;
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatusComplete;
	RMobilePhone::TMobilePhoneNetworkStatus networkStatusComplete = RMobilePhone::ENetworkStatusUnknown;;
	TMockLtsyData2<RMobilePhone::TMobilePhoneRegistrationStatus, RMobilePhone::TMobilePhoneNetworkStatus> 
		dataStatusComplete(registrationStatusComplete, networkStatusComplete);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyNetworkRegistrationStatusChange
 	//-------------------------------------------------------------------------
 	
	iPhone.NotifyNetworkRegistrationStatusChange(requestStatus, registrationStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyNetworkRegistrationStatusChange);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	dataStatusComplete.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-MNNRSC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyNetworkRegistrationStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyNetworkRegistrationStatusChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus1;
	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatus2;

	RMobilePhone::TMobilePhoneRegistrationStatus registrationStatusComplete;
	RMobilePhone::TMobilePhoneNetworkStatus networkStatusComplete = RMobilePhone::ENetworkStatusUnknown;;
	TMockLtsyData2<RMobilePhone::TMobilePhoneRegistrationStatus, RMobilePhone::TMobilePhoneNetworkStatus> 
		dataStatusComplete(registrationStatusComplete, networkStatusComplete);

	registrationStatusComplete = RMobilePhone::ENotRegisteredNoService;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyNetworkRegistrationStatusChange
 	//-------------------------------------------------------------------------

	iPhone.NotifyNetworkRegistrationStatusChange(requestStatus1, registrationStatus1);
	phone2.NotifyNetworkRegistrationStatusChange(requestStatus2, registrationStatus2);
		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	dataStatusComplete.SerialiseL(data);	
	iMockLTSY.CompleteL(EMobilePhoneNotifyNetworkRegistrationStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
		
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(registrationStatus1, registrationStatusComplete);
	ASSERT_EQUALS(registrationStatus2, registrationStatusComplete);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-PGCI-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCellInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetCellInfo when result is not cached.
 	//-------------------------------------------------------------------------
	
	RMobilePhone::TMobilePhoneCellInfoV9 mockCellInfo;
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 37;
	mockCellInfo.iTimingAdvance = 42;
	mockCellInfo.iStatus = RMobilePhone::ECellInfoStatusCurrent;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> mockCellInfoData(mockCellInfo);
	mockCellInfoData.SerialiseL(data);

	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	TPckg< RMobilePhone::TMobilePhoneCellInfoV9 > cellInfoPckg( cellInfo );
	
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoIndReq);
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);
	
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrNone);
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
 
    ASSERT_EQUALS(cellInfo.iCellId,			mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo.iMode, 			mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo.iTimingAdvance, 	mockCellInfo.iTimingAdvance);
    ASSERT_EQUALS(cellInfo.iStatus       , 	mockCellInfo.iStatus);
    
 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetCellInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo2;
	TPckg< RMobilePhone::TMobilePhoneCellInfoV9 > cellInfo2Pckg( cellInfo2 );
	
	iPhone.GetCellInfo(requestStatus, cellInfo2Pckg);
	
	// Cell Info cache should be enabled and clean, so our request should 
	// complete straight away without going to the LTSY
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL(); 
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
 
    ASSERT_EQUALS(cellInfo2.iCellId,			mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo2.iMode, 				mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo2.iTimingAdvance, 	mockCellInfo.iTimingAdvance); 
    ASSERT_EQUALS(cellInfo2.iStatus, 			mockCellInfo.iStatus); 
    
    
    
    // 1. Tell MockLTSY to complete an IndReq with KErrNotSupported. This should set cache ability to Disabled and cache state to Dirty
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrNotSupported);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
    
    // 2. Do a GetCellInfo call. The request should go to the MockLTSY. Tell MockLTSY to complete.
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 122;
	mockCellInfo.iTimingAdvance = 12;
	mockCellInfo.iStatus = RMobilePhone::ECellInfoStatusNotCurrent;
	mockCellInfoData.SerialiseL(data);


	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);


	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(cellInfo.iCellId,			mockCellInfo.iCellId);
	ASSERT_EQUALS(cellInfo.iMode, 			mockCellInfo.iMode);
	ASSERT_EQUALS(cellInfo.iTimingAdvance, 	mockCellInfo.iTimingAdvance);
	ASSERT_EQUALS(cellInfo.iStatus, 	    mockCellInfo.iStatus);

    // 3. Tell MockLTSY to complete an IndReq with KErrNone. This should set cache ability to Enabled.
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp,KErrNone);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
    
    // 4. Do a GetCellInfo call. The request should go to the MockLTSY (cache state should be Dirty)
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 17;
	mockCellInfo.iTimingAdvance = 30;
	mockCellInfo.iStatus = RMobilePhone::ECellInfoStatusUnknown;
	mockCellInfoData.SerialiseL(data);

	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);


	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(cellInfo.iCellId,			mockCellInfo.iCellId);
	ASSERT_EQUALS(cellInfo.iMode, 			mockCellInfo.iMode);
	ASSERT_EQUALS(cellInfo.iTimingAdvance, 	mockCellInfo.iTimingAdvance);
	ASSERT_EQUALS(cellInfo.iStatus, 		mockCellInfo.iStatus);
    
    // 5. Do another GetCellInfo call. The request should complete straight away (cache should be Enabled and Clean)
	iPhone.GetCellInfo(requestStatus, cellInfo2Pckg);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL(); 
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(cellInfo2.iCellId,			mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo2.iMode, 			mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo2.iTimingAdvance, 	mockCellInfo.iTimingAdvance); 
    ASSERT_EQUALS(cellInfo2.iStatus, 	mockCellInfo.iStatus); 
    
    // 6. Tell MockLTSY to complete an IndReq with an error code other than KErrNone or KErrNotSupported (e.g. KErrGeneral) to verify that cache is disabled too.
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrGeneral);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
    
    // 7. Do a GetCellInfo call. The request should go to the MockLTSY (cache disabled)
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 117;
	mockCellInfo.iTimingAdvance = 13;
	mockCellInfo.iStatus = RMobilePhone::ECellInfoStatusNotCurrent;
	mockCellInfoData.SerialiseL(data);

	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);


	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(cellInfo.iCellId,			mockCellInfo.iCellId);
	ASSERT_EQUALS(cellInfo.iMode, 			mockCellInfo.iMode);
	ASSERT_EQUALS(cellInfo.iTimingAdvance, 	mockCellInfo.iTimingAdvance);
	ASSERT_EQUALS(cellInfo.iStatus, 	mockCellInfo.iStatus);

	// 8. Complete the GetCellInfo call with an error. This should set cache to dirty.
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrGeneral);


	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// 9. To check that cache is dirty, do another GetCellInfo that should go to the MockLTSY
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 5;
	mockCellInfo.iTimingAdvance = 6;
	mockCellInfo.iStatus = RMobilePhone::ECellInfoStatusUnknown;
	mockCellInfoData.SerialiseL(data);

	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);


	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(cellInfo.iCellId,			mockCellInfo.iCellId);
	ASSERT_EQUALS(cellInfo.iMode, 			mockCellInfo.iMode);
	ASSERT_EQUALS(cellInfo.iTimingAdvance, 	mockCellInfo.iTimingAdvance);
	ASSERT_EQUALS(cellInfo.iStatus, 	mockCellInfo.iStatus);
    
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-PGCI-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCellInfo0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	TPckg< RMobilePhone::TMobilePhoneCellInfoV9 > cellInfoPckg( cellInfo );
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetCellInfo
 	//-------------------------------------------------------------------------
 	// 1. Test Cancellation with no response from LTSY
 	
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoIndReq);
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);

    iPhone.CancelAsyncRequest(EMobilePhoneGetCellInfo);
		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    //2. Test Cancellation after indicator returned
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);
	iPhone.GetCellInfo(requestStatus, cellInfoPckg);
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrNone);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    iPhone.CancelAsyncRequest(EMobilePhoneGetCellInfo);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
    
    //3. Test that GetCellInfo still works fine
	RMobilePhone::TMobilePhoneCellInfoV9 mockCellInfo;
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 111;
	mockCellInfo.iTimingAdvance = 2;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> mockCellInfoData(mockCellInfo);
	mockCellInfoData.SerialiseL(data);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	User::WaitForRequest(mockLtsyStatus);

	iPhone.GetCellInfo(requestStatus, cellInfoPckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
 
    ASSERT_EQUALS(cellInfo.iCellId,			mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo.iMode, 			mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo.iTimingAdvance, 	mockCellInfo.iTimingAdvance);

	CleanupStack::PopAndDestroy(2); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-NTWC-PGCI-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCellInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCellInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCellInfo0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::GetCellInfo
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfov5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfov5Pckg ( networkInfov5);
	iPhone.GetCellInfo(requestStatus, networkInfov5Pckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetCellInfo
 	//-------------------------------------------------------------------------

	TBuf8<1> smallDes;

	iPhone.GetCellInfo(requestStatus, smallDes);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(1, this); // this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-PGCI-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCellInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCellInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestGetCellInfo0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
	
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo1;
	TPckg< RMobilePhone::TMobilePhoneCellInfoV9 > cellInfoPckg1 ( cellInfo1 );

	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo2;
	TPckg< RMobilePhone::TMobilePhoneCellInfoV9 > cellInfoPckg2 ( cellInfo2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetCellInfo with cach
 	//-------------------------------------------------------------------------
	RMobilePhone::TMobilePhoneCellInfoV9 mockCellInfo;
	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 37;
	mockCellInfo.iTimingAdvance = 42;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> mockCellInfoData(mockCellInfo);
	mockCellInfoData.SerialiseL(data);

	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	TPckg< RMobilePhone::TMobilePhoneCellInfoV9 > cellInfoPckg( cellInfo );
	
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoIndReq);
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);

	iPhone.GetCellInfo(requestStatus1, cellInfoPckg1);
	phone2.GetCellInfo(requestStatus2, cellInfoPckg2);

    iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrNone);
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	
    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    ASSERT_EQUALS(cellInfo1.iMode       , mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo1.iCellId       , mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo1.iTimingAdvance       , mockCellInfo.iTimingAdvance);

    ASSERT_EQUALS(cellInfo2.iMode       , mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo2.iCellId       , mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo2.iTimingAdvance       , mockCellInfo.iTimingAdvance);

	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RMobilePhone::GetCellInfo without cach
 	//-------------------------------------------------------------------------

    // Disable the cache
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrNotSupported);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);

	mockCellInfo.iMode = RMobilePhone::ECellModeGsm;
	mockCellInfo.iCellId = 32;
	mockCellInfo.iTimingAdvance = 14;
	mockCellInfoData.SerialiseL(data);

	
	iMockLTSY.ExpectL(ECtsyPhoneCellInfoReq);

	iPhone.GetCellInfo(requestStatus1, cellInfoPckg1);
	phone2.GetCellInfo(requestStatus2, cellInfoPckg2);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	
    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    ASSERT_EQUALS(cellInfo1.iMode       , mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo1.iCellId       , mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo1.iTimingAdvance       , mockCellInfo.iTimingAdvance);

    ASSERT_EQUALS(cellInfo2.iMode       , mockCellInfo.iMode);
    ASSERT_EQUALS(cellInfo2.iCellId       , mockCellInfo.iCellId);
    ASSERT_EQUALS(cellInfo2.iTimingAdvance       , mockCellInfo.iTimingAdvance);
    
    
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-NTWC-PNCIC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCellInfoChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCellInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCellInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus requestStatus;
	
    RMobilePhone::TMobilePhoneCellInfoV9 cellInfoComplete;
    cellInfoComplete.iMode = RMobilePhone::ECellModeWcdma;
    cellInfoComplete.iCellId = 15;
    cellInfoComplete.iTimingAdvance = 74;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> mockData2(cellInfoComplete);
	
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	TPckg<RMobilePhone::TMobilePhoneCellInfoV9> cellInfoPckg(cellInfo);

	mockData2.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus; 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyCellInfoChange when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECtsyPhoneCellInfoIndReq);
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp,KErrNone);

    ASSERT_EQUALS(KRequestPending, requestStatus.Int());
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
    
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo.iTimingAdvance);

	// test cache using GetCellInfo 
	iPhone.GetCellInfo(requestStatus,cellInfoPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo.iTimingAdvance);
	
	// Register again and change the cell info
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    cellInfoComplete.iCellId = 442;
    cellInfoComplete.iTimingAdvance = 74;
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo.iTimingAdvance);
	
	// Change the cell info and then register notification
    cellInfoComplete.iCellId = 121;
    cellInfoComplete.iTimingAdvance = 4;
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo.iTimingAdvance);
	
	// register notification and then diable the cache
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);
    ASSERT_EQUALS(KRequestPending, requestStatus.Int());
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp,KErrNotSupported);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	// register again and make sure that we get an error
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	// enable the cache and check notification again
	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp,KErrNone);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
	User::After(KOneSecond);
	ASSERT_EQUALS(KRequestPending, requestStatus.Int());
    cellInfoComplete.iCellId = 12;
    cellInfoComplete.iTimingAdvance = 5;
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo.iTimingAdvance);
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::NotifyCellInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrGeneral);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-PNCIC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyCellInfoChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyCellInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCellInfoChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
	TPckg<RMobilePhone::TMobilePhoneCellInfoV9> cellInfoPckg(cellInfo);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyCellInfoChange
 	//-------------------------------------------------------------------------
 	

	iMockLTSY.ExpectL(ECtsyPhoneCellInfoIndReq);
	iPhone.NotifyCellInfoChange(requestStatus, cellInfoPckg);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp, KErrNone);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	User::WaitForRequest(mockLtsyStatus);

	iPhone.CancelAsyncRequest(EMobilePhoneNotifyCellInfoChange);
	
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-NTWC-PNCIC-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyCellInfoChange with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyCellInfoChange with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCellInfoChange0003L()
	{

// This test should test sending bad parameter data for NotifyCellInfoChange
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RMobilePhone::NotifyCellInfoChange
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneNetworkInfoV5 networkInfov5;
	TPckg< RMobilePhone::TMobilePhoneNetworkInfoV5 > networkInfov5Pckg ( networkInfov5);
	iPhone.NotifyCellInfoChange(requestStatus, networkInfov5Pckg);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::NotifyCellInfoChange
 	//-------------------------------------------------------------------------

	TBuf8<1> smallDes;

	iPhone.NotifyCellInfoChange(requestStatus, smallDes);
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(1, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-NTWC-PNCIC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyCellInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyCellInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyNetworkControlFU::TestNotifyCellInfoChange0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;

    RMobilePhone::TMobilePhoneCellInfoV9 cellInfoComplete;
    cellInfoComplete.iMode = RMobilePhone::ECellModeGsm;
    cellInfoComplete.iCellId = 35;
    cellInfoComplete.iTimingAdvance = 44;
	
	TMockLtsyData1<RMobilePhone::TMobilePhoneCellInfoV9> mockData2(cellInfoComplete);
	
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo1;
	TPckg<RMobilePhone::TMobilePhoneCellInfoV9> cellInfoPckg1(cellInfo1);
	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo2;
	TPckg<RMobilePhone::TMobilePhoneCellInfoV9> cellInfoPckg2(cellInfo2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyCellInfoChange
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(ECtsyPhoneCellInfoIndReq);

	iPhone.NotifyCellInfoChange(requestStatus1, cellInfoPckg1);

	phone2.NotifyCellInfoChange(requestStatus2, cellInfoPckg2);

	iMockLTSY.CompleteL(ECtsyPhoneCellInfoIndComp,KErrNone);
    ASSERT_EQUALS(KRequestPending, requestStatus1.Int());
    ASSERT_EQUALS(KRequestPending, requestStatus2.Int());
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    User::WaitForRequest(mockLtsyStatus);
    
    ASSERT_EQUALS(KRequestPending, requestStatus1.Int());
    ASSERT_EQUALS(KRequestPending, requestStatus2.Int());
	mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(ECtsyPhoneCellInfoComp, KErrNone, data);
	
	

	
	User::WaitForRequest(requestStatus1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo1.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo1.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo1.iTimingAdvance);
	ASSERT_EQUALS(cellInfoComplete.iMode, cellInfo2.iMode);
	ASSERT_EQUALS(cellInfoComplete.iCellId, cellInfo2.iCellId);
	ASSERT_EQUALS(cellInfoComplete.iTimingAdvance, cellInfo2.iTimingAdvance);

	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}
