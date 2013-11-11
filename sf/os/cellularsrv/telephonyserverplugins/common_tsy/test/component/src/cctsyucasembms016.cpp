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

#include "cctsyucasembms016.h"
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

CTestSuite* CCTsyUCaseMbms016::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms016, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms016, CCtsyIntegrationTestMbms0002L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms016, CCtsyIntegrationTestMbms0003L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0029
 * @SYMFssID BA/CTSY/MB-0029
 * @SYMTestCaseDesc MBMS local context Configuration when Sessions used
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - The Client requests to add Session to the list of services for receivng using RPacketContext:: UpdateMbmsSessionList  where aAction=EAddService.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Context configuration with sessions.
 *
 * @return - none
 */
void CCTsyUCaseMbms016::CCtsyIntegrationTestMbms0001L()
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
    
    				
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
    }
    
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0030
 * @SYMFssID BA/CTSY/MB-0030
 * @SYMTestCaseDesc MBMS local context Configuration when Sessions used when max number of session exceed
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - The Client requests to add Session and get error code KErrOverflow
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Client gets error code KErrOverflow
 *
 * @return - none
 */
void CCTsyUCaseMbms016::CCtsyIntegrationTestMbms0002L()
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
    
    // activate context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);    
    
 	TMockLtsyData1 <TInfoName > expData( contextId );
	expData.SerialiseL( expectData );
	
 	TMockLtsyData1 <TInfoName> outData( contextId );
	outData.SerialiseL( data );      
    
    // add maximum number of sessions    
    for( TInt i = 0; i < maxSessions; i++ )
        {
        TMbmsSessionId session(i);  
        
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
        packetMbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
        
        User::WaitForRequest( requestStatus );
        ASSERT_EQUALS( KErrNone, requestStatus.Int() ) ;
        }    
  	
  	// and one to cause KErrOverflow
    TMbmsSessionId extraSession(100);  
        
    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
    packetMbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, extraSession);    
        
    User::WaitForRequest( requestStatus );
    ASSERT_EQUALS( KErrOverflow, requestStatus.Int() ) ;
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
    }
    
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0031
 * @SYMFssID BA/CTSY/MB-0031
 * @SYMTestCaseDesc MBMS local context Configuration when Sessions used when adding existing session id
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - The Client requests to add Session and get error code KErrAlreadyExists
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Test adding existing ID number
 *
 * @return - none
 */
void CCTsyUCaseMbms016::CCtsyIntegrationTestMbms0003L()
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

    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
					
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
    
    // activate context
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	 
	status = SetMbmsActiveL(packetMbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);     
    
 	TMockLtsyData1 <TInfoName > expData( contextId );
	expData.SerialiseL( expectData );
	
 	TMockLtsyData1 <TInfoName> outData( contextId );
	outData.SerialiseL( data );      
    
    // add maximum number of sessions - 1  
    for( TInt i = 0; i < maxSessions - 1; i++ )
        {
        TMbmsSessionId session(i);  
        
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
        packetMbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
        
        User::WaitForRequest( requestStatus );
        TInt ret = requestStatus.Int();
        ASSERT_EQUALS( KErrNone, ret ) ;
        }    
  	
  
  	// and one to cause KErrOverflow
    TMbmsSessionId extraSession(0);  
        
    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
    packetMbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, extraSession);    
        
    User::WaitForRequest( requestStatus );
    ASSERT_EQUALS( KErrAlreadyExists, requestStatus.Int() ) ;        
					
	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy( 5 ); //packetMbmsContext, packetService, expectData, data, this
      }
    
