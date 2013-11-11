// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file The TEFUnit integration test suite for MBMS Context support in the Common TSY.
*/

#include "cctsyucasembms006.h"
#include "cctsyactiveretriever.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

#include <pcktcs.h>
#include <pcktlist.h>
#include "pcktretrieve.h"
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>

CTestSuite* CCTsyUCaseMbms006::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;  
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms006, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms006, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0011
 * @SYMFssID BA/CTSY/PKTS-0011
 * @SYMTestCaseDesc Support being notified when moving to a location area not supporting MBMS.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMbmsNetworkServiceStatusChange
 * @SYMTestExpectedResults Pass - MBMS network availability status is returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that NotifyMbmsNetworkServiceStatusChange completes correctly with correct MBMS network status.
 *
 * @return - none
 */
void CCTsyUCaseMbms006::CCtsyIntegrationTestMbms0001L()
    {
        
	//
	// SET UP
	//
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
				
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
 
 	// Open new context             
	TInfoName contextId;
    RPacketMbmsContext packetMbmsContext;
    packetMbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( packetMbmsContext );
   
   
   	TRequestStatus requestStatus;
   	TRequestStatus requestStatus2;
	TMbmsNetworkServiceStatus regStatus;
   	TRequestStatus mockLtsyStatus;
   	TMbmsNetworkServiceStatus regStatuscomplete = EMbmsSupported; 	
   
    // Check RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork
  	RPacketService::TRegistrationStatus regStatus3;
  	TRequestStatus requestStatus3;
    RPacketService::TRegistrationStatus reqStatusComplete3 = RPacketService::ERegisteredOnHomeNetwork;
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData(reqStatusComplete3);
	ltsyData.SerialiseL(data2);
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data2);
	regStatus3 = RPacketService::TRegistrationStatus(0);

	packetService.GetNtwkRegStatus(requestStatus3, regStatus3);
	
	User::WaitForRequest(requestStatus3);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());
	ASSERT_TRUE(reqStatusComplete3 == regStatus3);
			
    // Ensure packet service status is RPacketService::EStatusAttached.
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData2(sendStatus, isResumed);
	ltsyData2.SerialiseL(data);
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);	

	// post RPacketService::NotifyMbmsNetworkServiceStatusChange
    packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, regStatus);
    
	TMbmsNetworkServiceStatus sndData3 = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData3(sndData3);
	data.Close();
	ltsyData3.SerialiseL(data);		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
    
    // Ensure RPacketService::NotifyMbmsNetworkServiceStatusChange completes with status EMbmsSupported.
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    ASSERT_TRUE(regStatuscomplete == regStatus);
	 
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//	
	
	regStatuscomplete = EMbmsSupportUnknown; 
			
	// post RPacketService::NotifyMbmsNetworkServiceStatusChange	
    packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, regStatus);
     	
	// User moves to different location area that does not support MBMS.
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
	
	// Ensure RPacketService::GetStaticCaps returns without caps KCapsMBMSSupported.
   	TUint staticCaps = 0;
   	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
						    RPacketService::KCapsMSClassSupported |
							RPacketService::KCapsNotifyMSClassSupported;
									
	TMockLtsyData1<TUint> ltsyData4(expectedCaps);
	data2.Close();
	ltsyData4.SerialiseL(data2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL( EPacketGetStaticCaps, KErrNone, data2 );
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			   		
   	TInt ret = packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4);
 	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, ret);	
	
    regStatuscomplete = EMbmsNotSupported;
    TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData6(regStatuscomplete);
	ltsyData6.SerialiseL(data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());			

    // Ensure RPacketService::NotifyMbmsNetworkServiceStatusChange completes with status EMbmsNotSupported
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_TRUE(regStatuscomplete == regStatus);	
		
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2 this 
    }
    
    /**
 * @SYMTestCaseID BA-CTSY-INT-MB-0012
 * @SYMFssID BA/CTSY/PKTS-0012
 * @SYMTestCaseDesc Support being notified when moving to a location area not supporting MBMS.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMbmsNetworkServiceStatusChange
 * @SYMTestExpectedResults Pass - MBMS network availability status is returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test:  Verify that NotifyMbmsNetworkServiceStatusChange completes correctly with correct MBMS network status.
 *
 * @return - none
 */
void CCTsyUCaseMbms006::CCtsyIntegrationTestMbms0002L()
    {
        
	//
	// SET UP
	//
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
				
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
    
    // Open new context             
	TInfoName contextId;
    RPacketMbmsContext packetMbmsContext;
    packetMbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( packetMbmsContext );
   
   
   	TRequestStatus requestStatus;
   	TRequestStatus requestStatus2;
	TMbmsNetworkServiceStatus regStatus;
   	TRequestStatus mockLtsyStatus;
   	TMbmsNetworkServiceStatus regStatuscomplete = EMbmsSupported; 	
   
    // Check RMobilePhone::GetNetworkRegistrationStatus returns ERegisteredOnHomeNetwork
  	RPacketService::TRegistrationStatus regStatus3;
  	TRequestStatus requestStatus3;
    RPacketService::TRegistrationStatus reqStatusComplete3 = RPacketService::ERegisteredOnHomeNetwork;
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData(reqStatusComplete3);
	ltsyData.SerialiseL(data2);
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data2);
	regStatus3 = RPacketService::TRegistrationStatus(0);

	packetService.GetNtwkRegStatus(requestStatus3, regStatus3);
	
	User::WaitForRequest(requestStatus3);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());
	ASSERT_TRUE(reqStatusComplete3 == regStatus3);
			
    // Ensure packet service status is RPacketService::EStatusAttached.
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData2(sendStatus, isResumed);
	ltsyData2.SerialiseL(data);
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);	

	// post RPacketService::NotifyMbmsNetworkServiceStatusChange
    packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, regStatus);

	TMbmsNetworkServiceStatus sndData3 = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData3(sndData3);
	data.Close();
	ltsyData3.SerialiseL(data);		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
    
    // Ensure RPacketService::NotifyMbmsNetworkServiceStatusChange completes with status EMbmsSupported.
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    ASSERT_TRUE(regStatuscomplete == regStatus);
	 
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//	
	

	regStatuscomplete = EMbmsSupportUnknown; 
			
	// post RPacketService::NotifyMbmsNetworkServiceStatusChange	
    packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus3, regStatus);
     	
    // User moves to different location area that does not support MBMS.
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
	
	// Ensure RPacketService::GetStaticCaps returns without caps KCapsMBMSSupported.	
   	TUint staticCaps = 0;
   	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
						    RPacketService::KCapsMSClassSupported |
							RPacketService::KCapsNotifyMSClassSupported;
									
	TMockLtsyData1<TUint> ltsyData4(expectedCaps);
	data2.Close();
	ltsyData4.SerialiseL(data2);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL( EPacketGetStaticCaps, KErrNone, data2 );
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			   		
   	TInt ret = packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4);
 	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, ret);	
			
    regStatuscomplete = EMbmsNotSupported;
    TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData6(regStatuscomplete);
	ltsyData6.SerialiseL(data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNotSupported);
		
    // Ensure RPacketService::NotifyMbmsNetworkServiceStatusChange completes with status EMbmsNotSupported
	User::WaitForRequest(requestStatus3);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus3.Int());	

	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2 this 
     }
    
