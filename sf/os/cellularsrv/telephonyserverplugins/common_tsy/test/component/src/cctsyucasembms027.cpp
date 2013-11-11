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

#include "CCTsyUCaseMbms027.h"
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

CTestSuite* CCTsyUCaseMbms027::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms027, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms027, CCtsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0045
 * @SYMFssID BA/CTSY/MB-0045
 * @SYMTestCaseDesc Support retrieving sessions relating to MBMS context
 * @SYMTestPriority High
 * @SYMTestActions RPacketMbmsContext::OpenNewContext, RPacketMbmsContext::SetConfig, RPacketMbmsContext::UpdateMbmsSessionList RPacketMbmsContext::Activate, RPacketMbmsContext::Deactivate
 * @SYMTestExpectedResults Pass - Client receives list of sessions for given MBMS context. 
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that retrieving sessions are supported.
 *
 * @return - none
 */
void CCTsyUCaseMbms027::CCtsyIntegrationTestMbms0001L()
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
              
	TInfoName contextId;
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( mbmsContext );
    
    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
	//
	// SET UP END
	//    
    
    // initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId); 
    ASSERT_EQUALS(KErrNone, status);	
    
    // data for SetConfig
    TRequestStatus requestStatus;
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
    setConfig.iTmgi.SetServiceId(1); 
    setConfig.iTmgi.SetMCC(111);
    setConfig.iTmgi.SetMNC(111);
    setConfig.iMbmsAccessBearer = E3GBearer;
    setConfig.iMbmsServicePriority = EMbmsHighServicePriority;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsSessionFlag = ETrue;
	
	// configure context
    status = SetMbmsConfigL(mbmsContext, contextId, setConfig);
	ASSERT_EQUALS(KErrNone, status);	

	
	// add sessions
	TInt maxSessions( 5 );	
	
	TMockLtsyData1 <TInfoName > expData( contextId );
	expData.SerialiseL( expectData );
	
 	TMockLtsyData1 <TInfoName> outData( contextId );
	outData.SerialiseL( data );	
    
    for( TInt i = 0; i < maxSessions; i++ )
	    {
	    TMbmsSessionId session(i);  
	    
	    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
	    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
	    mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
	    
	    User::WaitForRequest( requestStatus );
	    ASSERT_EQUALS( KErrNone, requestStatus.Int() ) ;	
	    }
    
	// Activate MBMS context
	status = SetMbmsActiveL(mbmsContext, setConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* session = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(session);	
    	
	CRetrievePcktMbmsSessionList* sessionList = CRetrievePcktMbmsSessionList::NewL(mbmsContext, *session);        
    CleanupStack::PushL(sessionList);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*sessionList);
    
    scheduler.AddRetrieverL(retriever);
	
	//
	// TEST START
	//
	
	
	// The Client requests MBMS Context Session List from CTSY
    sessionList->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(KErrNone, retriever.iStatus.Int());
	
	// CTSY returns a list of sessions relating to a MBMS context 
	for( TUint i = 0; i < session->iSessionIdList.Count(); i++)
	    {
	    TMbmsSessionId sessionId = session->iSessionIdList[i];
	    ASSERT_EQUALS( sessionId, i);	    
	    }
	
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());	 	
 	AssertMockLtsyStatusL();
	
	// deactivate
	status = SetMbmsDeactiveL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// delete context
	status = SetMbmsDeleteL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// close context
	mbmsContext.Close( );        
            
	AssertMockLtsyStatusL();
	
	//
	// TEST END
	//	
	CActiveScheduler::Install(NULL);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 7, this ); // sessionList, session, expectData, data, mbmsContext, packetService, this 
    }
    

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0046
 * @SYMFssID BA/CTSY/MB-0046
 * @SYMTestCaseDesc Support retrieving sessions relating to MBMS context
 * @SYMTestPriority High
 * @SYMTestActions RPacketMbmsContext::OpenNewContext, RPacketMbmsContext::SetConfig, RPacketMbmsContext::UpdateMbmsSessionList RPacketMbmsContext::Activate, RPacketMbmsContext::Deactivate
 * @SYMTestExpectedResults Pass - Client receives KerrNotFound with empty list of sessions for given MBMS context.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that retrieving sessions are supported.
 *
 * @return - none
 */
void CCTsyUCaseMbms027::CCtsyIntegrationTestMbms0002L()
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
              
	TInfoName contextId;
    RPacketMbmsContext mbmsContext;
    mbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( mbmsContext );
    
    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
	//
	// SET UP END
	//    
    
    // initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId); 
    ASSERT_EQUALS(KErrNone, status);	
    
    // data for SetConfig
    TRequestStatus requestStatus;
    RPacketMbmsContext::TContextConfigMbmsV1 setConfig;
    setConfig.iTmgi.SetServiceId(1); 
    setConfig.iTmgi.SetMCC(111);
    setConfig.iTmgi.SetMNC(111);
    setConfig.iMbmsAccessBearer = E3GBearer;
    setConfig.iMbmsServicePriority = EMbmsHighServicePriority;
    setConfig.iMbmsServiceMode = KBroadcast;
    setConfig.iMbmsSessionFlag = ETrue;
	
	// configure context
    status = SetMbmsConfigL(mbmsContext, contextId, setConfig);
	ASSERT_EQUALS(KErrNone, status);	

	// Activate MBMS context
	status = SetMbmsActiveL(mbmsContext, setConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* session = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(session);	
    	
	CRetrievePcktMbmsSessionList* sessionList = CRetrievePcktMbmsSessionList::NewL(mbmsContext, *session);        
    CleanupStack::PushL(sessionList);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*sessionList);
    
    scheduler.AddRetrieverL(retriever);
	
	//
	// TEST START
	//

	// Monitor list is empty. CTSY completes with KErrNotFound
    sessionList->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(KErrNotFound, retriever.iStatus.Int());
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());	 	
 	
 	AssertMockLtsyStatusL();

	// deactivate
	status = SetMbmsDeactiveL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// delete context
	status = SetMbmsDeleteL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// close context
	mbmsContext.Close( );     
	AssertMockLtsyStatusL();
	
	//
	// TEST END
	//	
	CActiveScheduler::Install(NULL);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 7, this ); // sessionList, session, expectData, data, mbmsContext, packetService, this 
      }
    
