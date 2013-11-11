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

#include "cctsyucasembms019.h"
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

CTestSuite* CCTsyUCaseMbms019::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms019, CCtsyIntegrationTestMbms0001L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0035
 * @SYMFssID BA/CTSY/MB-0035
 * @SYMTestCaseDesc MBMS data reception resumes
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Context1 is receiving.  Context2 is active idle.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS data reception resumes.
 *
 * @return - none
 */
void CCTsyUCaseMbms019::CCtsyIntegrationTestMbms0001L()
    {
        
	//
	// SET UP
	//
	        
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RBuf8 data;
	CleanupClosePushL(data); 	
	
	TInt maxSessions( 5 );
	RPacketService packetService;
	OpenPacketServiceL( packetService,
	                    RPacketService::EStatusUnattached, 
	                    RPacketService::EAttachWhenNeeded,
	                    maxSessions,
	                    maxSessions );
    CleanupClosePushL( packetService );
              
	TInfoName contextId1, contextId2;
    RPacketMbmsContext context1, context2;
    context1.OpenNewContext( packetService, contextId1 );
    context2.OpenNewContext( packetService, contextId2 );
    CleanupClosePushL( context1 );
    CleanupClosePushL( context2 );
    
	// data for NotifyStatusChange
	TRequestStatus	requestStatus, requestStatus2;
	RPacketContext::TContextStatus  contextStatus, contextStatus2;     
	TContextMisc  completeMisc, completeMisc2;
	TRequestStatus mockLtsyStatus, mockLtsyStatus2;
	
    // initialise contexts				
	TInt status = InitMbmsContextL(context1, contextId1);	
	ASSERT_EQUALS( KErrNone, status );
	status = InitMbmsContextL(context2, contextId2);	
	ASSERT_EQUALS( KErrNone, status );
	
	// 1st context configuration	
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig1;
    setConfig1.iMbmsAccessBearer = E2GBearer;
    setConfig1.iMbmsServiceMode = KBroadcast;
    setConfig1.iMbmsServicePriority = EMbmsLowServicePriority;
    setConfig1.iMbmsSessionFlag = ETrue;
    
    // setup context1 
    status = SetMbmsConfigL( context1, contextId1, setConfig1 );
    ASSERT_EQUALS( KErrNone, status );
    // status = receiving at first
    
	// context activation           	        	  	
	status = SetMbmsActiveL( context1, setConfig1, contextId1 );
	ASSERT_EQUALS( KErrNone, status );	    
        
	// 2nd context configuration	
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig2;
    setConfig2.iMbmsAccessBearer = E2GBearer;
    setConfig2.iMbmsServiceMode = KBroadcast;
    setConfig2.iMbmsServicePriority = EMbmsHighServicePriority;
    setConfig2.iMbmsSessionFlag = ETrue;   
    
    // setup context2 
    status = SetMbmsConfigL( context2, contextId2, setConfig2 );
    ASSERT_EQUALS( KErrNone, status );
    // status = receiving at first
    
	// context activation           	        	  	
	status = SetMbmsActiveL( context2, setConfig2, contextId2 );
	ASSERT_EQUALS( KErrNone, status ); 
    
    // Phone is up and running. 
    // Client has 2 MBMS contexts set up (Context1 for Service1 and Context2 for Service2).  
    // Context2 has higher priority than Context1.
    // Handset only supports receiving 1 MBMS service at a time
    // Context1 is currently suspended (i.e. Context1 status = EStatusSuspended)
    // Context2 is currently receiving (i.e. Context1 status = EStatusReceiving)
    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);    

    // set context 1 suspending       
    completeMisc.iStatus = RPacketContext::EStatusSuspended;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData4(contextId1, completeMisc);
	ltsyData4.SerialiseL(data);	
	//send request
	context1.NotifyStatusChange(requestStatus, contextStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	AssertMockLtsyStatusL();

	iMockLTSY.NotifyTerminated(mockLtsyStatus);  
	
    // context 2 is receiving
    completeMisc2.iStatus = RPacketContext::EStatusReceiving;      
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData3(contextId2, completeMisc2);
	ltsyData3.SerialiseL(data);	
	//send request
	context2.NotifyStatusChange(requestStatus2, contextStatus2);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus2);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(completeMisc2.iStatus, contextStatus2);		
	AssertMockLtsyStatusL();     
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//

    // Network sends MBMS indication that Sevice2 has finished delivery.
    // LTSY completes indication to CTSY for Context2  with status of context = EStatusActivated
    // commpletes pending Context2, NotifyStatusChange with status = EStatusActivated.
    iMockLTSY.NotifyTerminated(mockLtsyStatus2);
    completeMisc2.iStatus = RPacketContext::EStatusActive;
    
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData2(contextId2, completeMisc2);
	ltsyData2.SerialiseL(data);	
	//send request
	context2.NotifyStatusChange(requestStatus2, contextStatus2);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus2);    
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(completeMisc2.iStatus, contextStatus2);		
	AssertMockLtsyStatusL(); 	
    
    // CTSY completes pending Context1, NotifyStatusChange with status = EStatusReceiving.
    completeMisc.iStatus = RPacketContext::EStatusReceiving;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId1, completeMisc);
	ltsyData.SerialiseL(data);	
	//send request
	context1.NotifyStatusChange(requestStatus, contextStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);		
	AssertMockLtsyStatusL(); 	
	
	// post-conditions: Context1 is receiving.  Context2 is active idle.
		
	context1.GetStatus(contextStatus);
	context2.GetStatus(contextStatus2);
	
	ASSERT_EQUALS( RPacketContext::EStatusReceiving, contextStatus );  	
	ASSERT_EQUALS( RPacketContext::EStatusActive, contextStatus2 ); 
	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); // context2, context1, data, packetService, this 
      }
    
