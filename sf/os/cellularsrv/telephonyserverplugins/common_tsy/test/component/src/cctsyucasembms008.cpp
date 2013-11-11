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

#include "cctsyucasembms008.h"
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

#define MBMS_MONITORSERVICECOUNTONE	1
#define MBMS_MONITORSERVICECOUNTMULTIPLE 3

CTestSuite* CCTsyUCaseMbms008::CreateSuiteL(const TDesC& aName) 
	{
	SUB_SUITE;     
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms008, CCtsyIntegrationTestMbms0001L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms008, CCtsyIntegrationTestMbms0002L );
    ADD_TEST_STEP_ISO_CPP(CCTsyUCaseMbms008, CCtsyIntegrationTestMbms0003L );
	END_SUITE;
	}

//
// Actual test cases
//

/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0014
 * @SYMFssID BA/CTSY/PKTS-0014
 * @SYMTestCaseDesc Support updating monitor list by adding an entry.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMbmsNetworkServiceStatusChange, RPacketService::UpdateMbmsMonitorServiceListL
 * @SYMTestExpectedResults Pass - Service is added to the monitored service list.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify that services is added correctly to the monitored service list.
 *
 * @return - none
 */
void CCTsyUCaseMbms008::CCtsyIntegrationTestMbms0001L()
    {
        
	//
	// SET UP
	//
	        
   	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	

	RBuf8 data;
	CleanupClosePushL(data);
				
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

   	TRequestStatus requestStatus;
   	TRequestStatus requestStatus2;
   	TRequestStatus mockLtsyStatus;
   	    
    // Ensure packet service status is RPacketService::EStatusAttached.
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
     	    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	
   	TInt maxAllowed = 3; 
	TInt maxvalue = 3;
 	
 	// check maximum monitored service value .
 	TMockLtsyData1<TInt> ltsyData2(maxvalue);
 	data.Close();
	ltsyData2.SerialiseL(data);
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());		
	ASSERT_EQUALS( maxvalue, maxAllowed );

 	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	// post RPacketService::NotifyMbmsNetworkServiceAvailabilityChange
	packetService.NotifyMbmsServiceAvailabilityChange( requestStatus );
		
	//List retriever
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
							
	RBuf8 expectData1;
	CleanupClosePushL(expectData1);
	TInt i;
    TMockLtsyData1<TInt> expExpect1(i);
    expExpect1.SerialiseL(expectData1);
    
	RBuf8 CompleteData;
	CleanupClosePushL(CompleteData);
	
	TRequestStatus aReqStatus;
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
    
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		existingParams.iTmgi.SetServiceId(i); 
	    existingParams.iTmgi.SetMCC(i);
	    existingParams.iTmgi.SetMNC(i);
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( existingParams );
		}
	
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, expectData1);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, CompleteData);
	
	// PacketService::UpdateMbmsMonitorServiceListL
	packetService.UpdateMbmsMonitorServiceListL(aReqStatus, EAddEntries, multipleEntries);	
	
	// wait for completion of UpdateMbmsMonitorServiceListL(	
	User::WaitForRequest(aReqStatus);
	// wait for completion of NotifyMbmsServiceAvailabilityChange
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS( KErrNone, aReqStatus.Int() );

	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS(MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate() );
	
	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );
	
	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	// ensure that Services is added correctly into the monitored service list	
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual( *multipleEntries, *list ) );

	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 10, this ); // packetMbmsContext, packetService, this 
    }
 
 /**
 * @SYMTestCaseID BA-CTSY-INT-MB-0015
 * @SYMFssID BA/CTSY/PKTS-0015
 * @SYMTestCaseDesc Support updating monitor list by adding an entry.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMbmsNetworkServiceStatusChange, RPacketService::UpdateMbmsMonitorServiceListL
 * @SYMTestExpectedResults Pass - Service is added to the monitored service list.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test:  Verify that services is added correctly to the monitored service list.
 *
 * @return - none
 */
void CCTsyUCaseMbms008::CCtsyIntegrationTestMbms0002L()
    {
        
	//
	// SET UP
	//
	        
   	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	

	RBuf8 data;
	CleanupClosePushL(data);
				
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

   	TRequestStatus requestStatus;
   	TRequestStatus requestStatus2;
   	TRequestStatus mockLtsyStatus;
   	    
    // Ensure packet service status is RPacketService::EStatusAttached.
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
	TInt maxAllowed = 3; 
	TInt maxvalue = 3;
 	
 	// check maximum monitored service value
 	TMockLtsyData1<TInt> ltsyData2(maxvalue);
 	data.Close();
	ltsyData2.SerialiseL(data);
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());		
	ASSERT_EQUALS( maxvalue, maxAllowed );

 	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	// post RPacketService::NotifyMbmsNetworkServiceAvailabilityChange
	packetService.NotifyMbmsServiceAvailabilityChange( requestStatus );
		
	//List retriever
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
							
	RBuf8 expectData1;
	CleanupClosePushL(expectData1);
	TInt i;
    TMockLtsyData1<TInt> expExpect1(i);
    expExpect1.SerialiseL(expectData1);
    
	RBuf8 CompleteData;
	CleanupClosePushL(CompleteData);
	
	TRequestStatus aReqStatus;
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
    
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE +1;i++)
		{
		existingParams.iTmgi.SetServiceId(i); 
	    existingParams.iTmgi.SetMCC(i);
	    existingParams.iTmgi.SetMNC(i);
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( existingParams );
		}
	
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, expectData1);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, CompleteData);
	
	// PacketService::UpdateMbmsMonitorServiceListL	
	packetService.UpdateMbmsMonitorServiceListL(aReqStatus, EAddEntries, multipleEntries);	
	
	// wait for completion of UpdateMbmsMonitorServiceListL(	
	User::WaitForRequest(aReqStatus);
	// wait for completion of NotifyMbmsServiceAvailabilityChange
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();
	// Ensure RPacketService::NotifyMbmsNetworkServiceStatusChange completes with status KErrOverflow		
	ASSERT_EQUALS( KErrOverflow, aReqStatus.Int() );
        		
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 8, this ); // packetMbmsContext, packetService, this 
    }
       
/**
 * @SYMTestCaseID BA-CTSY-INT-MB-0016
 * @SYMFssID BA/CTSY/PKTS-0016
 * @SYMTestCaseDesc Support updating monitor list by adding an entry.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMbmsNetworkServiceStatusChange, RPacketService::UpdateMbmsMonitorServiceListL
 * @SYMTestExpectedResults Pass - Service is added to the monitored service list.  
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test:  Verify that services is added correctly to the monitored service list.
 *
 * @return - none
 */
void CCTsyUCaseMbms008::CCtsyIntegrationTestMbms0003L()
    {
        
	//
	// SET UP
	//
	        
   	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	

	RBuf8 data;
	CleanupClosePushL(data);
				
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

   	TRequestStatus requestStatus;
   	TRequestStatus requestStatus2;
   	TRequestStatus mockLtsyStatus;
   	    
    // Ensure packet service status is RPacketService::EStatusAttached.
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);
	RPacketService::TStatus contextStatus;
	packetService.NotifyStatusChange(requestStatus2, contextStatus);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data); //
	
	User::WaitForRequest(mockLtsyStatus);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
    
	//
	// SET UP END
	//    
            
	//
	// TEST START
	//
	
 	TInt maxAllowed = 3; 
	TInt maxvalue = 3;
 	
 	// check maximum monitored service value
 	TMockLtsyData1<TInt> ltsyData2(maxvalue);
 	data.Close();
	ltsyData2.SerialiseL(data);
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	ASSERT_EQUALS( maxvalue, maxAllowed );

 	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	// post RPacketService::NotifyMbmsNetworkServiceAvailabilityChange
	packetService.NotifyMbmsServiceAvailabilityChange( requestStatus );
		
	//List retriever
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
							
	RBuf8 expectData1;
	CleanupClosePushL(expectData1);
	TInt i;
    TMockLtsyData1<TInt> expExpect1(i);
    expExpect1.SerialiseL(expectData1);
    
	RBuf8 CompleteData;
	CleanupClosePushL(CompleteData);
	
	TRequestStatus aReqStatus;
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
    
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE;i++)
		{
		existingParams.iTmgi.SetServiceId(i); 
	    existingParams.iTmgi.SetMCC(i);
	    existingParams.iTmgi.SetMNC(i);
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( existingParams );
		}
	
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, expectData1);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, CompleteData);

	// PacketService::UpdateMbmsMonitorServiceListL		
	packetService.UpdateMbmsMonitorServiceListL(aReqStatus, EAddEntries, multipleEntries);	
	
	// wait for completion of UpdateMbmsMonitorServiceListL(	
	User::WaitForRequest(aReqStatus);
	// wait for completion of NotifyMbmsServiceAvailabilityChange
	User::WaitForRequest( requestStatus );
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS( KErrNone, aReqStatus.Int() );

	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS(MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate() );

	monitorList->Start( activeRetriever->Status() );
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS( 0, CActiveRetriever::ResetRequestsNumber() );
	ASSERT_EQUALS( KErrNone, activeRetriever->iStatus.Int() );
	
	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
		
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual( *multipleEntries, *list ) );       
            	
	//-------------------------------------------------------------------------
	// Test add entries which already exists
 	//-------------------------------------------------------------------------

	// clear list
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, expectData1);
	TInt complete = 1;
 	TMockLtsyData1< TInt > dataForComplete(complete);
    dataForComplete.SerialiseL(CompleteData);
   	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, CompleteData);
   	CPcktMbmsMonitoredServiceList* nullPtr = NULL;	
	packetService.UpdateMbmsMonitorServiceListL(aReqStatus, ERemoveAllEntries, nullPtr );	
	User::WaitForRequest(aReqStatus);
	ASSERT_EQUALS( KErrNone, aReqStatus.Int() );
	
	// Add entry twice
	complete = 1;
 	dataForComplete.SerialiseL(CompleteData);
 	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, expectData1);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, CompleteData);

	// clear list
	CPcktMbmsMonitoredServiceList* oneEntry = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(oneEntry);
	
	TInt ix = 0;
	while( oneEntry->Enumerate() > 0 )
		{
		oneEntry->DeleteEntryL(ix);	
		}
	RPacketService::TMbmsServiceAvailabilityV1 param;
	param.iTmgi.SetServiceId(0);
	oneEntry->AddEntryL( param );
 	packetService.UpdateMbmsMonitorServiceListL(requestStatus, EAddEntries, oneEntry);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	packetService.UpdateMbmsMonitorServiceListL(requestStatus, EAddEntries, oneEntry);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());	
	
	
 	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	// ensure that Services is added correctly into the monitored service list		
	ASSERT_EQUALS(MBMS_MONITORSERVICECOUNTONE, list->Enumerate() );
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*oneEntry, *list) );

	
	//
	// TEST END
	//	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 12, this ); // packetMbmsContext, packetService, this 
     }
    
