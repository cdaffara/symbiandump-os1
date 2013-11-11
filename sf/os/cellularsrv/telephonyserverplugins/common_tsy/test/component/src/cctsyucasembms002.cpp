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

#include "cctsyucasembms002.h"
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

CTestSuite* CCTsyUCaseMbms002::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;   
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms002, CCtsyIntegrationTestMbms0001L );  
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms002, CCtsyIntegrationTestMbms0002L ); 
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms002, CCtsyIntegrationTestMbms0003L );  
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0002
 * @SYMFssID BA/CTSY/PKTS-0002
 * @SYMTestCaseDesc Support getting current MBMS network availability with attempt to GMM Attach if required.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService:: GetMbmsNetworkServiceStatus
 * @SYMTestExpectedResults Pass - MBMS network availability status is returned correctly.  Status can be Supported, Not Supported.or Unknown..
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS network availability status is returned correctly
 *
 * @return - none
 */
void CCTsyUCaseMbms002::CCtsyIntegrationTestMbms0001L()
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
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	TRequestStatus requestStatus;
    TBool aAttemptAttach = ETrue;
    TMbmsNetworkServiceStatus regStatus;
	
	TMockLtsyData1<TBool> ltsyData2(aAttemptAttach);
	ltsyData2.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data);	

    TMbmsNetworkServiceStatus regStatusComplete = EMbmsSupported;
    TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData3(regStatusComplete);
	ltsyData3.SerialiseL(data2);

    // Client request PacketService::GetMbmsNetworkServiceStatus
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
		
    // Ensure packet service status is RPacketService::EStatusAttached. 
   	TRequestStatus mockLtsyStatus;	
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);
		
	TRequestStatus requestStatus2;
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);	
	
	// Ensure that PacketService::GetMbmsNetworkServiceStatus returns status EMbmsSupported		
    iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data2);
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_TRUE(regStatusComplete == regStatus);
	
	// Ensure packet service status is RPacketService::EStatusUnattached
	sendStatus = RPacketService::EStatusUnattached;
	isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData4(sendStatus, isResumed);
	ltsyData4.SerialiseL(data);

	packetService.NotifyStatusChange(requestStatus2, contextStatus);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);

	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);			
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2, this 
    }
 

    /**
 * @SYMTestCaseID BA-CTSY-INT-MB-0003
 * @SYMFssID BA/CTSY/PKTS-0003
 * @SYMTestCaseDesc Support getting current MBMS network availability with attempt to GMM Attach if required.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService:: GetMbmsNetworkServiceStatus
 * @SYMTestExpectedResults Pass - MBMS network availability status is returned correctly.  Status can be Supported, Not Supported.or Unknown..
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS network availability status is returned correctly.
 *
 * @return - none
 */
void CCTsyUCaseMbms002::CCtsyIntegrationTestMbms0002L()
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
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	
	TRequestStatus requestStatus;
    TBool aAttemptAttach = ETrue;
    TMbmsNetworkServiceStatus regStatus;
	
    TMockLtsyData1<TBool> ltsyData(aAttemptAttach);
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data);	
    
    TMbmsNetworkServiceStatus regStatusComplete = EMbmsSupportUnknown;
    TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData2(regStatusComplete);
	ltsyData2.SerialiseL(data2);

    // Client request PacketService::GetMbmsNetworkServiceStatus
	// Ensure that PacketService::GetMbmsNetworkServiceStatus returns status EMbmsSupportUnknown because attemp to attach has failed.
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
    iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data2);
    
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_TRUE(regStatusComplete == regStatus);
	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2, this 
    }
    
    /**
 * @SYMTestCaseID BA-CTSY-INT-MB-0004
 * @SYMFssID BA/CTSY/PKTS-0004
 * @SYMTestCaseDesc Support getting current MBMS network availability with attempt to GMM Attach if required.
 * @SYMTestPriority High
 * @SYMTestActions  RPacketService:: GetMbmsNetworkServiceStatus
 * @SYMTestExpectedResults Pass - MBMS network availability status failed with an error and CTSY completes request with an error.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS network availability status is returned correctly.
 *
 * @return - none
 */
void CCTsyUCaseMbms002::CCtsyIntegrationTestMbms0003L()
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
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	TRequestStatus requestStatus;
    TBool aAttemptAttach = ETrue;
    TMbmsNetworkServiceStatus regStatus;
	
	TMockLtsyData1<TBool> ltsyData(aAttemptAttach);
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data);	
	
	TMbmsNetworkServiceStatus regStatusComplete = EMbmsSupported;
    TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData2(regStatusComplete);
	ltsyData2.SerialiseL(data2);

    // Client request PacketService::GetMbmsNetworkServiceStatus
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
	
    // Ensure packet service status is RPacketService::EStatusAttached. 
    TRequestStatus mockLtsyStatus;	
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData3(sendStatus, isResumed);
	ltsyData3.SerialiseL(data);
		
	TRequestStatus requestStatus2;
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); 
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);	

	// Ensure that PacketService::GetMbmsNetworkServiceStatus returns error	 
    iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrGeneral);
        
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrGeneral, requestStatus.Int() );   
		
	// Ensure packet service status is RPacketService::EStatusUnattached
	sendStatus = RPacketService::EStatusUnattached;
	isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData4(sendStatus, isResumed);
	ltsyData4.SerialiseL(data);

	packetService.NotifyStatusChange(requestStatus2, contextStatus);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);

	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);

	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2, this 
    } 
    
