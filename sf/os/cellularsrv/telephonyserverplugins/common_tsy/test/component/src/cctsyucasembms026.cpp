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

#include "cctsyucasembms026.h"
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

CTestSuite* CCTsyUCaseMbms026::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms026, CCtsyIntegrationTestMbms0001L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0044
 * @SYMFssID BA/CTSY/MB-0044
 * @SYMTestCaseDesc MBMS Session Update for Service due to session successfully received 
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Session state is successfully updated.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that MBMS Session Update for Service due to session successfully received.
 *
 * @return - none
 */
void CCTsyUCaseMbms026::CCtsyIntegrationTestMbms0001L()
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
	TInt maxSessions( 1 );	
	
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
	
	RPacketService::TStatus sendStatus = RPacketService::EStatusActive;
	TBool isResumed = EFalse;	
	TInt err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);
	
	
	RPacketService::TStatus attachStatus;
    TInt ret = packetService.GetStatus(attachStatus);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RPacketService::EStatusActive, attachStatus);
	
	// Ensure MBMS Context status is Active
	RPacketContext::TContextStatus expectedStatus = RPacketContext::EStatusActive;
	RPacketContext::TContextStatus contextStatus;
	ret = mbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS((TInt)expectedStatus, (TInt)contextStatus);
	
	// Add a session
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* session = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(session);	
    	
	CRetrievePcktMbmsSessionList* sessionList = CRetrievePcktMbmsSessionList::NewL(mbmsContext, *session);        
    CleanupStack::PushL(sessionList);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*sessionList);
    
    scheduler.AddRetrieverL(retriever);
        
    sessionList->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(KErrNone, retriever.iStatus.Int());
    
    // ensure there is session added to a MBMS context 
	for( TUint i = 0; i < session->iSessionIdList.Count(); i++)
	    {
	    TMbmsSessionId sessionId = session->iSessionIdList[i];
	    ASSERT_EQUALS( sessionId, i);	    
	    }
	
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());	 	
 	AssertMockLtsyStatusL();
    
    // start sessions receiving
    TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusReceiving;
 	TMockLtsyData2 <TInfoName, TContextMisc>ltsyData1(contextId, completeMisc);
	ltsyData1.SerialiseL(data);
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange , KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    
    // Check context status is RPacketContext::EStatusReceiving
	expectedStatus = RPacketContext::EStatusReceiving;
	ret = mbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS((TInt)expectedStatus, (TInt)contextStatus);
    
    
	//
	// SET UP END
	//    
            
            
            
	//
	// TEST START
	//

	// Upper layers finish receiving all data associated with MBMS session.
	completeMisc.iStatus = RPacketContext::EStatusActive;
	ltsyData1.SerialiseL(data);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange , KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);

    // Check context status is RPacketContext::EStatusActive
	expectedStatus = RPacketContext::EStatusActive;
	ret = mbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS((TInt)expectedStatus, (TInt)contextStatus);
    
    // The Client requests UpdateSession for context from ETEL API to remove session id.
	expData.SerialiseL( expectData );
	outData.SerialiseL( data );	

    for( TInt i = 0; i < maxSessions; i++ )
	    {
	    TMbmsSessionId session(i);  
	    
	    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
	    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
	    // CTSY completes UpdateSession to client. 
	    mbmsContext.UpdateMbmsSessionList(requestStatus, ERemoveEntries, session);    
	    
	    User::WaitForRequest( requestStatus );
	    ASSERT_EQUALS( KErrNone, requestStatus.Int() ) ;	
	    }
 	
 	// Ensure session list is empty
	sessionList->Start( retriever.Status() );	 	
	retriever.Activate();
	scheduler.StartScheduler();
	
	ASSERT_EQUALS(KErrNotFound, retriever.iStatus.Int());
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());	 
	
    
    // deactivate context 
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
	CleanupStack::PopAndDestroy( 7, this ); // sessionList, session, expectData, data, packetMbmsContext, packetService, this 
       }
    
