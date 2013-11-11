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
// The TEFUnit test suite for Fax in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyfaxfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyFaxFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestWaitForEndOfPage0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestTerminateFaxSession0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestGetFaxSettings0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestSetFaxSettings0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestAdoptFaxSharedHeaderFile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestRead0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFaxFU, TestWrite0001L);

	END_SUITE;
	}
	
CCTsyFaxFU::~CCTsyFaxFU()
	{
	RFs fs;
	if(fs.Connect() == KErrNone)
		{
		_LIT(KFileName, "C:\\file.txt");
		fs.Delete(KFileName);
		fs.Close();
		}
	}
	
void CCTsyFaxFU::OpenFaxL()
	{
	RFax  fax;
	RLine line;
	RCall call;
	
	OpenCallFaxLC(line, call);
	CleanupClosePushL(call);
	
	TInt res = fax.Open(call);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(fax);
	
	CleanupStack::PopAndDestroy(1); // fax
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	CloseIncomingCallL(call, 1, mobileService);

	CleanupStack::PopAndDestroy(2); // call, ;ine
	}

void CCTsyFaxFU::OpenCallVoiceLC(RLine& aLine, RCall& aCall)
	{
	TInt errorCode = KErrNone;
    TBuf<128> name;
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
	
	OpenLineVoiceLC(aLine);
	CreateIncomingCallL(aLine, 1, KMmTsyDataLineName, name, mobileService, callStatus);
   	errorCode = aCall.OpenExistingCall(aLine, name);
   	ASSERT_EQUALS(KErrNone, errorCode);
	}

void CCTsyFaxFU::OpenLineVoiceLC(RLine& aLine)
	{
	TInt errorCode = aLine.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, errorCode)
	CleanupClosePushL(aLine);
	}

void CCTsyFaxFU::OpenCallFaxLC(RLine& aLine, RCall& aCall)
	{
	TInt errorCode = KErrNone;
    TBuf<128> name;
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
	
	OpenLineFaxLC(aLine);
	CreateIncomingCallL(aLine, 1, KMmTsyDataLineName, name, mobileService, callStatus);
   	errorCode = aCall.OpenExistingCall(aLine, name);
   	ASSERT_EQUALS(KErrNone, errorCode);
	}

void CCTsyFaxFU::OpenLineFaxLC(RLine& aLine)
	{
	TInt errorCode = aLine.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, errorCode)
	CleanupClosePushL(aLine);
	}

void CCTsyFaxFU::OpenCallDataLC(RLine& aLine, RCall& aCall)
	{
	TInt errorCode = KErrNone;
    TBuf<128> name;
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	RMobileCall::TMobileCallStatus callStatus = RMobileCall::EStatusConnected;
	
	OpenLineDataLC(aLine);
	CreateIncomingCallL(aLine, 1, KMmTsyDataLineName, name, mobileService, callStatus);
   	errorCode = aCall.OpenExistingCall(aLine, name);
   	ASSERT_EQUALS(KErrNone, errorCode);
	}

void CCTsyFaxFU::OpenLineDataLC(RLine& aLine)
	{
	TInt errorCode = aLine.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode)
	CleanupClosePushL(aLine);
	}

//
// Actual test cases
//

/**
@SYMTestCaseID BA-CTSY-FAX-FWFEOP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RFax::WaitForEndOfPage
@SYMTestPriority High
@SYMTestActions Invokes RFax::WaitForEndOfPage
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestWaitForEndOfPage0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();
	
	RFax  fax;
	RLine line;
	RCall call;
	
	OpenCallFaxLC(line, call);
	CleanupClosePushL(call);
	
	TInt res = fax.Open(call);
	CleanupClosePushL(fax);
	
	// Test can not be completed until RFax::Open is implemented
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 250101);
	ASSERT_EQUALS(KErrNotSupported, res)

	RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // fax, call, line, this
	}

/**
@SYMTestCaseID BA-CTSY-FAX-FTFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RFax::TerminateFaxSession
@SYMTestPriority High
@SYMTestActions Invokes RFax::TerminateFaxSession
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestTerminateFaxSession0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RFax  fax;
	RLine line;
	RCall call;
	
	OpenCallFaxLC(line, call);
	CleanupClosePushL(call);
	
	TInt res = fax.Open(call);
	CleanupClosePushL(fax);
	
	// Test can not be completed until RFax::Open is implemented
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 250101);
	ASSERT_EQUALS(KErrNotSupported, res)

	RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // fax, call, line, this
	}

/**
@SYMTestCaseID BA-CTSY-FAX-CGFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetFaxSettings for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetFaxSettings for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestGetFaxSettings0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	
	OpenCallVoiceLC(line, call);
	CleanupClosePushL(call);
	
	RCall::TFaxSessionSettings faxSettings;
	
	TInt res = call.GetFaxSettings(faxSettings);
	ASSERT_EQUALS(KErrNotSupported, res) // GetFaxSettings is not supported.

	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // this, call, line
	}

/**
@SYMTestCaseID BA-CTSY-FAX-CSFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::SetFaxSettings for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::SetFaxSettings for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestSetFaxSettings0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line; 
	RCall call;
	
	OpenCallVoiceLC(line, call);
	CleanupClosePushL(call);
	
	_LIT(KFaxId, "FaxId");
	RCall::TFaxSessionSettings faxSettings;
	faxSettings.iFaxId.Copy(KFaxId);
	
	TInt res = call.SetFaxSettings(faxSettings);
	ASSERT_EQUALS(KErrNotSupported, res) // SetFaxSettings is not supported.

	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // this, line, call
	}

/**
@SYMTestCaseID BA-CTSY-FAX-CAFSHF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AdoptFaxSharedHeaderFile for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AdoptFaxSharedHeaderFile for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestAdoptFaxSharedHeaderFile0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line; 
	RCall call;
	
	OpenCallVoiceLC(line, call);
	CleanupClosePushL(call);
	
	RFs fs;
	TInt res = fs.Connect();
	CleanupClosePushL(fs);
	RFile file;
	_LIT(KFileName, "C:\\file.txt");
	if(res == KErrNone)
		{
		TInt shareRes = fs.ShareProtected();
		if(shareRes == KErrNone)
			{
			res = file.Create(fs, KFileName, EFileRead | EFileWrite | EFileShareAny); // This file will be deleted in the destructor of the test suite.
			if(res == KErrNone)
				{
				res = call.AdoptFaxSharedHeaderFile(file);
				/* 
				AdoptFaxSharedHeaderFile returns KErrNotSupported because
				function CCallBase::SetFaxSharedHeaderFile always returns KErrNotSupported.
				(see "TInt CCallBase::Service(const RMessage2& aMessage,CReqEntry* aReqEntry)", 
				case EEtelAdoptFaxSharedHeaderFile, in file ET_CORE.CPP)
				*/
				ASSERT_EQUALS(KErrNotSupported, res)
				}
			}
		}
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // fs, this, call, line
	}

/**
@SYMTestCaseID BA-CTSY-FAX-FR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RFax::Read
@SYMTestPriority High
@SYMTestActions Invokes RFax::Read
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestRead0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RFax  fax;
	RLine line;
	RCall call;
	
	OpenCallFaxLC(line, call);
	CleanupClosePushL(call);
	
	TInt res = fax.Open(call);
	CleanupClosePushL(fax);
	
	// Test can not be completed until RFax::Open is implemented
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 250101);
	ASSERT_EQUALS(KErrNotSupported, res)

	RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // fax, call, line, this
	}

/**
@SYMTestCaseID BA-CTSY-FAX-FW-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RFax::Write
@SYMTestPriority High
@SYMTestActions Invokes RFax::Write
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFaxFU::TestWrite0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RFax  fax;
	RLine line;
	RCall call;
	
	OpenCallFaxLC(line, call);
	CleanupClosePushL(call);
	
	TInt res = fax.Open(call);
	CleanupClosePushL(fax);
	
	// Test can not be completed until RFax::Open is implemented
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 250101);
	ASSERT_EQUALS(KErrNotSupported, res)

	RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	CloseIncomingCallL(call, 1, mobileService);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // fax, call, line, this
	}

