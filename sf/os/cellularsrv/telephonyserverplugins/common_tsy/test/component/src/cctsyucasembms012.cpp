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

#include "cctsyucasembms012.h"
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

CTestSuite* CCTsyUCaseMbms012::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;        
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms012, CCTsyIntegrationTestMbms0001L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0022
 * @SYMFssID BA/CTSY/PKTS-0022
 * @SYMTestCaseDesc Support being notified when service availability changes
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - CTSY get new service statuses and informs client.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that NotifyMbmsNetworkServiceStatusChange completes correctly.
 *
 * @return - none
 */
void CCTsyUCaseMbms012::CCTsyIntegrationTestMbms0001L()
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
 	
 	// check maximum monitored service value.
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
		
	RBuf8 expectData1;
	CleanupClosePushL( expectData1 );
	TInt i;
    TMockLtsyData1<TInt> expExpect1( i );
    expExpect1.SerialiseL( expectData1 );
    
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
	
	iMockLTSY.ExpectL( EPacketUpdateMbmsMonitorServiceList, expectData1 );
	iMockLTSY.CompleteL( EPacketUpdateMbmsMonitorServiceList, KErrNone, completeData );
	
	packetService.UpdateMbmsMonitorServiceListL(aReqStatus, EAddEntries, multipleEntries );	
	User::WaitForRequest( aReqStatus );
	ASSERT_EQUALS( KErrNone, aReqStatus.Int() );
		
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS(MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate() );
	
   	RBuf8 expectData2;
	CleanupClosePushL( expectData2 );
	TInt j;
    TMockLtsyData1<TInt> expExpect2( j );
    expExpect2.SerialiseL( expectData2 );
    
	RBuf8 completeData2;
	CleanupClosePushL( completeData2 );
	
    RPacketService::TMbmsServiceAvailabilityV1 updatedParams;
    
    CPcktMbmsMonitoredServiceList* multipleEntries2 = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL( multipleEntries2 );
	
	for( TInt i = 0; i < MBMS_MONITORSERVICECOUNTMULTIPLE; i++ )
		{
		updatedParams.iTmgi.SetServiceId( i ); 
	    updatedParams.iTmgi.SetMCC( i );
	    updatedParams.iTmgi.SetMNC( i );
	    updatedParams.iMbmsServiceMode = KBroadcast;
	    updatedParams.iMbmsAccessBearer = E2GBearer;
	   	updatedParams.iMbmsAvailabilityStatus = EMbmsAvailable;
		multipleEntries2->AddEntryL( updatedParams );
		}


 	//-------------------------------------------------------------------------
	// TEST: Successful completion request of
	// RPacketService::NotifyMbmsServiceAvailabilityChange.
 	//-------------------------------------------------------------------------
  
	// RPacketService::NotifyMbmsServiceAvailabilityListChange
	packetService.NotifyMbmsServiceAvailabilityChange( requestStatus );
	
	TInt complete2 = 0;
 	TMockLtsyData1< TInt > dataForComplete2( complete2 );
    dataForComplete2.SerialiseL( completeData2 );
    
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
   	iMockLTSY.CompleteL( EPacketNotifyMbmsServiceAvailabilityChange, KErrNone, completeData2 );
			
	User::WaitForRequest( mockLtsyStatus );
	AssertMockLtsyStatusL();
	// ensure that RPacketService::NotifyMbmsServiceAvailabilityListChange completes with status KErrNone
	ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );	 
	User::WaitForRequest( requestStatus );	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );
	
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );
	
	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	// check that monitored list has been updated correctly.		
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual( *multipleEntries2, *list ) );

	//
	// TEST END
	//	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 14, this ); // multipleEntries2, expectData2, list, multipleEntries, completeData, expectData1, activeRetriever, monitorList, data, packetMbmsContext, packetService, this
   }
    
