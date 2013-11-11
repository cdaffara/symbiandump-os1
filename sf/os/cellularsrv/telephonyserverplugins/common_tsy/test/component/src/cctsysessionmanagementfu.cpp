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
// The TEFUnit test suite for SessionManagement in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsysessionmanagementfu.h"
#include <etel.h>
#include <etelmm.h>
#include <ctsy/mmtsy_names.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>


_LIT(KDoubleColon, "::");


CTestSuite* CCTsySessionManagementFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestInitialise0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestInitialise0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpen0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpen0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestGetTsyName0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestGetTsyName0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestEnumeratePhones0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestUnloadPhoneModule0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestUnloadPhoneModule0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestGetTsyVersionNumber0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestGetTsyVersionNumber0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestSetExtendedErrorGranularity0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestReferenceCount0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestReferenceCount0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestReferenceCount00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpenExistingCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpenExistingCall0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpenExistingCall0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpenExistingCall0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpenExistingCall00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestOpenExistingCall00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsySessionManagementFU, TestInitialiseMM0001L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SESMG-PI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::Initialise
@SYMTestPriority High
@SYMTestActions Invokes RPhone::Initialise
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestInitialise0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TInt ret = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);

	// test synchronous version of RPhone::Initialise
	ret = iPhone.Initialise();
	ASSERT_EQUALS(KErrNone, ret);

	// test asynchronous version of RPhone::Initialise	
	TRequestStatus requestStatus;
	
	iPhone.Initialise(requestStatus);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	CleanupStack::PopAndDestroy(1, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-PI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RPhone::Initialise
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RPhone::Initialise
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestInitialise0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TInt ret = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);


	// Open second client
	RTelServer telServer2;
	ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	TRequestStatus requestStatus;
	TRequestStatus requestStatus2;

	iPhone.Initialise(requestStatus);
	phone2.Initialise(requestStatus2);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	CleanupStack::PopAndDestroy(3, this);

	}



/**
@SYMTestCaseID BA-CTSY-SESMG-PO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::Open
@SYMTestPriority High
@SYMTestActions Invokes RPhone::Open
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpen0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TInt err = iPhone.Open(iTelServer, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	
	CleanupStack::PopAndDestroy(1, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-SESMG-PO-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::Open with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPhone::Open with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpen0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	//"::" needed to cover some branches in name analyze
	_LIT(KBadPhoneName, "::Bad::Phone::Name::");

	TInt err = iPhone.Open(iTelServer, KBadPhoneName);
	ASSERT_EQUALS(KErrNotFound, err);
	
	CleanupStack::PopAndDestroy(1, this); 

	}


/**
@SYMTestCaseID BA-CTSY-SESMG-TGTN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::GetTsyName
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::GetTsyName
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestGetTsyName0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TInt index(0);
	TName tsyName;
	
	_LIT(KTsyName, "PHONETSY");
	
	TInt ret = iTelServer.GetTsyName(index, tsyName);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(0, tsyName.Compare(KTsyName));
	
	CleanupStack::PopAndDestroy(1, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-TGTN-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::GetTsyName with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::GetTsyName with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestGetTsyName0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	// test on negative index
	TInt index(-1);
	TName tsyName;
		
	TInt ret = iTelServer.GetTsyName(index, tsyName);
	ASSERT_EQUALS(KErrNotFound, ret);

	// test on a too large index
	index = 100;
		
	ret = iTelServer.GetTsyName(index, tsyName);
	ASSERT_EQUALS(KErrNotFound, ret);

	index = 0;

	// test on buffer overflow
	TBuf<1> littleName(KNullDesC);
	ret = iTelServer.GetTsyName(index, littleName);
	ASSERT_EQUALS(KErrOverflow, ret);
	
	CleanupStack::PopAndDestroy(1, this);

	}



/**
@SYMTestCaseID BA-CTSY-SESMG-TEP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::EnumeratePhones
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::EnumeratePhones
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestEnumeratePhones0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TInt ret = iTelServer.UnloadPhoneModule(KMmTsyModuleName);
	ASSERT_EQUALS(KErrNone, ret);
	
	TInt numPhones;
	
	// check if there are no phones loaded
	ret = iTelServer.EnumeratePhones(numPhones);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(0, numPhones);

	// now add a phone
	ret = iTelServer.LoadPhoneModule(KMmTsyModuleName);
	ASSERT_EQUALS(KErrNone, ret);

	ret = iTelServer.EnumeratePhones(numPhones);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_EQUALS(1, numPhones);

	CleanupStack::PopAndDestroy(1, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SESMG-TUPM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::UnloadPhoneModule
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::UnloadPhoneModule
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestUnloadPhoneModule0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	// test for loaded module
	TInt ret = iTelServer.LoadPhoneModule(KMmTsyModuleName);
	ASSERT_EQUALS(KErrNone, ret);

	ret = iTelServer.UnloadPhoneModule(KMmTsyModuleName);
	ASSERT_EQUALS(KErrNone, ret);


	// test for unload with extension
	//use the same name, but with extension
	_LIT(KMyTsyName, "phonetsy.tsy");

	ret = iTelServer.LoadPhoneModule(KMyTsyName);
	ASSERT_EQUALS(KErrNone, ret);

	ret = iTelServer.UnloadPhoneModule(KMyTsyName);
	ASSERT_EQUALS(KErrNone, ret);


	CleanupStack::PopAndDestroy(1, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-TUPM-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::UnloadPhoneModule with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::UnloadPhoneModule with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestUnloadPhoneModule0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	// test on bad name
	_LIT(KNoName, "noname");

	TInt ret = iTelServer.UnloadPhoneModule(KNoName);
	ASSERT_EQUALS(KErrNotFound, ret);


	// test for KNullDesC as module name

	ret = iTelServer.UnloadPhoneModule(KNullDesC);
	ASSERT_EQUALS(KErrNotFound, ret);

	CleanupStack::PopAndDestroy(1, this);

	}



/**
@SYMTestCaseID BA-CTSY-SESMG-TGTVN-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::GetTsyVersionNumber
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::GetTsyVersionNumber
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestGetTsyVersionNumber0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TVersion version(0, 0, 0);

	TInt ret = iTelServer.GetTsyVersionNumber(KMmTsyModuleName, version);
	ASSERT_EQUALS(KErrNone, ret);
	ASSERT_TRUE(0 <= version.iBuild);	// just common sense check
	ASSERT_TRUE(0 <= version.iMajor);
	ASSERT_TRUE(0 <= version.iMinor);

	CleanupStack::PopAndDestroy(1, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-TGTVN-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::GetTsyVersionNumber with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::GetTsyVersionNumber with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestGetTsyVersionNumber0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	TVersion version(0, 0, 0);


	_LIT(KBadName, "BadName");

	// test not existiong tsy
	TInt ret = iTelServer.GetTsyVersionNumber(KBadName, version);
	ASSERT_EQUALS(KErrNotFound, ret);


	TFileName bigName;

	bigName.SetMax();	
	bigName.Repeat(_L(" "));

	// check buffer overlow, Etel calls PanicClient
	ret = iTelServer.GetTsyVersionNumber(bigName, version);
	ASSERT_EQUALS(KErrGeneral, ret);

	CleanupStack::PopAndDestroy(2, this); 

	}


/**
@SYMTestCaseID BA-CTSY-SESMG-TSEEG-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RTelServer::SetExtendedErrorGranularity
@SYMTestPriority High
@SYMTestActions Invokes RTelServer::SetExtendedErrorGranularity
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestSetExtendedErrorGranularity0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RTelServer::TErrorGranularity granularity(RTelServer::EErrorExtended);

	TInt ret = iTelServer.SetExtendedErrorGranularity(granularity);
	ASSERT_EQUALS(KErrNone, ret);

	CleanupStack::PopAndDestroy(1, this); 
	
	}




void CCTsySessionManagementFU::AuxReferenceCountL(const TDesC &aName)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhoneL();

    TName lineName(aName);    
    RLine line;
    
    TInt ret  = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(line);  
    
    TName name;
	RCall call;
    
    ret = OpenNewCall(line, call, lineName, name);    
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call);    

	// test on successful completion
	TInt count = call.ReferenceCount();
    ASSERT_EQUALS(1, count);

	AssertMockLtsyStatusL();

	// test on successful completion, increasing count
	RCall call2;
	
	ret = call2.OpenExistingCall(line, name);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call2);  
	
	count = call.ReferenceCount();
    ASSERT_EQUALS(2, count);

	AssertMockLtsyStatusL();

	// test on successful completion, decreasing count
	CleanupStack::PopAndDestroy(1);

	count = call.ReferenceCount();
    ASSERT_EQUALS(1, count);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-CRC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::ReferenceCount for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::ReferenceCount for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestReferenceCount0001L()
	{

	AuxReferenceCountL(KMmTsyVoice1LineName);
	
	}





/**
@SYMTestCaseID BA-CTSY-SESMG-CRC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::ReferenceCount for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::ReferenceCount for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestReferenceCount0006L()
	{

	AuxReferenceCountL(KMmTsyDataLineName);

	}




/**
@SYMTestCaseID BA-CTSY-SESMG-CRC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::ReferenceCount for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::ReferenceCount for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestReferenceCount00011L()
	{

	AuxReferenceCountL(KMmTsyFaxLineName);	
	
	}




void CCTsySessionManagementFU::AuxOpenExisting1L(const TDesC &aName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(aName);   
    RLine line;
    
    TInt ret  = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(line);  
    
    TName callname;
	RCall call;
	
    ret = OpenNewCall(line, call, lineName, callname);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call);    


	// test on successful completion
	RCall call2;
	
	ret = call2.OpenExistingCall(line, callname);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call2);  
	
	AssertMockLtsyStatusL();

	// test on successful completion (another params set)
    TName name;
    name = lineName;
    name.Append(KDoubleColon);
    name.Append(callname);

	RCall call3;
	
	ret = call3.OpenExistingCall(iPhone, name);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call3);  
	
	AssertMockLtsyStatusL();

	// test on successful completion (another params set)
	RCall call4;

    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    name.Append(callname);
	
	ret = call4.OpenExistingCall(iTelServer, name);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call4);  

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(6, this); 
		
	}



void CCTsySessionManagementFU::AuxOpenExisting3L(const TDesC &aName)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(aName);    
    RLine line;
    
    TInt ret  = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(line);  
    
    TName callname;
	RCall call;
	
    ret = OpenNewCall(line, call, lineName, callname);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call);    


	// test on bad call name

	_LIT(KBadCallName, "BadCallName");

	RCall call2;
	
	ret = call2.OpenExistingCall(line, KBadCallName);
    ASSERT_EQUALS(KErrNotFound, ret);
	
	AssertMockLtsyStatusL();


	// test on bad line name

	_LIT(KBadLineName, "BadLineName");

    TName name;
    name = KBadLineName;
    name.Append(KDoubleColon);
    name.Append(callname);

	ret = call2.OpenExistingCall(iPhone, name);
    ASSERT_EQUALS(KErrNotSupported, ret);
	
	AssertMockLtsyStatusL();


	// test on bad call name and correct line

    name = lineName;
    name.Append(KDoubleColon);
    name.Append(KBadCallName);

	ret = call2.OpenExistingCall(iPhone, name);
    ASSERT_EQUALS(KErrNotFound, ret);
	
	AssertMockLtsyStatusL();


	// test on bad phone name

	_LIT(KBadPhoneName, "BadPhoneName");

    name = KBadPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    name.Append(callname);
	
	ret = call2.OpenExistingCall(iTelServer, name);
    ASSERT_EQUALS(KErrNotFound, ret);

	AssertMockLtsyStatusL();


	// test on bad line name and correct phone

    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(KBadLineName);
    name.Append(KDoubleColon);
    name.Append(callname);
	
	ret = call2.OpenExistingCall(iTelServer, name);
    ASSERT_EQUALS(KErrNotSupported, ret);

	AssertMockLtsyStatusL();


	// test on bad call name with correct phone and line

    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    name.Append(KBadCallName);
	
	ret = call2.OpenExistingCall(iTelServer, name);
    ASSERT_EQUALS(KErrNotFound, ret);


	// test on string analyze

	_LIT(KBad, "::::::::");

    name = KBad;
	
	ret = call2.OpenExistingCall(iTelServer, name);
    ASSERT_EQUALS(KErrNotFound, ret);

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-SESMG-COEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::OpenExistingCall for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::OpenExistingCall for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpenExistingCall0001L()
	{
	
	AuxOpenExisting1L(KMmTsyVoice1LineName);
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-COEC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::OpenExistingCall with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::OpenExistingCall with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpenExistingCall0003L()
	{
	
	AuxOpenExisting3L(KMmTsyVoice1LineName);
	
	}



/**
@SYMTestCaseID BA-CTSY-SESMG-COEC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::OpenExistingCall for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::OpenExistingCall for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpenExistingCall0006L()
	{
	
	AuxOpenExisting1L(KMmTsyDataLineName);
	
	}




/**
@SYMTestCaseID BA-CTSY-SESMG-COEC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::OpenExistingCall with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::OpenExistingCall with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpenExistingCall0008L()
	{

	AuxOpenExisting3L(KMmTsyDataLineName);

	}




/**
@SYMTestCaseID BA-CTSY-SESMG-COEC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::OpenExistingCall for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::OpenExistingCall for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpenExistingCall00011L()
	{

	AuxOpenExisting1L(KMmTsyFaxLineName);	
	
	}


/**
@SYMTestCaseID BA-CTSY-SESMG-COEC-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::OpenExistingCall with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::OpenExistingCall with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestOpenExistingCall00013L()
	{

	AuxOpenExisting3L(KMmTsyFaxLineName);	

	}



/**
@SYMTestCaseID BA-CTSY-SESMG-MIM-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::InitialiseMM
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::InitialiseMM
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySessionManagementFU::TestInitialiseMM0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	
	TRequestStatus requestStatus;

	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId = 0;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	
	iPhone.InitialiseMM(requestStatus, tableSettingsPckg);
	
	User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); 
	
	}

