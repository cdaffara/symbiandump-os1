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

#include "cctsyucasembms017.h"
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

CTestSuite* CCTsyUCaseMbms017::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms017, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms017, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0032
 * @SYMFssID BA/CTSY/MB-0032
 * @SYMTestCaseDesc MBMS local context Activation
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - MBMS context actives properly.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Context activation sequence.
 *
 * @return - none
 */
void CCTsyUCaseMbms017::CCtsyIntegrationTestMbms0001L()
    {
        
	//
	// SET UP
	//
    TInt maxSessions( 5 );
    	        
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 data;
    CleanupClosePushL(data);    	        
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
			
	RPacketService packetService;
	OpenPacketServiceL( packetService,
	                    RPacketService::EStatusUnattached, 
	                    RPacketService::EAttachWhenNeeded,
	                    maxSessions,
	                    maxSessions );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus requestStatus;
    RPacketContext::TContextStatus contextStatus; 
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
		
	// Initialise context				
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);	
	ASSERT_EQUALS( KErrNone, status );
	
	// set configuration	
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
	
	// verify configuration			
	ASSERT_EQUALS( setConfig.iMbmsAccessBearer, getConfig.iMbmsAccessBearer );
	ASSERT_EQUALS( setConfig.iMbmsServiceMode, getConfig.iMbmsServiceMode );
	ASSERT_EQUALS( setConfig.iMbmsServicePriority, getConfig.iMbmsServicePriority );
	ASSERT_EQUALS( setConfig.iMbmsSessionFlag, getConfig.iMbmsSessionFlag );		   	    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
    
	// status -> RPacketContext::EStatusInactive
	packetMbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS( RPacketContext::EStatusInactive, contextStatus );      
      
 	TMockLtsyData1 <TInfoName > expData( contextId );
	expData.SerialiseL( expectData );
	        	
  	// start to activate context
  	RPacketMbmsContext::TContextConfigMbmsV1 config;
	TMockLtsyData2< TInfoName, RPacketMbmsContext::TContextConfigMbmsV1 > ltsyData1(contextId, config);
	ltsyData1.SerialiseL(data);	
	
 	iMockLTSY.ExpectL( EPacketContextActivate, expectData );
 	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);

    // activate context
	packetMbmsContext.Activate( requestStatus );
	User::WaitForRequest( requestStatus );  
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );		
	
	// status -> RPacketContext::EStatusActive
	packetMbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS( RPacketContext::EStatusActive, contextStatus );
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
    }
    
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0033
 * @SYMFssID BA/CTSY/MB-0033
 * @SYMTestCaseDesc MBMS local context Activation with max number of contexts
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Activation of MBMS context exceeds max active MBMS services supported by stack.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Activate too many context 
 *
 * @return - none
 */
void CCTsyUCaseMbms017::CCtsyIntegrationTestMbms0002L()
    {
        
	//
	// SET UP
	//
    TInt maxSessions( 5 );
    	        
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 data;
    CleanupClosePushL(data);    	        
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
			
	RPacketService packetService;
	OpenPacketServiceL( packetService,
	                    RPacketService::EStatusUnattached, 
	                    RPacketService::EAttachWhenNeeded,
	                    maxSessions,
	                    maxSessions );
	CleanupClosePushL( packetService );
	
	// Open new context
	TInfoName contextId, extraId;	
	TInfoName hostCid;
	RPacketMbmsContext packetMbmsContext, extraMbmsContext;	
	packetMbmsContext.OpenNewContext( packetService, contextId );	
	CleanupClosePushL( packetMbmsContext );
	
	TRequestStatus requestStatus;
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
			
    // initialise context				
	TInt status = InitMbmsContextL(packetMbmsContext, contextId);	
	ASSERT_EQUALS( KErrNone, status );
	
	// set configuration	
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
    setConfig.iMbmsAccessBearer = E2GBearer;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsServicePriority = EMbmsLowServicePriority;
    setConfig.iMbmsSessionFlag = ETrue;
    
    status = SetMbmsConfigL( packetMbmsContext, contextId, setConfig );
    ASSERT_EQUALS( KErrNone, status );
       
	// context activation           	        	  
  	RPacketMbmsContext::TContextConfigMbmsV1 config;	
	status = SetMbmsActiveL( packetMbmsContext, config, contextId );
	ASSERT_EQUALS( KErrNone, status );		
	
	// insert max number of session to context
 	TMockLtsyData1 <TInfoName > expData( contextId );
	expData.SerialiseL( expectData );
	
 	TMockLtsyData1 <TInfoName> outData( contextId );
	outData.SerialiseL( data );      
    		
    for( TInt i = 0; i < maxSessions; i++ )
        {
        TMbmsSessionId session(i);  
        
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
        packetMbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
        
        User::WaitForRequest( requestStatus );
        ASSERT_EQUALS( KErrNone, requestStatus.Int() ) ;
        } 	
    
    // try to add one more new context ie. active MBMS service 
	extraMbmsContext.OpenNewContext( packetService, extraId );   
	
	// init new context
	status = InitMbmsContextL(extraMbmsContext, extraId);	
	ASSERT_EQUALS( KErrNone, status ); 
	
	// config context				
    status = SetMbmsConfigL( extraMbmsContext, extraId, setConfig );    
    ASSERT_EQUALS( KErrNone, status );					
    
	// activate extra context	
	status = SetMbmsActiveL( extraMbmsContext, config, extraId );
	ASSERT_EQUALS( KErrUmtsMaxNumOfContextExceededByPhone, status );	
					
	//
	// TEST END
	//	
		 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
      }
    
