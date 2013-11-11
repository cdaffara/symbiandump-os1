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
// The TEFUnit test suite for CallOwnership in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallownershipfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyCallOwnershipFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestLoanDataPort0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestLoanDataPort0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestLoanDataPort0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestLoanDataPort0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestLoanDataPort00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestRecoverDataPort0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestRecoverDataPort0006L);
    ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestRecoverDataPort0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestRecoverDataPort00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestAcquireOwnership0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestAcquireOwnership0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestAcquireOwnership0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestAcquireOwnership0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestAcquireOwnership00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestTransferOwnership0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestTransferOwnership0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallOwnershipFU, TestTransferOwnership00011L);

	END_SUITE;
	}


/**
Test support failure in CTSY for RCall::LoanDataPort
@param aName The name of the line from which the call is to be opened to test for RCall::LoanDataPort
@param aCallId callId ltsy must allocate for the call
*/
void CCTsyCallOwnershipFU::TestLoanDataPortNotSupportedL(const TDesC &aName, const TInt aCallId)
	{

	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = OpenNewCall(line, call, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	RMobilePhone::TMobileService mobileService( RMobilePhone::EServiceUnspecified );
	if ( aName == KMmTsyVoice1LineName )
		{
		mobileService = RMobilePhone::EVoiceService;
		}
	else if ( aName == KMmTsyVoice2LineName )
		{
		mobileService = RMobilePhone::EAuxVoiceService;
		}
	else if ( aName == KMmTsyFaxLineName )
		{
		mobileService = RMobilePhone::EFaxService;
		}
	if ( (mobileService == RMobilePhone::EVoiceService) ||
			(mobileService == RMobilePhone::EAuxVoiceService) )
		{
		DialL(call, aCallId, mobileService);
		}

	// synchronous version testing
	RCall::TCommPort dataPort;
	dataPort.iCsy.Copy(_L("qwerty"));
	dataPort.iPort.Copy(_L("asdfgh"));
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNotSupported, err)
	AssertMockLtsyStatusL();
	
	// asynchronous version testing
	TRequestStatus reqStatus;
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(2); //call, line
	AssertMockLtsyStatusL();
	
	}


/**
Test support failure in CTSY for RCall::RecoverDataPort
@param aName The name of the line from which the call is to be opened to test for RCall::RecoverDataPort
*/
void CCTsyCallOwnershipFU::TestRecoverDataPortNotSupportedL(const TDesC &aName)
	{
	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = OpenNewCall(line, call, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	err = call.RecoverDataPort();
	ASSERT_EQUALS(KErrNotSupported, err)
	
	CleanupStack::PopAndDestroy(2); // call, line
	AssertMockLtsyStatusL();
	
	}


/**
Test support failure in CTSY for RCall::AcquireOwnership
@param aName The name of the line from which the call is to be opened to test for RCall::AcquireOwnership
*/
void CCTsyCallOwnershipFU::TestAcquireOwnershipNotSupportedL(const TDesC &aName)
	{
	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = OpenNewCall(line, call, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	TRequestStatus reqStatus;
	call.AcquireOwnership(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(2); // call, line
	AssertMockLtsyStatusL();
	
	}


/**
Test support failure in CTSY for RCall::TransferOwnership
@param aName The name of the line from which the call is to be opened to test for RCall::TransferOwnership
*/
void CCTsyCallOwnershipFU::TestTransferOwnershipNotSupportedL(const TDesC &aName)
	{
	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = OpenNewCall(line, call, aName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	err = call.TransferOwnership();
	ASSERT_EQUALS(KErrNotSupported, err)
	
	CleanupStack::PopAndDestroy(2); // call, line
	AssertMockLtsyStatusL();
	
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-COWN-CLDP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::LoanDataPort for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::LoanDataPort for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestLoanDataPort0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestLoanDataPortNotSupportedL(KMmTsyVoice1LineName, 1);

	TestLoanDataPortNotSupportedL(KMmTsyVoice2LineName, 2);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CLDP-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::LoanDataPort for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::LoanDataPort for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestLoanDataPort0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	TName callName;
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	// Prepare data
	TInt callId = 0;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	RCall::TCommPort expDataPort;
	expDataPort.iCsy.Copy(_L("qwerty"));
	expDataPort.iPort.Copy(_L("asdfgh"));
	TMockLtsyCallData1<RCall::TCommPort> expData1(callId, mobileService, expDataPort);
	
	callId = 1;
	TMockLtsyCallData1<RCall::TCommPort> expData2(callId, mobileService, expDataPort);	

	RCall::TCommPort dataPort;
	dataPort.iCsy.Copy(expDataPort.iCsy);
	dataPort.iPort.Copy(expDataPort.iPort);

	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// Test: failure to dispatch request to CTSY when call is not active
	//-------------------------------------------------------------------------
	
    // Basically succeeding of LoanDataPort() request depends from LTSY.
    // Probably LTSY returns error to LoanDataPort() request when data call is
    // not active. But in theory also KErrNone is possible.  
        
	// Synchronous version testing. LTSY returns KErrNone.
	expData1.SerialiseL(data); 
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data, KErrNone);
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL(); 

	// Asynchronous version testing. Fails because data port is already loaned.
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrEtelPortAlreadyLoaned, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	// Recover data port before next test phase.
	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data, KErrNone);
	err = call.RecoverDataPort();
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();       
    
	// Synchronous version testing. LTSY returns error KErrEtelCallNotActive.   
    iMockLTSY.ExpectL(EEtelCallLoanDataPort, data, KErrEtelCallNotActive);  	
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrEtelCallNotActive, err)
	AssertMockLtsyStatusL();	

	// Asynchronous version testing. LTSY returns error KErrEtelCallNotActive.
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data, KErrEtelCallNotActive); 
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrEtelCallNotActive, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	// Close data call. After this CCallBase should call CTSY's RelinquishOwnership()
	// method instead of RecoverDataPortAndRelinquishOwnership().		
	CleanupStack::PopAndDestroy(1); // call
	
	// Open data call again.
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);		                  	

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    callId = 1;

	DialL(call, callId, mobileService);
	
	data.Close();
	expData2.SerialiseL(data);
	
	// Synchronous version testing. LTSY returns error KErrNotSupported.
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data, KErrNotSupported);
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNotSupported, err)
	AssertMockLtsyStatusL();

	// Asynchronous version testing. LTSY returns error KErrNotSupported.
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data, KErrNotSupported);
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::LoanDataPort.
 	//-------------------------------------------------------------------------

	// Synchronous version testing. LTSY returns KErrNone.
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	// Turn back environment state.
	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);
	err = call.RecoverDataPort();
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	// Asynchronous version testing. LTSY returns KErrNone.
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

	// Turn back environment state.
	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);
	err = call.RecoverDataPort();
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when call is owned by other client
 	//-------------------------------------------------------------------------

	// Open second client.
	RTelServer telServer2;
	err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	RCall call2;
	RLine line2;

	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line2);

	err = call2.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call2);

	// Try to loan data port when call is owned by other client.
	err = call2.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrEtelNotCallOwner, err)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY when port is already loaned.
 	//-------------------------------------------------------------------------

	// Loan data port.
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	// Try to loan data port when the port is loaned by other client.
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrEtelPortAlreadyLoaned, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data, KErrNone);

	CleanupStack::PopAndDestroy(8, this); // call2, line2, phone2, telServer2, 
	                                      // call, line, data, this	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CLDP-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::LoanDataPort for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::LoanDataPort for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestLoanDataPort0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = OpenNewCall(line, call, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	// prepare data
	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );

	RCall::TCommPort expDataPort;
	expDataPort.iCsy.Copy(_L("qwerty"));
	expDataPort.iPort.Copy(_L("asdfgh"));
	TMockLtsyCallData1<RCall::TCommPort> expData(KCallId, KMobileService, expDataPort);
	expData.SerialiseL(data);

	RCall::TCommPort dataPort;
	dataPort.iCsy.Copy(expDataPort.iCsy);
	dataPort.iPort.Copy(expDataPort.iPort);

	TRequestStatus reqStatus;
	
	DialL(call, KCallId, KMobileService);

	//-------------------------------------------------------------------------
	// Test cancelling of RCall::LoanDataPort
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
	call.LoanDataPort(reqStatus, dataPort);
	
	call.LoanDataPortCancel();
	
	User::WaitForRequest(reqStatus);
	// cause cancel is not used in CTSY for this request
	// it completes with KErrNone
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data, KErrNone);

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CLDP-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::LoanDataPort with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::LoanDataPort with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestLoanDataPort0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	err = OpenNewCall(line, call, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
	DialL(call, KCallId, KMobileService);
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RCall::LoanDataPort
 	//-------------------------------------------------------------------------
 	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330101);
	// Test passing out of bounds parameters (KnullDesC as port) doesn't cause an error in ctsy.
	// Request pass to ltsy, so it returns CErrCorrupt. So request completes with KErrCorrupt instead of KErrNone.

	// Asynchronous version testing.
	RCall::TCommPort dataPort;
	dataPort.iCsy.Copy(KNullDesC);  // incorrect argument
	dataPort.iPort.Copy(KNullDesC); // incorrect argument
	TRequestStatus reqStatus;
	call.LoanDataPort(reqStatus, dataPort);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())
	AssertMockLtsyStatusL();

	// Synchronous version testing.
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrArgument, err)
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // call, line, this

	}


/**
@SYMTestCaseID BA-CTSY-COWN-CLDP-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::LoanDataPort for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::LoanDataPort for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestLoanDataPort00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330104);
	// RCall::LoanDataPort does not return KErrNotSupported for fax calls
	// Request pass to ltsy, so it returns KErrCorrupt instead of KErrNotSupported.
	
	TestLoanDataPortNotSupportedL(KMmTsyFaxLineName, 1);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CRDP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::RecoverDataPort for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::RecoverDataPort for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestRecoverDataPort0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestRecoverDataPortNotSupportedL(KMmTsyVoice1LineName);

	TestRecoverDataPortNotSupportedL(KMmTsyVoice2LineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CRDP-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::RecoverDataPort for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::RecoverDataPort for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestRecoverDataPort0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RCall call;
	RLine line;

	TInt err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	TName callName;
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
	DialL(call, KCallId, KMobileService);

	RCall::TCommPort dataPort;
	dataPort.iCsy.Copy(_L("qwerty"));
	dataPort.iPort.Copy(_L("asdfgh"));
	TMockLtsyCallData1<RCall::TCommPort> expData(KCallId, KMobileService, dataPort);
	expData.SerialiseL(data);
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data, KErrNotSupported);
	err = call.RecoverDataPort();
	ASSERT_EQUALS(KErrNotSupported, err)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::RecoverDataPort.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);
	err = call.RecoverDataPort();
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330201);
	// CMmDataCallTsy::RecoverDataPort does not checks error value
	// returned from ltsy and allways recovers data port
	// so CTSY recovers data port in precedent test step
	// and test for successful completion request gets returned
	// a KErrEtelPortNotLoanedToClient error instead of KErrNone.
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY when port is not loaned.
 	//-------------------------------------------------------------------------

	err = call.RecoverDataPort();
	ASSERT_EQUALS(KErrEtelPortNotLoanedToClient, err)
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when call is owned by other client
 	//-------------------------------------------------------------------------

	// loan data port by first client
	iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
	err = call.LoanDataPort(dataPort);
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	// Open second client
	RTelServer telServer2;
	err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	RCall call2;
	RLine line2;

	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line2);

	err = call2.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call2);

	// test
	err = call2.RecoverDataPort();
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330203);
	// Request passes to MockLTSY as though the client owns the call.
	// So MockLtsy returns KErrCorrupt because there is no expected IPC.
	ASSERT_EQUALS(KErrEtelNotCallOwner, err)
	AssertMockLtsyStatusL();
	
    iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);
	err = call.RecoverDataPort();
    ASSERT_EQUALS(KErrNone, err)

	CleanupStack::PopAndDestroy(8, this); // call2, line2, phone2, telServer2, call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CRDP-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::RecoverDataPort for data call when the call become idle
@SYMTestPriority High
@SYMTestActions Invokes RCall::RecoverDataPort for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestRecoverDataPort0007L()
    {

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);
    
    RCall call;
    RLine line;

    TInt err = line.Open(iPhone, KMmTsyDataLineName);
    ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(line);
    
    TName callName;
    err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
    ASSERT_EQUALS(KErrNone, err)
    CleanupClosePushL(call);

    const TInt KCallId( 1 );
    const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
    DialL(call, KCallId, KMobileService);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusConnected; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData2(KCallId, 
                                                    mobileService, mobileCallStatus);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    
    RCall::TCommPort dataPort;
    dataPort.iCsy.Copy(_L("qwerty"));
    dataPort.iPort.Copy(_L("asdfgh"));
    TMockLtsyCallData1<RCall::TCommPort> expData(KCallId, KMobileService, dataPort);
    expData.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
    err = call.LoanDataPort(dataPort);
    ASSERT_EQUALS(KErrNone, err)
    AssertMockLtsyStatusL();

    //-------------------------------------------------------------------------
    // Make the call idle
    //-------------------------------------------------------------------------
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    

    TInt hangUpCause = KErrGsmReleaseByUser;
    TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData1(KCallId, mobileService, hangUpCause, 
                                             autoStChangeDisable);
    data.Close();
    mockData1.SerialiseL(data);                                               
    iMockLTSY.ExpectL(EEtelCallHangUp, data, KErrNone);                                              
    
    TRequestStatus reqStatus;
    call.HangUp(reqStatus);
        
    mobileCallStatus = RMobileCall::EStatusIdle; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData3(KCallId, 
                                                    mobileService, mobileCallStatus);
    data.Close();
    mockData3.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, data);   

    data.Close();
    expData.SerialiseL(data);
	
	// Expect to RecoverDataPort from CTSY
    iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);
        
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrNone, reqStatus.Int());   
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(4, this); // call, line, data, this
    
    }


/**
@SYMTestCaseID BA-CTSY-COWN-CRDP-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::RecoverDataPort for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::RecoverDataPort for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestRecoverDataPort00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330202);
	// RCall::RecoverDataPort returns KErrEtelPortNotLoanedToClient
	// instead of KErrNotSupported for fax calls

	TestRecoverDataPortNotSupportedL(KMmTsyFaxLineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CAO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AcquireOwnership for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AcquireOwnership for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestAcquireOwnership0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	TestAcquireOwnershipNotSupportedL(KMmTsyVoice1LineName);

	TestAcquireOwnershipNotSupportedL(KMmTsyVoice2LineName);

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-COWN-CAO-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AcquireOwnership for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AcquireOwnership for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestAcquireOwnership0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	// make call having ownership
	RCall call;
	RLine line;

	err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	TName callName;
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY when call is not active.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	call.AcquireOwnership(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrEtelCallNotActive, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::AcquireOwnership.
 	//-------------------------------------------------------------------------

	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
	DialL(call, KCallId, KMobileService);

	// setting and execute request
	RCall call2;
	RLine line2;

	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line2);

	// open existing call to send take ownership request
	err = call2.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call2);

	call2.AcquireOwnership(reqStatus);

	// close 1st call to give ownership to call2
	call.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4); // call2, line2, call, line
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CAO-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::AcquireOwnership for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::AcquireOwnership for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestAcquireOwnership0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	// make call having ownership
	RCall call;
	RLine line;

	err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	TName callName;
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
	DialL(call, KCallId, KMobileService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::AcquireOwnership
 	//-------------------------------------------------------------------------
 	
	// setting and execute request
	RCall call2;
	RLine line2;

	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line2);

	// open existing call to send take ownership request
	err = call2.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call2);

	TRequestStatus reqStatus;
	call2.AcquireOwnership(reqStatus);

	call2.AcquireOwnershipCancel();

	// close 1st call to give ownership to call2
	call.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4); // call2, line2, call, line
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CAO-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::AcquireOwnership for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::AcquireOwnership for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestAcquireOwnership0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	// Open 3rd client
	RTelServer telServer3;
	err = telServer3.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer3);

	RMobilePhone phone3;
	err = phone3.Open(telServer3, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone3);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::AcquireOwnership
 	//-------------------------------------------------------------------------

	// make call having ownership
	RCall call;
	RLine line;

	err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	TName callName;
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
	DialL(call, KCallId, KMobileService);

	// setting and execute 1st request
	RCall call2;
	RLine line2;

	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line2);

	err = call2.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call2);

	TRequestStatus reqStatus;
	call2.AcquireOwnership(reqStatus);

	// setting and execute 2nd request
	RCall call3;
	RLine line3;

	err = line3.Open(phone3, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line3);

	err = call3.OpenExistingCall(line3, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call3);

	TRequestStatus reqStatus2;
	call3.AcquireOwnership(reqStatus2);

	// close 1st call to give ownership to call2
	call.Close();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())

	// close 2nd call to give ownership to call3
	call2.Close();

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6); // call3, line3, call2, line2, call, line
	CleanupStack::PopAndDestroy(5, this); // phone3, telServer3, phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-COWN-CAO-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::AcquireOwnership for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::AcquireOwnership for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestAcquireOwnership00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330301);
	// RCall::AcquireOwnership returns KErrEtelCallNotActive
	// instead of KErrNotSupported for fax calls

	TestAcquireOwnershipNotSupportedL(KMmTsyFaxLineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CTO-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::TransferOwnership for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::TransferOwnership for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestTransferOwnership0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TestTransferOwnershipNotSupportedL(KMmTsyVoice1LineName);

	TestTransferOwnershipNotSupportedL(KMmTsyVoice2LineName);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CTO-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::TransferOwnership for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::TransferOwnership for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestTransferOwnership0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
    CleanupClosePushL(expectData);

	RBuf8 completeData;
    CleanupClosePushL(completeData);    

	// Open second client
	RTelServer telServer2;
	TInt err = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	err = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	CleanupClosePushL(phone2);

	RCall call;
	RLine line;

	err = line.Open(iPhone, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line);
	
	TName callName;
	err = OpenNewCall(line, call, KMmTsyDataLineName, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY when call is not active
 	//-------------------------------------------------------------------------

	err = call.TransferOwnership();
	
	// CTSY grants call ownerhip to the ETel API client which for example
	// dials or answers incoming call. Call is currently in idle state so
	// nobody has ownership to this call object. Therefore CTSY returns error
	// KErrEtelNotCallOwner to TransferOwnership() request. 
	ASSERT_EQUALS(KErrEtelNotCallOwner, err)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY when there is no client
	// interested in the call ownership.
 	//-------------------------------------------------------------------------

	// make call having ownership
	const TInt KCallId( 1 );
	const RMobilePhone::TMobileService KMobileService( RMobilePhone::ECircuitDataService );
	DialL(call, KCallId, KMobileService);

	// make connected (just for coverage)
	ChangeCallStatusL(KCallId, KMobileService, RMobileCall::EStatusConnected);

	err = call.TransferOwnership();
	ASSERT_EQUALS(KErrEtelNoClientInterestedInThisCall, err)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::TransferOwnership.
 	//-------------------------------------------------------------------------

	// setting and execute AcquireOwnership request
	RCall call2;
	RLine line2;

	err = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(line2);

	// open existing call to send take ownership request
	err = call2.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, err)
	CleanupClosePushL(call2);

	TRequestStatus reqStatus;
	call2.AcquireOwnership(reqStatus);

	// TransferOwnership to call2
	err = call.TransferOwnership();
	ASSERT_EQUALS(KErrNone, err)
	AssertMockLtsyStatusL();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when call is owned by other client
 	//-------------------------------------------------------------------------

	err = call.TransferOwnership();
	ASSERT_EQUALS(KErrEtelNotCallOwner, err)
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// Hang up data call before closing objects. This is because closing of 
	// connected call object is bad client behaviour. RelinquishOwnership process 
	// does not go through if data call is in connected state and last client 
	// closes handle to the data call object.
	
    TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData1(KCallId, mobileService, hangUpCause, 
                                             autoStChangeDisable);
    mockData1.SerialiseL(expectData);                                              	
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone);                                              
	
	//Make HangUp request
	call2.HangUp(reqStatus);
		
	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle; 
	TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockData2(KCallId, 
	                                                mobileService, mobileCallStatus);
	mockData2.SerialiseL(completeData);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, completeData);	
		
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());	
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	//-------------------------------------------------------------------------

    AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(9, this); // call2, line2, call, line, phone2, 
	                                      // telServer2, completeData, expectData, this
	}


/**
@SYMTestCaseID BA-CTSY-COWN-CTO-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::TransferOwnership for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::TransferOwnership for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallOwnershipFU::TestTransferOwnership00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 330401);
	// RCall::TransferOwnership returns KErrEtelNotCallOwner
	// instead of KErrNotSupported for fax calls

	TestTransferOwnershipNotSupportedL(KMmTsyFaxLineName);

	CleanupStack::PopAndDestroy(this);
	
	}
