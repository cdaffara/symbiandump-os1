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

#include "cctsyucasembms007.h"
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

CTestSuite* CCTsyUCaseMbms007::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;  
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms007, CCtsyIntegrationTestMbms0001L ); 
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0013
 * @SYMFssID BA/CTSY/PKTS-0013
 * @SYMTestCaseDesc Support being notified when MBMS network service availability changes to unavailable.
 * @SYMTestPriority High
 * @SYMTestActions  RPacketService::NotifyMbmsNetworkServiceStatusChange
 * @SYMTestExpectedResults Pass - MBMS network availability status is returned correctly.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that NotifyMbmsNetworkServiceStatusChange completes correctly with correct MBMS network status.
 *
 * @return - none
 */
void CCTsyUCaseMbms007::CCtsyIntegrationTestMbms0001L()
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
   		
    // Ensure packet service status is RPacketService::EStatusAttached.
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);
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
    
	TMbmsNetworkServiceStatus sndData = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData2(sndData);
	data.Close();
	ltsyData2.SerialiseL(data);		
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
    
	// Ensure packet service status is RPacketService::EStatusUnattached
   	sendStatus = RPacketService::EStatusUnattached;
	isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData3(sendStatus, isResumed);
	ltsyData3.SerialiseL(data);
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
		 
	TMbmsNetworkServiceStatus sndData2 = EMbmsSupportUnknown;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData4(sndData2);
	data.Close();
	ltsyData4.SerialiseL(data);		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
    // Ensure RPacketService::NotifyMbmsNetworkServiceStatusChange completes with status EMbmsSupportUnknown.
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    ASSERT_TRUE(regStatuscomplete == regStatus);
	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // packetMbmsContext, packetService, data, data2, this 
    }
    
