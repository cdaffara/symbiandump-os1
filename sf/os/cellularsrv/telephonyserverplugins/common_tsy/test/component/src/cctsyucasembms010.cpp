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

#include "cctsyucasembms010.h"
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
#define MBMS_MONITORSERVICECOUNTZERO 0

CTestSuite* CCTsyUCaseMbms010::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;     
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms010, CCTsyIntegrationTestMbms0001L );    
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0019
 * @SYMFssID BA/CTSY/PKTS-0019
 * @SYMTestCaseDesc Support removing all entries from monitor list
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::EnumerateNifs, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate, RPacketService::EnumerateContextsInNif, RPacketContext::Deactivate
 * @SYMTestExpectedResults Pass - Monitored service list is empty.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test:Verify that all services is removed correctly to the monitored service list.
 *
 * @return - none
 */
void CCTsyUCaseMbms010::CCTsyIntegrationTestMbms0001L()
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
	
    
  	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RBuf8 data;
	CleanupClosePushL( data );
	
 	// check maximum monitored service value	
	TInt maxvalue( 10 );
 	TMockLtsyData1<TInt> ltsyData( maxvalue );
 	data.Close();
	ltsyData.SerialiseL( data );

 	TRequestStatus mockLtsyStatus;
 	iMockLTSY.NotifyTerminated( mockLtsyStatus );	
	iMockLTSY.CompleteL( EPacketEnumerateMbmsMonitorServiceList, KErrNone, data );
	User::WaitForRequest( mockLtsyStatus );	
	ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );
	
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
	
	TRequestStatus reqStatus;
    RPacketService::TMbmsServiceAvailabilityV1 param;
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL( multipleEntries );
	
	for( TInt i = 0; i < MBMS_MONITORSERVICECOUNTMULTIPLE; i++ )
		{
		param.iTmgi.SetServiceId( i ); 
	    param.iTmgi.SetMCC( i );
	    param.iTmgi.SetMNC( i );
	    param.iMbmsServiceMode = KBroadcast;
	    param.iMbmsAccessBearer = E2GBearer;
		multipleEntries->AddEntryL( param );
		}
	
	iMockLTSY.ExpectL( EPacketUpdateMbmsMonitorServiceList, expectData );
	
	TInt complete( 1 );
	TMockLtsyData1< TInt > entryData( complete );
	entryData.SerialiseL( completeData );
	iMockLTSY.CompleteL( EPacketUpdateMbmsMonitorServiceList, KErrNone, completeData );

	// PacketService::UpdateMbmsMonitorServiceListL		
	packetService.UpdateMbmsMonitorServiceListL( reqStatus, EAddEntries, multipleEntries );	
	User::WaitForRequest( reqStatus );
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS( MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate() );
	// ensure that Services is added correctly into the monitored service list	
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual( *multipleEntries, *list ) );
 
 	TMockLtsyData1< TInt > dataForComplete( complete );
	iMockLTSY.ExpectL( EPacketUpdateMbmsMonitorServiceList, expectData );
    dataForComplete.SerialiseL( completeData );
   	iMockLTSY.CompleteL( EPacketUpdateMbmsMonitorServiceList, KErrNone, completeData );
		
	CPcktMbmsMonitoredServiceList* nullPtr( NULL );
	// PacketService::UpdateMbmsMonitorServiceListL				
	packetService.UpdateMbmsMonitorServiceListL( reqStatus, ERemoveAllEntries, nullPtr );	
	User::WaitForRequest( reqStatus );
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );

	list = monitorList->RetrieveListL();
	
	TInt ix( 0 );
	while( multipleEntries->Enumerate() > 0 )
		{
		multipleEntries->DeleteEntryL( ix );	
		}
	
	ASSERT_EQUALS( MBMS_MONITORSERVICECOUNTZERO, list->Enumerate() );
	// ensure that all Services are removed correctly into the monitored service list
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual( *multipleEntries, *list ) );

 	AssertMockLtsyStatusL();
	CActiveScheduler::Install( NULL );
	CleanupStack::PopAndDestroy( 10, this );
    }
    
