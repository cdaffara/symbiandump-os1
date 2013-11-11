// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file The TEFUnit test suite for PacketService in the Common TSY.
*/

#include "cctsypacketservicefu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

#include <pcktcs.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "cctsyactiveretriever.h"
#include "Cmmpacketservicetsy.h"
#include "pcktretrieve.h"
#include "e32property.h"
#include "cmockltsyfactory.h"
#include "utils.h"

#define MBMS_MONITORSERVICECOUNTZERO						0
#define MBMS_MONITORSERVICECOUNTONE							1
#define MBMS_MONITORSERVICECOUNTMULTIPLE					3

const TInt KOneSecond=1000000;  // Used in a time out function, 1 second (in microSeconds)

CTestSuite* CCTsyPacketServiceFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetPreferredBearer0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetPreferredBearer0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextAdded0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextAdded0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextAdded0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyStatusChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyStatusChange0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyStatusChange0004aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyStatusChange0001aL);	
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetPreferredBearer0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetPreferredBearer0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetPreferredBearer0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetContextNameInNif0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetContextNameInNif0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetContextNameInNif0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMSClassChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMSClassChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMSClassChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetCurrentReleaseMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateContextsInNif0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateContextsInNif0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateContextsInNif0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyAttachModeChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetDefaultContextParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetDefaultContextParams0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetDefaultContextParams0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyDynamicCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyDynamicCapsChange0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyDynamicCapsChange0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyDynamicCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyDynamicCapsChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyReleaseModeChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestRejectActivationRequest0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestRejectActivationRequest0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestRejectActivationRequest0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestRejectActivationRequest0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetDefaultContextParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetDefaultContextParams0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetDefaultContextParams0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetDefaultContextParams0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetDefaultContextParams0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyChangeOfNtwkRegStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyChangeOfNtwkRegStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyChangeOfNtwkRegStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextActivationRequested0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextActivationRequested0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextActivationRequested0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextActivationRequested0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyContextActivationRequested0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestAttach0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestAttach0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestAttach0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestAttach0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestDetach0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestDetach0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestDetach0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestDetach0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestOpen0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestOpen0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestOpen0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMSClass0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMSClass0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetNifInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetNifInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetNifInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetMSClass0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestDeactivateNIF0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateNifs0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateNifs0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetAttachMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetAttachMode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetAttachMode0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetStaticCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetStaticCaps0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetStaticCaps0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetAttachMode0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetAttachMode0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestSetAttachMode0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetContextInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetContextInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetContextInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetDynamicCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetDynamicCaps0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetNtwkRegStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetNtwkRegStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetNtwkRegStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateContexts0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateContexts0004L);
	
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestCompleteNotifyEGprsInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestCompleteSetAlwaysOn0001L);
	
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestTsyMbmsSupportCaps0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestTsyMbmsSupportCaps0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMbmsNetworkServiceStatusChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMbmsNetworkServiceStatusChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMbmsNetworkServiceStatusChange0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMbmsNetworkServiceStatus0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMbmsNetworkServiceStatus0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMbmsNetworkServiceStatus0001cL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMbmsNetworkServiceStatus0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMbmsNetworkServiceStatus0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestGetMbmsNetworkServiceStatus0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMbmsServiceAvailabilityChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMbmsServiceAvailabilityChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestNotifyMbmsServiceAvailabilityChange0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsMonitorServiceList0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsMonitorServiceList0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsMonitorServiceList0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsMonitorServiceList0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestStart0001L); 
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestStart0002L); 
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateMbmsMonitorServiceList0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateMbmsMonitorServiceList0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateMbmsMonitorServiceList0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateMbmsActiveServiceList0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestEnumerateMbmsActiveServiceList0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsSessionList0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsSessionList0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsSessionList0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestUpdateMbmsSessionList0005L);
    
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestRetrievePcktMbmsSessionListStart0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketServiceFU, TestRetrievePcktMbmsSessionListStart0002L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGPB-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetPreferredBearer
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetPreferredBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetPreferredBearer0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	//-------------------------------------------------------------------------
	// TEST D: RPacketService::GetPreferredBearer again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------	 		
 	TRequestStatus	requestStatus;
	RPacketService::TPreferredBearer bearer;
	packetService.GetPreferredBearer(requestStatus, bearer);
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketService::EBearerPacketSwitched, bearer);
	
	// --- change PreferredBearer -----------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// data for setting
	RPacketService::TPreferredBearer newBearer = RPacketService::EBearerCircuitSwitched;
	TMockLtsyData1 <RPacketService::TPreferredBearer> ltsyData(newBearer);
	ltsyData.SerialiseL(data);		
	
	iMockLTSY.CompleteL(EPacketNotifyAttachModeChange, KErrNone, data);	
	User::WaitForRequest(mockLtsyStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
		
	//-------------------------------------------------------------------------
	// TEST D2: RPacketService::GetPreferredBearer again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------	 
	packetService.GetPreferredBearer(requestStatus, bearer);
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(newBearer, bearer);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGPB-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetPreferredBearer
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetPreferredBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetPreferredBearer0004L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);	
	CleanupClosePushL(packetService2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetPreferredBearer
 	//-------------------------------------------------------------------------

	TRequestStatus	requestStatus;
	RPacketService::TPreferredBearer bearer;
	
	TRequestStatus	requestStatus2;
	RPacketService::TPreferredBearer bearer2;
	
	packetService.GetPreferredBearer(requestStatus, bearer);
	packetService2.GetPreferredBearer(requestStatus2, bearer2);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketService::EBearerPacketSwitched, bearer);
	
	User::WaitForRequest(requestStatus2);	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(RPacketService::EBearerPacketSwitched, bearer2);
	
	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCA-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyContextAdded
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyContextAdded
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextAdded0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	 
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus mockLtsyStatus;
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion 
	// from LTSY.
 	//-------------------------------------------------------------------------
 	// Data for CompleteL
	TInfoName sndName;
	_LIT(Kname, "newName");
	sndName.Copy(Kname);	
	TInt sndResult = 0;
	TMockLtsyData2 <TInfoName, TInt > ltsyData(sndName, sndResult);
	ltsyData.SerialiseL(data);	
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyContextAdded when result is not cached.
 	//------------------------------------------------------------------------- 	
 
	TRequestStatus	requestStatus;
	TInfoName contextId;
	
	// sent request
	packetService.NotifyContextAdded(requestStatus, contextId);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);	
	
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	// wait for request
	User::WaitForRequest(requestStatus);	
	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sndName, contextId);	
	
	//-------------------------------------------------------------------------
	// TEST First call of API when sndResult != KErrNone and empty name is used
 	//------------------------------------------------------------------------- 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus); 		
	sndResult = KErrGeneral;
	sndName.Zero();
	TMockLtsyData2 <TInfoName, TInt > ltsyData2(sndName, sndResult);
	data.Close();
	ltsyData2.SerialiseL(data);	

	// sent request
	packetService.NotifyContextAdded(requestStatus, contextId);
				
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);	
	
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	// wait for request
	User::WaitForRequest(requestStatus);	
	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	sndName.Append(KStringExternal);
	ASSERT_EQUALS(sndName, contextId);
		
		
	//-------------------------------------------------------------------------
	// TEST Second call of API when sndResult != KErrNone and empty name is used 
 	//------------------------------------------------------------------------- 
 	 		
 	iMockLTSY.NotifyTerminated(mockLtsyStatus); 		
	sndResult = KErrGeneral;
	sndName.Zero();
	TMockLtsyData2 <TInfoName, TInt > ltsyData3(sndName, sndResult);
	data.Close();
	ltsyData3.SerialiseL(data);

	// sent request
	packetService.NotifyContextAdded(requestStatus, contextId);
				
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);	
	
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	// wait for request
	User::WaitForRequest(requestStatus);	
	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	sndName.Append(KStringExternal2);
	ASSERT_EQUALS(sndName, contextId);
	
	
	//-------------------------------------------------------------------------
	// TEST Call API when sndResult != KErrNone and non-empty name is used 
 	//------------------------------------------------------------------------- 	
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus); 		
	sndResult = KErrGeneral;
	_LIT(Kname2, "NN");
	sndName.Copy(Kname2);
	TMockLtsyData2 <TInfoName, TInt > ltsyData4(sndName, sndResult);
	data.Close();
	ltsyData4.SerialiseL(data);

	// sent request
	packetService.NotifyContextAdded(requestStatus, contextId);
				
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);	
	
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	// wait for request
	User::WaitForRequest(requestStatus);	
	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	sndName.Copy(KStringExternal2);
	ASSERT_EQUALS(sndName, contextId);	
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(1); // packetService 
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCA-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyContextAdded
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyContextAdded
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextAdded0002L()
	{
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyContextAdded
 	//-------------------------------------------------------------------------
 		
 	TRequestStatus	requestStatus;
	TInfoName contextId;

	packetService.NotifyContextAdded(requestStatus, contextId);
    packetService.CancelAsyncRequest(EPacketNotifyContextAdded);
			 	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	CleanupStack::PopAndDestroy(2); // packetService, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCA-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyContextAdded
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyContextAdded
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextAdded0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyContextAdded
 	//-------------------------------------------------------------------------

	// Data for CompleteL
	TInfoName sndName;
	_LIT(Kname, "newName");
	sndName.Copy(Kname);	
	TInt sndResult = 0;
	TMockLtsyData2 <TInfoName, TInt > ltsyData(sndName, sndResult);
	ltsyData.SerialiseL(data);	

	TRequestStatus	requestStatus;
	TInfoName contextId;	
	// sent first request
	packetService.NotifyContextAdded(requestStatus, contextId);
	
	TRequestStatus	requestStatus2;
	TInfoName contextId2;
	// sent second request
	packetService2.NotifyContextAdded(requestStatus2, contextId2);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);		
		
	// wait for the first request
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sndName, contextId);
	
	// wait for the second request
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sndName, contextId2);

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}




/**
@SYMTestCaseID BA-CTSY-PKTS-PSNSC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

	// Data for CompleteL
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);	
		
	TRequestStatus requestStatus;
	RPacketService::TStatus contextStatus;
	//send request	
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);
	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	// wait for request
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::NotifyStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------
 
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	 	
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST E_1: Unsolicited completion of RPacketService::NotifyStatusChange
	// from LTSY. When isresumed == ETrue
 	//------------------------------------------------------------------------- 
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
 	isResumed = ETrue;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData0(sendStatus, isResumed);
	data.Close();
	ltsyData0.SerialiseL(data);	 	 	 	
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//-------------------------------------------------------------------------
	// TEST E_2: Unsolicited completion of RPacketService::NotifyStatusChange
	// from LTSY. When Packet is Suspended  
 	//-------------------------------------------------------------------------
 	// Change RPacketService status  to RPacketService::EStatusSuspended 		
	sendStatus = RPacketService::EStatusSuspended;
	isResumed = EFalse;	
	TInt err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);	
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 	
	isResumed = ETrue;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData2(sendStatus, isResumed);
	data.Close();
	ltsyData2.SerialiseL(data);	 	
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	
	//-------------------------------------------------------------------------
	// TEST E_3: Unsolicited completion of RPacketService::NotifyStatusChange
	// from LTSY. When Packet is Suspended, has RPacketContext and there are iDialUpContext 
	// and iSecondaryDialUpContext.
 	//-------------------------------------------------------------------------
 	
 	// ----  Cause iDialUpContext to be created --------------
 	iMockLTSY.NotifyTerminated(mockLtsyStatus); 		
	TInt sndResult = KErrGeneral;
	TInfoName sndName;
	sndName.Zero();
	TMockLtsyData2 <TInfoName, TInt > ltsyData3(sndName, sndResult);
	data.Close();
	ltsyData3.SerialiseL(data);	
	// Issue the Complete to create iDialUpContext
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);		
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);	
	CleanupClosePushL(packetContext);
 	
 	// ----  Cause iSecondaryDialUpContext to be created --------------
 	sndName.Zero();
 	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete to create iSecondaryDialUpContext
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);		
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	
 	// Change RPacketService status  to RPacketService::EStatusSuspended 		
	sendStatus = RPacketService::EStatusSuspended;
	isResumed = EFalse;	
	err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 	
	isResumed = ETrue;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData4(sendStatus, isResumed);
	data.Close();
	ltsyData4.SerialiseL(data);	 	
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
 	AssertMockLtsyStatusL();
 	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNSC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyStatusChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyStatusChange
 	//------------------------------------------------------------------------- 	
	// Data for CompleteL
	RPacketService::TStatus sendStatus = RPacketService::EStatusSuspended;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	RPacketService::TStatus contextStatus;
	
	//send request	
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	// Cancel request
    packetService.CancelAsyncRequest(EPacketNotifyStatusChange);
		
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);
	 	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);		
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(1); // packetService 
	CleanupStack::PopAndDestroy(2); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSNSC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyStatusChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyStatusChange
 	//-------------------------------------------------------------------------

	// Data for CompleteL
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	RPacketService::TStatus contextStatus;	
	// sent first request
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	
	TRequestStatus requestStatus2;
	RPacketService::TStatus contextStatus2;
	// sent second request
	packetService2.NotifyStatusChange(requestStatus2, contextStatus2);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);		
		
	// wait for the first request
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);	
	// wait for the second request
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendStatus, contextStatus2);

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNSC-0004a
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyStatusChange0004aL()
    {
                    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(iTelServer,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);
    
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(packetService2);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RPacketService::NotifyStatusChange
    //-------------------------------------------------------------------------

    // Data for CompleteL
    RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
    TBool isResumed = EFalse;
    TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
    ltsyData.SerialiseL(data);  
    
    TRequestStatus requestStatus;
    RPacketService::TStatus contextStatus;  
    // sent first request
    packetService.NotifyStatusChange(requestStatus, contextStatus);
    
    TRequestStatus requestStatus2;
    RPacketService::TStatus contextStatus2;
    // sent second request
    packetService2.NotifyStatusChange(requestStatus2, contextStatus2);
    
    // Issue the Complete...    
    iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);     
        
    // wait for the first request
    User::WaitForRequest(requestStatus);        
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(sendStatus, contextStatus);   
    // wait for the second request
    User::WaitForRequest(requestStatus2);       
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(sendStatus, contextStatus2);

    AssertMockLtsyStatusL();
    
    // Activate both clients again
    packetService.NotifyStatusChange(requestStatus, contextStatus);
    User::After(KOneSecond);
    packetService2.NotifyStatusChange(requestStatus2, contextStatus2);
    User::After(KOneSecond);
    
    // Send cancel to the first client
    packetService.CancelAsyncRequest(EPacketNotifyStatusChange);
    User::WaitForRequest(requestStatus);       
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
    
    AssertMockLtsyStatusL();
    
    CleanupStack::PopAndDestroy(2); // packetService, packetService2
    CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNSC-0001a
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyStatusChange request which is completed with error. 
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyStatusChange0001aL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: RPacketService::TStatus changes from RPacketService::EStatusAttached
	// to RPacketService::EStatusUnattached with error KErrGsmPSConnectionBarred
 	//-------------------------------------------------------------------------

	//Data for CompleteL
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool> ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(completeData);	
		
	RPacketService::TStatus contextStatus;
    
    //Send request		
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	
	//Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, completeData);
	
	//NotifyStatusChange should be completed with status RPacketService::EStatusAttached
	//and KErrNone
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
		
	//Data for CompleteL
	sendStatus = RPacketService::EStatusUnattached;
	isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool> ltsyData2(sendStatus, isResumed);
	completeData.Close();
	ltsyData2.SerialiseL(completeData);	

    //Send request	
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	
	//Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrGsmPSConnectionBarred, completeData);
	
	//NotifyStatusChange should be completed with status RPacketService::EStatusUnattached
	//and KErrGsmPSConnectionBarred
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmPSConnectionBarred, requestStatus.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);	
	
	//-------------------------------------------------------------------------
	// Test B: RPacketService::TStatus changes from RPacketService::EStatusAttached
	// to RPacketService::EStatusSuspended with error KErrGsmCSConnectionBarred
 	//-------------------------------------------------------------------------

	//Data for CompleteL
	sendStatus = RPacketService::EStatusAttached;
	completeData.Close();
	ltsyData.SerialiseL(completeData);	
		
	//Send request	
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	
	//Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, completeData);
	
	//NotifyStatusChange should be completed with status RPacketService::EStatusAttached
	//and KErrNone
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);
		
	//Data for CompleteL
	sendStatus = RPacketService::EStatusSuspended;
	isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool> ltsyData3(sendStatus, isResumed);
	completeData.Close();
	ltsyData3.SerialiseL(completeData);	

	//Send request	
	packetService.NotifyStatusChange(requestStatus, contextStatus);
	
	//Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrGsmCSConnectionBarred, completeData);
	
	//NotifyStatusChange should be completed with status RPacketService::EStatusUnattached
	//and KErrGsmCSConnectionBarred
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmCSConnectionBarred, requestStatus.Int());
	ASSERT_EQUALS(sendStatus, contextStatus);			
	
	CleanupStack::PopAndDestroy(3, this); // packetService, completeData, this	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSSPB-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetPreferredBearer
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetPreferredBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetPreferredBearer0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	TRequestStatus	requestStatus;
	RPacketService::TPreferredBearer bearer = RPacketService::EBearerCircuitSwitched; 
	// data for ExpectL
	TMockLtsyData1<RPacketService::TPreferredBearer> expPrefBearerPckg(bearer);
	expPrefBearerPckg.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketSetPrefBearer, data, KErrNotSupported);
	
	packetService.SetPreferredBearer(requestStatus, bearer);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketSetPrefBearer, data);
	iMockLTSY.CompleteL(EPacketSetPrefBearer, KErrGeneral);	
	
	
	packetService.SetPreferredBearer(requestStatus, bearer);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::SetPreferredBearer when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketSetPrefBearer, data);
	iMockLTSY.CompleteL(EPacketSetPrefBearer, KErrNone);	

	packetService.SetPreferredBearer(requestStatus, bearer);	
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::SetPreferredBearer
	// from LTSY.
 	//-------------------------------------------------------------------------
 	
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	
	iMockLTSY.CompleteL(EPacketSetPrefBearer, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSPB-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::SetPreferredBearer
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::SetPreferredBearer
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetPreferredBearer0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::SetPreferredBearer
 	//-------------------------------------------------------------------------
	
	TRequestStatus	requestStatus;
	RPacketService::TPreferredBearer bearer = RPacketService::EBearerCircuitSwitched; 
	
	TRequestStatus	requestStatus2;
	RPacketService::TPreferredBearer bearer2 = RPacketService::EBearerPacketSwitched;
	
	// data for ExpectL
	TMockLtsyData1<RPacketService::TPreferredBearer> expPrefBearerPckg(bearer);
	expPrefBearerPckg.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketSetPrefBearer, data);
	iMockLTSY.CompleteL(EPacketSetPrefBearer, KErrNone);	
	
	// send first request
	packetService.SetPreferredBearer(requestStatus, bearer);	
	packetService2.SetPreferredBearer(requestStatus2, bearer2);
	
	//wait for first answer
	User::WaitForRequest(requestStatus); 	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//wait for second answer
	User::WaitForRequest(requestStatus2); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	CleanupStack::PopAndDestroy(2);	 //packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSPB-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetPreferredBearer with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetPreferredBearer and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetPreferredBearer0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::SetPreferredBearer
 	//-------------------------------------------------------------------------
 	TRequestStatus	requestStatus;
	RPacketService::TPreferredBearer bearer = RPacketService::EBearerCircuitSwitched; 	
	
	// data for ExpectL
	TMockLtsyData1<RPacketService::TPreferredBearer> expPrefBearerPckg(bearer);
	expPrefBearerPckg.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketSetPrefBearer, data);

	packetService.SetPreferredBearer(requestStatus, bearer);	
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(1); //packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCNIN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetContextNameInNif
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetContextNameInNif
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetContextNameInNif0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetContextNameInNif when result is not cached.
 	//-------------------------------------------------------------------------	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext); 
	
	// Open one more  context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService, contextId2);
	CleanupClosePushL(packetContext2);
	
	TRequestStatus	requestStatus;
	TInfoName existingContextName;
	existingContextName.Copy(contextId);
	TInfoName contextName;
	TInt index = 1;	// ask for the second context's name
		
	packetService.GetContextNameInNif(requestStatus, existingContextName, index, contextName);
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(contextId2, contextName);	 	

	//----------------------------------------------------------------------------
	// Test with context with different PDP address
	//----------------------------------------------------------------------------
	// Open third context
	TInfoName contextId3;	
	RPacketContext packetContext3;
	packetContext3.OpenNewContext(packetService, contextId3);
	CleanupClosePushL(packetContext3);
	
	//initialisation of the third context
	TInt err = InitContextL (packetContext3, contextId3);
	ASSERT_EQUALS(KErrNone, err);
		
	// data for setting another PDP adddress
	RPacketContext::TContextConfigGPRS contextConfig;
	_LIT(KPdpAdd, "Some Add");
	contextConfig.iPdpAddress.Copy(KPdpAdd);
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);	
	TRequestStatus	setConfigStatus;
	
	// data for ExpectL
	TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(contextConfig, contextId3);
    expExpect.SerialiseL(expectData);
    // data for CompleteL
    TMockLtsyData1<TInfoName > ltsyData(contextId3);
    ltsyData.SerialiseL(data);
		
	iMockLTSY.ExpectL(EPacketContextSetConfig,expectData);				
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);	
	// set another PDP adddress		
	packetContext3.SetConfig(setConfigStatus, pckgConfig);	
	// wait for SetConfig
	User::WaitForRequest(setConfigStatus);
	ASSERT_EQUALS(KErrNone, setConfigStatus.Int());
				
	// ask for the third context's name
	index = 2;			
	packetService.GetContextNameInNif(requestStatus, existingContextName, index, contextName);
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); //packetContext3, packetContext2, packetContext, packetService
	CleanupStack::PopAndDestroy(3, this); // expectData, data, this		
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCNIN-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetContextNameInNif with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetContextNameInNif with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetContextNameInNif0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketService::GetContextNameInNif
 	//------------------------------------------------------------------------- 	

	// -- Case with wrong existingContextName ---------------------------------
	TRequestStatus	requestStatus;
	_LIT(KWrongName, "SomeName");
	TBuf <0x100> existingContextName(KWrongName);	
	TInfoName contextName;
	TInt index = 0;
	
	packetService.GetContextNameInNif(requestStatus, existingContextName, index, contextName);
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	// -- Case with wrong index ---------------------------------
	existingContextName.Copy(contextId);	
	index = -1;
	
	packetService.GetContextNameInNif(requestStatus, existingContextName, index, contextName);
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketService::GetContextNameInNif
 	//------------------------------------------------------------------------- 
 	
	// ----------- Wrong Size of fourth parameter -----------------------------
 	existingContextName.Copy(contextId);
	TBuf <1> contextName2; // descriptor with wrong size
	index = 0;
	
	packetService.GetContextNameInNif(requestStatus, existingContextName, index, contextName2);
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
 	 	
 	// ----------- Wrong Size of second parameter ----------------------------	
 	_LIT(KVeryLongName, "Some_Very_Very_Very_Very_Very_Very_Very_LONG_Name");
 	TBuf <0x100> existingContextName3(KVeryLongName);
	index = 0;
		
	packetService.GetContextNameInNif(requestStatus, existingContextName3, index, contextName);
	User::WaitForRequest(requestStatus); 	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());		
	
	CleanupStack::PopAndDestroy(2); //packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCNIN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetContextNameInNif
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetContextNameInNif
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetContextNameInNif0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);
	
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetContextNameInNif
 	//-------------------------------------------------------------------------

	TRequestStatus	requestStatus;
	TInfoName existingContextName;
	existingContextName.Copy(contextId);
	TInfoName contextName;
	TInt index = 0;	
	//send first request		
	packetService.GetContextNameInNif(requestStatus, existingContextName, index, contextName);
	
	TRequestStatus	requestStatus2;
	TInfoName contextName2;
	//send second request		
	packetService2.GetContextNameInNif(requestStatus2, existingContextName, index, contextName2);
	
	// wait for first answer	
	User::WaitForRequest(requestStatus); 		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(contextId, contextName);	 

	// wait for second answer	
	User::WaitForRequest(requestStatus2); 		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(contextId, contextName2);
	
	CleanupStack::PopAndDestroy(3); //packetContext, packetService2, packetService
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNMCC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyMSClassChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyMSClassChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMSClassChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	
  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyMSClassChange when result is not cached.
 	//-------------------------------------------------------------------------

	// Data for CompleteL	
	RPacketService::TMSClass sendClass = RPacketService::EMSClassSuspensionRequired;
	TMockLtsyData1 < RPacketService::TMSClass > ltsyData(sendClass);
	ltsyData.SerialiseL(data);		
	
	TRequestStatus requestStatus;
	RPacketService::TMSClass msClass;
	//send request
	packetService.NotifyMSClassChange(requestStatus, msClass);
		
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyMSClassChange, KErrNone, data);

	// wait for CompleteL		
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for request
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendClass, msClass);

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNMCC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyMSClassChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyMSClassChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMSClassChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();	
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyMSClassChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus requestStatus;
	RPacketService::TMSClass msClass;

	packetService.NotifyMSClassChange(requestStatus, msClass);
    packetService.CancelAsyncRequest(EPacketNotifyMSClassChange);
    			 	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	CleanupStack::PopAndDestroy(2); // packetService, this
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSNMCC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyMSClassChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyMSClassChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMSClassChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyMSClassChange
 	//-------------------------------------------------------------------------

	// Data for CompleteL
	RPacketService::TMSClass sendClass = RPacketService::EMSClassPacketSwitchedOnly;
	TMockLtsyData1 < RPacketService::TMSClass > ltsyData(sendClass);
	ltsyData.SerialiseL(data);
	
	TRequestStatus requestStatus;
	RPacketService::TMSClass msClass;	
	// sent first request
	packetService.NotifyMSClassChange(requestStatus, msClass);
	
	TRequestStatus requestStatus2;
	RPacketService::TMSClass msClass2;
	// sent second request
	packetService2.NotifyMSClassChange(requestStatus2, msClass2);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyMSClassChange, KErrNone, data);	
		
	// wait for the first request
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendClass, msClass);	
	// wait for the second request
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendClass, msClass2);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCRM-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetCurrentReleaseMode
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetCurrentReleaseMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetCurrentReleaseMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Test API is not supported
	TRequestStatus requestStatus;
	RPacketService::TPacketReleaseMode releaseMode;
	
	packetService.GetCurrentReleaseMode(requestStatus, releaseMode);
	User::WaitForRequest(requestStatus);			
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSECIN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::EnumerateContextsInNif
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateContextsInNif
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateContextsInNif0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus requestStatus;	
	
	//-------------------------------------------------------------------------
	// TEST EnumerateContextsInNif without any Context
 	//-------------------------------------------------------------------------
	TInt count;		
	TInfoName someContext(_L("SomeName"));
	packetService.EnumerateContextsInNif(requestStatus, someContext, count);
	User::WaitForRequest(requestStatus);			
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(0, count);	
		
	// open first context
	TInfoName contextId; 
 	RPacketContext packetContext;
 	packetContext.OpenNewContext(packetService, contextId);
 	CleanupClosePushL(packetContext);	 	
 	 	
 	// open second context
	TInfoName contextId2; 
 	RPacketContext packetContext2;
 	packetContext2.OpenNewContext(packetService, contextId2);
 	CleanupClosePushL(packetContext2);
 	
	InitContextL(packetContext2, contextId2);
    // ------- change PdpAddress for second context ---------------------------
    RPacketContext::TContextConfigGPRS contextConfig;
    _LIT(KPdpAdd, "Some Add");
    contextConfig.iPdpAddress.Copy(KPdpAdd);
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);      

	TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(contextConfig, contextId2);
    expExpect.SerialiseL(data);
    iMockLTSY.ExpectL(EPacketContextSetConfig, data);

    TMockLtsyData1<TInfoName > contexName(contextId2);
    data.Close();
    contexName.SerialiseL(data);        
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    packetContext2.SetConfig(requestStatus, pckgConfig);    
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	// -------------------------------------------------------------------------
 	
 	// open third context
	TInfoName contextId3; 
 	RPacketContext packetContext3;
 	packetContext3.OpenNewContext(packetService, contextId3);
 	CleanupClosePushL(packetContext3);
 	 	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::EnumerateContextsInNif when result is not cached.
 	//-------------------------------------------------------------------------
	packetService.EnumerateContextsInNif(requestStatus, contextId, count);
	User::WaitForRequest(requestStatus);			
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(2, count);	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); //packetContext3, packetContext2, packetContext, packetService, data, this	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSECIN-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::EnumerateContextsInNif with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateContextsInNif with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateContextsInNif0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// open new context
	TInfoName contextId; 
 	RPacketContext packetContext;
 	packetContext.OpenNewContext(packetService, contextId);
 	CleanupClosePushL(packetContext);

	//-------------------------------------------------------------------------
	// Test A: Test passing out of bounds parameters to
	// RPacketService::EnumerateContextsInNif
 	//------------------------------------------------------------------------- 	
	TRequestStatus requestStatus;
	TInt count;
 	_LIT(KLongName, "Very_very_very_very_very_very_very_very_very_very_very_LongName");
 	TBuf <0x100> someLongName(KLongName);		
	// test wrong context' name
	packetService.EnumerateContextsInNif(requestStatus, someLongName, count);
	User::WaitForRequest(requestStatus);			
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSECIN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::EnumerateContextsInNif
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::EnumerateContextsInNif
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateContextsInNif0004L()
	{
						
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);	
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::EnumerateContextsInNif
 	//-------------------------------------------------------------------------
	
	TRequestStatus requestStatus;
	TInt count;
	//send first request		
	packetService.EnumerateContextsInNif(requestStatus, contextId, count);
	
	TRequestStatus requestStatus2;
	TInt count2;
	//send second request		
	packetService2.EnumerateContextsInNif(requestStatus2, contextId, count2);
	
	// wait for first answer	
	User::WaitForRequest(requestStatus); 		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(1, count);	 

	// wait for second answer	
	User::WaitForRequest(requestStatus2); 		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(1, count2);
	
	CleanupStack::PopAndDestroy(3); //packetContext, packetService2, packetService
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNAMC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyAttachModeChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyAttachModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyAttachModeChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService); 

	// Post request
	TRequestStatus requestStatus;
	RPacketService::TAttachMode attachMode;		
		
	// check that this request isn't supported by CTSY
	packetService.NotifyAttachModeChange(requestStatus, attachMode);
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(); //packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGDCP-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetDefaultContextParams
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetDefaultContextParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetDefaultContextParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS packetData;
	TPckg <RPacketContext::TContextConfigGPRS> pckg(packetData);		

    // -----------------------------------------------------------------------
    // Send request, when there is no DefaultContextParams in CTSY
    // -----------------------------------------------------------------------    	
	packetService.GetDefaultContextParams(requestStatus, pckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());			 	

 	//-------------------------------------------------------------------------
	// TEST C1: Successful completion request of
	// RPacketService::GetDefaultContextParams when result is not cached
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// generate completion
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 
	
	packetService.GetDefaultContextParams(requestStatus, pckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, pckg().iPdpType);
	ASSERT_EQUALS((TUint)0, pckg().iPdpCompression);	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RPacketService::GetDefaultContextParams when result is not cached
 	//-------------------------------------------------------------------------
	RPacketContext::TContextConfigR99_R4 packetData2;
	TPckg <RPacketContext::TContextConfigR99_R4> pckg2(packetData2);
	
	packetService.GetDefaultContextParams(requestStatus, pckg2);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, pckg().iPdpType);
	ASSERT_EQUALS((TUint)0, pckg().iPdpCompression);	
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// TEST C3: Successful completion request of
	// RPacketService::GetDefaultContextParams when result is not cached
 	//-------------------------------------------------------------------------
	RPacketContext::TContextConfig_R5 packetData3;
	TPckg <RPacketContext::TContextConfig_R5> pckg3(packetData3);
	
	packetService.GetDefaultContextParams(requestStatus, pckg3);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(2, this); // packetService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGDCP-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetDefaultContextParams with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetDefaultContextParams with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetDefaultContextParams0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus requestStatus;
	TPacketDataConfigBase packetData;
	TPckg <TPacketDataConfigBase> pckg(packetData);		

    //-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketService::GetDefaultContextParams
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// generate completion
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 
	
	packetService.GetDefaultContextParams(requestStatus, pckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGDCP-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetDefaultContextParams
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetDefaultContextParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetDefaultContextParams0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetDefaultContextParams
 	//-------------------------------------------------------------------------

	// data for first request
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS packetData;
	TPckg <RPacketContext::TContextConfigGPRS> pckg(packetData);
	
	// data for second request
	TRequestStatus requestStatus2;
	RPacketContext::TContextConfigR99_R4 packetData2;
	TPckg <RPacketContext::TContextConfigR99_R4> pckg2(packetData2);
		
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// generate completion
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 
		
	// send first request 
	packetService.GetDefaultContextParams(requestStatus, pckg);
	
	// send second request 
	packetService2.GetDefaultContextParams(requestStatus2, pckg2);
	
	// wait for first answer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, pckg().iPdpType);
	ASSERT_EQUALS((TUint)0, pckg().iPdpCompression);
	
	// wait for second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, pckg2().iPdpType);

	CleanupStack::PopAndDestroy(2); // packetService2, packetService
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNDCC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyDynamicCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyDynamicCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyDynamicCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyDynamicCapsChange when result is not cached.
	// using  CMmPacketServiceGsmWcdmaExt::CompleteDynamicCapsChange function	
 	//-------------------------------------------------------------------------	
	// data for CompleteL	
	TBool gprsIsSupportedOnCell = EFalse;
	TBool csServicesIsSupportedOnCell = ETrue;
	TMockLtsyData2 <TBool, TBool> ltsyData(gprsIsSupportedOnCell, csServicesIsSupportedOnCell);
	ltsyData.SerialiseL(data);	
			
	TRequestStatus requestStatus;
	RPacketService::TDynamicCapsFlags caps;
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS((TUint)RPacketService::KCapsRxCSCall, caps);

	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyDynamicCapsChange when result is not cached.
	// using  CMmPacketServiceGsmWcdmaExt::SetDynamicCapsFlag function	
 	//-------------------------------------------------------------------------	
	RPacketContext::TContextConfigGPRS contextConfig;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigGPRS > ltsyData1(contextId, contextConfig);
	data.Close();
	ltsyData1.SerialiseL(data);
				
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketContextActivate, KErrGeneral, data);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS((TUint)(RPacketService::KCapsRxCSCall | RPacketService::KCapsActivate), caps);		

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyDynamicCapsChange when result is not cached.
	// using  CMmPacketServiceGsmWcdmaExt::CompleteTransferCapsChange	
 	//-------------------------------------------------------------------------	
	TDynamicTransferCapsFlags transferCaps = KCapsHSDPA;
	TMockLtsyData1< TDynamicTransferCapsFlags > ltsyData2(transferCaps);
	data.Close();
	ltsyData2.SerialiseL(data);
				
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyTransferCapsIPC, KErrNone, data);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS((TUint) (RPacketService::KCapsRxCSCall |
						   RPacketService::KCapsActivate | 
						   RPacketService::KCapsHSDPA), caps);		

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
		
	}
	
	
/**
@SYMTestCaseID BA-CTSY-PKTS-PSNDCC-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyDynamicCapsChange with different RPacketService status
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyDynamicCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyDynamicCapsChange0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
  	//-------------------------------------------------------------------------
	// TEST Successful completion request of
	// RPacketService::NotifyDynamicCapsChange when result is not cached.
	// using  CMmPacketServiceGsmWcdmaExt::CompleteDynamicCapsChange function
	// with  different RPacketService status  	
 	//-------------------------------------------------------------------------	
 	 
 	// Change RPacketService status  to RPacketService::EStatusAttached 		
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;	
	TInt err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);	
 	
	// data for CompleteL	
	TBool gprsIsSupportedOnCell = ETrue;
	TBool csServicesIsSupportedOnCell = ETrue;	
	TMockLtsyData2 <TBool, TBool> ltsyData1_2(gprsIsSupportedOnCell, csServicesIsSupportedOnCell);
	ltsyData1_2.SerialiseL(data);	
			
	//send request
	TRequestStatus requestStatus;
	RPacketService::TDynamicCapsFlags caps;
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// Prepare sampleCaps
	TUint sampleCaps = 0;  // expected caps from CTSY
	// Deativate KCapsManualAttach in dynamicCaps 
    sampleCaps &= ~RPacketService::KCapsManualAttach;
    // Activate KCapsSMSTransfer, KCapsManualDetach and
    // KCapsRxContextActivationReq in dynamicCaps 
    sampleCaps |= RPacketService::KCapsSMSTransfer |
                   RPacketService::KCapsManualDetach |
                   RPacketService::KCapsRxContextActivationReq;	                   
    sampleCaps |= RPacketService::KCapsRxCSCall;
    		
	ASSERT_EQUALS(sampleCaps, caps);
	
	
	//-------------------------------------------------------------------------		
	// Change RPacketService status  to RPacketService::EStatusSuspended 		
	sendStatus = RPacketService::EStatusSuspended;
	isResumed = EFalse;	
	err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);	
	
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);			
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// Prepare sampleCaps	
	sampleCaps = 0;  // expected caps from CTSY
	sampleCaps |= RPacketService::KCapsManualDetach;      
    // Deativate KCapsSMSTransfer, KCapsManualAttach and
    // KCapsRxContextActivationReq in dynamicCaps 
    sampleCaps &= ~( RPacketService::KCapsSMSTransfer |
    				 RPacketService::KCapsManualAttach |
    				 RPacketService::KCapsRxContextActivationReq );  				   
	sampleCaps |= RPacketService::KCapsRxCSCall;
	ASSERT_EQUALS(sampleCaps, caps);	
	 
	 
	//-------------------------------------------------------------------------
	// change RPacketService status  to RPacketService::EStatusActive
	// to check status changing after status EStatusSuspended	
	sendStatus = RPacketService::EStatusActive;
	isResumed = EFalse;	
	err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);
	
	// and issue completition
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
				
	//-------------------------------------------------------------------------			
	// Now Change RPacketService status  to RPacketService::EStatusUnattached 		
	sendStatus = RPacketService::EStatusUnattached;
	isResumed = EFalse;		
	err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);	
	
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);			
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// Prepare sampleCaps		
	sampleCaps = 0;  // expected caps from CTSY
	sampleCaps |= RPacketService::KCapsManualAttach;
    sampleCaps &= ~( RPacketService::KCapsSMSTransfer |
                     RPacketService::KCapsManualDetach |
                     RPacketService::KCapsRxContextActivationReq ); 				   
	sampleCaps |= RPacketService::KCapsRxCSCall;
	ASSERT_EQUALS(sampleCaps, caps);
	
	
	
	//-------------------------------------------------------------------------		
	// Change Context status to Active
	SetContextActiveL(contextId);	
	// Change RPacketService status  to RPacketService::EStatusActive 		
	sendStatus = RPacketService::EStatusActive;
	isResumed = EFalse;	
	err = SetPacketServiceStatusL(sendStatus, isResumed);	
	ASSERT_EQUALS(KErrNone, err);	
	
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);			
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// Prepare sampleCaps
	sampleCaps = 0;  // expected caps from CTSY
	// Deativate KCapsManualAttach in dynamicCaps 
    sampleCaps &= ~RPacketService::KCapsManualAttach;
    sampleCaps |= RPacketService::KCapsSMSTransfer |
                   RPacketService::KCapsManualDetach |
                   RPacketService::KCapsRxContextActivationReq;	
	sampleCaps |= RPacketService::KCapsRxCSCall;                     
	ASSERT_EQUALS(sampleCaps, caps);	
	
	
	//-------------------------------------------------------------------------
	// TEST  Successful completion request of
	// RPacketService::NotifyDynamicCapsChange when result is not cached.
	// using  CMmPacketServiceGsmWcdmaExt::CompleteTransferCapsChange
	// with different params	
 	//-------------------------------------------------------------------------	
	TDynamicTransferCapsFlags transferCaps = KCapsEGPRS;
	TMockLtsyData1< TDynamicTransferCapsFlags > ltsyData2(transferCaps);
	data.Close();
	ltsyData2.SerialiseL(data);
				
	//send request
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyTransferCapsIPC, KErrNone, data);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	// Prepare sampleCaps
    sampleCaps &= ~RPacketService::KCapsHSDPA;
    sampleCaps |= RPacketService::KCapsRxCSCall |
                  RPacketService::KCapsRxContextActivationReq;                  
    sampleCaps |= RPacketService::KCapsEGPRS;   
	ASSERT_EQUALS(sampleCaps, caps);		
	
	//-------------------Issue the same Complete to increase coverage... -------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyTransferCapsIPC, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
		
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNDCC-0001c
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyDynamicCapsChange (HSUPA)
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyDynamicCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyDynamicCapsChange0001cL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService); 
    
    TDynamicTransferCapsFlags transferCaps = KCapsHSDPA;
	TMockLtsyData1< TDynamicTransferCapsFlags > ltsyData(transferCaps);
	data.Close();
	ltsyData.SerialiseL(data);
	
	// Send complete from LTSY before Notify request
	TRequestStatus mockLtsyStatus;	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyTransferCapsIPC, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	
	// Send notify request
	RPacketService::TDynamicCapsFlags caps;
    TRequestStatus requestStatus;
    packetService.NotifyDynamicCapsChange(requestStatus, caps);
	
	// Change caps
	transferCaps = KCapsHSDPA | KCapsHSUPA;
	TMockLtsyData1< TDynamicTransferCapsFlags > ltsyData2(transferCaps);
	data.Close();
	ltsyData2.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketNotifyTransferCapsIPC, KErrNone, data);	
    
    User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	const RPacketService::TDynamicCapsFlags KHsdpa(RPacketService::KCapsHSDPA);
    const RPacketService::TDynamicCapsFlags KHsupa(RPacketService::KCapsHSUPA);
    ASSERT_EQUALS(KHsdpa | KHsupa, caps);
    
    AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(3); // packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNDCC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyDynamicCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyDynamicCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyDynamicCapsChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyDynamicCapsChange
 	//------------------------------------------------------------------------- 	
			
	TRequestStatus requestStatus;
	RPacketService::TDynamicCapsFlags caps;

	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	packetService.CancelAsyncRequest(EPacketNotifyDynamicCapsChange);
			
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetService, this	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSNDCC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyDynamicCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyDynamicCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyDynamicCapsChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyDynamicCapsChange
 	//-------------------------------------------------------------------------
	
	// data for CompleteL	
	TBool gprsIsSupportedOnCell = ETrue;
	TBool csServicesIsSupportedOnCell = EFalse;
	TMockLtsyData2 <TBool, TBool> ltsyData(gprsIsSupportedOnCell, csServicesIsSupportedOnCell);
	ltsyData.SerialiseL(data);	
			
	// data for first request	
	TRequestStatus requestStatus;
	RPacketService::TDynamicCapsFlags caps;
	
	// data for second request	
	TRequestStatus requestStatus2;
	RPacketService::TDynamicCapsFlags caps2;
	
	packetService.NotifyDynamicCapsChange(requestStatus, caps);
	packetService2.NotifyDynamicCapsChange(requestStatus2, caps2);
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);
		
	// wait for first answer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS((TUint)RPacketService::KCapsManualAttach, caps);
	
	// wait for second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	ASSERT_EQUALS((TUint)RPacketService::KCapsManualAttach, caps2);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetService2, packetService
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNRMC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyReleaseModeChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyReleaseModeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyReleaseModeChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);		
	
	// -----------------------------------------------------------------
	// just check that this API isn't supported by CTSY
	// -----------------------------------------------------------------
	TRequestStatus requestStatus;
	RPacketService::TPacketReleaseMode releaseMode;
	packetService.NotifyReleaseModeChange(requestStatus, releaseMode);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::NotifyReleaseModeChange
	// from LTSY. 
	// This test is used for CMmPacketServiceTsy::CompleteNetworkModeChange
 	//-------------------------------------------------------------------------
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// data for CompleteL
	RMobilePhone::TMobilePhoneNetworkMode ntwkMode(
                                        RMobilePhone::ENetworkModeGsm );
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkMode> pckg(ntwkMode );
	pckg.SerialiseL(data);
	// generate completion
	iMockLTSY.CompleteL(EPacketNotifyReleaseModeChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSRAR-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::RejectActivationRequest
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::RejectActivationRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestRejectActivationRequest0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketRejectActivationRequest, KErrNotSupported);
			
	TRequestStatus requestStatus;
	packetService.RejectActivationRequest(requestStatus);	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::RejectActivationRequest when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketRejectActivationRequest);
	iMockLTSY.CompleteL(EPacketRejectActivationRequest, KErrNone);
	
	packetService.RejectActivationRequest(requestStatus);	
		
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::RejectActivationRequest
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// generate completion
	iMockLTSY.CompleteL(EPacketRejectActivationRequest, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // RPacketService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSRAR-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::RejectActivationRequest
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::RejectActivationRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestRejectActivationRequest0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::RejectActivationRequest
 	//-------------------------------------------------------------------------
 	TRequestStatus requestStatus;
 	
	iMockLTSY.ExpectL(EPacketRejectActivationRequest);
	iMockLTSY.CompleteL(EPacketRejectActivationRequest, KErrNone, 5);
	
	packetService.RejectActivationRequest(requestStatus);	
	packetService.CancelAsyncRequest(EPacketRejectActivationRequest);
		
	User::WaitForRequest(requestStatus);
	
	// TSY has started a request and it is not possible to then
    // cancel this request. TSY proceed as though the Cancel never happened. The server's 
    // call to the TSY cancel function will return synchronously.
    
    // so check that reguest completed as without CancelAsyncRequest
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(2); // packetService, this	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSRAR-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::RejectActivationRequest
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::RejectActivationRequest
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestRejectActivationRequest0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::RejectActivationRequest
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketRejectActivationRequest);
	iMockLTSY.CompleteL(EPacketRejectActivationRequest, KErrNone);
	
	// send first request
	TRequestStatus requestStatus;
	packetService.RejectActivationRequest(requestStatus);	
	
	// send second request
	TRequestStatus requestStatus2;
	packetService2.RejectActivationRequest(requestStatus2);				
	
	//wait for first answer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	//wait for second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	AssertMockLtsyStatusL();	

	CleanupStack::PopAndDestroy(2); // packetService2, packetService
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSRAR-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::RejectActivationRequest with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::RejectActivationRequest and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestRejectActivationRequest0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::RejectActivationRequest
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	iMockLTSY.ExpectL(EPacketRejectActivationRequest);
	
	packetService.RejectActivationRequest(requestStatus);	
		
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSDCP-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetDefaultContextParams
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetDefaultContextParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetDefaultContextParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS configGPRS;
	_LIT(KSameName, "AccessPointName");
	configGPRS.iAccessPointName.Copy(KSameName);
	configGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
	configGPRS.iPdpCompression = RPacketContext::KPdpHeaderCompression;	
	TPckg <RPacketContext::TContextConfigGPRS> pckg(configGPRS);	
	
	// data for ExpectL
	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expPckg(configGPRS );
	expPckg.SerialiseL(expectData);
	
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData, KErrNotSupported);
		
	packetService.SetDefaultContextParams(requestStatus, pckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData);
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrGeneral);
		
	packetService.SetDefaultContextParams(requestStatus, pckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::SetDefaultContextParams when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData);
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);
		
	packetService.SetDefaultContextParams(requestStatus, pckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	
	//-------------------------------------------------------------------------
	// TEST C_2: Successful completion request of
	// RPacketService::SetDefaultContextParams whith another params.
 	//-------------------------------------------------------------------------
 	RPacketContext::TContextConfigR99_R4 configR99;	
	_LIT(KSameName2, "AccessPointName2");
	configR99.iAccessPointName.Copy(KSameName2);
	configR99.iPdpType = RPacketContext::EPdpTypeIPv6;	
	TPckg <RPacketContext::TContextConfigR99_R4> pckg2(configR99);	
	
	// data for ExpectL
	TMockLtsyData1<RPacketContext::TContextConfigR99_R4> expPckg2(configR99);
	expectData.Close();
	expPckg2.SerialiseL(expectData);
 	
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData);
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);
		
	packetService.SetDefaultContextParams(requestStatus, pckg2);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::SetDefaultContextParams
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// generate completion
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // expectData, packetService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSDCP-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::SetDefaultContextParams
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::SetDefaultContextParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetDefaultContextParams0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TRequestStatus mockLtsyStatus;	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::SetDefaultContextParams
 	//-------------------------------------------------------------------------
 	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS configGPRS;
	TPckg <RPacketContext::TContextConfigGPRS> pckg(configGPRS);
	
	// data for ExpectL
	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expPckg(configGPRS );
	expPckg.SerialiseL(expectData);
	
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData);
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone, 5);
			
	// send request	
	packetService.SetDefaultContextParams(requestStatus, pckg);
	//cancel request	
	packetService.CancelAsyncRequest(EPacketSetDefaultContextParams);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	
	// TSY has started a request and it is not possible to then
    // cancel this request. TSY proceed as though the Cancel never happened. The server's 
    // call to the TSY cancel function will return synchronously.
    
    // so check that reguest completed as without CancelAsyncRequest	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	 	

	// Wait for completion of iMockLTSY.NotifyTerminated	
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // expectData, packetService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSDCP-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetDefaultContextParams with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetDefaultContextParams with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetDefaultContextParams0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	TRequestStatus requestStatus;
	TInt garbage;
	TPckg <TInt> pckg(garbage);	
	
	// call function with wrong argument 	
	packetService.SetDefaultContextParams(requestStatus, pckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	

	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSDCP-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::SetDefaultContextParams
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::SetDefaultContextParams
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetDefaultContextParams0004L()
	{			
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::SetDefaultContextParams
 	//-------------------------------------------------------------------------

	// data for first call of API
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS configGPRS;
	_LIT(KSameName, "AccessPointName");
	configGPRS.iAccessPointName.Copy(KSameName);
	configGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
	configGPRS.iPdpCompression = RPacketContext::KPdpHeaderCompression;	
	TPckg <RPacketContext::TContextConfigGPRS> pckg(configGPRS);	
	// data for the first ExpectL
	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expPckg(configGPRS);
	expPckg.SerialiseL(expectData);
	
	// data for second call of API
	TRequestStatus requestStatus2;
	RPacketContext::TContextConfigR99_R4 configR99;	
	TPckg <RPacketContext::TContextConfigR99_R4> pckg2(configR99);	
	// data for the second ExpectL
	TMockLtsyData1<RPacketContext::TContextConfigR99_R4> expPckg2(configR99);
	expPckg2.SerialiseL(expectData2);
	
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData);
	iMockLTSY.CompleteL(EPacketSetDefaultContextParams, KErrNone);	
		
	// send first request		
	packetService.SetDefaultContextParams(requestStatus, pckg);		

	// send second request		
	packetService2.SetDefaultContextParams(requestStatus2, pckg2);
	
	//wait for first answer	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//wait for second answer	
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();		
	CleanupStack::PopAndDestroy(4); // expectData2, expectData, packetService2, packetService
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSSDCP-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetDefaultContextParams with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetDefaultContextParams and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetDefaultContextParams0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::SetDefaultContextParams
 	//------------------------------------------------------------------------- 	
 	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS configGPRS;
	_LIT(KSameName, "AccessPointName");
	configGPRS.iAccessPointName.Copy(KSameName);
	configGPRS.iPdpType = RPacketContext::EPdpTypeIPv6;
	configGPRS.iPdpCompression = RPacketContext::KPdpHeaderCompression;	
	TPckg <RPacketContext::TContextConfigGPRS> pckg(configGPRS);
	
	// data for  ExpectL
	TMockLtsyData1<RPacketContext::TContextConfigGPRS> expPckg(configGPRS);
	expPckg.SerialiseL(expectData);
	
	iMockLTSY.ExpectL(EPacketSetDefaultContextParams, expectData);
	packetService.SetDefaultContextParams(requestStatus, pckg);	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //expectData,  packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCONRS-0001
@SYMPREQ 1551
@SYMCR 1595
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyChangeOfNtwkRegStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyChangeOfNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyChangeOfNtwkRegStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
 	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	//-------------------------------------------------------------------------
	// TEST notification from the MocKLTSY when 
	// there is no client NotifyChangeOfNtwkRegStatus pending
 	//-------------------------------------------------------------------------  	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	RPacketService::TRegistrationStatus sndData0 = RPacketService::ERegisteredOnHomeNetwork;
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData0(sndData0);
	ltsyData0.SerialiseL(data); 
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyChangeOfNtwkRegStatus, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyChangeOfNtwkRegStatus when result is not cached.
 	//-------------------------------------------------------------------------    	
	TRequestStatus requestStatus;
	RPacketService::TRegistrationStatus registrationStatus;	
	
	// Post request		
	packetService.NotifyChangeOfNtwkRegStatus(requestStatus, registrationStatus);	
	
	// firstly issue notification  from  the MockLTSY 
    // which has the same registration state as is already saved
   	RPacketService::TRegistrationStatus sndData = RPacketService::ERegisteredOnHomeNetwork;
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData(sndData);
	data.Close();
	ltsyData.SerialiseL(data);    
	// Issue the Complete with the same registration state as is already saved
	iMockLTSY.CompleteL(EPacketNotifyChangeOfNtwkRegStatus, KErrNone, data);
	 
	// check that NotifyChangeOfNtwkRegStatus isn't completed
	ASSERT_EQUALS (KRequestPending, requestStatus.Int());
	
	//data for Complete with new registration state
	RPacketService::TRegistrationStatus sndData2 = RPacketService::ERegisteredRoaming;
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData2(sndData2);
	data.Close();
	ltsyData2.SerialiseL(data);		
	// Issue the Complete with new registration state
	iMockLTSY.CompleteL(EPacketNotifyChangeOfNtwkRegStatus, KErrNone, data);
	
	// wait for completion of NotifyChangeOfNtwkRegStatus
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(sndData2, registrationStatus);
		
	//-------------------------------------------------------------------------
	// TEST : Completion  of request 
	// RPacketService::NotifyChangeOfNtwkRegStatus with extended error code.
 	//-------------------------------------------------------------------------    	
		
	// Post request		
	packetService.NotifyChangeOfNtwkRegStatus(requestStatus, registrationStatus);	
		
	//data for Complete with registration state
	data.Close();
	ltsyData.SerialiseL(data);    
	// Issue the Complete with the same registration state as is already saved
	iMockLTSY.CompleteL(EPacketNotifyChangeOfNtwkRegStatus, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound), data);

	// check that NotifyChangeOfNtwkRegStatus completed with extended error code
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS (KErrGsmCCNetworkOutOfOrder, requestStatus.Int());
	ASSERT_EQUALS(sndData, registrationStatus);
	
	// Check that basic error code is returned when error granuality set to EErrorBasic
	RTelServer::TErrorGranularity granularity(RTelServer::EErrorBasic);
	TInt ret = iTelServer.SetExtendedErrorGranularity(granularity);
	ASSERT_EQUALS(KErrNone, ret);

	// Post request		
	packetService.NotifyChangeOfNtwkRegStatus(requestStatus, registrationStatus);	
		
	//data for Complete with registration state
	data.Close();
	ltsyData2.SerialiseL(data);    

	// Issue the Complete with the same registration state as is already saved
	iMockLTSY.CompleteL(EPacketNotifyChangeOfNtwkRegStatus, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound), data);
	
	// check that NotifyChangeOfNtwkRegStatus completed with basic error code
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS (KErrNotFound, requestStatus.Int());
	ASSERT_EQUALS(sndData2, registrationStatus);
		
	CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(2, this); // data, this	
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCONRS-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyChangeOfNtwkRegStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyChangeOfNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyChangeOfNtwkRegStatus0002L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;		
	
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyChangeOfNtwkRegStatus
 	//------------------------------------------------------------------------- 	
 	TRequestStatus requestStatus;
	RPacketService::TRegistrationStatus registrationStatus;	
	
	packetService.NotifyChangeOfNtwkRegStatus(requestStatus, registrationStatus);
    packetService.CancelAsyncRequest(EPacketNotifyChangeOfNtwkRegStatus); 	
 	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2);// packetService, data
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCONRS-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyChangeOfNtwkRegStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyChangeOfNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyChangeOfNtwkRegStatus0004L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 
	
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyChangeOfNtwkRegStatus
 	//-------------------------------------------------------------------------
	
	RPacketService::TRegistrationStatus sndData = RPacketService::ERegisteredRoaming;
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData(sndData);
	ltsyData.SerialiseL(data); 	 	
 		
	TRequestStatus requestStatus;
	RPacketService::TRegistrationStatus registrationStatus;	
	
	TRequestStatus requestStatus2;
	RPacketService::TRegistrationStatus registrationStatus2;	
	
	packetService.NotifyChangeOfNtwkRegStatus(requestStatus, registrationStatus);	
	packetService2.NotifyChangeOfNtwkRegStatus(requestStatus2, registrationStatus2);
				
	iMockLTSY.CompleteL(EPacketNotifyChangeOfNtwkRegStatus, KErrNone, data);
	
	// wait for first
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(sndData, registrationStatus);
	
	// wait for second
	User::WaitForRequest(requestStatus2);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	ASSERT_EQUALS(sndData, registrationStatus2);	

	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCAR-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyContextActivationRequested
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyContextActivationRequested
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextActivationRequested0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
  	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RPacketService::NotifyContextActivationRequested 
 	//-------------------------------------------------------------------------

	// Data for CompleteL
	RPacketContext::TContextConfigGPRS sndContextParam;	
	sndContextParam.iPdpType         = RPacketContext::EPdpTypeIPv6;
	sndContextParam.iAccessPointName.Copy(_L8("PointName"));
	sndContextParam.iPdpAddress.Copy(_L8("PdpAddress"));
	sndContextParam.iPdpCompression = 112233;
	sndContextParam.iAnonymousAccessReqd = RPacketContext::ERequired;
	sndContextParam.iUseEdge = ETrue;
	sndContextParam.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	sndContextParam.iProtocolConfigOption.iAuthInfo.iUsername.Copy(_L8("username1"));
	sndContextParam.iProtocolConfigOption.iAuthInfo.iPassword.Copy(_L8("password1234"));
	sndContextParam.iProtocolConfigOption.iChallenge.Copy(_L8("pseudorandom1"));
	sndContextParam.iProtocolConfigOption.iResponse.Copy(_L8("response1234")); 
	sndContextParam.iProtocolConfigOption.iId = 0x01;
	sndContextParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(_L8("PriDNS_1")); 
	sndContextParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(_L8("SecDNS_2")); 
	sndContextParam.iProtocolConfigOption.iMiscBuffer.Copy(_L8("buffer1234")); 
	sndContextParam.iNWIContext = ETrue;
	TMockLtsyData1 <RPacketContext::TContextConfigGPRS > ltsyData(sndContextParam);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS getContextParam;	
	TPckg<RPacketContext::TContextConfigGPRS> pckgParam(getContextParam);
	
	// Send request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);		
	// Wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    // Wait for answer 
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	// Check that test case's and received TSY's packet context parameters match
	ASSERT_EQUALS(sndContextParam.iPdpType, getContextParam.iPdpType);
	ASSERT_EQUALS(sndContextParam.iAccessPointName, getContextParam.iAccessPointName);
	ASSERT_EQUALS(sndContextParam.iPdpAddress, getContextParam.iPdpAddress);
	ASSERT_EQUALS(sndContextParam.iPdpCompression, getContextParam.iPdpCompression);
	ASSERT_EQUALS(sndContextParam.iAnonymousAccessReqd, getContextParam.iAnonymousAccessReqd);
    ASSERT_EQUALS(sndContextParam.iUseEdge, getContextParam.iUseEdge);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iAuthInfo.iProtocol, getContextParam.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iAuthInfo.iUsername, getContextParam.iProtocolConfigOption.iAuthInfo.iUsername);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iAuthInfo.iPassword, getContextParam.iProtocolConfigOption.iAuthInfo.iPassword);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iChallenge, getContextParam.iProtocolConfigOption.iChallenge);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iResponse, getContextParam.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iId, getContextParam.iProtocolConfigOption.iId);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, getContextParam.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, getContextParam.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	
	ASSERT_EQUALS(sndContextParam.iProtocolConfigOption.iMiscBuffer, getContextParam.iProtocolConfigOption.iMiscBuffer);	
	ASSERT_EQUALS(sndContextParam.iNWIContext, getContextParam.iNWIContext);	
	
 	//-------------------------------------------------------------------------
	// TEST B: Unsolicited completion of RPacketService::NotifyContextActivationRequested
	// from LTSY.
 	//-------------------------------------------------------------------------
 	
	// Issue Unsolicited completion	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);		
	// Wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // RPacketService, data, this
	
	}
	
/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCAR-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyContextActivationRequested
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyContextActivationRequested requests using different parameter class versions
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextActivationRequested0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);  
	
	TRequestStatus requestStatus;	
	TRequestStatus mockLtsyStatus;
	
	// ------------------------------------------------------------------------
	// NotifyContextActivationRequested 
	// with RPacketContext::TContextConfig_R5 as parameter	
	// ------------------------------------------------------------------------

	// Create R5 related data for CompleteL
	RPacketContext::TContextConfig_R5 testCaseContextR5;	
	testCaseContextR5.iPdpType         = RPacketContext::EPdpTypeIPv6;
	testCaseContextR5.iAccessPointName.Copy(_L8("PointName"));
    testCaseContextR5.iPdpAddress.Copy(_L8("PdpAddress"));
	testCaseContextR5.iUseEdge = EFalse;
	testCaseContextR5.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolNone;
	testCaseContextR5.iProtocolConfigOption.iAuthInfo.iUsername.Copy(_L8("username"));
	testCaseContextR5.iProtocolConfigOption.iAuthInfo.iPassword.Copy(_L8("password"));
	testCaseContextR5.iProtocolConfigOption.iChallenge.Copy(_L8("pseudorandom_data"));
	testCaseContextR5.iProtocolConfigOption.iResponse.Copy(_L8("123abc"));
	testCaseContextR5.iProtocolConfigOption.iId = 0x11;
	testCaseContextR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(_L8("PrimaryDNS"));
	testCaseContextR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(_L8("SecondaryDNS"));
	testCaseContextR5.iProtocolConfigOption.iMiscBuffer.Copy(_L8("Miscellaneous buffer content"));
	testCaseContextR5.iNWIContext = ETrue;
	testCaseContextR5.iPFI = RPacketContext::EBestEffort;
	// R5 parameters
	testCaseContextR5.iPdpHeaderCompression = RPacketContext::ENoHeaderCompression;
	testCaseContextR5.iPdpDataCompression = RPacketContext::ENoDataCompression;
	TMockLtsyData1<RPacketContext::TContextConfig_R5> ltsyData(testCaseContextR5);
	ltsyData.SerialiseL(data);	
	
	// Create R5 related package that will received data from licensee TSY
	RPacketContext::TContextConfig_R5 getContextR5Param;	
	TPckg<RPacketContext::TContextConfig_R5> pckgParamR5(getContextR5Param);
			
	// Send request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParamR5);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);		
	// Wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    User::WaitForRequest(requestStatus);

    // Check that test case's and received TSY's packet context parameters match
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS( testCaseContextR5.iPdpType, getContextR5Param.iPdpType );
    ASSERT_EQUALS( testCaseContextR5.iAccessPointName, getContextR5Param.iAccessPointName );
	ASSERT_EQUALS( testCaseContextR5.iPdpAddress, getContextR5Param.iPdpAddress );
	ASSERT_EQUALS( testCaseContextR5.iUseEdge, getContextR5Param.iUseEdge );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iAuthInfo.iProtocol, getContextR5Param.iProtocolConfigOption.iAuthInfo.iProtocol );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iAuthInfo.iUsername, getContextR5Param.iProtocolConfigOption.iAuthInfo.iUsername );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iAuthInfo.iPassword, getContextR5Param.iProtocolConfigOption.iAuthInfo.iPassword );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iChallenge, getContextR5Param.iProtocolConfigOption.iChallenge );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iResponse, getContextR5Param.iProtocolConfigOption.iResponse );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iId, getContextR5Param.iProtocolConfigOption.iId );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, getContextR5Param.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, getContextR5Param.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );
	ASSERT_EQUALS( testCaseContextR5.iProtocolConfigOption.iMiscBuffer, getContextR5Param.iProtocolConfigOption.iMiscBuffer );
	ASSERT_EQUALS( testCaseContextR5.iNWIContext, getContextR5Param.iNWIContext );
	ASSERT_EQUALS( testCaseContextR5.iPFI, getContextR5Param.iPFI );
	ASSERT_EQUALS( testCaseContextR5.iPdpHeaderCompression, getContextR5Param.iPdpHeaderCompression );
	ASSERT_EQUALS( testCaseContextR5.iPdpDataCompression, getContextR5Param.iPdpDataCompression );

	// Resend request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParamR5);

    AssertMockLtsyStatusL();

	// ------------------------------------------------------------------------
	// NotifyContextActivationRequested 
	// with RPacketContext::TContextConfigR99_R4 as parameter	
	// ------------------------------------------------------------------------	
	
	// Cancel request before making new request with TContextConfigR99_R4
    packetService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
	
	// Create R99_R4 related data for CompleteL
	RPacketContext::TContextConfigR99_R4 testCaseContextR99_R4;	
	testCaseContextR99_R4.iPdpType = RPacketContext::EPdpTypeIPv6;
	testCaseContextR99_R4.iAccessPointName.Copy(_L8("PointName"));
	testCaseContextR99_R4.iPdpAddress.Copy(_L8("PdpAddress"));
	testCaseContextR99_R4.iUseEdge = ETrue;
	testCaseContextR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolPAP;
	testCaseContextR99_R4.iProtocolConfigOption.iAuthInfo.iUsername.Copy(_L8("username_username"));
	testCaseContextR99_R4.iProtocolConfigOption.iAuthInfo.iPassword.Copy(_L8("password_password"));
	testCaseContextR99_R4.iProtocolConfigOption.iChallenge.Copy(_L8("pseudorandom_pseudorandom"));
	testCaseContextR99_R4.iProtocolConfigOption.iResponse.Copy(_L8("pseudorandom_response"));
	testCaseContextR99_R4.iProtocolConfigOption.iId = 0x92;
	testCaseContextR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(_L8("DnsAddresses_PrimaryDns") );
	testCaseContextR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(_L8("DnsAddresses_SecondaryDns") );
	testCaseContextR99_R4.iProtocolConfigOption.iMiscBuffer.Copy(_L8("miscbuffer"));
	testCaseContextR99_R4.iNWIContext = EFalse;
	testCaseContextR99_R4.iPFI = RPacketContext::ESignalling;
	TMockLtsyData1<RPacketContext::TContextConfigR99_R4> ltsyDataR99_R4(testCaseContextR99_R4);
	data.Close();
	ltsyDataR99_R4.SerialiseL(data);	
	
	// Create R99_R4 related package that will received data from licensee TSY
	RPacketContext::TContextConfigR99_R4 getContextParametersR99_R4;	
	TPckg<RPacketContext::TContextConfigR99_R4> pckgParamR99_R4(getContextParametersR99_R4);	
	
	// Send request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParamR99_R4);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);
	// Wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	User::WaitForRequest(requestStatus);
	
    // Check that test case's and received TSY's packet context parameters match
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS( testCaseContextR99_R4.iPdpType, getContextParametersR99_R4.iPdpType );
    ASSERT_EQUALS( testCaseContextR99_R4.iAccessPointName, getContextParametersR99_R4.iAccessPointName );
	ASSERT_EQUALS( testCaseContextR99_R4.iPdpAddress, getContextParametersR99_R4.iPdpAddress );
	ASSERT_EQUALS( testCaseContextR99_R4.iUseEdge, getContextParametersR99_R4.iUseEdge );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol, getContextParametersR99_R4.iProtocolConfigOption.iAuthInfo.iProtocol );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iAuthInfo.iUsername, getContextParametersR99_R4.iProtocolConfigOption.iAuthInfo.iUsername );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iAuthInfo.iPassword, getContextParametersR99_R4.iProtocolConfigOption.iAuthInfo.iPassword );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iChallenge, getContextParametersR99_R4.iProtocolConfigOption.iChallenge );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iResponse, getContextParametersR99_R4.iProtocolConfigOption.iResponse );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iId, getContextParametersR99_R4.iProtocolConfigOption.iId );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, getContextParametersR99_R4.iProtocolConfigOption.iDnsAddresses.iPrimaryDns );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, getContextParametersR99_R4.iProtocolConfigOption.iDnsAddresses.iSecondaryDns );
	ASSERT_EQUALS( testCaseContextR99_R4.iProtocolConfigOption.iMiscBuffer, getContextParametersR99_R4.iProtocolConfigOption.iMiscBuffer );
	ASSERT_EQUALS( testCaseContextR99_R4.iNWIContext, getContextParametersR99_R4.iNWIContext );
	ASSERT_EQUALS( testCaseContextR99_R4.iPFI, getContextParametersR99_R4.iPFI );

	// Resend request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParamR99_R4);

	AssertMockLtsyStatusL();		
	
    // RPacketService, data, this 
	CleanupStack::PopAndDestroy(3, this); 	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCAR-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyContextActivationRequested
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyContextActivationRequested
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextActivationRequested0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyContextActivationRequested
 	//------------------------------------------------------------------------- 	 	
	// Data for CompleteL
	RPacketContext::TContextConfigGPRS configGPRS;
	TPckg<RPacketContext::TContextConfigGPRS> pckgParam(configGPRS);

	TRequestStatus requestStatus;
	
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
	packetService.CancelAsyncRequest(EPacketNotifyContextActivationRequested);
	
    // Wait for answer 
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // this, packetService	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCAR-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyContextActivationRequested with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyContextActivationRequested with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextActivationRequested0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	
	//---------------------------------------------------------------------------------
	// send wrong type of parameter
	//---------------------------------------------------------------------------------	
	// Data for CompleteL
	TPacketDataConfigBase context;
	TMockLtsyData1 <TPacketDataConfigBase> ltsyData(context);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	TInt contextParameters; // wrong type of parameter	
	TPckg<TInt> pckgParam(contextParameters);
	
	// send request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);		
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    
    // wait for answer 
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//---------------------------------------------------------------------------------
	// type of parameter to send is  RPacketContext::TContextConfigCDMA  
	// isn't supported
	//---------------------------------------------------------------------------------
	RPacketContext::TContextConfigCDMA contextParametersCDMA;	
	TPckg<RPacketContext::TContextConfigCDMA> pckgParamCDMA(contextParametersCDMA);
	
	// send request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParamCDMA);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);		
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    
    // wait for answer 
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	CleanupStack::PopAndDestroy();// packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSNCAR-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyContextActivationRequested
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyContextActivationRequested
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyContextActivationRequested0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting 
	// RPacketService::NotifyContextActivationRequested
 	//-------------------------------------------------------------------------
	
	// Data for CompleteL
	RPacketContext::TContextConfigGPRS sendConfigGPRS;	
	sendConfigGPRS.iPdpType         = RPacketContext::EPdpTypeIPv6;
	sendConfigGPRS.iPdpAddress.Copy(_L8("PdpAddress"));
	sendConfigGPRS.iAccessPointName.Copy(_L8("PointName"));
	TMockLtsyData1 <RPacketContext::TContextConfigGPRS > ltsyData(sendConfigGPRS);
	ltsyData.SerialiseL(data);	
		
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS getConfigGPRS;	
	TPckg<RPacketContext::TContextConfigGPRS> pckgParam(getConfigGPRS);	
	// Send first request
	packetService.NotifyContextActivationRequested(requestStatus, pckgParam);
		
	TRequestStatus requestStatus2;
	RPacketContext::TContextConfigR99_R4 getConfigR99;	
	TPckg<RPacketContext::TContextConfigR99_R4> pckgParam2(getConfigR99);	
	// Send second request
	packetService2.NotifyContextActivationRequested(requestStatus2, pckgParam2);
		
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextActivationRequested, KErrNone, data);		
	// Wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
    
    // Wait for the first answer 
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(sendConfigGPRS.iPdpType, getConfigGPRS.iPdpType);
	ASSERT_EQUALS(sendConfigGPRS.iAccessPointName, getConfigGPRS.iAccessPointName);
	ASSERT_EQUALS(sendConfigGPRS.iPdpAddress, getConfigGPRS.iPdpAddress);
	ASSERT_EQUALS(sendConfigGPRS.iPdpCompression, getConfigGPRS.iPdpCompression);
	ASSERT_EQUALS(sendConfigGPRS.iAnonymousAccessReqd, getConfigGPRS.iAnonymousAccessReqd);
    ASSERT_EQUALS(sendConfigGPRS.iUseEdge, getConfigGPRS.iUseEdge);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol, getConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername, getConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword, getConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iChallenge, getConfigGPRS.iProtocolConfigOption.iChallenge);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iResponse, getConfigGPRS.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iId, getConfigGPRS.iProtocolConfigOption.iId);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, getConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, getConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iMiscBuffer, getConfigGPRS.iProtocolConfigOption.iMiscBuffer);	
	ASSERT_EQUALS(sendConfigGPRS.iNWIContext, getConfigGPRS.iNWIContext);		
	
	// Wait for the second answer 
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(sendConfigGPRS.iPdpType, getConfigR99.iPdpType);
	ASSERT_EQUALS(sendConfigGPRS.iAccessPointName, getConfigR99.iAccessPointName);
	ASSERT_EQUALS(sendConfigGPRS.iPdpAddress, getConfigR99.iPdpAddress);
    ASSERT_EQUALS(sendConfigGPRS.iUseEdge, getConfigR99.iUseEdge);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iAuthInfo.iProtocol, getConfigR99.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iAuthInfo.iUsername, getConfigR99.iProtocolConfigOption.iAuthInfo.iUsername);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iAuthInfo.iPassword, getConfigR99.iProtocolConfigOption.iAuthInfo.iPassword);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iChallenge, getConfigR99.iProtocolConfigOption.iChallenge);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iResponse, getConfigR99.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iId, getConfigR99.iProtocolConfigOption.iId);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, getConfigR99.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, getConfigR99.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	
	ASSERT_EQUALS(sendConfigGPRS.iProtocolConfigOption.iMiscBuffer, getConfigR99.iProtocolConfigOption.iMiscBuffer);	
	ASSERT_EQUALS(sendConfigGPRS.iNWIContext, getConfigR99.iNWIContext);
	ASSERT_EQUALS(RPacketContext::EBestEffort, getConfigR99.iPFI );	

	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSA-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Attach
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Attach
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestAttach0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
	TRequestStatus requestStatus;
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketAttach, KErrNotSupported);	
	packetService.Attach(requestStatus);
	
	User::WaitForRequest(requestStatus);
	//prepare error code 
	TInt errorCode = ( KErrGeneral & 0x0000FFFF ) |( KErrGsmMMNetworkFailure << 16 );	
	errorCode = (errorCode>>16) | 0xFFFF0000;
	ASSERT_EQUALS(errorCode, requestStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketAttach);
	iMockLTSY.CompleteL(EPacketAttach, KErrGeneral);
	
	packetService.Attach(requestStatus);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	// TEST B: failure with specific error (KErrGsmPSConnectionBarred) from LTSY->CTSY 
	iMockLTSY.ExpectL(EPacketAttach);
	iMockLTSY.CompleteL(EPacketAttach, KErrGsmPSConnectionBarred);
		
	packetService.Attach(requestStatus);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmPSConnectionBarred, requestStatus.Int());
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::Attach when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketAttach);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone);
	
	packetService.Attach(requestStatus);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST D: RPacketService::Attach again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
	packetService.Attach(requestStatus);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrAlreadyExists, requestStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::Attach
	// from LTSY.
 	//-------------------------------------------------------------------------	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//Unsolicited completion
	iMockLTSY.CompleteL(EPacketAttach, KErrNone);
	User::WaitForRequest(mockLtsyStatus); 
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSA-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::Attach
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::Attach
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestAttach0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::Attach
 	//-------------------------------------------------------------------------
 	TRequestStatus requestStatus;
	iMockLTSY.ExpectL(EPacketAttach);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone, 5);	
	packetService.Attach(requestStatus);
	packetService.CancelAsyncRequest(EPacketAttach);
	
	User::WaitForRequest(requestStatus);
	// TSY has started a request and it is not possible to then
    // cancel this request. The best thing for the TSY to do in this case
    // is to proceed as though the Cancel never happened. The server's 
    // call to the TSY cancel function will return synchronously.
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetService, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSA-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::Attach
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::Attach
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestAttach0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::Attach
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	iMockLTSY.ExpectL(EPacketAttach);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone, 2);
	
	packetService.Attach(requestStatus);	
	packetService2.Attach(requestStatus2);
	
	// wait for the first answer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for the second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSA-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Attach with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Attach and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestAttach0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::Attach
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	iMockLTSY.ExpectL(EPacketAttach);
	packetService.Attach(requestStatus);
		
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSD-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Detach
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Detach
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestDetach0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 dataExpect;
    CleanupClosePushL(dataExpect);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService); 
      
    // make status "attached"
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketAttach, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
     //-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EPacketDetach, KErrNotSupported);

    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGsmMMNetworkFailure, requestStatus.Int());

    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketAttach, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();

    iMockLTSY.ExpectL(EPacketDetach);
    iMockLTSY.CompleteL(EPacketDetach, KErrGeneral);

    // data for ExpectL
    RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
    TMockLtsyData1<RPacketService::TAttachMode> ltsyData2(attachMode);
    ltsyData2.SerialiseL(dataExpect);
     
    iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);

    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

     //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RPacketService::Detach when result is not cached.
     //-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketAttach, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();

    iMockLTSY.ExpectL(EPacketDetach);
    iMockLTSY.CompleteL(EPacketDetach, KErrNone);

    iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);

    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

     //-------------------------------------------------------------------------
    // TEST D: RPacketService::Detach again
     //-------------------------------------------------------------------------

    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrAlreadyExists, requestStatus.Int());

    //---------------------------------
    // Detach if context is not active
    //---------------------------------
       
    // Open new context
    TInfoName contextId; 
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
          
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketAttach, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();

    iMockLTSY.ExpectL(EPacketDetach);
    iMockLTSY.CompleteL(EPacketDetach, KErrNone);

    iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);

    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    AssertMockLtsyStatusL();

    //---------------------------------
    // Detach if context is active
    //---------------------------------
           
    // initialisation of the context 
    TInt status = InitContextL(packetContext, contextId);
    ASSERT_EQUALS(KErrNone, status);

    // Set context in Active state
    status = SetContextActiveL(contextId);
    ASSERT_EQUALS(KErrNone, status); 
       
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketAttach, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();

    packetService.Detach(requestStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrInUse, requestStatus.Int());
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RPacketService::Detach
    // from LTSY. With attach mode == RPacketService::EAttachWhenNeeded;
    // and 
    //------------------------------------------------------------------------- 

    // --- Set AttachMode  to RPacketService::EAttachWhenNeeded
    // data for ExpectL
    attachMode = RPacketService::EAttachWhenNeeded;
    TMockLtsyData1<RPacketService::TAttachMode> ltsyData3(attachMode);
    dataExpect.Close();
    ltsyData3.SerialiseL(dataExpect);

    // Set attachMode status to  EAttachWhenNeeded 
    iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);		
    iMockLTSY.CompleteL(EPacketSetAttachMode, KErrNone);
    	
    packetService.SetAttachMode(requestStatus, attachMode);	
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    // send Unsolicited completion
    iMockLTSY.CompleteL(EPacketDetach, KErrNone);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    CleanupStack::PopAndDestroy(1); //packetContext
    CleanupStack::PopAndDestroy(1); //packetService
    CleanupStack::PopAndDestroy(3, this); //dataExpect,  data, this

    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PSD-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::Detach
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::Detach
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestDetach0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);	

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::Detach
 	//-------------------------------------------------------------------------
 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.ExpectL(EPacketDetach);
	iMockLTSY.CompleteL(EPacketDetach, KErrNone);
	
	// data for ExpectL
 	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
 	TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
 	ltsyData.SerialiseL(dataExpect);
	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);
	
    TRequestStatus requestStatus;
	packetService.Detach(requestStatus);
	packetService.CancelAsyncRequest(EPacketDetach);
	User::WaitForRequest(requestStatus);
	// TSY has started a request and it is not possible to then
    // cancel this request. The best thing for the TSY to do in this case
    // is to proceed as though the Cancel never happened. The server's 
    // call to the TSY cancel function will return synchronously
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(1); //packetService
	CleanupStack::PopAndDestroy(3, this); //dataExpect,  data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSD-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::Detach
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::Detach
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestDetach0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data); 
	
	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect); 

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::Detach
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	iMockLTSY.ExpectL(EPacketDetach);
	iMockLTSY.CompleteL(EPacketDetach, KErrNone);
	
	// data for ExpectL
 	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
 	TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
 	ltsyData.SerialiseL(dataExpect); 		
	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);
	
	TRequestStatus requestStatus;
	packetService.Detach(requestStatus);

	TRequestStatus requestStatus2;
	packetService2.Detach(requestStatus2);
		
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	CleanupStack::PopAndDestroy(2);
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, dataExpect, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSD-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Detach with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Detach and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestDetach0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);	

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::Detach
 	//-------------------------------------------------------------------------

	// make status "attached"
    TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.ExpectL(EPacketDetach);
	iMockLTSY.CompleteL(EPacketAttach, KErrNone);

	// data for ExpectL
 	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenPossible;
 	TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
 	ltsyData.SerialiseL(dataExpect); 		
	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);

	packetService.Detach(requestStatus);
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(3, this); // dataExpect, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);	

    RPacketService::TStatus status;
    TInt ret = packetService.GetStatus(status);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RPacketService::EStatusUnattached, status);

    // now change status 
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	RPacketService::TStatus sendStatus = RPacketService::EStatusAttached;
	TBool isResumed = EFalse;
	TMockLtsyData2 <RPacketService::TStatus, TBool > ltsyData(sendStatus, isResumed);
	ltsyData.SerialiseL(data);	
		
	iMockLTSY.CompleteL(EPacketNotifyStatusChange, KErrNone, data);

	User::WaitForRequest(mockLtsyStatus);

    // get new status
    ret = packetService.GetStatus(status);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RPacketService::EStatusAttached, status);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSO-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Open
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Open
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestOpen0001L()
	{
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);


    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.ExpectL(EPacketNotifyStatusChange);
	
	TBool resumed = EFalse;
    RPacketService::TStatus	status(RPacketService::EStatusUnattached);
    TMockLtsyData2<RPacketService::TStatus , TBool> comNotifyStatus(status, resumed);
	comNotifyStatus.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketNotifyStatusChange,KErrNone, data);
	
	TBool expInitState = ETrue;	 
	TMockLtsyData1<TBool> expGetAttachMode(expInitState);
	data.Close();
	expGetAttachMode.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetAttachMode, data);
	
	RPacketService::TAttachMode mode(RPacketService::EAttachWhenNeeded);
	TMockLtsyData1<RPacketService::TAttachMode> comGetAttachMode(mode);
	data.Close();
	comGetAttachMode.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetAttachMode,KErrNone, data);
	
    TInt maxServices = 10;
	TMockLtsyData1< TInt > iniData(maxServices);
    iniData.SerialiseL(data); 	
	iMockLTSY.ExpectL(EPacketEnumerateMbmsMonitorServiceList);
	iMockLTSY.CompleteL( EPacketEnumerateMbmsMonitorServiceList, KErrNone, data );
	
	TInt maxMonitorServices = 10; 
	TMockLtsyData1< TInt > iniData2(maxMonitorServices);
    iniData2.SerialiseL(data); 	
    iMockLTSY.ExpectL(EPacketEnumerateMbmsActiveServiceList);
	iMockLTSY.CompleteL( EPacketEnumerateMbmsActiveServiceList, KErrNone, data );
	
	//this is actually a sync call to LTSY where return represents a context id offset rather than error code.
	iMockLTSY.ExpectL(EPacketInitProxiesIPC, 3);	
	
	//this actually completes before all messages between CTSY<->LTSY have finished.
	RPacketService packetService;
	TInt ret = packetService.Open(iPhone);
    ASSERT_EQUALS(KErrNone, ret); 
	CleanupClosePushL(packetService);
		
	// Wait for the end of initialisation
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(1); 
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSO-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::Open with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::Open with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestOpen0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone phone;

    // using unopened RMobilePhone as a bad param, panic expected
	RPacketService packetService;
	TInt ret = packetService.Open(phone);
    ASSERT_EQUALS(KErrArgument, ret); 
	CleanupClosePushL(packetService);

	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSO-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::Open
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::Open
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestOpen0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone phone3;
	ret = phone3.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	CleanupStack::PopAndDestroy(1);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGMC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetMSClass
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetMSClass
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetMSClass0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	
	RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
	TRequestStatus requestStatus;
	RPacketService::TMSClass curClass;
	RPacketService::TMSClass maxClass;
	
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetMSClass when result is not cached.
 	//-------------------------------------------------------------------------

    packetService.GetMSClass(requestStatus, curClass, maxClass);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KMaxMsClass, maxClass);
	ASSERT_EQUALS(RPacketService::EMSClassUnknown, curClass);
	

	// change current msclass
	RPacketService::TMSClass sendClass = RPacketService::EMSClassSuspensionRequired;
	TMockLtsyData1 < RPacketService::TMSClass > ltsyData(sendClass);
	ltsyData.SerialiseL(data);		
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyMSClassChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);


    //get new msclass
    packetService.GetMSClass(requestStatus, curClass, maxClass);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(RPacketService::EMSClassSuspensionRequired, curClass);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //packetService 
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSGMC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetMSClass
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetMSClass
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetMSClass0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);


    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);
    
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetMSClass
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.
	
	TRequestStatus requestStatus;
	RPacketService::TMSClass curClass;
	RPacketService::TMSClass maxClass;	

    packetService.GetMSClass(requestStatus, curClass, maxClass);


	TRequestStatus requestStatus2;
	RPacketService::TMSClass curClass2;
	RPacketService::TMSClass maxClass2;	

    packetService2.GetMSClass(requestStatus2, curClass2, maxClass2);


	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KMaxMsClass, maxClass);
	ASSERT_EQUALS(RPacketService::EMSClassUnknown, curClass);

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(KMaxMsClass, maxClass2);
	ASSERT_EQUALS(RPacketService::EMSClassUnknown, curClass2);

	CleanupStack::PopAndDestroy(2);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSGNI-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetNifInfo
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetNifInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetNifInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	

	TRequestStatus requestStatus;
	TInt index(0);
	TPckgBuf<RPacketService::TNifInfoV2> nifInfo;

 	//-------------------------------------------------------------------------
	// Completion request of RPacketService::GetNifInfo when there is no context and so no nif
 	//-------------------------------------------------------------------------

	packetService.GetNifInfo(requestStatus, index, nifInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());


    //** open new context ****************************        
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
	
    //Completion request of RPacketService::GetNifInfo when there is 1 context
	packetService.GetNifInfo(requestStatus, index, nifInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	TName contextName(contextId);
	RPacketService::TNifInfoV2 nifData(nifInfo());
	ASSERT_EQUALS(contextName, nifData.iContextName);
	ASSERT_EQUALS(RPacketContext::EStatusUnknown, nifData.iNifStatus);


    //** change pdp address 1 *****************
    InitContextL(packetContext, contextId);

    RPacketContext::TContextConfigGPRS contextConfig;
    _LIT(KPdpAdd, "Some Add");
    contextConfig.iPdpAddress.Copy(KPdpAdd);
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);    
    

	TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(contextConfig, contextId);
    expExpect.SerialiseL(expectData);

    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    packetContext.SetConfig(requestStatus, pckgConfig);
    
	User::WaitForRequest(requestStatus);
    
    //** open second context ******************    
	RPacketContext packetContext2;
    TInfoName contextId2; 
    packetContext2.OpenNewContext(packetService, contextId2);
    CleanupClosePushL(packetContext2);

    InitContextL(packetContext2, contextId2);

    //** change pdp address 2 *****************
    contextConfig.iPdpAddress.Copy(KPdpAdd);
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig2(contextConfig);    
    

	TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect2(contextConfig, contextId2);
    expectData.Close();
    expExpect2.SerialiseL(expectData);

    TMockLtsyData1<TInfoName > contexName2(contextId2);
    data.Close();
    contexName2.SerialiseL(data);
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    packetContext2.SetConfig(requestStatus, pckgConfig2);
    
	User::WaitForRequest(requestStatus);

    //******************************************
    
    // check one more test branch
    index = 1;
    
	packetService.GetNifInfo(requestStatus, index, nifInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); //packetService, packetContext, packetContext2		
	CleanupStack::PopAndDestroy(3, this); // expectData, data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSGNI-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetNifInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetNifInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetNifInfo0003L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);


    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);

	
	TRequestStatus requestStatus;
	TInt index(0);
	

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketService::GetNifInfo
 	//-------------------------------------------------------------------------
 	
 	index = -1;
	TPckgBuf<RPacketService::TNifInfoV2> nifInfo;
	
	packetService.GetNifInfo(requestStatus, index, nifInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

 	index = 1;
	
	packetService.GetNifInfo(requestStatus, index, nifInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
    index = 0;
	TPckgBuf<RPacketContext::TDnsInfoV2> wrongInfo;

	packetService.GetNifInfo(requestStatus, index, wrongInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); //packetService, packetContext	
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGNI-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetNifInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetNifInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetNifInfo0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);
    
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
	RPacketContext packetContext2;
    TInfoName contextId2; 
    packetContext2.OpenNewContext(packetService2, contextId2);
    CleanupClosePushL(packetContext2);
    
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetNifInfo
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
	TInt index(0);
	TPckgBuf<RPacketService::TNifInfoV2> nifInfo;
	
	packetService.GetNifInfo(requestStatus, index, nifInfo);
	
	
	TRequestStatus requestStatus2;
	TInt index2(0);
	TPckgBuf<RPacketService::TNifInfoV2> nifInfo2;
	
	packetService2.GetNifInfo(requestStatus2, index2, nifInfo2);

	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	TName contextName(contextId);
	RPacketService::TNifInfoV2 nifData(nifInfo());
	ASSERT_EQUALS(contextName, nifData.iContextName);
	ASSERT_EQUALS(RPacketContext::EStatusUnknown, nifData.iNifStatus);


	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	TName contextName2(contextId);
	RPacketService::TNifInfoV2 nifData2(nifInfo2());
	ASSERT_EQUALS(contextName2, nifData2.iContextName);
	ASSERT_EQUALS(RPacketContext::EStatusUnknown, nifData2.iNifStatus);

	CleanupStack::PopAndDestroy(4);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSSMSC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetMSClass
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetMSClass
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetMSClass0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);


	TRequestStatus requestStatus;
	RPacketService::TMSClass msclass(RPacketService::EMSClassDualMode);
	
	packetService.SetMSClass(requestStatus, msclass);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}




/**
@SYMTestCaseID BA-CTSY-PKTS-PSDN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::DeactivateNIF
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::DeactivateNIF
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestDeactivateNIF0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);

	
	// Test API is not supported

	TRequestStatus requestStatus;
	
	packetService.DeactivateNIF(requestStatus, contextId);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2);     //packetService, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSEN-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::EnumerateNifs
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateNifs
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateNifs0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);


	TRequestStatus requestStatus;
	TInt nifNum(0);
	

 	//-------------------------------------------------------------------------
	// Successful completion request of
	// RPacketService::EnumerateNifs when there is  no contexts and no nifs
 	//-------------------------------------------------------------------------
	
	packetService.EnumerateNifs(requestStatus, nifNum);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, nifNum);

	
	// add context
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::EnumerateNifs when 1 context and therefore 1 nif.
 	//-------------------------------------------------------------------------
	
	packetService.EnumerateNifs(requestStatus, nifNum);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(1, nifNum);

	// add context
	RPacketContext packetContext2;
    TInfoName contextId2; 
    packetContext2.OpenNewContext(packetService, contextId2);
    CleanupClosePushL(packetContext2);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::EnumerateNifs when there are multiple contexts 
 	//-------------------------------------------------------------------------
	
	packetService.EnumerateNifs(requestStatus, nifNum);
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(1, nifNum);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); //packetContext2
	CleanupStack::PopAndDestroy(2); //packetService, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}




/**
@SYMTestCaseID BA-CTSY-PKTS-PSEN-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::EnumerateNifs
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::EnumerateNifs
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateNifs0004L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);
    
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
	RPacketContext packetContext2;
    TInfoName contextId2; 
    packetContext2.OpenNewContext(packetService2, contextId2);
    CleanupClosePushL(packetContext2);
    

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::EnumerateNifs
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	TInt nifNum(0);
		
	packetService.EnumerateNifs(requestStatus, nifNum);


	TRequestStatus requestStatus2;
	TInt nifNum2(0);
		
	packetService2.EnumerateNifs(requestStatus2, nifNum2);
	
	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(1, nifNum);


	User::WaitForRequest(requestStatus2);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(1, nifNum2);

	CleanupStack::PopAndDestroy(4);
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGAM-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetAttachMode
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetAttachMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetAttachMode0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	TBool sndData = FALSE;
	TMockLtsyData1<TBool> ltsyData(sndData);
	ltsyData.SerialiseL(dataExpect);
		
	iMockLTSY.ExpectL(EPacketGetAttachMode, dataExpect, KErrNotSupported);
	
	// Post request
	TRequestStatus requestStatus;
	RPacketService::TAttachMode attachMode;	
	packetService.GetAttachMode(requestStatus, attachMode);		
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	RPacketService::TAttachMode retMode = RPacketService::EAttachWhenNeeded;
	TMockLtsyData1<RPacketService::TAttachMode> ltsyDataComplete(retMode);
	ltsyDataComplete.SerialiseL(dataComplete);
	iMockLTSY.ExpectL(EPacketGetAttachMode, dataExpect);	
	
	iMockLTSY.CompleteL(EPacketGetAttachMode, KErrGeneral, dataComplete);
	
	packetService.GetAttachMode(requestStatus, attachMode);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetAttachMode when result is not cached.
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EPacketGetAttachMode, dataExpect);		
	iMockLTSY.CompleteL(EPacketGetAttachMode, KErrNone, dataComplete);
	
	packetService.GetAttachMode(requestStatus, attachMode);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(retMode, attachMode);
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetSubscriberId
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketGetAttachMode, KErrNone, dataComplete);
	User::WaitForRequest(mockLtsyStatus);
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(3, this); // dataExpect, dataComplete, this	
	}
	
	
/**
@SYMTestCaseID BA-CTSY-PKTS-PSGAM-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetAttachMode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetAttachMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetAttachMode0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetAttachMode
 	//-------------------------------------------------------------------------
	
	// data for ExpectL  
	TBool sndData = FALSE;
	TMockLtsyData1<TBool> ltsyData(sndData);
	ltsyData.SerialiseL(data);
	
	// data for CompleteL
	RPacketService::TAttachMode retMode = RPacketService::EAttachWhenNeeded;
	TMockLtsyData1<RPacketService::TAttachMode> ltsyDataComplete(retMode);
	ltsyDataComplete.SerialiseL(dataComplete);	
	
	iMockLTSY.ExpectL(EPacketGetAttachMode, data);		
	iMockLTSY.CompleteL(EPacketGetAttachMode, KErrNone, dataComplete);
	
	// send first request
	TRequestStatus requestStatus;
	RPacketService::TAttachMode attachMode;
	packetService.GetAttachMode(requestStatus, attachMode);	
		
	// send second request
	TRequestStatus requestStatus2;
	RPacketService::TAttachMode attachMode2;
	packetService2.GetAttachMode(requestStatus2, attachMode2);
	
	//wait for first
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(retMode, attachMode);	
	
	//wait for second
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data, dataComplete, this
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSGAM-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetAttachMode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetAttachMode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetAttachMode0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
		
	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::GetAttachMode
 	//-------------------------------------------------------------------------
 	
	// Expect request in LTSY
	TBool sndData = FALSE;
	TMockLtsyData1<TBool> ltsyData(sndData);
	ltsyData.SerialiseL(dataExpect);
	
	iMockLTSY.ExpectL(EPacketGetAttachMode, dataExpect);
	
	// Post request
	TRequestStatus requestStatus;
	RPacketService::TAttachMode attachMode;		
	packetService.GetAttachMode(requestStatus, attachMode);
	User::WaitForRequest(requestStatus);

	// Check status is KErrTimedOut
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int())	
	
	CleanupStack::PopAndDestroy(); //packetService
	CleanupStack::PopAndDestroy(2, this); // dataExpect, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGSC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetStaticCaps
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetStaticCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetStaticCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
  	TRequestStatus mockLtsyStatus;	
  	// Check static caps are 
	// RPacketService::KCapsSuspendSupported |
	// RPacketService::KCapsNetworkAvailabilitySupported | 
	// RPacketService::KCapsSetDefaultContextSupported | 
	// RPacketService::KCapsChangeAttachModeSupported | 
	// RPacketService::KCapsGetDataTransferredSupported | 
	// RPacketService::KCapsPreferredBearerSupported | 
	// RPacketService::KCapsPdpDataCompSupported | 
	// RPacketService::KCapsPdpHeaderCompSupported | 
	// RPacketService::KCapsMSClassSupported |
	// RPacketService::KCapsNotifyMSClassSupported		  	
    TUint staticCaps = 0;
   	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
						    RPacketService::KCapsMSClassSupported |
							RPacketService::KCapsNotifyMSClassSupported;

	TMockLtsyData1<TUint> ltsyData(expectedCaps);
	data.Close();
	ltsyData.SerialiseL(data);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL( EPacketGetStaticCaps, KErrNone, data );
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			   		
   	TInt ret = packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4);
 	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, ret);				
	ASSERT_EQUALS(expectedCaps, staticCaps);

	CleanupStack::PopAndDestroy(3, this); // data, packetService, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSGSC-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetStaticCaps. Caps retrieved from LTSY
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetStaticCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetStaticCaps0001bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	RProperty::Define( KMockLtsyFactoryProperty, EVersionProperty, RProperty::EInt );
	
	//Get current version
	TInt version(0);
	RProperty::Get( KMockLtsyFactoryProperty, EVersionProperty, version );
	RProperty::Set( KMockLtsyFactoryProperty, EVersionProperty, EVersion3 );
	
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
	TUint caps;
	RPacketContext::TProtocolType protType(RPacketContext::EPdpTypeIPv4);

	TInt ret = packetService.GetStaticCaps(caps, protType);

	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, ret);
	
	ASSERT_EQUALS(KMmPacketServiceStaticCaps | RPacketService::KCapsHSUPASupported, caps);
	
	//Reset original version
	RProperty::Set( KMockLtsyFactoryProperty, EVersionProperty, version );
	
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSGSC-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetStaticCaps with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetStaticCaps with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetStaticCaps0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
	TUint caps;

	TInt ret = packetService.GetStaticCaps(caps, (RPacketContext::TProtocolType)(-1));
//    ERR_PRINTF2(_L("<font color=Orange>@CTSYKnownFailure: defect id = %d</font>"), 12701);
	ASSERT_EQUALS(KErrArgument, ret);	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSSAM-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetAttachMode
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetAttachMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetAttachMode0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);	
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenNeeded;
	TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
	ltsyData.SerialiseL(dataExpect);	
	
	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect, KErrNotSupported);
	
	// Post request
	TRequestStatus requestStatus;
	packetService.SetAttachMode(requestStatus, attachMode);		
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
 	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);		
	iMockLTSY.CompleteL(EPacketSetAttachMode, KErrGeneral);
	
	packetService.SetAttachMode(requestStatus, attachMode);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
 	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetAttachMode when result is not cached.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);		
	iMockLTSY.CompleteL(EPacketSetAttachMode, KErrNone);
	
	packetService.SetAttachMode(requestStatus, attachMode);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetSubscriberId
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketSetAttachMode, KErrNone);
	User::WaitForRequest(mockLtsyStatus);

	
 	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(3, this); // dataExpect, dataComplete, this
	}
	
/**
@SYMTestCaseID BA-CTSY-PKTS-PSSAM-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::SetAttachMode
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::SetAttachMode
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetAttachMode0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::SetAttachMode
 	//-------------------------------------------------------------------------

	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenNeeded;
	TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
	ltsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketSetAttachMode, data);		
	iMockLTSY.CompleteL(EPacketSetAttachMode, KErrNone);
	
	// send first request
	TRequestStatus requestStatus;	
	packetService.SetAttachMode(requestStatus, attachMode);	
	
	// send second request 
	TRequestStatus requestStatus2;	
	packetService2.SetAttachMode(requestStatus2, attachMode);		
	
	// wait for first
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for second
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());
	
	CleanupStack::PopAndDestroy(2);  //packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSSAM-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::SetAttachMode with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::SetAttachMode and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestSetAttachMode0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);
		
	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::GetAttachMode
 	//-------------------------------------------------------------------------

	// Expect request in LTSY
	RPacketService::TAttachMode attachMode = RPacketService::EAttachWhenNeeded;
	TMockLtsyData1<RPacketService::TAttachMode> ltsyData(attachMode);
	ltsyData.SerialiseL(dataExpect);	
		
	iMockLTSY.ExpectL(EPacketSetAttachMode, dataExpect);
	
	// Post request
	TRequestStatus requestStatus;		
	packetService.SetAttachMode(requestStatus, attachMode);
	User::WaitForRequest(requestStatus);

	// Check status is KErrTimedOut
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int())	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetService, dataExpect, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCI-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetContextInfo
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetContextInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetContextInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
	

	TRequestStatus requestStatus;
	TInt index(0);
	RPacketService::TContextInfo info;
	
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetContextInfo when result is not cached.
 	//-------------------------------------------------------------------------
	
	packetService.GetContextInfo(requestStatus, index, info);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	TBuf<KMaxName> testBuf(contextId);
	
	ASSERT_EQUALS(testBuf, info.iName);
	ASSERT_EQUALS(RPacketContext::EStatusUnknown, info.iStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCI-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetContextInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetContextInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetContextInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
	
	TRequestStatus requestStatus;
	RPacketService::TContextInfo info;
	TInt index(-1);
	
	packetService.GetContextInfo(requestStatus, index, info);	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);

    index = 1;
	packetService.GetContextInfo(requestStatus, index, info);	
	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(2);  //packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGCI-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetContextInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetContextInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetContextInfo0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetContextInfo
 	//-------------------------------------------------------------------------
	
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);
    
	RPacketContext packetContext;
    TInfoName contextId; 
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
	RPacketContext packetContext2;
    TInfoName contextId2; 
    packetContext2.OpenNewContext(packetService2, contextId2);
    CleanupClosePushL(packetContext2);
    
	
	TRequestStatus requestStatus;
	RPacketService::TContextInfo info;
	TInt index(0);
		
	packetService.GetContextInfo(requestStatus, index, info);		
	

	TRequestStatus requestStatus2;
	RPacketService::TContextInfo info2;
	TInt index2(1);

	packetService2.GetContextInfo(requestStatus2, index2, info2);	

	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	TBuf<KMaxName> testBuf(contextId);
		
	ASSERT_EQUALS(testBuf, info.iName);
	ASSERT_EQUALS(RPacketContext::EStatusUnknown, info.iStatus);

	
	User::WaitForRequest(requestStatus2);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	TBuf<KMaxName> testBuf2(contextId2);
		
	ASSERT_EQUALS(testBuf2, info2.iName);
	ASSERT_EQUALS(RPacketContext::EStatusUnknown, info2.iStatus);


	CleanupStack::PopAndDestroy(4);  //packetService(2) packetContexts(2)
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}




/**
@SYMTestCaseID BA-CTSY-PKTS-PSGDC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetDynamicCaps
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetDynamicCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetDynamicCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
    // first get the default
    RPacketService::TDynamicCapsFlags caps;
    TInt ret = packetService.GetDynamicCaps(caps);
    ASSERT_EQUALS(KErrNone, ret); 
    const RPacketService::TDynamicCapsFlags KZeroCaps(0);
    ASSERT_EQUALS(KZeroCaps, caps); 

	//now change caps
	TBool gprsIsSupportedOnCell = EFalse;
	TBool csServicesIsSupportedOnCell = ETrue;
	TMockLtsyData2 <TBool, TBool> ltsyData(gprsIsSupportedOnCell, csServicesIsSupportedOnCell);
	ltsyData.SerialiseL(data);	
				
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyDynamicCapsChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	
		
    //now get new caps
    ret = packetService.GetDynamicCaps(caps);
    ASSERT_EQUALS(KErrNone, ret); 
    const RPacketService::TDynamicCapsFlags KRxCs(RPacketService::KCapsRxCSCall);
    ASSERT_EQUALS(KRxCs, caps); 


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(); //packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSGDC-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetDynamicCaps (HSUPA)
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetDynamicCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetDynamicCaps0001bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    RPacketService::TDynamicCapsFlags caps;
    
    // Get default caps
    TInt ret = packetService.GetDynamicCaps(caps);
    ASSERT_EQUALS(KErrNone, ret); 
    const RPacketService::TDynamicCapsFlags KZeroCaps(0);
    ASSERT_EQUALS(KZeroCaps, caps); 
    
    // Change caps
    TDynamicTransferCapsFlags transferCaps = KCapsHSDPA | KCapsHSUPA;
	TMockLtsyData1< TDynamicTransferCapsFlags > ltsyData(transferCaps);
	data.Close();
	ltsyData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketNotifyTransferCapsIPC, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	
	// Get new caps
	ret = packetService.GetDynamicCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    
    const RPacketService::TDynamicCapsFlags KHsupa(RPacketService::KCapsHSUPA);
    ASSERT_TRUE(KHsupa&caps);
    
    AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(3); // packetService, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PSGNRS-0001
@SYMPREQ 1551
@SYMCR 1595
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetNtwkRegStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetNtwkRegStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    	
    //-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	// Queue response
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus, KErrNotSupported);
	// Post request
	TRequestStatus requestStatus;
	RPacketService::TRegistrationStatus regStatus;
	packetService.GetNtwkRegStatus(requestStatus, regStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetNtwkRegStatus when result is not cached.
 	//-------------------------------------------------------------------------
    RPacketService::TRegistrationStatus reqStatusComplete = RPacketService::ERegisteredOnHomeNetwork;
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData(reqStatusComplete);
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data);
	regStatus = RPacketService::TRegistrationStatus(0);

	packetService.GetNtwkRegStatus(requestStatus, regStatus);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE(reqStatusComplete == regStatus);
	
	//-------------------------------------------------------------------------
	// TEST : Completion of request 
	// RPacketService::GetNtwkRegStatus with extended error code.
 	//-------------------------------------------------------------------------
  
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound), data);
	regStatus = RPacketService::TRegistrationStatus(0);

	packetService.GetNtwkRegStatus(requestStatus, regStatus);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmCCNetworkOutOfOrder, requestStatus.Int());
	
	// Check that basic error code is returned when error granuality set to EErrorBasic
	RTelServer::TErrorGranularity granularity(RTelServer::EErrorBasic);

	TInt ret = iTelServer.SetExtendedErrorGranularity(granularity);
	ASSERT_EQUALS(KErrNone, ret);

	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, SYMBIAN_EXTERR(KErrGsmCCNetworkOutOfOrder,KErrNotFound), data);
	regStatus = RPacketService::TRegistrationStatus(0);

	packetService.GetNtwkRegStatus(requestStatus, regStatus);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::SetDefaultContextParams
	// from LTSY.
 	//-------------------------------------------------------------------------
  	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
 	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data);
 	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//-------------------------------------------------------------------------
	// TEST E_2: Unsolicited completion of RPacketService::SetDefaultContextParams
	// from LTSY with the same reqStatusComplete
 	//-------------------------------------------------------------------------
  	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
 	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data);
 	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//-------------------------------------------------------------------------
	// TEST E_3: Unsolicited completion of RPacketService::SetDefaultContextParams
	// from LTSY when there is NotifyChangeOfNtwkRegStatus was called
 	//-------------------------------------------------------------------------	
	RPacketService::TRegistrationStatus registrationStatus;		
	// Post NotifyChangeOfNtwkRegStatus request		
	packetService.NotifyChangeOfNtwkRegStatus(requestStatus, registrationStatus);
		
	reqStatusComplete = RPacketService::ERegisteredRoaming;
	TMockLtsyData1<RPacketService::TRegistrationStatus> ltsyData2(reqStatusComplete);
	data.Close();
	ltsyData2.SerialiseL(data);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
 	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data);
 	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(reqStatusComplete, registrationStatus);
	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrGeneral, data);
	packetService.GetNtwkRegStatus(requestStatus, regStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
   	ASSERT_EQUALS(KErrGeneral, requestStatus.Int()); 


	AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSGNRS-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetNtwkRegStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetNtwkRegStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetNtwkRegStatus0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);
    

    const RPacketService::TRegistrationStatus KRegStatus(RPacketService::ENotRegisteredNotSearching);
    
	// Expect call down to LTSY for this IPC from first client
	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);

	RPacketService::TRegistrationStatus regStatus(KRegStatus);
	TMockLtsyData1<RPacketService::TRegistrationStatus> regData(regStatus);
	regData.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketGetNtwkRegStatus, KErrNone, data);

	RPacketService::TRegistrationStatus regStatus1;
	TRequestStatus requestStatus1;
	packetService.GetNtwkRegStatus(requestStatus1, regStatus1);

	RPacketService::TRegistrationStatus regStatus2;
	TRequestStatus requestStatus2;
	packetService2.GetNtwkRegStatus(requestStatus2, regStatus2);

	// Check result for first client
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	ASSERT_TRUE(regStatus1 == KRegStatus);

	// Check result for second client
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // packetService packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSGNRS-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetNtwkRegStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetNtwkRegStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetNtwkRegStatus0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    	
    	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::GetNtwkRegStatus
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketGetNtwkRegStatus);

	TRequestStatus requestStatus;
	RPacketService::TRegistrationStatus regStatus;
	
	packetService.GetNtwkRegStatus(requestStatus, regStatus);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int()); 

    CleanupStack::PopAndDestroy(); // packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}



/**
@SYMTestCaseID BA-CTSY-PKTS-PSEC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::EnumerateContexts
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateContexts
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateContexts0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService); 


	
	//-------------------------------------------------------------------------
	// TEST D: RPacketService::EnumerateContexts again, this time CTSY
	// will get result from the cache.
 	//------------------------------------------------------------------------- 	
	TRequestStatus requestStatus;
 	TInt count;
 	TInt maxAllowed; 	
	
	packetService.EnumerateContexts(requestStatus, count, maxAllowed);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, count);
	// Maximum number of simultaneous contexts supported by this phone
	const TInt maxNumberOfContexts = 11;
	ASSERT_EQUALS(maxNumberOfContexts, maxAllowed);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::EnumerateContexts when result is not cached.
 	//-------------------------------------------------------------------------	
  
    // Open new context
	TInfoName contextId;
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext); 
	
	// send request to get number of opened contexts
	packetService.EnumerateContexts(requestStatus, count, maxAllowed);	
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(1, count);
	ASSERT_EQUALS(maxNumberOfContexts, maxAllowed);
 
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2);// packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this	
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PSEC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::EnumerateContexts
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::EnumerateContexts
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateContexts0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::EnumerateContexts
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus;
 	TInt count;
 	TInt maxAllowed; 
 	
 	TRequestStatus requestStatus2;
 	TInt count2;
 	TInt maxAllowed2;	
	
	//first request
	packetService.EnumerateContexts(requestStatus, count, maxAllowed);
	packetService2.EnumerateContexts(requestStatus2, count2, maxAllowed2);
	
	//wait for first answer
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, count);
	// Maximum number of simultaneous contexts supported by this phone
	const TInt maxNumberOfContexts = 11;
	ASSERT_EQUALS(maxNumberOfContexts, maxAllowed);
	
	//wait for second answer
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(0, count2);
	ASSERT_EQUALS(maxNumberOfContexts, maxAllowed2);
		
	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}
	
	

/**
@SYMTestCaseID BA-CTSY-PKTS-PSCNEIC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for internal CMmPacketServiceTsy::CompleteNotifyEGprsInfoChange
@SYMTestPriority High
@SYMTestExpectedResults Pass
@SYMTestType CT 
*/
void CCTsyPacketServiceFU::TestCompleteNotifyEGprsInfoChange0001L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TBool lastEdgeGprsSupport = EFalse;	
	TMockLtsyData1 <TBool> ltsyData(lastEdgeGprsSupport);
	ltsyData.SerialiseL(data);	
		
	//-------------------------------------------------------------------------
	// Test CompleteNotifyEGprsInfoChange with no CustomTsy
 	//-------------------------------------------------------------------------
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());			
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// Test CompleteNotifyEGprsInfoChange with CustomTsy
 	//-------------------------------------------------------------------------
	
	// prepare iMockLTSY.ExpectLs for RMmCustomAPI opening
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
	RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
	currentlyRetrievedCache.iCacheId	= 1;
	currentlyRetrievedCache.iRecordId	= 0;		
	TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
	data.Close();
	ltsyDataCustomAPI.SerialiseL(data);			
	iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);
				
	// Create CustomTsy			
	RMmCustomAPI customAPI;		
	TInt ret = customAPI.Open (iPhone);	
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(customAPI);
	
		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	iMockLTSY.CompleteL(EMmTsyNotifyEGprsInfoChangeIPC, KErrNone, data);	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	CleanupStack::PopAndDestroy(4, this); //  customAPI, packetService, data, this
	
	}
	
	
	
	
/**
@SYMTestCaseID BA-CTSY-PKTS-PSCSAO-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for internal CMmPacketServiceTsy::CompleteSetAlwaysOn
@SYMTestPriority High
@SYMTestExpectedResults Pass
@SYMTestType CT 
*/
void CCTsyPacketServiceFU::TestCompleteSetAlwaysOn0001L()
	{	
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// data for CompleteL
	TInt result = 0;
	TMockLtsyData1 <TInt> ltsyData(result);
	ltsyData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// Test CompleteSetAlwaysOn with no CustomTsy
	// and when  EMultimodePacketServiceReqHandleUnknown == reqHandle 
	// (line 3157 in CMmPacketServiceTsy.cpp)
 	//-------------------------------------------------------------------------
 	TRequestStatus mockLtsyStatus;
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrNone, data);	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());			
	AssertMockLtsyStatusL();
		
	
	//-------------------------------------------------------------------------
	// Test CompleteSetAlwaysOn with  CustomTsy
 	//------------------------------------------------------------------------- 	 	
 	
	// prepare iMockLTSY.ExpectLs for RMmCustomAPI opening
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);
	RMmCustomAPI::TViagCacheRecordId currentlyRetrievedCache;
	currentlyRetrievedCache.iCacheId	= 1;
	currentlyRetrievedCache.iRecordId	= 0;		
	TMockLtsyData1 <RMmCustomAPI::TViagCacheRecordId> ltsyDataCustomAPI(currentlyRetrievedCache);
	data.Close();
	ltsyDataCustomAPI.SerialiseL(data);			
	iMockLTSY.ExpectL(EReadViagHomeZoneCacheIPC, data);
	
 	// Create CustomTsy			
	RMmCustomAPI customAPI;		
	TInt ret = customAPI.Open (iPhone);	
	ASSERT_EQUALS(KErrNone, ret); 	
	CleanupClosePushL(customAPI);
	
	TRequestStatus requestStatus; 
	RMmCustomAPI::TSetAlwaysOnMode mode = RMmCustomAPI::EAlwaysModeBoth; 	
	
	// data for ExpectL
	TMockLtsyData1 <RMmCustomAPI::TSetAlwaysOnMode> expData(mode);
	expData.SerialiseL(expectData);
	
	// prepare ExpectL and CompleteL
	iMockLTSY.ExpectL(ECustomSetAlwaysOnMode, expectData);	
	iMockLTSY.CompleteL(ECustomSetAlwaysOnMode, KErrNone, data);
 	
 	// Send request to set AlwaysOnMode	 
    customAPI.SetAlwaysOn(requestStatus, mode);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());				
	
	CleanupStack::PopAndDestroy(5, this); // customAPI, packetService,expectData,  data, this	
	}
	

/**
@SYMTestCaseID BA-CTSY-PKTS-PTMS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::CompleteGetStaticCaps
@SYMTestPriority 
@SYMTestActions Invokes RPacketService::CompleteGetStaticCaps with KCapsMBMSSupported capability
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestTsyMbmsSupportCaps0001L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService, RPacketService::EStatusUnattached, RPacketService::EAttachWhenNeeded);
    CleanupClosePushL(packetService);
  	TRequestStatus mockLtsyStatus;
 	
 	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RPacketService::CompleteGetStaticCaps.
 	//------------------------------------------------------------------------- 
 			
 	// data for ExpectL
	// Ensure RPacketService::GetStaticCaps returns caps KCapsMBMSSupported.			  	
    TUint staticCaps = 0;
   	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
						    RPacketService::KCapsMSClassSupported |
							RPacketService::KCapsNotifyMSClassSupported |
							RPacketService::KCapsMBMSSupported;	

	TMockLtsyData1<TUint> ltsyData(expectedCaps);
	data.Close();
	ltsyData.SerialiseL(data);    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL( EPacketGetStaticCaps, KErrNone, data );
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			
	TInt ret = packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4);
	ASSERT_EQUALS(KErrNone, ret);	 	
	ASSERT_EQUALS(staticCaps, expectedCaps);
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 3, this );  // packetService, data, this	
    }
    
/**
@SYMTestCaseID BA-CTSY-PKTS-PTMS-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::CompleteGetStaticCaps
@SYMTestPriority 
@SYMTestActions Invokes RPacketService::CompleteGetStaticCaps without KCapsMBMSSupported capability
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestTsyMbmsSupportCaps0001bL()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService, RPacketService::EStatusUnattached, RPacketService::EAttachWhenNeeded);
    CleanupClosePushL(packetService);
  	TRequestStatus mockLtsyStatus;
 	
 	//-------------------------------------------------------------------------
	// TEST A: Successful completion request of
	// RPacketService::CompleteGetStaticCaps.
 	//------------------------------------------------------------------------- 
 			
 	// data for ExpectL
	// Ensure RPacketService::GetStaticCaps returns caps KCapsMBMSSupported.			  	
    TUint staticCaps = 0;
   	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
						    RPacketService::KCapsMSClassSupported |
							RPacketService::KCapsNotifyMSClassSupported;

	TMockLtsyData1<TUint> ltsyData(expectedCaps);
	data.Close();
	ltsyData.SerialiseL(data);    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL( EPacketGetStaticCaps, KErrNone, data );
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			
	TInt ret = packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4);
	ASSERT_EQUALS(KErrNone, ret);	 	
	ASSERT_EQUALS(staticCaps, expectedCaps);
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 3, this );  // packetService, data, this	
    }
        
/**
@SYMTestCaseID BA-CTSY-PKTS-PNMNSSC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMbmsNetworkServiceStatusChange0001L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
 
 	TRequestStatus mockLtsyStatus;
 	   	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion 
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TMbmsNetworkServiceStatus sndData0 = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData0(sndData0);
	ltsyData0.SerialiseL(data); 
	
	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyMbmsNetworkServiceStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus	requestStatus;
	TMbmsNetworkServiceStatus mbmsNetworkServiceStatus;
	
	// sent request
	packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, mbmsNetworkServiceStatus);
	
	// firstly issue notification  from  the MockLTSY 
    // which has the same service status as is already saved
   	TMbmsNetworkServiceStatus compData = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData(compData);
	data.Close();
	ltsyData.SerialiseL(data);    
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// Issue the Complete with the same service status state as is already saved
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	 
	
	// check that NotifyMbmsNetworkServiceStatusChange isn't completed
	ASSERT_EQUALS (KRequestPending, requestStatus.Int());
	
	//data for Complete with new service status
	TMbmsNetworkServiceStatus compData2 = EMbmsNotSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData2(compData2);
	data.Close();
	ltsyData2.SerialiseL(data);		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// Issue the Complete with new service status
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	
	
	// wait for completion of NotifyMbmsNetworkServiceStatusChange
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(EMbmsNotSupported, mbmsNetworkServiceStatus);
 	//-------------------------------------------------------------------------
	// TEST D: RPacketService::NotifyMbmsNetworkServiceStatusChange again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	// sent request
	packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, mbmsNetworkServiceStatus);
	
	//data for Complete with new service status
	TMbmsNetworkServiceStatus compData3 = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData3(compData3);
	data.Close();
	ltsyData3.SerialiseL(data);		
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// Issue the Complete with new service status
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(EMbmsSupported, mbmsNetworkServiceStatus);
    		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 3, this );  // packetService, data, this		
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PNMNSSC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMbmsNetworkServiceStatusChange0002L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
 
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyMbmsNetworkServiceStatusChange
 	//-------------------------------------------------------------------------

 	TRequestStatus requestStatus;
	TMbmsNetworkServiceStatus regStatus;	
	
	packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, regStatus);
    packetService.CancelAsyncRequest(EPacketNotifyMbmsNetworkServiceStatusChange); 	

	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2);// packetServicethis, data
		
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PNMNSSC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyMbmsNetworkServiceStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMbmsNetworkServiceStatusChange0003L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyChangeOfNtwkRegStatus
 	//-------------------------------------------------------------------------
	
	TMbmsNetworkServiceStatus sndData = EMbmsNotSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData(sndData);
	ltsyData.SerialiseL(data); 	 	
 		
	TRequestStatus requestStatus;
	TMbmsNetworkServiceStatus regStatus;	
	
	TRequestStatus requestStatus2;
	TMbmsNetworkServiceStatus regStatus2;	
	
	// Post first request	
	packetService.NotifyMbmsNetworkServiceStatusChange(requestStatus, regStatus);
		
	// Post second request	
	packetService2.NotifyMbmsNetworkServiceStatusChange(requestStatus2, regStatus2);
				
	// Issue the Complete...	
 	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	
	// wait for first
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(sndData, regStatus);
	
	// wait for second
	User::WaitForRequest(requestStatus2);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	ASSERT_EQUALS(sndData, regStatus2);	

	CleanupStack::PopAndDestroy(2); // packetService, packetService2
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PGMNSSC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetMbmsNetworkServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetMbmsNetworkServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetMbmsNetworkServiceStatus0001L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    //-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Post request
    TRequestStatus requestStatus;
    TMbmsNetworkServiceStatus mbmsNetworkServiceStatus;
	
	// Queue response
    TBool expData = ETrue;
	TMockLtsyData1<TBool> ltsyData(expData);
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data,  KErrNotSupported);

	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data,  KErrNone);

	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);

	iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrGeneral);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetMbmsNetworkServiceStatus --> EMbmsSupported.
 	//-------------------------------------------------------------------------	

    // data for ExpectL
	data.Close();
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data,  KErrNone);

	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);

	// data for CompleteL
	TMbmsNetworkServiceStatus completeMbmsNetworkServiceStatus = EMbmsSupported;
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyCompData(completeMbmsNetworkServiceStatus);
	data.Close();
	ltsyCompData.SerialiseL(data);
	iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data);

	
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_EQUALS(completeMbmsNetworkServiceStatus, mbmsNetworkServiceStatus);
		
	//-------------------------------------------------------------------------
	// TEST D: Successful completion request of
	// RPacketService::GetMbmsNetworkServiceStatus --> read value from CTSY cache.
 	//-------------------------------------------------------------------------	
 	
	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_EQUALS(completeMbmsNetworkServiceStatus, mbmsNetworkServiceStatus);
	
	//-------------------------------------------------------------------------
	// TEST E: changing the cache with notifiyer
	// RPacketService::GetMbmsNetworkServiceStatus --> read value from CTSY cache.
 	//-------------------------------------------------------------------------	
	completeMbmsNetworkServiceStatus = EMbmsNotSupported;
	data.Close();
	TRequestStatus mockLtsyStatus;
	ltsyCompData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketNotifyMbmsNetworkServiceStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus); 	

	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_EQUALS(completeMbmsNetworkServiceStatus, mbmsNetworkServiceStatus);
	
	//-------------------------------------------------------------------------
	// TEST F: changing the cache with getter
	// RPacketService::GetMbmsNetworkServiceStatus --> read value from CTSY cache.
 	//-------------------------------------------------------------------------	
	completeMbmsNetworkServiceStatus = EMbmsSupported;
	data.Close();
	ltsyCompData.SerialiseL(data);
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus); 	

	packetService.GetMbmsNetworkServiceStatus(requestStatus, ETrue, mbmsNetworkServiceStatus);
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_EQUALS(completeMbmsNetworkServiceStatus, mbmsNetworkServiceStatus);
	
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 3, this );  //  packetService, data, this	
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PGMNSSC-0001b
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetMbmsNetworkServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetMbmsNetworkServiceStatus with status EMbmsNotSupported
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyPacketServiceFU::TestGetMbmsNetworkServiceStatus0001bL()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetMbmsNetworkServiceStatus --> EMbmsNotSupported.
 	//-------------------------------------------------------------------------	
 	
 	// Post request
    TRequestStatus requestStatus;
    TBool aAttemptAttach = ETrue;
   	TMbmsNetworkServiceStatus regStatus;
   	
     // data for ExpectL
    TMbmsNetworkServiceStatus regStatusComplete = EMbmsNotSupported;
    TMockLtsyData1<TBool> ltsyData(aAttemptAttach);
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus,data);     
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData2(regStatusComplete);
	ltsyData2.SerialiseL(data2);
    // data for CompleteL
	iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data2);
	regStatus = TMbmsNetworkServiceStatus(2);
	
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_TRUE(regStatusComplete == regStatus);
	
	//-------------------------------------------------------------------------
	// TEST D: Successful completion request of
	// RPacketService::GetMbmsNetworkServiceStatus --> read value from CTSY cache.
 	//-------------------------------------------------------------------------	
 	
  	// data for ExpectL
  	aAttemptAttach = EFalse;	
    TMbmsNetworkServiceStatus aMBMSServiceStatus = EMbmsNotSupported;  
    TMockLtsyData1<TBool> ltsyDataa(aAttemptAttach);
	ltsyDataa.SerialiseL(data); 

	regStatus = TMbmsNetworkServiceStatus(2);
		
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
    User::WaitForRequest(requestStatus);
	ASSERT_EQUALS( KErrNone, requestStatus.Int() ); 
	ASSERT_TRUE(aMBMSServiceStatus == regStatus);
	
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 4, this );  // packetService, data2, data, this	
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PGMNSSC-0001c
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetMbmsNetworkServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetMbmsNetworkServiceStatus with status EMbmsSupportUnknown
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyPacketServiceFU::TestGetMbmsNetworkServiceStatus0001cL()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::GetMbmsNetworkServiceStatus --> EMbmsNotSupported.
 	//-------------------------------------------------------------------------	
 	
 	// Post request
    TRequestStatus requestStatus;
    TBool aAttemptAttach = ETrue;
    TMbmsNetworkServiceStatus regStatus;
   	
     // data for ExpectL
    TMbmsNetworkServiceStatus regStatusComplete = EMbmsSupportUnknown;
    TMockLtsyData1<TBool> ltsyData(aAttemptAttach);
	ltsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus,data);     
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData2(regStatusComplete);
	ltsyData2.SerialiseL(data2);
    // data for CompleteL
	iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data2);
	regStatus = TMbmsNetworkServiceStatus(0);
	
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
    User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );   
	ASSERT_TRUE(regStatusComplete == regStatus);
	
	//-------------------------------------------------------------------------
	// TEST D: Successful completion request of
	// RPacketService::GetMbmsNetworkServiceStatus --> read value from CTSY cache.
 	//-------------------------------------------------------------------------	
 	
 	 aAttemptAttach = EFalse;	
    // data for ExpectL
    TMbmsNetworkServiceStatus aMBMSServiceStatus = EMbmsSupportUnknown;  
    TMockLtsyData1<TBool> ltsyDataa(aAttemptAttach);
	ltsyDataa.SerialiseL(data); 

	regStatus = TMbmsNetworkServiceStatus(0);
		
	packetService.GetMbmsNetworkServiceStatus(requestStatus, aAttemptAttach, regStatus);
    User::WaitForRequest(requestStatus);
	ASSERT_EQUALS( KErrNone, requestStatus.Int() ); 
	ASSERT_TRUE(aMBMSServiceStatus == regStatus);
	
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 4, this );  // packetService, data2, data, this	
    }
    	
/**
@SYMTestCaseID BA-CTSY-PKTS-PGMNSSC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::GetMbmsNetworkServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::GetMbmsNetworkServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetMbmsNetworkServiceStatus0002L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    	
    //-------------------------------------------------------------------------
    // Test cancelling of RPacketService::GetMbmsNetworkServiceStatus
    //-------------------------------------------------------------------------
     	
    // data for ExpectL
    TBool attemptAttach = ETrue;
    TMbmsNetworkServiceStatus regStatusComplete = EMbmsSupported;
    TMockLtsyData1<TBool> ltsyData(attemptAttach);
    data.Close();
    ltsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data); 
    	    
    // Post request
    TRequestStatus requestStatus;
	packetService.GetMbmsNetworkServiceStatus(requestStatus, attemptAttach, regStatusComplete);
    packetService.CancelAsyncRequest(EPacketGetMbmsNetworkServiceStatus);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	AssertMockLtsyStatusL();		
	
	CleanupStack::PopAndDestroy( 3, this );  // packetService, data, this	
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PGMNSSC-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::GetMbmsNetworkServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::GetMbmsNetworkServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetMbmsNetworkServiceStatus0003L()
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret); 
    CleanupClosePushL(packetService2);   
    
    //-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetMbmsNetworkServiceStatus
	// iMbmsStatuscached = FALSE
 	//-------------------------------------------------------------------------
    
    // data for ExpectL    	
    TBool aAttemptAttach = ETrue;
	TMockLtsyData1<TBool> ltsyData2(aAttemptAttach);
	ltsyData2.SerialiseL(data2);
    
	// first client
	TMbmsNetworkServiceStatus regStatus1 = EMbmsSupported;
	iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data2);     
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData(regStatus1);
	ltsyData.SerialiseL(data);
	
    // data for CompleteL
	iMockLTSY.CompleteL(EPacketGetMbmsNetworkServiceStatus, KErrNone, data);	
	
	// send first request
	TRequestStatus requestStatus1;
  	packetService.GetMbmsNetworkServiceStatus(requestStatus1, aAttemptAttach, regStatus1);
   	
	// send second request
	TRequestStatus requestStatus2;
    TMbmsNetworkServiceStatus regStatus2 = EMbmsNotSupported;
   	packetService2.GetMbmsNetworkServiceStatus(requestStatus2, aAttemptAttach, regStatus2);

	//wait for first answer
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	//wait for second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());

    //-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::GetMbmsNetworkServiceStatus
	// iMbmsStatuscached = TRUE
 	//-------------------------------------------------------------------------
 	
 	// data for ExpectL    	
    aAttemptAttach = EFalse;
	TMockLtsyData1<TBool> ltsyData3(aAttemptAttach);
	ltsyData3.SerialiseL(data2);
    
	// first client
	regStatus1 = EMbmsSupported;    
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyDataa(regStatus1);
	ltsyDataa.SerialiseL(data);	
	
	// send first request
  	packetService.GetMbmsNetworkServiceStatus(requestStatus1, aAttemptAttach, regStatus1);
   	
	// send second request
    regStatus2 = EMbmsNotSupported;
   	packetService2.GetMbmsNetworkServiceStatus(requestStatus2, aAttemptAttach, regStatus2);

	//wait for first answer
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	//wait for second answer
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetService packetService2
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data2, data, this
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PGMNSSC-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::GetMbmsNetworkServiceStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::GetMbmsNetworkServiceStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestGetMbmsNetworkServiceStatus0005L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 data2;
	CleanupClosePushL(data2);
	
    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    	
    	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::GetMbmsNetworkServiceStatus
 	//-------------------------------------------------------------------------

	// Post request
	TMbmsNetworkServiceStatus regStatus = EMbmsSupported;
	TRequestStatus requestStatus;
    TBool aAttemptAttach = ETrue;
    
	// data for ExpectL   
	TMockLtsyData1<TBool> ltsyData2(aAttemptAttach);
	ltsyData2.SerialiseL(data2);
    iMockLTSY.ExpectL(EPacketGetMbmsNetworkServiceStatus, data2); 
     
	TMockLtsyData1<TMbmsNetworkServiceStatus> ltsyData(regStatus);
	ltsyData.SerialiseL(data);	

	packetService.GetMbmsNetworkServiceStatus(requestStatus,aAttemptAttach, regStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(4, this); // packetService, data2, data, this

    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PNMSASC-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::NotifyMbmsServiceAvailabilityChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::NotifyMbmsServiceAvailabilityChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestNotifyMbmsServiceAvailabilityChange0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup,this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService ); 
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
 
	TInt maxValue = 10;
 
 	// set maximum monitored service value during TSY startup.
 	TMockLtsyData1<TInt> ltsyData(maxValue);
 	data.Close();
	ltsyData.SerialiseL(data);
 	// data for CompleteL
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

  	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Issue the Complete...
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
   	iMockLTSY.CompleteL(EPacketNotifyMbmsServiceAvailabilityChange, KErrNone);
   		
	User::WaitForRequest(mockLtsyStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
  	//-------------------------------------------------------------------------
	// TEST B: Test Notify after update
 	//-------------------------------------------------------------------------
	
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	RPacketService::TMbmsServiceAvailabilityV1 entry1, entry2;
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		entry1.iTmgi.SetServiceId(i); 
		entry1.iTmgi.SetMCC(i);
		entry1.iTmgi.SetMNC(i);
		entry1.iMbmsServiceMode = KBroadcast;
		entry1.iMbmsAccessBearer = E2GBearer;
		entry1.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( entry1 );
		}
	
	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);

	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);

    // post notifiyer
	packetService.NotifyMbmsServiceAvailabilityChange(requestStatus);
	TRequestStatus reqStatus;
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries);	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	//wait for notifiyer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS( KErrNone, requestStatus.Int() );
	
			
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyMbmsServiceAvailabilityChange.
 	//-------------------------------------------------------------------------
  
	// post a request
	packetService.NotifyMbmsServiceAvailabilityChange(requestStatus);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
   	iMockLTSY.CompleteL(EPacketNotifyMbmsServiceAvailabilityChange, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	 
	
	// wait for completion of NotifyMbmsServiceAvailabilityChange
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::NotifyMbmsServiceAvailabilityChange with one available entries.
 	//-------------------------------------------------------------------------
    CPcktMbmsMonitoredServiceList* availEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(availEntries);
	availEntries->AddEntryL(multipleEntries->GetEntryL(1));
    TMockLtsyData1<CPcktMbmsMonitoredServiceList*> expExpect2(availEntries);
    data.Close();
    expExpect2.SerialiseL(data);

	// post a request
	packetService.NotifyMbmsServiceAvailabilityChange(requestStatus);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
   	iMockLTSY.CompleteL(EPacketNotifyMbmsServiceAvailabilityChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	 
	
	// wait for completion of NotifyMbmsServiceAvailabilityChange
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	//Check that the avaiability is correct
  	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);
	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());	
 	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	ASSERT_EQUALS(multipleEntries->Enumerate(), list->Enumerate());
	for(TInt i = 0; i < multipleEntries->Enumerate(); ++i)
		{
		entry1 = multipleEntries->GetEntryL(i);
		entry2 = list->GetEntryL(i);
		ASSERT_EQUALS(entry1.iMbmsAccessBearer, entry2.iMbmsAccessBearer);
		ASSERT_EQUALS(entry1.iMbmsServiceMode, entry2.iMbmsServiceMode);
		ASSERT_EQUALS(entry1.iTmgi.GetServiceId(), entry2.iTmgi.GetServiceId());
		ASSERT_EQUALS(entry1.iTmgi.GetMCC(), entry2.iTmgi.GetMCC());
		ASSERT_EQUALS(entry1.iTmgi.GetMNC(), entry2.iTmgi.GetMNC());
		if(i == 1)
			{
			ASSERT_EQUALS(EMbmsAvailable, entry2.iMbmsAvailabilityStatus);
			}
		else
			{
			ASSERT_EQUALS(EMbmsUnavailable, entry2.iMbmsAvailabilityStatus);
			}
		}
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::UpdateMbmsMonitorServiceListL
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	 	
 	// Issue the Complete...	
 	iMockLTSY.CompleteL(EPacketNotifyMbmsServiceAvailabilityChange, KErrNone);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 			   	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(9, this);
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PNMSASC-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc 
@SYMTestPriority High 
@SYMTestActionsInvokes cancelling of RPacketService::NotifyMbmsServiceAvailabilityChange 
@SYMTestExpectedResults Pass 
@SYMTestType CT 
*/
void CCTsyPacketServiceFU::TestNotifyMbmsServiceAvailabilityChange0002L()
    {
    OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    RPacketService packetService;       
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
 
  	TRequestStatus requestStatus;	

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::NotifyMbmsServiceAvailabilityChange
 	//-------------------------------------------------------------------------
 				
	// sent request
	packetService.NotifyMbmsServiceAvailabilityChange(requestStatus);

    // Cancel request
    packetService.CancelAsyncRequest(EPacketNotifyMbmsServiceAvailabilityChange);
    
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetservice, data, this
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PNMSASC-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc 
@SYMTestPriority High 
@SYMTestActions Invokes multiple client requests to RPacketService::NotifyMbmsServiceAvailabilityChange  
@SYMTestExpectedResults Pass 
@SYMTestType CT 
*/
void CCTsyPacketServiceFU::TestNotifyMbmsServiceAvailabilityChange0004L()
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	RPacketService packetService2;               
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2); 
	
	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;
	TRequestStatus mockLtsyStatus;
 
   	TInt maxAllowed = 10; 
	TInt maxvalue = 10;
 	
 	// check maximum monitored service value during TSY startup.
 	TMockLtsyData1<TInt> ltsyData(maxvalue);
 	data.Close();
	ltsyData.SerialiseL(data);
	
 	// data for CompleteL
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	ASSERT_EQUALS( maxvalue, maxAllowed );
	
   	//**** Add existing list ****//
 	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	//List retriever
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
    
	RBuf8 completeData;
	CleanupClosePushL(completeData);    
	
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	
	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		existingParams.iTmgi.SetServiceId(i); 
	    existingParams.iTmgi.SetMCC(i);
	    existingParams.iTmgi.SetMNC(i);
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL(existingParams);
		}
	
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> params(expData);
    data.Close();
    params.SerialiseL(data);

	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, completeData);
	
	packetService.UpdateMbmsMonitorServiceListL(requestStatus, EAddEntries, multipleEntries);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 
		
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL(list);
	
	ASSERT_EQUALS(MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate() );

	CleanupStack::PopAndDestroy(list);

   	//**** Add existing list ****//
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::NotifyMbmsServiceAvailabilityChange
 	//-------------------------------------------------------------------------
  
	// Post first request	
	packetService.NotifyMbmsServiceAvailabilityChange(requestStatus);
	
	// Post second request
	packetService2.NotifyMbmsServiceAvailabilityChange(requestStatus2);
	
	// Issue the Complete...
	CPcktMbmsMonitoredServiceList* availEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(availEntries);
	
	RPacketService::TMbmsServiceAvailabilityV1 updatedParams;

	updatedParams.iTmgi.SetServiceId(1); 
	updatedParams.iTmgi.SetMCC(1);
	updatedParams.iTmgi.SetMNC(1);
	updatedParams.iMbmsServiceMode = KBroadcast;
	updatedParams.iMbmsAccessBearer = E2GBearer;
	updatedParams.iMbmsAvailabilityStatus = EMbmsAvailable;
	
	availEntries->AddEntryL(updatedParams);
	TMockLtsyData1<CPcktMbmsMonitoredServiceList*> availParams(availEntries);
	data.Close();
	availParams.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
   	iMockLTSY.CompleteL(EPacketNotifyMbmsServiceAvailabilityChange, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	 

	// wait for second completion of NotifyMbmsServiceAvailabilityChange
	User::WaitForRequest(requestStatus2);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());		
		
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	
	list = monitorList->RetrieveListL();
	CleanupStack::PushL(list);
		
	RPacketService::TMbmsServiceAvailabilityV1 entry1, entry2;
	
	ASSERT_EQUALS(multipleEntries->Enumerate(), list->Enumerate());
	
	for(TInt i = 0; i < multipleEntries->Enumerate(); ++i)
		{
		entry1 = multipleEntries->GetEntryL(i);
		entry2 = list->GetEntryL(i);
		ASSERT_EQUALS(entry1.iMbmsAccessBearer, entry2.iMbmsAccessBearer);
	  	ASSERT_EQUALS(entry1.iMbmsServiceMode, entry2.iMbmsServiceMode);
	  	ASSERT_EQUALS(entry1.iTmgi.GetServiceId(), entry2.iTmgi.GetServiceId());
	  	ASSERT_EQUALS(entry1.iTmgi.GetMCC(), entry2.iTmgi.GetMCC());
	  	ASSERT_EQUALS(entry1.iTmgi.GetMNC(), entry2.iTmgi.GetMNC());
	  	if(i == 1)
	  		{
	  		ASSERT_EQUALS(EMbmsAvailable, entry2.iMbmsAvailabilityStatus);
	 		}
		else
			{
			ASSERT_EQUALS(EMbmsUnavailable, entry2.iMbmsAvailabilityStatus);
			}
		}

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(13, this); // availEntries, list, zeroEntry, multipleEntries, completeData,
	// activeRetriever, monitorList, packetService2, packetService, phone2, telServer2, data, this
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PUMMSL-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::UpdateMbmsMonitorServiceListL
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::UpdateMbmsMonitorServiceListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/

void CCTsyPacketServiceFU::TestUpdateMbmsMonitorServiceList0001L()
    {
  	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

  	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	TInt maxvalue = 100;
 	TMockLtsyData1<TInt> ltsyData(maxvalue);
 	data.Close();
	ltsyData.SerialiseL(data);
	// Set the maximal number of services 
 	TRequestStatus mockLtsyStatus;
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	CPcktMbmsMonitoredServiceList* oneEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(oneEntry);
    RPacketService::TMbmsServiceAvailabilityV1 entry;
    entry.iTmgi.SetServiceId(15);
    entry.iTmgi.SetMCC(15);
    entry.iTmgi.SetMNC(6);
    entry.iMbmsServiceMode = KBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    oneEntry->AddEntryL(entry);

	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);

	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = oneEntry; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data, KErrNotSupported);

	TRequestStatus reqStatus;
 	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, oneEntry);

	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());	
	
	
 	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_EQUALS(0, list->Enumerate() );
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*zeroEntry, *list) );
	CleanupStack::PopAndDestroy( list );
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = oneEntry; // list to add
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);

	
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, oneEntry);	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrGeneral);	
	User::WaitForRequest(reqStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	
	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*zeroEntry, *list) );
	CleanupStack::PopAndDestroy( list );
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::UpdateMbmsMonitorServiceListL
 	//-------------------------------------------------------------------------
 	
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	RPacketService::TMbmsServiceAvailabilityV1 param2;
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		param2.iTmgi.SetServiceId(i); 
	    param2.iTmgi.SetMCC(i);
	    param2.iTmgi.SetMNC(i);
	    param2.iMbmsServiceMode = KBroadcast;
	    param2.iMbmsAccessBearer = E2GBearer;
		multipleEntries->AddEntryL( param2 );
		}
	
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries);	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*multipleEntries, *list) );
	CleanupStack::PopAndDestroy( list );
   
 
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketService::UpdateMbmsMonitorServiceListL
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);	

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RPacketService::UpdateMbmsMonitorServiceListL
 	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Test removing one entry 
 	//-------------------------------------------------------------------------

	oneEntry->DeleteEntryL(0);
	oneEntry->AddEntryL(multipleEntries->GetEntryL(1));
	expData.iActionType = ERemoveEntries;
	expData.iServiceList = multipleEntries; // existing list
	expData.iListOfChanges = oneEntry; // list to add
    data.Close();
    expExpect1.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, ERemoveEntries, oneEntry);	
   	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	multipleEntries->DeleteEntryL(1);
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*multipleEntries, *list) );
	CleanupStack::PopAndDestroy( list );

	//-------------------------------------------------------------------------
	// Test adding entries again
 	//-------------------------------------------------------------------------
    CPcktMbmsMonitoredServiceList* multipleEntries2 = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(multipleEntries2);
	for( TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		param2.iTmgi.SetServiceId(i+10); 
	    param2.iTmgi.SetMCC(i+10);
	    param2.iTmgi.SetMNC(i+10);
	    param2.iMbmsServiceMode = KBroadcast;
	    param2.iMbmsAccessBearer = E2GBearer;
		multipleEntries2->AddEntryL( param2 );
		}

	expData.iActionType = EAddEntries;
	expData.iServiceList = multipleEntries; // existing list
	expData.iListOfChanges = multipleEntries2; // list to add
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	
	
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries2);	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	for(TInt i = 0; i < multipleEntries2->Enumerate(); ++i)
		{
		multipleEntries->AddEntryL(multipleEntries2->GetEntryL(i));
		}
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*multipleEntries, *list) );
	CleanupStack::PopAndDestroy( list );

	//-------------------------------------------------------------------------
	// Test try adding more entries - one fails, other success
 	//-------------------------------------------------------------------------
	while(multipleEntries2->Enumerate() > 0)
		{
		multipleEntries2->DeleteEntryL(0);
		}
	for( TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		param2.iTmgi.SetServiceId(i+20); 
	    param2.iTmgi.SetMCC(i+20);
	    param2.iTmgi.SetMNC(i+20);
	    param2.iMbmsServiceMode = KBroadcast;
	    param2.iMbmsAccessBearer = E2GBearer;
		multipleEntries2->AddEntryL( param2 );
		}

	expData.iActionType = EAddEntries;
	expData.iServiceList = multipleEntries; // existing list
	expData.iListOfChanges = multipleEntries2; // list to add
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries2 );
	while(multipleEntries2->Enumerate()>1)
		{
		multipleEntries->AddEntryL(multipleEntries2->GetEntryL(0));
		multipleEntries2->DeleteEntryL(0);
		}
	TMockLtsyData1<CPcktMbmsMonitoredServiceList *> compData(multipleEntries2);
    data.Close();
    compData.SerialiseL(data);
    iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrMbmsImpreciseServiceEntries, data);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrMbmsImpreciseServiceEntries, reqStatus.Int() );
	
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*multipleEntries, *list) );
	CleanupStack::PopAndDestroy( list );

	//-------------------------------------------------------------------------
	// Test add entries which already exists
 	//-------------------------------------------------------------------------

	while(multipleEntries2->Enumerate()>0)
		{
		multipleEntries2->DeleteEntryL(0);
		}
	while(oneEntry->Enumerate()>0)
		{
		oneEntry->DeleteEntryL(0);
		}
	for( TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		param2.iTmgi.SetServiceId(i+10); 
	    param2.iTmgi.SetMCC(i+10);
	    param2.iTmgi.SetMNC(i+10);
	    param2.iMbmsServiceMode = KBroadcast;
	    param2.iMbmsAccessBearer = E2GBearer;
		multipleEntries2->AddEntryL( param2 );
		param2.iTmgi.SetServiceId(i+110); 
	    param2.iTmgi.SetMCC(i+110);
	    param2.iTmgi.SetMNC(i+110);
	    param2.iMbmsServiceMode = KBroadcast;
	    param2.iMbmsAccessBearer = E2GBearer;
		multipleEntries2->AddEntryL( param2 );
		oneEntry->AddEntryL( param2 );
		}

	expData.iActionType = EAddEntries;
	expData.iServiceList = multipleEntries; // existing list
	expData.iListOfChanges = oneEntry; // list to add
    data.Close();
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	
	
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries2);	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	for(TInt i = 0; i < oneEntry->Enumerate(); ++i)
		{
		multipleEntries->AddEntryL(oneEntry->GetEntryL(i));
		}
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*multipleEntries, *list) );
	CleanupStack::PopAndDestroy( list );
	
	//-------------------------------------------------------------------------
	// Test removing all entries
 	//-------------------------------------------------------------------------

	expData.iActionType = ERemoveAllEntries;
	expData.iServiceList = multipleEntries; // existing list
	expData.iListOfChanges = NULL; // list to add
    data.Close();
    expExpect1.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, ERemoveAllEntries, NULL);	
   	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
	
	
	ASSERT_EQUALS(0, list->Enumerate() );
	CleanupStack::PopAndDestroy( list );
	
		
 	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy( 9, this );
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PUMMSL-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::UpdateMbmsMonitorServiceListL
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::UpdateMbmsMonitorServiceListL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsMonitorServiceList0002L()
	{

	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
		
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

	RBuf8 data;
	CleanupClosePushL(data);
	
		
	CPcktMbmsMonitoredServiceList* entryList = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(entryList);
	RPacketService::TMbmsServiceAvailabilityV1 param;
	param.iTmgi.SetServiceId(0); 
    entryList->AddEntryL( param );
	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);

	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = entryList; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    data.Close();
    expExpect1.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::UpdateMbmsMonitorServiceListL
 	//-------------------------------------------------------------------------
 	
   // Expected data from CTSY
 	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	
	TRequestStatus reqStatus;
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, entryList);	
	packetService.CancelAsyncRequest(EPacketUpdateMbmsMonitorServiceList);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(5);

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-PUMMSL-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::UpdateMbmsMonitorServiceListL with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::UpdateMbmsMonitorServiceListL with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsMonitorServiceList0003L()
	{	
	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
		
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

	RBuf8 data;
	CleanupClosePushL(data);
	
	// check maximum monitored service value during TSY startup.
	TInt maxvalue = 0;
 	TMockLtsyData1<TInt> ltsyData(maxvalue);
 	data.Close();
	ltsyData.SerialiseL(data);
 	// data for CompleteL
 	TRequestStatus mockLtsyStatus;
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
		
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketService::UpdateMbmsMonitorServiceListL
 	//-------------------------------------------------------------------------
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
       
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);

	// create maximum number of entries plus 1
	for(TInt i=0; i < MBMS_MONITORSERVICECOUNTMULTIPLE + 1 ;i++)
		{
		existingParams.iTmgi.SetServiceId(i); 
	    existingParams.iTmgi.SetMCC(i);
	    existingParams.iTmgi.SetMNC(i);
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( existingParams );
		}

	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);
	
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> params(expData);
    data.Close();
    params.SerialiseL(data);
	
 	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, data);

	TRequestStatus reqStatus;
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrOverflow, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // zeroEntry, multipleEntries, data, packetService, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PUMMSL-0005
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::UpdateMbmsMonitorServiceListL with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::UpdateMbmsMonitorServiceListL and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsMonitorServiceList0005L()
	{

	OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();
		
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketService::UpdateMbmsMonitorServiceListL
 	//-------------------------------------------------------------------------
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
       
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	
	for(TInt i=0; i < MBMS_MONITORSERVICECOUNTMULTIPLE + 1 ;i++)
		{
		existingParams.iTmgi.SetServiceId(i); 
	    existingParams.iTmgi.SetMCC(i);
	    existingParams.iTmgi.SetMNC(i);
	    existingParams.iMbmsServiceMode = KBroadcast;
	    existingParams.iMbmsAccessBearer = E2GBearer;
	   	existingParams.iMbmsAvailabilityStatus = EMbmsUnavailable;
		multipleEntries->AddEntryL( existingParams );
		}

	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);
	
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> params(expData);
    data.Close();
    params.SerialiseL(data);
    
 	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
 	
	TRequestStatus reqStatus;
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // zeroEntry, multipleEntries, data, packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTS-RPMMSL-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrievePcktMbmsMonitoredServices::Start 
@SYMTestPriority High
@SYMTestActions Invokes CRetrievePcktMbmsMonitoredServices::Start 
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);


	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrievePcktMbmsMonitoredServices::Start
 	//-------------------------------------------------------------------------
	
	// Set the list
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = CPcktMbmsMonitoredServiceList::NewL(); // existing list
	CleanupStack::PushL( expData.iServiceList );
	expData.iListOfChanges = CPcktMbmsMonitoredServiceList::NewL(); // list to add
	CleanupStack::PushL( expData.iListOfChanges );
    RPacketService::TMbmsServiceAvailabilityV1 entry;
    entry.iTmgi.SetServiceId(15);
    entry.iTmgi.SetMCC(15);
    entry.iTmgi.SetMNC(6);
    entry.iMbmsServiceMode = KBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    expData.iListOfChanges->AddEntryL(entry);
    entry.iTmgi.SetServiceId(5);
    entry.iTmgi.SetMCC(12);
    entry.iTmgi.SetMNC(7);
    entry.iMbmsServiceMode = KSelectedBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E2GBearer;
    expData.iListOfChanges->AddEntryL(entry);
    entry.iTmgi.SetServiceId(8);
    entry.iTmgi.SetMCC(115);
    entry.iTmgi.SetMNC(36);
    entry.iMbmsServiceMode = KBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    expData.iListOfChanges->AddEntryL(entry);
    entry.iTmgi.SetServiceId(115);
    entry.iTmgi.SetMCC(1);
    entry.iTmgi.SetMNC(46);
    entry.iMbmsServiceMode = KSelectedBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    expData.iListOfChanges->AddEntryL(entry);

    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	TRequestStatus reqStatus;
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, expData.iListOfChanges );	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
	// Reading the list
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	
	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL( list );
		
	ASSERT_TRUE( TComparator<CPcktMbmsMonitoredServiceList>::IsEqual(*expData.iListOfChanges, *list) );


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}


/**
@SYMTestCaseID BA-CTSY-PKTS-RPMMSL-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrievePcktMbmsMonitoredServices::Start 
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrievePcktMbmsMonitoredServices::Start 
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestStart0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

	RBuf8 data;
	CleanupClosePushL(data);

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);

	//-------------------------------------------------------------------------
	// Test cancelling of CRetrievePcktMbmsMonitoredServices::Start
 	//-------------------------------------------------------------------------
 	
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());
	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Test cancelling of CRetrievePcktMbmsMonitoredServices::Start
	// this time cancel on 2nd phase
 	//-------------------------------------------------------------------------

	
	TRequestStatus reqStatus;
    RPacketService::TMbmsServiceAvailabilityV1 existingParams;
    
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList::NewL();
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
	
	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    data.Close();
    expExpect1.SerialiseL(data);
    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, multipleEntries);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );

	monitorList->Start(activeRetriever->iStatus);
	activeRetriever->Activate(CActiveRetriever::ECasePhase2Cancel);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->iStatus.Int());
	
	AssertMockLtsyStatusL();
	
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(7, this); 

	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PEMMSL-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::EnumerateMbmsMonitorServiceList
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateMbmsMonitorServiceList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateMbmsMonitorServiceList0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup,this ) );
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL( data );

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService ); 
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
 	TInt count;
 	TInt maxAllowed ; 	
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::EnumerateMbmsMonitorServiceList during startup.
 	//-------------------------------------------------------------------------

 	TInt maxvalue = 40;
 	// set maximum monitored service value during TSY startup.
 	TMockLtsyData1<TInt> ltsyData(maxvalue);
 	data.Close();
	ltsyData.SerialiseL(data);
 	// data for CompleteL
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketService::EnumerateMbmsMonitorServiceList when List is empty.
 	//-------------------------------------------------------------------------
 	
    // send request
	packetService.EnumerateMbmsMonitorServiceList(requestStatus, count, maxAllowed);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
 	ASSERT_EQUALS( KErrNone, requestStatus.Int() ); 
  	
  	ASSERT_EQUALS( maxvalue, maxAllowed );  	
  	ASSERT_EQUALS( 0, count );
  	
   	//-------------------------------------------------------------------------
	// TEST D: Successful completion request of
	// RPacketService::EnumerateMbmsMonitorServiceList when List is not empty.
 	//-------------------------------------------------------------------------
 
	// Set the list
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = CPcktMbmsMonitoredServiceList::NewL(); // existing list
	CleanupStack::PushL( expData.iServiceList );
	expData.iListOfChanges = CPcktMbmsMonitoredServiceList::NewL(); // list to add
	CleanupStack::PushL( expData.iListOfChanges );
    RPacketService::TMbmsServiceAvailabilityV1 entry;
    entry.iTmgi.SetServiceId(15);
    entry.iTmgi.SetMCC(15);
    entry.iTmgi.SetMNC(6);
    entry.iMbmsServiceMode = KBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    expData.iListOfChanges->AddEntryL(entry);
    entry.iTmgi.SetServiceId(5);
    entry.iTmgi.SetMCC(12);
    entry.iTmgi.SetMNC(7);
    entry.iMbmsServiceMode = KSelectedBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E2GBearer;
    expData.iListOfChanges->AddEntryL(entry);
    entry.iTmgi.SetServiceId(8);
    entry.iTmgi.SetMCC(115);
    entry.iTmgi.SetMNC(36);
    entry.iMbmsServiceMode = KBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    expData.iListOfChanges->AddEntryL(entry);
    entry.iTmgi.SetServiceId(115);
    entry.iTmgi.SetMCC(1);
    entry.iTmgi.SetMNC(46);
    entry.iMbmsServiceMode = KSelectedBroadcast;
    entry.iMbmsAvailabilityStatus = EMbmsAvailabilityUnknown;
    entry.iMbmsAccessBearer = E3GBearer;
    expData.iListOfChanges->AddEntryL(entry);

    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    expExpect1.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	TRequestStatus reqStatus;
	packetService.UpdateMbmsMonitorServiceListL(reqStatus, EAddEntries, expData.iListOfChanges );	
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS( KErrNone, reqStatus.Int() );
	
		
	// send EnumerateMbmsMonitorServiceList request
	packetService.EnumerateMbmsMonitorServiceList(requestStatus, count, maxvalue);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
 	ASSERT_EQUALS( KErrNone, requestStatus.Int() ); 
  	
  	ASSERT_EQUALS( maxAllowed, maxvalue );  	
  	ASSERT_EQUALS( expData.iListOfChanges->Enumerate(), count);
  		
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 5, this ); 	
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PEMMSL-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::EnumerateMbmsMonitorServiceList
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::EnumerateMbmsMonitorServiceList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateMbmsMonitorServiceList0002L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL(TCleanupItem( Cleanup,this ) );
	OpenPhoneL();

	RPacketService packetService;		
	// Open PacketService
	OpenPacketServiceL( packetService );
	CleanupClosePushL( packetService ); 
	
	TRequestStatus requestStatus;
 	TInt count;
 	TInt maxAllowed ; 	
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::EnumerateMbmsMonitorServiceList
 	//-------------------------------------------------------------------------

    // send request
	packetService.EnumerateMbmsMonitorServiceList(requestStatus, count, maxAllowed);
	packetService.CancelAsyncRequest(EPacketEnumerateMbmsMonitorServiceList);
    User::WaitForRequest(requestStatus);
 	ASSERT_TRUE(( KErrNone == requestStatus.Int()) || (KErrCancel == requestStatus.Int()) ); 
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( 2, this ); 	
    }


/**
@SYMTestCaseID BA-CTSY-PKTS-PEMMSL-0004
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketService::EnumerateMbmsMonitorServiceList
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketService::EnumerateMbmsMonitorServiceList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateMbmsMonitorServiceList0004L()
    {
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);
	
	RBuf8 data;
	CleanupClosePushL( data );

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;	
	TRequestStatus mockLtsyStatus;

 	TInt maxAllowed = 40; 
	TInt maxvalue = 40;

 	// check maximum monitored service value during TSY startup.
 	TMockLtsyData1<TInt> ltsyData(maxvalue);
 	data.Close();
	ltsyData.SerialiseL(data);
 	// data for CompleteL
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketEnumerateMbmsMonitorServiceList, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		
	ASSERT_EQUALS( maxvalue, maxAllowed );
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketService::EnumerateMbmsMonitorServiceList
 	//-------------------------------------------------------------------------
 	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);
	
	//List retriever
	CRetrievePcktMbmsMonitoredServices* monitorList = CRetrievePcktMbmsMonitoredServices::NewL(packetService);	 
	CleanupStack::PushL(monitorList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*monitorList);
	CleanupStack::PushL(activeRetriever);
	scheduler.AddRetrieverL(*activeRetriever);
    
	RBuf8 completeData;
	CleanupClosePushL(completeData);

    RPacketService::TMbmsServiceAvailabilityV1 param2;
    
    CPcktMbmsMonitoredServiceList* multipleEntries = CPcktMbmsMonitoredServiceList ::NewL();
	CleanupStack::PushL(multipleEntries);
	
	for(TInt i=0;i < MBMS_MONITORSERVICECOUNTMULTIPLE ;i++)
		{
		param2.iTmgi.SetServiceId(i); 
	    param2.iTmgi.SetMCC(i);
	    param2.iTmgi.SetMNC(i);
	    param2.iMbmsServiceMode = KBroadcast;
	    param2.iMbmsAccessBearer = E2GBearer;
		multipleEntries->AddEntryL(param2);
		}
    
	CPcktMbmsMonitoredServiceList* zeroEntry = CPcktMbmsMonitoredServiceList::NewL();
	CleanupStack::PushL(zeroEntry);
	
	TMmMbmsMonitorServiceListParams expData;
	expData.iActionType = EAddEntries;
	expData.iServiceList = zeroEntry; // existing list
	expData.iListOfChanges = multipleEntries; // list to add
    TMockLtsyData1<TMmMbmsMonitorServiceListParams> expExpect1(expData);
    data.Close();
    expExpect1.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketUpdateMbmsMonitorServiceList, data);
	iMockLTSY.CompleteL(EPacketUpdateMbmsMonitorServiceList, KErrNone, completeData);
	
	packetService.UpdateMbmsMonitorServiceListL(requestStatus, EAddEntries, multipleEntries);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	monitorList->Start(activeRetriever->Status());
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());

	CPcktMbmsMonitoredServiceList* list = monitorList->RetrieveListL();
	CleanupStack::PushL(list);
	
	ASSERT_EQUALS(MBMS_MONITORSERVICECOUNTMULTIPLE, list->Enumerate());
	
	maxvalue = 40;
	TInt maxvalue2 = 40;
	TInt count1, count2;
	
	// send EnumerateMbmsMonitorServiceList request
	packetService.EnumerateMbmsMonitorServiceList(requestStatus, count1, maxvalue);
	
	// send EnumerateMbmsMonitorServiceList request
	packetService2.EnumerateMbmsMonitorServiceList(requestStatus2, count2, maxvalue2);
	
	// wait for first answer
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
 	ASSERT_EQUALS(KErrNone, requestStatus.Int() ); 
  	ASSERT_EQUALS(maxvalue, maxAllowed );  	
  	ASSERT_EQUALS(count1, MBMS_MONITORSERVICECOUNTMULTIPLE);
  	
  	// wait for second answer
    User::WaitForRequest(requestStatus2);

 	ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 
  	ASSERT_EQUALS(maxvalue2, maxAllowed);  	
  	ASSERT_EQUALS(count2, MBMS_MONITORSERVICECOUNTMULTIPLE);		
  	
	CleanupStack::PopAndDestroy( 12, this ); // list zeroEntry multipleEntries completeData
	// activeRetriever monitorList data packetService2 packetService phone2 telServer2 this
	}

/**
@SYMTestCaseID BA-CTSY-PKTS-PEMASL-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketService::EnumerateMbmsActiveServiceList
@SYMTestPriority High
@SYMTestActions Invokes RPacketService::EnumerateMbmsActiveServiceList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateMbmsActiveServiceList0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService,
                        RPacketService::EStatusUnattached,
                        RPacketService::EAttachWhenNeeded, 123 );
    CleanupClosePushL( packetService );
        
	RBuf8 data;
	CleanupClosePushL( data );    
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );  
	
	// TEST initialise
	
	// Open new context
	TInfoName contextId1;
	TInfoName contextId2;	
	RPacketMbmsContext mbmsContext1;	
	RPacketMbmsContext mbmsContext2;	
	mbmsContext1.OpenNewContext( packetService, contextId1 );	 
	mbmsContext2.OpenNewContext( packetService, contextId2 );	 
	CleanupClosePushL( mbmsContext1 );	
	CleanupClosePushL( mbmsContext2 );	
		                    
	// initialisation of the both context 
	TInt status = InitMbmsContextL(mbmsContext1, contextId1);
	ASSERT_EQUALS(KErrNone, status);
    status = InitMbmsContextL(mbmsContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
	
	TInt count(0);
	TInt maxAllowed(0);
	
    TRequestStatus requestStatus;
    
 	// data to and from LTSY
 	TMockLtsyData2 < TInt, TInt > expData( count, maxAllowed );
	expData.SerialiseL( expectData );
	
 	TMockLtsyData2 < TInt, TInt > outData( count, maxAllowed );
	outData.SerialiseL( data );	
	
    //-------------------------------------------------------------------------
 	// TEST C, part a; no active and/or receiving contexts
 	//-------------------------------------------------------------------------	 
       	
    packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 
    		
    User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(0, count);
	ASSERT_EQUALS(123, maxAllowed);
	
    //-------------------------------------------------------------------------
 	// TEST C, part b; active both contexts
 	//------------------------------------------------------------------------- 		
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig1;
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig2;
		
	// activate context 
	status = SetMbmsActiveL(mbmsContext1, contextConfig1, contextId1);	 
	ASSERT_EQUALS(KErrNone, status); 
	
	status = SetMbmsActiveL(mbmsContext2, contextConfig2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
            
    packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 
       
    User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(2, count);        
	ASSERT_EQUALS(123, maxAllowed);
    //-------------------------------------------------------------------------
 	// TEST C, part a; deactivate one context
 	//-------------------------------------------------------------------------     	
	status = SetMbmsDeactiveL(mbmsContext2, contextId2);
    ASSERT_EQUALS(KErrNone, status);
       
    packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed);  
    User::WaitForRequest( requestStatus );
      			
	ASSERT_EQUALS(1, count);  	        	   
	ASSERT_EQUALS(123, maxAllowed);
 	ASSERT_EQUALS(KErrNone, requestStatus.Int() ); 	 
 	
    CleanupStack::PopAndDestroy(6); // data, this    
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PEMASL-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketService::EnumerateMbmsActiveServiceList
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketService::EnumerateMbmsActiveServiceList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestEnumerateMbmsActiveServiceList0002L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService,
                        RPacketService::EStatusUnattached,
                        RPacketService::EAttachWhenNeeded, 123 );
    CleanupClosePushL( packetService );
        

	//-------------------------------------------------------------------------
	// Test cancelling of RPacketService::EnumerateMbmsActiveServiceList
 	//-------------------------------------------------------------------------
 	TInt count, maxAllowed;
 	TRequestStatus requestStatus;
    packetService.EnumerateMbmsActiveServiceList(requestStatus, count, maxAllowed); 

    packetService.CancelAsyncRequest(EPacketEnumerateMbmsActiveServiceList);
		
    User::WaitForRequest(requestStatus);
	ASSERT_TRUE((KErrCancel == requestStatus.Int()) || ( KErrNone == requestStatus.Int()));

	CleanupStack::PopAndDestroy(2); // packetService, this
	
	}
    

/**
@SYMTestCaseID BA-CTSY-MCF-PUMSL-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsSessionList0001L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
        
	RBuf8 data;
	CleanupClosePushL( data );    
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );  
	
	// TEST initialise
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;	
	mbmsContext.OpenNewContext( packetService, contextId );	 
	CleanupClosePushL( mbmsContext );	
		                    
	// initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	// activate context 
	status = SetMbmsActiveL(mbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	TInt maxSessions( 5 );		
    TRequestStatus requestStatus;
 
    RPacketMbmsContext::CMbmsSession* aSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession);
      
 	// data to and from LTSY
    TMmMbmsActiveServiceListParams params;
 
 	TMockLtsyData1 <TInfoName> outData( contextId );
	outData.SerialiseL( data );	
	
    //-------------------------------------------------------------------------
 	// initialise session list
 	//-------------------------------------------------------------------------
    
    for( TInt i = 0; i < maxSessions; i++ )
        {
        TMbmsSessionId session(i);  
     
        params.iActionType = EAddEntries;
        params.iServiceList = aSession;
        params.iChangedItem = session;
        
     	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams > expData( contextId, params );
    	expData.SerialiseL( expectData );
       
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
        mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
        
        User::WaitForRequest( requestStatus );
        TInt ret = requestStatus.Int();
        ASSERT_EQUALS( KErrNone, ret ) ;
        }
        
    //-------------------------------------------------------------------------
 	// create retrievers
 	//-------------------------------------------------------------------------        
                            
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

	TInt rstatus = retriever.iStatus.Int();	 
	ASSERT_EQUALS(KErrNone, rstatus);
	
    //-------------------------------------------------------------------------
 	// TEST C, part a; check existing items
 	//-------------------------------------------------------------------------	
	
	for( TUint i = 0; i < session->iSessionIdList.Count(); i++)
	    {
	    TMbmsSessionId sessionId = session->iSessionIdList[i];
	    ASSERT_EQUALS( sessionId, i);	    
	    }
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());	 	
 	AssertMockLtsyStatusL();
	
    //-------------------------------------------------------------------------
 	// TEST C, part b; remove existing items
 	//-------------------------------------------------------------------------
    
    for( TInt i = maxSessions-1; i >= 0; i-- )
        {
        TMbmsSessionId session(i);  
 
   		params.iActionType = ERemoveEntries;
        params.iServiceList = aSession;
        params.iChangedItem = session;
       
        TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams > expData( contextId, params );
   	   	expData.SerialiseL( expectData );
              
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
        mbmsContext.UpdateMbmsSessionList(requestStatus, ERemoveEntries, session);    
        
        User::WaitForRequest( requestStatus );
        TInt ret = requestStatus.Int();
        ASSERT_EQUALS( KErrNone, ret ) ;
        } 	
        
 	sessionList->Start( retriever.Status() );
	retriever.Activate();
	scheduler.StartScheduler();

	// check status for each retriever
	rstatus = retriever.iStatus.Int();	 
	ASSERT_EQUALS(KErrNotFound, rstatus);

	
    //-------------------------------------------------------------------------
 	// TEST C, part c; add new items again
 	//-------------------------------------------------------------------------
    
    for( TInt i = 0; i < maxSessions; i++ )
        {
        TMbmsSessionId session(i);  
        
        params.iActionType = EAddEntries;
        params.iServiceList = aSession;
        params.iChangedItem = session;
       
        TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams > expData( contextId, params );
        expData.SerialiseL( expectData );
      
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
        mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
        
        User::WaitForRequest( requestStatus );
        
        AssertMockLtsyStatusL();
        TInt ret = requestStatus.Int();
        ASSERT_EQUALS( KErrNone, ret ) ;
        }
        
    // verify that count is maxSessions        
 	sessionList->Start( retriever.Status() );
	retriever.Activate();
	scheduler.StartScheduler();
			
	// there should be a max number of sessions now
	TInt maxCount = session->iSessionIdList.Count();
	ASSERT_EQUALS( maxSessions, maxCount );         
        
    //-------------------------------------------------------------------------
 	// TEST C, part d; then remove all 
 	//-------------------------------------------------------------------------        
    
	params.iActionType = ERemoveAllEntries;
	params.iServiceList = aSession;
	params.iChangedItem = 0;
	  
	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams > expData( contextId, params );
	expData.SerialiseL( expectData );
	  
    // then remove all        
    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
    mbmsContext.UpdateMbmsSessionList(requestStatus, ERemoveAllEntries, 0);    
    
    User::WaitForRequest( requestStatus );
    TInt ret = requestStatus.Int();
    ASSERT_EQUALS( KErrNone, ret ) ;  
    
 	sessionList->Start( retriever.Status() );
	retriever.Activate();
	scheduler.StartScheduler();
	
	// check status for each retriever
	rstatus = retriever.iStatus.Int();	 
	ASSERT_EQUALS(KErrNotFound, rstatus);
			
    // Done !  
    CActiveScheduler::Install(NULL);      
	CleanupStack::PopAndDestroy(8); //  session, sessionList, retriever     
    } 	

/**
@SYMTestCaseID BA-CTSY-MCF-PUMSL-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsSessionList0002L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
        
	RBuf8 data;
	CleanupClosePushL( data );    
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );  
	
	// TEST initialise
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;	
	mbmsContext.OpenNewContext( packetService, contextId );	 
	CleanupClosePushL( mbmsContext );	
		                    
	// initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	contextConfig.iMbmsSessionFlag = ETrue;
	// activate context 
	status = SetMbmsActiveL(mbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	TInt maxSessions( 5 );		
    TRequestStatus requestStatus;
    
    RPacketMbmsContext::CMbmsSession* aSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession);
         
    	// data to and from LTSY
    TMmMbmsActiveServiceListParams params;
    
    TMockLtsyData1 <TInfoName> outData( contextId );
   	outData.SerialiseL( data );	
   	
    //-------------------------------------------------------------------------
	// initialise session list
	//-------------------------------------------------------------------------
   
   for( TInt i = 0; i < maxSessions; i++ )
       {
       TMbmsSessionId session(i);  
    
       params.iActionType = EAddEntries;
       params.iServiceList = aSession;
       params.iChangedItem = session;
       
       TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams > expData( contextId, params );
   	   expData.SerialiseL( expectData );
      
       iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
       iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
       mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);
       // send canceling...
       mbmsContext.CancelAsyncRequest(EPacketContextUpdateMbmsSessionList);
       
       User::WaitForRequest( requestStatus );
       TInt ret = requestStatus.Int();
       ASSERT_TRUE( ret== KErrCancel || ret == KErrNone ) ;
       }
        
    //-------------------------------------------------------------------------
 	// create retrievers
 	//-------------------------------------------------------------------------        
                            
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* session = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(session);	
    	
	CRetrievePcktMbmsSessionList* sessionList = CRetrievePcktMbmsSessionList::NewL(mbmsContext, *session);        
    CleanupStack::PushL(sessionList);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever(*sessionList);
    
    scheduler.AddRetrieverL(retriever);		
    //-------------------------------------------------------------------------
 	// TEST C, part a; check existing items
 	//-------------------------------------------------------------------------	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.ExpectL(EPacketGetMbmsSessionListCancel, KErrNone);
    iMockLTSY.ExpectL(EPacketGetMbmsSessionListPhase1, KErrNone);
    iMockLTSY.CompleteL(EPacketGetMbmsSessionListCancel, KErrNone);

    sessionList->Start( retriever.Status() );	 	
	retriever.Activate(CActiveRetriever::ECaseGeneralCancelCase);
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, retriever.iStatus.Int());
 	
    // Done !  
    CActiveScheduler::Install(NULL);      
	CleanupStack::PopAndDestroy(8); //  session, sessionList, retriever     
    } 
    	

/**
@SYMTestCaseID BA-CTSY-MCF-PUMSL-0003
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketMbmsContext::UpdateMbmsSessionList
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsSessionList0003L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService,
                        RPacketService::EStatusUnattached,
                        RPacketService::EAttachWhenNeeded, 
                        KMaxNumberMbmsContexts );
    CleanupClosePushL( packetService );
        
    // general buffers 
	RBuf8 data, data1, data2, data3;
	CleanupClosePushL( data ); 	
	CleanupClosePushL( data1 );    
	CleanupClosePushL( data2 );    
	CleanupClosePushL( data3 );   
	
	RBuf8 expectData, expectData1, expectData2, expectData3;
	CleanupClosePushL( expectData ); 
	CleanupClosePushL( expectData1 );  
	CleanupClosePushL( expectData2 );  
	CleanupClosePushL( expectData3 );
	
	// TEST initialise
	
	// Opens and initiliases three contexts;
	// context 1; contains sessions between idStart1 - idEnd1
	// context 2; contains sessions between idStart2 - idEnd2
	// context 3; cannot have any services
	
	TInfoName contextId1, contextId2, contextId3;	
	RPacketMbmsContext mbmsContext1, mbmsContext2, mbmsContext3;	
	
	mbmsContext1.OpenNewContext( packetService, contextId1 );	 
	mbmsContext2.OpenNewContext( packetService, contextId2 );
	mbmsContext3.OpenNewContext( packetService, contextId3 );
	CleanupClosePushL( mbmsContext1 );	
	CleanupClosePushL( mbmsContext2 );	
	CleanupClosePushL( mbmsContext3 );	
		                    
	// initialisation of the contexts
	TInt status = InitMbmsContextL(mbmsContext1, contextId1);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 
	status = InitMbmsContextL(mbmsContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 
	status = InitMbmsContextL(mbmsContext3, contextId3);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig1, contextConfig2, contextConfig3;	
	contextConfig1.iMbmsSessionFlag = ETrue;
	contextConfig2.iMbmsSessionFlag = ETrue;
	contextConfig3.iMbmsSessionFlag = EFalse;
	
    // configure contexts			
	status = SetMbmsConfigL(mbmsContext1, contextId1, contextConfig1);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 			
	status = SetMbmsConfigL(mbmsContext2, contextId2, contextConfig2);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 		
	status = SetMbmsConfigL(mbmsContext3, contextId3, contextConfig3);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 
	
	// activate contexts 	
	status = SetMbmsActiveL(mbmsContext1, contextConfig1, contextId1);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 
	status = SetMbmsActiveL(mbmsContext2, contextConfig2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 
	status = SetMbmsActiveL(mbmsContext3, contextConfig3, contextId3);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL(); 	
			
	TMbmsSessionId set1[KMaxSessionsInSet1], check1[KMaxSessionsInSet1];
	TMbmsSessionId set2[KMaxSessionsInSet2], check2[KMaxSessionsInSet2];
	TMbmsSessionId set3[KMaxSessionsInSet3];
	TInt tmp = 0;
	for( TInt i = 0; i < KMaxSessionsInSet1; i++ )
	    {
	    set1[i] = check1[i] = tmp;
	    tmp++;
	    }
	tmp = 0;	    
	for( TInt i = 0; i < KMaxSessionsInSet2; i++ )
	    {
	    set2[i] = check2[i] = tmp;
	    tmp++;
	    }
	tmp = 0;
	for( TInt i = 0; i < KMaxSessionsInSet3; i++ )
	    {
	    set3[i] = tmp;
	    tmp++;
	    }	    	    
    TRequestStatus requestStatus;

 	TMockLtsyData1 <TInfoName> outData1( contextId1 );
 	TMockLtsyData1 <TInfoName> outData2( contextId2 );
 	TMockLtsyData1 <TInfoName> outData3( contextId3 );
	outData1.SerialiseL( data1 );	
	outData2.SerialiseL( data2 );	
	outData3.SerialiseL( data3 );	
	
    //-------------------------------------------------------------------------
 	// initialise session list
 	//-------------------------------------------------------------------------
    RPacketMbmsContext::CMbmsSession* mbmsSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(mbmsSession);
    
    TMmMbmsActiveServiceListParams params;
    params.iActionType = EAddEntries;
 	params.iServiceList = mbmsSession;
 	
    for( TMbmsSessionId i = 0; i < KMaxSessionsInSet1; i++ )
        {
     	params.iChangedItem = set1[i];
     	
     	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams> expData1( contextId1, params );
     	expData1.SerialiseL(expectData1); 
    	
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData1);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data1);            
        mbmsContext1.UpdateMbmsSessionList(requestStatus, EAddEntries, set1[i]);    
        
        User::WaitForRequest( requestStatus );
        ASSERT_EQUALS( KErrNone, requestStatus.Int() ) ;   
        }
    
    for( TMbmsSessionId i = 0; i < KMaxSessionsInSet2; i++ )
        {
     	params.iChangedItem = set2[i];
     	
     	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams> expData2( contextId2, params );
     	expData2.SerialiseL(expectData2); 
     	
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData2);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data2);            
        mbmsContext2.UpdateMbmsSessionList(requestStatus, EAddEntries, set2[i]);    
        
        User::WaitForRequest( requestStatus );
        ASSERT_EQUALS( KErrNone, requestStatus.Int() ) ;
        }
        
    for( TMbmsSessionId i = 0; i < KMaxSessionsInSet3; i++ )
        {
     	params.iChangedItem = set3[i];
     	
     	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams> expData3( contextId3, params );
     	expData3.SerialiseL(expectData3); 
     	
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNotSupported/*, data3*/);            
        mbmsContext3.UpdateMbmsSessionList(requestStatus, EAddEntries, set3[i]);    
        
        User::WaitForRequest( requestStatus );        
        ASSERT_EQUALS( KErrNotSupported, requestStatus.Int() ) ;
        }        
                
    //-------------------------------------------------------------------------
 	// create retrievers
 	//-------------------------------------------------------------------------        
                            
	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);    
	
    RPacketMbmsContext::CMbmsSession* aSession1 = RPacketMbmsContext::CMbmsSession::NewL();  
    RPacketMbmsContext::CMbmsSession* aSession2 = RPacketMbmsContext::CMbmsSession::NewL();  
    RPacketMbmsContext::CMbmsSession* aSession3 = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(aSession1);	
    CleanupStack::PushL(aSession2);
    CleanupStack::PushL(aSession3);
    	
	CRetrievePcktMbmsSessionList* aSessionlist1 = CRetrievePcktMbmsSessionList::NewL(mbmsContext1, *aSession1);        
	CRetrievePcktMbmsSessionList* aSessionlist2 = CRetrievePcktMbmsSessionList::NewL(mbmsContext2, *aSession2);
	CRetrievePcktMbmsSessionList* aSessionlist3 = CRetrievePcktMbmsSessionList::NewL(mbmsContext3, *aSession3);
    CleanupStack::PushL(aSessionlist1);
    CleanupStack::PushL(aSessionlist2);
    CleanupStack::PushL(aSessionlist3);

    CActiveRetriever::ResetRequestsNumber();
    CActiveRetriever retriever1(*aSessionlist1);
    CActiveRetriever retriever2(*aSessionlist2);
    CActiveRetriever retriever3(*aSessionlist3);
    
    scheduler.AddRetrieverL(retriever1);
    scheduler.AddRetrieverL(retriever2);
    scheduler.AddRetrieverL(retriever3);
 
    aSessionlist1->Start( retriever1.Status() );	 	
	retriever1.Activate();
	scheduler.StartScheduler();

    aSessionlist2->Start( retriever2.Status() );	 	
	retriever2.Activate();
	scheduler.StartScheduler();
	
    aSessionlist3->Start( retriever3.Status() );	 	
	retriever3.Activate();
	scheduler.StartScheduler();	
	
	// check status for each retriever
	TInt rstatus = retriever1.iStatus.Int();	 
	ASSERT_EQUALS(KErrNone, rstatus);
    rstatus = retriever2.iStatus.Int();	 
	ASSERT_EQUALS(KErrNone, rstatus);		
	rstatus = retriever3.iStatus.Int();	 
	ASSERT_EQUALS(KErrNotFound, rstatus);

    //-------------------------------------------------------------------------
 	// TEST C, part a; check existing items for each contexts
 	//-------------------------------------------------------------------------	
    TBool passed(ETrue);
    
	for( TUint i = 0; i < KMaxSessionsInSet1; i++)
	    {
	    passed &= contains(aSession1->iSessionIdList[i], check1);
	    }
	    
	for( TUint i = 0; i < KMaxSessionsInSet2; i++)
	    {
	    passed &= contains(aSession2->iSessionIdList[i], check2);
	    }	    
	    
    // context 3 does not have any sessions
    ASSERT_TRUE(passed);	    
	
    //-------------------------------------------------------------------------
 	// TEST C, part b; remove second items for each contexts
 	//-------------------------------------------------------------------------	  	
    TMbmsSessionId removedID1, removedID2;
    
    // update context 1
    removedID1 = aSession1->iSessionIdList[1]; 

    params.iActionType = ERemoveEntries;
 	params.iServiceList = aSession1;
 	params.iChangedItem = removedID1;
 	
 	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams> expData1( contextId1, params );
 	expectData1.Close();
 	expData1.SerialiseL(expectData1); 
	
    
    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData1);
    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data1);            
    mbmsContext1.UpdateMbmsSessionList(requestStatus, ERemoveEntries, removedID1);    
    
    User::WaitForRequest( requestStatus );   
    ASSERT_EQUALS( KErrNone, requestStatus.Int() );  
    AssertMockLtsyStatusL();  
    
    removedID2 = aSession2->iSessionIdList[1];
 
    params.iActionType = ERemoveEntries;
 	params.iServiceList = aSession2;
 	params.iChangedItem = removedID2;
 	
 	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams> expData2( contextId2, params );
 	expectData2.Close();
 	expData2.SerialiseL(expectData2); 
    
    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData2);
    iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data2);            
    mbmsContext2.UpdateMbmsSessionList(requestStatus, ERemoveEntries, removedID2);    
    
    User::WaitForRequest( requestStatus );    
    ASSERT_EQUALS( KErrNone, requestStatus.Int() );  
    AssertMockLtsyStatusL();      
    
    // calculate total number of session (should be total max -2)
    aSessionlist1->Start( retriever1.Status() );	 	
	retriever1.Activate();
	scheduler.StartScheduler();

    aSessionlist2->Start( retriever2.Status() );	 	
	retriever2.Activate();
	scheduler.StartScheduler();    
    
    TInt diff = KMaxSessionsInSet1 + KMaxSessionsInSet2 - 2;
    diff -= aSession1->iSessionIdList.Count();
    diff -= aSession2->iSessionIdList.Count();
    
    ASSERT_EQUALS(KZero, diff);
    
    // and verify that removed items are not in session lists
	passed = ! contains(removedID1, aSession1->iSessionIdList);	    	    
	passed &= ! contains(removedID2, aSession2->iSessionIdList);	   	    
	    
    // difference should be 0 right now
    ASSERT_TRUE(passed); 
    
    //-------------------------------------------------------------------------
 	// TEST C, part c; test max number of context/services
 	//-------------------------------------------------------------------------	       
    TInt fillRet = KErrNone, newContexts;
    	    	
    newContexts = 0;    	    	
    while( KErrNone == fillRet )
        {
	    TInfoName contextId;	
	    RPacketMbmsContext mbmsContext;	
	    
	
	    fillRet = mbmsContext.OpenNewContext( packetService, contextId );	 
    	CleanupClosePushL( mbmsContext );
    	newContexts++;
    		
        TMockLtsyData1 <TInfoName> outData( contextId );
    	outData.SerialiseL( data );
        
    	if( KErrNone == fillRet )
    	    {
    	    // allow session
	        RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	        contextConfig.iMbmsSessionFlag = ETrue;
	            	    
    	    // init a new context
    	    status = InitMbmsContextL(mbmsContext, contextId);
	        ASSERT_EQUALS(KErrNone, status);
	        AssertMockLtsyStatusL(); 
	        
        	// activate contexts 	
        	status = SetMbmsActiveL(mbmsContext, contextConfig, contextId);
        	if( KErrNone == status )
        	    {        	            	    
            	// insert one service for each context
            	TMbmsSessionId session(1);
            	
                params.iActionType = EAddEntries;
                params.iServiceList = mbmsSession;
                params.iChangedItem = session;
               
                TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams > expData( contextId, params );
                expectData.Close();
                expData.SerialiseL( expectData );
            	
                iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);
                iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, data);            
                mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);  
                  
                User::WaitForRequest( requestStatus );          
                if (KErrNone != requestStatus.Int())
                	{
                	fillRet = requestStatus.Int();	
                	}
        	    }
        	else
        	    {
        	    fillRet = status; 
        	    }
    	    }        
        }
        
    // now active context number should be equals as KMaxNumberMbmsContexts
    TInt fullCount, maxAllowed;
    packetService.EnumerateMbmsActiveServiceList(requestStatus, fullCount, maxAllowed); 
       
    User::WaitForRequest( requestStatus );	
	ASSERT_EQUALS(KErrNone, status);    
	ASSERT_EQUALS(KMaxNumberMbmsContexts, fullCount); 
	ASSERT_EQUALS(KMaxNumberMbmsContexts, maxAllowed); 
    
    CActiveScheduler::Install(NULL);      
	CleanupStack::PopAndDestroy(20 + newContexts); //  session, sessionList, retriever     
    } 
    
/**
@SYMTestCaseID BA-CTSY-MCF-PUMSL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketMbmsContext::UpdateMbmsSessionList with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketMbmsContext::UpdateMbmsSessionList and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestUpdateMbmsSessionList0005L()
    {
    OpenEtelServerL( EUseExtendedError );
	CleanupStack::PushL( TCleanupItem( Cleanup, this ) );
	OpenPhoneL();	
	
	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );
	
	RBuf8 expectData;
	CleanupClosePushL( expectData );  
	
	// Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;	
	mbmsContext.OpenNewContext( packetService, contextId );	 
	CleanupClosePushL( mbmsContext );	
		                    
	
	// initialisation of the context 
	TInt status = InitMbmsContextL(mbmsContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	RPacketMbmsContext::TContextConfigMbmsV1 contextConfig;	
	contextConfig.iMbmsSessionFlag = ETrue;
	
	// activate context 
	status = SetMbmsActiveL(mbmsContext, contextConfig, contextId);
	ASSERT_EQUALS(KErrNone, status);
			
    TRequestStatus requestStatus;

    RPacketMbmsContext::CMbmsSession* mbmsSession = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(mbmsSession);

    TMbmsSessionId session(0);  
     
    TMmMbmsActiveServiceListParams params;
    params.iActionType = EAddEntries;
	params.iServiceList = mbmsSession;
	params.iChangedItem = session;
	
	TMockLtsyData2 <TInfoName, TMmMbmsActiveServiceListParams> expData( contextId, params );
	expData.SerialiseL(expectData); 
    
    iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, expectData);            
    mbmsContext.UpdateMbmsSessionList(requestStatus, EAddEntries, session);    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // mbmsSession, mbmsContext, expactData, packetService, this   
	 
    }

/**
@SYMTestCaseID BA-CTSY-PKTS-PMSLS-0001
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for CRetrievePcktMbmsSessionList::Start
@SYMTestPriority High
@SYMTestActions Invokes CRetrievePcktMbmsSessionList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestRetrievePcktMbmsSessionListStart0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 reqData;
	CleanupClosePushL(reqData);


	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

    // Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;	
	mbmsContext.OpenNewContext( packetService, contextId );	 
	CleanupClosePushL( mbmsContext );	

    RPacketMbmsContext::CMbmsSession* session = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(session);	

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

    CRetrievePcktMbmsSessionList* asyncRetrieveList = CRetrievePcktMbmsSessionList::NewL(mbmsContext, *session);    
	CleanupStack::PushL(asyncRetrieveList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*asyncRetrieveList);
	CleanupStack::PushL(activeRetriever);
	
	scheduler.AddRetrieverL(*activeRetriever);

    // Set a list
    TMbmsSessionIdList sessions;
    CleanupClosePushL( sessions );
    sessions.Append(20);
    sessions.Append(4);
    sessions.Append(13);
    sessions.Append(17);
    sessions.Append(5);
    sessions.Append(1);
    sessions.Append(112);
	TRequestStatus reqStatus;
	RPacketMbmsContext::CMbmsSession *serverSessions = RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(serverSessions);
    for(TInt i = 0; i < sessions.Count(); ++i)
        {
        TMmMbmsActiveServiceListParams expectedData;
        expectedData.iActionType = EAddEntries;
        expectedData.iServiceList = serverSessions;
        expectedData.iChangedItem = sessions[i];
        reqData.Close();
        TMockLtsyData2<TInfoName, TMmMbmsActiveServiceListParams>  expLtsyData(contextId, expectedData);
        expLtsyData.SerialiseL(reqData);
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, reqData);
        mbmsContext.UpdateMbmsSessionList(reqStatus, EAddEntries, sessions[i]);
        TMockLtsyData1<TInfoName>  completeLtsyData(contextId);
        reqData.Close();
        completeLtsyData.SerialiseL(reqData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, reqData);
        User::WaitForRequest( reqStatus );
        ASSERT_EQUALS( KErrNone, reqStatus.Int() ) ;
        serverSessions->iSessionIdList.Append( sessions[i] );
        }
        

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// CRetrievePcktMbmsSessionList::Start
 	//-------------------------------------------------------------------------

	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	scheduler.StartScheduler();

	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrNone, activeRetriever->iStatus.Int());
	//asyncRetrieveList->RestoreListL();
    ASSERT_EQUALS(sessions.Count(), session->iSessionIdList.Count());
    for(TInt i = 0; i < sessions.Count(); ++i)
        {
        ASSERT_EQUALS(sessions[i], session->iSessionIdList[i]);
        }

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(9, this); // this, reqData, packetService, mbmsContext, session, asyncRetrieveList, activeRetriever, sessions, serverSessions
	}
/**
@SYMTestCaseID BA-CTSY-PKTS-PMSLS-0002
@SYMPREQ 1551
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of CRetrievePcktMbmsSessionList::Start
@SYMTestPriority High
@SYMTestActions Invokes cancelling of CRetrievePcktMbmsSessionList::Start
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketServiceFU::TestRetrievePcktMbmsSessionListStart0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 reqData;
	CleanupClosePushL(reqData);


	RPacketService packetService;
    OpenPacketServiceL( packetService );
    CleanupClosePushL( packetService );

    // Open new context
	TInfoName contextId;	
	RPacketMbmsContext mbmsContext;	
	mbmsContext.OpenNewContext( packetService, contextId );	 
	CleanupClosePushL( mbmsContext );	

    RPacketMbmsContext::CMbmsSession* session = RPacketMbmsContext::CMbmsSession::NewL();  
    CleanupStack::PushL(session);	

	CFilteringActiveScheduler scheduler;
	CActiveScheduler::Install(&scheduler);

    CRetrievePcktMbmsSessionList* asyncRetrieveList = CRetrievePcktMbmsSessionList::NewL(mbmsContext, *session);    
	CleanupStack::PushL(asyncRetrieveList);

	CActiveRetriever::ResetRequestsNumber();
	CActiveRetriever* activeRetriever = CActiveRetriever::NewL(*asyncRetrieveList);
	CleanupStack::PushL(activeRetriever);
	
	scheduler.AddRetrieverL(*activeRetriever);

    // Set a list
    TMbmsSessionIdList sessions;
    CleanupClosePushL( sessions );
    sessions.Append(20);
    sessions.Append(4);
    sessions.Append(13);
    sessions.Append(17);
    sessions.Append(5);
    sessions.Append(1);
    sessions.Append(112);
	TRequestStatus reqStatus;
	RPacketMbmsContext::CMbmsSession *serverSessions = RPacketMbmsContext::CMbmsSession::NewL();
	CleanupStack::PushL(serverSessions);
    for(TInt i = 0; i < sessions.Count(); ++i)
        {
        TMmMbmsActiveServiceListParams expectedData;
        expectedData.iActionType = EAddEntries;
        expectedData.iServiceList = serverSessions;
        expectedData.iChangedItem = sessions[i];
        reqData.Close();
        TMockLtsyData2<TInfoName, TMmMbmsActiveServiceListParams>  expLtsyData(contextId, expectedData);
        expLtsyData.SerialiseL(reqData);
        iMockLTSY.ExpectL(EPacketContextUpdateMbmsSessionList, reqData);
        mbmsContext.UpdateMbmsSessionList(reqStatus, EAddEntries, sessions[i]);
        TMockLtsyData1<TInfoName>  completeLtsyData(contextId);
        reqData.Close();
        completeLtsyData.SerialiseL(reqData);
        iMockLTSY.CompleteL(EPacketContextUpdateMbmsSessionList, KErrNone, reqData);
        User::WaitForRequest( reqStatus );
        ASSERT_EQUALS( KErrNone, reqStatus.Int() ) ;
        serverSessions->iSessionIdList.Append( sessions[i] );
        }
        

 	//-------------------------------------------------------------------------
	// Test cancelling of CRetrievePcktMbmsSessionList::Start
 	//-------------------------------------------------------------------------
	asyncRetrieveList->Start(activeRetriever->iStatus);
	activeRetriever->Activate();
	asyncRetrieveList->Cancel();
	scheduler.StartScheduler();
	ASSERT_EQUALS(0, CActiveRetriever::ResetRequestsNumber());
	ASSERT_EQUALS(KErrCancel, activeRetriever->Status().Int())
	
	AssertMockLtsyStatusL();
	
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(9); 
	
	}

/**
A small helper class to check if SessionId is defined array
*/    
TBool CCTsyPacketServiceFU::contains( TUint item, TUint* array)
    {   
    while( array )
        {
        if( item == *array )
            {
            return ETrue;            
            }
        array++;
        }
    return EFalse;
    }

TBool CCTsyPacketServiceFU::contains( TUint item, const RArray<TUint> &array )
    {   
    for( TInt i = 0; i < array.Count(); i++ )
    if( item == array[i] )
        {
        return ETrue;            
        }
    return EFalse;
    }  
