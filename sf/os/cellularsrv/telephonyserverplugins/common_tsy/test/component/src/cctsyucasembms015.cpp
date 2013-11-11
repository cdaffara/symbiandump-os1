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
// cctsyucasembms013.cpp
// 
//

/**
 @file The TEFUnit integration test suite for MBMS Context support in the Common TSY.
*/

#include "cctsyucasembms015.h"
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

CTestSuite* CCTsyUCaseMbms015::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms015, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms015, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0027
 * @SYMFssID BA/CTSY/MB-0027
 * @SYMTestCaseDesc MBMS local context Configuration and no Sessions used
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Context is properly configured
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test context configuration .
 *
 * @return - none
 */
void CCTsyUCaseMbms015::CCtsyIntegrationTestMbms0001L()
    {
        
	//
	// SET UP
	//
	        
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 data;
    CleanupClosePushL(data);    	        
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
			
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus requestStatus;

    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	// The Client requests Open New MBMS context from ETEL API.
    // Open New MBMS context from CTSY with context name "mbms".
    // CTSY reserve resources for MBMS context.
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);	
	ASSERT_EQUALS( KErrNone, status );
	
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
    setConfig.iMbmsAccessBearer = E2GBearer;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsServicePriority = EMbmsLowServicePriority;
    setConfig.iMbmsSessionFlag = ETrue;
    
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(setConfig); 	
	
	TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);
    	
    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
        			
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);
    
    packetMbmsContext.SetConfig(requestStatus, pckgConfig);
    
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();    
    
    // get configuration
	RPacketMbmsContext::TContextConfigMbmsV1 getConfig;
	TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckg2Config(getConfig);     
	
	packetMbmsContext.GetConfig(requestStatus, pckg2Config);	
	User::WaitForRequest(requestStatus);	
		
	ASSERT_EQUALS( setConfig.iMbmsAccessBearer, getConfig.iMbmsAccessBearer );
	ASSERT_EQUALS( setConfig.iMbmsServiceMode, getConfig.iMbmsServiceMode );
	ASSERT_EQUALS( setConfig.iMbmsServicePriority, getConfig.iMbmsServicePriority );
	ASSERT_EQUALS( setConfig.iMbmsSessionFlag, getConfig.iMbmsSessionFlag );		   	    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
  	
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
    }

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0028
 * @SYMFssID BA/CTSY/MB-0028
 * @SYMTestCaseDesc MBMS local context Configuration fails
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - CTSY completes error back to client
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test context configuration with error.
 *
 * @return - none
 */
void CCTsyUCaseMbms015::CCtsyIntegrationTestMbms0002L()
    {
        
	//
	// SET UP
	//
	        
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 data;
    CleanupClosePushL(data);    	        
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
			
	RPacketService packetService;
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus requestStatus;

    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	
	// ===  Basic test flow ===
	// The Client requests Open New MBMS context from ETEL API.
    // ETEL API requests Open New MBMS context from CTSY with context name "mbms".	
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);	
	ASSERT_EQUALS( KErrNone, status );
	
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
    setConfig.iMbmsAccessBearer = E2GBearer;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsServicePriority = EMbmsLowServicePriority;
    setConfig.iMbmsSessionFlag = ETrue;
    
    TPckg<RPacketMbmsContext::TContextConfigMbmsV1> pckgConfig(setConfig); 	
	
	TMockLtsyData2<RPacketMbmsContext::TContextConfigMbmsV1, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);
    	
    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
        			
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrGeneral, data);
    // SetConfig completes with an error
    packetMbmsContext.SetConfig(requestStatus, pckgConfig);
    
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();    
    				   	    
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());  
  	
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
     }
    
