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
// The TEFUnit test suite for LineControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsylinecontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "MmTsy_conf.h"

CTestSuite* CCTsyLineControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetHookStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetHookStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetHookStatus0003L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetCaps0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetCaps00011L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetInfo00011L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetStatus0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetStatus00011L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetLineInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetLineInfo0003L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestEnumerateLines0001L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyCapsChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyCapsChange00012L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyStatusChange00014L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetMobileLineStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetMobileLineStatus0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestGetMobileLineStatus00011L);

	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyLineControlFU, TestNotifyMobileLineStatusChange00014L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-LINC-LGHS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetHookStatus for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetHookStatus for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetHookStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
    
    // Test API is not supported
    RCall::THookStatus hookStatus;
    err = line.GetHookStatus(hookStatus);    
    ASSERT_EQUALS(KErrNotSupported, err);    
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // line, this
	
	}

 
/**
@SYMTestCaseID BA-CTSY-LINC-LGHS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetHookStatus for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetHookStatus for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetHookStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
    
    // Test API is not supported
    RCall::THookStatus hookStatus;
    err = line.GetHookStatus(hookStatus);    
    ASSERT_EQUALS(KErrNotSupported, err); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGHS-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetHookStatus for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetHookStatus for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetHookStatus0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
    
    // Test API is not supported
    RCall::THookStatus hookStatus;
    err = line.GetHookStatus(hookStatus);  
    ASSERT_EQUALS(KErrNotSupported, err); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCaps for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCaps for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetCaps 
	// Get initial value of caps for KMmTsyVoice1LineName-line
	//-------------------------------------------------------------------------	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
		
   	RLine::TCaps expectedCaps;
	expectedCaps.iFlags = RLine::KCapsVoice | RLine::KCapsEventIncomingCall;	 	
	RLine::TCaps caps;
		
	err = line.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(expectedCaps.iFlags, caps.iFlags);	
	AssertMockLtsyStatusL();	
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetCaps 
	// Get initial value of caps for KMmTsyVoice2LineName-line
	//-------------------------------------------------------------------------	
	RLine line2;
	err = line2.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line2);
		
   	expectedCaps.iFlags = RLine::KCapsVoice | RLine::KCapsEventIncomingCall | RMobileLine::KCapsAuxVoice;	 	
			
	err = line2.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, err)
	ASSERT_EQUALS(expectedCaps.iFlags, caps.iFlags);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line2, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-LGC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCaps for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCaps for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetCaps0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetCaps 
	// Get initial value of caps for KMmTsyDataLineName-line
	//-------------------------------------------------------------------------	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
		
   	RLine::TCaps expectedCaps;
	expectedCaps.iFlags = RLine::KCapsData | RLine::KCapsEventIncomingCall;	 	
	RLine::TCaps caps;
		
	err = line.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(expectedCaps.iFlags, caps.iFlags);	
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(2, this); // line, this	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCaps for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCaps for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetCaps00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetCaps 
	// Get initial value of caps for KMmTsyFaxLineName-line
	//-------------------------------------------------------------------------	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
		
   	RLine::TCaps expectedCaps;
	expectedCaps.iFlags = RLine::KCapsFax | RLine::KCapsEventIncomingCall;	 	
	RLine::TCaps caps;
		
	err = line.GetCaps(caps);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(expectedCaps.iFlags, caps.iFlags);	
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(2, this); // line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-LGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetInfo for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetInfo for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetInfo0001L()
	{		
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();		

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetInfo 
	// Get initial value of caps for KMmTsyVoice1LineName-line
	//-------------------------------------------------------------------------	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
			
	RLine::TLineInfo info;	
	AssertMockLtsyStatusL();	
	err = line.GetInfo(info);
	ASSERT_EQUALS(KErrNone, err);	
	ASSERT_EQUALS (RCall::EHookStatusUnknown, info.iHookStatus);// hard-coded value
	ASSERT_EQUALS(RCall::EStatusIdle, info.iStatus);
	TName emptyName;
	TName voice11(_L("Voice11"));
	TName voice12(_L("Voice12"));
	ASSERT_EQUALS(emptyName, info.iNameOfCallForAnswering);
	ASSERT_EQUALS(voice11, info.iNameOfLastCallAdded);
	
	//-------------------------------------------------------------------------
	// TEST D: RLine::GetInfo again, this time CTSY
	// will get result from the cache.	
 	//-----------------------------------------------------------------------	
	
	//-------------------------------------------------------------------------
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);
	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
 	//-------------------------------------------------------------------------
 		
 	// Get changed info
 	err = line.GetInfo(info);
 	ASSERT_EQUALS (RCall::EHookStatusUnknown, info.iHookStatus);// hard-coded value
 	ASSERT_EQUALS(RCall::EStatusRinging, info.iStatus);
	ASSERT_EQUALS(voice11, info.iNameOfCallForAnswering);
	ASSERT_EQUALS(voice12, info.iNameOfLastCallAdded);	
	AssertMockLtsyStatusL();		
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetInfo for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetInfo for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetInfo0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();	


	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetInfo 
	// Get initial value of caps for KMmTsyVoice1LineName-line
	//-------------------------------------------------------------------------	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
			
	RLine::TLineInfo info;		
	err = line.GetInfo(info);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, err);	
	ASSERT_EQUALS (RCall::EHookStatusUnknown, info.iHookStatus);// hard-coded value
	ASSERT_EQUALS(RCall::EStatusIdle, info.iStatus);
	TName emptyName;
	TName data1(_L("Data1"));
	TName data2(_L("Data2"));
	ASSERT_EQUALS(emptyName, info.iNameOfCallForAnswering);
	ASSERT_EQUALS(data1, info.iNameOfLastCallAdded);
	
	//-------------------------------------------------------------------------
	// TEST D: RLine::GetInfo again, this time CTSY
	// will get result from the cache.	
 	//-----------------------------------------------------------------------	
	
	//-------------------------------------------------------------------------
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);
	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
 	//-------------------------------------------------------------------------
 		
 	// Get changed info
 	err = line.GetInfo(info);
 	ASSERT_EQUALS (RCall::EHookStatusUnknown, info.iHookStatus);// hard-coded value
 	ASSERT_EQUALS(RCall::EStatusRinging, info.iStatus);
	ASSERT_EQUALS(data1, info.iNameOfCallForAnswering);
	ASSERT_EQUALS(data2, info.iNameOfLastCallAdded);	
	AssertMockLtsyStatusL();		
	
	CleanupStack::PopAndDestroy(3, this); // call, line,  this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetInfo for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetInfo for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetInfo00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();		

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetInfo 
	// Get initial value of caps for KMmTsyVoice1LineName-line
	//-------------------------------------------------------------------------	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
			
	RLine::TLineInfo info;		
	err = line.GetInfo(info);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, err);	
	ASSERT_EQUALS (RCall::EHookStatusUnknown, info.iHookStatus);// hard-coded value
	ASSERT_EQUALS(RCall::EStatusIdle, info.iStatus);
	TName emptyName;
	TName fax1(_L("Fax1"));
	TName fax2(_L("Fax2"));
	ASSERT_EQUALS(emptyName, info.iNameOfCallForAnswering);
	ASSERT_EQUALS(fax1, info.iNameOfLastCallAdded);
	
	//-------------------------------------------------------------------------
	// TEST D: RLine::GetInfo again, this time CTSY
	// will get result from the cache.	
 	//-----------------------------------------------------------------------		

	//-------------------------------------------------------------------------
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);
	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);    
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
 	//-------------------------------------------------------------------------
 		
 	// Get changed info
 	err = line.GetInfo(info);
 	ASSERT_EQUALS (RCall::EHookStatusUnknown, info.iHookStatus);// hard-coded value
 	ASSERT_EQUALS(RCall::EStatusRinging, info.iStatus);
	ASSERT_EQUALS(fax1, info.iNameOfCallForAnswering);
	ASSERT_EQUALS(fax2, info.iNameOfLastCallAdded);	
	AssertMockLtsyStatusL();		
	
	CleanupStack::PopAndDestroy(3, this); // call, line,  this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetStatus for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetStatus for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
   	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetStatus when result is not cached.
 	//-------------------------------------------------------------------------
	RCall::TStatus callStatus;
	// get initial status
    err = line.GetStatus(callStatus);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RCall::EStatusIdle, callStatus);    
    
 	//-------------------------------------------------------------------------
	// TEST D: RLine::GetStatus again, this time CTSY
	// will get result from the cache.
	// modify status by creating incoming call
 	//------------------------------------------------------------------------- 
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
     
 	// get modified status
    err = line.GetStatus(callStatus);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RCall::EStatusRinging, callStatus);
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //call, line,  this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetStatus for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetStatus for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetStatus0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
   	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetStatus when result is not cached.
 	//-------------------------------------------------------------------------
	RCall::TStatus callStatus;
	// get initial status
    err = line.GetStatus(callStatus);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RCall::EStatusIdle, callStatus);    
    
 	//-------------------------------------------------------------------------
	// TEST D: RLine::GetStatus again, this time CTSY
	// will get result from the cache.
	// modify status by creating incoming call
 	//------------------------------------------------------------------------- 
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
     
 	// get modified status
    err = line.GetStatus(callStatus);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RCall::EStatusRinging, callStatus);
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGS-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetStatus for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetStatus for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetStatus00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
   	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::GetStatus when result is not cached.
 	//-------------------------------------------------------------------------
	RCall::TStatus callStatus;
	// get initial status
    err = line.GetStatus(callStatus);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RCall::EStatusIdle, callStatus);    
    
 	//-------------------------------------------------------------------------
	// TEST D: RLine::GetStatus again, this time CTSY
	// will get result from the cache.
	// modify status by creating incoming call
 	//------------------------------------------------------------------------- 
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
     
 	// get modified status
    err = line.GetStatus(callStatus);
    ASSERT_EQUALS(KErrNone, err);
    ASSERT_EQUALS(RCall::EStatusRinging, callStatus);
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-PGLI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::GetLineInfo
@SYMTestPriority High
@SYMTestActions Invokes RPhone::GetLineInfo
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetLineInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// TEST with no line opened
	// get initial LineInfo with index = 0
 	//------------------------------------------------------------------------- 	
	TInt index(0);
	RPhone::TLineInfo lineInfo;
	TInt err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusUnknown);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)(RLine::KCapsVoice|RLine::KCapsEventIncomingCall));
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyVoice1LineName));	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST with no line opened
	// get initial LineInfo with index = 1
 	//------------------------------------------------------------------------- 	
	index = 1;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusUnknown);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010008);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyVoice2LineName));	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST with no line opened
	// get initial LineInfo with index = 2
 	//------------------------------------------------------------------------- 	
	index = 2;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusUnknown);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x10001);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyDataLineName));	
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST with no line opened
	// get initial LineInfo with index = 3
 	//------------------------------------------------------------------------- 	
	index = 3;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusUnknown);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x10002);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyFaxLineName));	
	AssertMockLtsyStatusL();
		
	
	//-------------------------------------------------------------------------
	// TEST with with line opened
	// get LineInfo for Voice1 line
 	//------------------------------------------------------------------------- 
	// now create voice1 line
	RLine line;
	err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
    
    // get initial voice1 line info 
    index = 0;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusIdle);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010004);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyVoice1LineName));	
	AssertMockLtsyStatusL();
	
	// ----------- change voice1 line info --------------------------------------
	// create new incoming call
	TInt callId(1);
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    TName incomingCallName;	
	err = CreateIncomingCallL(line, callId, KNullDesC, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call;	
    err = call.OpenExistingCall(line, incomingCallName);    
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call);
    // --------------------------------------------------------------------------
    	
	// get changed voice1 line info
	index = 0;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusRinging);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010004);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyVoice1LineName));	
	AssertMockLtsyStatusL();
	
			
	//-------------------------------------------------------------------------
	// TEST with with line opened
	// get LineInfo for Voice2 line
 	//------------------------------------------------------------------------- 
	// now create voice2 line
	RLine line2;
	err = line2.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line2);
    
    // get initial voice2 line info 
    index = 1;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusIdle);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x0001000C);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyVoice2LineName));	
	AssertMockLtsyStatusL();
	
	// ----------- change voice2 line info --------------------------------------
	// create new incoming call
	TInt callId2(2);
	RMobileCall::TMobileCallStatus mobileCallStatus2 = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService2 = RMobilePhone::EAuxVoiceService;
    TName incomingCallName2;    
	err = CreateIncomingCallL(line2, callId2, KNullDesC, incomingCallName2, 
	                               mobileService2, mobileCallStatus2);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call2;	
    err = call2.OpenExistingCall(line2, incomingCallName2);    
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call2);
    // --------------------------------------------------------------------------
    	
	// get changed voice2 line info
	index = 1;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusRinging);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x0001000C);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyVoice2LineName));	
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// TEST with with line opened
	// get LineInfo for Data line
 	//------------------------------------------------------------------------- 
	// now create Data line
	RLine line3;
	err = line3.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line3);
    
    // get initial Data line info 
    index = 2;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusIdle);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010001);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyDataLineName));	
	AssertMockLtsyStatusL();
	
	// ----------- change Data line info --------------------------------------
	// create new incoming call
	TInt callId3(3);
	RMobileCall::TMobileCallStatus mobileCallStatus3 = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService3 = RMobilePhone::ECircuitDataService;
    TName incomingCallName3;    
	err = CreateIncomingCallL(line3, callId3, KNullDesC, incomingCallName3, 
	                               mobileService3, mobileCallStatus3);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call3;	
    err = call3.OpenExistingCall(line3, incomingCallName3);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call3);
    // --------------------------------------------------------------------------
    	
	// get changed Data line info
	index = 2;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusRinging);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010001);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyDataLineName));	
	AssertMockLtsyStatusL();
	
	
	//-------------------------------------------------------------------------
	// TEST with with line opened
	// get LineInfo for Fax line
 	//------------------------------------------------------------------------- 
	// now create Fax line
	RLine line4;
	err = line4.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line4);
    
    // get initial Fax line info 
    index = 3;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusIdle);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010002);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyFaxLineName));	
	AssertMockLtsyStatusL();
	
	// ----------- change Fax line info --------------------------------------
	// create new incoming call
	TInt callId4(4);
	RMobileCall::TMobileCallStatus mobileCallStatus4 = RMobileCall::EStatusRinging;
    RMobilePhone::TMobileService mobileService4 = RMobilePhone::EFaxService;
    TName incomingCallName4;    
	err = CreateIncomingCallL(line4, callId4, KNullDesC, incomingCallName4, 
	                               mobileService4, mobileCallStatus4);	
	ASSERT_EQUALS(KErrNone, err);	
	RCall call4;	
    err = call4.OpenExistingCall(line4, incomingCallName4);
    ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(call4);
    // --------------------------------------------------------------------------
    	
	// get changed Fax line info
	index = 3;
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusRinging);	
	ASSERT_EQUALS(lineInfo.iLineCapsFlags, (TUint32)0x00010002);
	ASSERT_TRUE(0 == lineInfo.iName.Compare(KMmTsyFaxLineName));	
	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(9, this); // ...  this	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-PGLI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::GetLineInfo with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RPhone::GetLineInfo with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetLineInfo0003L()
	{
// This test should test sending bad parameter data for GetLineInfo

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	
	TInt index(-1); // out of bound undex 
	RPhone::TLineInfo lineInfo;
	TInt err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNotFound, err);
	AssertMockLtsyStatusL();	
		
	index = KMmTsyNumOfSupportedLines; // out of bound undex
	err = iPhone.GetLineInfo(index, lineInfo);
	ASSERT_EQUALS(KErrNotFound, err);
	AssertMockLtsyStatusL();

	// Done !
	CleanupStack::PopAndDestroy(1, this); //  this

	}


/**
@SYMTestCaseID BA-CTSY-LINC-PEL-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RPhone::EnumerateLines
@SYMTestPriority High
@SYMTestActions Invokes RPhone::EnumerateLines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestEnumerateLines0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	//-------------------------------------------------------------------------
	// TEST C
 	//------------------------------------------------------------------------- 	
	TInt cnt;
	TInt err = iPhone.EnumerateLines(cnt);
	ASSERT_EQUALS(KErrNone, err);
	ASSERT_EQUALS(KMmTsyNumOfSupportedLines, cnt); // hard-coded value
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(1, this); //  this	
	}
	
	

/**
@SYMTestCaseID BA-CTSY-LINC-LNCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyCapsChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyCapsChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyCapsChange0002L()
	{

// This test should test cancellation of NotifyCapsChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);

 	//-------------------------------------------------------------------------
	// Test cancelling of RLine::NotifyCapsChange
 	//------------------------------------------------------------------------- 	
 	TRequestStatus reqStatus;
    RLine::TCaps caps;
    line.NotifyCapsChange(reqStatus, caps);
    
    // cancelling of RLine::NotifyCapsChange
    line.CancelAsyncRequest(EETelLineCapsChangeNotification);
    
    User::WaitForRequest(reqStatus);
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());	
 
	CleanupStack::PopAndDestroy(2); // line, this	
	}



/**
@SYMTestCaseID BA-CTSY-LINC-LNCC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyCapsChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyCapsChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyCapsChange0007L()
	{

// This test should test cancellation of NotifyCapsChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);

 	//-------------------------------------------------------------------------
	// Test cancelling of RLine::NotifyCapsChange
 	//------------------------------------------------------------------------- 	
 	TRequestStatus reqStatus;
    RLine::TCaps caps;
    line.NotifyCapsChange(reqStatus, caps);
    
    // cancelling of RLine::NotifyCapsChange
    line.CancelAsyncRequest(EETelLineCapsChangeNotification);
    
    User::WaitForRequest(reqStatus);
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());	
 
	CleanupStack::PopAndDestroy(2); // line, this		
	}



/**
@SYMTestCaseID BA-CTSY-LINC-LNCC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyCapsChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyCapsChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyCapsChange00012L()
	{

// This test should test cancellation of NotifyCapsChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);

 	//-------------------------------------------------------------------------
	// Test cancelling of RLine::NotifyCapsChange
 	//------------------------------------------------------------------------- 	
 	TRequestStatus reqStatus;
    RLine::TCaps caps;
    line.NotifyCapsChange(reqStatus, caps);
    
    // cancelling of RLine::NotifyCapsChange
    line.CancelAsyncRequest(EETelLineCapsChangeNotification);
    
    User::WaitForRequest(reqStatus);
    AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());	
 
	CleanupStack::PopAndDestroy(2); // line, this	
	
	}


	
/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::NotifyStatusChange when result is not cached.
 	//------------------------------------------------------------------------- 	

    RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    // -----------  change Voice status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;
  
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);      
	AssertMockLtsyStatusL();
    
    // close line
    CloseLineL(line, callId, mobileService);    
    CleanupStack::Pop(1); //line
    AssertMockLtsyStatusL();
    
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RLine::NotifyStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------

    //open new line 
	err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
    // use another status for coverage increasing
	mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);     
	   
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData3(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData3.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());      
    
    //-------------------------------------------------------------------------
	// TEST C: 
	// for voice KMmTsyVoice2LineName and for coverage increase
 	//------------------------------------------------------------------------- 

    RLine line2;
	err = line2.Open(iPhone, KMmTsyVoice2LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line2);
    
    line2.NotifyStatusChange(reqStatus, callStatus);
    
    // ---try to change Voice2 status 
    // with mobileCallInfo.iStatus = RMobileCall::EStatusUnknown----------------
   	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    callId = 2;
    mobileService = RMobilePhone::EAuxVoiceService;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData4(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData4.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);      
	AssertMockLtsyStatusL();
  
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::NotifyStatusChange when event EEtelLineNotifyCallAdded occurs
 	//------------------------------------------------------------------------- 

    RLine line3;
	err = line3.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line3);
    
   	line3.NotifyStatusChange(reqStatus, callStatus);
    
    // completes NotifyStatusChange    
 	iMockLTSY.NotifyTerminated(mockLtsyStatus);
 	
 	callId = 1;
 	mobileCallInfo.iStatus = RMobileCall::EStatusConnected;
 	mobileService = RMobilePhone::ECircuitDataService;
 	   
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData6(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData6.SerialiseL(data);
		
	iMockLTSY.CompleteL(EEtelLineNotifyCallAdded, KErrNone, data);
	
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	// wait for NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);      
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); //line3, line2, line, expectData, data, this
	
	}	
	


/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0001a
@SYMDefectId  DEF126595
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0001aL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	//Line name is set to lower case, "voice1"
	RLine line;
	_LIT( KMmTsyVoice1LineNameLowerCase, "voice1");
	TInt err = line.Open(iPhone, KMmTsyVoice1LineNameLowerCase);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::NotifyStatusChange when result is not cached
    // and voice line name is lower case.
 	//------------------------------------------------------------------------- 	
    RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    // -----------  change Voice status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;
  
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);      
	AssertMockLtsyStatusL();
    
	//Get Call Info
	RLine::TCallInfo callInfo;
    TInt ret = line.GetCallInfo( 0, callInfo);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(callInfo.iStatus==RCall::EStatusDialling);
	
    // close line
    CloseLineL(line, callId, mobileService);    
    CleanupStack::Pop(1); //line
    AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(3, this); //
	}	
	
/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0002L()
	{

// This test should test cancellation of NotifyStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
 	//-------------------------------------------------------------------------
	// Test cancelling of RLine::NotifyStatusChange
 	//-------------------------------------------------------------------------
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;  
    
 	line.NotifyStatusChange(reqStatus, callStatus);
 	
 	// cancel request
    line.CancelAsyncRequest(EEtelLineNotifyStatusChange);
 	 	
 	User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());
 
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // line,   this
	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);	
    
    RLine line2;
	err = line2.Open(phone2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RLine::NotifyStatusChange
 	//------------------------------------------------------------------------- 	
 	//first client
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    //second client
 	RCall::TStatus callStatus2;
    TRequestStatus reqStatus2;     
    line2.NotifyStatusChange(reqStatus2, callStatus2);    
    
    // -----------  change Voice status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for first NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);  
    
     // wait for second NotifyStatusChange completion
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus2);   	

	// Done !
	CleanupStack::PopAndDestroy(6, this); // .... phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyStatusChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyStatusChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::NotifyStatusChange when result is not cached.
 	//------------------------------------------------------------------------- 	
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    // -----------  change Data status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnected;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);   
    
  	// close line
  	mobileService = RMobilePhone::ECircuitDataService;
    CloseLineL(line, callId, mobileService);    
    CleanupStack::Pop(1); //line
    AssertMockLtsyStatusL();	
   		
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RLine::NotifyStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------
 	//open new line 
	err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
    // use another status for coverage increasing
	mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);   	   
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData2.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());   
    
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // line, expectData, data, this
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyStatusChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyStatusChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0007L()
	{

// This test should test cancellation of NotifyStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
 	//-------------------------------------------------------------------------
	// Test cancelling of RLine::NotifyStatusChange
 	//-------------------------------------------------------------------------
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;  
    
 	line.NotifyStatusChange(reqStatus, callStatus);
 	
 	// cancel request
 	line.CancelAsyncRequest(EEtelLineNotifyStatusChange);
 	 	
 	User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());
 
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // line,   this
	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyStatusChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyStatusChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange0009L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);	
    
    RLine line2;
	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RLine::NotifyStatusChange
 	//------------------------------------------------------------------------- 	
 	//first client
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    //second client
 	RCall::TStatus callStatus2;
    TRequestStatus reqStatus2;     
    line2.NotifyStatusChange(reqStatus2, callStatus2);    
    
    // -----------  change Data status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;      
     
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for first NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);  
    
     // wait for second NotifyStatusChange completion
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus2);   	

	// Done !
	CleanupStack::PopAndDestroy(6, this); // .... phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyStatusChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyStatusChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
 
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RLine::NotifyStatusChange when result is not cached.
 	//------------------------------------------------------------------------- 	
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    // -----------  change Data status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnected;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);   
    
   
    mobileService = RMobilePhone::EFaxService;  
   	// close line
    CloseLineL(line, callId, mobileService);    
    CleanupStack::Pop(1); //line
    AssertMockLtsyStatusL();	
   	
   		
 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RLine::NotifyStatusChange
	// from LTSY.
 	//-------------------------------------------------------------------------
 	//open new line 
	err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
    // use another status for coverage increasing
	mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);        
    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData2.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());   
    
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //line, expectData, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyStatusChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyStatusChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange00012L()
	{

// This test should test cancellation of NotifyStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);
    
 	//-------------------------------------------------------------------------
	// Test cancelling of RLine::NotifyStatusChange
 	//-------------------------------------------------------------------------
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;  
    
 	line.NotifyStatusChange(reqStatus, callStatus);
 	
 	// cancel request
 	line.CancelAsyncRequest(EEtelLineNotifyStatusChange);
 	 	
 	User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());
 
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // line,   this
	
	}



/**
@SYMTestCaseID BA-CTSY-LINC-LNSC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyStatusChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyStatusChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyStatusChange00014L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	
	RLine line;
	TInt err = line.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line);	
    
    RLine line2;
	err = line2.Open(phone2, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(line2);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RLine::NotifyStatusChange
 	//------------------------------------------------------------------------- 	
 	//first client
 	RCall::TStatus callStatus;
    TRequestStatus reqStatus;     
    line.NotifyStatusChange(reqStatus, callStatus);
    
    //second client
 	RCall::TStatus callStatus2;
    TRequestStatus reqStatus2;     
    line2.NotifyStatusChange(reqStatus2, callStatus2);    
    
    // -----------  change Data status ---------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    //------------------------------------------------------------------------- 
    
    // wait for first NotifyStatusChange completion
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);  
    
     // wait for second NotifyStatusChange completion
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());    
    // check callStatus
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus2);   	

	// Done !
	CleanupStack::PopAndDestroy(6, this); // .... phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGMLS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLine::GetMobileLineStatus for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RMobileLine::GetMobileLineStatus for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetMobileLineStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);
        
    // Get initial MobileLineStatus
    RMobileCall::TMobileCallStatus callStatus;    
    mobileLine.GetMobileLineStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);     
    
    //------- change MobileLineStatus -----------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;
        
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // Get changed MobileLineStatus    
    mobileLine.GetMobileLineStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileLine, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-LGMLS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLine::GetMobileLineStatus for data lines
@SYMTestPriority High
@SYMTestActions Invokes RMobileLine::GetMobileLineStatus for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetMobileLineStatus0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);
        
    // Get initial MobileLineStatus
    RMobileCall::TMobileCallStatus callStatus;    
    mobileLine.GetMobileLineStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);     
    
    //------- change MobileLineStatus -----------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // Get changed MobileLineStatus    
    mobileLine.GetMobileLineStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileLine, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LGMLS-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLine::GetMobileLineStatus for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RMobileLine::GetMobileLineStatus for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestGetMobileLineStatus00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);
        
    // Get initial MobileLineStatus
    RMobileCall::TMobileCallStatus callStatus;    
    mobileLine.GetMobileLineStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusIdle, callStatus);     
    
    //------- change MobileLineStatus -----------------------------------------
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // Get changed MobileLineStatus    
    mobileLine.GetMobileLineStatus(callStatus);
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus); 

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // mobileLine, data, this	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLine::NotifyMobileLineStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RMobileLine::NotifyMobileLineStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileLine::NotifyMobileLineStatusChange when result is not cached.
 	//-------------------------------------------------------------------------	
	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;
	
	// set notification
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);	
	
	//------- change MobileLineStatus -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);	
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // wait for notification
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);  	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //mobileLine, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileLine::NotifyMobileLineStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileLine::NotifyMobileLineStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange0002L()
	{

// This test should test cancellation of NotifyMobileLineStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
   	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileLine::NotifyMobileLineStatusChange
 	//-------------------------------------------------------------------------
 	
 	// set notification
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);	
	
	// cancelling of RMobileLine::NotifyMobileLineStatusChange
    mobileLine.CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);        
	
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());	
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); //mobileLine,  this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileLine::NotifyMobileLineStatusChange for voice lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileLine::NotifyMobileLineStatusChange for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange0004L()
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
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
    RMobileLine mobileLine2;
	err = mobileLine2.Open(phone2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine2);    

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileLine::NotifyMobileLineStatusChange
 	//-------------------------------------------------------------------------

   	// set first notification
	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;	
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);		
	
	// set second notification
	TRequestStatus reqStatus2;	
	RMobileCall::TMobileCallStatus callStatus2;	
	mobileLine2.NotifyMobileLineStatusChange(reqStatus2, callStatus2);		
	
	//------- change MobileLineStatus -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);	
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // wait for first notification
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);  	
    
    // wait for second notification
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus2);  	

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(6, this); // mobileLine2, mobileLine, phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLine::NotifyMobileLineStatusChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes RMobileLine::NotifyMobileLineStatusChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileLine::NotifyMobileLineStatusChange when result is not cached.
 	//-------------------------------------------------------------------------	
	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;
	
	// set notification
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);	
	
	//------- change MobileLineStatus -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);	
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // wait for notification
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);  	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //mobileLine, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileLine::NotifyMobileLineStatusChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileLine::NotifyMobileLineStatusChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange0007L()
	{

// This test should test cancellation of NotifyMobileLineStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
   	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileLine::NotifyMobileLineStatusChange
 	//-------------------------------------------------------------------------
 	
 	// set notification
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);	
	
	// cancelling of RMobileLine::NotifyMobileLineStatusChange
    mobileLine.CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);        
	
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); //mobileLine,  this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileLine::NotifyMobileLineStatusChange for data lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileLine::NotifyMobileLineStatusChange for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange0009L()
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
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
    RMobileLine mobileLine2;
	err = mobileLine2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine2);    

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileLine::NotifyMobileLineStatusChange
 	//-------------------------------------------------------------------------

   	// set first notification
	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;	
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);		
	
	// set second notification
	TRequestStatus reqStatus2;	
	RMobileCall::TMobileCallStatus callStatus2;	
	mobileLine2.NotifyMobileLineStatusChange(reqStatus2, callStatus2);		
	
	//------- change MobileLineStatus -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnected;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);	
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // wait for first notification
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);  	
    
    // wait for second notification
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus2);  	

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(6, this); // mobileLine2, mobileLine, phone2, telServer2, data, this
	}



/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileLine::NotifyMobileLineStatusChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RMobileLine::NotifyMobileLineStatusChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
    //-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileLine::NotifyMobileLineStatusChange when result is not cached.
 	//-------------------------------------------------------------------------	
	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;
	
	// set notification
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);	
	
	//------- change MobileLineStatus -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnecting;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);	
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // wait for notification
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);  	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //mobileLine, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileLine::NotifyMobileLineStatusChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileLine::NotifyMobileLineStatusChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange00012L()
	{

// This test should test cancellation of NotifyMobileLineStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
   	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;
	
 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileLine::NotifyMobileLineStatusChange
 	//-------------------------------------------------------------------------
 	
 	// set notification
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);	
	
	// cancelling of RMobileLine::NotifyMobileLineStatusChange
    mobileLine.CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);        
	
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int()); 	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); //mobileLine,  this
	
	}

/**
@SYMTestCaseID BA-CTSY-LINC-LNMLSC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileLine::NotifyMobileLineStatusChange for fax lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileLine::NotifyMobileLineStatusChange for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyLineControlFU::TestNotifyMobileLineStatusChange00014L()
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
	
	RMobileLine mobileLine;
	TInt err = mobileLine.Open(iPhone, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine);    
    
    RMobileLine mobileLine2;
	err = mobileLine2.Open(phone2, KMmTsyFaxLineName);
	ASSERT_EQUALS(KErrNone, err);
    CleanupClosePushL(mobileLine2);    

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileLine::NotifyMobileLineStatusChange
 	//-------------------------------------------------------------------------

   	// set first notification
	TRequestStatus reqStatus;	
	RMobileCall::TMobileCallStatus callStatus;	
	mobileLine.NotifyMobileLineStatusChange(reqStatus, callStatus);		
	
	// set second notification
	TRequestStatus reqStatus2;	
	RMobileCall::TMobileCallStatus callStatus2;	
	mobileLine2.NotifyMobileLineStatusChange(reqStatus2, callStatus2);		
	
	//------- change MobileLineStatus -----------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    TInt callId(1);          
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusConnected;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
			
	User::WaitForRequest(mockLtsyStatus);	
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());    
    //------------------------------------------------------------------------- 
    
    // wait for first notification
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus);  	
    
    // wait for second notification
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());    
    ASSERT_EQUALS(RMobileCall::EStatusDialling, callStatus2);  	

	AssertMockLtsyStatusL();
	// Done !
	CleanupStack::PopAndDestroy(6, this); // mobileLine2, mobileLine, phone2, telServer2, data, this

	}




