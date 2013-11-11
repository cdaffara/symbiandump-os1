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

#include "cctsyucasembms014.h"
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

CTestSuite* CCTsyUCaseMbms014::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms014, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms014, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0025
 * @SYMFssID BA/CTSY/INT/MB/0025
 * @SYMTestCaseDesc MBMS local context Initialisation
 * @SYMTestPriority High
 * @SYMTestActions  
 * @SYMTestExpectedResults Pass - Initialisation is successful.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test context Initialisation.
 *
 * @return - none
 */
void CCTsyUCaseMbms014::CCtsyIntegrationTestMbms0001L()
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
	
	
	// The Client requests Open New MBMS context from ETEL API.
	TInfoName contextId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );

    // Ensure a proper name is gevien to mbms context
    ASSERT_EQUALS( 0, contextId.Match(_L("MBMS_Context_0")));
	RBuf8 data;
	CleanupClosePushL(data);

	// data for initialisation of the context 
	TRequestStatus initStatus;
	RPacketMbmsContext::TDataChannelV2 dataCh;
	
	TPckg<RPacketMbmsContext::TDataChannelV2> pckgInit(dataCh);	
    TMockLtsyData2< TInfoName, RPacketMbmsContext::TDataChannelV2 > ltsyInit(contextId, dataCh);
    
	ltsyInit.SerialiseL(data);	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, data);
	
	TMockLtsyData1< TInfoName > ltsyData(contextId);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data);
	//initialisation of the context
	packetMbmsContext.InitialiseContext(initStatus, pckgInit);
	User::WaitForRequest(initStatus);
	
	ASSERT_EQUALS( KETelExtPcktV2, dataCh.ExtensionId());
	ASSERT_EQUALS( KErrNone, initStatus.Int());
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 4 ); //packetMbmsContext, packetService, data, this
    }
    
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0026
 * @SYMFssID BA/CTSY/INT/MB/0026
 * @SYMTestCaseDesc MBMS local context Initialisation with LTSY error
 * @SYMTestPriority High
 * @SYMTestActions  
 * @SYMTestExpectedResults Pass - CTSY completes with error  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test context initialisation when LTSY fails on init
 *
 * @return - none
 */
void CCTsyUCaseMbms014::CCtsyIntegrationTestMbms0002L()
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


	// The Client requests Open New MBMS context from ETEL API.
	TInfoName contextId;	
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );

    // Ensure a proper name is gevien to mbms context
    ASSERT_EQUALS( 0, contextId.Match(_L("MBMS_Context_0")));
	RBuf8 data;
	CleanupClosePushL(data);
	
	// data for initialisation of the context 
	TRequestStatus initStatus;
	RPacketMbmsContext::TDataChannelV2 dataCh;
	TPckg<RPacketMbmsContext::TDataChannelV2> pckgInit(dataCh);	
    TMockLtsyData2< TInfoName, RPacketMbmsContext::TDataChannelV2 > ltsyInit(contextId, dataCh);
    
	ltsyInit.SerialiseL(data);	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, data);
	
	TMockLtsyData1< TInfoName > ltsyData(contextId);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrGeneral, data);
	//initialisation of the context
	packetMbmsContext.InitialiseContext(initStatus, pckgInit);
	User::WaitForRequest(initStatus);
	
	ASSERT_EQUALS( KETelExtPcktV2, dataCh.ExtensionId());
	ASSERT_EQUALS( KErrGeneral, initStatus.Int());
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 4 ); //packetMbmsContext, packetService, data, this
     }
    
