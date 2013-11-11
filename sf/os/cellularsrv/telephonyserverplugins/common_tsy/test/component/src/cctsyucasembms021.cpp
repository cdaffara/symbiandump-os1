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

#include "cctsyucasembms021.h"
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

CTestSuite* CCTsyUCaseMbms021::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms021, CCtsyIntegrationTestMbms0001L );
	END_SUITE;
	}

//
// Actual test cases
//
  
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0037
 * @SYMFssID BA/CTSY/MB-0037
 * @SYMTestCaseDesc MBMS is not currently being used
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::InitialiseContext, RPacketContext::SetConfig, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Client gets correct capability. No MBMS contexts are in receiving state.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that Context status is moved from receiving state to active state as a result of reception finishing
 *
 * @return - none
 */
void CCTsyUCaseMbms021::CCtsyIntegrationTestMbms0001L()
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
    
    mbmsContext.NotifyStatusChange(requestStatus, contextStatus);	
    
    TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusReceiving;
 	TMockLtsyData2 <TInfoName, TContextMisc>ltsyData1(contextId, completeMisc);
	ltsyData1.SerialiseL(data);
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange , KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    User::WaitForRequest(requestStatus);
	
	// Ensure RPacketContext::NotifyStatusChange completes MBMS context status RPacketContext::EStatusReceiving	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	AssertMockLtsyStatusL();
    
    // Check RPacketContext::GetStatus returns RPacketContext::EStatusReceiving
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
	
 
	// Ensure GetDynamicCaps capabilities return RPacketService::KCapsMBMS caps
	RPacketService::TDynamicCapsFlags caps;
	
	packetService.GetDynamicCaps(caps);
	ASSERT_TRUE(RPacketService::KCapsMBMS & caps);	

    // post RPacketService::NotifyDynamicCapsChange
	TRequestStatus dynamicCapsStatus;
	packetService.NotifyDynamicCapsChange(dynamicCapsStatus, caps);

    // post RPacketService::NotifyStatusChange
	mbmsContext.NotifyStatusChange(requestStatus, contextStatus);	

	// Context status is moved from receiving state to active state as a result of reception finishing.
    completeMisc.iStatus = RPacketContext::EStatusActive;
	ltsyData1.SerialiseL(data);
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange , KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    User::WaitForRequest(requestStatus);
	
	// Ensure RPacketContext::NotifyStatusChange completes MBMS context status RPacketContext::EStatusActive	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	AssertMockLtsyStatusL();
    
    // Check RPacketContext::GetStatus returns RPacketContext::EStatusActive
	expectedStatus = RPacketContext::EStatusActive;
	ret = mbmsContext.GetStatus(contextStatus);
	ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS((TInt)expectedStatus, (TInt)contextStatus);

   	// Ensure RPacketService::NotifyDynamicCapsChange doens't return RPacketService::KCapsMBMS
   	User::WaitForRequest(dynamicCapsStatus);
   	ASSERT_EQUALS(KErrNone, dynamicCapsStatus.Int());
   	
   	RPacketService::TDynamicCapsFlags expectedCaps;	
   	expectedCaps = caps;
   	expectedCaps &= ~RPacketService::KCapsMBMS;
   	ASSERT_EQUALS(expectedCaps, caps );	
	
    // Ensure Dynamic capabilities doens't return RPacketService::KCapsMBMS
    packetService.GetDynamicCaps(caps);
    ASSERT_EQUALS(KErrNone, dynamicCapsStatus.Int());
    expectedCaps = caps;
    expectedCaps &= ~RPacketService::KCapsMBMS;
	ASSERT_EQUALS(expectedCaps, caps );	
	AssertMockLtsyStatusL();

	// deactivate context 
	status = SetMbmsDeactiveL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// delete context
	status = SetMbmsDeleteL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// close context
	mbmsContext.Close( );     
	        
    	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CActiveScheduler::Install(NULL);    
	CleanupStack::PopAndDestroy( 7, this ); // sessionlist, session, expectData, mbmsContext, packetService, data, this
      }
    
