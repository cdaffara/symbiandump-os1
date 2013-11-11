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

#include "cctsyucasembms011.h"
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

#define MBMS_MONITORSERVICECOUNTMULTIPLE 3

CTestSuite* CCTsyUCaseMbms011::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms011, CCTsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms011, CCTsyIntegrationTestMbms0002L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0020
 * @SYMFssID BA/CTSY/PKTS-0020
 * @SYMTestCaseDesc Support retrieving current availability status for each service currently being monitored
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Every service has a valid status (Unknown, Unavailable or Available).  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Support retrieving current availability status.
 *
 * @return - none
 */
void CCTsyUCaseMbms011::CCTsyIntegrationTestMbms0001L()
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
    RPacketMbmsContext packetMbmsContext;
    packetMbmsContext.OpenNewContext( packetService, contextId );
    CleanupClosePushL( packetMbmsContext );
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	RBuf8 data;
	CleanupClosePushL( data );
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
 
   	TInt maxAllowed( 10 ); 
	TInt maxvalue( 10 );

 	// check maximum monitored service value
 	TMockLtsyData1<TInt> ltsyData( maxvalue );
 	data.Close();
	ltsyData.SerialiseL( data );
 	iMockLTSY.NotifyTerminated( mockLtsyStatus );	
	iMockLTSY.CompleteL( EPacketEnumerateMbmsMonitorServiceList, KErrNone, data );
	User::WaitForRequest( mockLtsyStatus );	
	ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );		
	ASSERT_EQUALS( maxvalue, maxAllowed );
	
 	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install( &scheduler );
	
	//List retriever
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL( packetService );	 
	CleanupStack::PushL( monitorList );

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL( *monitorList );
	CleanupStack::PushL( activeRetriever );
	scheduler.AddRetrieverL( *activeRetriever );
		
	RBuf8 expectData;
	CleanupClosePushL( expectData );
	TInt i;
    TMockLtsyData1<TInt> expExpect( i );
    expExpect.SerialiseL( expectData );
    
	RBuf8 completeData;
	CleanupClosePushL( completeData );
	
	TRequestStatus aReqStatus;
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL( multipleEntries );
	
	for( TInt i = 0; i < MBMS_MONITORSERVICECOUNTMULTIPLE; i++ )
		{
		existingParams.iTmgi.SetServiceId( i ); 
	    existingParams.iTmgi.SetMCC( i );
	    existingParams.iTmgi.SetMNC( i );
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( existingParams );
		}
	
	iMockLTSY.ExpectL( EPacketUpdateMbmsMonitorServiceList, expectData );
	iMockLTSY.CompleteL( EPacketUpdateMbmsMonitorServiceList, KErrNone, completeData );

	// PacketService::UpdateMbmsMonitorServiceListL	
	packetService.UpdateMbmsMonitorServiceListL( aReqStatus, EAddEntries, multipleEntries );	
	User::WaitForRequest( aReqStatus );
	ASSERT_EQUALS( KErrNone, aReqStatus.Int() );
		
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS( MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate() );

	
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	// ensure that every service has a valid status
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );
	
	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	// ensure that Services is added correctly into the monitored service list			
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual( *multipleEntries, *list ) );

	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 11, this ); // list, multipleEntries, completeData, expectData, activeRetriever, monitorList, data, packetMbmsContext, packetService, this 
    }

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0021
 * @SYMFssID BA/CTSY/PKTS-0021
 * @SYMTestCaseDesc Support retrieving current availability status for each service currently being monitored
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Every service has a valid status (Unknown, Unavailable or Available).  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Support retrieving current availability status.
 *
 * @return - none
 */
void CCTsyUCaseMbms011::CCTsyIntegrationTestMbms0002L()
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

	RBuf8 data;
	CleanupClosePushL( data );

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install( &scheduler );
    
	//
	// SET UP END
	//    
	
	//
	// TEST START
	//
	
	
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL( packetService );	 
	CleanupStack::PushL( monitorList );

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL( *monitorList );
	CleanupStack::PushL( activeRetriever );
	scheduler.AddRetrieverL( *activeRetriever );

	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	// ensure that Status is KErrNotFound because list is empty
	ASSERT_EQUALS( KErrNotFound, activeRetriever->iStatus.Int() );
	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CActiveScheduler::Install( NULL );
	CleanupStack::PopAndDestroy( 5, this ); //  activeRetriever, monitorList, data, packetService, this, list
   }
    
