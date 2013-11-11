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
// The TEFUnit test suite for UssdMessaging in the Common TSY.
// 
//

/**
 @file
*/

#include "cctsyussdmessagingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyUssdMessagingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessage0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessage0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessage0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessage0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessage0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendRelease0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendRelease0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendRelease0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendRelease0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendRelease0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendRelease0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestReceiveMessage0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestReceiveMessage0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestReceiveMessage0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestReceiveMessage0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestNotifyNetworkRelease0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestNotifyNetworkRelease0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestNotifyNetworkRelease0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestNotifyNetworkRelease0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessageNoFdnCheck0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessageNoFdnCheck0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessageNoFdnCheck0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessageNoFdnCheck0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestSendMessageNoFdnCheck0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestGetCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUssdMessagingFU, TestGetCaps0003L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-USSDM-USM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessage0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	attributes.iFlags  = flags;
	attributes.iFormat = format;
	attributes.iType   = type;
	attributes.iDcs   = dcs;
	
	TName name = _L("Name1");
	TPckg<TName> msgData(name);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name, attributes);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessage, data, KErrNotSupported);

	ussdMessaging.SendMessage(requestStatus, msgData, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessage, data);
    iMockLTSY.CompleteL(EMobileUssdMessagingSendMessage, KErrGeneral);

	ussdMessaging.SendMessage(requestStatus, msgData, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileUssdMessaging::SendMessage when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessage, data);
    iMockLTSY.CompleteL(EMobileUssdMessagingSendMessage, KErrNone);

	ussdMessaging.SendMessage(requestStatus, msgData, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileUssdMessaging::SendMessage
	// from LTSY.
 	//-------------------------------------------------------------------------

    TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendMessage, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST : to increase CMmUssdTsy::ExtFunc coverage
	// covers "if ( ERfsStateInfoInactive == iMmPhone->GetRfStateInfo() && 
	// ( !IsRequestPossibleInOffline( aIpc ) ) )" condition
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TRfStateInfo rfInfo = ERfsStateInfoInactive;
	TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
	
	data.Close();
	mockData1.SerialiseL(data);

	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	ussdMessaging.SendMessage(requestStatus, msgData, msgAttributes);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGsmOfflineOpNotAllowed, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileUssdMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileUssdMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessage0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	attributes.iFlags  = flags;
	attributes.iFormat = format;
	attributes.iType   = type;
	attributes.iDcs   = dcs;
	
	TName name = _L("Name1");
	TPckg<TName> msgData(name);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name, attributes);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileUssdMessaging::SendMessage
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessage, data);
    iMockLTSY.CompleteL(EMobileUssdMessagingSendMessage, KErrNone, 10);
	
	ussdMessaging.SendMessage(requestStatus, msgData, msgAttributes);
    ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingSendMessage);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // data, this, ussdMessaging
	
	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendMessage with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessage with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessage0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// function parameter

	TName name = _L("Name1");
	TPckg<TName> msgData(name);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileUssdMessaging::SendMessage
 	//-------------------------------------------------------------------------

	TBuf8<1> badSizeDescriptor;

	ussdMessaging.SendMessage(requestStatus, msgData, badSizeDescriptor);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	
	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging, ussdMessaging

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileUssdMessaging::SendMessage
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileUssdMessaging::SendMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessage0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileUssdMessaging ussdMessaging1;
	ret = ussdMessaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging1);

	RMobileUssdMessaging ussdMessaging2;
	ret = ussdMessaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// parameters for first function

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes1;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes1(attributes1);
	
	attributes1.iFlags  = flags;
	attributes1.iFormat = format;
	attributes1.iType   = type;
	attributes1.iDcs    = dcs;

	TName name1 = _L("Name1");
	TPckg<TName> msgData1(name1);

 	//-------------------------------------------------------------------------
	// parameters for second function

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes2;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes2(attributes2);

	flags  = 10;
	format = RMobileUssdMessaging::EFormatPackedString;
	type   = RMobileUssdMessaging::EUssdMOAcknowledgement;
	dcs    = 20;
	
	attributes2.iFlags  = flags;
	attributes2.iFormat = format;
	attributes2.iType   = type;
	attributes2.iDcs    = dcs;
	
	TName name2 = _L("Name2");
	TPckg<TName> msgData2(name2);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name1, attributes1);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileUssdMessaging::SendMessage
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessage, data);
    iMockLTSY.CompleteL(EMobileUssdMessagingSendMessage, KErrNone, 10);
	ussdMessaging1.SendMessage(requestStatus1, msgData1, msgAttributes1);

	ussdMessaging2.SendMessage(requestStatus2, msgData2, msgAttributes2);
	
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	CleanupStack::PopAndDestroy(6, this); // phone2, telServer2, data, this, 
	                                      // ussdMessaging1, ussdMessaging2

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USM-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendMessage with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessage and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessage0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	attributes.iFlags  = flags;
	attributes.iFormat = format;
	attributes.iType   = type;
	attributes.iDcs   = dcs;
	
	TName name = _L("Name1");
	TPckg<TName> msgData(name);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name, attributes);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileUssdMessaging::SendMessage
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessage, data);

	ussdMessaging.SendMessage(requestStatus, msgData, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendRelease
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendRelease0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
    TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// function parameters
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);
	
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV3;

	TUint8 opCode = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV3.iOpCode         = opCode;
	completeSendV3.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData1(completeSendV3);

	mockData1.SerialiseL(data);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease);
    iMockLTSY.CompleteL(EMobileUssdMessagingSendRelease, KErrGeneral, data);
	ussdMessaging.SendRelease(requestStatus, pckgSendV3);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileUssdMessaging::SendRelease when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendRelease, KErrNone, data);
	ussdMessaging.SendRelease(requestStatus, pckgSendV3);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	ASSERT_EQUALS(opCode, sendV3.iOpCode);	
	ASSERT_EQUALS(0, completeSendV3.iAdditionalInfo.Compare(sendV3.iAdditionalInfo));	

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileUssdMessaging::SendRelease
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendRelease, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging
	
	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USR-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendRelease
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendRelease0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

 	//-------------------------------------------------------------------------
	// function parameters
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);
	
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV3;

	TUint8 opCode = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV3.iOpCode         = opCode;
	completeSendV3.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData1(completeSendV3);

	mockData1.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease, KErrNotSupported);
	ussdMessaging.SendRelease(requestStatus, pckgSendV3);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendRelease, KErrNone, data);
	ussdMessaging.SendRelease(requestStatus, pckgSendV3);
	
    User::WaitForRequest(requestStatus);        

	ASSERT_EQUALS(KErrNone, requestStatus.Int());	

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3); // data, this, ussdMessaging

	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileUssdMessaging::SendRelease
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileUssdMessaging::SendRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendRelease0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

 	//-------------------------------------------------------------------------
	// function parameters
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);
	
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV3;

	TUint8 opCode = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV3.iOpCode         = opCode;
	completeSendV3.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData1(completeSendV3);

	mockData1.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileUssdMessaging::SendRelease
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendRelease, KErrNone, data, 10);

	ussdMessaging.SendRelease(requestStatus, pckgSendV3);
    ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingSendRelease);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // data, this, ussdMessaging
	
	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendRelease with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendRelease with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendRelease0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV3;

	TUint8 opCode = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV3.iOpCode         = opCode;
	completeSendV3.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData1(completeSendV3);

	mockData1.SerialiseL(data);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileUssdMessaging::SendRelease
 	//-------------------------------------------------------------------------

	TBuf8<1> badSizeDescriptor;
	ussdMessaging.SendRelease(requestStatus, badSizeDescriptor);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileUssdMessaging::SendRelease
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileUssdMessaging::SendRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendRelease0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileUssdMessaging ussdMessaging1;
	ret = ussdMessaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging1);

	RMobileUssdMessaging ussdMessaging2;
	ret = ussdMessaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

 	//-------------------------------------------------------------------------
	// parameters for first function
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV31;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV31(sendV31);
	
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV31;

	TUint8 opCode = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV31.iOpCode         = opCode;
	completeSendV31.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData11(completeSendV31);

 	//-------------------------------------------------------------------------
	// parameters for second function
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV32;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV32(sendV32);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileUssdMessaging::SendRelease
 	//-------------------------------------------------------------------------
	
	RBuf8 data;
	CleanupClosePushL(data); // PUSH
	mockData11.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendRelease, KErrNone, data, 10);
	ussdMessaging1.SendRelease(requestStatus1, pckgSendV31);

	ussdMessaging2.SendRelease(requestStatus2, pckgSendV32);
	
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	
	ASSERT_EQUALS(opCode, sendV31.iOpCode);	
	ASSERT_EQUALS(0, sendV31.iAdditionalInfo.Compare(KAdditionalInfo1));	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // data, ussdMessaging2, ussdMessaging1,
										  // phone2, telServer2, this                                   
	}


/**
@SYMTestCaseID BA-CTSY-USSDM-USR-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendRelease with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendRelease and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendRelease0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// function parameters
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileUssdMessaging::SendRelease
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileUssdMessagingSendRelease);
	ussdMessaging.SendRelease(requestStatus, pckgSendV3);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-URM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestReceiveMessage0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	TBuf8<KMaxName> name;

 	//-------------------------------------------------------------------------

	RMobileUssdMessaging::TMobileUssdAttributesV1 completeAttributes;

	completeAttributes.iFlags  = flags;
	completeAttributes.iFormat = format;
	completeAttributes.iType   = type;
	completeAttributes.iDcs    = dcs;
	
	TBuf8<KMaxName> completeName = _L8("Name1");

	TMockLtsyData2<TBuf8<KMaxName>, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(completeName, completeAttributes);

	mockData2.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    iMockLTSY.CompleteL(EMobileUssdMessagingReceiveMessage, KErrGeneral, data);

	ussdMessaging.ReceiveMessage(requestStatus, name, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileUssdMessaging::ReceiveMessage when result is not cached.
 	//-------------------------------------------------------------------------

    iMockLTSY.CompleteL(EMobileUssdMessagingReceiveMessage, KErrNone, data);

	ussdMessaging.ReceiveMessage(requestStatus, name, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	ASSERT_EQUALS(flags , attributes.iFlags );
	ASSERT_EQUALS(format, attributes.iFormat);
	ASSERT_EQUALS(type  , attributes.iType  );
	ASSERT_EQUALS(dcs   , attributes.iDcs   );

	ASSERT_EQUALS(0, name.Compare(completeName));

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileUssdMessaging::ReceiveMessage
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileUssdMessagingReceiveMessage, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging
	
	}


/**
@SYMTestCaseID BA-CTSY-USSDM-URM-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileUssdMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileUssdMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestReceiveMessage0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	TBuf8<KMaxName> name;

 	//-------------------------------------------------------------------------

	RMobileUssdMessaging::TMobileUssdAttributesV1 completeAttributes;

	completeAttributes.iFlags  = flags;
	completeAttributes.iFormat = format;
	completeAttributes.iType   = type;
	completeAttributes.iDcs    = dcs;
	
	TBuf8<KMaxName> completeName = _L8("Name1");

	TMockLtsyData2<TBuf8<KMaxName>, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(completeName, completeAttributes);

	mockData2.SerialiseL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileUssdMessaging::ReceiveMessage
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
    iMockLTSY.CompleteL(EMobileUssdMessagingReceiveMessage, KErrNone, data, 10);
	
	ussdMessaging.ReceiveMessage(requestStatus, name, msgAttributes);
    ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingReceiveMessage);
		
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
 	
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3); // data, this, ussdMessaging
	
	}


/**
@SYMTestCaseID BA-CTSY-USSDM-URM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::ReceiveMessage with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::ReceiveMessage with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestReceiveMessage0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------

	RMobileUssdMessaging::TMobileUssdAttributesV1 completeAttributes;

	completeAttributes.iFlags  = flags;
	completeAttributes.iFormat = format;
	completeAttributes.iType   = type;
	completeAttributes.iDcs    = dcs;
	
	TBuf8<KMaxName> completeName = _L8("Name1");

	TMockLtsyData2<TBuf8<KMaxName>, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(completeName, completeAttributes);

	mockData2.SerialiseL(data);

	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RPacketQoS::SetProfileParameters
 	//-------------------------------------------------------------------------

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);

	TBuf8<1> badName;
	
    iMockLTSY.CompleteL(EMobileUssdMessagingReceiveMessage, KErrNone, data);

	ussdMessaging.ReceiveMessage(requestStatus, badName, msgAttributes);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileUssdMessaging::ReceiveMessage
 	//-------------------------------------------------------------------------

	TBuf8<KMaxName> name;
	TBuf8<1> badSizeDescriptor;

	ussdMessaging.ReceiveMessage(requestStatus, name, badSizeDescriptor);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();	
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging

	}


/**
@SYMTestCaseID BA-CTSY-USSDM-URM-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileUssdMessaging::ReceiveMessage
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileUssdMessaging::ReceiveMessage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestReceiveMessage0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileUssdMessaging ussdMessaging1;
	ret = ussdMessaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging1);

	RMobileUssdMessaging ussdMessaging2;
	ret = ussdMessaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// parameters for first function

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes1;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes1(attributes1);
	
	TBuf8<KMaxName> name1;

 	//-------------------------------------------------------------------------
	// parameters for second function

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes2;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes2(attributes2);
	
	TBuf8<KMaxName> name2;

 	//-------------------------------------------------------------------------

	RMobileUssdMessaging::TMobileUssdAttributesV1 completeAttributes1;

	completeAttributes1.iFlags  = flags;
	completeAttributes1.iFormat = format;
	completeAttributes1.iType   = type;
	completeAttributes1.iDcs    = dcs;
	
	TBuf8<KMaxName> completeName1 = _L8("Name1");

	TMockLtsyData2<TBuf8<KMaxName>, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData21(completeName1, completeAttributes1);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileUssdMessaging::ReceiveMessage
 	//-------------------------------------------------------------------------

	mockData21.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileUssdMessagingReceiveMessage, KErrNone, data, 10);
	ussdMessaging1.ReceiveMessage(requestStatus1, name1, msgAttributes1);

	ussdMessaging2.ReceiveMessage(requestStatus2, name2, msgAttributes2);
	
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        

    // CTSY treats USSD receive requests as multiple completion requests, and completes
    // all client requests when a message comes in.
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	

	ASSERT_EQUALS(completeAttributes1.iFlags , attributes1.iFlags );
	ASSERT_EQUALS(completeAttributes1.iFormat, attributes1.iFormat);
	ASSERT_EQUALS(completeAttributes1.iType  , attributes1.iType  );
	ASSERT_EQUALS(completeAttributes1.iDcs   , attributes1.iDcs   );

	ASSERT_EQUALS(0, name1.Compare(completeName1));

	ASSERT_EQUALS(completeAttributes1.iFlags , attributes2.iFlags );
	ASSERT_EQUALS(completeAttributes1.iFormat, attributes2.iFormat);
	ASSERT_EQUALS(completeAttributes1.iType  , attributes2.iType  );
	ASSERT_EQUALS(completeAttributes1.iDcs   , attributes2.iDcs   );

	ASSERT_EQUALS(0, name2.Compare(completeName1));

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); // phone2, telServer2, data, this,
	                                      // ussdMessaging1, ussdMessaging2

	}

/**
@SYMTestCaseID BA-CTSY-USSDM-UNNR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::NotifyNetworkRelease
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::NotifyNetworkRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestNotifyNetworkRelease0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

 	//-------------------------------------------------------------------------
	// function parameters
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);
	
	// optional parameter
	RMobileUssdMessaging::TMobileUssdAttributesV1 attrV1;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pckgAttrV1(attrV1);
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV3;
	
	TUint8 opCode = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV3.iOpCode         = opCode;
	completeSendV3.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData1(completeSendV3);
	mockData1.SerialiseL(data);

	// optional parameter
	RMobileUssdMessaging::TMobileUssdAttributesV1 completeAttrV1;
	completeAttrV1.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType;
	completeAttrV1.iFormat = RMobileUssdMessaging::EFormatPackedString;
	completeAttrV1.iType = RMobileUssdMessaging::EUssdMOAcknowledgement;
	
	TMockLtsyData2< RMobilePhone::TMobilePhoneSendSSRequestV3, 
					RMobileUssdMessaging::TMobileUssdAttributesV1 > 
					mockData2(completeSendV3,completeAttrV1);
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	mockData2.SerialiseL(data2);
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrGeneral, data);
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3);

    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

	
	// other version of API (with optional TMobileUssdAttributesV1 parameter)
	
	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNotReady, data2);
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3, pckgAttrV1);

    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotReady, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileUssdMessaging::NotifyNetworkRelease
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNone, data);
	
	// other version of API (with optional TMobileUssdAttributesV1 parameter)
	
	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNone, data2);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileUssdMessaging::NotifyNetworkRelease when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNone, data);
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3);

    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	ASSERT_EQUALS(opCode, sendV3.iOpCode);	
	ASSERT_EQUALS(0, sendV3.iAdditionalInfo.Compare(KAdditionalInfo1));	

	
	// other version of API (with optional TMobileUssdAttributesV1 parameter)
	
	ussdMessaging.Close();
	ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	
	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNone, data2);
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3, pckgAttrV1);

    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	ASSERT_EQUALS(opCode, sendV3.iOpCode);	
	ASSERT_EQUALS(0, sendV3.iAdditionalInfo.Compare(KAdditionalInfo1));
	
	ASSERT_EQUALS(completeAttrV1.iFlags ,attrV1.iFlags);
	ASSERT_EQUALS(completeAttrV1.iFormat ,attrV1.iFormat);
	ASSERT_EQUALS(completeAttrV1.iType ,attrV1.iType);

	CleanupStack::PopAndDestroy(4, this); // data2, ussdMessaging, data, this, 
	
	}


/**
@SYMTestCaseID BA-CTSY-USSDM-UNNR-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileUssdMessaging::NotifyNetworkRelease
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileUssdMessaging::NotifyNetworkRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestNotifyNetworkRelease0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileUssdMessaging::NotifyNetworkRelease
 	//-------------------------------------------------------------------------
 	
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingNotifyNetworkRelease);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	// again with the optional parameter
	RMobileUssdMessaging::TMobileUssdAttributesV1 attrV1;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pckgAttrV1(attrV1);
	
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3, pckgAttrV1);
	ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingNotifyNetworkRelease);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2); // this, ussdMessaging
	
	}

/**
@SYMTestCaseID BA-CTSY-USSDM-UNNR-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::NotifyNetworkRelease with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::NotifyNetworkRelease with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestNotifyNetworkRelease0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileUssdMessaging::NotifyNetworkRelease
 	//-------------------------------------------------------------------------

	TBuf8<1> badSizeDescriptor;
	ussdMessaging.NotifyNetworkRelease(requestStatus, badSizeDescriptor);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	// again with the optional parameter
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV3;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV3(sendV3);
	
	ussdMessaging.NotifyNetworkRelease(requestStatus, pckgSendV3, badSizeDescriptor);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());
	
	CleanupStack::PopAndDestroy(3); // this, ussdMessaging, data
	}

/**
@SYMTestCaseID BA-CTSY-USSDM-UNNR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileUssdMessaging::NotifyNetworkRelease
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileUssdMessaging::NotifyNetworkRelease
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestNotifyNetworkRelease0004L()
	{

					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileUssdMessaging ussdMessaging1;
	ret = ussdMessaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging1);

	RMobileUssdMessaging ussdMessaging2;
	ret = ussdMessaging2.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging2);

 	//-------------------------------------------------------------------------
	// parameters for first function
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV31;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV31(sendV31);

 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneSendSSRequestV3 completeSendV31;

	TUint8 opCode1 = 100;
	_LIT(KAdditionalInfo1, "additionalInfo1");
	
	completeSendV31.iOpCode         = opCode1;
	completeSendV31.iAdditionalInfo = KAdditionalInfo1;

	TMockLtsyData1< RMobilePhone::TMobilePhoneSendSSRequestV3 > mockData11(completeSendV31);

	RMobileUssdMessaging::TMobileUssdAttributesV1 completeAttrV1;
	completeAttrV1.iFlags = RMobileUssdMessaging::KUssdDataFormat | RMobileUssdMessaging::KUssdMessageType;
	completeAttrV1.iFormat = RMobileUssdMessaging::EFormatPackedString;
	completeAttrV1.iType = RMobileUssdMessaging::EUssdMTRequest;
	
	TMockLtsyData2< RMobilePhone::TMobilePhoneSendSSRequestV3, 
					RMobileUssdMessaging::TMobileUssdAttributesV1 > 
					mockData2(completeSendV31,completeAttrV1);
	
	RBuf8 data2;
	CleanupClosePushL(data2);
	mockData2.SerialiseL(data2);
		
	//-------------------------------------------------------------------------
	// parameters for second function
	
	RMobilePhone::TMobilePhoneSendSSRequestV3 sendV32;
	TPckg<RMobilePhone::TMobilePhoneSendSSRequestV3> pckgSendV32(sendV32);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileUssdMessaging::NotifyNetworkRelease
 	//-------------------------------------------------------------------------

	mockData11.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNone, data, 10);
	ussdMessaging1.NotifyNetworkRelease(requestStatus1, pckgSendV31);

	ussdMessaging2.NotifyNetworkRelease(requestStatus2, pckgSendV32);

    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	

	ASSERT_EQUALS(opCode1, sendV31.iOpCode);	
	ASSERT_EQUALS(0, sendV31.iAdditionalInfo.Compare(KAdditionalInfo1));	

	ASSERT_EQUALS(opCode1, sendV32.iOpCode);	
	ASSERT_EQUALS(0, sendV32.iAdditionalInfo.Compare(KAdditionalInfo1));	


	
	AssertMockLtsyStatusL();
	
	//
	// repeat tests with optional second parameter
	// 
	ussdMessaging1.Close();
	ret = ussdMessaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	ussdMessaging2.Close();
	ret = ussdMessaging2.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	
	RMobileUssdMessaging::TMobileUssdAttributesV1 attrV1;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pckgAttrV1(attrV1);

	RMobileUssdMessaging::TMobileUssdAttributesV1 attrV12;
	RMobileUssdMessaging::TMobileUssdAttributesV1Pckg pckgAttrV12(attrV12);

	iMockLTSY.CompleteL(EMobileUssdMessagingNotifyNetworkRelease, KErrNone, data2, 5);
	ussdMessaging1.NotifyNetworkRelease(requestStatus1, pckgSendV31, pckgAttrV1);
	ussdMessaging2.NotifyNetworkRelease(requestStatus2, pckgSendV32, pckgAttrV12);	
	
    User::WaitForRequest(requestStatus1);        
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

    User::WaitForRequest(requestStatus2);        
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());	

	ASSERT_EQUALS(opCode1, sendV31.iOpCode);	
	ASSERT_EQUALS(0, sendV31.iAdditionalInfo.Compare(KAdditionalInfo1));	

	ASSERT_EQUALS(opCode1, sendV32.iOpCode);	
	ASSERT_EQUALS(0, sendV32.iAdditionalInfo.Compare(KAdditionalInfo1));		
	
	ASSERT_EQUALS(completeAttrV1.iFlags ,attrV1.iFlags);
	ASSERT_EQUALS(completeAttrV1.iFormat ,attrV1.iFormat);
	ASSERT_EQUALS(completeAttrV1.iType ,attrV1.iType);	
	
	ASSERT_EQUALS(completeAttrV1.iFlags ,attrV12.iFlags);
	ASSERT_EQUALS(completeAttrV1.iFormat ,attrV12.iFormat);
	ASSERT_EQUALS(completeAttrV1.iType ,attrV12.iType);		
	
	CleanupStack::PopAndDestroy(7, this); // phone2, telServer2, data, this
	                                      // ussdMessaging1, ussdMessaging2, data2 (inv. order)

	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USMNFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessageNoFdnCheck0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	attributes.iFlags  = flags;
	attributes.iFormat = format;
	attributes.iType   = type;
	attributes.iDcs   = dcs;
	
	TName name = _L("Name1");
	TPckg<TName> msgData(name);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name, attributes);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileUssdMessagingSendMessageNoFdnCheck, data, KErrNotSupported);

	ussdMessaging.SendMessageNoFdnCheck(requestStatus, msgData, msgAttributes);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());	

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileUssdMessagingSendMessageNoFdnCheck, data);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendMessageNoFdnCheck, KErrGeneral);

	ussdMessaging.SendMessageNoFdnCheck(requestStatus, msgData, msgAttributes);

	/*
	 * Wrong IPC is passed to ResetTsyReqHandle() EMultimodeUssdSendMessage 
	 * instead of EMultimodeUssdSendMessageNoFdnCheck by call of 
	 * CMmUssdTsy::CompleteSendMessageNoFdnCheck(). This reduces to panic 
	 * with EEtelFaultNotRecognisedTsyHandle.
	 */

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());	

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileUssdMessaging::SendMessageNoFdnCheck when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileUssdMessagingSendMessageNoFdnCheck, data);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendMessageNoFdnCheck, KErrNone);

	ussdMessaging.SendMessageNoFdnCheck(requestStatus, msgData, msgAttributes);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileUssdMessaging::SendMessageNoFdnCheck
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendMessageNoFdnCheck, KErrNone);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(3, this); // data, ussdMessaging, this
	
	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USMNFC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileUssdMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileUssdMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessageNoFdnCheck0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	TRequestStatus requestStatus;

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	attributes.iFlags  = flags;
	attributes.iFormat = format;
	attributes.iType   = type;
	attributes.iDcs   = dcs;
	
	TName name = _L("Name1");
	TPckg<TName> msgData(name);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name, attributes);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileUssdMessaging::SendMessageNoFdnCheck
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
	mockData2.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileUssdMessagingSendMessageNoFdnCheck, data);
    iMockLTSY.CompleteL(EMobileUssdMessagingSendMessageNoFdnCheck, KErrNone, 10);
	
	ussdMessaging.SendMessageNoFdnCheck(requestStatus, msgData, msgAttributes);
    ussdMessaging.CancelAsyncRequest(EMobileUssdMessagingSendMessageNoFdnCheck);
		
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	CleanupStack::PopAndDestroy(3); // data, this, ussdMessaging
	
	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USMNFC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendMessageNoFdnCheck with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessageNoFdnCheck with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessageNoFdnCheck0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

 	//-------------------------------------------------------------------------
	// function parameter

	TName name = _L("Name1");
	TPckg<TName> msgData(name);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileUssdMessaging::SendMessage
 	//-------------------------------------------------------------------------

	TBuf8<1> badSizeDescriptor;

	ussdMessaging.SendMessageNoFdnCheck(requestStatus, msgData, badSizeDescriptor);
	
    User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrArgument, requestStatus.Int());	
	
	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging, ussdMessaging

	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USMNFC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileUssdMessaging::SendMessageNoFdnCheck
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileUssdMessaging::SendMessageNoFdnCheck
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessageNoFdnCheck0004L()
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
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RMobileUssdMessaging ussdMessaging1;
	ret = ussdMessaging1.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging1);

	RMobileUssdMessaging ussdMessaging2;
	ret = ussdMessaging2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// parameters for first function

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes1;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes1(attributes1);
	
	attributes1.iFlags  = flags;
	attributes1.iFormat = format;
	attributes1.iType   = type;
	attributes1.iDcs    = dcs;

	TName name1 = _L("Name1");
	TPckg<TName> msgData1(name1);

 	//-------------------------------------------------------------------------
	// parameters for second function

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes2;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes2(attributes2);

	flags  = 10;
	format = RMobileUssdMessaging::EFormatPackedString;
	type   = RMobileUssdMessaging::EUssdMOAcknowledgement;
	dcs    = 20;
	
	attributes2.iFlags  = flags;
	attributes2.iFormat = format;
	attributes2.iType   = type;
	attributes2.iDcs    = dcs;
	
	TName name2 = _L("Name2");
	TPckg<TName> msgData2(name2);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name1, attributes1);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileUssdMessaging::SendMessageNoFdnCheck
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileUssdMessagingSendMessageNoFdnCheck, data);
	iMockLTSY.CompleteL(EMobileUssdMessagingSendMessageNoFdnCheck, KErrNone, 10);
	ussdMessaging1.SendMessageNoFdnCheck(requestStatus1, msgData1, msgAttributes1);

	ussdMessaging2.SendMessageNoFdnCheck(requestStatus2, msgData2, msgAttributes2);

	User::WaitForRequest(requestStatus1);
	ASSERT_EQUALS(KErrNone, requestStatus1.Int());	

	User::WaitForRequest(requestStatus2);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());	

	CleanupStack::PopAndDestroy(6, this); // phone2, telServer2, data, this, 
	                                      // ussdMessaging1, ussdMessaging2

	}

/**
@SYMTestCaseID BA-CTSY-USSDM-USMNFC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::SendMessageNoFdnCheck with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::SendMessageNoFdnCheck and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestSendMessageNoFdnCheck0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TRequestStatus requestStatus;

	TUint32 flags = 100;
	RMobileUssdMessaging::TMobileUssdDataFormat format = RMobileUssdMessaging::EFormatUnspecified;
	RMobileUssdMessaging::TMobileUssdMessageType type  = RMobileUssdMessaging::EUssdMORequest;
	TUint8 dcs = 200;

 	//-------------------------------------------------------------------------
	// function parameters

	RMobileUssdMessaging::TMobileUssdAttributesV1 attributes;
	TPckg<RMobileUssdMessaging::TMobileUssdAttributesV1> msgAttributes(attributes);
	
	attributes.iFlags  = flags;
	attributes.iFormat = format;
	attributes.iType   = type;
	attributes.iDcs   = dcs;
	
	TName name = _L("Name1");
	TPckg<TName> msgData(name);

 	//-------------------------------------------------------------------------

	TMockLtsyData2<TName, RMobileUssdMessaging::TMobileUssdAttributesV1> 
		mockData2(name, attributes);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileUssdMessaging::SendMessageNoFdnCheck
 	//-------------------------------------------------------------------------

	mockData2.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileUssdMessagingSendMessageNoFdnCheck, data);

	ussdMessaging.SendMessageNoFdnCheck(requestStatus, msgData, msgAttributes);

	User::WaitForRequest(requestStatus);        
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());	

	CleanupStack::PopAndDestroy(3, this); // data, this, ussdMessaging

	}

/**
@SYMTestCaseID BA-CTSY-USSDM-UGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::GetCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::GetCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestGetCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);
	
	TUint32 format = RMobileUssdMessaging::KCapsPackedString;
	TUint32 types  = KUssdTypeCaps;
		
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileUssdMessaging::GetCaps when result is not cached.
 	//-------------------------------------------------------------------------

	RMobileUssdMessaging::TMobileUssdCapsV1 capsV1;
	TPckg<RMobileUssdMessaging::TMobileUssdCapsV1> pckg(capsV1);
	ret = ussdMessaging.GetCaps(pckg);
	ASSERT_EQUALS(KErrNone, ret);
	
	ASSERT_EQUALS(format, capsV1.iUssdFormat); // hard coded value
	ASSERT_EQUALS(types , capsV1.iUssdTypes ); // hard coded value

	CleanupStack::PopAndDestroy(2, this); // this, ussdMessaging
	
	}

/**
@SYMTestCaseID BA-CTSY-USSDM-UGC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileUssdMessaging::GetCaps with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobileUssdMessaging::GetCaps with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUssdMessagingFU::TestGetCaps0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileUssdMessaging ussdMessaging;
	TInt ret = ussdMessaging.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(ussdMessaging);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileUssdMessaging::GetCaps
 	//-------------------------------------------------------------------------

	TBuf8<1> badSizeDescriptor;
	
	ret = ussdMessaging.GetCaps(badSizeDescriptor);
	ASSERT_EQUALS(KErrArgument, ret);

	CleanupStack::PopAndDestroy(2, this); // this, ussdMessaging

	}

