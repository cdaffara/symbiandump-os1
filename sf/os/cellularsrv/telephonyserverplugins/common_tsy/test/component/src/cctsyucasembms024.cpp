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

#include "cctsyucasembms024.h"
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

CTestSuite* CCTsyUCaseMbms024::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms024, CCtsyIntegrationTestMbms0001L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0042
 * @SYMFssID BA/CTSY/MB-0042
 * @SYMTestCaseDesc MBMS local context Closure.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - MBMS context is Closed. 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS context is closed successfully.
 *
 * @return - none
 */
void CCTsyUCaseMbms024::CCtsyIntegrationTestMbms0001L()
    {
    
    //
	// SET UP
	//
   	        
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
			
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 data;
    CleanupClosePushL(data);    	        
	
	// Open new context
	TInfoName contextId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	// post RPacketMbmsContext::NotifyStatusChange
	TRequestStatus requestStatus;
    RPacketContext::TContextStatus contextStatus; 
	packetMbmsContext.NotifyStatusChange(requestStatus, contextStatus);
	
    TInt status = InitMbmsContextL(packetMbmsContext, contextId);	
	ASSERT_EQUALS( KErrNone, status );
	
	// Ensure RPacketContext::NotifyStatusChange completes MBMS context status RPacketContext::EStatusInactive
    User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusInactive, contextStatus);	
  	
  	AssertMockLtsyStatusL();    
    
    // Ensure RPacketContext::GetStatus returns status RPacketContext::EStatusInactive
	packetMbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS( RPacketContext::EStatusInactive, contextStatus );     
	
	// set configuration
	RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
    setConfig.iMbmsAccessBearer = E2GBearer;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsServicePriority = EMbmsLowServicePriority;
    setConfig.iMbmsSessionFlag = EFalse;
    
    status = SetMbmsConfigL(packetMbmsContext,contextId, setConfig);
  	ASSERT_EQUALS( KErrNone, status );

 	TMockLtsyData1 <TInfoName > expData( contextId );
	expData.SerialiseL( expectData );
	
	// re-post notify
	packetMbmsContext.NotifyStatusChange(requestStatus, contextStatus);
        	
  	// activate context
  	status = SetMbmsActiveL(packetMbmsContext, setConfig, contextId );
	ASSERT_EQUALS( KErrNone, status );
 	
 	// Ensure RPacketContext::NotifyStatusChange completes MBMS context status RPacketContext::EStatusActivating
    User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActivating, contextStatus);	
 	
 	// Ensure RPacketContext::NotifyStatusChange completes MBMS context status RPacketContext::EStatusActive
 	packetMbmsContext.NotifyStatusChange(requestStatus, contextStatus);
 	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusActive, contextStatus);
	
	// Ensure RPacketContext::GetStatus returns status RPacketContext::EStatusActive
	packetMbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS( RPacketContext::EStatusActive, contextStatus );
	
	// Ensure MBMS context count is greater than 0
	TInt count;
	TInt maxAllowed;
	
	packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 
    User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_TRUE(0 < count);
	
	// re-post notify
	packetMbmsContext.NotifyStatusChange(requestStatus, contextStatus);
	// Deactivate context
	status = SetMbmsDeactiveL(packetMbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// Ensure RPacketContext::GetStatus returns status RPacketContext::EStatusDeactivating
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusDeactivating, contextStatus);	
	
	// re-post notify
	packetMbmsContext.NotifyStatusChange(requestStatus, contextStatus);
	
	// Ensure RPacketContext::NotifyStatusChange completes MBMS context status RPacketContext::EStatusInactive
    User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EStatusInactive, contextStatus);	
 	
	// Ensure RPacketContext::GetStatus returns status RPacketContext::EStatusInactive
	packetMbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS( RPacketContext::EStatusInactive, contextStatus );
	
	
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	// The Client requests Close MBMS context from ETEL API.
	packetMbmsContext.Close( );
    
	// Ensure MBMS context count is  0
	packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 
    		
    User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(0 , count);
		
    					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); // packetMbmsContext, packetService, expectData, data, this
   }
    
