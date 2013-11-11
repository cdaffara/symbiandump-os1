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

#include "cctsyucasembms003.h"
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

CTestSuite* CCTsyUCaseMbms003::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;  
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms003, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms003, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0005
 * @SYMFssID BA/CTSY/PKTS-0005
 * @SYMTestCaseDesc Support getting current MBMS network availability with CTSY cache clean
 * @SYMTestPriority High
 * @SYMTestActions RPacketService:: GetMbmsNetworkServiceStatus
 * @SYMTestExpectedResults Pass - MBMS network availability status is returned correctly.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS network availability status is returned correctly.
 *
 * @return - none
 */
void CCTsyUCaseMbms003::CCtsyIntegrationTestMbms0001L()
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
    iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data2);
    
	// The Client requests MBMS service status via RPacketService:: GetMbmsNetworkServiceStatus with aAttemptAttach = FALSE.
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
	
	// Ensure RPacketService:: GetMbmsNetworkServiceStatus returns EMbmsSupportUnknown
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
 * @SYMTestCaseID BA-CTSY-INT-MB-0006
 * @SYMFssID BA/CTSY/PKTS-0006
 * @SYMTestCaseDesc Support getting current MBMS network availability with CTSY cache clean
 * @SYMTestPriority High
 * @SYMTestActions RPacketService:: GetMbmsNetworkServiceStatus
 * @SYMTestExpectedResults Pass - MBMS network availability status failed with an error and CTSY completes request with an error.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS network availability status is returned correctly.
 *
 * @return - none
 */
void CCTsyUCaseMbms003::CCtsyIntegrationTestMbms0002L()
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
    iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNotSupported);

	// The Client requests MBMS service status via RPacketService:: GetMbmsNetworkServiceStatus
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
	
    // Ensure RPacketService::GetMbmsNetworkServiceStatus returns EMbmsSupportUnknown
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNotSupported, requestStatus.Int() );   	
	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2, this 
    } 
    
