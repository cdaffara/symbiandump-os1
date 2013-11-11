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
// The TEFUnit test suite for PacketContext in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsypacketcontextfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

#include <pcktcs.h>
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>



CTestSuite* CCTsyPacketContextFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyStatusChange0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyStatusChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyStatusChange0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyStatusChange0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDeactivate0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDeactivate0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDeactivate0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDeactivate0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDeactivate0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDnsInfo0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDnsInfo0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDnsInfo0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDnsInfo0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDnsInfo0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestLoanCommPort0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestLoanCommPort0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestLoanCommPort0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestLoanCommPort0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestLoanCommPort0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetProfileName0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetProfileName0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetProfileName0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetProfileName0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetProfileName0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestRecoverCommPort0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestRecoverCommPort0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestRecoverCommPort0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestRecoverCommPort0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestRecoverCommPort0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetLastErrorCause0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetLastErrorCause0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetLastErrorCause0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetLastErrorCause0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetLastErrorCause0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestInitialiseContext0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestInitialiseContext0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestInitialiseContext0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestInitialiseContext0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestInitialiseContext0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionSpeed0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionSpeed0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionSpeed0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionSpeed0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionSpeed0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestModifyActiveContext0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestModifyActiveContext0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestModifyActiveContext0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestModifyActiveContext0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestModifyActiveContext0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConfigChanged0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyDataTransferred0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyDataTransferred0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyDataTransferred0003L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyDataTransferred0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyDataTransferred0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestEnumeratePacketFilters0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestEnumeratePacketFilters0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestEnumeratePacketFilters0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestEnumeratePacketFilters0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestEnumeratePacketFilters0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDataVolumeTransferred0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDataVolumeTransferred0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDataVolumeTransferred0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDataVolumeTransferred0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetDataVolumeTransferred0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionSpeedChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionSpeedChange0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionSpeedChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionSpeedChange0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionSpeedChange0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDelete0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDelete0002L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDelete0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDelete0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestDelete0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestActivate0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestActivate0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestActivate0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestActivate0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestActivate0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestActivate0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConfig0001L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConfig0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConfig0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConfig0004L);
//	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConfig0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetStatus0001L);
    //ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetStatus0002L);
    //ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetStatus0003L);
    //ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetStatus0004L);
    //ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetStatus0005L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestSetConfig0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestSetConfig0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestSetConfig0003L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestSetConfig0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestSetConfig0005L);    
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionInfo0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionInfo0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestGetConnectionInfo0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionInfoChange0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionInfoChange0002L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestNotifyConnectionInfoChange0004L);
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewSecondaryContext0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewSecondaryContext0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewSecondaryContext0003L);   
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewSecondaryContext0003bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewSecondaryContext0003cL);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewContext0001L);
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenNewContext0003L);    
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenExistingContext0001L);    
    ADD_TEST_STEP_ISO_CPP(CCTsyPacketContextFU, TestOpenExistingContext0003L);   

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyStatusChange0001L()
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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// data for NotifyStatusChange
	TRequestStatus	requestStatus;
	RPacketContext::TContextStatus  contextStatus; 

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::NotifyStatusChange when result is not cached.
 	//------------------------------------------------------------------------- 	
 	//data for ComleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	ltsyData.SerialiseL(data);	
	//send request
	packetContext.NotifyStatusChange(requestStatus, contextStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Test when rejection cause is used
 	//------------------------------------------------------------------------- 	
 	//data for ComleteL     
 	_LIT(KSomeCause, "1");
 	completeMisc.iRejectionCause.Copy(KSomeCause);
    completeMisc.iStatus = RPacketContext::EStatusActive;//EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData2(contextId, completeMisc);
	data.Close();
	ltsyData2.SerialiseL(data);	
	//send request
	packetContext.NotifyStatusChange(requestStatus, contextStatus);
	
	//send completion
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrGeneral, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);
	AssertMockLtsyStatusL();
 
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::NotifyStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------
 	//data for ComleteL    
    completeMisc.iStatus = RPacketContext::EStatusInactive;//EStatusActive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData1(contextId, completeMisc);
	data.Close();
	ltsyData1.SerialiseL(data);	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyStatusChange0002L()
	{

// This test should test cancellation of NotifyStatusChange
// If this API does not have a cancel, the test step should be completely removed.

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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// data for NotifyStatusChange
	TRequestStatus	requestStatus;
	RPacketContext::TContextStatus  contextStatus; 

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::NotifyStatusChange
 	//------------------------------------------------------------------------- 	
	//data for ComleteL    
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	ltsyData.SerialiseL(data);	
	//send request
	packetContext.NotifyStatusChange(requestStatus, contextStatus);	
	// send canceling...
	packetContext.CancelAsyncRequest(EPacketContextNotifyStatusChange);
	
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	
	// Wait for completion of iMockLTSY.NotifyTerminated
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::NotifyStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::NotifyStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyStatusChange0004L()
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::NotifyStatusChange
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.
	
	//data for ComleteL  for first context
 	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusInactive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	ltsyData.SerialiseL(data);	
	
	//data for ComleteL  for second context
 	TContextMisc  completeMisc2;
    completeMisc2.iStatus = RPacketContext::EStatusSuspended;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData2(contextId2, completeMisc2);
	ltsyData2.SerialiseL(data2);	
		
	// initialize data for first context's NotifyStatusChange 
	TRequestStatus	requestStatus; 
	RPacketContext::TContextStatus  contextStatus = RPacketContext::EStatusDeleted; 
	
	// initialize data for second context's NotifyStatusChange 
	TRequestStatus	requestStatus2;
	RPacketContext::TContextStatus  contextStatus2 = RPacketContext::EStatusDeleted; 	
		
	//send first request
	packetContext.NotifyStatusChange(requestStatus, contextStatus);	
	
	//send second request
	packetContext2.NotifyStatusChange(requestStatus2, contextStatus2);	
	
	//send completion for first comtext
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
		
	// wait for first context NotifyStatusChange
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);	
	// check that contextStatus2 ((of the second context)) isn't changed
	ASSERT_EQUALS(RPacketContext::EStatusDeleted, contextStatus2);	
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion for second comtext
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data2);	
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
		
	// wait for second context NotifyStatusChange
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(completeMisc2.iStatus, contextStatus2);	
	// check that contextStatus (of the first context) isn't changed
	ASSERT_EQUALS(completeMisc.iStatus, contextStatus);
	
	AssertMockLtsyStatusL();
	
	// Done !
	CleanupStack::PopAndDestroy(4);// packetContext2, packetContext, packetService2, packetService
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data, data2, this

	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Deactivate
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Deactivate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDeactivate0001L()
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
	// TEST A: failure to dispatch request to LTSY
 	//------------------------------------------------------------------------- 	
 	TMockLtsyData1 <TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
 	iMockLTSY.ExpectL(EPacketContextDeactivate, data, KErrNotFound);		
 	
 	TRequestStatus requestStatus;
	packetContext.Deactivate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------  	
    iMockLTSY.ExpectL(EPacketContextDeactivate, data);
	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrGeneral, data);
	
	packetContext.Deactivate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::Deactivate when result is not cached.
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EPacketContextDeactivate, data);
	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrNone, data);
	
	packetContext.Deactivate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::Deactivate
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCD-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::Deactivate
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::Deactivate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDeactivate0002L()
	{

// This test should test cancellation of Deactivate
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();	
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::Deactivate
 	//-------------------------------------------------------------------------
 	TRequestStatus	requestStatus;
 	TMockLtsyData1 <TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketContextDeactivate, data);
	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrNone, data, 10);
	
	packetContext.Deactivate(requestStatus);
	packetContext.CancelAsyncRequest(EPacketContextDeactivate);
	User::WaitForRequest(requestStatus);
	
	// CTSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.    
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2); // data, this	
	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCD-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::Deactivate
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::Deactivate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDeactivate0004L()
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::Deactivate
 	//-------------------------------------------------------------------------
	
	TRequestStatus	requestStatus;
 	TMockLtsyData1 <TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
	
	// data for second context
	TRequestStatus	requestStatus2;
 	TMockLtsyData1 <TInfoName > ltsyData2(contextId2);
	ltsyData2.SerialiseL(data2);
	
	iMockLTSY.ExpectL(EPacketContextDeactivate, data);
	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrNone, data, 10);
	
	iMockLTSY.ExpectL(EPacketContextDeactivate, data2);
	iMockLTSY.CompleteL(EPacketContextDeactivate, KErrNone, data2, 10);
	
	packetContext.Deactivate(requestStatus);
	packetContext2.Deactivate(requestStatus2);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::PopAndDestroy(4); // packetContext, packetService, packetContext2, packetService2
	CleanupStack::PopAndDestroy(5, this); // phone2, telServer2, data, data2, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCD-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Deactivate with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Deactivate and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDeactivate0005L()
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
	// Test A: Test timeout of RPacketContext::Deactivate
 	//-------------------------------------------------------------------------
	TRequestStatus	requestStatus;
	TMockLtsyData1 <TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);

	iMockLTSY.ExpectL(EPacketContextDeactivate, data);	
	packetContext.Deactivate(requestStatus);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGDI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetDnsInfo
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetDnsInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetDnsInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);

	 //------------------------------------------------------------------------
	// TEST D: RPacketContext::GetDnsInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
	TRequestStatus	requestStatus;
	RPacketContext::TDnsInfoV2  dnsInfo;
	
	// init dnsInfo with same data, in order to check that it will be changed
	_LIT(Kdummy, "Something");
	dnsInfo.iPrimaryDns.Copy(Kdummy);
	dnsInfo.iSecondaryDns.Copy(Kdummy);	
	TPckg<RPacketContext::TDnsInfoV2> pckgDnsInfo(dnsInfo);	
	
	// call API itself
	packetContext.GetDnsInfo(requestStatus, pckgDnsInfo);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	TBuf8<50> null50;
	ASSERT_EQUALS(null50, dnsInfo.iPrimaryDns);
	ASSERT_EQUALS(null50, dnsInfo.iSecondaryDns);
		
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this	
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCGDI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetDnsInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetDnsInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetDnsInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
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
	// RPacketContext::GetDnsInfo
 	//-------------------------------------------------------------------------
	TRequestStatus	requestStatus;
	TInt  wrongDnsInfo;
	TPckg<TInt> pckgDnsInfo(wrongDnsInfo);	
	
	packetContext.GetDnsInfo(requestStatus, pckgDnsInfo);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCLCP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::LoanCommPort
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::LoanCommPort
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestLoanCommPort0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	//-------------------------------------------------------------------------
	// Test that this function isn't supported
 	//-------------------------------------------------------------------------
	TRequestStatus	 requestStatus;
	RCall::TCommPort dataPort;
	packetContext.LoanCommPort(requestStatus, dataPort);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGPN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetProfileName
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetProfileName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetProfileName0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	
	
	//-------------------------------------------------------------------------
	//  check that no profile has been defined
 	//-------------------------------------------------------------------------
	TName qosProfile;
	TName null;
	TInt status = packetContext.GetProfileName(qosProfile);
	ASSERT_EQUALS(KErrNone, status);
	// check that no profile has been defined
	ASSERT_EQUALS(null, qosProfile);
	
	//-------------------------------------------------------------------------
	// Get Name of existing QoS
 	//-------------------------------------------------------------------------
	// create New QoS
	RPacketQoS packetQoS;
	TName qosName;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);
	
	status = packetContext.GetProfileName(qosProfile);
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(qosName, qosProfile);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //packetQoS, packetService, packetContext, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCRCP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::RecoverCommPort
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::RecoverCommPort
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestRecoverCommPort0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	
	
	//-------------------------------------------------------------------------
	// Test that this function isn't supported
 	//-------------------------------------------------------------------------
	TRequestStatus	 requestStatus;
	packetContext.RecoverCommPort(requestStatus);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this
	
	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCGLEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetLastErrorCause
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetLastErrorCause
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetLastErrorCause0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	
	//-------------------------------------------------------------------------
	// Check that there is no error on begin
 	//-------------------------------------------------------------------------
	TInt error;
	TInt status = packetContext.GetLastErrorCause(error);
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(KErrNone, error);

	
 	//-------------------------------------------------------------------------
	// Emulate some error
 	//-------------------------------------------------------------------------
 		
	// -------Set context in to Activating state -------------------------------
	TContextMisc  completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusActivating;    
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData1(contextId, completeMisc);
	ltsyData1.SerialiseL(data);	
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	// ------- Emulate an error  -------------------------------
    completeMisc.iStatus = RPacketContext::EStatusActive;    
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextId, completeMisc);
	data.Close();
	ltsyData.SerialiseL(data);		

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	//send completion
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrGeneral, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	// Get the error
	status = packetContext.GetLastErrorCause(error);
	ASSERT_EQUALS(KErrNone, status);
	ASSERT_EQUALS(KErrGeneral, error);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data, this
		
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::InitialiseContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::InitialiseContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestInitialiseContext0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
		
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	TInfoName hostCidName;
    TMockLtsyData2< TInfoName, TInfoName > ltsyExpect(contextId, hostCidName);
	ltsyExpect.SerialiseL(expectData);
 	
 	// data for initialisation of the context 
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);
	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext,expectData, KErrNotSupported);
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, expectData);
	TMockLtsyData1< TInfoName > ltsyData(contextId);
	data.Close();
	ltsyData.SerialiseL(data);	
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrGeneral, data);
	
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::InitialiseContext when result is not cached.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, expectData);
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data);
	
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	RPacketContext::TDataChannelV2 retDataCh = pckgInit();
    ASSERT_FALSE(retDataCh.iChannelId.Compare(contextId));
  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::InitialiseContext
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // expectData, data, this, packetService, packetContext	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::InitialiseContext
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::InitialiseContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestInitialiseContext0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------
 	TInfoName hostCidName;
    TMockLtsyData2< TInfoName, TInfoName > ltsyExpect(contextId, hostCidName);
	ltsyExpect.SerialiseL(expectData);
 	
 	// data for initialisation of the context 
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);
	
	TMockLtsyData1< TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, expectData);	
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data, 10);
	
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);	
	//cancel reguest
	packetContext.CancelAsyncRequest(EPacketContextInitialiseContext);
	
	User::WaitForRequest(requestStatus);	
	// CTSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.  
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	 	

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(3); // expectData, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCIC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::InitialiseContext with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::InitialiseContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestInitialiseContext0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
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
	// RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------

	// data for initialisation of the context 
	TRequestStatus requestStatus;
	RCall::TCommPort dataCh; // old version of parameter
	TPckg<RCall::TCommPort> pckgInit(dataCh);
	
	//initialisation of the context
	packetContext.InitialiseContext(requestStatus, pckgInit);	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();	 	

	CleanupStack::PopAndDestroy(3, this); 
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::InitialiseContext
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::InitialiseContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestInitialiseContext0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService, contextId2);
	CleanupClosePushL(packetContext2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.
	
	// for the first request
	TInfoName hostCidName;
    TMockLtsyData2< TInfoName, TInfoName > ltsyExpect(contextId, hostCidName);
	ltsyExpect.SerialiseL(expectData);
	
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);	
	TMockLtsyData1< TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
	
	// for the second request
    TMockLtsyData2< TInfoName, TInfoName > ltsyExpect2(contextId2, hostCidName);
	ltsyExpect2.SerialiseL(expectData2);
	
	TRequestStatus requestStatus2;
	RPacketContext::TDataChannelV2 dataCh2;
	TPckg<RPacketContext::TDataChannelV2> pckgInit2(dataCh2);	
	TMockLtsyData1< TInfoName > ltsyData2(contextId2);
	ltsyData2.SerialiseL(data2);
	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, expectData);	
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data);
	
	iMockLTSY.ExpectL(EPacketContextInitialiseContext, expectData2);	
	iMockLTSY.CompleteL(EPacketContextInitialiseContext, KErrNone, data2);
	
	//first request
	packetContext.InitialiseContext(requestStatus, pckgInit);
	
	//second request
	packetContext2.InitialiseContext(requestStatus2, pckgInit2);	
		
	// wait for the first answer
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	RPacketContext::TDataChannelV2 retDataChannel = pckgInit();
    ASSERT_FALSE(retDataChannel.iChannelId.Compare(contextId));
         
    // wait for the second answer
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	
	
    retDataChannel = pckgInit2();
    ASSERT_FALSE(retDataChannel.iChannelId.Compare(contextId2));
        	
    AssertMockLtsyStatusL();	 	
	CleanupStack::PopAndDestroy(8, this); //packetContext2, packetContext, packetService, expectData, expectData2, data, data2, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCIC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::InitialiseContext with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::InitialiseContext and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestInitialiseContext0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::InitialiseContext
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	RPacketContext::TDataChannelV2 dataCh;
	TPckg<RPacketContext::TDataChannelV2> pckgInit(dataCh);	
	
	TInfoName hostCidName;
    TMockLtsyData2< TInfoName, TInfoName > ltsyExpect(contextId, hostCidName);
	ltsyExpect.SerialiseL(expectData);

	iMockLTSY.ExpectL(EPacketContextInitialiseContext, expectData);
		
	packetContext.InitialiseContext(requestStatus, pckgInit);
	
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	// Done !
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //packetService, packetContext, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGCS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetConnectionSpeed
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetConnectionSpeed
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConnectionSpeed0001L()
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
	// TEST D: RPacketContext::ModifyActiveContext again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	TUint rate;
	TUint null = 0;
	packetContext.GetConnectionSpeed(requestStatus, rate);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(null, rate);
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Now change the Connection Speed
	// and check that GetConnectionSpeed return appropriate value
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	TInt connectionSpeed = 9600;	
	TMockLtsyData2< TInfoName, TInt > ltsyData(contextId, connectionSpeed);
	ltsyData.SerialiseL(data);
	
	// change the Connection Speed
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionSpeedChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// get changed  Connection Speed
	packetContext.GetConnectionSpeed(requestStatus, rate);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TUint)connectionSpeed, rate);
	
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data, this	
	}



/**
Cleanup CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>*
*/
void CCTsyPacketContextFU::CleanupArrayPtrFlat(TAny* aSelf)
	{
	if(aSelf)
		{
		CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* array (NULL);
		array = static_cast<CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>*>(aSelf);
		array->ResetAndDestroy();
		delete array;
		}	
	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCMAC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::ModifyActiveContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::ModifyActiveContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestModifyActiveContext0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectDataSPP;
	CleanupClosePushL(expectDataSPP);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the context 
	TInt err = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, err);
		
 	//-------------------------------------------------------------------------
	// TEST: call ModifyActiveContext when context wasn't active
 	//-------------------------------------------------------------------------	
	TRequestStatus requestStatus;	
	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
	
 	
 	// Set context in Active state
 	err = SetContextActiveL(contextId);
 	ASSERT_EQUALS(KErrNone, err);	
 	
	//-------------------------------------------------------------------------
	// TEST: call ModifyActiveContext when context is active, but with no QoS object created
 	//------------------------------------------------------------------------- 	
	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	
	// ------ create New QoS -------------------------------------------------
	RPacketQoS packetQoS;
	TName qosName;
	packetQoS.OpenNewQoS(packetContext, qosName);
	CleanupClosePushL(packetQoS);	
	//-------------------------------------------------------------------------
	// TEST: call ModifyActiveContext when context is active, and QoS object is created, but 
	// with no ProfileParameters
 	//------------------------------------------------------------------------- 
 	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
 	
 	
	// ------ SetProfileParameters --------------------------------------------
	RPacketQoS::TQoSGPRSRequested gprsRequested;	
	TPckg<RPacketQoS::TQoSGPRSRequested> pckg(gprsRequested);
	
	// prepare data for expectDataSPP ... 
	// ... context status
	RPacketContext::TContextStatus contextStatus = RPacketContext::EStatusActive;
	// ... get  contextConfig
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg< RPacketContext::TContextConfigGPRS > contextConfigPckg( contextConfig );	
	packetContext.GetConfig(requestStatus, contextConfigPckg);
	User::WaitForRequest(requestStatus);	
	
	// data for SetProfileParameters's ExpectL
	TInfoName primaryContextName;	
	TContextParams	contextParams;
	contextParams.iContextName = contextId;
	contextParams.iContextType = contextConfig.iNWIContext;
	contextParams.iPdpType = contextConfig.iPdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;		
	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect(contextParams, gprsRequested);
    expExpect.SerialiseL(expectDataSPP);
	
	iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectDataSPP);
	packetQoS.SetProfileParameters(requestStatus, pckg);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	// data for ExpectL
 	RPacketQoS::TQoSGPRSRequested* qosProfile97 = &gprsRequested;
    	
 	TContextConfiguration expectConfig;
 	expectConfig.iContextName = contextId;
    expectConfig.iQosParams97 = qosProfile97;
    expectConfig.iQosParams99 = NULL;
	expectConfig.iQosParamsR5 = NULL;
    expectConfig.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
	CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* mediaAuthorizationArray(NULL);
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
 																			ltsyExpect(expectConfig, mediaAuthorizationArray);
	ltsyExpect.SerialiseL(expectData);	 	
 	//ExpectL
 	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData, KErrNotSupported);
 	
 	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 	TBuf8<1> rejectionCode(1);
 	TMockLtsyData2 < TInfoName, TBuf8<1> > ltsyData2(contextId, rejectionCode);
	ltsyData2.SerialiseL(data);
 	
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrGeneral, data);
 	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::ModifyActiveContext when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data);
 	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST Successful completion request of
	// RPacketContext::ModifyActiveContext with some filter added
 	//-------------------------------------------------------------------------	
	// Open new secondary context
	TInfoName contextIdSecondary;	
	RPacketContext packetContextSecondary;
	err = packetContextSecondary.OpenNewSecondaryContext(packetService, contextId, contextIdSecondary);
 	ASSERT_EQUALS(KErrNone, err);	
	CleanupClosePushL(packetContextSecondary);
	
    // setting environement (add media authorization to CTSY)
	mediaAuthorizationArray = new(ELeave) CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>(1);
	CleanupStack::PushL(TCleanupItem(CleanupArrayPtrFlat,mediaAuthorizationArray));

    RPacketContext::CTFTMediaAuthorizationV3* media  = RPacketContext::CTFTMediaAuthorizationV3::NewL(); 
    CleanupStack::PushL(media);
    media->iAuthorizationToken = _L8("testtok");
    RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier fid;
	fid.iIPFlowNumber = 2;
	fid.iMediaComponentNumber = 3;
	media->iFlowIds.Append(fid);
	fid.iIPFlowNumber = 4;
	fid.iMediaComponentNumber = 5;
	media->iFlowIds.Append(fid);
	mediaAuthorizationArray->AppendL(media);
	CleanupStack::Pop(); //media
 	
 	// update contextConfig
 	packetContext.GetConfig(requestStatus, contextConfigPckg);	
	User::WaitForRequest(requestStatus);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
		expAddData(contextConfig, 0, contextIdSecondary, media);
	data.Close();
	expAddData.SerialiseL(data);
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);

 	TMockLtsyData1<TInfoName > contexName(contextIdSecondary);
	data.Close();
    contexName.SerialiseL(data);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

 	packetContextSecondary.AddMediaAuthorizationL(requestStatus, *media);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// set context Active
	SetContextActiveL(contextIdSecondary);
	AssertMockLtsyStatusL();
	
	RPacketQoS packetQoSSecondary;
	RPacketQoS::TQoSGPRSRequested secondaryRequested;
	err = CreateQoSAndSetProfileParamLC(packetQoSSecondary, packetContextSecondary,
											 contextIdSecondary, secondaryRequested, contextId);// contextId -- is name of "primary" context
	ASSERT_EQUALS(KErrNone, err);
	AssertMockLtsyStatusL();
	
	// Add filter
	RPacketContext::TPacketFilterV2 filter;
	filter.iId = 1;
	RPacketContext::TPacketFilterV2Pckg filterPckg (filter);
	err = AddContextPacketFilterL(packetContextSecondary, contextIdSecondary,filter, ETrue);// ETrue means that this request doesn't reache LTSY
	ASSERT_EQUALS(KErrNone, err);
	AssertMockLtsyStatusL();		
	
	// data for ExpectL    	
	expectConfig.iContextName = contextIdSecondary;
	expectConfig.iQosParams97 = &secondaryRequested;
	expectConfig.iQosParams99 = NULL;
	expectConfig.iQosParamsR5 = NULL;
	expectConfig.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
	                                                     ltsyExpect2(expectConfig, mediaAuthorizationArray);
	expectData.Close();
	ltsyExpect2.SerialiseL(expectData); 
	
	// data for CompleteL
	TMockLtsyData2 < TInfoName, TBuf8<1> > ltsyData3(contextIdSecondary, rejectionCode);
	data.Close();
	ltsyData3.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data);
 	packetContextSecondary.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	
	//-------------------------------------------------------------------------
	// TEST Successful completion request of
	// RPacketContext::ModifyActiveContext with some filter removed
 	//-------------------------------------------------------------------------	
	TInt filterId = 1;
	packetContextSecondary.RemovePacketFilter(requestStatus, filterId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	AssertMockLtsyStatusL();		
	
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data);
 	packetContextSecondary.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	

  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::ModifyActiveContext
	// from LTSY.
 	//-------------------------------------------------------------------------	
 	TRequestStatus mockLtsyStatus;
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4);	// packetQoSSecondary, mediaAuthorizationArray, packetContextSecondary, packetQoS
	CleanupStack::PopAndDestroy(6, this); //packetContext, packetService, expectDataSPP, expectData, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCMAC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::ModifyActiveContext
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::ModifyActiveContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestModifyActiveContext0002L()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
 	// Set context in Active state
 	status = SetContextActiveL(contextId);
 	ASSERT_EQUALS(KErrNone, status);
 	
 	//SetProfileParameters
 	RPacketQoS::TQoSGPRSRequested gprsRequested;
 	RPacketQoS packetQoS;
 	status = CreateQoSAndSetProfileParamLC(packetQoS, packetContext ,contextId, gprsRequested);
	ASSERT_EQUALS(KErrNone, status);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::ModifyActiveContext
 	//-------------------------------------------------------------------------
 	// data for ExpectL
 	RPacketQoS::TQoSGPRSRequested* qosProfile97 = &gprsRequested;
    	
	CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* expectArray = NULL;
 	TContextConfiguration expectConfig;
 	expectConfig.iContextName = contextId;
    expectConfig.iQosParams97 = qosProfile97;
    expectConfig.iQosParams99 = NULL;
	expectConfig.iQosParamsR5 = NULL;
    expectConfig.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
 																			ltsyExpect(expectConfig, expectArray);
	ltsyExpect.SerialiseL(expectData);	 
 	
 	TRequestStatus requestStatus;
 	
 	// data for CompleteL
	TBuf8<1> rejectionCode(1);
 	TMockLtsyData2 < TInfoName, TBuf8<1> > ltsyData2(contextId, rejectionCode);
	ltsyData2.SerialiseL(data);
 	
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data, 10);
 	packetContext.ModifyActiveContext(requestStatus);
 	 	
 	// cancel request
 	packetContext.CancelAsyncRequest(EPacketContextModifyActiveContext); 	
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	// TSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	CleanupStack::PopAndDestroy();//packetQoS
	CleanupStack::PopAndDestroy(5, this); // packetService, packetContext,  expectData, data,  this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCMAC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::ModifyActiveContext
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::ModifyActiveContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestModifyActiveContext0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);
	
	RBuf8 expectData3;
	CleanupClosePushL(expectData3);
	
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService, contextId2);
	CleanupClosePushL(packetContext2);
	
	// Open third context
	TInfoName contextId3;	
	RPacketContext packetContext3;
	packetContext3.OpenNewContext(packetService, contextId3);
	CleanupClosePushL(packetContext3);

	// initialisation of the first context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
 	// Set first context in Active state
 	status = SetContextActiveL(contextId);
 	ASSERT_EQUALS(KErrNone, status);
 	
 	//SetProfileParameters first context's
 	RPacketQoS::TQoSGPRSRequested gprsRequested;
 	RPacketQoS packetQoS;
 	status = CreateQoSAndSetProfileParamLC(packetQoS, packetContext,contextId, gprsRequested);
	ASSERT_EQUALS(KErrNone, status);
	
	
	// initialisation of the second context 
	status = InitContextL(packetContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
		
 	// Set second context in Active state
 	status = SetContextActiveL(contextId2);
 	ASSERT_EQUALS(KErrNone, status);
 	
 	//SetProfileParameters second context's
 	RPacketQoS::TQoSR99_R4Requested rel99Requested;
 	RPacketQoS packetQoS2;
 	status = CreateQoSAndSetProfileParamLC(packetQoS2, packetContext2, contextId2, rel99Requested);
	ASSERT_EQUALS(KErrNone, status);
	
	
	// initialisation of the third context 
	status = InitContextL(packetContext3, contextId3);
	ASSERT_EQUALS(KErrNone, status);
		
 	// Set third context in Active state
 	status = SetContextActiveL(contextId3);
 	ASSERT_EQUALS(KErrNone, status);
 	
 	//SetProfileParameters third context's
 	RPacketQoS::TQoSR5Requested r5Requested;	
	RPacketQoS packetQoS3;
 	status = CreateQoSAndSetProfileParamLC(packetQoS3, packetContext3, contextId3, r5Requested);
	ASSERT_EQUALS(KErrNone, status);
		
 	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::ModifyActiveContext
 	//------------------------------------------------------------------------- 	
	TBuf8<1> rejectionCode(1);
	// data for the first ExpectL     	
	CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* expectArray = NULL;
 	TContextConfiguration expectConfig;
 	expectConfig.iContextName = contextId;
    expectConfig.iQosParams97 = &gprsRequested;
    expectConfig.iQosParams99 = NULL;
	expectConfig.iQosParamsR5 = NULL;
    expectConfig.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
 																			ltsyExpect(expectConfig, expectArray);
	ltsyExpect.SerialiseL(expectData);	 
	
	// data for the second ExpectL 
  	TContextConfiguration expectConfig2;
 	expectConfig2.iContextName = contextId2;
    expectConfig2.iQosParams97 = NULL;
    expectConfig2.iQosParams99 = &rel99Requested;
	expectConfig2.iQosParamsR5 = NULL;
    expectConfig2.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
 																			ltsyExpect2(expectConfig2, expectArray);
	ltsyExpect2.SerialiseL(expectData2);	 
	
	// data for the third ExpectL 
  	TContextConfiguration expectConfig3;
 	expectConfig3.iContextName = contextId3;
    expectConfig3.iQosParams97 = NULL;
    expectConfig3.iQosParams99 = NULL;
	expectConfig3.iQosParamsR5 = &r5Requested;
    expectConfig3.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
 																			ltsyExpect3(expectConfig3, expectArray);
	ltsyExpect3.SerialiseL(expectData3);	 
	
  
    // prepare and send first request
	TMockLtsyData2 < TInfoName, TBuf8<1> > dataForComplete(contextId, rejectionCode);
	dataForComplete.SerialiseL(data);	 	
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data, 10);  	
 	TRequestStatus requestStatus; 	
 	packetContext.ModifyActiveContext(requestStatus);
 
	// prepare and send first request 
 	TMockLtsyData2 < TInfoName, TBuf8<1> > dataForComplete2(contextId2, rejectionCode);
	data.Close();
	dataForComplete2.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData2);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data, 10); 	
 	TRequestStatus requestStatus2;
 	packetContext2.ModifyActiveContext(requestStatus2); 	
 	
 	// prepare and send third request 
 	TMockLtsyData2 < TInfoName, TBuf8<1> > dataForComplete3(contextId3, rejectionCode);
	data.Close();
	dataForComplete3.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData3);
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data, 10); 	
 	TRequestStatus requestStatus3;
 	packetContext3.ModifyActiveContext(requestStatus3);  	
 	
 	// wait for first answer
 	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	// wait for second answer
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	// wait for third answer
	User::WaitForRequest(requestStatus3);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());
	
	CleanupStack::PopAndDestroy(3); // packetQoS, packetQoS2, packetQoS3, 
	CleanupStack::PopAndDestroy(11, this); //packetContext, packetContext2,packetContext3
											//packetService, phone2,telServer2, 
										   //expectData3, expectData2, expectData, data, this									   

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCMAC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::ModifyActiveContext with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::ModifyActiveContext and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestModifyActiveContext0005L()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	
	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
 	// Set context in Active state
 	status = SetContextActiveL(contextId);
 	ASSERT_EQUALS(KErrNone, status);
 	
 	//SetProfileParameters
 	RPacketQoS::TQoSGPRSRequested gprsRequested;
 	RPacketQoS packetQoS;
 	status = CreateQoSAndSetProfileParamLC(packetQoS, packetContext, contextId, gprsRequested);
	ASSERT_EQUALS(KErrNone, status);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::ModifyActiveContext
 	//------------------------------------------------------------------------- 
 	TRequestStatus requestStatus; 	
 	// data for ExpectL
 	RPacketQoS::TQoSGPRSRequested* qosProfile97 = &gprsRequested;    	
	CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* expectArray = NULL;
 	TContextConfiguration expectConfig;
 	expectConfig.iContextName = contextId;
    expectConfig.iQosParams97 = qosProfile97;
    expectConfig.iQosParams99 = NULL;
	expectConfig.iQosParamsR5 = NULL;
    expectConfig.iConfiguration = RPacketContext::EPdpTypeIPv4;    	 	
 	
 	TMockLtsyData2 < TContextConfiguration, CArrayPtrFlat<RPacketContext::CTFTMediaAuthorizationV3>* > 
 																			ltsyExpect(expectConfig, expectArray);
	ltsyExpect.SerialiseL(expectData);	 	
	
	iMockLTSY.ExpectL(EPacketContextModifyActiveContext, expectData);
	
 	packetContext.ModifyActiveContext(requestStatus); 	
 		
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(6, this); //packetQoS, packetService, packetContext, expectData, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConfigChanged0001L()
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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::NotifyConfigChanged when result is not cached.
 	//-------------------------------------------------------------------------
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig;	
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);	
	
	//data for ComleteL   
 	RPacketContext::TContextConfigGPRS configComplete;  	 	
 	// (just this data is copied in CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2 
	// see CMmPacketContextGsmWcdma.cpp line 339) 	  
    _LIT(KPointName, "Some Name");
    configComplete.iAccessPointName.Copy(KPointName);
    _LIT(KPdpAdd, "Some Add");
    configComplete.iPdpAddress.Copy(KPdpAdd);    	
	configComplete.iPdpCompression = RPacketContext::KPdpDataCompression;		
	_LIT(KPrimaryDns, "Some PrimaryDns");
	configComplete.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	_LIT(KSecondaryDns, "Some SecondaryDns");
	configComplete.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);	
 	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData(contextId, configComplete);
	ltsyData.SerialiseL(data);	

	// send request	
	packetContext.NotifyConfigChanged(requestStatus, pckgConfig);
		
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
		
	// wait for request
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(configComplete.iAccessPointName, 		contextConfig.iAccessPointName);
	ASSERT_EQUALS(configComplete.iPdpAddress, 			contextConfig.iPdpAddress );
	ASSERT_EQUALS(configComplete.iPdpCompression, 		contextConfig.iPdpCompression);
	ASSERT_EQUALS(configComplete.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, 	contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(configComplete.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	

	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::NotifyConfigChanged
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	CleanupStack::PopAndDestroy(2); // packetService, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}
	
	
/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes CMmPacketServiceTsy::DialUpContext via RPacketContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConfigChanged0001bL()
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
	// call completition of RPacketContext::NotifyConfigChanged
	// to call CMmPacketServiceTsy::DialUpContext
	// with ContextID ==  KStringExternal
 	//------------------------------------------------------------------------- 	
 	//data for ComleteL   
 	RPacketContext::TContextConfigGPRS configComplete;  	 
 	TInfoName ContextID;	
 	_LIT( KStringExternal,  "External"  );
 	ContextID.Copy(KStringExternal);
  	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData(ContextID, configComplete);
	ltsyData.SerialiseL(data);	
 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	
	//-------------------------------------------------------------------------
	// call completition of RPacketContext::NotifyConfigChanged
	// to call CMmPacketServiceTsy::DialUpContext
	// with ContextID ==  KStringExternal2
 	//------------------------------------------------------------------------- 	
 	//data for ComleteL     	 
  	_LIT( KStringExternal2,  "External2"  );
 	ContextID.Copy(KStringExternal2);
  	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData2(ContextID, configComplete);
	data.Close();
	ltsyData2.SerialiseL(data);	
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	
	//-------------------------------------------------------------------------
	// call completition of RPacketContext::NotifyConfigChanged
	// to call CMmPacketServiceTsy::DialUpContext
	// with (ContextID !=  KStringExternal2)&&(ContextID !=  KStringExternal)
 	//------------------------------------------------------------------------- 	
 	//data for ComleteL     	 
  	_LIT( KSomeId,  "SomeId"  );
 	ContextID.Copy(KSomeId);
  	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData3(ContextID, configComplete);
	data.Close();
	ltsyData3.SerialiseL(data);
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	
	CleanupStack::PopAndDestroy(2); // packetService, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCC-0001c
@SYMComponent  telephony_ctsy
@SYMDEF DEF126915
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyConfigChanged using R99/R4 Context params
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyConfigChange. R99/R4 Context params returned
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConfigChanged0001cL()
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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::NotifyConfigChanged, using TContextConfigR99_R4 return 
	// params, when result is not cached.
 	//-------------------------------------------------------------------------
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig;	
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);	
	
	//data for ComleteL   
 	RPacketContext::TContextConfigR99_R4 configComplete;  	 	
 	// (just this data is copied in CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2 
	// see CMmPacketContextGsmWcdma.cpp line 339) 	  
    _LIT(KPointName, "Some Name");
    configComplete.iAccessPointName.Copy(KPointName);
    _LIT(KPdpAdd, "Some Add");
    configComplete.iPdpAddress.Copy(KPdpAdd);    	
	_LIT(KPrimaryDns, "Some PrimaryDns");
	configComplete.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	_LIT(KSecondaryDns, "Some SecondaryDns");
	configComplete.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);	
 	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigR99_R4 > ltsyData(contextId, configComplete);
	ltsyData.SerialiseL(data);	

	// send request	
	packetContext.NotifyConfigChanged(requestStatus, pckgConfig);
		
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
		
	// wait for request
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(configComplete.iAccessPointName, 		contextConfig.iAccessPointName);
	ASSERT_EQUALS(configComplete.iPdpAddress, 			contextConfig.iPdpAddress );
	ASSERT_EQUALS(configComplete.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, 	contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(configComplete.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	

	CleanupStack::PopAndDestroy(2); // packetService, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConfigChanged0002L()
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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::NotifyConfigChanged
 	//------------------------------------------------------------------------- 	
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig;	
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);	
	
	//data for ComleteL    
 	RPacketContext::TContextConfigGPRS configComplete;
 	_LIT (KNewName, "Some new Name");
 	configComplete.iPdpAddress.Copy(KNewName); 	
 	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData(contextId, configComplete);
	ltsyData.SerialiseL(data);	

	// send request	
	packetContext.NotifyConfigChanged(requestStatus, pckgConfig);	
	//send cancellation
	packetContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
		
	// sent completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	
	// wait for request
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // packetService, packetContext
	CleanupStack::PopAndDestroy(2); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyConfigChanged with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyConfigChanged with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConfigChanged0003L()
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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketContext::NotifyConfigChanged
 	//-------------------------------------------------------------------------
	// data for NotifyConfigChanged	
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigCDMA contextConfig;	
	TPckg<RPacketContext::TContextConfigCDMA> pckgConfig(contextConfig);	

	// send request	
	packetContext.NotifyConfigChanged(requestStatus, pckgConfig);

	// wait for request
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	CleanupStack::PopAndDestroy(2); // packetService, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::NotifyConfigChanged
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::NotifyConfigChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConfigChanged0004L()
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::NotifyConfigChanged
 	//-------------------------------------------------------------------------
	
	// data for NotifyConfigChanged	1
	TRequestStatus requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig1;	
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig1(contextConfig1);	
	
	//data for ComleteL 1   
 	RPacketContext::TContextConfigGPRS configComplete1;
 	// (just this data is copied in CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2 
	// see CMmPacketContextGsmWcdma.cpp line 339) 	  
    _LIT(KPointName, "Some Name");
    configComplete1.iAccessPointName.Copy(KPointName);
    _LIT(KPdpAdd, "Some Add");
    configComplete1.iPdpAddress.Copy(KPdpAdd);    	
	configComplete1.iPdpCompression = RPacketContext::KPdpDataCompression;		
	_LIT(KPrimaryDns, "Some PrimaryDns");
	configComplete1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	_LIT(KSecondaryDns, "Some SecondaryDns");
	configComplete1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);	 	
 	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData(contextId, configComplete1);
	ltsyData.SerialiseL(data);
	
	
	// data for NotifyConfigChanged	2
	TRequestStatus requestStatus2;
	RPacketContext::TContextConfigR99_R4 contextConfig2;	
	TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig2(contextConfig2);	
	
	//data for ComleteL 2 
 	RPacketContext::TContextConfigGPRS configComplete2; 
 	// (just this data is copied in CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2 
	// see CMmPacketContextGsmWcdma.cpp line 339) 	  
    _LIT(KPointName2, "Some Name2");
    configComplete2.iAccessPointName.Copy(KPointName2);
    _LIT(KPdpAdd2, "Some Add2");
    configComplete2.iPdpAddress.Copy(KPdpAdd2);    	
	configComplete2.iPdpCompression = RPacketContext::KPdpDataCompression;		
	_LIT(KPrimaryDns2, "Some PrimaryDns2");
	configComplete2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns2);
	_LIT(KSecondaryDns2, "Some SecondaryDns2");
	configComplete2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns2);
 	TMockLtsyData2 < TInfoName, RPacketContext::TContextConfigGPRS > ltsyData2(contextId2, configComplete2);
	ltsyData2.SerialiseL(data2);
	
	// send first request	
	packetContext.NotifyConfigChanged(requestStatus, pckgConfig1);
	
	// send second request	
	packetContext2.NotifyConfigChanged(requestStatus2, pckgConfig2);
	
	// sent first completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data);
	
	// sent second completion
	iMockLTSY.CompleteL(EPacketContextNotifyConfigChanged, KErrNone, data2);	
	
	// wait for first answer
	User::WaitForRequest(requestStatus);
		
	// wait for second answer
	User::WaitForRequest(requestStatus2);	
	AssertMockLtsyStatusL();		
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	
	
	ASSERT_EQUALS(configComplete1.iAccessPointName, 	contextConfig1.iAccessPointName);
	ASSERT_EQUALS(configComplete1.iPdpAddress, 			contextConfig1.iPdpAddress );
	ASSERT_EQUALS(configComplete1.iPdpCompression, 		contextConfig1.iPdpCompression);
	ASSERT_EQUALS(configComplete1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, 	
				  contextConfig1.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(configComplete1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns,
				  contextConfig1.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
				  	
	ASSERT_EQUALS(configComplete2.iAccessPointName, 	contextConfig2.iAccessPointName);
	ASSERT_EQUALS(configComplete2.iPdpAddress, 			contextConfig2.iPdpAddress);
	ASSERT_EQUALS(configComplete2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, 	 
				  contextConfig2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(configComplete2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, 
				  contextConfig2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
				  
	CleanupStack::PopAndDestroy(9, this); //packetContext2, packetContext, packetService2, packetService, phone2, telServer2, data,data2 this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNDT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyDataTransferred
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyDataTransferred
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyDataTransferred0001L()
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
	//check that this requests is not supported by CTSY
 	//-------------------------------------------------------------------------	
	TRequestStatus requestStatus;
	RPacketContext::TDataVolume volume;
	TUint rcvdGranularity = 1;
	TUint sentGranularity = 1;
	
	packetContext.NotifyDataTransferred(requestStatus, volume, rcvdGranularity, sentGranularity);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::NotifyConfigChanged
	// from LTSY.
 	//-------------------------------------------------------------------------
 	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	RPacketContext::TDataVolume dataVolume;
	dataVolume. iBytesReceived = 111;	
	TMockLtsyData2 < TInfoName, RPacketContext::TDataVolume > ltsyData(contextId, dataVolume);
	ltsyData.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyDataTransferred, KErrNone, data);
	
	User::WaitForRequest(mockLtsyStatus);		
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // packetService, packetContext, data, this	
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCEPF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::EnumeratePacketFilters
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::EnumeratePacketFilters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestEnumeratePacketFilters0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	// Open new context 
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	
	
  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::EnumeratePacketFilters when result is not cached.
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;
	TInt count = 11;  // some nonzero value 
	packetContext.EnumeratePacketFilters(requestStatus, count);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, count);
	
	
	//-------------------------------------------------------------------------
	// Add filter and test again
 	//-------------------------------------------------------------------------	
	// Open new secondary context
	TInfoName contextIdSecondary;	
	RPacketContext packetContextSecondary;
	packetContextSecondary.OpenNewSecondaryContext(packetService, contextId, contextIdSecondary);
	CleanupClosePushL(packetContextSecondary);
			
	RPacketQoS packetQoSSecondary;
	RPacketQoS::TQoSGPRSRequested secondaryRequested;
	TInt err = CreateQoSAndSetProfileParamLC(packetQoSSecondary, packetContextSecondary,
											 contextIdSecondary, secondaryRequested, contextId);	
	ASSERT_EQUALS(KErrNone, err);	
	CleanupClosePushL(packetQoSSecondary);
		
	// add new filter
	RPacketContext::TPacketFilterV2 filter;
	filter.iId = 1;
	RPacketContext::TPacketFilterV2Pckg filterPckg (filter);	
	err = AddContextPacketFilterL(packetContextSecondary, contextIdSecondary, filter);
		
	ASSERT_EQUALS(KErrNone, err);	
		
	// send request
	packetContextSecondary.EnumeratePacketFilters(requestStatus, count);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(1, count);		

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); // packetQoSSecondary, packetContextSecondary, packetService, packetContext,data, this	
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCEPF-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::EnumeratePacketFilters
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::EnumeratePacketFilters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestEnumeratePacketFilters0004L()
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::EnumeratePacketFilters
 	//-------------------------------------------------------------------------
	
	TRequestStatus requestStatus;
	TInt count = 11;  // some nonzero value 
	TRequestStatus requestStatus2;
	TInt count2 = 22;  // some nonzero value 
	
	// send first request
	packetContext.EnumeratePacketFilters(requestStatus, count);
	// send second request
	packetContext2.EnumeratePacketFilters(requestStatus2, count2);
	
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(0, count);
	
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(0, count2);
		
	CleanupStack::PopAndDestroy(7, this); // packetContext2, packetContext, packetService2, packetService, phone2, telServer2, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGDVT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetDataVolumeTransferred
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetDataVolumeTransferred
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetDataVolumeTransferred0001L()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);		
		
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
 	
	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred,expectData, KErrNotSupported);	
	
	TRequestStatus requestStatus;
	RPacketContext::TDataVolume volume;	
	packetContext.GetDataVolumeTransferred(requestStatus, volume);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------	
 	// data for CompleteL
 	RPacketContext::TDataVolume volumeCompleteL;
 	volumeCompleteL.iBytesReceived = 0x111;
 	volumeCompleteL.iBytesSent     = 0x222;
 	TMockLtsyData2 <TInfoName, RPacketContext::TDataVolume > ltsyData1(contextId, volumeCompleteL);
	ltsyData1.SerialiseL(data);
 	
 	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred, expectData);	 	
 	iMockLTSY.CompleteL(EPacketContextGetDataVolumeTransferred, KErrGeneral, data);	
	
	packetContext.GetDataVolumeTransferred(requestStatus, volume);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::GetDataVolumeTransferred when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred, expectData);	 	
 	iMockLTSY.CompleteL(EPacketContextGetDataVolumeTransferred, KErrNone, data);	
	
	packetContext.GetDataVolumeTransferred(requestStatus, volume);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(volumeCompleteL.iBytesReceived, volume.iBytesReceived);
	ASSERT_EQUALS(volumeCompleteL.iBytesSent, volume.iBytesSent);
	AssertMockLtsyStatusL();

  	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::GetDataVolumeTransferred
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EPacketContextGetDataVolumeTransferred, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);		
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
		
	//-------------------------------------------------------------------------
	// TEST of RPacketContext::GetDataVolumeTransferred
	// when ContextStatus == RPacketContext::EStatusDeleted in CMmPacketContextTsy
 	//------------------------------------------------------------------------- 		
	
	// set Context status as RPacketContext::EStatusDeleted	
	TInt err = SetContextStatusL(contextId, RPacketContext::EStatusDeleted);
	ASSERT_EQUALS(KErrNone, err);	
	
	// send request	
	packetContext.GetDataVolumeTransferred(requestStatus, volume);
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(volumeCompleteL.iBytesReceived, volume.iBytesReceived);
	ASSERT_EQUALS(volumeCompleteL.iBytesSent, volume.iBytesSent);		

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // packetContext, packetService, expectData, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGDVT-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::GetDataVolumeTransferred
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::GetDataVolumeTransferred
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetDataVolumeTransferred0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);	
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::GetDataVolumeTransferred
 	//-------------------------------------------------------------------------
 	TRequestStatus requestStatus;
	RPacketContext::TDataVolume volume;	
	
	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	// data for CompleteL
 	RPacketContext::TDataVolume volumeCompleteL;
 	volumeCompleteL.iBytesReceived = 0x111;
 	volumeCompleteL.iBytesSent     = 0x222;
 	TMockLtsyData2 <TInfoName, RPacketContext::TDataVolume > ltsyData1(contextId, volumeCompleteL);
	ltsyData1.SerialiseL(data);
 	 	
 	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred, expectData);	 	
 	 	
 	// send request  	
 	packetContext.GetDataVolumeTransferred(requestStatus, volume);
 	
 	//sent cancelling
 	packetContext.CancelAsyncRequest(EPacketContextGetDataVolumeTransferred); 	
 	
 	//send complete request
 	iMockLTSY.CompleteL(EPacketContextGetDataVolumeTransferred, KErrNone, data, 10);
 	
	User::WaitForRequest(requestStatus);		
	// TSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(volumeCompleteL.iBytesReceived, volume.iBytesReceived);
	ASSERT_EQUALS(volumeCompleteL.iBytesSent, volume.iBytesSent);
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(5, this); // packetContext, packetService,expectData, data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGDVT-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::GetDataVolumeTransferred
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::GetDataVolumeTransferred
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetDataVolumeTransferred0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
		
	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);

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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::GetDataVolumeTransferred
 	//-------------------------------------------------------------------------

	// data for first CompleteL
 	RPacketContext::TDataVolume volumeCompleteL;
 	volumeCompleteL.iBytesReceived = 0x111;
 	volumeCompleteL.iBytesSent     = 0x222;
 	TMockLtsyData2 <TInfoName, RPacketContext::TDataVolume > ltsyData1(contextId, volumeCompleteL);
	ltsyData1.SerialiseL(data);
	
	// data for second CompleteL
  	TMockLtsyData2 <TInfoName, RPacketContext::TDataVolume > ltsyData2(contextId2, volumeCompleteL);
	ltsyData2.SerialiseL(data2);	
	
	// for the first request
	TRequestStatus requestStatus;
	RPacketContext::TDataVolume volume;
	
	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	// for the second request
	TRequestStatus requestStatus2;
	RPacketContext::TDataVolume volume2;
	
	TMockLtsyData1 <TInfoName> expData2(contextId2);
	expData2.SerialiseL(expectData2);
	
	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred, expectData);	 	
 	iMockLTSY.CompleteL(EPacketContextGetDataVolumeTransferred, KErrNone, data);
	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred, expectData2); 	 
	iMockLTSY.CompleteL(EPacketContextGetDataVolumeTransferred, KErrNone, data2);
	
	// send first request
	packetContext.GetDataVolumeTransferred(requestStatus, volume);		
	
	// send second request
	packetContext2.GetDataVolumeTransferred(requestStatus2, volume2);
	
	// wait for first answer
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(volumeCompleteL.iBytesReceived, volume.iBytesReceived);
	ASSERT_EQUALS(volumeCompleteL.iBytesSent, volume.iBytesSent);
	
	// wait for second answer
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(volumeCompleteL.iBytesReceived, volume2.iBytesReceived);
	ASSERT_EQUALS(volumeCompleteL.iBytesSent, volume2.iBytesSent);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // expectData2, expectData
	CleanupStack::PopAndDestroy(9, this); //packetContext2, packetContext, packetService2, packetService, phone2, telServer2, data,data2 this

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGDVT-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetDataVolumeTransferred with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetDataVolumeTransferred and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetDataVolumeTransferred0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::GetDataVolumeTransferred
 	//------------------------------------------------------------------------- 	
	
	TRequestStatus requestStatus;
	RPacketContext::TDataVolume volume;
	
	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	iMockLTSY.ExpectL(EPacketContextGetDataVolumeTransferred, expectData);
	
	packetContext.GetDataVolumeTransferred(requestStatus, volume);	
	
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //packetContext, packetService,  expectData, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyConnectionSpeedChange
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::NotifyConnectionSpeedChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConnectionSpeedChange0001L()
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
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);		
	
  	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::NotifyConnectionSpeedChange when result is not cached.
 	//-------------------------------------------------------------------------
	// data for CompleteL
	TInt completeRate = 9600;
	TMockLtsyData2 <TInfoName, TInt > ltsyData(contextId, completeRate);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	TUint rate;
	// send request
	packetContext.NotifyConnectionSpeedChange(requestStatus, rate);
	
	// send completion 
	iMockLTSY.CompleteL (EPacketContextNotifyConnectionSpeedChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(completeRate, (TInt)rate);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::NotifyConnectionSpeedChange
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// send completion 
	iMockLTSY.CompleteL (EPacketContextNotifyConnectionSpeedChange, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //packetContext, packetService, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::NotifyConnectionSpeedChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::NotifyConnectionSpeedChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConnectionSpeedChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);	
	CleanupClosePushL(packetContext);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::NotifyConnectionSpeedChange
 	//-------------------------------------------------------------------------
 	
	// data for CompleteL
	TInt completeRate = 9600;
	TMockLtsyData2 <TInfoName, TInt > ltsyData(contextId, completeRate);
	ltsyData.SerialiseL(data);	
	
	TRequestStatus requestStatus;
	TUint rate;
	// send request
	packetContext.NotifyConnectionSpeedChange(requestStatus, rate);
	packetContext.CancelAsyncRequest(EPacketContextNotifyConnectionSpeedChange);
	
	// send completion 
	iMockLTSY.CompleteL (EPacketContextNotifyConnectionSpeedChange, KErrNone, data);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());	

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);	
	AssertMockLtsyStatusL();		
	CleanupStack::PopAndDestroy(4, this); //packetContext, packetService, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::NotifyConnectionSpeedChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::NotifyConnectionSpeedChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConnectionSpeedChange0004L()
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::NotifyConnectionSpeedChange
 	//-------------------------------------------------------------------------
	TUint rate;	
	TRequestStatus requestStatus;	
	TRequestStatus requestStatus2;
	TUint rate2;
	
		
	// data for CompleteL 1
	TInt completeRate = 9600;
	TMockLtsyData2 <TInfoName, TInt > ltsyData(contextId, completeRate);
	ltsyData.SerialiseL(data);

	// data for CompleteL 2
	TInt completeRate2 = 4800;
	TMockLtsyData2 <TInfoName, TInt > ltsyData2(contextId2, completeRate2);
	ltsyData2.SerialiseL(data2);
	
	// send first request	
	packetContext.NotifyConnectionSpeedChange(requestStatus, rate);
	
	// send second request	
	packetContext2.NotifyConnectionSpeedChange(requestStatus2, rate2);
	
	// sent first completion
	iMockLTSY.CompleteL (EPacketContextNotifyConnectionSpeedChange, KErrNone, data);
	
	// sent second completion
	iMockLTSY.CompleteL (EPacketContextNotifyConnectionSpeedChange, KErrNone, data2);	
	
	// wait for first answer
	User::WaitForRequest(requestStatus);
		
	// wait for second answer
	User::WaitForRequest(requestStatus2);
	
	AssertMockLtsyStatusL();		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_EQUALS(completeRate, (TInt)rate);
	ASSERT_EQUALS(completeRate2, (TInt)rate2);
	
	CleanupStack::PopAndDestroy(9, this); //packetContext2, packetContext, packetService2, packetService, phone2, telServer2, data,data2 this

	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCDL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Delete
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDelete0001L()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);	
	CleanupClosePushL(packetContext);
	
	
	//-------------------------------------------------------------------------
	// SEND Delete request when ContextStatus is Unknown
 	//-------------------------------------------------------------------------
	TRequestStatus requestStatus;	
	packetContext.Delete(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());	
	AssertMockLtsyStatusL();
	
	
 	// Set context in Active state
 	SetContextActiveL(contextId);
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::Delete
	// from LTSY.
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
	TRequestStatus mockLtsyStatus;
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	// send completion 
	iMockLTSY.CompleteL(EPacketContextDelete, KErrNone, data);
	// wait for completion
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	iMockLTSY.ExpectL(EPacketContextDelete, expectData, KErrNotSupported);

	packetContext.Delete(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL (EPacketContextDelete, expectData);
	iMockLTSY.CompleteL(EPacketContextDelete, KErrGeneral, data);

	packetContext.Delete(requestStatus);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::Delete when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL (EPacketContextDelete, expectData);
	iMockLTSY.CompleteL(EPacketContextDelete, KErrNone, data);

	packetContext.Delete(requestStatus);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: failure to modify context parameters when context deleted
 	//-------------------------------------------------------------------------
	packetContext.ModifyActiveContext(requestStatus);
	User::WaitForRequest(requestStatus);		
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());

	CleanupStack::PopAndDestroy(5, this); //packetContext, packetService, expectData, data, this	
	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCDL-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::Delete
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::Delete
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDelete0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);

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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);
	
	// Set first context in Active state
 	SetContextActiveL(contextId);
 	
 	// Set second context in Active state
 	SetContextActiveL(contextId2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::Delete
 	//-------------------------------------------------------------------------
	
 	// data for first CompleteL
 	TMockLtsyData1 <TInfoName > ltsyData(contextId);
	ltsyData.SerialiseL(data);
 	
	// data for second CompleteL
 	TMockLtsyData1 <TInfoName > ltsyData2(contextId2);
	ltsyData2.SerialiseL(data2);	
	
	// for the first request
	TRequestStatus requestStatus;
	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	// for the second request
	TRequestStatus requestStatus2;
	TMockLtsyData1 <TInfoName> expData2(contextId2);
	expData2.SerialiseL(expectData2);
	
		
	iMockLTSY.ExpectL(EPacketContextDelete, expectData);	 	
 	iMockLTSY.CompleteL(EPacketContextDelete, KErrNone, data);
	iMockLTSY.ExpectL(EPacketContextDelete, expectData2); 	 
	iMockLTSY.CompleteL(EPacketContextDelete, KErrNone, data2);
	
	// send first request
	packetContext.Delete(requestStatus);		
	
	// send second request
	packetContext2.Delete(requestStatus2);
	
	// wait for first answer
	User::WaitForRequest(requestStatus);		
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	// wait for second answer
	User::WaitForRequest(requestStatus2);		
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // expectData2, expectData
	CleanupStack::PopAndDestroy(9, this); //packetContext2, packetContext, packetService2, packetService, phone2, telServer2, data,data2 this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCDL-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Delete with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Delete and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestDelete0005L()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;	
	packetContext.OpenNewContext(packetService, contextId);	
	CleanupClosePushL(packetContext);
	
	// Set context in Active state
 	SetContextActiveL(contextId);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::Delete
 	//------------------------------------------------------------------------- 	
 	TRequestStatus requestStatus; 	
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	iMockLTSY.ExpectL (EPacketContextDelete, expectData);

	packetContext.Delete(requestStatus);
		
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	
	AssertMockLtsyStatusL();
	 	
	CleanupStack::PopAndDestroy(5, this); //packetContext, packetService, data, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Activate
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Activate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestActivate0001L()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	TRequestStatus requestStatus;
	iMockLTSY.ExpectL(EPacketContextActivate, expectData, KErrNotSupported);
	
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	RPacketContext::TContextConfigGPRS contextConfig;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigGPRS > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrGeneral, data);
	
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::Activate when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
		
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
 
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::Activate
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(3, this); // expectData, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCA-0001b
@SYMComponent  telephony_ctsy

@SYMDEF DEF126915
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Activate with R99/R4 params
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Activate with R99/R4 context config params
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestActivate0001bL()
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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::Activate, using TContextConfigR99_R4, when result is 
	// not cached.
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	TRequestStatus requestStatus;
	
	RPacketContext::TContextConfigR99_R4 contextConfig;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigR99_R4 > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);
	
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
	
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(3, this); // expectData, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::Activate
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::Activate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestActivate0002L()
	{

// This test should test cancellation of Activate
// If this API does not have a cancel, the test step should be completely removed.

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
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
		
 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::Activate
 	//------------------------------------------------------------------------- 
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
 
   	TRequestStatus	requestStatus;	
 	RPacketContext::TContextConfigGPRS contextConfig;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigGPRS > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data, 10);
	
	packetContext.Activate(requestStatus);
	packetContext.CancelAsyncRequest(EPacketContextActivate);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	// TSY has started a request and it is not possible to then cancel 
    // this request. The best thing for the TSY to do in this case is to
    // proceed as though the Cancel never happened.
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(3); //  this, expectData, data	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCA-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Activate with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Activate with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestActivate0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();


	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// -----------------------------------------------------------------------
	// call Activate without initialisation of the context
	// -----------------------------------------------------------------------
	TRequestStatus	requestStatus;
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int()); 	
 	
	CleanupStack::PopAndDestroy(3, this); //packetContext, packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::Activate
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::Activate
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestActivate0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	// Open PacketService for the first client
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open PacketService for the second client
	RPacketService packetService2;
	packetService2.Open(phone2);
	CleanupClosePushL(packetService2);

	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the first context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);
	
	// initialisation of the second context 
	status = InitContextL(packetContext2, contextId2);
	ASSERT_EQUALS(KErrNone, status);
			
 	// data for ExpectL for first client
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	// data for CompleteL for first client
	RPacketContext::TContextConfigGPRS contextConfig;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigGPRS > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);	
	
	// data for ExpectL for second client
 	TMockLtsyData1 <TInfoName> expData2(contextId2);
	expData2.SerialiseL(expectData2);
	
	// data for CompleteL for second client
	RPacketContext::TContextConfigGPRS contextConfig2;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigGPRS > ltsyData2(contextId2, contextConfig2);
	ltsyData2.SerialiseL(data2);
	
	// first request
	TRequestStatus	requestStatus;
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);		
	packetContext.Activate(requestStatus);
	
	// second request
	TRequestStatus	requestStatus2;
	iMockLTSY.ExpectL(EPacketContextActivate, expectData2);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data2);		
	packetContext2.Activate(requestStatus2);	
	
	// wait for first answer
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());		
	
	// wait for the second answer
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());		
	
	CleanupStack::PopAndDestroy(11, this); // ... phone2, telServer2, expectData, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCA-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::Activate with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Activate and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestActivate0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);		
 
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::Activate
 	//-------------------------------------------------------------------------
 	TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
 	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
 	
 	TRequestStatus	requestStatus;
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int()); 	

	CleanupStack::PopAndDestroy(5, this); // data, expectData, packetContext, packetService, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetConfig
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConfig0001L()
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
		
	// Open new context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);				
 	
 	//-------------------------------------------------------------------------
	// TEST C1: Successful completion request of
	// RPacketContext::GetConfig with  RPacketContext::TContextConfigGPRS param
 	//-------------------------------------------------------------------------
	TRequestStatus	requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig); 
	
	packetContext.GetConfig(requestStatus, pckgConfig);
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	TBuf8<50> null50; 	
	TBuf8<252> nullName; 
	ASSERT_EQUALS(nullName, 					contextConfig.iAccessPointName);
	ASSERT_EQUALS(RPacketContext::ENotRequired, contextConfig.iAnonymousAccessReqd);
	ASSERT_EQUALS(0, 							contextConfig.iNWIContext);
	ASSERT_EQUALS(null50, 						contextConfig.iPdpAddress );
	ASSERT_EQUALS((TUint)0,						contextConfig.iPdpCompression);
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, contextConfig.iPdpType );	
	ASSERT_EQUALS(0, 							contextConfig.iUseEdge);			
	ASSERT_EQUALS(RPacketContext::EProtocolNone,contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(null50, 		contextConfig.iProtocolConfigOption.iAuthInfo.iUsername);
	ASSERT_EQUALS(null50, 		contextConfig.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(null50,		contextConfig.iProtocolConfigOption.iChallenge);
	ASSERT_EQUALS(null50, 		contextConfig.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS((TUint8)0, 	contextConfig.iProtocolConfigOption.iId);
	ASSERT_EQUALS(null50, 		contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(null50, 		contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	
	
	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RPacketContext::GetConfig with  RPacketContext::TContextConfigR99_R4 param
 	//-------------------------------------------------------------------------
	RPacketContext::TContextConfigR99_R4 contextConfig2;
	TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig2(contextConfig2); 
	
	packetContext.GetConfig(requestStatus, pckgConfig2);
	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();			
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 		
	ASSERT_EQUALS(nullName, 					contextConfig2.iAccessPointName);
	ASSERT_EQUALS(0, 							contextConfig2.iNWIContext);
	ASSERT_EQUALS(RPacketContext::EBestEffort, 	contextConfig2.iPFI);
	ASSERT_EQUALS(null50, 						contextConfig2.iPdpAddress);
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, contextConfig2.iPdpType );
	ASSERT_EQUALS(0, 							contextConfig2.iUseEdge);
	ASSERT_EQUALS(RPacketContext::EProtocolNone,contextConfig2.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(null50, 		contextConfig2.iProtocolConfigOption.iAuthInfo.iUsername);
	ASSERT_EQUALS(null50, 		contextConfig2.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(null50, 		contextConfig2.iProtocolConfigOption.iChallenge);
	ASSERT_EQUALS(null50, 		contextConfig2.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS((TUint8)0, 	contextConfig2.iProtocolConfigOption.iId);
	ASSERT_EQUALS(null50, 		contextConfig2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(null50, 		contextConfig2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	
	//-------------------------------------------------------------------------
	// TEST D: RPacketContext::GetConfig again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
 	
 	// initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);		
 
	//value to Config...
	RPacketContext::TContextConfigGPRS setConfig;       
    _LIT(KPointName, "Some Name");
    setConfig.iAccessPointName.Copy(KPointName);
	setConfig.iAnonymousAccessReqd = RPacketContext::ERequired;
	setConfig.iNWIContext = ETrue;	
	_LIT(KPdpAdd, "Some Add");
    setConfig.iPdpAddress.Copy(KPdpAdd);	
	setConfig.iPdpCompression = RPacketContext::KPdpDataCompression;	
	setConfig.iPdpType = RPacketContext::EPdpTypeIPv6;	
	setConfig.iUseEdge = ETrue;		
	setConfig.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	_LIT(KUsername, "Some Username");
	setConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	_LIT(KPassword, "Some Password");
	setConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	_LIT(KChallenge, "Challenge");
	setConfig.iProtocolConfigOption.iChallenge.Copy(KChallenge);
	_LIT(KResponse, "Response");
	setConfig.iProtocolConfigOption.iResponse.Copy(KResponse);
	setConfig.iProtocolConfigOption.iId = 10;
	_LIT(KPrimaryDns, "Some PrimaryDns");
	setConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	_LIT(KSecondaryDns, "Some SecondaryDns");
	setConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);	
	TPckg<RPacketContext::TContextConfigGPRS> pckgSet(setConfig);
	
	// Set new value to Config
	TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);    
    TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);    
    packetContext.SetConfig(requestStatus, pckgSet);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    // Get new values
    packetContext.GetConfig(requestStatus, pckgConfig);
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(setConfig.iAccessPointName, 		contextConfig.iAccessPointName);
	ASSERT_EQUALS(setConfig.iAnonymousAccessReqd, 	contextConfig.iAnonymousAccessReqd);
	ASSERT_EQUALS(setConfig.iNWIContext, 			contextConfig.iNWIContext);
	ASSERT_EQUALS(setConfig.iPdpAddress, 			contextConfig.iPdpAddress );
	ASSERT_EQUALS(setConfig.iPdpCompression, 		contextConfig.iPdpCompression);
	ASSERT_EQUALS(setConfig.iPdpType,				contextConfig.iPdpType );	
	ASSERT_EQUALS(setConfig.iUseEdge, 				contextConfig.iUseEdge);			
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iAuthInfo.iProtocol,			contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iAuthInfo.iUsername, 			contextConfig.iProtocolConfigOption.iAuthInfo.iUsername);
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iAuthInfo.iPassword, 			contextConfig.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iChallenge, 					contextConfig.iProtocolConfigOption.iChallenge);
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iResponse, 					contextConfig.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iId, 						 	contextConfig.iProtocolConfigOption.iId);
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns, 	contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(setConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns, 	contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);	
	
	CleanupStack::PopAndDestroy(5, this); // expectData, data, packetContext, packetService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetConfig with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetConfig with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConfig0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

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
	// RPacketContext::GetConfig
 	//-------------------------------------------------------------------------
	TRequestStatus	requestStatus;
	RPacketContext::TContextConfigCDMA contextConfig;
	TPckg<RPacketContext::TContextConfigCDMA> pckgConfig(contextConfig); 
	
	packetContext.GetConfig(requestStatus, pckgConfig);
	
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int()); 	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::GetConfig
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::GetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConfig0004L()
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
	
	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	packetContext2.OpenNewContext(packetService2, contextId2);
	CleanupClosePushL(packetContext2);
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::GetConfig
 	//-------------------------------------------------------------------------
	
	TRequestStatus	requestStatus;
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig); 
	
	TRequestStatus	requestStatus2;
	RPacketContext::TContextConfigR99_R4 contextConfig2;
	TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig2(contextConfig2); 
	
	// send first request
	packetContext.GetConfig(requestStatus, pckgConfig);
	// send second request
	packetContext2.GetConfig(requestStatus2, pckgConfig2);
	
	// wait for the first answer
	User::WaitForRequest(requestStatus);	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	TBuf8<50> null50; 	
	TBuf8<252> nullName; 
	ASSERT_EQUALS(nullName, contextConfig.iAccessPointName);
	ASSERT_EQUALS(RPacketContext::ENotRequired, contextConfig.iAnonymousAccessReqd);
	ASSERT_EQUALS(0, contextConfig.iNWIContext);
	ASSERT_EQUALS(null50, contextConfig.iPdpAddress );
	ASSERT_EQUALS((TUint)0, contextConfig.iPdpCompression);
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, contextConfig.iPdpType );	
	ASSERT_EQUALS(0, contextConfig.iUseEdge);			
	ASSERT_EQUALS(RPacketContext::EProtocolNone, contextConfig.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(null50, contextConfig.iProtocolConfigOption.iAuthInfo.iUsername);
	ASSERT_EQUALS(null50, contextConfig.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(null50, contextConfig.iProtocolConfigOption.iChallenge);
	ASSERT_EQUALS(null50, contextConfig.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS((TUint8)0, contextConfig.iProtocolConfigOption.iId);
	ASSERT_EQUALS(null50, contextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(null50, contextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	
	// wait for the second answer
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 		
	ASSERT_EQUALS(nullName, contextConfig2.iAccessPointName);
	ASSERT_EQUALS(0, contextConfig2.iNWIContext);
	ASSERT_EQUALS(RPacketContext::EBestEffort, contextConfig2.iPFI);
	ASSERT_EQUALS(null50, contextConfig2.iPdpAddress);
	ASSERT_EQUALS(RPacketContext::EPdpTypeIPv4, contextConfig2.iPdpType );
	ASSERT_EQUALS(0, contextConfig2.iUseEdge);
	ASSERT_EQUALS(RPacketContext::EProtocolNone, contextConfig2.iProtocolConfigOption.iAuthInfo.iProtocol);	
	ASSERT_EQUALS(null50, contextConfig2.iProtocolConfigOption.iAuthInfo.iUsername);
	ASSERT_EQUALS(null50, contextConfig2.iProtocolConfigOption.iAuthInfo.iPassword);
	ASSERT_EQUALS(null50, contextConfig2.iProtocolConfigOption.iChallenge);
	ASSERT_EQUALS(null50, contextConfig2.iProtocolConfigOption.iResponse);	
	ASSERT_EQUALS((TUint8)0, contextConfig2.iProtocolConfigOption.iId);
	ASSERT_EQUALS(null50, contextConfig2.iProtocolConfigOption.iDnsAddresses.iPrimaryDns);
	ASSERT_EQUALS(null50, contextConfig2.iProtocolConfigOption.iDnsAddresses.iSecondaryDns);
	
	CleanupStack::PopAndDestroy(7, this); //packetContext2, packetContext, packetService2, packetService, phone2, telServer2, this
	}



/**
@SYMTestCaseID BA-CTSY-PKTC-PCGS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetStatus
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetStatus0001L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);

    // test 1: context not inited
    RPacketContext::TContextStatus contextStatus;    
    packetContext.GetStatus(contextStatus);
    ASSERT_EQUALS(RPacketContext::EStatusUnknown, contextStatus);
    
    // initialisation of the context 
	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);
		
    // test2: context inited
    packetContext.GetStatus(contextStatus);
    ASSERT_EQUALS(RPacketContext::EStatusInactive, contextStatus);
    

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2); //packetContext, packetService
    CleanupStack::PopAndDestroy(2, this); // data, this 
    }


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::GetStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::GetStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
/*void CCTsyPacketContextFU::TestGetStatus0002L()
    {

// This test should test cancellation of GetStatus
// If this API does not have a cancel, the test step should be completely removed.

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test cancelling of RPacketContext::GetStatus
    //-------------------------------------------------------------------------
    
    // Remove the test above if this is not relevant for this API

    // Wait for completion of iMockLTSY.NotifyTerminated
    // User::WaitForRequest(mockLtsyStatus);
    CleanupStack::PopAndDestroy(2); // data, this
    
    }*/


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetStatus with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetStatus with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
/*void CCTsyPacketContextFU::TestGetStatus0003L()
    {

// This test should test sending bad parameter data for GetStatus
// If this API does not have any parameters, then remove this test completely.

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test A: Test passing wrong version of parameters to
    // RPacketContext::GetStatus
    //-------------------------------------------------------------------------

    // Remove the test above if it is not relevant for this API

    //-------------------------------------------------------------------------
    // Test B: Test passing wrong descriptor size to parameter in
    // RPacketContext::GetStatus
    //-------------------------------------------------------------------------

    // Remove the test above if it is not relevant for this API
    
    //-------------------------------------------------------------------------
    // Test C: Test passing out of bounds parameters to
    // RPacketContext::GetStatus
    //-------------------------------------------------------------------------
    
    // Remove the test above if it is not relevant for this API

    // Done !
    CleanupStack::PopAndDestroy(2, this); // data, this

    }*/


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::GetStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::GetStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
/*void CCTsyPacketContextFU::TestGetStatus0004L()
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
    // Test A: Test multiple clients requesting RPacketContext::GetStatus
    //-------------------------------------------------------------------------

    // If this API is flow controlled by Etel or if this API is
    // synchronous, remove this test completely.

    // To write this test:
    // If this API has parameters which have different versions, then each
    // client in this test should use a different version of parameter.
    // Otherwise, they should use the same version.
    // All versions of parameters should be used. i.e. If there are three
    // versions of a parameter, there should be three clients, one using
    // each one. If the parameter does not have different versions, use
    // two clients each using the same version.


    // Done !
    CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

    }*/


/**
@SYMTestCaseID BA-CTSY-PKTC-PCGS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
/*void CCTsyPacketContextFU::TestGetStatus0005L()
    {


    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    //-------------------------------------------------------------------------
    // Test A: Test timeout of RPacketContext::GetStatus
    //-------------------------------------------------------------------------

    // If this API does not have a timeout, then remove this test completely.

    // Done !
    CleanupStack::PopAndDestroy(2, this); // data, this

    }*/

 /**
@SYMTestCaseID BA-CTSY-PKTC-PCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::SetConfig
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::SetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestSetConfig0001L()
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
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // initialisation of the context 
 	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);  
 
	// data for SetContext
    RPacketContext::TContextConfigGPRS setConfig;
     _LIT(KPointName, "Some Name");
    setConfig.iAccessPointName.Copy(KPointName);
	setConfig.iAnonymousAccessReqd = RPacketContext::ERequired;
	setConfig.iNWIContext = ETrue;	
	_LIT(KPdpAdd, "Some Add");
    setConfig.iPdpAddress.Copy(KPdpAdd);	
	setConfig.iPdpCompression = RPacketContext::KPdpDataCompression;	
	setConfig.iPdpType = RPacketContext::EPdpTypeIPv6;	
	setConfig.iUseEdge = ETrue;		
	setConfig.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	_LIT(KUsername, "Some Username");
	setConfig.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername);
	_LIT(KPassword, "Some Password");
	setConfig.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword);
	_LIT(KChallenge, "Challenge");
	setConfig.iProtocolConfigOption.iChallenge.Copy(KChallenge);
	_LIT(KResponse, "Response");
	setConfig.iProtocolConfigOption.iResponse.Copy(KResponse);
	setConfig.iProtocolConfigOption.iId = 10;
	_LIT(KPrimaryDns, "Some PrimaryDns");
	setConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns);
	_LIT(KSecondaryDns, "Some SecondaryDns");
	setConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns);    
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(setConfig);    
    
    TRequestStatus requestStatus;    

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------
	TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData, KErrNotSupported);

    packetContext.SetConfig(requestStatus, pckgConfig);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());


    //-------------------------------------------------------------------------
    // TEST B: failure on completion of pending request from LTSY->CTSY
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);

    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
    
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrGeneral, data);
    packetContext.SetConfig(requestStatus, pckgConfig);
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // RPacketContext::SetConfig when result is not cached.
    //-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    packetContext.SetConfig(requestStatus, pckgConfig);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
       
    //--------------------------------------------
    // Successful completion for R99 
    //--------------------------------------------     
    RPacketContext::TContextConfigR99_R4 setConfig99;    
     _LIT(KPointName99, "Some Name99");
    setConfig99.iAccessPointName.Copy(KPointName99);
	setConfig99.iNWIContext = ETrue;	
	_LIT(KPdpAdd99, "Some Add99");
    setConfig99.iPdpAddress.Copy(KPdpAdd99);		
	setConfig99.iPdpType = RPacketContext::EPdpTypeIPv6;			
	setConfig99.iProtocolConfigOption.iAuthInfo.iProtocol = RPacketContext::EProtocolCHAP;
	_LIT(KUsername99, "Some Username99");
	setConfig99.iProtocolConfigOption.iAuthInfo.iUsername.Copy(KUsername99);
	_LIT(KPassword99, "Some Password99");
	setConfig99.iProtocolConfigOption.iAuthInfo.iPassword.Copy(KPassword99);
	_LIT(KChallenge99, "Challenge99");
	setConfig99.iProtocolConfigOption.iChallenge.Copy(KChallenge99);
	_LIT(KResponse99, "Response99");
	setConfig99.iProtocolConfigOption.iResponse.Copy(KResponse99);
	setConfig99.iProtocolConfigOption.iId = 10;
	_LIT(KPrimaryDns99, "Some PrimaryDns99");
	setConfig99.iProtocolConfigOption.iDnsAddresses.iPrimaryDns.Copy(KPrimaryDns99);
	_LIT(KSecondaryDns99, "Some SecondaryDns99");
	setConfig99.iProtocolConfigOption.iDnsAddresses.iSecondaryDns.Copy(KSecondaryDns99); 	
	setConfig99.iPFI = RPacketContext::ESignalling;	   
    TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig3(setConfig99);    

 	TMockLtsyData2<RPacketContext::TContextConfigR99_R4, TInfoName > expExpect4(setConfig99, contextId);
    expectData.Close();
    expExpect4.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    packetContext.SetConfig(requestStatus, pckgConfig3);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    //--------------------------------------------
    // failure to dispatch request for R99 
    //--------------------------------------------    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData, KErrNotSupported);
    //iMockLTSY.CompleteL(EPacketContextSetConfig, KErrGeneral, data);

    packetContext.SetConfig(requestStatus, pckgConfig3);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    AssertMockLtsyStatusL();
        
    //-------------------------------------------------------------------------
    // TEST E: Unsolicited completion of RPacketContext::SetConfig
    // from LTSY.
    //-------------------------------------------------------------------------
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data, 0);
    User::WaitForRequest(mockLtsyStatus);        
    
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(2); //packetContext, packetService
    CleanupStack::PopAndDestroy(3, this); // data, expectData, this    
    }


/**
@SYMTestCaseID BA-CTSY-PKTC-PCSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::SetConfig
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::SetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestSetConfig0002L()
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
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // initialisation of the context 
 	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status); 
	
	// data for SetConfig
    RPacketContext::TContextConfigGPRS contextConfig;
    _LIT(KPdpAdd, "Some Add");
    contextConfig.iPdpAddress.Copy(KPdpAdd);
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);    
    
    // data for ExpectL
    TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(contextConfig, contextId);
    expExpect.SerialiseL(expectData);
    
    //data for CompleteL    
    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);    

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    
    TRequestStatus requestStatus;
    //-------------------------------------------------------------------------
    // Test cancelling of RPacketContext::SetConfig
    //-------------------------------------------------------------------------    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data, 20);

    packetContext.SetConfig(requestStatus, pckgConfig);
	
    packetContext.CancelAsyncRequest(EPacketContextSetConfig);

    User::WaitForRequest(requestStatus);    
    AssertMockLtsyStatusL();    
    // TSY has started a request and it is not possible to then cancel 
    // this request. (see CMmPacketContextTsy.cpp line 483)
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
     
    CleanupStack::PopAndDestroy(2);
    CleanupStack::PopAndDestroy(3, this); // expectData, data, this    
    }


/**
@SYMTestCaseID BA-CTSY-PKTC-PCSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::SetConfig with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::SetConfig with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestSetConfig0003L()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // initialisation of the context 
	 TInt status = InitContextL(packetContext, contextId);
	 ASSERT_EQUALS(KErrNone, status);  
 
    //-------------------------------------------------------------------------
    // Test A: Test passing wrong version of parameters to
    // RPacketContext::SetConfig
    //-------------------------------------------------------------------------

    RPacketContext::TContextConfigCDMA contextConfig;
    TPckg<RPacketContext::TContextConfigCDMA> pckgConfig(contextConfig);    

    TRequestStatus requestStatus;
        
    packetContext.SetConfig(requestStatus, pckgConfig);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    AssertMockLtsyStatusL();
    

    //------------------------------------------------
    //set config for context that was not inited, using GPRS
    //------------------------------------------------
    TInfoName contextId2;  
    RPacketContext packetContext2;
    packetContext2.OpenNewContext(packetService, contextId2);
    CleanupClosePushL(packetContext2);
    

    RPacketContext::TContextConfigGPRS contextConfig2;
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig2(contextConfig2);    
    
    packetContext2.SetConfig(requestStatus, pckgConfig2);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
    AssertMockLtsyStatusL();
    

    //------------------------------------------------
    //set config for context that was not inited, using R99_R4
    //------------------------------------------------
    RPacketContext::TContextConfigR99_R4 contextConfig3;
    TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig3(contextConfig3);    

    packetContext2.SetConfig(requestStatus, pckgConfig3);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
    AssertMockLtsyStatusL();

    CleanupStack::PopAndDestroy(1); //packetContext2
    
    CleanupStack::PopAndDestroy(2); //packetContext packetService
    CleanupStack::PopAndDestroy(2, this); // data, this

    }


/**
@SYMTestCaseID BA-CTSY-PKTC-PCSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::SetConfig
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::SetConfig
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestSetConfig0004L()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 data2;
    CleanupClosePushL(data2);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);
    
    RBuf8 expectData2;
    CleanupClosePushL(expectData2);

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);

    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);


    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);

    RPacketService packetService2;                
    ret = packetService2.Open(phone2);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(packetService2);
    
    // Open first context
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // Open second context
    TInfoName contextId2; 
    RPacketContext packetContext2;
    packetContext2.OpenNewContext(packetService2, contextId2);
    CleanupClosePushL(packetContext2);


    // initialisation of the first context 
 	TInt status = InitContextL(packetContext, contextId);
 	ASSERT_EQUALS(KErrNone, status);
  
 	// initialisation of the second context 
	 status = InitContextL(packetContext2, contextId2);
 	ASSERT_EQUALS(KErrNone, status);
    
    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);


    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting RPacketContext::SetConfig
    //-------------------------------------------------------------------------    
    RPacketContext::TContextConfigGPRS contextConfig;
    _LIT(KPdpAdd, "Some Add");
    contextConfig.iPdpAddress.Copy(KPdpAdd);
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);
    // for the first ExpectL    
    TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(contextConfig, contextId);
    expExpect.SerialiseL(expectData);
    
    RPacketContext::TContextConfigR99_R4 contextConfig2;
    TPckg<RPacketContext::TContextConfigR99_R4> pckgConfig2(contextConfig2);
    // for the second ExpectL
    TMockLtsyData2<RPacketContext::TContextConfigR99_R4, TInfoName > expExpect2(contextConfig2, contextId2);
    expExpect2.SerialiseL(expectData2);
    
    // for the first ComleteL
    TMockLtsyData1< TInfoName > ltsyData(contextId);
    ltsyData.SerialiseL(data);
    
    // for the second ComleteL
    TMockLtsyData1< TInfoName > ltsyData2(contextId2);
    ltsyData2.SerialiseL(data2);    

    TRequestStatus  requestStatus;    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data, 10);

    TRequestStatus  requestStatus2;    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData2);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data2, 10);

    packetContext.SetConfig(requestStatus, pckgConfig);
    packetContext2.SetConfig(requestStatus2, pckgConfig2);
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());

    User::WaitForRequest(mockLtsyStatus);

    // packetContext, packetService, packetContext2, packetService2
    //one2, telServer2, data2,
    CleanupStack::PopAndDestroy(2); //expectData2, expectData
    CleanupStack::PopAndDestroy(9, this); // data, this

    }


/**
@SYMTestCaseID BA-CTSY-PKTC-PCSC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::SetConfig with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::SetConfig and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestSetConfig0005L()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RBuf8 expectData;
    CleanupClosePushL(expectData);

    //-------------------------------------------------------------------------
    // Test A: Test timeout of RPacketContext::SetConfig
    //-------------------------------------------------------------------------

    RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // initialisation of the first context 
    TInt status = InitContextL(packetContext, contextId);
    ASSERT_EQUALS(KErrNone, status);

    RPacketContext::TContextConfigGPRS contextConfig;
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(contextConfig);    
    TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(contextConfig, contextId);
    expExpect.SerialiseL(expectData);

    TRequestStatus requestStatus;
    
    TMockLtsyData1<TInfoName > contexName(contextId);
    contexName.SerialiseL(data);
    
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    
    packetContext.SetConfig(requestStatus, pckgConfig);

    User::WaitForRequest(requestStatus);
    
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());
    AssertMockLtsyStatusL();
    
    
    CleanupStack::PopAndDestroy(2); //packetContext packetService, 
    CleanupStack::PopAndDestroy(3, this); // data, expectData, this

    }


#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

/**
@SYMTestCaseID BA-CTSY-PKTC-PCGCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetConnectionInfo
@SYMTestPriority High
@SYMTestActions Invokes requests to RPacketContext::GetConnectionInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConnectionInfo0001L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	TRequestStatus mockLtsyStatus;	
    TRequestStatus requestStatus;
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;
	
 	//-------------------------------------------------------------------------
	// TEST A1: Successful completion of RPacketContext::GetConnectionInfo
	// request.
 	//-------------------------------------------------------------------------
 	
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1(reqConnectionInfoV1); 
	    	
	//Get connection info
	packetContext.GetConnectionInfo(requestStatus, pckgReqConnectionInfoV1);	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS((TUint32)0,                                     reqConnectionInfoV1.iValid);

 	//-------------------------------------------------------------------------
	// TEST A2: Successful completion of RPacketContext::GetConnectionInfo
	// request where RPacketContext::TConnectionInfoV1 parameter contains 
	// valid data.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory;
	respConnectionInfoV1.iHSDPACategory = 5;
	respConnectionInfoV1.iValid |= RPacketContext::KHSUPACategory;
	respConnectionInfoV1.iHSUPACategory = 6;	    	
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData(contextId, respConnectionInfoV1);
	ltsyData.SerialiseL(data);
	
	// Change connection information
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//Get connection info
	packetContext.GetConnectionInfo(requestStatus, pckgReqConnectionInfoV1);
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1.iHSUPACategory);
	
    CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data, this	
    }

/**
@SYMTestCaseID BA-CTSY-PKTC-PCGCI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for canceling RPacketContext::GetConnectionInfo 
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetConnectionInfo cancel request
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConnectionInfo0002L()
	{					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	TRequestStatus mockLtsyStatus;	
    TRequestStatus requestStatus;
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;
	
 	//-------------------------------------------------------------------------
	// TEST A1: Cancel RPacketContext::GetConnectionInfo request 
	// successfully.
 	//-------------------------------------------------------------------------
 	
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1(reqConnectionInfoV1); 
	    	
	//Get connection info
	packetContext.GetConnectionInfo(requestStatus, pckgReqConnectionInfoV1);
	
	//Cancel connection info request
	packetContext.CancelAsyncRequest(EPacketContextGetConnectionInfo);	
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	//requestStatus should be KErrNone if CTSY completed GetConnectionInfo() 
	//request before cancel request arrived. requestStatus should be KErrCancel 
	//if CTSY received cancel request before before GetConnectionInfo() request
	//was completed.
	if ( !( requestStatus.Int() == KErrNone ||  
	        requestStatus.Int() == KErrCancel ) )
		{
		//Wrong error value. Check fails here.
		ASSERT_EQUALS(KErrNone, requestStatus.Int());	
		}
	
    CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this	
    }

/**
@SYMTestCaseID BA-CTSY-PKTC-PCGCI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::GetConnectionInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::GetConnectionInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestGetConnectionInfo0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	TRequestStatus mockLtsyStatus;	
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1_1;
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1_2;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;
	
	//-------------------------------------------------------------------------
	// TEST A1: Successful completion of RPacketContext::GetConnectionInfo
	// request.
 	//-------------------------------------------------------------------------
 	
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1_1(reqConnectionInfoV1_1);
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1_2(reqConnectionInfoV1_2);
	    	
	//Get connection info
	packetContext.GetConnectionInfo(requestStatus1, pckgReqConnectionInfoV1_1);
	packetContext.GetConnectionInfo(requestStatus2, pckgReqConnectionInfoV1_2);
		
	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);	
	AssertMockLtsyStatusL();
	
	//Check results
	ASSERT_EQUALS(KErrNone, requestStatus1.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1_1.ExtensionId());
	ASSERT_EQUALS((TUint32)0,                                     reqConnectionInfoV1_1.iValid);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1_2.ExtensionId());
	ASSERT_EQUALS((TUint32)0,                                     reqConnectionInfoV1_2.iValid);
	
	//-------------------------------------------------------------------------
	// TEST A2: Successful completion of RPacketContext::GetConnectionInfo
	// request where RPacketContext::TConnectionInfoV1 parameter contains 
	// valid data.
 	//-------------------------------------------------------------------------
 	
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory;
	respConnectionInfoV1.iHSDPACategory = 5;
	respConnectionInfoV1.iValid |= RPacketContext::KHSUPACategory;
	respConnectionInfoV1.iHSUPACategory = 6;	    	
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData(contextId, respConnectionInfoV1);
	ltsyData.SerialiseL(data);
	
	// Change connection information
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	
 	//Get connection info
	packetContext.GetConnectionInfo(requestStatus1, pckgReqConnectionInfoV1_1);
	packetContext.GetConnectionInfo(requestStatus2, pckgReqConnectionInfoV1_2);
	
	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	
	//Check results
	ASSERT_EQUALS(KErrNone, requestStatus1.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1_1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1_1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_1.iHSUPACategory);
 	ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1_2.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_2.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1_2.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_2.iHSUPACategory);
	
	CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data
	}
	
/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::NotifyConnectionInfoChange
@SYMTestPriority High
@SYMTestActions Invokes requests to RPacketContext::NotifyConnectionInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConnectionInfoChange0001L()
	{
	INFO_PRINTF1(_L("Testing notification of change in TConnectionInfoV1.iValid .."));
	TestNotifyConnectionInfoChange0001_workerL(ETrue,EFalse,EFalse);
	INFO_PRINTF1(_L("Testing notification of change in TConnectionInfoV1.iHSDPACategory .."));
	TestNotifyConnectionInfoChange0001_workerL(EFalse,ETrue,EFalse);
	INFO_PRINTF1(_L("Testing notification of change in TConnectionInfoV1.iHSUPACategory .."));
	TestNotifyConnectionInfoChange0001_workerL(EFalse,EFalse,ETrue);
	INFO_PRINTF1(_L("Testing that no change leads to no notification .."));
	TestNotifyConnectionInfoChange0001_workerL(ETrue,ETrue,ETrue,ETrue);
	INFO_PRINTF1(_L(" All ok!"));
	}


void CCTsyPacketContextFU::TestNotifyConnectionInfoChange0001_workerL(TBool aChangeValid,TBool aChangeHSDPACategory,TBool aChangeHSUPACategory,TBool aResendFirst)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextId;
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
	
	TRequestStatus mockLtsyStatus;
    TRequestStatus requestStatus;
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;
	
 	//-------------------------------------------------------------------------
	// TEST A1: Successful completion of
	// RPacketContext::NotifyConnectionInfoChange request.
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1(reqConnectionInfoV1); 
	    	
	//Make NotifyConnectionInfoChange request
	packetContext.NotifyConnectionInfoChange(requestStatus, pckgReqConnectionInfoV1);

	//Send response via MockLTSY
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory;
	respConnectionInfoV1.iHSDPACategory = 1;
	respConnectionInfoV1.iHSUPACategory = 0;
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData1(contextId, respConnectionInfoV1);
	ltsyData1.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//NotifyConnectionInfoChange request is completed to test client
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,			reqConnectionInfoV1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,	reqConnectionInfoV1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,	reqConnectionInfoV1.iHSUPACategory);
	
	
	// special case to ensure no notification occurs when same data is received twice
	if(aResendFirst)
		{
		iMockLTSY.NotifyTerminated(mockLtsyStatus);
		iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
		User::WaitForRequest(mockLtsyStatus);
		ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());		
		}
	
 	//-------------------------------------------------------------------------
	// TEST A2: Successful completion of
	// RPacketContext::NotifyConnectionInfoChange request where
	// RPacketContext::TConnectionInfoV1 parameter data changes.
 	//-------------------------------------------------------------------------
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	//Make NotifyConnectionInfoChange request
	packetContext.NotifyConnectionInfoChange(requestStatus, pckgReqConnectionInfoV1);
	
	//Send response via MockLTSY
	respConnectionInfoV1.iValid = aChangeValid ? (RPacketContext::KHSDPACategory | RPacketContext::KHSUPACategory) : RPacketContext::KHSDPACategory ;
	respConnectionInfoV1.iHSDPACategory = aChangeHSDPACategory ? 5 : 1;
	respConnectionInfoV1.iHSUPACategory = aChangeHSUPACategory ? 6 : 0;
	
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData2(contextId, respConnectionInfoV1);
	data.Close();
	ltsyData2.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//NotifyConnectionInfoChange request is completed to test client
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1.iHSUPACategory);
	
    CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data, this
    }

/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancel RPacketContext::NotifyConnectionInfoChange request
@SYMTestPriority High
@SYMTestActions Invokes cancel RPacketContext::NotifyConnectionInfoChange request
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConnectionInfoChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);	
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextId;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextId);
	CleanupClosePushL(packetContext);
		
	TRequestStatus mockLtsyStatus;	
    TRequestStatus requestStatus;
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;
	
 	//-------------------------------------------------------------------------
	// TEST A1: RPacketContext::NotifyConnectionInfoChange request can be
	// cancelled successfully. 
 	//-------------------------------------------------------------------------	
 	
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1(reqConnectionInfoV1); 
	    	
	//Make NotifyConnectionInfoChange request
	packetContext.NotifyConnectionInfoChange(requestStatus, pckgReqConnectionInfoV1);

	User::After( 1000 );
	
	//Cancel NotifyConnectionInfoChange request	
	packetContext.CancelAsyncRequest(EPacketContextNotifyConnectionInfoChange);
	
	//NotifyConnectionInfoChange request is completed to test client	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrCancel, requestStatus.Int()); 
	
 	//-------------------------------------------------------------------------
	// TEST A2: RPacketContext::NotifyConnectionInfoChange request can be
	// made again after cancelled. 
 	//-------------------------------------------------------------------------		

	iMockLTSY.NotifyTerminated(mockLtsyStatus); 	
	    	
	//Send response via MockLTSY. Nothing should happen because 
	//NotifyConnectionInfoChange request has been cancelled.
	respConnectionInfoV1.iHSDPACategory = 2;
	respConnectionInfoV1.iHSUPACategory = 3;
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory | RPacketContext::KHSUPACategory;
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData(contextId, respConnectionInfoV1);
	ltsyData.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);	
		
	respConnectionInfoV1.iHSDPACategory = 3;
	respConnectionInfoV1.iHSUPACategory = 4;
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory | RPacketContext::KHSUPACategory;
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyDat2(contextId, respConnectionInfoV1);
	ltsyData.SerialiseL(data);
	
	//Make NotifyConnectionInfoChange request
	packetContext.NotifyConnectionInfoChange(requestStatus, pckgReqConnectionInfoV1);
	
	//Send response again via MockLTSY	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());		
	
	//NotifyConnectionInfoChange request is completed to test client	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,       			  reqConnectionInfoV1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,       	  reqConnectionInfoV1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,       	  reqConnectionInfoV1.iHSUPACategory);	

    CleanupStack::PopAndDestroy(4, this); // packetContext, packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCNCIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::NotifyConnectionInfoChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::NotifyConnectionInfoChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestNotifyConnectionInfoChange0004L()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	//Open new context
	RPacketService packetService1;
	OpenPacketServiceL(packetService1);
	CleanupClosePushL(packetService1);
	
	TInfoName contextId;	
	RPacketContext packetContext1;
	packetContext1.OpenNewContext(packetService1, contextId);
	CleanupClosePushL(packetContext1);	
	
	// Open second client and existing context
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);
	
	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);	
	
	RPacketContext packetContext2;
    ret = packetContext2.OpenExistingContext(packetService2, contextId);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);    		
	
	//Open third client and new context
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);
	
	RMobilePhone phone3;
	ret = phone3.Open(telServer3,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);	
	
	RPacketService packetService3;                
	ret = packetService3.Open(phone3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService3);	
	
	TInfoName contextId2;	
	RPacketContext packetContext3;
	packetContext3.OpenNewContext(packetService3, contextId2);
	CleanupClosePushL(packetContext3);	
	
	TRequestStatus mockLtsyStatus;	
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus requestStatus3;
	RPacketContext::TConnectionInfoV1 respConnectionInfoV1;    
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1_1;
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1_1(reqConnectionInfoV1_1); 	
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1_2;
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1_2(reqConnectionInfoV1_2); 	
	RPacketContext::TConnectionInfoV1 reqConnectionInfoV1_3;
	TPckg<RPacketContext::TConnectionInfoV1> pckgReqConnectionInfoV1_3(reqConnectionInfoV1_3); 	
	
 	//-------------------------------------------------------------------------
	// TEST A1: Successful completion of 
	// RPacketContext::NotifyConnectionInfoChange request for multible clients.
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 	
	    	
	//Make NotifyConnectionInfoChange requests
	packetContext1.NotifyConnectionInfoChange(requestStatus1, pckgReqConnectionInfoV1_1);
	packetContext2.NotifyConnectionInfoChange(requestStatus2, pckgReqConnectionInfoV1_2);
	packetContext3.NotifyConnectionInfoChange(requestStatus3, pckgReqConnectionInfoV1_3);		

	//Send response via MockLTSY to first packet context
	respConnectionInfoV1.iHSDPACategory = 2;
	respConnectionInfoV1.iHSUPACategory = 3;
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory | RPacketContext::KHSUPACategory;
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData1(contextId, respConnectionInfoV1);
	ltsyData1.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//NotifyConnectionInfoChange request is completed to the test clients 1 & 2
	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);		
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus1.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1_1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,	reqConnectionInfoV1_1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_1.iHSUPACategory);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1_2.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_2.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,	reqConnectionInfoV1_2.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_2.iHSUPACategory);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
	
	//Call notification again
	packetContext1.NotifyConnectionInfoChange(requestStatus1, pckgReqConnectionInfoV1_1);
	packetContext2.NotifyConnectionInfoChange(requestStatus2, pckgReqConnectionInfoV1_2);	

	//Send response via MockLTSY to second packet context
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData2(contextId2, respConnectionInfoV1);
	data.Close();
	ltsyData2.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//NotifyConnectionInfoChange request is completed to the test client 3
	User::WaitForRequest(requestStatus3);		
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus3.Int()); 	
	ASSERT_EQUALS((TInt)TConnectionInfoBase::KConnectionInfoV1,   reqConnectionInfoV1_3.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_3.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,	reqConnectionInfoV1_3.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_3.iHSUPACategory);
	
	//Call notification again
	packetContext3.NotifyConnectionInfoChange(requestStatus3, pckgReqConnectionInfoV1_3);	
	
 	//-------------------------------------------------------------------------
	// TEST A2: Successful completion of 
	// RPacketContext::NotifyConnectionInfoChange request for multiple clients. 
	// RPacketContext::TConnectionInfoV1 parameter data changes.
 	//-------------------------------------------------------------------------		

	iMockLTSY.NotifyTerminated(mockLtsyStatus); 		    			

	//Send response via MockLTSY to first packet context	
	respConnectionInfoV1.iValid = RPacketContext::KHSDPACategory;
	respConnectionInfoV1.iHSDPACategory = 5;
	respConnectionInfoV1.iValid |= RPacketContext::KHSUPACategory;
	respConnectionInfoV1.iHSUPACategory = 6;	    	
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData3(contextId, respConnectionInfoV1);
	data.Close();	
	ltsyData3.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
			
	//NotifyConnectionInfoChange request is completed to the test clients 1 & 2
	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);		
	AssertMockLtsyStatusL();
	
	ASSERT_EQUALS(KErrNone, requestStatus1.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1_1.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_1.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1_1.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_1.iHSUPACategory);	
	
	ASSERT_EQUALS(KErrNone, requestStatus2.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1_2.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_2.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1_2.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_2.iHSUPACategory);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
		
	//Call notification again
	packetContext1.NotifyConnectionInfoChange(requestStatus1, pckgReqConnectionInfoV1_1);
	packetContext2.NotifyConnectionInfoChange(requestStatus2, pckgReqConnectionInfoV1_2);		
		
	//Send response via MockLTSY to second packet context
	TMockLtsyData2< TInfoName, RPacketContext::TConnectionInfoV1 > ltsyData4(contextId2, respConnectionInfoV1);
	data.Close();	
	ltsyData4.SerialiseL(data);
	
	iMockLTSY.CompleteL(EPacketContextNotifyConnectionInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());	
	
	//NotifyConnectionInfoChange request is completed to the test client 3	
	User::WaitForRequest(requestStatus3);		
	AssertMockLtsyStatusL();	
	
	ASSERT_EQUALS(KErrNone, requestStatus3.Int()); 	
	ASSERT_EQUALS(respConnectionInfoV1.ExtensionId(),   reqConnectionInfoV1_3.ExtensionId());
	ASSERT_EQUALS(respConnectionInfoV1.iValid,          reqConnectionInfoV1_3.iValid);
	ASSERT_EQUALS(respConnectionInfoV1.iHSDPACategory,  reqConnectionInfoV1_3.iHSDPACategory);
	ASSERT_EQUALS(respConnectionInfoV1.iHSUPACategory,  reqConnectionInfoV1_3.iHSUPACategory);	
	
	CleanupStack::PopAndDestroy(12, this); //packetContext3, packetService3, phone3, telServer3,
									       //packetContext2, packetService2, phone2, telServer2, 
									       //packetContext1, packetService1, data, this			
    }

#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

/**
@SYMTestCaseID BA-CTSY-PKTC-PCONSC-0001
@SYMDEF DEF125564
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenNewSecondaryContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewSecondaryContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/    
void CCTsyPacketContextFU::TestOpenNewSecondaryContext0001L()
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
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::SetConfig when result is not cached.
 	//-------------------------------------------------------------------------
	TInfoName contextIdSecondary;	
	RPacketContext packetContextSecondary;
	TInt err = packetContextSecondary.OpenNewSecondaryContext(packetService, contextId, contextIdSecondary);	
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContextSecondary);
	
    //-------------------------------------------------------------------------
	// Try to Activate SecondaryContext in order to 
	// call CMmPacketQoSGsmWcdmaExt::ProfileAsked()
 	//-------------------------------------------------------------------------
 	
 	// initialisation of the secondary context 
	TInt status = InitContextL(packetContextSecondary, contextIdSecondary, contextId);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL();
	
	// create new QoS
	RBuf profileName;
	CleanupClosePushL(profileName);
	RPacketQoS packetQoS;
	packetQoS.OpenNewQoS(packetContextSecondary, profileName);
	CleanupClosePushL(packetQoS);
	
 	
	// try to activate SecondaryContext	in order to call CMmPacketQoSGsmWcdmaExt::ProfileAsked()
	TRequestStatus requestStatus;
	packetContextSecondary.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();
	
			
	//-------------------------------------------------------------------------	
	// Try to open too many contexts
 	//-------------------------------------------------------------------------	
	RPacketContext newPacketContext[KMmMaxNumberOfContexts - 2];
 	for (TInt i = 0; i < KMmMaxNumberOfContexts - 2; i++) // -2 -- because there are already 2 contexts
 		{ 		
 			TInfoName newContextIdSecondary;
 			err = newPacketContext[i].OpenNewSecondaryContext(packetService, contextId, newContextIdSecondary);
 			ASSERT_EQUALS(KErrNone, err);
    		CleanupClosePushL(newPacketContext[i]);
 		}
 		
 	RPacketContext OneMorePacketContext;
 	TInfoName contextIdSecondary1;
 	err = OneMorePacketContext.OpenNewSecondaryContext(packetService, contextId, contextIdSecondary1);
 	TInt expectedError = ( KErrOverflow & 0x0000FFFF ) |
                    	 ( KErrUmtsMaxNumOfContextExceededByPhone << 16 );
 	ASSERT_EQUALS(expectedError, err); 	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy( (KMmMaxNumberOfContexts - 2) ); // set of contexts...	
	CleanupStack::PopAndDestroy(8, this); // this...
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCONSC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for Activation of RPacketContext::OpenNewSecondaryContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::Activate after RPacketContext::RemovePacketFilter for SecondaryContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/    
void CCTsyPacketContextFU::TestOpenNewSecondaryContext0001bL()
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
    
    // Open new primary packet context
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);
    
    // initialisation of the context 
 	TInt status = InitContextL(packetContext, contextId);
	ASSERT_EQUALS(KErrNone, status);  
 
	// data for SetContext
    RPacketContext::TContextConfigGPRS setConfig;
     _LIT(KPointName, "Some Name");
    setConfig.iAccessPointName.Copy(KPointName);
    TPckg<RPacketContext::TContextConfigGPRS> pckgConfig(setConfig);    
    
    TRequestStatus requestStatus;    

    TMockLtsyData2<RPacketContext::TContextConfigGPRS, TInfoName > expExpect(setConfig, contextId);
    expExpect.SerialiseL(expectData);
    
    TMockLtsyData1<TInfoName > contextName(contextId);
   	contextName.SerialiseL(data);
 	  
    iMockLTSY.ExpectL(EPacketContextSetConfig, expectData);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

    packetContext.SetConfig(requestStatus, pckgConfig);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());  
   
    TMockLtsyData1 <TInfoName> expData(contextId);
	expData.SerialiseL(expectData);
	
	// Activate this primary context.
	RPacketContext::TContextConfigGPRS contextConfig;
	TMockLtsyData2< TInfoName, RPacketContext::TContextConfigGPRS > ltsyData1(contextId, contextConfig);
	ltsyData1.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextActivate, expectData);
	iMockLTSY.CompleteL(EPacketContextActivate, KErrNone, data);
		
	packetContext.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
    
   	// Open new secondary context
	TInfoName contextIdSecondary;	
	RPacketContext packetContextSecondary;
	TInt err = packetContextSecondary.OpenNewSecondaryContext(packetService, contextId, contextIdSecondary);
 	ASSERT_EQUALS(KErrNone, err);	
	CleanupClosePushL(packetContextSecondary);
	
   	// initialisation of the secondary context 
	status = InitContextL(packetContextSecondary, contextIdSecondary, contextId);
	ASSERT_EQUALS(KErrNone, status);
	AssertMockLtsyStatusL();
	
	// Add filter
	RPacketContext::TPacketFilterV2 filter;
	filter.iId = 1;
	RPacketContext::TPacketFilterV2Pckg filterPckg (filter);
	err = AddContextPacketFilterL(packetContextSecondary, contextIdSecondary,filter);// ETrue means that this request doesn't reache LTSY
	ASSERT_EQUALS(KErrNone, err);
	AssertMockLtsyStatusL();	
	
	// set context Active
	SetContextActiveL(contextIdSecondary);
	AssertMockLtsyStatusL();
	
	// Open and set QoS profile
	RPacketQoS packetQoSSecondary;
	RPacketQoS::TQoSGPRSRequested secondaryRequested;
	err = CreateQoSAndSetProfileParamLC(packetQoSSecondary, packetContextSecondary,
											 contextIdSecondary, secondaryRequested, contextId);// contextId -- is name of "primary" context
	ASSERT_EQUALS(KErrNone, err);
	AssertMockLtsyStatusL();

	// Remove the already added packet filter
	TInt filterId = 1;
	packetContextSecondary.RemovePacketFilter(requestStatus, filterId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	AssertMockLtsyStatusL();		
	
	// Activate the secondary context
				
	packetContextSecondary.Activate(requestStatus);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(7, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTC-PCONSC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenNewSecondaryContext with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewSecondaryContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestOpenNewSecondaryContext0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);     
      
    RPacketContext packetContextSecondary;
 	TInfoName contextIdSecondary;
 	
    // -----------------------------------------------------------------------
    // Test with too long ContextId
    // ----------------------------------------------------------------------    
    TBuf<KMaxInfoName + 1> longContextId;
    _LIT(KLongId, "qwertyuiopasdfghj");
    longContextId.Copy(KLongId);
	TInt err = packetContextSecondary.OpenNewSecondaryContext(packetService, longContextId, contextIdSecondary);    
	ASSERT_EQUALS(KErrArgument, err);	
	//We need to close the sub sessiun before we open a new context 
	packetContextSecondary.Close();
	
 	
	
	// -----------------------------------------------------------------------
    // Previous OpenNewSecondaryContext failed with KErrArgument
    // so try OpenNewSecondaryContext with correct params
    // ----------------------------------------------------------------------     
	err = packetContextSecondary.OpenNewSecondaryContext(packetService, contextId, contextIdSecondary);    
	ASSERT_EQUALS(KErrNone, err);         	   
        
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // this, ...
	}
	
	
	
/**
@SYMTestCaseID BA-CTSY-PKTC-PCONSC-0003b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenNewSecondaryContext with wrong ContextId
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewSecondaryContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestOpenNewSecondaryContext0003bL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext); 
   
	
	// -----------------------------------------------------------------------
    // Test with wrong ContextId
    // ----------------------------------------------------------------------
    RPacketContext packetContextSecondary;
    TInfoName wrongContextId;
    _LIT(KSameId, "WrongId");
    wrongContextId.Copy(KSameId);    
    
    TInfoName contextIdSecondary;
    TInt err = packetContextSecondary.OpenNewSecondaryContext(packetService, wrongContextId, contextIdSecondary);
	CleanupClosePushL(packetContextSecondary);        
    ASSERT_EQUALS(KErrArgument, err);
    
    AssertMockLtsyStatusL();    
	CleanupStack::PopAndDestroy(4, this); // this, ...
	}
	
	
	
/**
@SYMTestCaseID BA-CTSY-PKTC-PCONSC-0003c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenNewSecondaryContext with with empty PacketService
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewSecondaryContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestOpenNewSecondaryContext0003cL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
		
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
    
    TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext); 
	      
    // -----------------------------------------------------------------------
    // Test with empty PacketService
    // -----------------------------------------------------------------------    
    RPacketService emptyPacketService;
    RPacketContext packetContextSecondary;
    TInfoName contextIdSecondary; 
    // there should be EEtelPanicNullHandle panic 
    TInt err = packetContextSecondary.OpenNewSecondaryContext(emptyPacketService, contextId, contextIdSecondary);
    
    AssertMockLtsyStatusL();    
	CleanupStack::PopAndDestroy(3, this); // this, ...
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCONC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenNewContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/    
void CCTsyPacketContextFU::TestOpenNewContext0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
		
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);    
       
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::SetConfig when result is not cached.
 	//------------------------------------------------------------------------
	TInfoName contextId;  
    RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // packetContext, packetService, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCONC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenNewContext with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenNewContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestOpenNewContext0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);
	
	//-------------------------------------------------------------------------	
	// Try to open too many contexts
 	//-------------------------------------------------------------------------                	
    TInt err = 0;  	
 	for (TInt i = 0; i < KMmMaxNumberOfContexts; i++)
 		{
 			RPacketContext newPacketContext;
 			TInfoName contextId;
 			err = newPacketContext.OpenNewContext(packetService, contextId);
 			ASSERT_EQUALS(KErrNone, err);
    		CleanupClosePushL(newPacketContext); 			
 		}
 		
 	RPacketContext OneMorePacketContext;
 	TInfoName contextId1;
 	err = OneMorePacketContext.OpenNewContext(packetService, contextId1);
 	TInt expectedError = ( KErrOverflow & 0x0000FFFF ) |
                    	 ( KErrUmtsMaxNumOfContextExceededByPhone << 16 );
 	ASSERT_EQUALS(expectedError, err); 		

	//-------------------------------------------------------------------------
	// Use non-created packetService
 	//-------------------------------------------------------------------------	
	RPacketService packetService2;       
    TInfoName contextId2;
    RPacketContext packetContext2;
        
    // there should be expected EEtelPanicNullHandle panic 
    packetContext2.OpenNewContext(packetService2, contextId2);  
      
	CleanupStack::PopAndDestroy (KErrUmtsMaxNumOfContextExceededByPhone);
	CleanupStack::PopAndDestroy(2, this); //  packetService1, this
	}

	
/**
@SYMTestCaseID BA-CTSY-PKTC-PCONEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenExistingContext
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenExistingContext
@SYMTestExpectedResults Pass
@SYMTestType CT
*/    
void CCTsyPacketContextFU::TestOpenExistingContext0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();	
	
	RBuf8 data;
    CleanupClosePushL(data);
	
	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);    
        
	TInfoName contextId; 
	RPacketContext packetContext;
    packetContext.OpenNewContext(packetService, contextId);
    CleanupClosePushL(packetContext);   
          
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::SetConfig when result is not cached.
 	//-------------------------------------------------------------------------
 	RPacketContext packetContext2;
	TInt err = packetContext2.OpenExistingContext(packetService, contextId);	
	CleanupClosePushL(packetContext2);
	ASSERT_EQUALS(KErrNone, err);
		
	// -----------------------------------------------------------------------
    // Test contextId == KStringExternal and iDialUpContext created
    // ----------------------------------------------------------------------           
    TRequestStatus	mockLtsyStatus;
    // Data for CompleteL
	TInfoName sndName;	
	TInt sndResult = KErrNotFound;
	TMockLtsyData2 <TInfoName, TInt > ltsyData(sndName, sndResult);
	ltsyData.SerialiseL(data);	
    
    // ---  create iDialUpContext at first -----------
	iMockLTSY.NotifyTerminated(mockLtsyStatus); 
 	// Issue the Complete...	
	iMockLTSY.CompleteL(EPacketNotifyContextAdded, KErrNone, data);
	// wait for completion 
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	//------------------------------------------
	
	RPacketContext packetContext3;
    TInfoName longContextId3;
    _LIT( KStringExternal,  "External"  );
    longContextId3.Copy(KStringExternal);
	err = packetContext3.OpenExistingContext(packetService, longContextId3);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext3); 		
		
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); //packetContext3, packetContext2, packetContext, packetService, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTC-PCONEC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::OpenExistingContext with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::OpenExistingContext with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketContextFU::TestOpenExistingContext0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RPacketService packetService;
    OpenPacketServiceL(packetService);
    CleanupClosePushL(packetService);    
        
	TInfoName contextId;
	//-------------------------------------------------------------------------
	// Test with non-existing context
 	//------------------------------------------------------------------------- 
 	_LIT(KSomeContext, "SomeContext");
	contextId.Copy(KSomeContext);
	RPacketContext packetContext1;
	TInt err = packetContext1.OpenExistingContext(packetService, contextId);
	ASSERT_EQUALS(KErrNotFound, err);
	AssertMockLtsyStatusL();	
	
	// -----------------------------------------------------------------------
    // Test with too long ContextId
    // ----------------------------------------------------------------------    
    RPacketContext packetContext2;
    TBuf<KMaxInfoName + 1> longContextId;
    _LIT(KLongId, "qwertyuiopasdfghj");
    longContextId.Copy(KLongId);
	err = packetContext2.OpenExistingContext(packetService, longContextId);    
	ASSERT_EQUALS(KErrOverflow, err);
	
	// -----------------------------------------------------------------------
    // Test contextId == KStringExternal, but when iDialUpContext is NOT created
    // ----------------------------------------------------------------------    
    RPacketContext packetContext3;
    TInfoName longContextId3;
    _LIT( KStringExternal,  "External"  );
    longContextId3.Copy(KStringExternal);
	err = packetContext3.OpenExistingContext(packetService, longContextId3);    
	ASSERT_EQUALS(KErrNotFound, err);		

    //-------------------------------------------------------------------------
	// Test with epmty contextId
 	//-------------------------------------------------------------------------
 	RPacketContext packetContext4;
 	contextId.Zero();
    // there should be expected KErrBadName panic  	
 	err = packetContext4.OpenExistingContext(packetService, contextId); 	
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	}

