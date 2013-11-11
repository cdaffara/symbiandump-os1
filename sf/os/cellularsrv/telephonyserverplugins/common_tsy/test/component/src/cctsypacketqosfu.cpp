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
// The TEFUnit test suite for PacketQoS in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsypacketqosfu.h"
#include <ctsy/serviceapi/cmmutility.h> // TContextMisc
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <pcktcs.h>
#include <etelqos.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "Cmmpacketdef.h"

CTestSuite* CCTsyPacketQoSFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddMediaAuthorizationL0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddMediaAuthorizationL0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddMediaAuthorizationL0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddMediaAuthorizationL0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestRemoveMediaAuthorization0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestRemoveMediaAuthorization0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestRemoveMediaAuthorization0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetProfileParameters0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetProfileParameters0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetProfileParameters0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestNotifyProfileChanged0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestNotifyProfileChanged0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestNotifyProfileChanged0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestNotifyProfileChanged0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestSetProfileParameters0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestSetProfileParameters0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestSetProfileParameters0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestSetProfileParameters0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestSetProfileParameters0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetProfileCapabilities0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetProfileCapabilities0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetProfileCapabilities0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestCreateNewTFT0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddPacketFilter0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddPacketFilter0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddPacketFilter0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestAddPacketFilter0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestRemovePacketFilter0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestRemovePacketFilter0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetPacketFilterInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetPacketFilterInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestGetPacketFilterInfo0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestDeleteTFT0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestOpenNewQoS0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestOpenNewQoS0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestOpenExistingQoS0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestOpenExistingQoS0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPacketQoSFU, TestOpenExistingQoS0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAMA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::AddMediaAuthorizationL
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::AddMediaAuthorizationL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddMediaAuthorizationL0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewSecondaryContext(packetService, contextName, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
	mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuthorization);

	mediaAuthorization->iAuthorizationToken = _L8("test");

	RPacketContext::CTFTMediaAuthorizationV3::TFlowIdentifier flowId;
	flowId.iIPFlowNumber         = 0x100;
	flowId.iMediaComponentNumber = 0x200;
	mediaAuthorization->iFlowIds.Append(flowId);
	flowId.iIPFlowNumber         = 0x10;
	flowId.iMediaComponentNumber = 0x20;
	mediaAuthorization->iFlowIds.Append(flowId);
	
	TMockLtsyContextData2< TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
		expExpect(contextConfig, configurationType, contextName2, mediaAuthorization);

	expExpect.SerialiseL(data);
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data, KErrNotSupported);

	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
 	
 	//-------------------------------------------------------------------------
	// TEST for increasing the coverage 
 	//-------------------------------------------------------------------------

	for (TInt index = 0; index < 3; index++)
		{
		RPacketContext::CTFTMediaAuthorizationV3* media= NULL;
		media = RPacketContext::CTFTMediaAuthorizationV3::NewL();
		CleanupStack::PushL(media);

		TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
			exp(contextConfig, configurationType, contextName2, mediaAuthorization);

		data.Close();
		exp.SerialiseL(data);

	    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data, KErrNotSupported);

		packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
		User::WaitForRequest(requestStatus);
		AssertMockLtsyStatusL();
		ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
		CleanupStack::Pop(media);
		}

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
 	TMockLtsyData1<TInfoName > contexName(contextName2);
    contexName.SerialiseL(completeData);
 	 	
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);    
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrGeneral, completeData);    

	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::AddMediaAuthorizationL when result is not cached.
 	//-------------------------------------------------------------------------
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData);

	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST for coverage increasing 
	// with context status != Unknown
 	//-------------------------------------------------------------------------	
	
	// set  context status to EStatusActive
	TInt err = SetContextActiveL(contextName2);
	AssertMockLtsyStatusL();
	
	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST for coverage increasing 
	// with context status != Unknown
 	//-------------------------------------------------------------------------	
	
	// set  context status to EStatusInactive 
	err = SetContextStatusL(contextName2, RPacketContext::EStatusInactive );
	
	// call AddMediaAuthorizationL itself
	iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData);

	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	CleanupStack::Pop(mediaAuthorization);
	CleanupStack::PopAndDestroy(3); // packetService1, packetContext1, packetContext2
	CleanupStack::PopAndDestroy(3); // completeData, data, this, dataSerialise
		
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAMA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::AddMediaAuthorizationL
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::AddMediaAuthorizationL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddMediaAuthorizationL0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewSecondaryContext(packetService, contextName, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::AddMediaAuthorizationL
 	//-------------------------------------------------------------------------
 	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
	mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuthorization);
	
	TMockLtsyContextData2< TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
		expExpect(contextConfig, configurationType, contextName2, mediaAuthorization);
	
	expExpect.SerialiseL(data);

	// data CompleteL
	TMockLtsyData1<TInfoName > contexName(contextName2);
    contexName.SerialiseL(completeData);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);
       
	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	

    packetContext2.CancelAsyncRequest(EPacketAddMediaAuthorization);
    
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	User::WaitForRequest(mockLtsyStatus);

	CleanupStack::Pop(mediaAuthorization);
	CleanupStack::PopAndDestroy(3); // packetService1, packetContext1, packetContext2
	CleanupStack::PopAndDestroy(3); //completeData, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAMA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::AddMediaAuthorizationL
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::AddMediaAuthorizationL
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddMediaAuthorizationL0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);
	RBuf8 completeData1;
	CleanupClosePushL(completeData1);

	RBuf8 completeData2;
	CleanupClosePushL(completeData2);

	// Open second client
	RTelServer telServer1;
	TInt ret = telServer1.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer1);

	RTelServer telServer1S;
	ret = telServer1S.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer1S);

	RTelServer telServer2S;
	ret = telServer2S.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2S);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone phone1S;
	ret = phone1S.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone1S);

	RMobilePhone phone2S;
	ret = phone2S.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2S);

	RPacketService packetService1;
	OpenPacketServiceL(packetService1);
	CleanupClosePushL(packetService1);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	RPacketService packetService1S;                
	ret = packetService1S.Open(phone1S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService1S);

	RPacketService packetService2S;                
	ret = packetService2S.Open(phone2S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2S);

	TInfoName contextName1;	
	RPacketContext packetContext1;
	ret = packetContext1.OpenNewContext(packetService1, contextName1);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext1);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewContext(packetService2, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TInfoName contextName1S;	
	RPacketContext packetContext1S;
	ret = packetContext1S.OpenNewSecondaryContext(packetService1S, contextName1, contextName1S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext1S);

	TInfoName contextName2S;	
	RPacketContext packetContext2S;
	ret = packetContext2S.OpenNewSecondaryContext(packetService2S, contextName2, contextName2S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2S);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::AddMediaAuthorizationL
 	//-------------------------------------------------------------------------

	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
	mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuthorization);

	// first client	
 	TMockLtsyData1<TInfoName > contexName1(contextName1S);
    contexName1.SerialiseL(completeData1);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
		expExpect1(contextConfig, configurationType, contextName1S, mediaAuthorization);
	expExpect1.SerialiseL(data1);

    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data1);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData1);

	packetContext1S.AddMediaAuthorizationL(requestStatus1, *mediaAuthorization);	

	// second client	
 	TMockLtsyData1<TInfoName > contexName2(contextName2S);
    contexName2.SerialiseL(completeData2);

	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3* > 
		expExpect2(contextConfig, configurationType, contextName2S, mediaAuthorization);
	expExpect2.SerialiseL(data2);

    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data2);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData2);

	packetContext2S.AddMediaAuthorizationL(requestStatus2, *mediaAuthorization);	

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::Pop(mediaAuthorization);
	CleanupStack::PopAndDestroy(14);
	CleanupStack::PopAndDestroy(5, this); // data1, data2, this, completeData1, completeData2

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAMA-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::AddMediaAuthorizationL with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::AddMediaAuthorizationL and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddMediaAuthorizationL0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewSecondaryContext(packetService, contextName, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TRequestStatus requestStatus;

	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketContext::AddMediaAuthorizationL
 	//-------------------------------------------------------------------------

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
	mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	CleanupStack::PushL(mediaAuthorization);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3*> 
		expExpect(contextConfig, configurationType, contextName2, mediaAuthorization);
	
	expExpect.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);
	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::Pop(mediaAuthorization);
	CleanupStack::PopAndDestroy(3); // packetService1, packetContext1, packetContext2
	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCRMA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::RemoveMediaAuthorization
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::RemoveMediaAuthorization
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestRemoveMediaAuthorization0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewSecondaryContext(packetService, contextName, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// TEST
 	//-------------------------------------------------------------------------

	_LIT8(KAuthorizationToken, "AuthorizationToken");
		
	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	RPacketContext::CTFTMediaAuthorizationV3* media = NULL;
	media = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	media->iAuthorizationToken = KAuthorizationToken;
	CleanupStack::PushL(media);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3*> 
		exp(contextConfig, configurationType, contextName2, media);

	exp.SerialiseL(data);

    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data, KErrNotSupported);

	packetContext2.AddMediaAuthorizationL(requestStatus, *media);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	RPacketContext::TAuthorizationToken authorization1;
	authorization1 = KAuthorizationToken;

	packetContext2.RemoveMediaAuthorization(requestStatus, authorization1);	
	ASSERT_EQUALS(KErrNone, ret);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST
 	//-------------------------------------------------------------------------
	
	RPacketContext::TAuthorizationToken authorizationToken;
	authorizationToken = KAuthorizationToken;

	packetContext.RemoveMediaAuthorization(requestStatus, authorizationToken);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST
 	//-------------------------------------------------------------------------
	
	// add new MediaAuthorization
 	//-------------------------------------------------------------------------

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization= NULL;
	mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	mediaAuthorization->iAuthorizationToken = KAuthorizationToken;
	CleanupStack::PushL(mediaAuthorization);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3*> 
		expExpect(contextConfig, configurationType, contextName2, mediaAuthorization);
	data.Close();
	expExpect.SerialiseL(data);

 	TMockLtsyData1<TInfoName > contexName(contextName2);
    contexName.SerialiseL(completeData);

    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData);    

	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	//-------------------------------------------------------------------------
	
	packetContext2.RemoveMediaAuthorization(requestStatus, authorizationToken);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::RemoveMediaAuthorization when result is not cached.
 	//-------------------------------------------------------------------------
	packetContext2.RemoveMediaAuthorization(requestStatus, authorizationToken);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	CleanupStack::Pop(mediaAuthorization);
	CleanupStack::Pop(media);
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // data, this, completeData	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCRMA-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::RemoveMediaAuthorization with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::RemoveMediaAuthorization with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestRemoveMediaAuthorization0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewSecondaryContext(packetService, contextName, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test add new MediaAuthorization
 	//-------------------------------------------------------------------------
	_LIT8(KAuthorizationToken,  "AuthorizationToken");
	_LIT8(KAuthorizationToken2, "Other AuthorizationToken");	

	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization= NULL;
	mediaAuthorization = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	mediaAuthorization->iAuthorizationToken = KAuthorizationToken;
	CleanupStack::PushL(mediaAuthorization);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3*> 
		expExpect(contextConfig, configurationType, contextName2, mediaAuthorization);

 	TMockLtsyData1<TInfoName > contexName(contextName2);
    contexName.SerialiseL(completeData);

	expExpect.SerialiseL(data);
	
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData);    

	packetContext2.AddMediaAuthorizationL(requestStatus, *mediaAuthorization);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	//-------------------------------------------------------------------------

	RPacketContext::TAuthorizationToken authorizationToken;
	authorizationToken = KAuthorizationToken2;
	 	
	packetContext2.RemoveMediaAuthorization(requestStatus, authorizationToken);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotFound, requestStatus.Int());

	CleanupStack::Pop(mediaAuthorization);
	CleanupStack::PopAndDestroy(6, this); // data, completeData, packetService, packetContext, packetContext2, this 

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCRMA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::RemoveMediaAuthorization
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::RemoveMediaAuthorization
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestRemoveMediaAuthorization0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 completeData1;
	CleanupClosePushL(completeData1);

	RBuf8 completeData2;
	CleanupClosePushL(completeData2);

	// Open second client
	RTelServer telServer1;
	TInt ret = telServer1.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer1);

	RTelServer telServer1S;
	ret = telServer1S.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer1S);

	RTelServer telServer2S;
	ret = telServer2S.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2S);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone phone1S;
	ret = phone1S.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone1S);

	RMobilePhone phone2S;
	ret = phone2S.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2S);

	RPacketService packetService1;
	OpenPacketServiceL(packetService1);
	CleanupClosePushL(packetService1);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	RPacketService packetService1S;                
	ret = packetService1S.Open(phone1S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService1S);

	RPacketService packetService2S;                
	ret = packetService2S.Open(phone2S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2S);

	TInfoName contextName1;	
	RPacketContext packetContext1;
	ret = packetContext1.OpenNewContext(packetService1, contextName1);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext1);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewContext(packetService2, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TInfoName contextName1S;	
	RPacketContext packetContext1S;
	ret = packetContext1S.OpenNewSecondaryContext(packetService1S, contextName1, contextName1S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext1S);

	TInfoName contextName2S;	
	RPacketContext packetContext2S;
	ret = packetContext2S.OpenNewSecondaryContext(packetService2S, contextName2, contextName2S);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2S);

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::RemoveMediaAuthorization
 	//-------------------------------------------------------------------------

	_LIT8(KAuthorizationToken1,  "AuthorizationToken1");
	_LIT8(KAuthorizationToken2,  "AuthorizationToken2");
	
	// add new MediaAuthorization
 	//-------------------------------------------------------------------------
	RPacketContext::TContextConfigGPRS contextConfig;
	TInt configurationType = 0;

	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization1 = NULL;
	mediaAuthorization1 = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	mediaAuthorization1->iAuthorizationToken = KAuthorizationToken1;
	CleanupStack::PushL(mediaAuthorization1);

 	TMockLtsyData1<TInfoName > contexName1(contextName1S);
    contexName1.SerialiseL(completeData1);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3*> 
		expExpect1(contextConfig, configurationType, contextName1S, mediaAuthorization1);
	expExpect1.SerialiseL(data1);
	
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data1);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData1);    

	packetContext1S.AddMediaAuthorizationL(requestStatus, *mediaAuthorization1);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RPacketContext::TAuthorizationToken authorizationToken;
	authorizationToken = KAuthorizationToken1;
	 	
	packetContext1S.RemoveMediaAuthorization(requestStatus1, authorizationToken);	

	// add new MediaAuthorization
 	//-------------------------------------------------------------------------
	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization2 = NULL;
	mediaAuthorization2 = RPacketContext::CTFTMediaAuthorizationV3::NewL();
	mediaAuthorization2->iAuthorizationToken = KAuthorizationToken2;
	CleanupStack::PushL(mediaAuthorization2);
	
 	TMockLtsyData1<TInfoName > contexName2(contextName2S);
    contexName2.SerialiseL(completeData2);
	
	TMockLtsyContextData2<TInfoName, RPacketContext::CTFTMediaAuthorizationV3*> 
		expExpect2(contextConfig, configurationType, contextName2S, mediaAuthorization2);
	expExpect2.SerialiseL(data2);
	
    iMockLTSY.ExpectL(EPacketAddMediaAuthorization, data2);
    iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, completeData2);    

	packetContext2S.AddMediaAuthorizationL(requestStatus, *mediaAuthorization2);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
 	//-------------------------------------------------------------------------

	authorizationToken = KAuthorizationToken2;
	 	
	packetContext2S.RemoveMediaAuthorization(requestStatus2, authorizationToken);	
	
	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::Pop(mediaAuthorization2);
	CleanupStack::Pop(mediaAuthorization1);
	CleanupStack::PopAndDestroy(14);
	CleanupStack::PopAndDestroy(5, this); // data1, data2, this, completeData1, completeData2

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QGPP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::GetProfileParameters
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::GetProfileParameters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetProfileParameters0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);
	
	TRequestStatus requestStatus;
	
	RPacketQoS::TQoSGPRSNegotiated packetGPRS;
	TPckg< RPacketQoS::TQoSGPRSNegotiated > profilePckgGPRS = packetGPRS;
	packetQoS.GetProfileParameters(requestStatus, profilePckgGPRS);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(packetGPRS.iPrecedence    , RPacketQoS::EUnspecifiedPrecedence);
	ASSERT_EQUALS(packetGPRS.iDelay         , RPacketQoS::EUnspecifiedDelayClass);
	ASSERT_EQUALS(packetGPRS.iReliability   , RPacketQoS::EUnspecifiedReliabilityClass);
	ASSERT_EQUALS(packetGPRS.iPeakThroughput, RPacketQoS::EUnspecifiedPeakThroughput);
	ASSERT_EQUALS(packetGPRS.iMeanThroughput, RPacketQoS::EUnspecifiedMeanThroughput);

	RPacketQoS::TQoSR5Negotiated packetR5;
    TPckg< RPacketQoS::TQoSR5Negotiated > profilePckgR5 = packetR5;
	packetQoS.GetProfileParameters(requestStatus, profilePckgR5);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(packetR5.iSignallingIndication      , 0);
	ASSERT_EQUALS(packetR5.iSourceStatisticsDescriptor, 
	              RPacketQoS::ESourceStatisticsDescriptorUnknown);
	ASSERT_EQUALS(packetR5.iTrafficClass              , 
	              RPacketQoS::ETrafficClassUnspecified);
	ASSERT_EQUALS(packetR5.iDeliveryOrderReqd         , 
	              RPacketQoS::EDeliveryOrderUnspecified);
	ASSERT_EQUALS(packetR5.iDeliverErroneousSDU       , 
	              RPacketQoS::EErroneousSDUDeliveryUnspecified);
	ASSERT_EQUALS(packetR5.iMaxSDUSize                , 0);
	ASSERT_EQUALS(packetR5.iMaxRate.iUplinkRate       , 0);
	ASSERT_EQUALS(packetR5.iMaxRate.iDownlinkRate     , 0);
	ASSERT_EQUALS(packetR5.iBER                       , 
	              RPacketQoS::EBERUnspecified);
	ASSERT_EQUALS(packetR5.iSDUErrorRatio             , 
	              RPacketQoS::ESDUErrorRatioUnspecified);
	ASSERT_EQUALS(packetR5.iTrafficHandlingPriority   , 
	              RPacketQoS::ETrafficPriorityUnspecified);
	ASSERT_EQUALS(packetR5.iTransferDelay             , 0);
	ASSERT_EQUALS(packetR5.iGuaranteedRate.iDownlinkRate, 0);
	ASSERT_EQUALS(packetR5.iGuaranteedRate.iUplinkRate  , 0);

	RPacketQoS::TQoSR99_R4Negotiated packetR99;
    TPckg< RPacketQoS::TQoSR99_R4Negotiated > profilePckgR99 = packetR99;
	packetQoS.GetProfileParameters(requestStatus, profilePckgR99);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(packetR99.iTrafficClass              , 
	              RPacketQoS::ETrafficClassUnspecified);
	ASSERT_EQUALS(packetR99.iDeliveryOrderReqd         , 
	              RPacketQoS::EDeliveryOrderUnspecified);
	ASSERT_EQUALS(packetR99.iDeliverErroneousSDU       , 
	              RPacketQoS::EErroneousSDUDeliveryUnspecified);
	ASSERT_EQUALS(packetR99.iMaxSDUSize                , 0);
	ASSERT_EQUALS(packetR99.iMaxRate.iUplinkRate       , 0);
	ASSERT_EQUALS(packetR99.iMaxRate.iDownlinkRate     , 0);
	ASSERT_EQUALS(packetR99.iBER                       , 
	              RPacketQoS::EBERUnspecified);
	ASSERT_EQUALS(packetR99.iSDUErrorRatio             , 
	              RPacketQoS::ESDUErrorRatioUnspecified);
	ASSERT_EQUALS(packetR99.iTrafficHandlingPriority   , 
	              RPacketQoS::ETrafficPriorityUnspecified);
	ASSERT_EQUALS(packetR99.iTransferDelay             , 0);
	ASSERT_EQUALS(packetR99.iGuaranteedRate.iDownlinkRate, 0);
	ASSERT_EQUALS(packetR99.iGuaranteedRate.iUplinkRate  , 0);

 	//-------------------------------------------------------------------------
	TPacketQoSGPRSNegotiated   packetQoSGPRS;
	TPacketQoSR99_R4Negotiated packetQoSR99_R4;
	TPacketQoSR5Negotiated     packetQoSR5;

 	//-------------------------------------------------------------------------
	// Initialization TPacketQoSGPRSNegotiated
	packetQoSGPRS.iPrecedence     = RPacketQoS::EPriorityHighPrecedence;
	packetQoSGPRS.iDelay          = RPacketQoS::EDelayClass3;
	packetQoSGPRS.iReliability    = RPacketQoS::EReliabilityClass2;
	packetQoSGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput16000;
	packetQoSGPRS.iMeanThroughput = RPacketQoS::EMeanThroughput100000;

 	//-------------------------------------------------------------------------
	// Initialization packetQoSR99_R4
	packetQoSR99_R4.iTrafficClass                 = RPacketQoS::ETrafficClassBackground;
	packetQoSR99_R4.iDeliveryOrderReqd            = RPacketQoS::EDeliveryOrderNotRequired;
	packetQoSR99_R4.iDeliverErroneousSDU          = RPacketQoS::EErroneousSDUDeliveryNotRequired;
	packetQoSR99_R4.iMaxSDUSize                   = 10;
	packetQoSR99_R4.iMaxRate.iUplinkRate          = 20;
	packetQoSR99_R4.iMaxRate.iDownlinkRate        = 100;
	packetQoSR99_R4.iBER                          = RPacketQoS::EBERSixPerHundredMillion;
	packetQoSR99_R4.iSDUErrorRatio                = RPacketQoS::ESDUErrorRatioOnePerMillion;
	packetQoSR99_R4.iTrafficHandlingPriority      = RPacketQoS::ETrafficPriority3;
	packetQoSR99_R4.iTransferDelay                = 1000;
	packetQoSR99_R4.iGuaranteedRate.iDownlinkRate = 5;
	packetQoSR99_R4.iGuaranteedRate.iUplinkRate   = 7;

 	//-------------------------------------------------------------------------
	// Initialization packetQoSR5
	packetQoSR5.iSignallingIndication         = ETrue;
	packetQoSR5.iSourceStatisticsDescriptor   = RPacketQoS::ESourceStatisticsDescriptorSpeech;
	packetQoSR5.iTrafficClass                 = RPacketQoS::ETrafficClassBackground;
	packetQoSR5.iDeliveryOrderReqd            = RPacketQoS::EDeliveryOrderNotRequired;
	packetQoSR5.iDeliverErroneousSDU          = RPacketQoS::EErroneousSDUDeliveryNotRequired;
	packetQoSR5.iMaxSDUSize                   = 12;
	packetQoSR5.iMaxRate.iUplinkRate          = 22;
	packetQoSR5.iMaxRate.iDownlinkRate        = 102;
	packetQoSR5.iBER                          = RPacketQoS::EBERSixPerHundredMillion;
	packetQoSR5.iSDUErrorRatio                = RPacketQoS::ESDUErrorRatioOnePerMillion;
	packetQoSR5.iTrafficHandlingPriority      = RPacketQoS::ETrafficPriority3;
	packetQoSR5.iTransferDelay                = 1002;
	packetQoSR5.iGuaranteedRate.iDownlinkRate = 7;
	packetQoSR5.iGuaranteedRate.iUplinkRate   = 9;

    TMockLtsyPacketQoSData1< TInfoName > mockData(packetQoSGPRS, packetQoSR99_R4, 
                                                  packetQoSR5  , contextName);
    mockData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketQoSNotifyProfileChanged, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// TEST RPacketQoS::GetProfileParameters again, this time CTSY
	// will get result from the cache for RPacketQoS::TQoSGPRSNegotiated.
 	//-------------------------------------------------------------------------

	packetQoS.GetProfileParameters(requestStatus, profilePckgGPRS);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(packetQoSGPRS.iPrecedence    , packetGPRS.iPrecedence    );
	ASSERT_EQUALS(packetQoSGPRS.iDelay         , packetGPRS.iDelay         );
	ASSERT_EQUALS(packetQoSGPRS.iReliability   , packetGPRS.iReliability   );
	ASSERT_EQUALS(packetQoSGPRS.iPeakThroughput, packetGPRS.iPeakThroughput);
	ASSERT_EQUALS(packetQoSGPRS.iMeanThroughput, packetGPRS.iMeanThroughput);

 	//-------------------------------------------------------------------------
	// TEST RPacketQoS::GetProfileParameters again, this time CTSY
	// will get result from the cache for RPacketQoS::TQoSR5Negotiated.
 	//-------------------------------------------------------------------------

	packetQoS.GetProfileParameters(requestStatus, profilePckgR5);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(packetQoSR5.iSignallingIndication        , packetR5.iSignallingIndication        );
	ASSERT_EQUALS(packetQoSR5.iSourceStatisticsDescriptor  , packetR5.iSourceStatisticsDescriptor  );
	ASSERT_EQUALS(packetQoSR5.iTrafficClass                , packetR5.iTrafficClass                );
	ASSERT_EQUALS(packetQoSR5.iDeliveryOrderReqd           , packetR5.iDeliveryOrderReqd           );
	ASSERT_EQUALS(packetQoSR5.iDeliverErroneousSDU         , packetR5.iDeliverErroneousSDU         );
	ASSERT_EQUALS(packetQoSR5.iMaxSDUSize                  , packetR5.iMaxSDUSize                  );
	ASSERT_EQUALS(packetQoSR5.iMaxRate.iUplinkRate         , packetR5.iMaxRate.iUplinkRate         );
	ASSERT_EQUALS(packetQoSR5.iMaxRate.iDownlinkRate       , packetR5.iMaxRate.iDownlinkRate       );
	ASSERT_EQUALS(packetQoSR5.iBER                         , packetR5.iBER                         );
	ASSERT_EQUALS(packetQoSR5.iSDUErrorRatio               , packetR5.iSDUErrorRatio               );
	ASSERT_EQUALS(packetQoSR5.iTrafficHandlingPriority     , packetR5.iTrafficHandlingPriority     );
	ASSERT_EQUALS(packetQoSR5.iTransferDelay               , packetR5.iTransferDelay               );
	ASSERT_EQUALS(packetQoSR5.iGuaranteedRate.iDownlinkRate, packetR5.iGuaranteedRate.iDownlinkRate);
	ASSERT_EQUALS(packetQoSR5.iGuaranteedRate.iUplinkRate  , packetR5.iGuaranteedRate.iUplinkRate  );

 	//-------------------------------------------------------------------------
	// TEST RPacketQoS::GetProfileParameters again, this time CTSY
	// will get result from the cache for RPacketQoS::TQoSR99_R4Negotiated.
 	//-------------------------------------------------------------------------

	packetQoS.GetProfileParameters(requestStatus, profilePckgR99);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(packetQoSR99_R4.iTrafficClass                , packetR99.iTrafficClass                );
	ASSERT_EQUALS(packetQoSR99_R4.iDeliveryOrderReqd           , packetR99.iDeliveryOrderReqd           );
	ASSERT_EQUALS(packetQoSR99_R4.iDeliverErroneousSDU         , packetR99.iDeliverErroneousSDU         );
	ASSERT_EQUALS(packetQoSR99_R4.iMaxSDUSize                  , packetR99.iMaxSDUSize                  );
	ASSERT_EQUALS(packetQoSR99_R4.iMaxRate.iUplinkRate         , packetR99.iMaxRate.iUplinkRate         );
	ASSERT_EQUALS(packetQoSR99_R4.iMaxRate.iDownlinkRate       , packetR99.iMaxRate.iDownlinkRate       );
	ASSERT_EQUALS(packetQoSR99_R4.iBER                         , packetR99.iBER                         );
	ASSERT_EQUALS(packetQoSR99_R4.iSDUErrorRatio               , packetR99.iSDUErrorRatio               );
	ASSERT_EQUALS(packetQoSR99_R4.iTrafficHandlingPriority     , packetR99.iTrafficHandlingPriority     );
	ASSERT_EQUALS(packetQoSR99_R4.iTransferDelay               , packetR99.iTransferDelay               );
	ASSERT_EQUALS(packetQoSR99_R4.iGuaranteedRate.iDownlinkRate, packetR99.iGuaranteedRate.iDownlinkRate);
	ASSERT_EQUALS(packetQoSR99_R4.iGuaranteedRate.iUplinkRate  , packetR99.iGuaranteedRate.iUplinkRate  );

	CleanupStack::PopAndDestroy(3); // packetService, packetContext, packetQoS
	CleanupStack::PopAndDestroy(3, this); // data, this, expectData	
	}

/**
@SYMTestCaseID BA-CTSY-PKTQ-QGPP-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::GetProfileParameters with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::GetProfileParameters with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetProfileParameters0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);
	
	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketQoS::GetProfileParameters
 	//-------------------------------------------------------------------------

	RPacketQoS::TQoSCDMA2000Negotiated packetCDMA2000;
    TPckg< RPacketQoS::TQoSCDMA2000Negotiated > profilePckgCDMA2000 = packetCDMA2000;
	packetQoS.GetProfileParameters(requestStatus, profilePckgCDMA2000);	
	User::WaitForRequest(requestStatus);
	TInt error = requestStatus.Int();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketQoS::GetProfileParameters
 	//-------------------------------------------------------------------------

	TBuf8<1> nullDescriptor;
	packetQoS.GetProfileParameters(requestStatus, nullDescriptor);	
	User::WaitForRequest(requestStatus);
	error = requestStatus.Int();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(3); // packetService, packetContext, packetQoS
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QGPP-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketQoS::GetProfileParameters
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketQoS::GetProfileParameters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetProfileParameters0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;
	TName profileName2;
	TName profileName3;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	// Open third client
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone phone3;
	ret = phone3.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	RPacketService packetService3;                
	ret = packetService3.Open(phone3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService3);

	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	ret = packetContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewContext(packetService2, contextId2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	// Open third context
	TInfoName contextId3;	
	RPacketContext packetContext3;
	ret = packetContext3.OpenNewContext(packetService3, contextId3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext3);

	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	RPacketQoS packetQoS2;
	ret = packetQoS2.OpenNewQoS(packetContext2, profileName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS2);

	RPacketQoS packetQoS3;
	ret = packetQoS3.OpenNewQoS(packetContext3, profileName3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS3);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketQoS::GetProfileParameters
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus requestStatus3;
	
	RPacketQoS::TQoSGPRSNegotiated packetGPRS;
	TPckg< RPacketQoS::TQoSGPRSNegotiated > profilePckgGPRS = packetGPRS;
	packetQoS.GetProfileParameters(requestStatus1, profilePckgGPRS);	

	RPacketQoS::TQoSR99_R4Negotiated packetR99;
    TPckg< RPacketQoS::TQoSR99_R4Negotiated > profilePckgR99 = packetR99;
	packetQoS2.GetProfileParameters(requestStatus2, profilePckgR99);	

	RPacketQoS::TQoSR5Negotiated packetR5;
    TPckg< RPacketQoS::TQoSR5Negotiated > profilePckgR5 = packetR5;
	packetQoS3.GetProfileParameters(requestStatus3, profilePckgR5);	

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	User::WaitForRequest(requestStatus3);
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());

	CleanupStack::PopAndDestroy(13);
	CleanupStack::PopAndDestroy(2, this); // this

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QNPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::NotifyProfileChanged
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::NotifyProfileChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestNotifyProfileChanged0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TName profileName;
	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);
	
	TRequestStatus requestNotify;
	TRequestStatus mockLtsyStatus;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketQoS::NotifyProfileChanged when result is not cached.
 	//-------------------------------------------------------------------------

	RPacketQoS::TQoSR5Negotiated qosR5Negotiated;
	TPckg< RPacketQoS::TQoSR5Negotiated > pckgQoSR5Negotiated(qosR5Negotiated);

	packetQoS.NotifyProfileChanged(requestNotify, pckgQoSR5Negotiated);
 	
	ChangeProfileDataL<RPacketQoS::TQoSR5Requested>(packetContext, packetQoS, contextName, data);

	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestNotify.Int());
	
	ASSERT_EQUALS(qosR5Negotiated.iSignallingIndication        , 1                                            );
	ASSERT_EQUALS(qosR5Negotiated.iSourceStatisticsDescriptor  , RPacketQoS::ESourceStatisticsDescriptorSpeech);
	ASSERT_EQUALS(qosR5Negotiated.iTrafficClass                , RPacketQoS::ETrafficClassStreaming           );
	ASSERT_EQUALS(qosR5Negotiated.iDeliveryOrderReqd           , RPacketQoS::EDeliveryOrderRequired           );
	ASSERT_EQUALS(qosR5Negotiated.iDeliverErroneousSDU         , RPacketQoS::EErroneousSDUDeliveryRequired    );
	ASSERT_EQUALS(qosR5Negotiated.iMaxSDUSize                  , 0x100                                        );
	ASSERT_EQUALS(qosR5Negotiated.iMaxRate.iUplinkRate         , 10000                                        );
	ASSERT_EQUALS(qosR5Negotiated.iMaxRate.iDownlinkRate       , 10000                                        );
	ASSERT_EQUALS(qosR5Negotiated.iBER                         , RPacketQoS::EBEROnePerHundredThousand        );
	ASSERT_EQUALS(qosR5Negotiated.iSDUErrorRatio               , RPacketQoS::ESDUErrorRatioOnePerTenThousand  );
	ASSERT_EQUALS(qosR5Negotiated.iTrafficHandlingPriority     , RPacketQoS::ETrafficPriority2                );
	ASSERT_EQUALS(qosR5Negotiated.iTransferDelay               , 1000                                         );
	ASSERT_EQUALS(qosR5Negotiated.iGuaranteedRate.iDownlinkRate, 1000                                         );
	ASSERT_EQUALS(qosR5Negotiated.iGuaranteedRate.iUplinkRate  , 1000                                         );
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketQoS::NotifyProfileChanged
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketQoSNotifyProfileChanged, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // packetService, packetContext, packetQoS
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QNPC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketQoS::NotifyProfileChanged
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketQoS::NotifyProfileChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestNotifyProfileChanged0002L()
	{
    
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketQoS::NotifyProfileChanged
 	//-------------------------------------------------------------------------
	RPacketQoS::TQoSGPRSNegotiated qosGPRSNegotiated;
	TPckg< RPacketQoS::TQoSGPRSNegotiated > pckgQoSGPRSNegotiated(qosGPRSNegotiated);

	TRequestStatus requestNotify;
	packetQoS.NotifyProfileChanged(requestNotify, pckgQoSGPRSNegotiated);	

    packetQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);

	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, requestNotify.Int());

    //-------------------------------------------------------------------------
    // Test change of profile in the LTSY when the Notification request has
	// been cancelled by the client
    //-------------------------------------------------------------------------
	RPacketQoS::TQoSR5Negotiated qosR5Negotiated;
	TPckg< RPacketQoS::TQoSR5Negotiated > pckgQoSR5Negotiated(qosR5Negotiated);

	ChangeProfileDataL<RPacketQoS::TQoSR5Requested>(packetContext, packetQoS, contextName, data);

    TRequestStatus requestGet;
	packetQoS.GetProfileParameters(requestGet, pckgQoSR5Negotiated);
	
	User::WaitForRequest(requestGet);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestGet.Int());
	    
	ASSERT_EQUALS(qosR5Negotiated.iSignallingIndication        , 1                                            );
	ASSERT_EQUALS(qosR5Negotiated.iSourceStatisticsDescriptor  , RPacketQoS::ESourceStatisticsDescriptorSpeech);
	ASSERT_EQUALS(qosR5Negotiated.iTrafficClass                , RPacketQoS::ETrafficClassStreaming           );
	ASSERT_EQUALS(qosR5Negotiated.iDeliveryOrderReqd           , RPacketQoS::EDeliveryOrderRequired           );
	ASSERT_EQUALS(qosR5Negotiated.iDeliverErroneousSDU         , RPacketQoS::EErroneousSDUDeliveryRequired    );
	ASSERT_EQUALS(qosR5Negotiated.iMaxSDUSize                  , 0x100                                        );
	ASSERT_EQUALS(qosR5Negotiated.iMaxRate.iUplinkRate         , 10000                                        );
	ASSERT_EQUALS(qosR5Negotiated.iMaxRate.iDownlinkRate       , 10000                                        );
	ASSERT_EQUALS(qosR5Negotiated.iBER                         , RPacketQoS::EBEROnePerHundredThousand        );
	ASSERT_EQUALS(qosR5Negotiated.iSDUErrorRatio               , RPacketQoS::ESDUErrorRatioOnePerTenThousand  );
	ASSERT_EQUALS(qosR5Negotiated.iTrafficHandlingPriority     , RPacketQoS::ETrafficPriority2                );
	ASSERT_EQUALS(qosR5Negotiated.iTransferDelay               , 1000                                         );
	ASSERT_EQUALS(qosR5Negotiated.iGuaranteedRate.iDownlinkRate, 1000                                         );
	ASSERT_EQUALS(qosR5Negotiated.iGuaranteedRate.iUplinkRate  , 1000                                         );

	TRequestStatus requestNotify1;
	packetQoS.NotifyProfileChanged(requestNotify1, pckgQoSGPRSNegotiated);   

    ChangeProfileDataL<RPacketQoS::TQoSGPRSRequested>(packetContext, packetQoS, contextName, data);

	User::WaitForRequest(requestNotify1);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestNotify1.Int());

	CleanupStack::PopAndDestroy(3); // packetService, packetContext, packetQoS
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QNPC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::NotifyProfileChanged with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::NotifyProfileChanged with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestNotifyProfileChanged0003L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestNotify;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketQoS::NotifyProfileChanged
 	//-------------------------------------------------------------------------

	RPacketQoS::TQoSCDMA2000Negotiated qosCDMA2000Negotiated;
	TPckg< RPacketQoS::TQoSCDMA2000Negotiated > pckgQoSCDMA2000Negotiated = qosCDMA2000Negotiated;

	packetQoS.NotifyProfileChanged(requestNotify, pckgQoSCDMA2000Negotiated);	

	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestNotify.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketQoS::NotifyProfileChanged
 	//-------------------------------------------------------------------------

	TBuf8<1> nullDes;

	packetQoS.NotifyProfileChanged(requestNotify, nullDes);	

	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestNotify.Int());
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketQoS::NotifyProfileChanged
 	//-------------------------------------------------------------------------
 	
	TPacketDataConfigBase packetGPRS;
	TPckg< TPacketDataConfigBase > outOfBoundParameter = packetGPRS;
 	
	packetQoS.NotifyProfileChanged(requestNotify, outOfBoundParameter);	

	User::WaitForRequest(requestNotify);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestNotify.Int());

	CleanupStack::PopAndDestroy(3); // packetService, packetContext, packetQoS
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QNPC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketQoS::NotifyProfileChanged
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketQoS::NotifyProfileChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestNotifyProfileChanged0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;
	TName profileName2;
	TName profileName3;

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	// Open third client
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);

	RMobilePhone phone2;
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobilePhone phone3;
	ret = phone3.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	RPacketService packetService3;                
	ret = packetService3.Open(phone3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService3);

	// Open first context
	TInfoName contextId;	
	RPacketContext packetContext;
	ret = packetContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	// Open second context
	TInfoName contextId2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewContext(packetService2, contextId2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	// Open third context
	TInfoName contextId3;	
	RPacketContext packetContext3;
	ret = packetContext3.OpenNewContext(packetService3, contextId3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext3);

	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	RPacketQoS packetQoS2;
	ret = packetQoS2.OpenNewQoS(packetContext2, profileName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS2);

	RPacketQoS packetQoS3;
	ret = packetQoS3.OpenNewQoS(packetContext3, profileName3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS3);

	TRequestStatus requestNotify1;
	TRequestStatus requestNotify2;
	TRequestStatus requestNotify3;

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketQoS::NotifyProfileChanged
 	//-------------------------------------------------------------------------

	// first client
	RPacketQoS::TQoSGPRSNegotiated qosGPRSNegotiated;
	TPckg< RPacketQoS::TQoSGPRSNegotiated > pckgQoSGPRSNegotiated(qosGPRSNegotiated);
	packetQoS.NotifyProfileChanged(requestNotify1, pckgQoSGPRSNegotiated);	

	// second client
	RPacketQoS::TQoSR99_R4Negotiated qosR99_R4Negotiated;
	TPckg< RPacketQoS::TQoSR99_R4Negotiated > pckgQoSR99_R4Negotiated(qosR99_R4Negotiated);
	packetQoS2.NotifyProfileChanged(requestNotify2, pckgQoSR99_R4Negotiated);	

	// third client
	RPacketQoS::TQoSR5Negotiated qosR5Negotiated;
	TPckg< RPacketQoS::TQoSR5Negotiated > pckgQoSR5Negotiated(qosR5Negotiated);
	packetQoS3.NotifyProfileChanged(requestNotify3, pckgQoSR5Negotiated);	

	ChangeProfileDataL<RPacketQoS::TQoSGPRSRequested>(packetContext, packetQoS, contextId, data);
	User::WaitForRequest(requestNotify1);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestNotify1.Int());

	ChangeProfileDataL<RPacketQoS::TQoSR99_R4Requested>(packetContext2, packetQoS2, contextId2, data);
	User::WaitForRequest(requestNotify2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestNotify2.Int());

	ChangeProfileDataL<RPacketQoS::TQoSR5Requested>(packetContext3, packetQoS3, contextId3, data);
	User::WaitForRequest(requestNotify3);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestNotify3.Int());
	
	CleanupStack::PopAndDestroy(13);
	CleanupStack::PopAndDestroy(2, this); // this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QSPP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::SetProfileParameters
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::SetProfileParameters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestSetProfileParameters0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	RPacketContext::TContextStatus contextStatus;
	
	RPacketQoS::TQoSGPRSRequested packetGPRS;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS = packetGPRS;
	
	packetGPRS.iReqPrecedence     = RPacketQoS::EPriorityLowPrecedence;
	packetGPRS.iMinPrecedence     = RPacketQoS::EPriorityLowPrecedence;
	packetGPRS.iReqDelay          = RPacketQoS::EDelay360ms;
	packetGPRS.iMinDelay          = RPacketQoS::EDelay40ms;
	packetGPRS.iReqReliability    = RPacketQoS::EReliabilityClass3;
	packetGPRS.iMinReliability    = RPacketQoS::EReliabilityClass4;
	packetGPRS.iReqPeakThroughput = RPacketQoS::EPeakThroughput32000;
	packetGPRS.iMinPeakThroughput = RPacketQoS::EPeakThroughput32000;
	packetGPRS.iReqMeanThroughput = RPacketQoS::EMeanThroughput500000;
	packetGPRS.iMinMeanThroughput = RPacketQoS::EMeanThroughput1000000;

	packetContext.GetStatus(contextStatus);
	
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;
	
	packetContext.GetConfig(requestStatus, pckgContextConfig);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName;
	
    TContextParams	contextParams;
	contextParams.iContextName = contextName;
	contextParams.iContextType = contextConfig.iNWIContext;
	contextParams.iPdpType = contextConfig.iPdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;

	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect(contextParams, packetGPRS);
    expExpect.SerialiseL(expectData);

    TMockLtsyData1< TInfoName > contexName(contextName);
    contexName.SerialiseL(completeData);
    
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrGeneral, completeData);

	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketQoS::SetProfileParameters when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, completeData);

	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketQoS::SetProfileParameters
	// from LTSY.
 	//-------------------------------------------------------------------------

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, completeData, 0);
    User::WaitForRequest(mockLtsyStatus);        
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());  

 	//-------------------------------------------------------------------------
	// TEST for coverage increasing 
 	//-------------------------------------------------------------------------

 	struct TContextMisc completeMisc;
    completeMisc.iStatus = RPacketContext::EStatusActivating;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData(contextName, completeMisc);
	ltsyData.SerialiseL(data);	

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//refresh data for ExpectL, as iStatus was changed
	packetContext.GetStatus(contextStatus);
	contextParams.iContextStatus = contextStatus;
	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect2(contextParams, packetGPRS);
    expectData.Close();
    expExpect2.SerialiseL(expectData);

	
    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData);
   	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST coverage increasing 
 	//-------------------------------------------------------------------------

    completeMisc.iStatus = RPacketContext::EStatusActive;
 	TMockLtsyData2 <TInfoName, TContextMisc> ltsyData2(contextName, completeMisc);
	data.Close();
	ltsyData2.SerialiseL(data);	

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketContextNotifyStatusChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	//refresh data for ExpectL, as iStatus was changed
	packetContext.GetStatus(contextStatus);
	contextParams.iContextStatus = contextStatus;
	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect3(contextParams, packetGPRS);
    expectData.Close();
    expExpect3.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData);
    //iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, data);

	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	
    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData, KErrNotSupported);

	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();

	
	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(4, this); // data, this,expectData, completeData
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QSPP-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketQoS::SetProfileParameters
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketQoS::SetProfileParameters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestSetProfileParameters0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestStatus;
	
	RPacketQoS::TQoSGPRSRequested packetGPRS;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS = packetGPRS;

	RPacketContext::TContextStatus contextStatus;
	packetContext.GetStatus(contextStatus);
	
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;
	
	packetContext.GetConfig(requestStatus, pckgContextConfig);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName;
	
    TContextParams	contextParams;
	contextParams.iContextName = contextName;
	contextParams.iContextType = contextConfig.iNWIContext;
	contextParams.iPdpType = contextConfig.iPdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;

	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect(contextParams, packetGPRS);
    expExpect.SerialiseL(expectData);

    TMockLtsyData1< TInfoName > contexName(contextName);
    contexName.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketQoS::SetProfileParameters
 	//-------------------------------------------------------------------------
 	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, data, 10);
    
	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	

    packetQoS.CancelAsyncRequest(EPacketQoSSetProfileParams);

	User::WaitForRequest(requestStatus);
	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	User::WaitForRequest(mockLtsyStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(3, this); // data, this, expectData
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QSPP-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::SetProfileParameters with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::SetProfileParameters with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestSetProfileParameters0003L()
	{
    
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestStatus;
	
	RPacketContext::TContextStatus contextStatus;
	packetContext.GetStatus(contextStatus);
	
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;
	
	packetContext.GetConfig(requestStatus, pckgContextConfig);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName;
	
    TContextParams	contextParams;
	contextParams.iContextName = contextName;
	contextParams.iContextType = contextConfig.iNWIContext;
	contextParams.iPdpType = contextConfig.iPdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;
	
    TMockLtsyData1< TInfoName > dataContextName(contextName);
    dataContextName.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------

	// wrong version of parameter
	RPacketQoS::TQoSCDMA2000Requested packetCDMA2000;
	TPckg< RPacketQoS::TQoSCDMA2000Requested > pckgCDMA2000 = packetCDMA2000;

	packetQoS.SetProfileParameters(requestStatus, pckgCDMA2000);	

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------

	TBuf8<1> nullDescriptor;

	packetQoS.SetProfileParameters(requestStatus, nullDescriptor);	

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketQoS::SetProfileParameters
 	//-------------------------------------------------------------------------
 	
	TPacketDataConfigBase packetGPRS;
	TPckg< TPacketDataConfigBase > parameter = packetGPRS;

	packetQoS.SetProfileParameters(requestStatus, parameter);	

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QSPP-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketQoS::SetProfileParameters
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketQoS::SetProfileParameters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestSetProfileParameters0004L()
	{		
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data1;
	CleanupClosePushL(data1);

	RBuf8 data2;
	CleanupClosePushL(data2);

	RBuf8 data3;
	CleanupClosePushL(data3);

	TName profileName1;
	TName profileName2;
	TName profileName3;

	RBuf8 expectData1;
	CleanupClosePushL(expectData1);

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

	// Open third client
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);

	RMobilePhone phone3;
	ret = phone3.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	RPacketService packetService1;
	OpenPacketServiceL(packetService1);
	CleanupClosePushL(packetService1);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService2);

	RPacketService packetService3;                
	ret = packetService3.Open(phone3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetService3);
	
	TInfoName contextName1;	
	RPacketContext packetContext1;
	ret = packetContext1.OpenNewContext(packetService1, contextName1);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext1);

	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewContext(packetService2, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);

	TInfoName contextName3;	
	RPacketContext packetContext3;
	ret = packetContext3.OpenNewContext(packetService3, contextName3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext3);
	
	RPacketQoS packetQoS1;
	ret = packetQoS1.OpenNewQoS(packetContext1, profileName1);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS1);

	RPacketQoS packetQoS2;
	ret = packetQoS2.OpenNewQoS(packetContext2, profileName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS2);

	RPacketQoS packetQoS3;
	ret = packetQoS3.OpenNewQoS(packetContext3, profileName3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS3);

	TRequestStatus requestStatus;

	// for packetContext1
	RPacketQoS::TQoSGPRSRequested packetGPRS;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS = packetGPRS;

	RPacketContext::TContextStatus contextStatus1;
	packetContext1.GetStatus(contextStatus1);
	
	RPacketContext::TContextConfigGPRS contextConfig1;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig1 = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig1;
	
	packetContext1.GetConfig(requestStatus, pckgContextConfig1);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName1;
	
    TContextParams	contextParams1;
	contextParams1.iContextName = contextName1;
	contextParams1.iContextType = contextConfig1.iNWIContext;
	contextParams1.iPdpType = contextConfig1.iPdpType;
	contextParams1.iPrimaryContextName = primaryContextName1;
	contextParams1.iContextStatus = contextStatus1;

	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect1(contextParams1, packetGPRS);
    expExpect1.SerialiseL(expectData1);

    TMockLtsyData1< TInfoName > dataContextName1(contextName1);
    dataContextName1.SerialiseL(data1);

	// for packetContext2
	RPacketQoS::TQoSR99_R4Requested packetR99_R4;
	TPckg< RPacketQoS::TQoSR99_R4Requested > profilePckgR99_R4 = packetR99_R4;
	
	RPacketContext::TContextStatus contextStatus2;
	packetContext2.GetStatus(contextStatus2);
	
	RPacketContext::TContextConfigR99_R4 contextConfig2;
	TPckg< RPacketContext::TContextConfigR99_R4 > pckgContextConfig2 = 
	                   ( TPckg< RPacketContext::TContextConfigR99_R4 > ) contextConfig2;
	
	packetContext2.GetConfig(requestStatus, pckgContextConfig2);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName2;
	
    TContextParams	contextParams2;
	contextParams2.iContextName        = contextName2;
	contextParams2.iContextType        = contextConfig2.iNWIContext;
	contextParams2.iPdpType            = contextConfig2.iPdpType;
	contextParams2.iPrimaryContextName = primaryContextName2;
	contextParams2.iContextStatus      = contextStatus2;

	TMockLtsyData2<TContextParams, RPacketQoS::TQoSR99_R4Requested> 
	                    expExpect2(contextParams2, packetR99_R4);
    expExpect2.SerialiseL(expectData2);

    TMockLtsyData1< TInfoName > dataContextName2(contextName2);
    dataContextName2.SerialiseL(data2);

	// for packetContext3
	RPacketQoS::TQoSR5Requested packetR5;
	TPckg< RPacketQoS::TQoSR5Requested > profilePckgR5 = packetR5;
	
	RPacketContext::TContextStatus contextStatus3;
	packetContext3.GetStatus(contextStatus3);
	
	RPacketContext::TContextConfigGPRS contextConfig3;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig3 = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig3;
	
	packetContext3.GetConfig(requestStatus, pckgContextConfig3);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName3;
	
    TContextParams	contextParams3;
	contextParams3.iContextName = contextName3;
	contextParams3.iContextType = contextConfig3.iNWIContext;
	contextParams3.iPdpType = contextConfig3.iPdpType;
	contextParams3.iPrimaryContextName = primaryContextName3;
	contextParams3.iContextStatus = contextStatus3;

	TMockLtsyData2<TContextParams, RPacketQoS::TQoSR5Requested> 
	                    expExpect3(contextParams3, packetR5);
    expExpect3.SerialiseL(expectData3);

    TMockLtsyData1< TInfoName > dataContextName3(contextName3);
    dataContextName3.SerialiseL(data3);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketQoS::SetProfileParameters
 	//-------------------------------------------------------------------------

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TRequestStatus requestStatus3;

    iMockLTSY.ExpectL  (EPacketQoSSetProfileParams, expectData1);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, data1);
	packetQoS1.SetProfileParameters(requestStatus1, profilePckgGPRS);	

    iMockLTSY.ExpectL  (EPacketQoSSetProfileParams, expectData2);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, data2);
	packetQoS2.SetProfileParameters(requestStatus2, profilePckgR99_R4);	

    iMockLTSY.ExpectL  (EPacketQoSSetProfileParams, expectData3);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, data3);
	packetQoS3.SetProfileParameters(requestStatus3, profilePckgR5);	

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	User::WaitForRequest(requestStatus3);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());

	CleanupStack::PopAndDestroy(3);       // packetQoS1, packetQoS2, packetQoS3
	CleanupStack::PopAndDestroy(3);       // packetContext1, packetContext2, packetContext3
	CleanupStack::PopAndDestroy(3);       // packetService1, packetService2, packetService3
	CleanupStack::PopAndDestroy(4);       // telServer2, telServer3
	                                      // phone2, phone3
	CleanupStack::PopAndDestroy(7, this); // data1, data2, data3, expectData1,
	                                      // expectData2, expectData3,
	                                      // this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QSPP-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::SetProfileParameters with timeout
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::SetProfileParameters and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestSetProfileParameters0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RBuf8 expectData;
	CleanupClosePushL(expectData);

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestStatus;
	
	RPacketQoS::TQoSGPRSRequested packetGPRS;
	TPckg< RPacketQoS::TQoSGPRSRequested > profilePckgGPRS = packetGPRS;

	RPacketContext::TContextStatus contextStatus;
	packetContext.GetStatus(contextStatus);
	
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;
	
	packetContext.GetConfig(requestStatus, pckgContextConfig);
	User::WaitForRequest(requestStatus);

	TInfoName primaryContextName;
	
    TContextParams	contextParams;
	contextParams.iContextName = contextName;
	contextParams.iContextType = contextConfig.iNWIContext;
	contextParams.iPdpType = contextConfig.iPdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;

	TMockLtsyData2<TContextParams, RPacketQoS::TQoSGPRSRequested> 
	                    expExpect(contextParams, packetGPRS);
    expExpect.SerialiseL(expectData);
    
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RPacketQoS::SetProfileParameters
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, expectData);

	packetQoS.SetProfileParameters(requestStatus, profilePckgGPRS);	

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	CleanupStack::PopAndDestroy(3); // packetServicem, packetContext, packetQoS
	CleanupStack::PopAndDestroy(3, this); // data, this, expectData
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QGPC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::GetProfileCapabilities
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::GetProfileCapabilities
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetProfileCapabilities0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketQoS::GetProfileCapabilities when result is not cached.
 	//-------------------------------------------------------------------------

	RPacketQoS::TQoSCapsGPRS capsGPRS;
	TPckg<RPacketQoS::TQoSCapsGPRS> capsGPRSPckg(capsGPRS);
	packetQoS.GetProfileCapabilities(requestStatus, capsGPRSPckg);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(capsGPRS.iPrecedence, KMmPrecedence);
    ASSERT_EQUALS(capsGPRS.iDelay, KMmDelay);
    ASSERT_EQUALS(capsGPRS.iReliability, KMmReliability);
    ASSERT_EQUALS(capsGPRS.iPeak, KMmPeak);
    ASSERT_EQUALS(capsGPRS.iMean, KMmMean);
	
	RPacketQoS::TQoSCapsR99_R4 capsR99_R4;
	TPckg<RPacketQoS::TQoSCapsR99_R4> capsR99_R4Pckg(capsR99_R4);
	packetQoS.GetProfileCapabilities(requestStatus, capsR99_R4Pckg);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(capsR99_R4.iTrafficClass, KMmTrafficClass);
    ASSERT_EQUALS(capsR99_R4.iDeliveryOrderReqd, KMmDeliveryOrderReqd);
    ASSERT_EQUALS(capsR99_R4.iDeliverErroneousSDU, KMmErroneousSDUDelivery);
    ASSERT_EQUALS(capsR99_R4.iBER, KMmBitErrorRatio);
    ASSERT_EQUALS(capsR99_R4.iSDUErrorRatio, KMmSDUErrorRatio);
    ASSERT_EQUALS(capsR99_R4.iTrafficHandlingPriority, KMmTrafficHandlingPriority);
	
	RPacketQoS::TQoSCapsR5 capsR5;
	TPckg<RPacketQoS::TQoSCapsR5> capsR5Pckg(capsR5);
	packetQoS.GetProfileCapabilities(requestStatus, capsR5Pckg);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(capsR5.iTrafficClass, KMmTrafficClass);
    ASSERT_EQUALS(capsR5.iDeliveryOrderReqd, KMmDeliveryOrderReqd);
    ASSERT_EQUALS(capsR5.iDeliverErroneousSDU, KMmErroneousSDUDelivery);
    ASSERT_EQUALS(capsR5.iBER, KMmBitErrorRatio);
    ASSERT_EQUALS(capsR5.iSDUErrorRatio, KMmSDUErrorRatio);
    ASSERT_EQUALS(capsR5.iTrafficHandlingPriority, KMmTrafficHandlingPriority);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QGPC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::GetProfileCapabilities with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::GetProfileCapabilities with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetProfileCapabilities0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketQoS::GetProfileCapabilities
 	//-------------------------------------------------------------------------

	RPacketQoS::TQoSCapsCDMA2000 capsCDMA2000;
	TPckg<RPacketQoS::TQoSCapsCDMA2000> capsCDMA2000Pckg(capsCDMA2000);

	packetQoS.GetProfileCapabilities(requestStatus, capsCDMA2000Pckg);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketQoS::GetProfileCapabilities
 	//-------------------------------------------------------------------------

	TBuf8<1> nullDescriptor;
	packetQoS.GetProfileCapabilities(requestStatus, nullDescriptor);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QGPC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketQoS::GetProfileCapabilities
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketQoS::GetProfileCapabilities
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetProfileCapabilities0004L()
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

	// Open 3rd client
	RTelServer telServer3;
	ret = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer3);

	RMobilePhone phone3;
	ret = phone3.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone3);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketQoS::GetProfileCapabilities
 	//-------------------------------------------------------------------------

	TName profileName;

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextName;	
	RPacketContext packetContext;
	ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	RPacketQoS packetQoS;
	ret = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS);

	TName profileName2;

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);	
	CleanupClosePushL(packetService2);

	// Open new context
	TInfoName contextName2;	
	RPacketContext packetContext2;
	ret = packetContext2.OpenNewContext(packetService2, contextName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext2);
	
	RPacketQoS packetQoS2;
	ret = packetQoS2.OpenNewQoS(packetContext2, profileName2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS2);

	TName profileName3;

	RPacketService packetService3;                
	ret = packetService3.Open(phone3);
	ASSERT_EQUALS(KErrNone, ret);	
	CleanupClosePushL(packetService3);

	// Open new context
	TInfoName contextName3;	
	RPacketContext packetContext3;
	ret = packetContext3.OpenNewContext(packetService3, contextName3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext3);
	
	RPacketQoS packetQoS3;
	ret = packetQoS3.OpenNewQoS(packetContext3, profileName3);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetQoS3);

	TRequestStatus requestStatus;

	RPacketQoS::TQoSCapsGPRS capsGPRS;
	TPckg<RPacketQoS::TQoSCapsGPRS> capsGPRSPckg(capsGPRS);
	packetQoS.GetProfileCapabilities(requestStatus, capsGPRSPckg);

	TRequestStatus requestStatus2;
	
	RPacketQoS::TQoSCapsR99_R4 capsR99_R4;
	TPckg<RPacketQoS::TQoSCapsR99_R4> capsR99_R4Pckg(capsR99_R4);
	packetQoS2.GetProfileCapabilities(requestStatus2, capsR99_R4Pckg);

	TRequestStatus requestStatus3;

	RPacketQoS::TQoSCapsR5 capsR5;
	TPckg<RPacketQoS::TQoSCapsR5> capsR5Pckg(capsR5);
	packetQoS3.GetProfileCapabilities(requestStatus3, capsR5Pckg);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(capsGPRS.iPrecedence, KMmPrecedence);
    ASSERT_EQUALS(capsGPRS.iDelay, KMmDelay);
    ASSERT_EQUALS(capsGPRS.iReliability, KMmReliability);
    ASSERT_EQUALS(capsGPRS.iPeak, KMmPeak);
    ASSERT_EQUALS(capsGPRS.iMean, KMmMean);

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(capsR99_R4.iTrafficClass, KMmTrafficClass);
    ASSERT_EQUALS(capsR99_R4.iDeliveryOrderReqd, KMmDeliveryOrderReqd);
    ASSERT_EQUALS(capsR99_R4.iDeliverErroneousSDU, KMmErroneousSDUDelivery);
    ASSERT_EQUALS(capsR99_R4.iBER, KMmBitErrorRatio);
    ASSERT_EQUALS(capsR99_R4.iSDUErrorRatio, KMmSDUErrorRatio);
    ASSERT_EQUALS(capsR99_R4.iTrafficHandlingPriority, KMmTrafficHandlingPriority);

	User::WaitForRequest(requestStatus3);
	ASSERT_EQUALS(KErrNone, requestStatus3.Int());
    ASSERT_EQUALS(capsR5.iTrafficClass, KMmTrafficClass);
    ASSERT_EQUALS(capsR5.iDeliveryOrderReqd, KMmDeliveryOrderReqd);
    ASSERT_EQUALS(capsR5.iDeliverErroneousSDU, KMmErroneousSDUDelivery);
    ASSERT_EQUALS(capsR5.iBER, KMmBitErrorRatio);
    ASSERT_EQUALS(capsR5.iSDUErrorRatio, KMmSDUErrorRatio);
    ASSERT_EQUALS(capsR5.iTrafficHandlingPriority, KMmTrafficHandlingPriority);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // packetQoS3, packetContext3, packetService3,
	CleanupStack::PopAndDestroy(3); // packetQoS2, packetContext2, packetService2,
	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(6, this); // phone3, telServer3, phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCCNT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::CreateNewTFT
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::CreateNewTFT
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestCreateNewTFT0001L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt ret = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;
	
	packetContext.CreateNewTFT(requestStatus, 8);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2);
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAPF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::AddPacketFilter
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::AddPacketFilter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddPacketFilter0001L()
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
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::AddPacketFilter
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextAddPacketFilter, KErrNone);
	// wait for completion
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RPacketContext::TPacketFilterV2 contextFilter;
	contextFilter.iId = 1;// 1 - 8
	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);
	
	RPacketContext::TContextConfigGPRS configGPRS;
	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );
	CleanupStack::PushL(filterArray);

	contextFilter.iId = 1;
	filterArray->AppendL( contextFilter );

	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
			packetFilterData( configGPRS, 0, secContextId, filterArray );

	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data, KErrNotSupported);

	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);

	User::WaitForRequest(requestStatus);
	//ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50101);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName(secContextId);
    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrGeneral, data);

	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::AddPacketFilter when result is not cached.
 	//-------------------------------------------------------------------------

	SetContextActiveL(secContextId);
	AssertMockLtsyStatusL();

	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	SetContextStatusL(secContextId, RPacketContext::EStatusUnknown);
	AssertMockLtsyStatusL();

	filterArray->AppendL( contextFilter );
	contextFilter.iId = 2;
	filterArray->AppendL( contextFilter );
	data.Close();
	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);

	User::WaitForRequest(requestStatus);
	//ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50101);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST D: RPacketContext::AddPacketFilter again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	filterArray->Delete(0);
	contextFilter.iId = 4;
	filterArray->AppendL( contextFilter );
	data.Close();
	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);
	
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(4); // packetContext, packetPrimContext, packetService, filterArray
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAPF-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RPacketContext::AddPacketFilter
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RPacketContext::AddPacketFilter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddPacketFilter0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------
 	
	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RPacketContext::TPacketFilterV2 contextFilter;
	contextFilter.iId = 1;// 1 - 8
	//contextFilter.iEvaluationPrecedenceIndex = 0;
	
	RPacketContext::TContextConfigGPRS configGPRS;
	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );;
	CleanupStack::PushL(filterArray);
	filterArray->AppendL( contextFilter );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
			packetFilterData( configGPRS, 0, secContextId, filterArray );

	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName(secContextId);
    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data, 10);
	//iMockLTSY.CompleteL(EPacketContextAddPacketFilter, KErrNone, 10);

	TRequestStatus requestStatus;
	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	
	//packetContext.AddPacketFilterCancel();
	packetContext.CancelAsyncRequest(EPacketContextAddPacketFilter);
	packetContext.CancelAsyncRequest(EPacketContextSetConfig);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	 User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); // packetContext, packetPrimContext, packetService, filterArray
	CleanupStack::PopAndDestroy(2); // data, this	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAPF-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::AddPacketFilter with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::AddPacketFilter with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddPacketFilter0003L()
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
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);

	TRequestStatus requestStatus;
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------
 	
	RPacketContext::TPacketFilterV2 contextFilter;
	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);

	SetContextActiveL(secContextId);

	contextFilter.iId = 0;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 9;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 1;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	AssertMockLtsyStatusL();
	SetContextStatusL(secContextId, RPacketContext::EStatusActivating);

	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 0;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 9;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();
	SetContextStatusL(secContextId, RPacketContext::EStatusInactive);

	contextFilter.iId = 1;// 1 - 8
	packetPrimContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 0;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 9;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();
	SetContextStatusL(secContextId, RPacketContext::EStatusUnknown);

	contextFilter.iId = 1;// 1 - 8
	packetPrimContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 0;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	contextFilter.iId = 9;// 1 - 8
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------

	SetContextActiveL(secContextId);

	TBuf8<1> nullDescriptor;
	packetContext.AddPacketFilter(requestStatus, nullDescriptor);
	
	User::WaitForRequest(requestStatus);	
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(3); // packetContext, packetPrimContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCAPF-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::AddPacketFilter
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::AddPacketFilter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestAddPacketFilter0004L()
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
	// Test A: Test multiple clients requesting RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);

	// Open new context
	TInfoName contextId;	
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);

	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);	
	CleanupClosePushL(packetService2);

	// Open new context2
	TInfoName contextId2;	
	TInfoName secContextId2;	
	RPacketContext packetPrimContext2;
	err = packetPrimContext2.OpenNewContext(packetService2, contextId2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext2);

	RPacketContext packetContext2;
	err = packetContext2.OpenNewSecondaryContext(packetService2, contextId2, secContextId2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext2);

	TRequestStatus	requestStatus;
	TRequestStatus	requestStatus2;

	RPacketContext::TPacketFilterV2 contextFilter;
	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);
	contextFilter.iId = 1;// 1 - 8
	
	RPacketContext::TContextConfigGPRS configGPRS;
	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );;
	CleanupStack::PushL(filterArray);
	filterArray->AppendL( contextFilter );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
			packetFilterData( configGPRS, 0, secContextId, filterArray );
	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);
	
	TMockLtsyData1<TInfoName > contexName(secContextId);
    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);
	
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);

	RPacketContext::TPacketFilterV2 contextFilter2;
	RPacketContext::TPacketFilterV2Pckg contextFilterPckg2(contextFilter2);
	contextFilter2.iId = 2;// 1 - 8

	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray2 = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );;
	CleanupStack::PushL(filterArray2);
	filterArray2->AppendL( contextFilter2 );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* > 
			packetFilterData2( configGPRS, 0, secContextId2, filterArray2 );
	data.Close();
	packetFilterData2.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName2(secContextId2);
    data.Close();
    contexName2.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	packetContext2.AddPacketFilter(requestStatus2, contextFilterPckg2);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2);  // filterArray2, filterArray
	CleanupStack::PopAndDestroy(3); // packetContext2, packetPrimContext2, packetService2
	CleanupStack::PopAndDestroy(3); // packetContext, packetPrimContext, packetService
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCRPF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::RemovePacketFilter
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::RemovePacketFilter
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestRemovePacketFilter0001L()
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
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RPacketContext::RemovePacketFilter
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);	
	//send completion
	iMockLTSY.CompleteL(EPacketContextRemovePacketFilter, KErrNone);
	// wait for completion
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::RemovePacketFilter when result is not cached.
 	//-------------------------------------------------------------------------

	RPacketContext::TPacketFilterV2 contextFilter;
	contextFilter.iId = 1;// 1 - 8

	RPacketContext::TContextConfigGPRS configGPRS;
	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );;
	CleanupStack::PushL(filterArray);
	filterArray->AppendL( contextFilter );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >*  >
			packetFilterData( configGPRS, 0, secContextId, filterArray );
	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName(secContextId);
    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();

	packetContext.RemovePacketFilter(requestStatus, 1);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST D: RPacketContext::RemovePacketFilter again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	packetContext.RemovePacketFilter(requestStatus, 1);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(4); // packetContext, packetPrimContext, packetService, filterArray
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCRPF-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::RemovePacketFilter with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::RemovePacketFilter with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestRemovePacketFilter0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketContext::AddPacketFilter
 	//-------------------------------------------------------------------------

	RPacketService packetService;
	OpenPacketServiceL(packetService);
	CleanupClosePushL(packetService);
	
	// Open new context
	TInfoName contextId;	
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;

	packetContext.RemovePacketFilter(requestStatus, 0);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	packetContext.RemovePacketFilter(requestStatus, 1);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	packetContext.RemovePacketFilter(requestStatus, 9);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	CleanupStack::PopAndDestroy(3); // packetService, packetPrimContext, packetContext
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCGPFI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetPacketFilterInfo
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetPacketFilterInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetPacketFilterInfo0001L()
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
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketContext::GetPacketFilterInfo when result is not cached.
 	//-------------------------------------------------------------------------
	RPacketContext::TPacketFilterV2 contextFilter;
	contextFilter.iId = 1;// 1 - 8

	RPacketContext::TContextConfigGPRS configGPRS;
	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );;
	CleanupStack::PushL(filterArray);
	filterArray->AppendL( contextFilter );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* >
			packetFilterData( configGPRS, 0, secContextId, filterArray );
	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName(secContextId);
    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	RPacketContext::TPacketFilterV2 packetFilter;
	RPacketContext::TPacketFilterV2Pckg filterInfoPckg(packetFilter);
	packetContext.GetPacketFilterInfo(requestStatus, 0, filterInfoPckg);	
	User::WaitForRequest(requestStatus);

	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(contextFilter.iId                        , packetFilter.iId                        );
	ASSERT_EQUALS(contextFilter.iEvaluationPrecedenceIndex , packetFilter.iEvaluationPrecedenceIndex );
	ASSERT_EQUALS(0, memcmp(contextFilter.iSrcAddr, 
	                        packetFilter.iSrcAddr , 
	                        sizeof(RPacketContext::TIPAddress)));
	ASSERT_EQUALS(0, memcmp(contextFilter.iSrcAddrSubnetMask, 
	                        packetFilter.iSrcAddrSubnetMask, 
	                        sizeof(RPacketContext::TIPAddress)));
	ASSERT_EQUALS(contextFilter.iProtocolNumberOrNextHeader, packetFilter.iProtocolNumberOrNextHeader);
	ASSERT_EQUALS(contextFilter.iSrcPortMin                , packetFilter.iSrcPortMin                );
	ASSERT_EQUALS(contextFilter.iSrcPortMax                , packetFilter.iSrcPortMax                );
	ASSERT_EQUALS(contextFilter.iDestPortMin               , packetFilter.iDestPortMin               );
	ASSERT_EQUALS(contextFilter.iDestPortMax               , packetFilter.iDestPortMax               );
	ASSERT_EQUALS(contextFilter.iIPSecSPI                  , packetFilter.iIPSecSPI                  );
	ASSERT_EQUALS(contextFilter.iTOSorTrafficClass         , packetFilter.iTOSorTrafficClass         );
	ASSERT_EQUALS(contextFilter.iFlowLabel                 , packetFilter.iFlowLabel                 );
	
 	//-------------------------------------------------------------------------
	// TEST D: RPacketContext::GetPacketFilterInfo again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	packetContext.RemovePacketFilter(requestStatus, 1);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	iMockLTSY.NotifyTerminated(requestStatus);
	TBuf8<1> rejectionCode(1);
	TMockLtsyData2< TInfoName, TBuf8<1> > modifyContextData(secContextId, rejectionCode);
	data.Close();
	modifyContextData.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextModifyActiveContext, KErrNone, data);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	contextFilter.iId = 2;
	filterArray->Reset();
	filterArray->AppendL( contextFilter );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* >
			packetFilterData2( configGPRS, 0, secContextId, filterArray );
	data.Close();
	packetFilterData2.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	data.Close();
	contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	packetContext.GetPacketFilterInfo(requestStatus, 0, filterInfoPckg);	
	User::WaitForRequest(requestStatus);

	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(contextFilter.iId                        , packetFilter.iId                        );
	ASSERT_EQUALS(contextFilter.iEvaluationPrecedenceIndex , packetFilter.iEvaluationPrecedenceIndex );
	ASSERT_EQUALS(0, memcmp(contextFilter.iSrcAddr, 
	                        packetFilter.iSrcAddr , 
	                        sizeof(RPacketContext::TIPAddress)));
	ASSERT_EQUALS(0, memcmp(contextFilter.iSrcAddrSubnetMask, 
	                        packetFilter.iSrcAddrSubnetMask, 
	                        sizeof(RPacketContext::TIPAddress)));
	ASSERT_EQUALS(contextFilter.iProtocolNumberOrNextHeader, packetFilter.iProtocolNumberOrNextHeader);
	ASSERT_EQUALS(contextFilter.iSrcPortMin                , packetFilter.iSrcPortMin                );
	ASSERT_EQUALS(contextFilter.iSrcPortMax                , packetFilter.iSrcPortMax                );
	ASSERT_EQUALS(contextFilter.iDestPortMin               , packetFilter.iDestPortMin               );
	ASSERT_EQUALS(contextFilter.iDestPortMax               , packetFilter.iDestPortMax               );
	ASSERT_EQUALS(contextFilter.iIPSecSPI                  , packetFilter.iIPSecSPI                  );
	ASSERT_EQUALS(contextFilter.iTOSorTrafficClass         , packetFilter.iTOSorTrafficClass         );
	ASSERT_EQUALS(contextFilter.iFlowLabel                 , packetFilter.iFlowLabel                 );

	CleanupStack::PopAndDestroy(4); // packetContext, packetPrimContext, packetService, filterArray
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCGPFI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::GetPacketFilterInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::GetPacketFilterInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetPacketFilterInfo0003L()
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
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketContext::GetPacketFilterInfo
 	//-------------------------------------------------------------------------

	RPacketContext::TPacketFilterV2 packetFilter;
	RPacketContext::TPacketFilterV2Pckg filterInfoPckg(packetFilter);
	
	packetContext.GetPacketFilterInfo(requestStatus, 0, filterInfoPckg);	
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RPacketContext::GetPacketFilterInfo
 	//-------------------------------------------------------------------------

	TInt wrongPacketFilter;
	TPckg<TInt> wrongFilterInfoPckg(wrongPacketFilter);
	
	packetContext.GetPacketFilterInfo(requestStatus, 0, wrongFilterInfoPckg);	
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketContext::GetPacketFilterInfo
 	//-------------------------------------------------------------------------
	TBuf8<1> nullDescriptor;
	packetContext.GetPacketFilterInfo(requestStatus, 0, nullDescriptor);	
	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // packetContext, packetPrimContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-PCGPFI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPacketContext::GetPacketFilterInfo
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPacketContext::GetPacketFilterInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestGetPacketFilterInfo0004L()
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
	
	// Open new context
	TInfoName contextId;	
	TInfoName secContextId;	
	RPacketContext packetPrimContext;
	TInt err = packetPrimContext.OpenNewContext(packetService, contextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext);

	RPacketContext packetContext;
	err = packetContext.OpenNewSecondaryContext(packetService, contextId, secContextId);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;
	
	RPacketService packetService2;                
	ret = packetService2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);	
	CleanupClosePushL(packetService2);

	// Open new context
	TInfoName contextId2;	
	TInfoName secContextId2;	
	RPacketContext packetPrimContext2;
	err = packetPrimContext2.OpenNewContext(packetService2, contextId2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetPrimContext2);

	RPacketContext packetContext2;
	err = packetContext2.OpenNewSecondaryContext(packetService2, contextId2, secContextId2);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext2);
	
	TRequestStatus requestStatus2;
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RPacketContext::GetPacketFilterInfo
 	//-------------------------------------------------------------------------

	RPacketContext::TPacketFilterV2 contextFilter;
	contextFilter.iId = 1;// 1 - 8

	RPacketContext::TContextConfigGPRS configGPRS;
	CArrayFixFlat< RPacketContext::TPacketFilterV2 >* filterArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );;
	CleanupStack::PushL(filterArray);
	filterArray->AppendL( contextFilter );
	
	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >*  >
			packetFilterData( configGPRS, 0, secContextId, filterArray );
	packetFilterData.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName(secContextId);
    data.Close();
    contexName.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	RPacketContext::TPacketFilterV2Pckg contextFilterPckg(contextFilter);
	packetContext.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	//ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 50101);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	contextFilter.iId = 2;// 1 - 8
	filterArray->Reset();
	filterArray->AppendL( contextFilter );

	TMockLtsyContextData2< TInfoName, CArrayFixFlat< RPacketContext::TPacketFilterV2 >* >
			packetFilterData2( configGPRS, 0, secContextId2, filterArray );
	data.Close();
	packetFilterData2.SerialiseL(data);
	iMockLTSY.ExpectL(EPacketContextAddPacketFilter, data);

	TMockLtsyData1<TInfoName > contexName2(secContextId2);
    data.Close();
    contexName2.SerialiseL(data); 
	iMockLTSY.CompleteL(EPacketContextSetConfig, KErrNone, data);

	packetContext2.AddPacketFilter(requestStatus, contextFilterPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	RPacketContext::TPacketFilterV2 packetFilter;
	RPacketContext::TPacketFilterV2Pckg filterInfoPckg(packetFilter);
	packetContext.GetPacketFilterInfo(requestStatus, 0, filterInfoPckg);	

	RPacketContext::TPacketFilterV2 packetFilter2;
	RPacketContext::TPacketFilterV2Pckg filterInfoPckg2(packetFilter2);
	packetContext2.GetPacketFilterInfo(requestStatus2, 0, filterInfoPckg2);	

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	contextFilter.iId = 1;// 1 - 8
	ASSERT_EQUALS(contextFilterPckg, filterInfoPckg);

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	contextFilter.iId = 2;// 1 - 8
	ASSERT_EQUALS(contextFilterPckg, filterInfoPckg2);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); // packetContext2, packetPrimContext2, packetService2, filterArray
	CleanupStack::PopAndDestroy(3); // packetContext, packetPrimContext, packetService
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PKTQ-PCDT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketContext::DeleteTFT
@SYMTestPriority High
@SYMTestActions Invokes RPacketContext::DeleteTFT
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestDeleteTFT0001L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	packetContext.OpenNewContext(packetService, contextName);
	CleanupClosePushL(packetContext);
	
	TRequestStatus requestStatus;
	
	packetContext.DeleteTFT(requestStatus);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2);
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QONQ-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::OpenNewQoS
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::OpenNewQoS
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestOpenNewQoS0001L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketQoS::OpenNewQoS when result is not cached.
 	//-------------------------------------------------------------------------

	RPacketQoS packetQoS;
	TInfoName profileName;
	err = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, err);
	packetQoS.Close();
	
 	//-------------------------------------------------------------------------
	// TEST: Call OpenNewQoS without prior clothing
	// after successful completion of previous call
 	//-------------------------------------------------------------------------

	err = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, err);
	TInfoName profileName2;
	err = packetQoS.OpenNewQoS(packetContext, profileName2);
	// never get here because of etel panic 11 (EEtelPanicHandleNotClosed)
	ASSERT_TRUE( KErrNone != err );
	packetQoS.Close();
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QONQ-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::OpenNewQoS with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::OpenNewQoS with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestOpenNewQoS0003L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RPacketQoS::OpenNewQoS
 	//-------------------------------------------------------------------------

	TBuf<1> buf;
	RPacketQoS packetQoS;
	err = packetQoS.OpenNewQoS(packetContext, buf);
	ASSERT_EQUALS(KErrOverflow, err);
	packetQoS.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QOEQ-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::OpenExistingQoS
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::OpenExistingQoS
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestOpenExistingQoS0001L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	// Open new QoS
	RPacketQoS packetQoS;
	TInfoName profileName;
	err = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetQoS);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RPacketQoS::OpenExistingQoS when result is not cached.
 	//-------------------------------------------------------------------------

	RPacketQoS existingQoS;
	err = existingQoS.OpenExistingQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, err);
	existingQoS.Close();
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PKTQ-QOEQ-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::OpenExistingQoS with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::OpenExistingQoS with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestOpenExistingQoS0003L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	// Open new QoS
	RPacketQoS packetQoS;
	TInfoName profileName;
	err = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetQoS);

	//-------------------------------------------------------------------------
	// Test passing out of bounds parameters to
	// RPacketQoS::OpenExistingQoS
 	//-------------------------------------------------------------------------
 	
	TInfoName name;

	RPacketQoS existingQoS;
	name.Copy(_L("What's up"));
	err = existingQoS.OpenExistingQoS(packetContext, name);
	ASSERT_EQUALS(KErrNotFound, err);
	existingQoS.Close();

	name.Copy(_L(":"));
	err = existingQoS.OpenExistingQoS(packetContext, name);
	ASSERT_EQUALS(KErrNotFound, err);
	existingQoS.Close();

	name.Copy(_L("What's up:"));
	err = existingQoS.OpenExistingQoS(packetContext, name);
	ASSERT_EQUALS(KErrNotFound, err);
	existingQoS.Close();

	//-------------------------------------------------------------------------
	// Test passing wrong descriptor size to parameter in
	// RPacketQoS::OpenExistingQoS
 	//-------------------------------------------------------------------------

	TBuf<1> buf;
	err = existingQoS.OpenExistingQoS(packetContext, buf);
	ASSERT_EQUALS(KErrNotFound, err);
	existingQoS.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSY-PKTQ-QOEQ-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPacketQoS::OpenExistingQoS with out of bounds parameters
@SYMTestPriority High
@SYMTestActions Invokes RPacketQoS::OpenExistingQoS with out of bounds parameters
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPacketQoSFU::TestOpenExistingQoS0004L()
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
	TInfoName contextName;	
	RPacketContext packetContext;
	TInt err = packetContext.OpenNewContext(packetService, contextName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetContext);
	
	// Open new QoS
	RPacketQoS packetQoS;
	TInfoName profileName;
	err = packetQoS.OpenNewQoS(packetContext, profileName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(packetQoS);

	//-------------------------------------------------------------------------
	// Test passing out of bounds parameters to
	// RPacketQoS::OpenExistingQoS
 	//-------------------------------------------------------------------------
 	
	TInfoName name;

	RPacketQoS existingQoS;
	name.Copy(_L(":"));
	name.Append(profileName);
		
	err = existingQoS.OpenExistingQoS(packetContext, name);
	ASSERT_EQUALS(KErrNotFound, err);

	existingQoS.Close();
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // packetQoS, packetContext, packetService
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


template< class T >
void CCTsyPacketQoSFU::ChangeProfileDataL(RPacketContext& aPacketContext, RPacketQoS& aPacketQoS,
                                         TInfoName& aContextName, RBuf8& aData)
	{
	RBuf8 profileParamData1;
	CleanupClosePushL(profileParamData1);

	RBuf8 profileParamData2;
	CleanupClosePushL(profileParamData2);

	// call packetQoS.SetProfileParameters()
	//----------------------------------------------------------------------------
	TRequestStatus requestSetProfile;

	T qosRequested;
	TPckg< T > pckgQoSRequested(qosRequested);

	RPacketContext::TContextStatus contextStatus;
	aPacketContext.GetStatus(contextStatus);
	
	RPacketContext::TContextConfigGPRS contextConfig;
	TPckg< RPacketContext::TContextConfigGPRS > pckgContextConfig = 
	                   ( TPckg< RPacketContext::TContextConfigGPRS > ) contextConfig;
	
	aPacketContext.GetConfig(requestSetProfile, pckgContextConfig);
	User::WaitForRequest(requestSetProfile);

	TInfoName primaryContextName;
	
    TContextParams	contextParams;
	contextParams.iContextName = aContextName;
	contextParams.iContextType = contextConfig.iNWIContext;
	contextParams.iPdpType = contextConfig.iPdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;

	TMockLtsyData2<TContextParams, T> 
	                    expExpect(contextParams, qosRequested);
    expExpect.SerialiseL(profileParamData1);

    TMockLtsyData1< TInfoName > mockLTSYData(aContextName);
    mockLTSYData.SerialiseL(profileParamData2);

    iMockLTSY.ExpectL(EPacketQoSSetProfileParams, profileParamData1);
    iMockLTSY.CompleteL(EPacketQoSSetProfileParams, KErrNone, profileParamData2);

	aPacketQoS.SetProfileParameters(requestSetProfile, pckgQoSRequested);	

	User::WaitForRequest(requestSetProfile);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestSetProfile.Int());
	//----------------------------------------------------------------------------

	// call iMockLTSY.CompleteL(EPacketQoSNotifyProfileChanged, KErrNone, data);
	//----------------------------------------------------------------------------
	TPacketQoSGPRSNegotiated   profileCompleteQoSGPRS;
	TPacketQoSR99_R4Negotiated profileQoSR99_R4;
	TPacketQoSR5Negotiated     profileQoSR5;

	switch(qosRequested.ExtensionId())
		{
		case TPacketDataConfigBase::KConfigGPRS:
			{
			profileCompleteQoSGPRS.iPrecedence     = RPacketQoS::EPriorityHighPrecedence;
			profileCompleteQoSGPRS.iDelay          = RPacketQoS::EDelayClass3;
			profileCompleteQoSGPRS.iReliability    = RPacketQoS::EReliabilityClass2;
			profileCompleteQoSGPRS.iPeakThroughput = RPacketQoS::EPeakThroughput16000;
			profileCompleteQoSGPRS.iMeanThroughput = RPacketQoS::EMeanThroughput100000;
			}
			break;
		case TPacketDataConfigBase::KConfigRel99Rel4:
			{
			profileQoSR99_R4.iTrafficClass                 = RPacketQoS::ETrafficClassStreaming;			
			profileQoSR99_R4.iDeliveryOrderReqd            = RPacketQoS::EDeliveryOrderRequired;		
			profileQoSR99_R4.iDeliverErroneousSDU          = RPacketQoS::EErroneousSDUDeliveryRequired;	
			profileQoSR99_R4.iMaxSDUSize                   = 0x100;			
			profileQoSR99_R4.iMaxRate.iUplinkRate          = 10000;				
			profileQoSR99_R4.iMaxRate.iDownlinkRate        = 10000;				
			profileQoSR99_R4.iBER					       = RPacketQoS::EBEROnePerHundredThousand;
			profileQoSR99_R4.iSDUErrorRatio 			   = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
			profileQoSR99_R4.iTrafficHandlingPriority      = RPacketQoS::ETrafficPriority2;
			profileQoSR99_R4.iTransferDelay 			   = 1000;
			profileQoSR99_R4.iGuaranteedRate.iUplinkRate   = 1000;
			profileQoSR99_R4.iGuaranteedRate.iDownlinkRate = 1000;
			}
			break;
		case TPacketDataConfigBase::KConfigRel5:
			{
			profileQoSR5.iSignallingIndication         = ETrue;	
			profileQoSR5.iSourceStatisticsDescriptor   = RPacketQoS::ESourceStatisticsDescriptorSpeech;
			profileQoSR5.iTrafficClass                 = RPacketQoS::ETrafficClassStreaming;			
			profileQoSR5.iDeliveryOrderReqd            = RPacketQoS::EDeliveryOrderRequired;		
			profileQoSR5.iDeliverErroneousSDU          = RPacketQoS::EErroneousSDUDeliveryRequired;	
			profileQoSR5.iMaxSDUSize                   = 0x100;			
			profileQoSR5.iMaxRate.iUplinkRate          = 10000;				
			profileQoSR5.iMaxRate.iDownlinkRate        = 10000;				
			profileQoSR5.iBER					       = RPacketQoS::EBEROnePerHundredThousand;
			profileQoSR5.iSDUErrorRatio 			   = RPacketQoS::ESDUErrorRatioOnePerTenThousand;
			profileQoSR5.iTrafficHandlingPriority      = RPacketQoS::ETrafficPriority2;
			profileQoSR5.iTransferDelay 			   = 1000;
			profileQoSR5.iGuaranteedRate.iUplinkRate   = 1000;
			profileQoSR5.iGuaranteedRate.iDownlinkRate = 1000;
			}
			break;
		default:
			break;
		}

    TMockLtsyPacketQoSData1< TInfoName > mockData(profileCompleteQoSGPRS, 
                                                  profileQoSR99_R4, 
                                                  profileQoSR5, 
                                                  aContextName);
    aData.Close();
    mockData.SerialiseL(aData);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EPacketQoSNotifyProfileChanged, KErrNone, aData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	//----------------------------------------------------------------------------
	
	CleanupStack::PopAndDestroy(2);
	}
