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

#include "cctsyucasembms013.h"
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

CTestSuite* CCTsyUCaseMbms013::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms013, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms013, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0023
 * @SYMFssID: BA/CTSY/INT/MB/0023
 * @SYMTestCaseDesc MBMS local context Creation
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - New context is created.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Opening succesfully context.
 *
 * @return - none
 */
void CCTsyUCaseMbms013::CCtsyIntegrationTestMbms0001L()
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
	
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	// Client requests Open New MBMS context from ETEL API.
	TInfoName contextId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );

    // Ensure a proper name is gevien to mbms context
    ASSERT_EQUALS( 0, contextId.Match(_L("MBMS_Context_0")));
	
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 3 ); //packetMbmsContext, packetService, this
    }

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0024
 * @SYMFssID: BA/CTSY/INT/MB/0024
 * @SYMTestCaseDesc Creating too many MBMS local context 
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Error KErrMaxExceeded raised.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Opening too many contexts.
 *
 * @return - none
 */
void CCTsyUCaseMbms013::CCtsyIntegrationTestMbms0002L()
    {
        
	//
	// SET UP
	//
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
   
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//


    // If max number of MBMS context exceeded, CTSY completes Open 
    // New MBMS context with KErrMaxExceeded
    TInt status, maxSessions( 10 );
    
    RPacketService packetService;
	OpenPacketServiceL( packetService,
	                    RPacketService::EStatusUnattached, 
	                    RPacketService::EAttachWhenNeeded,
	                    maxSessions,
	                    maxSessions );   
    CleanupClosePushL( packetService );	                     
    
	// ===  Check context and service statuses === 
	// Open new context
	TInfoName contextId;	
	TInfoName hostCid;
	
	for( TInt i = 0; i < maxSessions; i++ )
	    {
    	RPacketMbmsContext packetMbmsContext;	
    	packetMbmsContext.OpenNewContext( packetService, contextId );	
    	CleanupClosePushL( packetMbmsContext );	    
	    }
    // max number of contexts exceed, add one; 
	RPacketMbmsContext maxMbmsContext;	
    status = maxMbmsContext.OpenNewContext( packetService, contextId );	
    CleanupClosePushL( maxMbmsContext );	 
       
	ASSERT_EQUALS( KErrOverflow, status );
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy( maxSessions ); //maxSessions 
	CleanupStack::PopAndDestroy( 3 ); //packetService, maxMbmsContext, this
     }
    
