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
// The TEFUnit test suite for UserToUserSignalling in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyusertousersignallingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyUserToUserSignallingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestGetUUSCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestGetUUSCaps0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestGetUUSCaps0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestReceiveUUI0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestReceiveUUI0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestReceiveUUI0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestReceiveUUI0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestActivateUUS0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestActivateUUS0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestActivateUUS0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestHangupWithUUI0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestHangupWithUUI0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestHangupWithUUI0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestNotifyUUSCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestNotifyUUSCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestNotifyUUSCapsChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestAnswerIncomingCallWithUUI0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestAnswerIncomingCallWithUUI0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestAnswerIncomingCallWithUUI0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestSendUUI0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestSendUUI0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestSendUUI0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestGetUUSSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestSetUUSSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyUserToUserSignallingFU, TestNotifyUUSSettingChange0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-UUS-CGUC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetUUSCaps for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetUUSCaps for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestGetUUSCaps0001L()
	{

	AuxGetUUSCapsL(KMmTsyVoice1LineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CGUC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetUUSCaps for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetUUSCaps for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestGetUUSCaps0002L()
	{

	AuxGetUUSCapsL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CGUC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetUUSCaps for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetUUSCaps for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestGetUUSCaps0003L()
	{

	AuxGetUUSCapsL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CRU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ReceiveUUI for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ReceiveUUI for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestReceiveUUI0001L()
	{

	AuxReceiveUUIVoiceL();

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CRU-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test to increase CMmCallTsy::ReceiveUUI coverage covers 
@SYMTestCaseDesc "if ( 0 < iReceivedUUI.Length() )" condition
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ReceiveUUI for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestReceiveUUI0001bL()
	{

	AuxReceiveUUIVoiceL(ETrue);
	
	}


/**
@SYMTestCaseID BA-CTSY-UUS-CRU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ReceiveUUI for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ReceiveUUI for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestReceiveUUI0002L()
	{

	AuxReceiveUUIDataAndFaxL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CRU-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ReceiveUUI for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ReceiveUUI for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestReceiveUUI0003L()
	{

	AuxReceiveUUIDataAndFaxL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CAU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ActivateUUS for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ActivateUUS for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestActivateUUS0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, KMmTsyVoice1LineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	RMobileCall::TMobileCallUUSRequestV1 uusRequest;
	RMobileCall::TMobileCallUUSRequestV1Pckg pckgUUSRequest(uusRequest);

	TInt serviceReq(1);
	uusRequest.iServiceReq = serviceReq;

	call.ActivateUUS(reqStatus, pckgUUSRequest);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNone, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, call, this

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CAU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ActivateUUS for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ActivateUUS for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestActivateUUS0002L()
	{

	AuxActivateUUSDataAndFaxL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CAU-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::ActivateUUS for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::ActivateUUS for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestActivateUUS0003L()
	{

	AuxActivateUUSDataAndFaxL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CHWU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::HangupWithUUI for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::HangupWithUUI for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestHangupWithUUI0001L()
	{

	AuxHangupWithUUIL(KMmTsyVoice1LineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CHWU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::HangupWithUUI for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::HangupWithUUI for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestHangupWithUUI0002L()
	{

	AuxHangupWithUUIL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CHWU-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::HangupWithUUI for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::HangupWithUUI for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestHangupWithUUI0003L()
	{

	AuxHangupWithUUIL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CNUCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyUUSCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyUUSCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestNotifyUUSCapsChange0001L()
	{

	AuxNotifyUUSCapsChangeL(KMmTsyVoice1LineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CNUCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyUUSCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyUUSCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestNotifyUUSCapsChange0002L()
	{

	AuxNotifyUUSCapsChangeL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CNUCC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyUUSCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyUUSCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestNotifyUUSCapsChange0003L()
	{

	AuxNotifyUUSCapsChangeL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CAICWU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallWithUUI for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallWithUUI for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestAnswerIncomingCallWithUUI0001L()
	{

	AuxAnswerIncomingCallWithUUIL(KMmTsyVoice1LineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CAICWU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallWithUUI for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallWithUUI for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestAnswerIncomingCallWithUUI0002L()
	{

	AuxAnswerIncomingCallWithUUIL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CAICWU-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::AnswerIncomingCallWithUUI for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::AnswerIncomingCallWithUUI for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestAnswerIncomingCallWithUUI0003L()
	{

	AuxAnswerIncomingCallWithUUIL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CSU-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SendUUI for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SendUUI for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestSendUUI0001L()
	{

	AuxSendUUIL(KMmTsyVoice1LineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CSU-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SendUUI for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SendUUI for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestSendUUI0002L()
	{

	AuxSendUUIL(KMmTsyDataLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-CSU-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SendUUI for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SendUUI for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestSendUUI0003L()
	{

	AuxSendUUIL(KMmTsyFaxLineName);

	}


/**
@SYMTestCaseID BA-CTSY-UUS-MGUUSS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetUUSSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetUUSSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestGetUUSSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	// Test API is not supported
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneUUSSetting setting;
	
	iPhone.GetUUSSetting(reqStatus, setting);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-UUS-MSUS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetUUSSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetUUSSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestSetUUSSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	// Test API is not supported
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneUUSSetting setting(RMobilePhone::EIncomingUUSAccepted);
	
	iPhone.SetUUSSetting(reqStatus, setting);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-UUS-MNUSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyUUSSettingChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyUUSSettingChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyUserToUserSignallingFU::TestNotifyUUSSettingChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	// Test API is not supported
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneUUSSetting setting;
	
	iPhone.NotifyUUSSettingChange(reqStatus, setting);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	
	}

void CCTsyUserToUserSignallingFU::AuxGetUUSCapsL(const TDesC& aLineName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TUint32 caps;
	TInt ret = call.GetUUSCaps(caps);	
	ASSERT_EQUALS (KErrNotSupported, ret);	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, call, this
	
	}

/*
 * Auxiliary function may be used only for KMmTsyDataLineName and KMmTsyFaxLineName.
 */
void CCTsyUserToUserSignallingFU::AuxActivateUUSDataAndFaxL(const TDesC& aLineName)
	{

	if (aLineName.Compare(KMmTsyVoice1LineName) == 0)
		{
		ASSERT_TRUE(false);
		}

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	RMobileCall::TMobileCallUUSRequestV1 uusRequest;
	RMobileCall::TMobileCallUUSRequestV1Pckg pckgUUSRequest(uusRequest);
	
	call.ActivateUUS(reqStatus, pckgUUSRequest);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, fax, this
	
	}

void CCTsyUserToUserSignallingFU::AuxHangupWithUUIL(const TDesC& aLineName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	RMobileCall::TMobileCallUUI uui;
		
	call.HangupWithUUI(reqStatus, uui);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, call, this
	
	}

void CCTsyUserToUserSignallingFU::AuxNotifyUUSCapsChangeL(const TDesC& aLineName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	TUint32 caps;
		
	call.NotifyUUSCapsChange(reqStatus, caps);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, call, this
	
	}

void CCTsyUserToUserSignallingFU::AuxAnswerIncomingCallWithUUIL(const TDesC& aLineName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	_LIT(KSomething, "Some");
	RMobileCall::TMobileCallUUI uui(KSomething);
	RMobileCall::TMobileCallParamsV1 callParam;
	RMobileCall::TMobileCallParamsV1Pckg pckgCallParam(callParam);
		
	call.AnswerIncomingCallWithUUI(reqStatus, pckgCallParam, uui);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // ... , this
	
	}

void CCTsyUserToUserSignallingFU::AuxSendUUIL(const TDesC& aLineName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	_LIT(KSomething, "Some");
	RMobileCall::TMobileCallUUI uui(KSomething);
	TBool more (EFalse);
		
	call.SendUUI(reqStatus, more, uui);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // ... , this
	
	}

/*
 * Auxiliary function may be used only for KMmTsyDataLineName and KMmTsyFaxLineName.
 */
void CCTsyUserToUserSignallingFU::AuxReceiveUUIDataAndFaxL(const TDesC& aLineName)
	{

	if (aLineName.Compare(KMmTsyVoice1LineName) == 0)
		{
		ASSERT_TRUE(false);
		}

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	OpenNewLineLC(line, aLineName);

	TName callName;
	RMobileCall call;
	OpenNewCallLC(call, line, callName);

	TRequestStatus reqStatus;
	RMobileCall::TMobileCallUUI uui;
	
	call.ReceiveUUI(reqStatus, uui);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNotSupported, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, call, this
	
	}

/*
 * Auxiliary function may be used only for KMmTsyVoice1LineName.
 * 
 * @capability aWaitForComplete - Indicator that determines when call of CompleteL()
 * must be complete. If client sets it to ETrue then call of CompleteL()
 * is completed befor call of ReceiveUUI(). Default variable is equal EFalse.
 */

void CCTsyUserToUserSignallingFU::AuxReceiveUUIVoiceL(TBool aWaitForComplete)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	OpenNewLineLC(line, KMmTsyVoice1LineName);

	TName callName;
	RMobileCall call;
	TInt callId(1);
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	CreateAndOpenCallForVoiceLC( line, call, callId, callName, mobileService);

	TRequestStatus reqStatus;
	TRequestStatus mockStatus;
	RMobileCall::TMobileCallUUI uui;

	_LIT(KUUI, "old witch");
	RMobileCall::TMobileCallUUI receiveUUI;
	receiveUUI.Append(KUUI);
	TMockLtsyCallData1<RMobileCall::TMobileCallUUI> mockData(callId, mobileService, receiveUUI);

	if (aWaitForComplete)
		{
		iMockLTSY.NotifyTerminated(mockStatus);
		}
	
	mockData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallReceiveUUI, KErrNone, data);
	
	if (aWaitForComplete)
		{
		/*
		 * Wait for CompleteL() to ensure that iReceivedUUI is updated 
		 * before call.ReceiveUUI() is called.
		 */
		User::WaitForRequest(mockStatus);
		ASSERT_EQUALS (KErrNone, mockStatus.Int());	
		}

	call.ReceiveUUI(reqStatus, uui);
	User::WaitForRequest(reqStatus);	
	ASSERT_EQUALS (KErrNone, reqStatus.Int());	

	ASSERT_EQUALS(0, receiveUUI.Compare(uui));

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // line, this, data, call
	
	}

/*
 * Opens a line by name.
 */
void CCTsyUserToUserSignallingFU::OpenNewLineLC(RLine& aLine, const TDesC& aLineName)
	{
	TInt errorCode = aLine.Open(iPhone, aLineName);
	ASSERT_EQUALS (KErrNone, errorCode);
	CleanupClosePushL(aLine);
	}

/*
 * Opens a new call for line.
 */
void CCTsyUserToUserSignallingFU::OpenNewCallLC(RMobileCall& aCall, RLine& aLine, TDes& aCallName)
	{
	TInt errorCode = aCall.OpenNewCall(aLine, aCallName);
	ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(aCall);
	}

/**
A function creates and opens new incomming call for voice with aCallId and gets aCallName.
*/
void CCTsyUserToUserSignallingFU::CreateAndOpenCallForVoiceLC( RLine& aLine, RCall& aCall, 
													TInt aCallId, TName& aCallName,
													RMobilePhone::TMobileService& aMobileService,
													RMobileCall::TMobileCallStatus aMobileCallStatus)
	{
	
	TName lineName; 

	switch(aMobileService)
		{
		case RMobilePhone::EVoiceService:
			{
			lineName = KMmTsyVoice1LineName;
			break;
			}
		case RMobilePhone::ECircuitDataService:
			{
			lineName = KMmTsyDataLineName;
			break;
			}
		case RMobilePhone::EFaxService:
			{
			lineName = KMmTsyFaxLineName;
			break;
			}
		default:
			{
			ASSERT_TRUE(false);
			}
		}
	
	// create new incoming call
	TInt errorCode = CreateIncomingCallL(aLine, aCallId, lineName, aCallName,
										aMobileService, aMobileCallStatus);
	ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(aCall);
	
	errorCode = aCall.OpenExistingCall(aLine, aCallName);
	ASSERT_EQUALS(KErrNone, errorCode);

	}

