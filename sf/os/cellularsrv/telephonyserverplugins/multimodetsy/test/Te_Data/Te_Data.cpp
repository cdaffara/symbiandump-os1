// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed to test the Data Line abilities of MMTSY and ETel.
// 
//

/**
 @file
*/

#include "Te_Data.h"

_LIT(KDataLineName,"Data");
_LIT(KCsyName, "ECUART.CSY");
_LIT(KConsoleTitle,"Action Required");

const TInt KTenPartOfSec = 100000 ;
const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)
const TInt KTenSeconds=10000000;	// Used in a time out function, 10 seconds (in microSeconds)
const TInt KFifteenSeconds=15000000;	// 5 seconds (in microSeconds)

_LIT8(KWriteTestData,"MM  TSY Data and Echo Server Test is in progress...");


CTestLineStatusIncommingCall::CTestLineStatusIncommingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineStatusIncommingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestLineStatusIncommingCall::doTestStepL( void )
/**	 Tests line status function
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-004.00 Test retrieval of a data line's current status (Generic)
 * @test GT83-TDATA-004.02 "Test retrieval of a data line's current status when the line's status is ""Idle"""
 * @test GT83-TDATA-004.04 "Test retrieval of a data line's current status when the line's status is ""Ringing"""
 * @test GT83-TDATA-004.07 "Test retrieval of a data line's current status when the line's status is ""Connected"""
 * @test GT83-TDATA-004.08 "Test retrieval of a data line's current status when the line's status is ""Hanging up"""
 * @test GT83-TDATA-014.05 Test opening an existing call from a data line by specifying the name of the line and the name of an existing call
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 * @test GT83-TDATA-031.00 Test retrieval of a mobile data line's current status (Generic)
 * @test GT83-TDATA-031.01 "Test retrieval of a mobile data line's current status when there is only one call on the line and it's status is ""Idle"""
 * @test GT83-TDATA-031.04 "Test retrieval of a mobile data line's current status when there is only one call on the line and it's status is ""Answering"""
 * @test GT83-TDATA-031.07 "Test retrieval of a mobile data line's current status when there is only one call on the line and it's status is ""Waiting CCBS Recall"""
*/
	{
	INFO_PRINTF1(_L("======Start test Test Line Status"));
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;

	RCall::TStatus lineStatus;
	TBuf<16> GoodNumber ;
	GetRemoteNumber(GoodNumber) ;
	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone,  KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	TInt ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.00 - *** Failed RMobileLine::GetStatus() ***")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-031.00 - *** Failed RMobileLine::GetMobileLineStatus() ")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-031.01")) ;

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Please ring the phone\n"));

	TName callName;
	/* @test GT83-AT-024	Test setting notification of an incoming call on a data line */
	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
	WaitWithTimeoutL(iStatus,60*KOneSecond);		// but timeout after 60 second


	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0 - *** Failed RMobileLine::NotifyIncomingCall() ***")) ;
	if(iStatus.Int() == KRequestPending)
		{
		iLine.NotifyIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	/* @test GT83-AT-36	Test retrieval of the current call status when it is ringing*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-0 - *** Failed RMobileLine::GetStatus() *** ")) ;
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TDATA-0")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-0 - *** Failed RMobileLine::GetMobileLineStatus() *** ")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-008.001")) ;
	INFO_PRINTF1(_L("Phone is ringing..."));

	//Test for incomming data call
	TestLineCapabilitiesL(iLine);

	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	ret = iDataCall.OpenExistingCall(iLine, callName);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TDATA-014.05 - *** Failed RMobileCall::OpenExistingCall() ***")) ;

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);

	// DEF55659-NCh : timout changed from 30 to 60 s. Current version of MM.TSY is pretty slow, due
	// to the way it deal with the new comms repository. This timeout must be changed back to 30s
	// when comms repository access code is updated.
	WaitWithTimeoutL(iStatus,60*KOneSecond);		// but timeout after 60 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00 -  *** Failed RMobileCall::AnswerIncomingCall() ***")) ;
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.04 - *** Failed RMobileLine::GetStatus() ***")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.04")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-031.04 - *** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-031.04")) ;
	TestLineCapabilitiesL(iLine);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.07 - *** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-031.07 - *** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-031.07")) ;

	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Hanging Up"));
	iDataCall.HangUp(iStatus);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is hang up*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.08 - *** Failed RMobileLine::GetStatus() *** ")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-0")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.09 - *** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-004.09")) ;

	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;  //???  line 825

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	//Check that line status is now idle. If line is still connected, Fail check and leave to 
	//prevent problems down the line
	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02 - *** Failed RMobileLine::GetStatus() ***")) ;
	TEST_CHECKL(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02 - *** FAILED TO HANGUP CALL!! ***" )) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02 - *** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	TEST_CHECKL(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestLineStatusOutgoingCall::CTestLineStatusOutgoingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineStatusOutgoingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestLineStatusOutgoingCall::doTestStepL( void )
/**	 Tests line status function
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-004.00 Test retrieval of a data line's current status (Generic)
 * @test GT83-TDATA-004.02 "Test retrieval of a data line's current status when the line's status is ""Idle"""
 * @test GT83-TDATA-004.03 "Test retrieval of a data line's current status when the line's status is ""Dialling"""
 * @test GT83-TDATA-004.07 "Test retrieval of a data line's current status when the line's status is ""Connected"""
 * @test GT83-TDATA-004.08 "Test retrieval of a data line's current status when the line's status is ""Hanging up"""
 * @test GT83-TDATA-014.05 Test opening an existing call from a data line by specifying the name of the line and the name of an existing call
 * @test GT83-TDATA-031.00 Test retrieval of a mobile data line's current status (Generic)
 * @test GT83-TDATA-031.01 "Test retrieval of a mobile data line's current status when there is only one call on the line and it's status is ""Idle"""
*/
	{
	INFO_PRINTF1(_L("======Start test Test Line Status"));

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;

	TName callName;

	RCall::TStatus lineStatus;
	TBuf<16> GoodNumber ;
	GetRemoteNumber(GoodNumber) ;
	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	TInt ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.00 - *** Failed RMobileLine::GetStatus() ***")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-031.00 - *** Failed RMobileLine::GetMobileLineStatus() ")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-031.01")) ;

	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	ret = iDataCall.OpenNewCall(iLine, callName);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TDATA-014.05 - *** Failed RMobileCall::OpenNewCall() ***")) ;

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Calling...\n"));

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iDataCall.Dial(iStatus, callDataParamsPckg, GoodNumber);
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-0 - *** Failed RMobileLine::GetStatus() *** ")) ;
	//Commented out because phone does not report that it is going into this state
	//TEST_CHECKL(lineStatus, RCall::EStatusDialling, _L("GT83-TDATA-004.03")) ;
	WaitWithTimeoutL(iStatus,90*KOneSecond); // but timeout after 90 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;
	
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.DialCancel();
		User::WaitForRequest(iStatus);
		}
	
	//Test for outgoing data call
	TestLineCapabilitiesL(iLine);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.07 - *** Failed RMobileLine::GetStatus() *** ")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-0 - *** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-0")) ;

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Call connected - listen for 20 seconds..."));

	User::After(20*KOneSecond); // hold call active for 20 seconds

	INFO_PRINTF1(_L("Hanging up"));
	iDataCall.HangUp(iStatus);

	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.08 - *** Failed RMobileLine::GetStatus() ***  ")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-004.08")) ;

	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;

	//This test has some troubles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	//Test line capabilities
	TestLineCapabilitiesL(iLine);

	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	TEST_CHECKL(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02 - *** Phone Failed to hang up ***")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-031.01 -  *** Failed RMobileLine::GetMobileLineStatus() *** ")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-031.01")) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestLineInfo::CTestLineInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineInfo"));
	}
//------------------------------------------------------------------

TVerdict CTestLineInfo::doTestStepL( void )
/**
 * This function gets the Line capabilities, line information, Number of calls
 * on the line and call information,
 * @test NONE	Test the ability to get the current line information of the data line (Generic)
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-005.01 "Test retrieval of the current hook status, when the line is currently ""off hook"""
 * @test GT83-TDATA-005.02 "Test retrieval of the current hook status, when the line is currently ""on hook"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 * @test GT83-TDATA-026.02 Test the ability to place an asynchronous request to hang up a data call
 */
	{
	INFO_PRINTF1(_L("======Start test Test Line Information"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01 - *** Failed RMobileLine::Open() ***"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06 - *** Failed RMobileCall::OpenNewCall() ***"));

	// Line Information
	RLine::TLineInfo lineInfo;
	TRAPD(ret, ret=iLine.GetInfo(lineInfo));
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetInfo ***"));
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Get Line Info. is NOT supported"));
		}
	else
		{
		INFO_PRINTF1(_L("Get Line Info is successful"));
		CHECKPOINT(lineInfo.iHookStatus,  RCall::EHookStatusOn, _L("GT83-TDATA-0")) ;
		TEST(lineInfo.iStatus == RCall::EStatusIdle) ;
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Name of last call added: %S")), &lineInfo.iNameOfLastCallAdded);
		}

	// Hook status information
	INFO_PRINTF1(_L("\nRetrieving Hook Status..."));
	RCall::THookStatus hookStatus;
	ret=iLine.GetHookStatus(hookStatus);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Get Hook status is NOT supported"));
		}
	else
		{
		CHECKPOINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TDATA-005.02")) ;
		INFO_PRINTF1(_L("The line is currently ON Hook"));
		}

	// Post 'Answer a Call' Notifications
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Please ring the phone\n"));

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);

	WaitWithTimeoutL(iStatus,6*KTenSeconds);		// but timeout after 60 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00 - *** Failed RMobileCall::AnswerIncomingCall() ***")) ;

	// Count the no. of calls on the line & Get call info.
	INFO_PRINTF1(_L("Retrieving no. of calls opened from the line.."));
	TInt lineCount=0;
	TEST(KErrNone == iLine.EnumerateCall(lineCount));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("No of calls opened = %d")), lineCount);

	INFO_PRINTF1(_L("Getting Call info.."));
	RLine::TCallInfo callInfo;
	ret=iLine.GetCallInfo(0,callInfo);
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call information is NOT supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Call information is successful"));
		}

	// Get the line's current hook status
	ret=iLine.GetHookStatus(hookStatus);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Get Hook status is NOT supported"));
		}
	else
		{
		TEST_CHECKL(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-005.01")) ;
		INFO_PRINTF1(_L("The line is currently OFF Hook"));
		}

	User::After(KTenSeconds);

	INFO_PRINTF1(_L("Hanging up"));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestLineNotificationsIncommingCall::CTestLineNotificationsIncommingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineNotificationsIncommingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestLineNotificationsIncommingCall::doTestStepL( void )
/** Tests line notifications
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-008.01 Test setting notification of an incoming call on a data line
 * @test GT83-TDATA-008.02 Test for a notification when there is an incoming call on a data line
 * @test GT83-TDATA-009.01 Test setting notification of a change in hook status of a data line
 * @test GT83-TDATA-009.02 "Test notification of a change in hook status of a data line from ""off"" to ""on"""
 * @test GT83-TDATA-009.03 "Test notification of a change in hook status of a data line from ""on"" to off"""
 * @test GT83-TDATA-010.01 Test setting notification of a change in the status of the data line
 * @test GT83-TDATA-010.02 "Test notification of a change in the status of the data line from ""idle"" to ""ringing"" to ""answering"" to ""connected"" to ""hanging up"" to ""idle"""
 * @test GT83-TDATA-011.01 Test setting notification of a call being added to a data line
 * @test GT83-TDATA-014.05 Test opening an existing call from a data line by specifying the name of the line and the name of an existing call
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 * @test GT83-TDATA-032.01 Test setting a request for a notification of a change in the status of the mobile data line
 * @test GT83-TDATA-032.07 "Test for a notification of a change in status of the mobile data line from ""connected"" to ""disconnecting"""
 * @test GT83-TDATA-032.08 "Test for a notification of a change in status of the mobile data line from ""disconnecting"" to ""idle"""
 * @test GT83-TDATA-032.09 "Test for a notification of a change in status of the mobile data line from ""idle"" to ""ringing"""
 * @test GT83-TDATA-032.10 "Test for a notification of a change in status of the mobile data line from ""ringing"" to ""answering"""
 * @test GT83-TDATA-032.11 "Test for a notification of a change in status of the mobile data line from ""answering"" to ""connected"""
*/
	{
	INFO_PRINTF1(_L("======Start test Test Line Notifications"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; 

	TBuf<16> GoodNumber ;
	GetRemoteNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TName callName1;
	TName callName2;
	TInt ret = KErrNone;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Please ring the phone\n"));

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call

	// @test GT83-AT-026	Test setting notification of a change in the status of the data line
	iLine.NotifyStatusChange(iStatus2, lineStatus);

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	// @test GT83-AT-037	Test setting notification of a change in hook status of a data line	void RLine::NotifyHookChange(TRequestStatus& aStatus, THookStatus& aHookStatus)	Function does not produce any unexpected errors. aStatus = KErrPending
	iLine.NotifyHookChange(iStatus4, hookStatus);

    /* @test GT83-AT-025	Test notification of a call being added to a data line 	*/
	iLine.NotifyCallAdded(iStatus5, callName2);

	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-008.01")) ;
	CHECKPOINT(iStatus2.Int(), KRequestPending, _L("GT83-TDATA-010.02")) ;
	CHECKPOINT(iStatus3.Int(), KRequestPending, _L("GT83-TDATA-032.01")) ;
	CHECKPOINT(iStatus4.Int(), KRequestPending, _L("GT83-TDATA-011.01")) ;
	CHECKPOINT(iStatus5.Int(), KRequestPending, _L("GT83-TDATA-009.01")) ;

	WaitWithTimeoutL(iStatus,90*KOneSecond); // but timeout after 90 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-008.02")) ;

	if(iStatus.Int() == KRequestPending)
		{
		iLine.NotifyIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "idle" to "ringing"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TDATA-010.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.09")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-032.09")) ;

	/* @test GT83-AT-025	Test notification of a call being added to a data line
	that currently has no calls*/
	User::WaitForRequest(iStatus5);
	CHECKPOINT(iStatus5.Int(), KErrNone, _L("GT83-TDATA-011.01")) ;
	CHECKPOINT(callName2 == callName1, 1,  _L("GT83-TDATA-011")) ;

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));

	/* @test GT83-AT-026	Test notification of a change in the status of the data line from
	 "idle" to "ringing" to "answering" to "connected" to "hanging up" to "idle"
	 */
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	ret = iDataCall.OpenExistingCall(iLine, callName1);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TDATA-014.05")) ;

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);
    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "ringing" to "answering"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.01")) ;
	CHECKPOINT(lineStatus, RCall::EStatusAnswering, _L("GT83-TDATA-010.01")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.10")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusAnswering, _L("GT83-TDATA-032.10")) ;
	WaitWithTimeoutL(iStatus,30*KOneSecond);		// but timeout after 30 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;	  //wrong code -2147483647


    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "answering"	to "connected"
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.01")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-010.01")) ;


	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.11")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-032.11")) ;

	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.02")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-009.02")) ;

	// now hangup call
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "connecting" to "hanging up"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-010.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.07")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-032.07")) ;

	// @test GT83-AT-026	Test notification of a change in the status of the data
	// line from "hanging up" to "idle"
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-010.02")) ;

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.08")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-032.08")) ;

	//!< @test GT83-AT-037	Test notification of a change in hook status of a data line
	//!<  from "off" to "on" aHookStatus = EHookStatusOff before the hook status changes,
	//!< and EHookStatusOn after the hook status changes
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.03")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TDATA-009.03")) ;
	//check that all request has been finished
	TEST(iStatus.Int()!= KRequestPending) ;
	TEST(iStatus2.Int() != KRequestPending) ;
	TEST(iStatus3.Int() != KRequestPending) ;
	TEST(iStatus4.Int() != KRequestPending) ;
	TEST(iStatus5.Int() != KRequestPending) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestLineNotificationsOutgoingCall::CTestLineNotificationsOutgoingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineNotificationsOutgoingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestLineNotificationsOutgoingCall::doTestStepL( void )
/** Tests line notifications
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-009.02 "Test notification of a change in hook status of a data line from ""off"" to ""on"""
 * @test GT83-TDATA-009.03 "Test notification of a change in hook status of a data line from ""on"" to off"""
 * @test GT83-TDATA-010.03 "Test notification of a change in the status of the data line from ""idle"" to ""dialling"" to ""connecting"" to ""connected"" to ""hanging up"" to ""idle"""
 * @test GT83-TDATA-014.05 Test opening an existing call from a data line by specifying the name of the line and the name of an existing call
 * @test GT83-TDATA-032.02 "Test for a notification of a change in status of the mobile data line from ""idle"" to ""dialling"""
 * @test GT83-TDATA-032.04 "Test for a notification of a change in status of the mobile data line from ""connecting"" to ""connected"""
 * @test GT83-TDATA-032.07 "Test for a notification of a change in status of the mobile data line from ""connected"" to ""disconnecting"""
 * @test GT83-TDATA-032.08 "Test for a notification of a change in status of the mobile data line from ""disconnecting"" to ""idle"""
*/
	{
	INFO_PRINTF1(_L("======Start test Test Line Notifications"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; 

	TBuf<16> GoodNumber ;
	GetRemoteNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TInt ret = KErrNone;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	ret = iDataCall.OpenNewCall(iLine);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TDATA-014.05")) ;
	// Now test call status during an outgoing call
	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Calling...\n"));

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iDataCall.Dial(iStatus, callDataParamsPckg, GoodNumber);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusDialling, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.02")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDialling, _L("GT83-TDATA-032.02")) ;

	User::WaitForRequest(iStatus); //whaiting for dial
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;

	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.03")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-009.03")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.13")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-032.13")) ;

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	INFO_PRINTF1(_L("Call connected - hanging up..."));

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	//HANG UP
	User::After(10*KOneSecond);
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.07")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-032.07")) ;

	/* @test GT83-AT-037	Test notification of a change in hook status of a data
	 line from "on" to off"	aHookStatus = EHookStatusOn before the hook status changes,
	 */
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.02")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TDATA-009.02")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.08")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-032.08")) ;

	TEST(iStatus != KRequestPending) ;
	TEST(iStatus2 != KRequestPending) ;
	TEST(iStatus3 != KRequestPending) ;
	TEST(iStatus4 != KRequestPending) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestLineCancels::CTestLineCancels()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineCancels"));
	}
//------------------------------------------------------------------

TVerdict CTestLineCancels::doTestStepL( void )
/**
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-008.02 Test for a notification when there is an incoming call on a data line
 * @test GT83-TDATA-008.03 Test the ability to cancel the notification of an incoming call on a data line
 * @test GT83-TDATA-009.01 Test setting notification of a change in hook status of a data line
 * @test GT83-TDATA-009.04 Test the ability to cancel a notification of a change in hook status of a data line
 * @test GT83-TDATA-010.02 "Test notification of a change in the status of the data line from ""idle"" to ""ringing"" to ""answering"" to ""connected"" to ""hanging up"" to ""idle"""
 * @test GT83-TDATA-010.04 Test the ability to cancel the notification of a change in the status of the data line
 * @test GT83-TDATA-011.01 Test setting notification of a call being added to a data line
 * @test GT83-TDATA-011.03 Test the ability to cancel a notification of a call being added to a data line
*/
	{
	INFO_PRINTF1(_L("======Start test Test Line Cancels"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;

	// Notify Incoming Call
	TName callName;
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	iLine.NotifyIncomingCall(iStatus, callName);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-008.02")) ;
	iLine.NotifyIncomingCallCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-008.03")) ;

	// Notify Status Change
	RCall::TStatus lineStatus;
	iLine.NotifyStatusChange(iStatus, lineStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-010.02")) ;
	User::After(KTenPartOfSec); //snack some sleep
	/** @test GT83-AT-026 Test the ability to cancel the notification of a change in the
	 status of the data line	aStatus = KErrCancel provided that the function
	 being cancelled has not already completed */
	iLine.NotifyStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-010.04")) ;
#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	// Notify Mobile Status Change
	RMobileCall::TMobileCallStatus lineStatus2;
	iLine.NotifyMobileLineStatusChange(iStatus, lineStatus2);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-0")) ;
	iLine.NotifyMobileLineStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-0")) ;
#endif
	// Notify Hook Change
	RCall::THookStatus hookStatus;
	iLine.NotifyHookChange(iStatus, hookStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-009.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	// @test GT83-AT-037	Test the ability to cancel a notification of a
	// change in hook status of a data line
	// aStatus = KErrCancel provided that the function being cancelled has not already completed
	iLine.NotifyHookChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-009.04")) ;

	// Notify Call Added
	iLine.NotifyCallAdded(iStatus, callName);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-011.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	/** GT83-AT-025	Test the ability to cancel a notification of a call being added
	to a data line*/
	iLine.NotifyCallAddedCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-011.03")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestCallInfo::CTestCallInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallInfo"));
	}
//------------------------------------------------------------------

TVerdict CTestCallInfo::doTestStepL( void )
/**
 * This function gets information pertaining to a call:
 * Bearer Service Information, Call duration
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-009.02 "Test notification of a change in hook status of a data line from ""off"" to ""on"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-018.01 "Test retrieval of the bearer service info of a call. Display to screen, all the bearer service information associated with the call."
 * @test GT83-TDATA-022.01 Test retrieval of the duration of a data call. Display the call duration to screen
 * @test GT83-TDATA-023.01 Test the ability to get the current call information of the data call. Display the call information to screen.
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 */
	{
	INFO_PRINTF1(_L("======Start test Test Call Information"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;
	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));
	//get status
	//GT83-AT-026	Test retrieval of a data line's current status.
	RCall::TStatus callStatus;
	CHECKPOINT(iDataCall.GetStatus(callStatus), KErrNone, _L("GT83-TD-015.02")) ;
	CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TD-015.02")) ;

	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;


	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Please ring the phone\n"));

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);
	WaitWithTimeoutL(iStatus,60*KOneSecond);		// but timeout after 60 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}
	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;

	// User Information
	INFO_PRINTF1(_L("\nThe call is now connected.\nRetrieving Call Information."));

	// Bearer Service Information
	INFO_PRINTF1(_L("\nGet Bearer Service Info..."));
	RCall::TBearerService bearerServiceInfo;
	TInt ret=iDataCall.GetBearerServiceInfo(bearerServiceInfo);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("GT83-TDATA-018.01")) ;
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get BearerService Information is NOT Supported"));
	if (ret==KErrNone)
		INFO_PRINTF1(_L("Get BearerService Information is successful"));

	// Get Call Information
	INFO_PRINTF1(_L("\nGet Current Call Info..."));
	ret=iDataCall.GetInfo(iCallInfo);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("GT83-TDATA-023.01")) ;
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Current Call info is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Displaying Information about the Current Call"));
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call name: %S")), &iCallInfo.iCallName);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Line name: %S")), &iCallInfo.iLineName);
		if (iCallInfo.iHookStatus==RCall::EHookStatusOff)
			{
			INFO_PRINTF1(_L("Current Hook Status: Off"));
			}
		if (iCallInfo.iStatus==RCall::EStatusConnected)
			{
			INFO_PRINTF1(_L("Current Call Status: Connected"));
			}
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call Duration: %d seconds")), iCallInfo.iDuration.Int());
		}

	// Hang up the Call
	User::After(5*KOneSecond);

	// Call Duration
	INFO_PRINTF1(_L("Get Call Duration..."));
	TTimeIntervalSeconds callDuration2;
	ret=iDataCall.GetCallDuration(callDuration2);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("GT83-TDATA-022.01")) ;
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Current Call Duration is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), callDuration2.Int());
		}

	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestCallNotifications::CTestCallNotifications()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallNotifications"));
	}
//------------------------------------------------------------------

TVerdict CTestCallNotifications::doTestStepL( void )
/**
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-002.01 Test retrieval of a data line's capabilities. Display the capabilities to screen
 * @test GT83-TDATA-008.01 Test setting notification of an incoming call on a data line
 * @test GT83-TDATA-009.02 "Test notification of a change in hook status of a data line from ""off"" to ""on"""
 * @test GT83-TDATA-009.03 "Test notification of a change in hook status of a data line from ""on"" to off"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-014.05 Test opening an existing call from a data line by specifying the name of the line and the name of an existing call
 * @test GT83-TDATA-015.04 "Test retrieval of the current call status when the call's status is ""ringing"""
 * @test GT83-TDATA-015.05 "Test retrieval of the current call status when the call's status is ""answering"""
 * @test GT83-TDATA-017.02 Test notification of a change in the call's capabilities. Print the call's capabilities to screen
 * @test GT83-TDATA-020.02 "Test notification of a change in the status of the data call from ""idle"" to ""ringing"" to ""answering"" to ""connected"" to ""hanging up"" to ""idle"""
 * @test GT83-TDATA-021.01 Test setting notification of a change in the data call's duration
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 * @test GT83-TDATA-034.01 Test setting notification of a change in the call's dynamic call control and call event capabilities
 * @test GT83-TDATA-034.02 Test notification of a change in the call's dynamic call control and call event capabilities. Print the call's capabilities to screen
 * @test GT83-TDATA-035.04 "Test retrieval of the current status of the data call when the call's status is ""ringing"""
 * @test GT83-TDATA-036.07 "Test for a notification of a change in status of the mobile data call from ""connected"" to ""disconnecting"""
 * @test GT83-TDATA-036.08 "Test for a notification of a change in status of the mobile data call from ""disconnecting"" to ""idle"""
 * @test GT83-TDATA-036.10 "Test for a notification of a change in status of the mobile data call from ""ringing"" to ""answering"""
 * @test GT83-TDATA-036.11 "Test for a notification of a change in status of the mobile data call from ""answering"" to ""connected"""
*/

	{
	// Post call status change notifications
	INFO_PRINTF1(_L("======Start test Test Call Notifications"));
	TName callName1;
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	RCall::TCaps caps;
	RCall::THookStatus hookStatus;
	TTimeIntervalSeconds time;

	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call

	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-008.01")) ;
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Please ring the phone\n"));

	WaitWithTimeoutL(iStatus,90*KOneSecond); // but timeout after 90 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-008.01")) ;
	if(iStatus.Int() == KRequestPending)
		{
		iLine.NotifyIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}


	TInt ret = iDataCall.OpenExistingCall(iLine, callName1);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TDATA-014.05")) ;

	iDataCall.GetStatus(callStatus);
	CHECKPOINT(callStatus, RCall::EStatusRinging, _L("GT83-TDATA-015.04")) ;
	iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-035.04")) ;

	ret=iDataCall.GetCaps(caps);
	Print_TCapsEnum(caps) ;
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-002.01")) ;
	TEST(caps.iFlags==(RCall::KCapsData | RCall::KCapsAnswer));

//	INFO_PRINTF1(_L("Opening New Data Call"));
//	TEST_CHECKL(iiDataCall.OpenExistingCall(theLine, callName2), KErrNone, _L("GT83-TDATA-013.06")) ;

	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	iDataCall.NotifyCapsChange(iStatus4, caps);
	iDataCall.NotifyHookChange(iStatus5, hookStatus);
	iDataCall.NotifyCallDurationChange(iStatus6, time) ;
	iDataCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);

	iDataCall.GetStatus(callStatus);
	CHECKPOINT(callStatus, RCall::EStatusRinging, _L("GT83-TDATA-015.05")) ;

	WaitWithTimeoutL(iStatus,60*KOneSecond);		// but timeout after 60 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;


	//iDataCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECKPOINT(callStatus, RCall::EStatusAnswering, _L("GT83-TDATA-020.10")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.10")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TDATA-036.10")) ;

	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TDATA-020.02")) ;

	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.11")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-036.11")) ;

	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-017.02")) ;
	Print_TCapsEnum(caps) ;
	CHECKPOINT(caps.iFlags, RCall::KCapsData, _L("GT83-TDATA-017.02")) ;

	iDataCall.NotifyCapsChange(iStatus4, caps);
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-017.02")) ;
	//TEST_CHECKL(caps.iFlags, (RCall::KCapsData | RCall::KCapsHangUp), _L("GT83-TDATA-017.02")) ;
	//MKV error

	User::WaitForRequest(iStatus5);
	CHECKPOINT(iStatus5.Int(), KErrNone, _L("GT83-TDATA-009.03")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-009.03")) ;

	User::WaitForRequest(iStatus6);
	CHECKPOINT_EXPR((iStatus6.Int()==KErrNone || iStatus6.Int()==KErrNotSupported), _L("GT83-TDATA-021.01")) ;
	if (iStatus6==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Notify Call Duration Change is NOT Supported"));
		}
	if (iStatus6==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), time.Int());
		}

	User::WaitForRequest(iStatus7);
	CHECKPOINT(iStatus7.Int(), KErrNone, _L("GT83-TDATA-034.01")) ;
	CHECKPOINT(mobileCallCaps.iCallControlCaps, RCall::KCapsData, _L("GT83-TDATA-034.01")) ;
	iDataCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);
	User::WaitForRequest(iStatus7);
	CHECKPOINT(iStatus7.Int(), KErrNone, _L("GT83-TDATA-034.02")) ;
	//TEST_CHECKL(mobileCallCaps.iCallControlCaps, (RCall::KCapsData | RCall::KCapsHangUp), _L("GT83-TDATA-034.02")) ;
	//MKV error

	// now hangup call
	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;	//sometimes timed out -33

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECKPOINT(callStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-020.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.07")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-036.07")) ;

	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-020.02")) ;

	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.08")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-036.08")) ;

	TEST(iStatus != KRequestPending) ;
	TEST(iStatus2 != KRequestPending) ;
	TEST(iStatus3 != KRequestPending) ;
	TEST(iStatus4 != KRequestPending) ;
	TEST(iStatus5 != KRequestPending) ;
	TEST(iStatus6 != KRequestPending) ;

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestCallStatusIncommingCall::CTestCallStatusIncommingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallStatusIncommingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestCallStatusIncommingCall::doTestStepL( void )
/**
 * This function tests call status at various times during calls
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-009.01 Test setting notification of a change in hook status of a data line
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-023.01 Test the ability to get the current call information of the data call. Display the call information to screen.
 * @test GT83-TDATA-035.02 "Test retrieval of the current status of the data call when the call's status is ""idle"""
 * @test GT83-TDATA-035.04 "Test retrieval of the current status of the data call when the call's status is ""ringing"""
 * @test GT83-TDATA-035.05 "Test retrieval of the current status of the data call when the call's status is ""answering"""
 * @test GT83-TDATA-035.07 "Test retrieval of the current status of the data call when the call's status is ""connected"""
 */
	{
	// Get the Call Status
	INFO_PRINTF1(_L("======Start test Get Call Status"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;

	// Open a Data line and Call
	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	TBuf<16> GoodNumber ;
	GetRemoteNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));


	TInt ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	TInt ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT_EXPR((ret2==KErrNone || ret2==KErrNotSupported), _L("GT83-TDATA-035.02")) ;
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call Status is NOT Supported"));
		}

	if (ret2==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Mobile Call Status is NOT Supported"));
		}

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an incomming call

	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Please ring the phone\n"));

	/* @test GT83-AT-024	Test setting notification of an incoming call on a data line */
	//commented out because connecting process too long and modem breaks connection
	// by temeout
//	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
//	WaitWithTimeoutL(iStatus,90*KOneSecond); // but timeout after 90 seconds

	ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.04")) ;
//	TEST_CHECKL(callStatus, RCall::EStatusRinging, _L("GT83-TDATA-004.04.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TDATA-035.07")) ;
//	TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-035.04")) ;

//	INFO_PRINTF1(_L("Phone is ringing..."));

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);
	ret=iDataCall.GetStatus(callStatus);
	//TEST_CHECKL(callStatus, RCall::EStatusAnswering, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	//TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TDATA-035.05")) ;
	WaitWithTimeoutL(iStatus,90*KOneSecond); // but timeout after 90 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.07")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TDATA-035.07")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		Print_RCall_TStatus(callStatus) ;
		CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.07")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-035.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}

	INFO_PRINTF1(_L("Hanging up..."));

	User::After(10*KOneSecond);
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TDATA-035.02")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestCallStatusOutgoingCall::CTestCallStatusOutgoingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallStatusOutgoingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestCallStatusOutgoingCall::doTestStepL( void )
/**
 * This function tests call status at various times during calls
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-009.01 Test setting notification of a change in hook status of a data line
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-023.01 Test the ability to get the current call information of the data call. Display the call information to screen.
 * @test GT83-TDATA-024.00 Test dialling a data call (Generic)
 * @test GT83-TDATA-035.02 "Test retrieval of the current status of the data call when the call's status is ""idle"""
 * @test GT83-TDATA-035.07 "Test retrieval of the current status of the data call when the call's status is ""connected"""
 */
	{
	// Get the Call Status
	INFO_PRINTF1(_L("======Start test Get Call Status..."));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;
	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	TBuf<16> GoodNumber ;
	GetRemoteNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));


	TInt ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("GT83-TDATA-004.02")) ;
	TInt ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT_EXPR((ret2==KErrNone || ret==KErrNotSupported), _L("GT83-TDATA-035.02")) ;

	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call Status is NOT Supported"));
		}

	if (ret2==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Mobile Call Status is NOT Supported"));
		}

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an outgoing call
	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Calling...\n"));

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iDataCall.Dial(iStatus, callDataParamsPckg, GoodNumber);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-024.00")) ;

	ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.07")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TDATA-035.07")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.07")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-035.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}

	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));

	INFO_PRINTF1(_L("Hanging up..."));

	User::After(10*KOneSecond);
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	ret=iDataCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TDATA-035.02")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	console->Printf(_L("...Disconnected\n"));
	User::After(10*KOneSecond);
    CleanupStack::PopAndDestroy(console);

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestCallCancels::CTestCallCancels()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallCancels"));
	}
//------------------------------------------------------------------

TVerdict CTestCallCancels::doTestStepL( void )
/**
 * This function posts and then cancels Call Notifications.
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-017.01 Test setting notification of a change in the call's capabilities
 * @test GT83-TDATA-017.03 Test the ability to cancel a notification of a change in the call's capabilities
 * @test GT83-TDATA-019.01 Test setting notification of a change in hook status of a data call
 * @test GT83-TDATA-019.04 Test the ability to cancel a notification of a change in hook status of a data call
 * @test GT83-TDATA-020.02 "Test notification of a change in the status of the data call from ""idle"" to ""ringing"" to ""answering"" to ""connected"" to ""hanging up"" to ""idle"""
 * @test GT83-TDATA-021.01 Test setting notification of a change in the data call's duration
 * @test GT83-TDATA-021.03 Test cancelling notification of a change in the data call's duration
 */
	{
	INFO_PRINTF1(_L("======Start test Test Call Cancels"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; 

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));

	// Call Status Change Cancel Request
	RCall::TStatus callStatus;
	iDataCall.NotifyStatusChange(iStatus, callStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-020.02")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-020.4")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	RMobileCall::TMobileCallStatus mobileCallStatus;
	iiDataCall.NotifyMobileCallStatusChange(iStatus, mobileCallStatus);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TDATA-0")) ;
	iiDataCall.NotifyMobileCallStatusChangeCancel();
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrCancel, _L("GT83-TDATA-0")) ;
#endif
	// Hook Change Cancel Request
	RCall::THookStatus hookStatus;
	iDataCall.NotifyHookChange(iStatus, hookStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-019.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyHookChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-019.04")) ;

	// Test Cancelling Notifications
	RCall::TCaps callCaps;
	iDataCall.NotifyCapsChange(iStatus, callCaps);
	TEST_CHECKL(iStatus.Int(), KRequestPending, _L("GT83-TDATA-017.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-017.03")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);
	iDataCall.NotifyMobileCallCapsChange(iStatus, mobileCallCapsPckg);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-0")) ;
	iDataCall.NotifyMobileCallCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-0")) ;
#endif

	TTimeIntervalSeconds callDuration;
	iDataCall.NotifyCallDurationChange(iStatus, callDuration);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-021.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyCallDurationChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-021.03")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestHandShake::CTestHandShake()
/** Each test step initialises it's own name
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	console->Printf(_L("Start the echo batch file then press any key to continue.\n"));
	console->Getch();
	console->Printf(_L("Te_echo.script is running.\n"));
	CleanupStack::PopAndDestroy(console);
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestHandShake"));
	}
//------------------------------------------------------------------
TInt CTestHandShake::DialRemoteModemTestL()
/**
 * Dials a data call.
 * This method opens a data line and call, and retrieves the status of the call.
 * It then dials a remote modem (initiated by t_data.exe), loans the Comm port,
 * connects to the Comms Server, opens a serial port and proceeds to transmit data to
 * the remote modem. It subsequently reads the data echoed back by the remote modem and
 * compares the transmitted and received data. The serial port, the connection to the
 * Comms Server, the data port, call and line are then closed.
 * @return TBool indicating whether this test was successful. If it's unsuccessful, the
 * test exits because it will not be possible to do the AnswerTest as the telephone no.
 * of the phone was not transferred due to the failure of the DialTest.
 *
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-004.02 "Test retrieval of a data line's current status when the line's status is ""Idle"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-024.00 Test dialling a data call (Generic)
 * @test GT83-TDATA-026.02 Test the ability to place an asynchronous request to hang up a data call
 * @test GT83-TDATA-028.02 Test the ability to recover a loaned port that was requested by an asynchronous request and the request completed
 */
	{

	INFO_PRINTF1(_L("======Start test Dial Remote Modem test"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;
	const TInt KNumOfBytes=51;		// Number of bytes of data to send to remote end

	// Open a data line and call
	INFO_PRINTF1(_L("Opening Data Line"));
	// GT83-AT-021	Test opening a data line from a phone that supports data functionality
	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));

	//GT83-AT-028	Test opening a new call from data line (Generic)
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));

	// Get the call's status
	RCall::TStatus callStatus;
	CHECKPOINT(iDataCall.GetStatus(callStatus), KErrNone, _L("GT83-TDATA-004.02"));
	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;
	TestGetCallCapabilitiesL(iDataCall) ;

	// Dial remote modem and obtain the call's current status
	TBuf<16> remoteNum;
	GetRemoteNumber(remoteNum);	

	INFO_PRINTF2(_L("Dialling %S...."), &remoteNum);

	// new stuff
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	INFO_PRINTF2(_L("Dialling %S...."), &remoteNum);
	iDataCall.Dial(iStatus, callDataParamsPckg, remoteNum);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TDATA-024.00"));

	CHECKPOINT(iDataCall.GetStatus(callStatus), KErrNone, _L("GT83-TDATA-024.00"));
	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;

	// Loan the Comm port
	TInt retValue=KErrNone;	// return value for the test
	RCall::TCommPort commPort;
	iDataCall.LoanDataPort(iStatus,commPort);
	User::WaitForRequest(iStatus);
	if (iStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Comm port loaned successfully"));
		}
	else if (iStatus==KErrCommsLineFail)
		{
		INFO_PRINTF1(_L("Loan Data port error: 'KErrCommsLineFail'."));
		INFO_PRINTF1(_L("This error is currently known to only happen with the Motorola"));
		INFO_PRINTF1(_L("Time port. Please see the PhoneInfo.cpp file for more information."));
		retValue=iStatus.Int();
		iDataCall.Close();
		iLine.Close();
		return retValue;
		}
	else
		{
		INFO_PRINTF2(_L("Error %d occured during"), iStatus.Int());
		INFO_PRINTF1(_L("Loan comm port request.\nUnable to continue."));
		retValue=iStatus.Int();
		iDataCall.Close();
		iLine.Close();
		return retValue;
		}

	// Connect to the Comms Server and load the CSY
	RCommServ cs;
	TEST_CHECKL(cs.Connect(), KErrNone, _L("GT83-TDATA-0"));
	TInt ret=cs.LoadCommModule(KCsyName);
	TESTL(ret==KErrNone || ret==KErrAlreadyExists);

	// Open a serial port
	RComm port;
	TEST_CHECKL(port.Open(cs,commPort.iPort,ECommShared), KErrNone, _L("*** Failed to open serial port ***"));
	INFO_PRINTF1(_L("Serial port opened successfully"));

	// Write own telephone number to the Data Port.
	TBuf<16> localNumUnicode;
	TBuf8<16> localNum;		// We need this to be large 

	GetLocalNumber(localNumUnicode);		// Get out own phone number in unicode
	localNum.Copy(localNumUnicode);		// Convert unicode to 8 bit string

	// The T_ECHO client we are talking to expects the localNum to be 12 chars.
	// Therefore we have to ensure our localNum is exactly 12 chars by padding with spaces.
	const TInt len=localNum.Length();
	if(len<12)
		localNum.AppendFill(TChar(' '),12-len);	
	localNum.SetLength(12);
	
	port.Write(iStatus,localNum);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to write to comm port ***"));
	INFO_PRINTF1(_L("Data stream 1 (tel. no.) written successfully"));

	// Write more data
	port.Write(iStatus, KWriteTestData);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed to write to comm port***")) ;
	INFO_PRINTF1(_L("Data stream 2 written successfully"));

	// Receive the echoed data from the remote machine
	TInt success=EFalse;
	TBuf8<KNumOfBytes> echoedData;
	port.Read(iStatus,KFifteenSeconds,echoedData);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus, KErrNone, _L("*** Failed to read echoed data!! ***"));

	if (iStatus==KErrTimedOut)
		{
		INFO_PRINTF1(_L("A Time out error occured during the read"));
		}
	else if (iStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Data stream read successfully"));
		success=ETrue;
		}
    else
		{
		INFO_PRINTF2(_L("Error %d occured"), iStatus.Int());
		}

	// Compare the received data and then display the data
	if (success)
		{
		TEST(echoedData == KWriteTestData) ;

		TBuf<KNumOfBytes> convertData;	// Convert the 8-bit data read from serial port to 16-
		convertData.Copy(echoedData);	// bit so that it can be displayed to the user.
		INFO_PRINTF2(_L("Echoed Data stream = %S"), &convertData);
		}

	// Close the serial port, connection to Comms Server and recover the data port
	port.Close();
	cs.Close();
	//GT83-AT-034	Test the ability to recover a loaned port (Generic)	RCall::RecoverDataPort
	TEST_CHECKL(iDataCall.RecoverDataPort(), KErrNone, _L("GT83-TDATA-028.02")) ;

	// Hang up the call
	INFO_PRINTF1(_L("Hanging up the Data call"));
	//GT83-AT-035	Test hanging up a data call (Generic)	RCall::HangUp
	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	TEST_CHECKL(iDataCall.HangUp(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***")) ;
	INFO_PRINTF2(_L("DataCall.HangUp() returns %d"), iDataCall.HangUp()) ;
	// Close the line and call.
	//GT83-AT-022	Test closing a data line (Generic)	RLine::Close

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	return retValue;
	}

//------------------------------------------------------------------
void CTestHandShake::AnswerIncomingCallTestL()
/**
 * Answers an incoming data call.
 * This method opens a data line and call, and retrieves the status of the call.
 * It then answers an incoming call from a remote modem (initiated by t_echo.exe),
 * loans the Comm port, connects to the Comms Server, opens a serial port and proceeds
 * to read data transmitted by the remote modem. It then echoes the received data
 * closes the serial port, its connection to the Comms Server, the data port, call and
 * line.
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-004.02 "Test retrieval of a data line's current status when the line's status is ""Idle"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-015.05 "Test retrieval of the current call status when the call's status is ""answering"""
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 * @test GT83-TDATA-026.02 Test the ability to place an asynchronous request to hang up a data call
 * @test GT83-TDATA-027.03 Test support for asynchronous loaning of the data port to the client when a connection has been established to a remote modem
 * @test GT83-TDATA-028.02 Test the ability to recover a loaned port that was requested by an asynchronous request and the request completed
*/
	{
	INFO_PRINTF1(_L("======Start test Answer Incoming call test"));
    //to make sure that line and call will be closed in leave case
	CleanupStack::PushL(*this) ; ;
	const TInt KExpectedBytes=57;		// Number of bytes of data expected from remote end

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));

	TEST_CHECKL(iLine.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	TEST_CHECKL(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));

	//get status
	//GT83-AT-026	Test retrieval of a data line's current status.
	RCall::TStatus callStatus;
	CHECKPOINT(iDataCall.GetStatus(callStatus), KErrNone, _L("GT83-TDATA-004.02"));
	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);
	INFO_PRINTF1(_L("Waiting for incoming call..."));

	// DEF55659-NCh : timout changed from 60s to 120s. Must be changed back to 10s when MMTSY is optimized
	WaitWithTimeoutL(iStatus,120*KOneSecond);	// but timeout after 120 seconds

	//cancel AnswerIncomingCall if request is still pending
	if(iStatus.Int() == KRequestPending)
		{
		iDataCall.AnswerIncomingCallCancel();
		User::WaitForRequest(iStatus);
		}

	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed AnswerIncomingCallTestL() ***")) ;

	CHECKPOINT(iDataCall.GetStatus(callStatus), KErrNone, _L("GT83-TDATA-015.05"));
	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	TestGetCallCapabilitiesL(iDataCall) ;

	// Loan the Comm port
	RCall::TCommPort commPort;

	// GT83-AT-033	Test support for loaning the data port to the client (Generic) RCall::LoanDataPort
	iDataCall.LoanDataPort(iStatus,commPort);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-027.03"));
	if (iStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Comm port loaned successfully"));
		}
	else if (iStatus==KErrCommsLineFail)
		{
		INFO_PRINTF1(_L("Loan Data port Error: 'KErrCommsLineFail'."));
		INFO_PRINTF1(_L("This error is currently known to only happen with the Motorola"));
		INFO_PRINTF1(_L("Time port. Please see the PhoneInfo.cpp file for more information."));
		return;
		}
	else
		{
		INFO_PRINTF2(_L("Error %d occured during"), iStatus.Int());
		INFO_PRINTF1(_L("Loan comm port request.\nUnable to continue."));
		return;
		}

	// Connect to the Comms Server and load the CSY
	RCommServ cs;
	TESTL(cs.Connect() == KErrNone);
	TInt ret=cs.LoadCommModule(KCsyName);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrAlreadyExists), _L("*** Failed to load CSY ***"));

	// Open a serial port
	RComm port;
	TEST_CHECKL(port.Open(cs,commPort.iPort,ECommShared), KErrNone, _L("*** Failed to open serial port ***"));
	INFO_PRINTF1(_L("Serial port opened successfully"));

	// Read a stream of data from the serial port
	TInt success=EFalse;		// Boolean indicating whether data read has been successful

	TBuf8<KExpectedBytes> readData;
	INFO_PRINTF1(_L("Posting a read..."));
	// DEF55659-NCh : timout changed from 20s to 40s. Must be changed back when MMTSY is optimized	
	port.Read(iStatus,KTenSeconds*4,readData);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus, KErrNone, _L("*** Failed to read data from serial port ***"));

	if (iStatus==KErrTimedOut)
		{
		INFO_PRINTF1(_L("A Time out error occured during the read"));
		}
	else if (iStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Data stream read successfully"));
		success=ETrue;
		}
    else
		{
		INFO_PRINTF2(_L("Error %d occured"), iStatus.Int());
		}

	// Display and then echo the received data
	if (success)
		{
		TBuf<KExpectedBytes> convertData;	// Convert the 8-bit data read from serial port to 16-bit
		convertData.Copy(readData); // so that it can be displayed to the screen
		INFO_PRINTF2(_L("Data stream = %S"), &convertData);

		// Echo the data
		//User::After(KFiveSeconds);		// Give remote end time to post a read request
		port.Write(iStatus,readData);
		User::WaitForRequest(iStatus);
		TESTL(iStatus == KErrNone);
		INFO_PRINTF1(_L("Data stream echoed successfully"));
		}

	// Close the serial port, connection to Comms Server and recover the data port
	port.Close();
	cs.Close();
	//GT83-AT-034	Test the ability to recover a loaned port (Generic)	RCall::RecoverDataPort
	TEST_CHECKL(iDataCall.RecoverDataPort(),KErrNone, _L("GT83-TDATA-028.02"));

	// Hang up the call
	INFO_PRINTF1(_L("Hanging up the Data call"));
	TEST_CHECKL(iDataCall.HangUp(), KErrNone, _L("*** Failed to Hang Up. If you are using an Ericsson phone, this is a PHONE related problem! ***"));

	//This test has some trobles with Ericsson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("DataCall.HangUp() returned status %d"), iDataCall.HangUp()) ;

	// Close the line and call
	//GT83-AT-022	Test closing a data line (Generic)	RLine::Close

	// Close the line and call
	CleanupStack::PopAndDestroy() ;

	INFO_PRINTF1(_L("======End test ====="));
	}
//------------------------------------------------------------------

TVerdict CTestHandShake::doTestStepL()
	{
	// DEF55659-NCh : this warning must be removed when the MMSTY is optimised.
	INFO_PRINTF1(_L("\n*** NOTE: this test is not reliable  and takes time     ***"));
	INFO_PRINTF1(_L("*** to run because timeout where changed due to the     ***"));
	INFO_PRINTF1(_L("*** way the TSY fetches info from the Comms repository. ***"));

	TInt ret = DialRemoteModemTestL();
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("The next test will start shortly..."));
		User::After(KTenSeconds);
		AnswerIncomingCallTestL();
		}
	else
		INFO_PRINTF1(_L("An error occured during the DialRemoteModemTest\n."));
	return TestStepResult() ;
	}
//------------------------------------------------------------------

CTestGetMobileDataCallCaps::CTestGetMobileDataCallCaps()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestGetMobileDataCallCapsL"));
	}
//------------------------------------------------------------------

TVerdict CTestGetMobileDataCallCaps::doTestStepL()
	{
	RMobileLine line;
	TEST_CHECKL(line.Open(iPhone, KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	RMobileCall call ;
	TEST_CHECKL(call.OpenNewCall(line), KErrNone, _L("GT83-TDATA-013.06"));

	RMobileCall::TMobileCallDataCapsV1 caps;
	RMobileCall::TMobileCallDataCapsV1Pckg capsPckg(caps);
	TEST(KErrNone == call.GetMobileDataCallCaps(capsPckg));

	User::After(KTenSeconds);

	call.Close();
	line.Close();
	return TestStepResult() ;
	}

//New object to load and parse the test config file

CConfigParams* CConfigParams::NewL(const TDesC &aCfgFileName)
	{
	CConfigParams *f;
	f=new (ELeave) CConfigParams();
	CleanupStack::PushL(f);
	//A leave during the ConstrucL is not fatal, just means there's no param file
	TRAP_IGNORE(f->ConstructL(aCfgFileName));
	CleanupStack::Pop();
	return f;
	}

void CConfigParams::ConstructL(const TDesC &aCfgFileName)
	{
	//Find the config file
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();
	//Location for the test parameter config file
	_LIT(KTinetConfigFilePaths,"\\;\\system\\data\\");
	TFindFile filePath(fs.iObj);
	User::LeaveIfError(filePath.FindByPath(aCfgFileName,&KTinetConfigFilePaths));
	//read the content of the file
	TAutoClose<RFile> fl;
	fl.PushL();
	User::LeaveIfError(fl.iObj.Open(fs.iObj,filePath.File(),EFileShareExclusive));
	User::LeaveIfError(fl.iObj.Read(iConfigParamBuf8));
	iConfigParamBuf.Copy(iConfigParamBuf8);
	CleanupStack::PopAndDestroy(2);
	iFileExist = ETrue;
	}

const TPtrC CConfigParams::FindAlphaVar(const TDesC &aVarName, const TDesC &aDefault)
	{
	if(!iFileExist)
		return TPtrC(aDefault);
	TInt pos=iConfigParamBuf.Find(aVarName);
	if (pos==KErrNotFound)
		return TPtrC(aDefault);
	TLex lex(iConfigParamBuf.Mid(pos));
	lex.SkipCharacters();
	lex.SkipSpaceAndMark();		// Should be at the start of the data
	lex.SkipCharacters();
	return TPtrC(lex.MarkedToken().Ptr(),lex.MarkedToken().Length());
	}

TInt CConfigParams::FindNumVar(const TDesC &aVarName, const TInt aDefault)
	{
	TInt result;
	TPtrC ptr = FindAlphaVar(aVarName,KNullDesC);
	if(ptr.Length())
		{
		TLex lex(ptr);
		if (lex.Val(result)==KErrNone)
			return result;
		}
	return aDefault;
	}

TBool CConfigParams::ConfigFileExist()
	{
	return iFileExist;
	}

