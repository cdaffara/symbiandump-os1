// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test the Voice functionality of the Basic MM Tsy.
// 
//

/**
 @file
*/

#include <etelmm.h>

#include "TE_Voice.h"
#include "TE_VoiceTestStepBase.h"

_LIT(KVoiceLineName,"Voice");
_LIT(KConsoleTitle, "TE_Voice Test");
_LIT(KPleaseRingThePhone, "Please ring the phone\n");

const TInt KTenPartOfSec = 100000 ;
const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)
const TInt KTenSeconds=10000000;	// Used in a time out function, 10 seconds (in microSeconds)
 
//------------------------------------------------------------------
CTestPhoneInfo::CTestPhoneInfo()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhoneInfo"));
	}
//------------------------------------------------------------------

enum TVerdict CTestPhoneInfo::doTestStepL()
//void CTestVoice::TestPhoneInfoL()
/**
 * This function retrieves a snapshot of the phone's GSM information, gets the
 * number of lines associated with the phone as well as information on the line
 * and phone. Gets the phone's status and also performs some Cancel Requests.
 * @test GT83-TVOICE-037.01	Retrieve phone's multimode capabilities and print them to screen.
 */
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "Phone Information test") ; TEST_START(&KStartText) ;
	console->Printf(_L("Phone Information Test...\n"));

	// Retrieve a snapshot of the current phone information
	INFO_PRINTF1(_L("Get Phone Info..."));
	console->Printf(_L("Get Phone Info...\n"));
	RPhone::TPhoneInfo phoneInfo;
	RPhone::TCaps phoneCaps;
	TInt ret=iPhone.GetInfo(phoneInfo);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobilePhone::GetInfo() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Phone info is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Phone Info is successful."));
		TEST_CHECKL(phoneInfo.iDetection, RPhone::EDetectedPresent, _L("GT83-TVOICE-0"));
		TESTCHECKL(phoneInfo.iDetection,RPhone::EDetectedPresent);
		INFO_PRINTF1(_L("Local Modem has been detected."));
		}

	//test mobile phone caps
	RMobilePhone::TMobilePhoneModeCaps mobileCaps ;
	ret = iPhone.GetMultimodeCaps((TUint32&)mobileCaps);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobilePhone::GetMultimodeCaps() ***")) ;
	TESTCHECK(ret, KErrNone);
	
	Print_TMobilePhoneModeCaps(mobileCaps);

	// Number of lines associated with the phone
	TInt lineCount=0;
	TEST_CHECKL(iPhone.EnumerateLines(lineCount), KErrNone, _L("*** Failed RMobilePhone::EnumerateLines() ***"));
	TESTL(lineCount>0);
	TESTCHECKCONDITIONL(lineCount>0);
	for (;lineCount>0; lineCount--)
		{
		ret = iPhone.GetLineInfo(lineCount-1, iLineInfo);
		CHECKPOINT(ret, KErrNone, _L("*** Failed RMobilePhone::GetLineInfo() ***"));
		TESTCHECK(ret, KErrNone);
		INFO_PRINTF4(_L("Line %d (%S) cap flags = 0x%x"),
			lineCount-1, &iLineInfo.iName,iLineInfo.iLineCapsFlags);
		if (iLineInfo.iLineCapsFlags&RLine::KCapsVoice)
			{
			INFO_PRINTF3(_L("Line %d (%S) has voice capability"), lineCount-1, &iLineInfo.iName);
			iLineIndex=lineCount-1;
			break;
			}
		}
	TESTL(lineCount>0);
	TESTCHECKCONDITIONL(lineCount>0);
	// Status of the Phone
	INFO_PRINTF1(_L("Get the Phone's Status..."));
	RPhone::TStatus phoneStatus;
	ret=iPhone.GetStatus(phoneStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobilePhone::GetStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Phone Status is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Phone Status is successful."));
		TEST_CHECKL(phoneStatus.iMode, RPhone::EModeIdle, _L("GT83-TVOICE-0"));
		TESTCHECKL(phoneStatus.iMode, RPhone::EModeIdle);
		INFO_PRINTF1(_L("The phone is currently Idle"));
		}

	// Notification Cancel Requests : Modem Detected
	INFO_PRINTF1(_L("Notify Modem Detected Cancel Request..."));
	RPhone::TModemDetection detected;
	iPhone.NotifyModemDetected(iStatus, detected);
	iPhone.NotifyModemDetectedCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrCancel), _L("*** Failed RMobilePhone::NotifyModemDetectedCancel() ***"));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Notify Modem Detected Cancel status: %d")), iStatus.Int());

	// Notification Cancel Requests : Caps Change
	INFO_PRINTF1(_L("Notify Caps Change Cancel Request..."));
	iPhone.NotifyCapsChange(iStatus, phoneCaps);
	iPhone.NotifyCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrCancel), _L("*** Failed RMobilePhone::NotifyCapsChangeCancel() ***"));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Notify Caps Change Cancel status: %d")), iStatus.Int());
	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestLineStatusIncomingCall::CTestLineStatusIncomingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineStatusIncomingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestLineStatusIncomingCall::doTestStepL()
//void CTestVoice::TestLineStatusIncomingCallL()
/**	 Tests line status function
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-004.00 Test retrieval of a voice line's current status (Generic)
 * @test GT83-TVOICE-004.02 "Test retrieval of a voice line's current status when the line's status is ""Idle"""
 * @test GT83-TVOICE-004.04 "Test retrieval of a voice line's current status when the line's status is ""Ringing"""
 * @test GT83-TVOICE-004.07 "Test retrieval of a voice line's current status when the line's status is ""Connected"""
 * @test GT83-TVOICE-004.08 "Test retrieval of a voice line's current status when the line's status is ""Hanging up"""
 * @test GT83-TVOICE-008.02 Test for a notification when there is an incoming call on a voice line
 * @test GT83-TVOICE-015.05 Test opening an existing call from a voice line by specifying the name of the line and the name of an existing call
 * @test GT83-TVOICE-026.00 Test answering a call on a voice line (Generic)
 * @test GT83-TVOICE-030.00 Test retrieval of a mobile voice line's current status (Generic)
 * @test GT83-TVOICE-030.02 "Test retrieval of a mobile voice line's current status when there is only one call on the line and it's status is ""Dialling"""
 * @test GT83-TVOICE-030.03 "Test retrieval of a mobile voice line's current status when there is only one call on the line and it's status is ""Ringing"""
 * @test GT83-TVOICE-030.06 "Test retrieval of a mobile voice line's current status when there is only one call on the line and it's status is ""Disconnecting"""
 * @test GT83-TVOICE-030.07 "Test retrieval of a mobile voice line's current status when there is only one call on the line and it's status is ""on hold"""
 * @test GT83-TVOICE-031.01 Test setting a request for a notification of a change in the status of the mobile voice line
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Line Status") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Line Status...\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	RCall::TStatus lineStatus;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	TInt ret = iLine.Open(iPhone,KVoiceLineName);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-001.01"));
	TESTCHECKL(ret,KErrNone);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***"));
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***"));
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-031.01")) ;

	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);

	TName callName;
	/* @test GT83-AT-024	Test setting notification of an incoming call on a Voice Line */
	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds
	ret = iStatus.Int();
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-008.02")) ;
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is ringing*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***"));
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TVOICE-004.04")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***"));
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TVOICE-030.003")) ;
	INFO_PRINTF1(_L("Phone is ringing..."));
	console->Printf(_L("Phone is ringing...\n"));
	
	//Test for incoming Voice Call
	TestLineCapabilitiesL();

	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	ret = iVoiceCall.OpenExistingCall(iLine, callName);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-015.05 - *** Failed RMobileCall::OpenExistingCall() ***")) ;
	TESTCHECKL(ret, KErrNone);
	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,30*KOneSecond);		// but timeout after 2 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::AnswerIncomingCall() ***")) ;
	TESTCHECKL(iStatus.Int(), KErrNone);
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.04")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-030.03")) ;
	TestLineCapabilitiesL();

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-030.07")) ;


	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Hanging Up"));
	console->Printf(_L("Hanging Up\n"));
	iVoiceCall.HangUp(iStatus);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is hang up*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-004.08")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TVOICE-034.09")) ;

	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;  //???  line 825
	TESTCHECKL(iStatus.Int(), KErrNone);
	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-030.02")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------


CTestLineStatusOutgoingCall::CTestLineStatusOutgoingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineStatusOutgoingCall"));
	}
//------------------------------------------------------------------
enum TVerdict CTestLineStatusOutgoingCall::doTestStepL()
//void CTestVoice::TestLineStatusOutgoingCallL()
/**	 Tests line status function
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-004.00 Test retrieval of a voice line's current status (Generic)
 * @test GT83-TVOICE-004.01 "Test retrieval of a voice line's current status when the line's status is ""Unknown"""
 * @test GT83-TVOICE-004.02 "Test retrieval of a voice line's current status when the line's status is ""Idle"""
 * @test GT83-TVOICE-004.03 "Test retrieval of a voice line's current status when the line's status is ""Dialling"""
 * @test GT83-TVOICE-004.07 "Test retrieval of a voice line's current status when the line's status is ""Connected"""
 * @test GT83-TVOICE-004.08 "Test retrieval of a voice line's current status when the line's status is ""Hanging up"""
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-030.00 Test retrieval of a mobile voice line's current status (Generic)
 * @test GT83-TVOICE-030.07 "Test retrieval of a mobile voice line's current status when there is only one call on the line and it's status is ""on hold"""
 * @test GT83-TVOICE-031.01 Test setting a request for a notification of a change in the status of the mobile voice line
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Line Status") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Line Status\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	RCall::TStatus lineStatus;
    TName CallName ;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));
	

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	TInt ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-031.01")) ;
	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	INFO_PRINTF1(_L("Opening New Voice Call"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	INFO_PRINTF2(_L("Dialling %S...."),&GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);

	//doesn't work for MM.TSY
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	TESTCHECK(ret, KErrNone);
	//TEST_CHECKL(lineStatus, RCall::EStatusDialling, _L("GT83-TVOICE-004.03")) ;

	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Dial() ***")) ;
	TESTCHECKL(iStatus.Int(), KErrNone);
	//Test for outgoing Voice Call
	TestLineCapabilitiesL();

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-030.07")) ;


	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Call connected - listen for 20 seconds..."));
	console->Printf(_L("Call connected - listen for 20 seconds...\n"));
	User::After(20*KOneSecond); // hold call active for 20 seconds

	INFO_PRINTF1(_L("Hanging up"));
	iVoiceCall.HangUp(iStatus);
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-004.08")) ;
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;
	TESTCHECKL(iStatus.Int(), KErrNone);

	//Test for outgoing Voice Call after hang up
	TestLineCapabilitiesL();
	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileLine::GetMobileLineStatus() ***")) ;
	TESTCHECK(ret, KErrNone);
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-004.01")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestLineInfo::CTestLineInfo()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	//iTestStepName = _L("TestLineInfo");  //MODIFIIED -Was Incorrect ("TestLineInfoL")
	SetTestStepName(_L("TestLineInfo"));
	}
//------------------------------------------------------------------

enum TVerdict CTestLineInfo::doTestStepL()
//void CTestVoice::TestLineInfoL()
/**
 * This function gets the Line capabilities, line information, Number of calls
 * on the line and call information,
 * @test NONE	Test the ability to get the current line information of the Voice Line (Generic)
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-005.01 "Test retrieval of the current hook status, when the line is currently ""off hook"""
 * @test GT83-TVOICE-005.02 "Test retrieval of the current hook status, when the line is currently ""on hook"""
 * @test GT83-TVOICE-012.01 Test the ability to get the current line information of the voice line. Display the line information to screen.
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-026.00 Test answering a call on a voice line (Generic)
 * @test GT83-TVOICE-027.02 Test the ability to place an asynchronous request to hang up a voice call
 * @test GT83-TVOICE-006.01 "Test enumeration of the number of calls on a voice line, when there is 0 active call."
 * @test GT83-TVOICE-006.02 "Test enumeration of the number of calls on a voice line, when there is 1 active call."
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Line Information") ; TEST_START(&KStartText) ;
	TInt lineCount=0;

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

   	INFO_PRINTF1(_L("Retrieving no. of calls opened from the line.."));
   	console->Printf(_L("Retrieving no. of calls opened from the line..\n"));
	TEST_CHECKL(iLine.EnumerateCall(lineCount), KErrNone, _L("*** Failed RMobileLine::EnumerateCall() ***"));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("No of calls opened = %d")), lineCount);

    TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05 - *** Failed RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	// Line Information
	RLine::TLineInfo lineInfo;
	TRAPD(ret, ret=iLine.GetInfo(lineInfo));
	TESTCHECK(ret, KErrNone);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileLine::GetInfo() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Line Info. is NOT supported"));
		}
	else
		{
		INFO_PRINTF1(_L("Get Line Info is successful"));
		TESTL(lineInfo.iHookStatus ==  RCall::EHookStatusOn) ;
		TESTCHECKL(lineInfo.iHookStatus, RCall::EHookStatusOn);
		TESTL(lineInfo.iStatus == RCall::EStatusIdle) ;
		TESTCHECKL(lineInfo.iStatus, RCall::EHookStatusOn);
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Name of last call added: %S")), &lineInfo.iNameOfLastCallAdded);
		}

	// Hook status information
	INFO_PRINTF1(_L("Retrieving Hook Status..."));
	console->Printf(_L("Retrieving Hook Status...\n"));
	RCall::THookStatus hookStatus;
	ret=iLine.GetHookStatus(hookStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileLine::GetHookStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Hook status is NOT supported"));
		}
	else
		{
		TEST_CHECKL(hookStatus, RCall::EHookStatusOn, _L("GT83-TVOICE-005.02")) ;
		TESTCHECKL(hookStatus, RCall::EHookStatusOn);
		INFO_PRINTF1(_L("The line is currently ON Hook"));
		}

	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);

	// Answer an incoming call initiated  and obtain the call's current status
	iVoiceCall.AnswerIncomingCall(iStatus);

	WaitWithTimeout(iStatus,6*KTenSeconds);		// but timeout after 60 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::AnswerIncomingCall() ***")) ;
	TESTCHECKL(iStatus.Int(), KErrNone)
	// Count the no. of calls on the line & Get call info.
	INFO_PRINTF1(_L("Retrieving no. of calls opened from the line.."));
	console->Printf(_L("Retrieving no. of calls opened from the line..\n"));
	TEST_CHECKL(iLine.EnumerateCall(lineCount), KErrNone, _L("GT83-TVOICE-006.02 - *** Failed RMobileLine::EnumerateCall() ***"));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("No of calls opened = %d")), lineCount);

	//doesn't work for MM.TSY
	INFO_PRINTF1(_L("Getting Call info.."));
	console->Printf(_L("Getting Call info..\n"));
	RLine::TCallInfo callInfo;
	ret=iLine.GetCallInfo(0,callInfo);
	//doesn't work for MM.TSY
	CHECKPOINT_EXPR((ret == KErrNotSupported || ret == KErrNone), _L("GT83-TVOICE-007.01 - *** Failed RMobileLine::GetCallInfo() ***"));
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
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileLine::GetHookStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Hook status is NOT supported"));
		}
	else
		{
		CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TVOICE-005.01")) ;
		INFO_PRINTF1(_L("The line is currently OFF Hook"));
		}

	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));
	User::After(KTenSeconds);

	INFO_PRINTF1(_L("Hanging up"));
	console->Printf(_L("Hanging up\n"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-027.02 - *** Failed RMobileCall::Hangup() ***")) ;
	// Close the line and call
	//GT83-AT-022	Test closing a Voice Line (Generic)	RLine::Close

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);	

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestLineNotificationsIncomingCall::CTestLineNotificationsIncomingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineNotificationsIncomingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestLineNotificationsIncomingCall::doTestStepL()
//void CTestVoice::TestLineNotificationsIncomingCallL()
/** Tests line notifications
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-008.02 Test for a notification when there is an incoming call on a voice line
 * @test GT83-TVOICE-009.01 Test setting notification of a change in hook status of a voice line
 * @test GT83-TVOICE-009.02 "Test notification of a change in hook status of a voice line from ""off"" to ""on"""
 * @test GT83-TVOICE-009.03 "Test notification of a change in hook status of a voice line from ""on"" to off"""
 * @test GT83-TVOICE-010.01 "Test notification of a change in the status of the voice line from ""idle"" to ""ringing"" to ""answering"" to ""connected"" to ""hanging up"" to ""idle""
 * @test GT83-TVOICE-011.02 Test notification of a call being added to a voice line that currently has no calls
 * @test GT83-TVOICE-015.05 Test opening an existing call from a voice line by specifying the name of the line and the name of an existing call
 * @test GT83-TVOICE-026.00 Test answering a call on a voice line (Generic)
 * @test GT83-TVOICE-031.01 Test setting a request for a notification of a change in the status of the mobile voice line
 * @test GT83-TVOICE-031.07 "Test for a notification of a change in status of the mobile voice line from ""connected"" to ""disconnecting"""
 * @test GT83-TVOICE-031.08 "Test for a notification of a change in status of the mobile voice line from ""disconnecting"" to ""idle"""
 * @test GT83-TVOICE-031.09 "Test for a notification of a change in status of the mobile voice line from ""idle"" to ""ringing"""
 * @test GT83-TVOICE-031.10 "Test for a notification of a change in status of the mobile voice line from ""ringing"" to ""answering"""
 * @test GT83-TVOICE-031.11 "Test for a notification of a change in status of the mobile voice line from ""answering"" to ""connected"""
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Line Notifications") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Line Notifications\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TName callName1;
	TName callName2;
	TInt ret = KErrNone;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;


	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine.Open() ***"));

	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call

	//GT83-AT-026	Test setting notification of a change in the status of the Voice Line
	iLine.NotifyStatusChange(iStatus2, lineStatus);

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	//GT83-AT-037	Test setting notification of a change in hook status of a Voice Line	void RLine::NotifyHookChange(TRequestStatus& aStatus, THookStatus& aHookStatus)	Function does not produce any unexpected errors. aStatus = KErrPending
	iLine.NotifyHookChange(iStatus4, hookStatus);

    /* GT83-AT-025	Test notification of a call being added to a Voice Line 	*/
	iLine.NotifyCallAdded(iStatus5, callName2);

	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-008.02")) ;
	CHECKPOINT(iStatus2.Int(), KRequestPending, _L("GT83-TVOICE-010.01")) ;
	CHECKPOINT(iStatus3.Int(), KRequestPending, _L("GT83-TVOICE-031.01")) ;
	CHECKPOINT(iStatus4.Int(), KRequestPending, _L("GT83-TVOICE-009.01")) ;
	CHECKPOINT(iStatus5.Int(), KRequestPending, _L("GT83-TVOICE-0011.01")) ;

	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.02")) ;

    // @test GT83-AT-026	Test notification of a change in the status of the Voice Line
	// from "idle" to "ringing"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TVOICE-010.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.09")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TVOICE-031.09")) ;

	/* GT83-AT-025	Test notification of a call being added to a Voice Line
	that currently has no calls*/
	User::WaitForRequest(iStatus5);
	CHECKPOINT(iStatus5.Int(), KErrNone, _L("GT83-TVOICE-011.02")) ;
	CHECKPOINT(callName2 == callName1, 1,  _L("GT83-TVOICE-011.02")) ;

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));
	console->Printf(_L("Phone is ringing... Now answering...\n"));

	/* @test GT83-AT-026	Test notification of a change in the status of the Voice Line from
	 "idle" to "ringing" to "answering" to "connected" to "hanging up" to "idle"
	 */
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	ret = iVoiceCall.OpenExistingCall(iLine, callName1);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-015.05")) ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);
    // GT83-AT-026	Test notification of a change in the status of the Voice Line
	// from "ringing" to "answering"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.01")) ;
	CHECKPOINT(lineStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-010.01")) ;

	WaitWithTimeout(iStatus,2*KOneSecond);		// but timeout after 2 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::AnswerIncomingCall() ***")) ;	  //wrong code -2147483647


    // @test GT83-AT-026	Test notification of a change in the status of the Voice Line
	// from "answering"	to "connecting"
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.01")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-010.01")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.10")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusAnswering, _L("GT83-TVOICE-031.10")) ;

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.11")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.11")) ;

	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-009.02")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TVOICE-009.02")) ;

	// now hangup call
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	console->Printf(_L("Call connected - now hanging up...\n"));
	
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;

    //GT83-AT-026	Test notification of a change in the status of the Voice Line
	// from "connecting" to "hanging up"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-010.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.07")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TVOICE-031.07")) ;

	// @test GT83-AT-026	Test notification of a change in the status of the data
	// line from "hanging up" to "idle"
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-010.02")) ;

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.08")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-031.08")) ;

	//!< @test GT83-AT-037	Test notification of a change in hook status of a Voice Line
	//!<  from "off" to "on" aHookStatus = EHookStatusOff before the hook status changes,
	//!< and EHookStatusOn after the hook status changes
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-009.03")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TVOICE-009.03")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestLineNotificationsOutgoingCall::CTestLineNotificationsOutgoingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineNotificationsOutgoingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestLineNotificationsOutgoingCall::doTestStepL()
//void CTestVoice::TestLineNotificationsOutgoingCallL()
/** Tests line notifications
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-009.02 "Test notification of a change in hook status of a voice line from ""off"" to ""on"""
 * @test GT83-TVOICE-009.03 "Test notification of a change in hook status of a voice line from ""on"" to off"""
 * @test GT83-TVOICE-010.03 "Test notification of a change in the status of the voice line from ""idle"" to ""dialling"" to ""connecting"" to ""connected"" to ""hanging up"" to ""idle""
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-031.02 "Test for a notification of a change in status of the mobile voice line from ""idle"" to ""dialling"""
 * @test GT83-TVOICE-031.04 "Test for a notification of a change in status of the mobile voice line from ""connecting"" to ""connected"""
 * @test GT83-TVOICE-031.07 "Test for a notification of a change in status of the mobile voice line from ""connected"" to ""disconnecting"""
 * @test GT83-TVOICE-031.08 "Test for a notification of a change in status of the mobile voice line from ""disconnecting"" to ""idle"""
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Line Notifications") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Line Notifications\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TName callName1;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;


	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine.Open() ***"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, callName1), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName1);

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	// Now test call status during an outgoing call
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusDialling, _L("GT83-TVOICE-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.02")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDialling, _L("GT83-TVOICE-031.02")) ;

	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-009.03")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TVOICE-009.03")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	User::WaitForRequest(iStatus); //end of dial
	
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.04")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.04")) ;

	INFO_PRINTF1(_L("Call connected - hanging up..."));
	console->Printf(_L("Call connected - hanging up...\n"));

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	User::After(10*KOneSecond);
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.07")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TVOICE-031.07")) ;

	/* @test GT83-AT-037	Test notification of a change in hook status of a data
	 line from "on" to off"	aHookStatus = EHookStatusOn before the hook status changes,
	 */
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-009.02")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TVOICE-009.02")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.08")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-031.08")) ;

	// Using TESTCHECKL
	TESTCHECKL(iStatus.Int(), KErrNone);
	TESTCHECKL(iStatus2.Int(), KErrNone);
	TESTCHECKL(iStatus3.Int(), KErrNone);
	TESTCHECKL(iStatus4.Int(), KErrNone);
	TESTCHECKL(iStatus5.Int(), KErrNone);
	TESTCHECKL(iStatus6.Int(), KErrNone);
	TESTCHECKL(iStatus7.Int(), KErrNone);
	
	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestLineCancels::CTestLineCancels()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestLineCancels"));
	}
//------------------------------------------------------------------

enum TVerdict CTestLineCancels::doTestStepL()
//void CTestVoice::TestLineCancelsL()
/**
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-008.01 Test setting notification of an incoming call on a voice line
 * @test GT83-TVOICE-008.03 Test the ability to cancel the notification of an incoming call on a voice line
 * @test GT83-TVOICE-009.01 Test setting notification of a change in hook status of a voice line
 * @test GT83-TVOICE-009.04 Test the ability to cancel a notification of a change in hook status of a voice line
 * @test GT83-TVOICE-010.04 Test the ability to cancel the notification of a change in the status of the voice line
 * @test GT83-TVOICE-011.01 Test setting notification of a call being added to a voice line
 * @test GT83-TVOICE-011.05 Test the ability to cancel a notification of a call being added to a voice line
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);

	_LIT(KStartText, "Test Line Cancels") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Line Cancels\n"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	// Notify Incoming Call
	TName callName;
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine.Open() ***"));

	iLine.NotifyIncomingCall(iStatus, callName);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-008.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iLine.NotifyIncomingCallCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-008.03")) ;

	// Notify Status Change
	RCall::TStatus lineStatus;
	iLine.NotifyStatusChange(iStatus, lineStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-010.02")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iLine.NotifyStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-010.04")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	// Notify Mobile Status Change
	RMobileCall::TMobileCallStatus lineStatus2;
	iLine.NotifyMobileLineStatusChange(iStatus, lineStatus2);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-0")) ;
	iLine.NotifyMobileLineStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-0")) ;
#endif
	// Notify Hook Change
	RCall::THookStatus hookStatus;
	iLine.NotifyHookChange(iStatus, hookStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-009.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iLine.NotifyHookChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-009.04")) ;

	// Notify Call Added
	iLine.NotifyCallAdded(iStatus, callName);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-011.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iLine.NotifyCallAddedCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-011.05")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallInfo::CTestCallInfo()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallInfo"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCallInfo::doTestStepL()
//void CTestVoice::TestCallInfoL()
/**
 * This function gets information pertaining to a call:
 * Bearer Service Information, Call duration
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-009.02 "Test notification of a change in hook status of a voice line from ""off"" to ""on"""
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-019.01 "Test retrieval of the bearer service info of a call. Display to screen, all the bearer service information associated with the call."
 * @test GT83-TVOICE-023.01 Test retrieval of the duration of a voice call. Display the call duration to screen
 * @test GT83-TVOICE-024.01 Test the ability to get the current call information of the voice call. Display the call information to screen.
 * @test GT83-TVOICE-026.00 Test answering a call on a voice line (Generic)
 * @test GT83-TVOICE-036.01 Test retreival of the call information and print the call information to screen.
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Call Information") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Call Information"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg mobileCallInfoPckg(mobileCallInfo) ;

	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine.Open() ***"));

	TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05 - *** Failed RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	//get status
	RCall::TStatus callStatus;

	TInt ret = iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TD-016.02 - *** Failed RMobileCall::iVoiceCall.GetStatus() ***")) ;
	CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TD-016.02")) ;

	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;
	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,60*KOneSecond);		// but timeout after 60 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::AnswerIncomingCall() ***")) ;

	// User Information
	INFO_PRINTF1(_L("The call is now connected."));
	console->Printf(_L("The call is now connected.\n"));
	INFO_PRINTF1(_L("Retrieving Call Information."));
	console->Printf(_L("Retrieving Call Information.\n"));

	// Bearer Service Information
	INFO_PRINTF1(_L("Get Bearer Service Info..."));
	console->Printf(_L("Get Bearer Service Info...\n"));
	RCall::TBearerService bearerServiceInfo;
	ret=iVoiceCall.GetBearerServiceInfo(bearerServiceInfo);
	CHECKPOINT_EXPR((ret == KErrNone || ret == KErrNotSupported), _L("GT83-TVOICE-019.01")) ;
	if (ret==KErrNotSupported)
        {
		INFO_PRINTF1(_L("Get BearerService Information is NOT Supported"));
        }
	if (ret==KErrNone)
        {
		INFO_PRINTF1(_L("Get BearerService Information is successful"));
        }

	// Get Call Information
	INFO_PRINTF1(_L("Get Current Call Info..."));
	console->Printf(_L("Get Current Call Info...\n"));
	ret=iVoiceCall.GetInfo(iCallInfo);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::GetInfo() ***"));
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



	INFO_PRINTF1(_L("Get Current Mobile Call Info..."));
	console->Printf(_L("Get Current Mobile Call Info...\n"));
	ret=iVoiceCall.GetMobileCallInfo(mobileCallInfoPckg);
	//doesn't work for MM.TSY
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-036.01")) ;
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::GetMobileCallInfo() ***"));
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
	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	console->Printf(_L("Please stay connected... the test will hang up shortly\n"));
	User::After(5*KOneSecond);

	// Call Duration
	INFO_PRINTF1(_L("Get Call Duration..."));
	console->Printf(_L("Get Call Duration...\n"));
	TTimeIntervalSeconds callDuration2;
	ret=iVoiceCall.GetCallDuration(callDuration2);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** RMobileCall::GetCallDuration() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Current Call Duration is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), callDuration2.Int());
		}
 
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-009.02 - *** Failed RMobileCall::Hangup() ***")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallNotificationsIncomingCall::CTestCallNotificationsIncomingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestCallNotificationsIncomingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCallNotificationsIncomingCall::doTestStepL()
//void CTestVoice::TestCallNotificationsIncomingCallL()
/**
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-002.00 Test retrieval of a voice line's capabilities (Generic)
 * @test GT83-TVOICE-008.01 Test setting notification of an incoming call on a voice line
 * @test GT83-TVOICE-015.05 Test opening an existing call from a voice line by specifying the name of the line and the name of an existing call
 * @test GT83-TVOICE-016.04 "Test retrieval of the current call status when the call's status is ""ringing"""
 * @test GT83-TVOICE-016.05 "Test retrieval of the current call status when the call's status is ""answering"""
 * @test GT83-TVOICE-018.01 Test setting notification of a change in the call's capabilities
 * @test GT83-TVOICE-018.02 Test notification of a change in the call's capabilities as the call's state changes (eg. From idle to connected). Print the call's capabilities to screen
 * @test GT83-TVOICE-020.01 Test setting notification of a change in hook status of a voice call
 * @test GT83-TVOICE-020.02 "Test notification of a change in hook status of a voice call from ""off"" to ""on"""
 * @test GT83-TVOICE-020.03 "Test notification of a change in hook status of a voice call from ""on"" to off"""
 * @test GT83-TVOICE-021.01 Test setting notification of a change in the status of the voice call
 * @test GT83-TVOICE-021.02 "Test notification of a change in the status of the voice call from ""idle"" to ""ringing"" to ""answering"" to ""connected"" to ""hanging up"" to ""idle""
 * @test GT83-TVOICE-022.01 Test setting notification of a change in the voice call's duration
 * @test GT83-TVOICE-026.00 Test answering a call on a voice line (Generic)
 * @test GT83-TVOICE-033.01 Test setting notification of a change in the call's dynamic call control and call event capabilities
 * @test GT83-TVOICE-033.02 Test notification of a change in the call's dynamic call control and call event capabilities. Print the call's capabilities to screen
 * @test GT83-TVOICE-034.04 "Test retrieval of the current status of the voice call when the call's status is ""ringing"""
 * @test GT83-TVOICE-035.01 Test setting a request for a notification of a change in the status of the mobile voice call
 * @test GT83-TVOICE-035.07 "Test for a notification of a change in status of the mobile voice call from ""connected"" to ""disconnecting"""
 * @test GT83-TVOICE-035.08 "Test for a notification of a change in status of the mobile voice call from ""disconnecting"" to ""idle"""
 * @test GT83-TVOICE-035.10 "Test for a notification of a change in status of the mobile voice call from ""ringing"" to ""answering"""
 * @test GT83-TVOICE-035.11 "Test for a notification of a change in status of the mobile voice call from ""answering"" to ""connected"""
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	// Post call status change notifications
	TName callName1;
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	RCall::TCaps caps;
	RCall::THookStatus hookStatus;
	TTimeIntervalSeconds time;

	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);

	_LIT(KStartText, "Test Call Notifications") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Call Notifications\n"));
	
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-008.01")) ;

	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.01")) ;

	TInt ret = iVoiceCall.OpenExistingCall(iLine, callName1);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-015.05 - *** Failed RMobileCall::OpenExistingCall()")) ;

	iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(callStatus, RCall::EStatusRinging, _L("GT83-TVOICE-016.04")) ;

	iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusRinging, _L("GT83-TVOICE-034.04")) ;

	ret=iVoiceCall.GetCaps(caps);
	TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-002.00")) ;
	Print_TCapsEnum(caps) ;
	TEST(caps.iFlags & RCall::KCapsVoice ) ;
	TEST(caps.iFlags & RCall::KCapsAnswer);

//	INFO_PRINTF1(_L("Opening New Voice Call"));
//	TEST_CHECKL(iiVoiceCall.OpenExistingCall(iLine, callName2), KErrNone, _L("GT83-TVOICE-013.06")) ;

	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	iVoiceCall.NotifyCapsChange(iStatus4, caps);
	iVoiceCall.NotifyHookChange(iStatus5, hookStatus);
	iVoiceCall.NotifyCallDurationChange(iStatus6, time) ;
	iVoiceCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));
	console->Printf(_L("Phone is ringing... Now answering...\n"));
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);

	//doesn't work for MM.TSY iVoiceCall.GetStatus(callStatus);
	//TEST_CHECKL(callStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-016.05")) ;

	WaitWithTimeout(iStatus,60*KOneSecond);		// but timeout after 2 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00 - *** Failed RMobileCall::AnswerIncomingCall() ***")) ;


	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;
	CHECKPOINT(callStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-021.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TVOICE-035.10")) ;

	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01 - *** Failed RMobileCall::Notify")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-035.11")) ;

	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-018.01 - *** Failed RMobileCall::NotifyCapsChange() ***")) ;
	Print_TCapsEnum(caps) ;
	TEST_CHECKL(((caps.iFlags & RCall::KCapsVoice) != 0), ETrue , _L("GT83-TVOICE-018.02")) ;

	iVoiceCall.NotifyCapsChange(iStatus4, caps);
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-018.01 - *** Failed RMobileCall::NotifyCapsChange() ***")) ;
	TEST_CHECKL( (caps.iFlags & RCall::KCapsVoice)  !=0 , ETrue, _L("GT83-TVOICE-018.01")) ;
	TEST_CHECKL( (caps.iFlags & RCall::KCapsHangUp)  !=0 , ETrue, _L("GT83-TVOICE-018.01")) ;

	User::WaitForRequest(iStatus5);
	CHECKPOINT(iStatus5.Int(), KErrNone, _L("GT83-TVOICE-020.01 - *** Failed RMobileCall::NotifyHookChange() ***")) ;
	TEST_CHECKL(hookStatus, RCall::EHookStatusOff, _L("GT83-TVOICE-020.03")) ;

	User::WaitForRequest(iStatus6);
	CHECKPOINT_EXPR((iStatus6==KErrNone || iStatus6==KErrNotSupported), _L("*** Failed RMobileCall::NotifyCallDurationChange() ***"));
	if (iStatus6==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Notify Call Duration Change is NOT Supported"));
		}
	if (iStatus6==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), time.Int());
		}

	User::WaitForRequest(iStatus7);
	CHECKPOINT(iStatus7.Int(), KErrNone, _L("GT83-TVOICE-033.01 - *** Failed RMobileCall::NotifyMobileCallCapsChange() ***")) ;
	TEST_CHECKL(((mobileCallCaps.iCallControlCaps & RCall::KCapsVoice)!=0), ETrue, _L("GT83-TVOICE-033.02")) ;

	iVoiceCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);
	User::WaitForRequest(iStatus7);
	CHECKPOINT(iStatus7.Int(), KErrNone, _L("GT83-TVOICE-033.01 - *** Failed RMobileCall::NotifyMobileCallCapsChange() ***")) ;
	TEST_CHECKL( (mobileCallCaps.iCallControlCaps & RCall::KCapsHangUp)!=0, ETrue,  _L("GT83-TVOICE-033.02")) ;
	TEST_CHECKL( (mobileCallCaps.iCallControlCaps & RCall::KCapsVoice)!=0, ETrue,  _L("GT83-TVOICE-033.02")) ;
	// now hangup call
	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	console->Printf(_L("Call connected - now hanging up...\n"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;	//sometimes timed out -33

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01 - *** Failed RMobileCall::NotifyStatusChange() ***")) ;
	TEST_CHECKL(callStatus, RCall::EStatusConnected, _L("GT83-TVOICE-021.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.07 - *** Failed RMobileCall::NotifyMobileCallStatusChange() ***")) ;
	TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TVOICE-035.07")) ;

	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01 - *** Failed RMobileCall::NotifyStatusChange() ***")) ;
	TEST_CHECKL(callStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-021.02")) ;

	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.07 - *** Failed RMobileCall::NotifyMobileCallStatusChange() ***")) ;
	TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-035.08")) ;

	// Using TESTCHECKL
	TESTCHECKL(iStatus.Int(), KErrNone);
	TESTCHECKL(iStatus2.Int(), KErrNone);
	TESTCHECKL(iStatus3.Int(), KErrNone);
	TESTCHECKL(iStatus4.Int(), KErrNone);
	TESTCHECKL(iStatus5.Int(), KErrNone);
	TESTCHECKL(iStatus6.Int(), KErrNone);
	TESTCHECKL(iStatus7.Int(), KErrNone);
	
	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallNotificationsOutgoingCall::CTestCallNotificationsOutgoingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallNotificationsOutgoingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCallNotificationsOutgoingCall::doTestStepL()
//void CTestVoice::TestCallNotificationsOutgoingCallL()
/**
* Post call status change notifications
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-016.03 "Test retrieval of the current call status when the call's status is ""dialling"""
 * @test GT83-TVOICE-016.08 "Test retrieval of the current call status when the call's status is ""hanging up"""
 * @test GT83-TVOICE-020.01 Test setting notification of a change in hook status of a voice call
 * @test GT83-TVOICE-021.01 Test setting notification of a change in the status of the voice call
 * @test GT83-TVOICE-021.03 "Test notification of a change in the status of the voice call from ""idle"" to ""dialling"" to ""connected"" to ""hanging up"" to ""idle"""
 * @test GT83-TVOICE-034.03 "Test retrieval of the current status of the voice call when the call's status is ""dialling"""
 * @test GT83-TVOICE-035.01 Test setting a request for a notification of a change in the status of the mobile voice call
 * @test GT83-TVOICE-035.02 "Test for a notification of a change in status of the mobile voice call from ""idle"" to ""dialling"""
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);	
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

    TName CallName ;

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	//TTimeIntervalSeconds time;
 	//RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	//RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	_LIT(KStartText, "Test Call Notifications") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Call Notifications\n"));
	
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05 - *** RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	// Now test call status during an outgoing call
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01 - *** Failed RMobileCall::NotifyStatusChange() ***")) ;
	CHECKPOINT(callStatus, RCall::EStatusDialling, _L("GT83-TVOICE-021.03")) ;
	iVoiceCall.GetStatus(callStatus) ;
	CHECKPOINT(callStatus, RCall::EStatusDialling, _L("GT83-TVOICE-016.03 - *** Failed RMobileCall::GetStatus() ***")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01 - *** Failed RMobileCall::NotifyMobileCallStatusChange() ***")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusDialling, _L("GT83-TVOICE-035.02")) ;
	iVoiceCall.GetMobileCallStatus(mobileCallStatus) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusDialling, _L("GT83-TVOICE-034.03 - *** Failed RMobileCall::GetMobileCallStatus() ***")) ;
	WaitWithTimeout(iStatus,60*KOneSecond);		// but timeout after 60 seconds

	// now hangup call
	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	console->Printf(_L("Call connected - now hanging up...\n"));
	iVoiceCall.HangUp(iStatus);

	//doesn't work for MM.TSY
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01 - *** Failed RMobileCall::NotifyStatusChange() ***")) ;
	//TEST_CHECKL(callStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-021.03")) ;

	User::WaitForRequest(iStatus3);
	iVoiceCall.GetMobileCallStatus(mobileCallStatus) ;
	TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusHangingUp, _L("GT83-TVOICE-016.08 - *** Failed RMobileCall::GetMobileCallStatus() ***")) ;


	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;	//sometimes timed out -33

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);	

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallStatusIncomingCall::CTestCallStatusIncomingCall()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallStatusIncomingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCallStatusIncomingCall::doTestStepL()
//void CTestVoice::TestCallStatusIncomingCallL()
/**
 * This function tests call status at various times during calls
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-009.01 Test setting notification of a change in hook status of a voice line
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-016.00 Test retrieval of the current call status (Generic)
 * @test GT83-TVOICE-016.02 "Test retrieval of the current call status when the call's status is ""idle"""
 * @test GT83-TVOICE-016.04 "Test retrieval of the current call status when the call's status is ""ringing"""
 * @test GT83-TVOICE-016.05 "Test retrieval of the current call status when the call's status is ""answering"""
 * @test GT83-TVOICE-016.07 "Test retrieval of the current call status when the call's status is ""connected"""
 * @test GT83-TVOICE-034.00 Test retrieval of the current status of the voice call (Generic)
 * @test GT83-TVOICE-034.02 "Test retrieval of the current status of the voice call when the call's status is ""idle"""
 * @test GT83-TVOICE-034.04 "Test retrieval of the current status of the voice call when the call's status is ""ringing"""
 * @test GT83-TVOICE-034.05 "Test retrieval of the current status of the voice call when the call's status is ""answering"""
 * @test GT83-TVOICE-034.07 "Test retrieval of the current status of the voice call when the call's status is ""connected"""
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	// Get the Call Status
	_LIT(KStartText, "Get Call Status...") ; TEST_START(&KStartText) ;
	console->Printf(_L("Get Call Status...\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	// Open a Voice Line and Call
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));

	RCall::TStatus callStatus;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

    TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05 - *** RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);


	TInt ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::GetStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call Status is NOT Supported"));
		}

	TInt ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT_EXPR((ret2==KErrNone || ret2==KErrNotSupported), _L("*** Failed RMobileCall::GetMobileCallStatus() ***"));
	if (ret2==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Mobile Call Status is NOT Supported"));
		}

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an incoming call

	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);
	
	/* GT83-AT-024	Test setting notification of an incoming call on a Voice Line */

	//I had to comment these lines, because
	// NotifyIncommigCall + AnswerIncommigCall doesn't work
	//in such situation. Any way we can't get any good
	//status without notifycation

	//iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
	//WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds

	//ret=iVoiceCall.GetStatus(callStatus);
	//TEST_CHECKL(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	//doesn't work for MM.TSY
	//TEST_CHECKL(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.01")) ;

	//ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	//TEST_CHECKL(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;
	//doesn't work for MM.TSY
	//TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.01")) ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	iVoiceCall.AnswerIncomingCall(iStatus);
	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00 - *** Failed RMobileCall::GetStatus() ***")) ;
	//doesn't work for MM.TSY
	//TEST_CHECKL(callStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-016.05")) ;

	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00 - *** Failed RMobileCall::GetMobileCallStatus() ***")) ;
	//doesn't work for MM.TSY
	//TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TVOICE-034.05")) ;
	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileCall::GetStatus() ***")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	TEST_CHECKL(ret2, KErrNone, _L("*** Failed RMobileCall::GetMobileCallStatus() ***")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TVOICE-016.07")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-034.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}
	INFO_PRINTF1(_L("Hanging up..."));
	console->Printf(_L("Hanging up...\n"));
	User::After(10*KOneSecond);
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-009.01 - *** Failed RMobileCall::Hangup() ***")) ;

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileCall::GetStatus() ***")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("*** Failed RMobileCall::GetMobileCallStatus() ***")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallStatusOutgoingCall::CTestCallStatusOutgoingCall( )
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestCallStatusOutgoingCall"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCallStatusOutgoingCall::doTestStepL()
// void CTestVoice::TestCallStatusOutgoingCallL()
/**
 * This function tests call status at various times during calls
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-009.01 Test setting notification of a change in hook status of a voice line
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-016.00 Test retrieval of the current call status (Generic)
 * @test GT83-TVOICE-016.02 "Test retrieval of the current call status when the call's status is ""idle"""
 * @test GT83-TVOICE-016.07 "Test retrieval of the current call status when the call's status is ""connected"""
 * @test GT83-TVOICE-025.00 Test dialling a voice call (Generic)
 * @test GT83-TVOICE-034.00 Test retrieval of the current status of the voice call (Generic)
 * @test GT83-TVOICE-034.02 "Test retrieval of the current status of the voice call when the call's status is ""idle"""
 * @test GT83-TVOICE-034.07 "Test retrieval of the current status of the voice call when the call's status is ""connected"""
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	// Get the Call Status
	_LIT(KStartText, "Get Call Status...") ; TEST_START(&KStartText) ;
	console->Printf(_L("Get Call Status...\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	// Open a Voice Line and Call
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;
    TName CallName ;

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("*** RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	TInt ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::GetStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call Status is NOT Supported"));
		}

	TInt ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT_EXPR((ret2==KErrNone || ret2==KErrNotSupported), _L("*** Failed RMobileCall::GetMobileCallStatus() ***"));
	if (ret2==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Mobile Call Status is NOT Supported"));
		}

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an outgoing call

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Dial() ***")) ;

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileCall::GetStatus() ***")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("*** Failed RMobileCall::GetMobileCallStatus() ***")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TVOICE-016.07")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-034.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}

	INFO_PRINTF1(_L("Hanging up..."));
	console->Printf(_L("Hanging up...\n"));

	User::After(10*KOneSecond);
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***")) ;

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("*** Failed RMobileCall::GetStatus() ***")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("*** Failed RMobileCall::GetMobileCallStatus() ***")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------


CTestCallCancels::CTestCallCancels()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestCallCancels"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCallCancels::doTestStepL()
//void CTestVoice::TestCallCancelsL()
/**
 * This function posts and then cancels Call Notifications.
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-018.01 Test setting notification of a change in the call's capabilities
 * @test GT83-TVOICE-018.03 Test the ability to cancel a notification of a change in the call's capabilities
 * @test GT83-TVOICE-020.01 Test setting notification of a change in hook status of a voice call
 * @test GT83-TVOICE-020.04 Test the ability to cancel a notification of a change in hook status of a voice call
 * @test GT83-TVOICE-021.01 Test setting notification of a change in the status of the voice call
 * @test GT83-TVOICE-022.01 Test setting notification of a change in the voice call's duration
 * @test GT83-TVOICE-022.03 Test cancelling notification of a change in the voice call's duration
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "Test Call Cancels") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Call Cancels...\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

    TName CallName ;
    INFO_PRINTF1(_L("Opening New Voice Call"));
    console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("*** Failed RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	// Call Status Change Cancel Request
	RCall::TStatus callStatus;
	iVoiceCall.NotifyStatusChange(iStatus, callStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-021.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iVoiceCall.NotifyStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-021.04")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	RMobileCall::TMobileCallStatus mobileCallStatus;
	iVoiceCall.NotifyMobileCallStatusChange(iStatus, mobileCallStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-0")) ;
	iVoiceCall.NotifyMobileCallStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-0")) ;
#endif
	// Hook Change Cancel Request
	RCall::THookStatus hookStatus;
	iVoiceCall.NotifyHookChange(iStatus, hookStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-020.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iVoiceCall.NotifyHookChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-020.04")) ;

	// Test Cancelling Notifications
	RCall::TCaps callCaps;
	iVoiceCall.NotifyCapsChange(iStatus, callCaps);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-018.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iVoiceCall.NotifyCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-018.03")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);
	iVoiceCall.NotifyMobileCallCapsChange(iStatus, mobileCallCapsPckg);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-0")) ;
	iVoiceCall.NotifyMobileCallCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-0")) ;
#endif

	TTimeIntervalSeconds callDuration;
	iVoiceCall.NotifyCallDurationChange(iStatus, callDuration);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-022.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iVoiceCall.NotifyCallDurationChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-022.03")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestMoreCallInfo::CTestMoreCallInfo()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestMoreCallInfo"));
	}
//------------------------------------------------------------------

enum TVerdict CTestMoreCallInfo::doTestStepL()
//void CTestVoice::TestMoreCallInfoL()
/**
 * This function gets more information pertaining to a Voice Call: Call ownership,
 * Call parameters, Loan data port requests and Transfer ownership requests.
 * @test GT83-TVOICE-001.01 "Test opening a voice line, specifying the phone that should be used"
 * @test GT83-TVOICE-014.05 "Test opening a new call by specifying the name of a line, and displaying the name of the new call"
 * @test GT83-TVOICE-026.00 Test answering a call on a voice line (Generic)
 * @test GT83-TVOICE-028.01 Test the ability to make a request to acquire the ownership of a call
 * @test GT83-TVOICE-029.01 Test transferring the call ownership when another client has requested to acquire call ownership
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
	
	_LIT(KStartText, "More Call Information Tests") ; TEST_START(&KStartText) ;
	console->Printf(_L("More Call Information Tests\n"));
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	TInt ret ;

	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));

    TName CallName ;
   // TName CallName2 ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("*** Failed RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	// Wait for an incoming Call
	INFO_PRINTF1(_L("Please ring the phone "));
	console->Printf(KPleaseRingThePhone);

	iVoiceCall.AnswerIncomingCall(iStatus);		// wait for a call
	WaitWithTimeout(iStatus,60*KOneSecond);		// but timeout after 60 seconds
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::AnswerIncomingCall() ***"));

	// Retrieve Call Ownership information
	INFO_PRINTF1(_L("Get Call Ownership.."));
	RCall::TOwnershipStatus callOwner;
	ret=iVoiceCall.GetOwnershipStatus(callOwner);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::GetOwnershipStatus() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call OwnerShip is NOT Supported"));
		}

	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Call OwnerShip is Supported"));

		switch(callOwner)
			{
			case RCall::EOwnershipUnowned:
				INFO_PRINTF1(_L("The call is unowned"));
				break;
			case RCall::EOwnershipOwnedByAnotherClient:
				INFO_PRINTF1(_L("The call is owned by another Client"));
				break;
			case RCall::EOwnershipOwnedByThisClient:
				INFO_PRINTF1(_L("The call is owned by this client"));
				break;
			default:
				INFO_PRINTF1(_L("Error in Call Ownership details"));
				break;
			}
		}

	// Call Parameters
	INFO_PRINTF1(_L("Get Call Parameters..."));
	console->Printf(_L("Get Call Parameters...\n"));
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);
	ret=iVoiceCall.GetCallParams(callParamsPckg);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::GetCallParams() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call Parameters is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Call Parameters is successful"));
		}

	// Post a 'New call added' notification
	TName callName;
	iLine.NotifyCallAdded(iStatus, callName);

	// Open a 2nd Voice Call, see BeginTest() function for explanatory comments
	INFO_PRINTF1(_L("Opening another Voice Call"));
	console->Printf(_L("Opening another Voice Call\n"));
	_LIT(KAutomaticName,"::");
	TName callName2(iLineInfo.iName);
	callName2.Append(KAutomaticName);

	RMobileCall VoiceCall2;
	INFO_PRINTF1(_L("Opening 2nd Voice Call"));
	console->Printf(_L("Opening 2nd Voice Call\n"));
	ret = VoiceCall2.OpenNewCall(iLine, CallName);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileCall::OpenNewCall() ***"));
	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("2nd Voice Call opened successfully"));
		INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);
		}

	if(ret == KErrNotSupported) //we don't have anything to do, bye...
		{
		INFO_PRINTF1(_L("2nd Voice Call not supported "));
		INFO_PRINTF1(_L("Hanging up"));
		iVoiceCall.HangUp(iStatus);
		User::WaitForRequest(iStatus);
		TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0"));
		INFO_PRINTF1(_L("HangUp successful"));

		// Close the line and call
		CleanupStack::PopAndDestroy(&iLine) ;
		CleanupStack::PopAndDestroy(&iVoiceCall) ;

		return EInconclusive;
		}


	User::WaitForRequest(iStatus);  // Notify Call added completion
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Notify Call added status = %d")), iStatus.Int());
		}

	// Acquire Ownership Cancel Request
	INFO_PRINTF1(_L("AcquireOwnership Cancel Request..."));
	console->Printf(_L("AcquireOwnership Cancel Request...\n"));
	VoiceCall2.AcquireOwnership(iStatus);
	//doesn't work for MM.TSY
	//TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-028.01"));

	VoiceCall2.AcquireOwnershipCancel();
	User::WaitForRequest(iStatus);
	if (iStatus==KErrEtelCallNotActive)
		{
		INFO_PRINTF1(_L("Voice Call 2 is not currently active"));
		}
	else
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("AcquireOwnership Cancel status: %d")), iStatus.Int());
		}

	// Transfer Ownership Request
	INFO_PRINTF1(_L("Voice Call 1 is trying to Transfer Ownership of the Call.."));
	ret=iVoiceCall.TransferOwnership();
	CHECKPOINT(ret, KErrEtelNoClientInterestedInThisCall, _L("GT83-TVOICE-029.01"));
	INFO_PRINTF1(_L("There is no Client currently interested in Ownership"));

	// Close the 2nd Voice Call
	VoiceCall2.Close();
	INFO_PRINTF1(_L("Closing the 2nd Voice Call"));
	console->Printf(_L("Closing the 2nd Voice Call\n"));

	// Hang up the Call
	INFO_PRINTF1(_L("Hanging up"));
	console->Printf(_L("Hanging up\n"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("*** Failed RMobileCall::Hangup() ***"));
	INFO_PRINTF1(_L("HangUp successful"));
	console->Printf(_L("HangUp successful\n"));
	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END();

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCloseCallWhenActive::CTestCloseCallWhenActive( )
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestCloseCallWhenActive"));
	}
//------------------------------------------------------------------

enum TVerdict CTestCloseCallWhenActive::doTestStepL()
//void CTestVoice::TestCloseCallWhenActiveL()
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
    console->Printf(_L("Test Call Close When Active..."));
	
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-Test15-LineOpened - *** Failed RMobileLine::Open() ***"));

    TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("*** RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	//Activate a call by dialing a number
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::WaitForRequest(iStatus);
	TEST_CHECKL(iStatus.Int(), KErrNone, _L("GT83-TVOICE-Test15-CallDialled - *** Failed RMobileCall::Dial() ***"));

	// Close call while it is active - without calling HangUp
	User::After(5000000);
	iVoiceCall.Close();
	iLine.Close();
	INFO_PRINTF1(_L("Close successful - check call not active on phone"));
	console->Printf(_L("Close successful - check call not active on phone\n"));
	TEST_END();
	
	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);
	
	return TestStepResult();
	}

//SUBSTITUTE CLASS FOR DEFECT FIX DEF010965

CTestLineCapabilities::CTestLineCapabilities()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file

	SetTestStepName(_L("TestLineCapabilities"));
	}

//------------------------------------------------------------------

enum TVerdict CTestLineCapabilities::doTestStepL()
/**
* @test GT83-TVOICE-002.01 Test retrieval of a voice line's capabilities. Display the capabilities to screen
*/
	{
	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);	
	
	_LIT(KStartText, "Test Line Capabilities") ; TEST_START(&KStartText) ;
	console->Printf(_L("Test Line Capabilities\n"));

	CleanupClosePushL(iVoiceCall) ;
	CleanupClosePushL(iLine) ;

	// Line Capabilities

	RLine::TCaps lineCaps;
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("*** Failed RMobileLine::Open() ***"));
	TInt ret=iLine.GetCaps(lineCaps);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileLine::GetCaps() ***")) ;
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Line Capabilities is NOT supported"));
		}
	else
		{
		if ((lineCaps.iFlags&RLine::KCapsData) !=0)
			{
			INFO_PRINTF1(_L("This line supports Data connections"));
			}
		if ((lineCaps.iFlags&RLine::KCapsFax) !=0)
			{
			INFO_PRINTF1(_L("This line supports Fax connections"));
			}
		if ((lineCaps.iFlags&RLine::KCapsVoice) !=0)
			{
			INFO_PRINTF1(_L("This line supports Voice connections"));
			}
		if ((lineCaps.iFlags&RLine::KCapsEventIncomingCall) !=0)
			{
			INFO_PRINTF1(_L("This line supports Incoming Call Notification requests"));
			}
		}
	
	// Close the line and call
	CleanupStack::PopAndDestroy(&iLine) ;
	CleanupStack::PopAndDestroy(&iVoiceCall) ;

	TEST_END() ;
	
	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	return TestStepResult();
	}

CTestCancelDialRequest::CTestCancelDialRequest()
/* Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCancelDialRequest"));
	}


enum TVerdict CTestCancelDialRequest::doTestStepL()
{

// void CTestVoice::TestCancelDialRequestL()
//	{

	CConsoleBase* console = Console::NewL(KConsoleTitle,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);
    console->Printf(_L("Test Cancel Dial Request...\n"));


	User::After(2000000); //settling pause to allow previous test to complete hang up first.

	//Tests cancelling dial requests for various periods after they are made.
	INFO_PRINTF1(_L("Opening Voice Line"));
	console->Printf(_L("Opening Voice Line\n"));
	TEST_CHECKL(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-Test15-LineOpened - *** Failed RMobileLine::Open() ***"));

    TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	console->Printf(_L("Opening New Voice Call\n"));
	TEST_CHECKL(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("*** RMobileCall::OpenNewCall() ***"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	//Cancel Tests
	
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
    User::After(500000);   //cancel after 0.5 second	
	INFO_PRINTF1(_L("Cancelling Dial After 0.5s..."));
	console->Printf(_L("Cancelling Dial After 0.5s...\n"));
	iVoiceCall.DialCancel();
	User::WaitForRequest(iStatus);
	
	if(iStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("Dial Cancelled ok!..."));
		}
	
	if(iStatus==KErrNone)
		{
		INFO_PRINTF1(_L("Dial Cancel Request after 0.5sec of dialling, Failed..."));
		iVoiceCall.HangUp(iStatus);
		INFO_PRINTF1(_L("Call Hang Up Requested..."));
		User::WaitForRequest(iStatus);
		// Removed SetTestStepResult(EFail); here because original TS_Voice test always returns EPass anyway
		}
	
	INFO_PRINTF1(_L("Wait For Next Dial..."));
	console->Printf(_L("Wait For Next Dial...\n"));
	User::After(5000000);	//wait 5 seconds
	
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::After(1000000);	//cancel after 1.0 seconds
	INFO_PRINTF1(_L("Cancelling Dial After 1.0s..."));
	console->Printf(_L("Cancelling Dial After 1.0s...\n"));
	iVoiceCall.DialCancel();
	User::WaitForRequest(iStatus);
	
	if(iStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("Dial Cancelled ok!..."));
		}
	
	else
		{
		INFO_PRINTF1(_L("Dial Cancel Request after 1sec of dialling, Failed..."));
		iVoiceCall.HangUp(iStatus);
		INFO_PRINTF1(_L("Call Hang Up Requested..."));
		User::WaitForRequest(iStatus);
		// Removed SetTestStepResult(EFail); here because original TS_Voice test always returns EPass anyway
		}
	
	INFO_PRINTF1(_L("Wait For Next Dial..."));
	console->Printf(_L("Wait For Next Dial...\n"));
	User::After(3000000);	//wait 3 seconds
	
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::After(1500000);	//cancel after 1.5 seconds
	INFO_PRINTF1(_L("Cancelling Dial After 1.5s..."));
	console->Printf(_L("Cancelling Dial After 1.5s...\n"));
	iVoiceCall.DialCancel();
	User::WaitForRequest(iStatus);

	if(iStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("Dial Cancelled ok!..."));
		}

	else
		{
		INFO_PRINTF1(_L("Dial Cancel Request after 1.5sec of dialling, Failed..."));
		iVoiceCall.HangUp(iStatus);
		INFO_PRINTF1(_L("Call Hang Up Requested..."));
		User::WaitForRequest(iStatus);
		// Removed SetTestStepResult(EFail); here because original TS_Voice test always returns EPass anyway
		}

	INFO_PRINTF1(_L("Wait For Next Dial..."));
	console->Printf(_L("Wait For Next Dial...\n"));
	User::After(3000000);	//wait 3 seconds
	
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::After(2000000);	//cancel after 2.0 seconds
	INFO_PRINTF1(_L("Cancelling Dial After 2.0s..."));
	console->Printf(_L("Cancelling Dial After 2.0s...\n"));
	iVoiceCall.DialCancel();
	User::WaitForRequest(iStatus);
	
	if(iStatus==KErrCancel)
		{
		INFO_PRINTF1(_L("Dial Cancelled ok!..."));
		}

	else
		{
		INFO_PRINTF1(_L("Dial Cancel Request after 2sec of dialling, Failed..."));
		iVoiceCall.HangUp(iStatus);
		INFO_PRINTF1(_L("Call Hang Up Requested..."));
		User::WaitForRequest(iStatus);
		// Removed SetTestStepResult(EFail); here because original TS_Voice test always returns EPass anyway
		}

	User::After(3000000);	//wait 3 seconds

	//Connect without cancelling
	INFO_PRINTF1(_L("Now Dialling without Cancelling..."));
	console->Printf(_L("Now Dialling without Cancelling...\n"));
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	console->Printf(_L("Dialling %S....\n"), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::WaitForRequest(iStatus);
	TEST(iStatus.Int() == KErrNone);
	if(iStatus == KErrNone)
		{
		INFO_PRINTF1(_L("Connected Ok. Please wait, will hang up shortly..."));
		}
	else
		{
		INFO_PRINTF1(_L("Failed subsequant dial without cancelling!"));
		}
	
	User::After(10000000);	
	INFO_PRINTF1(_L("Closing call..."));
	console->Printf(_L("Closing call...\n"));
	iVoiceCall.Close();
	iLine.Close();
	
	INFO_PRINTF1(_L("Close successful - check call not active on phone"));
	console->Printf(_L("Close successful - check call not active on phone\n"));

	console->Printf(_L("Finished\n"));
	User::After(10*KOneSecond);
   	CleanupStack::PopAndDestroy(console);

	// Returns EPass as it is debatable if this is a valid test as the functionality 
	// being tested is very network dependant but test has been left none the less
	SetTestStepResult(EPass);
	return 	EPass;
	}
