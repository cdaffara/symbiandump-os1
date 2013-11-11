// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
*/

#include "Te_SimVoice.h"

const TInt KTenPartOfSec = 100000 ;
const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)
const TInt KTenSeconds=10000000;	// Used in a time out function, 10 seconds (in microSeconds)
 
//------------------------------------------------------------------

CTestLineStatusIncommingCall::CTestLineStatusIncommingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLineStatusIncommingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestLineStatusIncommingCall::doTestStepL()
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
	INFO_PRINTF1(_L("Test Line Status"));

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	TName callName;

	RCall::TStatus lineStatus;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality

	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	TInt ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-031.01")) ;

	INFO_PRINTF1(_L("Please ring the phone"));

	/* @test GT83-AT-024	Test setting notification of an incoming call on a Voice Line */
	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
	TESTL(CallMeDear() == KErrNone) ;		// AMC - Added
	WaitWithTimeout(iStatus,15*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.02")) ;
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is ringing*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TVOICE-004.04")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TVOICE-030.003")) ;

	INFO_PRINTF1(_L("Phone is ringing..."));
	//Test for incomming Voice Call
	TestLineCapabilitiesL();

	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	ret = iVoiceCall.OpenExistingCall(iLine, callName);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-015.05")) ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 2 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.04")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-030.03")) ;
	TestLineCapabilitiesL();

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-030.07")) ;


	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Hanging Up"));
	iVoiceCall.HangUp(iStatus);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is hang up*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-004.08")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-034.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TVOICE-034.09")) ;

	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0AAA")) ;  //???  line 825

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-030.02")) ;

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.02")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestLineNotificationsIncommingCall::doTestStepL()
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
	INFO_PRINTF1(_L("Test Line Notifications"));
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TName callName1;
	TName callName2;
	TInt ret = KErrNone;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	INFO_PRINTF1(_L("Please ring the phone"));

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

	TESTL(CallMeDear() == KErrNone)  ; //init sim incoming call

	WaitWithTimeout(iStatus,15*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.02")) ;

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

	/* @test GT83-AT-026	Test notification of a change in the status of the Voice Line from
	 "idle" to "ringing" to "answering" to "connected" to "hanging up" to "idle"
	 */
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	ret = iVoiceCall.OpenExistingCall(iLine, callName1);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-015.05")) ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);

    // GT83-AT-026	Test notification of a change in the status of the Voice Line
	// from "ringing" to "answering"
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.01")) ;
	CHECKPOINT(lineStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-010.01")) ;

	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 2 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;	  //wrong code -1632960000


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
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0AAA")) ;

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
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallNotificationsIncommingCall::CTestCallNotificationsIncommingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallNotificationsIncommingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestCallNotificationsIncommingCall::doTestStepL()
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
	// Post call status change notifications
	TName callName1;
	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	RCall::TCaps caps;
	RCall::THookStatus hookStatus;
	TTimeIntervalSeconds time;

	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);

	INFO_PRINTF1(_L("Test Call Notifications"));
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	INFO_PRINTF1(_L("Please ring the phone"));

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-008.01")) ;


    TESTL(CallMeDear() == KErrNone)  ; //init sim incoming call


	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.01")) ;

	TInt ret = iVoiceCall.OpenExistingCall(iLine, callName1);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-015.05")) ;

	iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(callStatus, RCall::EStatusRinging, _L("GT83-TVOICE-016.04")) ;

	iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusRinging, _L("GT83-TVOICE-034.04")) ;

	ret=iVoiceCall.GetCaps(caps);
	Print_TCapsEnum(caps) ;
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-002.00")) ;
	TESTL((caps.iFlags & (RCall::KCapsVoice | RCall::KCapsAnswer)));

//	INFO_PRINTF1(_L("Opening New Voice Call"));
//	CHECKPOINT(iiVoiceCall.OpenExistingCall(iLine, callName2), KErrNone, _L("GT83-TVOICE-013.06")) ;

	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
#ifndef SIM_TSY
	iVoiceCall.NotifyCapsChange(iStatus4, caps);
#endif
	iVoiceCall.NotifyHookChange(iStatus5, hookStatus);
	iVoiceCall.NotifyCallDurationChange(iStatus6, time) ;
	iVoiceCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);

	//doesn't work for MM.TSY iVoiceCall.GetStatus(callStatus);
	//CHECKPOINT(callStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-016.05")) ;

	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 2 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;


	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;
	CHECKPOINT(callStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-021.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TVOICE-035.10")) ;

	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-035.11")) ;

#ifndef SIM_TSY
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-018.01")) ;
	Print_TCapsEnum(caps) ;
	CHECKPOINT(caps.iFlags, RCall::KCapsVoice, _L("GT83-TVOICE-018.02")) ;

	iVoiceCall.NotifyCapsChange(iStatus4, caps);
	User::WaitForRequest(iStatus4);
	CHECKPOINT(iStatus4.Int(), KErrNone, _L("GT83-TVOICE-018.01")) ;
	CHECKPOINT(caps.iFlags, (RCall::KCapsVoice | RCall::KCapsHangUp), _L("GT83-TVOICE-018.01")) ;
#endif

	User::WaitForRequest(iStatus5);
	CHECKPOINT(iStatus5.Int(), KErrNone, _L("GT83-TVOICE-020.01")) ;
	CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TVOICE-020.03")) ;

	User::WaitForRequest(iStatus6);

#ifndef SIM_TSY
	CHECKPOINT(iStatus6.Int(), KErrNone, _L("GT83-TVOICE-022.01")) ;
#endif

	if (iStatus6==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Notify Call Duration Change is NOT Supported"));
		}
	if (iStatus6==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), time.Int());
		}

	User::WaitForRequest(iStatus7);
#ifndef SIM_TSY
	CHECKPOINT(iStatus7.Int(), KErrNone, _L("GT83-TVOICE-033.01")) ;
	CHECKPOINT(mobileCallCaps.iCallControlCaps, RCall::KCapsVoice, _L("GT83-TVOICE-033.02")) ;
#endif

#ifndef SIM_TSY //MAY hang the app.
	iVoiceCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);
	User::WaitForRequest(iStatus7);
	CHECKPOINT(iStatus7.Int(), KErrNone, _L("GT83-TVOICE-033.01")) ;
	CHECKPOINT(mobileCallCaps.iCallControlCaps, (RCall::KCapsVoice | RCall::KCapsHangUp), _L("GT83-TVOICE-033.02")) ;
#endif

	// now hangup call
	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-020.01")) ;	//sometimes timed out -33

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;
	CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TVOICE-021.02")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.07")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TVOICE-035.07")) ;

	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;
	CHECKPOINT(callStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-021.02")) ;

	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-035.08")) ;

	TESTL(iStatus == KErrNone) ;
	TESTL(iStatus2 == KErrNone) ;
	TESTL(iStatus3 == KErrNone) ;
	TESTL(iStatus4 == KErrNone) ;
	TESTL(iStatus5 == KErrNone) ;
#ifndef SIM_TSY
	TESTL(iStatus6 == KErrNone) ;
	TESTL(iStatus7 == KErrNone) ;
#endif

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestCallNotificationsOutgoingCall::CTestCallNotificationsOutgoingCall()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestCallNotificationsOutgoingCall"));
	}
//------------------------------------------------------------------

TVerdict CTestCallNotificationsOutgoingCall::doTestStepL()
/**
 @test GT83-TVOICE-016.08	Test retrieval of the current call status when the call's status is "hanging up"
*/
	{
	// Post call status change notifications
	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

    TName CallName ;

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	//TTimeIntervalSeconds time;
 	//RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	//RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	INFO_PRINTF1(_L("Test Call Notifications"));
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	// Now test call status during an outgoing call
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;
	CHECKPOINT(callStatus, RCall::EStatusDialling, _L("GT83-TVOICE-021.03")) ;
	iVoiceCall.GetStatus(callStatus) ;
	CHECKPOINT(callStatus, RCall::EStatusDialling, _L("GT83-TVOICE-016.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-035.01")) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusDialling, _L("GT83-TVOICE-035.02")) ;
	iVoiceCall.GetMobileCallStatus(mobileCallStatus) ;
	CHECKPOINT(mobileCallStatus, RMobileCall::EStatusDialling, _L("GT83-TVOICE-034.03")) ;

	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;

	// now hangup call
	iVoiceCall.NotifyStatusChange(iStatus2, callStatus);
	iVoiceCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	iVoiceCall.HangUp(iStatus);

	//doesn't work for MM.TSY
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-021.01")) ;
	//CHECKPOINT(callStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-021.03")) ;

	User::WaitForRequest(iStatus3);
	iVoiceCall.GetMobileCallStatus(mobileCallStatus) ;
	TEST_CHECKL(mobileCallStatus, RMobileCall::EStatusHangingUp, _L("GT83-TVOICE-016.08")) ;


	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-020.01")) ;	//sometimes timed out -33

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestLineNotificationsOutgoingCall::doTestStepL()
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
	INFO_PRINTF1(_L("Test Line Notifications"));
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TName callName1;
	//TName callName2;
	//TInt ret = KErrNone;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, callName1), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName1);

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	// Now test call status during an outgoing call
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
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

#ifdef SIM_TSY //It has connecting status
	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnecting, _L("GT83-TVOICE-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.03")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnecting, _L("GT83-TVOICE-031.03")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
#endif

	User::WaitForRequest(iStatus); //end of dial

	User::WaitForRequest(iStatus2);
	CHECKPOINT(iStatus2.Int(), KErrNone, _L("GT83-TVOICE-010.03")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECKPOINT(iStatus3.Int(), KErrNone, _L("GT83-TVOICE-031.04")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.04")) ;

	INFO_PRINTF1(_L("Call connected - hanging up..."));

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	User::After(10*KOneSecond);
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0AAA")) ;

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

	TESTL(iStatus == KErrNone) ;
	TESTL(iStatus2 == KErrNone) ;
	TESTL(iStatus3 == KErrNone) ;
	TESTL(iStatus4 == KErrNone) ;
	TESTL(iStatus5 == KErrNone) ;
#ifndef SIM_TSY
	TESTL(iStatus6 == KErrNone) ;
	TESTL(iStatus7 == KErrNone) ;
#endif
	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestLineStatusOutgoingCall::doTestStepL()
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
	INFO_PRINTF1(_L("Test Line Status"));

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	RCall::TStatus lineStatus;
    TName CallName ;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	TInt ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-031.01")) ;
	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);

	//doesn't work for MM.TSY
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	//CHECKPOINT(lineStatus, RCall::EStatusDialling, _L("GT83-TVOICE-004.03")) ;

	WaitWithTimeout(iStatus,15*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0AAA")) ;
	//Test for outgoing Voice Call
	TestLineCapabilitiesL();

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.07")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-030.07")) ;


	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Call connected - listen for 20 seconds..."));

	User::After(20*KOneSecond); // hold call active for 20 seconds

	INFO_PRINTF1(_L("Hanging up"));
	iVoiceCall.HangUp(iStatus);
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-004.08")) ;
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0AAA")) ;

	//Test for outgoing Voice Call after hang up
	TestLineCapabilitiesL();
	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusIdle, _L("GT83-TVOICE-004.02")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-004.01")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestCallStatusOutgoingCall::doTestStepL()
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
	// Get the Call Status
	INFO_PRINTF1(_L("Get Call Status..."));
	// Open a Voice Line and Call
	INFO_PRINTF1(_L("Opening Voice Line"));

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;
    TName CallName ;

	TBuf<16> GoodNumber ;

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	GetGoodNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);


	TInt ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	TInt ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;

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
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an outgoing call

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iVoiceCall.Dial(iStatus, GoodNumber);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-025.00")) ;

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TVOICE-016.07")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-034.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}

	INFO_PRINTF1(_L("Hanging up..."));

	User::After(15*KOneSecond);
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-009.01")) ;

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestCallStatusIncommingCall::doTestStepL()
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
	// Get the Call Status
	INFO_PRINTF1(_L("Get Call Status..."));
	// Open a Voice Line and Call
	INFO_PRINTF1(_L("Opening Voice Line"));
    TName CallName ;

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TBuf<16> GoodNumber ;

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	GetGoodNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);


	TInt ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	TInt ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;

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
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an incomming call

	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	INFO_PRINTF1(_L("Please ring the phone "));

    TESTL(CallMeDear() == KErrNone)  ; //init sim incoming call
	//User::After(1000000L);


	/* GT83-AT-024	Test setting notification of an incoming call on a Voice Line */

	//I had to comment these lines, because
	// NotifyIncommigCall + AnswerIncommigCall doesn't work
	//in such situation. Any way we can't get any good
	//status without notifycation

	//iLine.NotifyIncomingCall(iStatus, CallName);	// wait for a call
	//WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds

	//ret=iVoiceCall.GetStatus(callStatus);
	//CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	//doesn't work for MM.TSY
	//CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.01")) ;

	//ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	//CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;
	//doesn't work for MM.TSY
	//CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.01")) ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	iVoiceCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,15*KOneSecond);				// but timeout after 7 seconds

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	//doesn't work for MM.TSY
	//CHECKPOINT(callStatus, RCall::EStatusAnswering, _L("GT83-TVOICE-016.05")) ;

	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;
	//doesn't work for MM.TSY
	//CHECKPOINT(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TVOICE-034.05")) ;
	
	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECKPOINT(callStatus, RCall::EStatusConnected, _L("GT83-TVOICE-016.07")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-034.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}
	INFO_PRINTF1(_L("Hanging up..."));

	User::After(15*KOneSecond);
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-009.01")) ;

	ret=iVoiceCall.GetStatus(callStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-016.00")) ;
	ret2=iVoiceCall.GetMobileCallStatus(mobileCallStatus);
	CHECKPOINT(ret2, KErrNone, _L("GT83-TVOICE-034.00")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TVOICE-016.02")) ;
		CHECKPOINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TVOICE-034.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestLineInfo::doTestStepL()
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
 */
	{
	INFO_PRINTF1(_L("Test Line Information"));
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	TInt ret ;
	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));
    TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	// Line Information
	RLine::TLineInfo lineInfo;
	TRAP(ret, ret=iLine.GetInfo(lineInfo));
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-012.01"));
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Get Line Info. is NOT supported"));
		}
	else
		{
		INFO_PRINTF1(_L("Get Line Info is successful"));
		CHECKPOINT(lineInfo.iHookStatus,  RCall::EHookStatusOn, _L("GT83-TVOICE-0AAA")) ;
		TESTL(lineInfo.iStatus == RCall::EStatusIdle) ;
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Name of last call added: %S")), &lineInfo.iNameOfLastCallAdded);
		}

	// Hook status information
	INFO_PRINTF1(_L("Retrieving Hook Status..."));
	RCall::THookStatus hookStatus;
	ret=iLine.GetHookStatus(hookStatus);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Get Hook status is NOT supported"));
		}
	else
		{
		CHECKPOINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TVOICE-005.02")) ;
		INFO_PRINTF1(_L("The line is currently ON Hook"));
		}

	INFO_PRINTF1(_L("Please ring the phone"));

    TESTL(CallMeDear() == KErrNone) ; //init sim incoming call


	// Answer an incoming call initiated  and obtain the call's current status
	iVoiceCall.AnswerIncomingCall(iStatus);

	WaitWithTimeout(iStatus,2*KTenSeconds);		// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;

	// Count the no. of calls on the line & Get call info.
	INFO_PRINTF1(_L("Retrieving no. of calls opened from the line.."));
	TInt lineCount=0;
	iLine.EnumerateCall(lineCount);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("No of calls opened = %d")), lineCount);

	//doesn't work for MM.TSY
	INFO_PRINTF1(_L("Getting Call info.."));
	RLine::TCallInfo callInfo;
	ret=iLine.GetCallInfo(0,callInfo);
	//doesn't work for MM.TSY
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-007.01"));

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
		CHECKPOINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TVOICE-005.01")) ;
		INFO_PRINTF1(_L("The line is currently OFF Hook"));
		}

	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(KTenSeconds);

	INFO_PRINTF1(_L("Hanging up"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-027.02")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestCallCancels::doTestStepL()
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
	INFO_PRINTF1(_L("Test Call Cancels"));
	// Open a Voice Line and Call
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

    TName CallName ;
    INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
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
	iiVoiceCall.NotifyMobileCallStatusChange(iStatus, mobileCallStatus);
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-0AAA")) ;
	iiVoiceCall.NotifyMobileCallStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-0AAA")) ;
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
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-0AAA")) ;
	iVoiceCall.NotifyMobileCallCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-0AAA")) ;


	TTimeIntervalSeconds callDuration;
	iVoiceCall.NotifyCallDurationChange(iStatus, callDuration);
	CHECKPOINT((iStatus==KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TVOICE-022.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iVoiceCall.NotifyCallDurationChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNotSupported, _L("GT83-TVOICE-022.03")) ;		// AMC - the SIM TSY does not support duration notifications...
#endif

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestLineCancels::doTestStepL()
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
	INFO_PRINTF1(_L("Test Line Cancels"));

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	// Notify Incoming Call
	TName callName;
	//GT83-AT-021	Test opening a Voice Line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

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
	CHECKPOINT(iStatus.Int(), KRequestPending, _L("GT83-TVOICE-0AAA")) ;
	iLine.NotifyMobileLineStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrCancel, _L("GT83-TVOICE-0AAA")) ;
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
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestMoreCallInfo::CTestMoreCallInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMoreCallInfo"));
	}
//------------------------------------------------------------------

TVerdict CTestMoreCallInfo::doTestStepL()
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
	INFO_PRINTF1(_L("More Call Information Tests"));
	TInt ret ;
	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

    TName callName;
	RCall::TStatus lineStatus;

	// my cases I start

	//RCall::TStatus callStatus;	
	RMobileCall::TMobileCallStatus mobileCallStatus;	
	TRequestStatus reqStatus0;
	RMobileCall spareCall;
	CleanupClosePushL(spareCall);

	// case 1 start
	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
	TESTL(CallMeDear() == KErrNone) ;		
	WaitWithTimeout(iStatus,15*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.02")) ;
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);


	ret = spareCall.OpenExistingCall(iLine, callName);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-015.05")) ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	spareCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 2 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.04")) ;
	spareCall.HangUp(iStatus);
	User::WaitForRequest(iStatus); 
	CleanupStack::Pop(&spareCall);
	spareCall.Close();
	// case 1 end

	// case 2 start
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	RMobileCall voiceCall4;
	CHECKPOINT(ret=voiceCall4.OpenNewCall(iLine, callName), KErrNone, _L("GT83-TVOICE-014.05"));
	voiceCall4.Dial(iStatus,GoodNumber);
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 7 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));	
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.04")) ;


	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call

	TESTL(CallMeDear() == KErrNone); 		// AMC - Added
	WaitWithTimeout(iStatus,15*KOneSecond);				// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-008.02")) ;
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TVOICE-004.04")) ;

	ret = spareCall.OpenExistingCall(iLine, callName);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-015.05")) ;
	
	CleanupClosePushL(spareCall);
	
	voiceCall4.Hold(iStatus);
	User::WaitForRequest(iStatus);
	TEST(iStatus==KErrNone);
	TEST(voiceCall4.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);

	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusRinging, _L("GT83-TVOICE-004.04")) ;
	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	spareCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 7 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;
	ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-004.00")) ;
	CHECKPOINT(lineStatus, RCall::EStatusConnected, _L("GT83-TVOICE-004.04")) ;
	spareCall.HangUp(iStatus);
	User::WaitForRequest(iStatus); 
	CleanupStack::Pop(&spareCall);
	spareCall.Close();
	voiceCall4.Close();
	// case 2 end
	// my cases I end

	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, callName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);

	// Wait for an incoming Call
	INFO_PRINTF1(_L("Please ring the phone"));

    iVoiceCall.AnswerIncomingCall(iStatus);		// set as an answering phone
	TESTL(CallMeDear() == KErrNone)  ;          // init sim incoming call 
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 15 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));

	// Retrieve Call Ownership information
	INFO_PRINTF1(_L("Get Call Ownership.."));
	RCall::TOwnershipStatus callOwner;
	ret=iVoiceCall.GetOwnershipStatus(callOwner);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-026.01"));

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
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);
	ret=iVoiceCall.GetCallParams(callParamsPckg);
#ifndef SIM_TSY
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-0AAA"));
#endif
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Call Parameters is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Call Parameters is successful"));
		}
	
	// Post a 'New call added' notification
	iLine.NotifyCallAdded(iStatus, callName);

	// Open a 2nd Voice Call, see BeginTest() function for explanatory comments
	INFO_PRINTF1(_L("Opening another Voice Call"));
	
	RMobileCall voiceCall2;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(ret=voiceCall2.OpenNewCall(iLine, callName), KErrNone, _L("GT83-TVOICE-014.05"));
	CleanupClosePushL(voiceCall2);
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);

	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("2nd Voice Call opened successfully"));
		}

	if(ret == KErrNotSupported) //we don't have anything to do, bye...
		{
		User::WaitForRequest(iStatus);  // Notify Call added completion


		INFO_PRINTF1(_L("2nd Voice Call are not supported"));
		INFO_PRINTF1(_L("Hanging up"));
		iVoiceCall.HangUp(iStatus);
		User::WaitForRequest(iStatus);

		CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-0AAA"));
		INFO_PRINTF1(_L("HangUp successful"));
		// Close the line and call
		CleanupStack::PopAndDestroy(&voiceCall2) ;
		CleanupStack::PopAndDestroy(&iVoiceCall);
		CleanupStack::PopAndDestroy(&iLine);
		return TestStepResult();
		}

	User::WaitForRequest(iStatus);  // Notify Call added completion
	if (iStatus==KErrNone)
		{
		INFO_PRINTF2(TRefByValue<const TDesC>(_L("Notify Call added status = %d")), iStatus.Int());
		}

	// my cases II start


	// 3rd case put 1st on hold, dial on other

	iVoiceCall.Hold(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);

	// Now test call status during an outgoing call
	
	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	voiceCall2.Dial(iStatus, GoodNumber);			
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 7 seconds
	TEST(voiceCall2.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	voiceCall2.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-020.01"));	


	iVoiceCall.Resume(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	// 4th case put 1st on hold, answer on other

	iVoiceCall.Hold(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusHold, _L("GT83-TVOICE-031.01")) ;


	voiceCall2.AnswerIncomingCall(iStatus);		// set as an answering phone
	TESTL(CallMeDear() == KErrNone)  ;          // init sim incoming call 
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 15 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.01")) ;
	// now hangup call

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	voiceCall2.HangUp(iStatus);
	User::WaitForRequest(iStatus);

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusHold, _L("GT83-TVOICE-031.01")) ;

	iVoiceCall.Resume(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.01")) ;


	// 5th case put 1st on hold, answer on other

	RCall::TCaps callCaps;
 	ret=iVoiceCall.GetCaps(callCaps);
 	CHECKPOINT(ret,KErrNone,_L("GT83-TVOICE-031.01"));
	TEST(callCaps.iFlags & RCall::KCapsVoice );
	TEST(callCaps.iFlags & RMobileCall::KCapsHold) ;
	TEST(callCaps.iFlags & RMobileCall::KCapsSwap) ;

	iVoiceCall.Hold(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);	


	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusHold, _L("GT83-TVOICE-031.01")) ;


	iVoiceCall.Swap(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.01")) ;

	iVoiceCall.Swap(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TEST(reqStatus0==KErrNone);
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);

	ret=iVoiceCall.GetCaps(callCaps);
	CHECKPOINT(ret,KErrNone,_L("GT83-TVOICE-031.01"));
	TEST(callCaps.iFlags & RCall::KCapsVoice );
	TEST(callCaps.iFlags & RMobileCall::KCapsSwap) ;
	TEST(callCaps.iFlags & RMobileCall::KCapsResume) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusHold, _L("GT83-TVOICE-031.01")) ;

	voiceCall2.AnswerIncomingCall(iStatus);		// set as an answering phone
	TESTL(CallMeDear() == KErrNone)  ;          // init sim incoming call 
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 15 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));
	TEST(voiceCall2.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	ret=voiceCall2.GetCaps(callCaps);
	CHECKPOINT(ret,KErrNone,_L("GT83-TVOICE-031.01"));
	TEST(callCaps.iFlags & RCall::KCapsVoice );
	TEST(callCaps.iFlags & RMobileCall::KCapsSwap) ;
	TEST(!(callCaps.iFlags & RMobileCall::KCapsHold)) ;

	ret=iVoiceCall.GetCaps(callCaps);
	CHECKPOINT(ret,KErrNone,_L("GT83-TVOICE-031.01"));
	TEST(callCaps.iFlags & RCall::KCapsVoice );
	TEST(callCaps.iFlags & RMobileCall::KCapsSwap) ;
	TEST(!(callCaps.iFlags & RMobileCall::KCapsResume)) ;
	
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.01")) ;


	voiceCall2.Swap(iStatus);
	User::WaitForRequest(iStatus);	
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));

	TEST(voiceCall2.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);
	
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-030.00")) ;
	CHECKPOINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TVOICE-031.01")) ;


	// try to activate call on hold whilst other is connected
	iVoiceCall.Swap(iStatus);
	User::WaitForRequest(iStatus);	
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));

	TEST(voiceCall2.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusConnected);

	
	TEST(iVoiceCall.GetMobileCallStatus(mobileCallStatus)==KErrNone);
	TEST(mobileCallStatus==RMobileCall::EStatusHold);

	
	// try to dial whilst have 2 active
	
	RMobileCall voiceCall3;
	CHECKPOINT(ret=voiceCall3.OpenNewCall(iLine, callName), KErrNone, _L("GT83-TVOICE-014.05"));
	CleanupClosePushL(voiceCall3);
	voiceCall3.Dial(iStatus,GoodNumber);
	User::WaitForRequest(iStatus);	
	CHECKPOINT(iStatus.Int(), KErrGeneral, _L("GT83-TVOICE-026.00"));
	voiceCall3.Close();
	CleanupStack::Pop(&voiceCall3);

	// try to activate call 
	iVoiceCall.Swap(iStatus);
	User::WaitForRequest(iStatus);	
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00"));

	// now hangup call

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	ret=iVoiceCall.GetCaps(callCaps);
	CHECKPOINT(ret,KErrNone,_L("GT83-TVOICE-031.01"));
	TEST(callCaps.iFlags & RCall::KCapsVoice );
	TEST(callCaps.iFlags & RMobileCall::KCapsSwap) ;
	TEST(!(callCaps.iFlags & RMobileCall::KCapsHold)) ;

	voiceCall2.HangUp(iStatus);
	User::WaitForRequest(iStatus);

	ret=iVoiceCall.GetCaps(callCaps);
	CHECKPOINT(ret,KErrNone,_L("GT83-TVOICE-031.01"));
	TEST(callCaps.iFlags & RCall::KCapsVoice );
	TEST(callCaps.iFlags & RMobileCall::KCapsSwap) ;
	TEST(callCaps.iFlags & RMobileCall::KCapsHold) ;
		

	//Close the 2nd Voice Call
	voiceCall2.Close();
	CleanupStack::Pop(&voiceCall2) ;
	INFO_PRINTF1(_L("Closing the 2nd Voice Call"));

	// my II cases end	

	// Hang up the Call
	INFO_PRINTF1(_L("Hanging up"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-029.02"));
	INFO_PRINTF1(_L("HangUp successful"));

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
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

TVerdict CTestCallInfo::doTestStepL()
/**
 * This function gets information pertaining to a call:
 * Bearer Service Information, Call duration, Remote Party Information (if available)
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
	INFO_PRINTF1(_L("Test Call Information"));
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg mobileCallInfoPckg(mobileCallInfo) ;

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-001.01"));

	TName CallName ;
	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("GT83-TVOICE-014.05"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

	//get status
	RCall::TStatus callStatus;
	CHECKPOINT(iVoiceCall.GetStatus(callStatus), KErrNone, _L("GT83-TD-016.02")) ;
	CHECKPOINT(callStatus, RCall::EStatusIdle, _L("GT83-TD-016.02")) ;

	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;
	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	INFO_PRINTF1(_L("Please ring the phone"));

    TESTL(CallMeDear() == KErrNone)  ; //init sim incoming call


	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a Voice Line (Generic)	RCall::AnswerIncomingCall
	iVoiceCall.AnswerIncomingCall(iStatus);
	WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 60 seconds
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-026.00")) ;

	// User Information
	INFO_PRINTF1(_L("The call is now connected."));
	INFO_PRINTF1(_L("Retrieving Call Information."));

	// Bearer Service Information
	INFO_PRINTF1(_L("Get Bearer Service Info..."));
	RCall::TBearerService bearerServiceInfo;
	TInt ret=iVoiceCall.GetBearerServiceInfo(bearerServiceInfo);
	CHECKPOINT(ret, KErrNotSupported, _L("GT83-TVOICE-019.01")) ;
	INFO_PRINTF1(_L("Get BearerService Information is NOT Supported"));

	// Get Call Information
	INFO_PRINTF1(_L("Get Current Call Info..."));
	ret=iVoiceCall.GetInfo(iCallInfo);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-024.01")) ;
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
	ret=iVoiceCall.GetMobileCallInfo(mobileCallInfoPckg);

	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-036.01")) ;
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Current Call info is NOT Supported"));
		}

	//Get Remote Party Info (PDEF108459)
 	RMobileCall::TMobileCallRemotePartyInfoV1 remoteParty;
 	RMobileCall::TMobileCallRemotePartyInfoV1Pckg remotePartyPckg(remoteParty);	
 	
 	TRequestStatus reqStatus;
 	iVoiceCall.NotifyRemotePartyInfoChange(reqStatus, remotePartyPckg);
 	User::WaitForRequest(reqStatus);	
 	
 	TESTL(reqStatus == KErrNone);
 	
 	if (remoteParty.iRemoteIdStatus == RMobileCall::ERemoteIdentityAvailable)
 		{			
 		INFO_PRINTF1(_L("Displaying Remote Party Information about the Current Call"));
 		_LIT(KCallingName, "Calling Name : %S");
 		INFO_PRINTF2(KCallingName, &remoteParty.iCallingName);
 		_LIT(KRemoteNumber, "Remote Number : %S");
 		INFO_PRINTF2(KRemoteNumber, &remoteParty.iRemoteNumber.iTelNumber);
   		}
	else
		{
		_LIT(KRemoteIdentityNotAvilable,"Remote Identity is not available.");
 		INFO_PRINTF1(KRemoteIdentityNotAvilable);
   		}

	// Hang up the Call
	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(5*KOneSecond);

	// Call Duration
	INFO_PRINTF1(_L("Get Call Duration..."));
	TTimeIntervalSeconds callDuration2;
	ret=iVoiceCall.GetCallDuration(callDuration2);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-023.01")) ;

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
	CHECKPOINT(iStatus.Int(), KErrNone, _L("GT83-TVOICE-009.02")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
	}
//------------------------------------------------------------------

CTestPhoneInfo::CTestPhoneInfo()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestPhoneInfo"));
	}
//------------------------------------------------------------------

TVerdict CTestPhoneInfo::doTestStepL()
/**
 * This function retrieves a snapshot of the phone's GSM information, gets the
 * number of lines associated with the phone as well as information on the line
 * and phone. Gets the phone's status and also performs some Cancel Requests.
 * @test GT83-TVOICE-037.01	Retrieve phone's multimode capabilities and print them to screen.
 */
	{
	INFO_PRINTF1(_L("Phone Information test"));

	// Retrieve a snapshot of the current phone information
	INFO_PRINTF1(_L("Get Phone Info..."));
	RPhone::TPhoneInfo phoneInfo;
	RPhone::TCaps phoneCaps;
	TInt ret=iPhone.GetInfo(phoneInfo);
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Phone info is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Phone Info is successful."));
		CHECKPOINT(phoneInfo.iDetection, RPhone::EDetectedPresent, _L("GT83-TVOICE-0AAA"));
		INFO_PRINTF1(_L("Local Modem has been detected."));
		}

	//test mobile phone caps
	RMobilePhone::TMobilePhoneModeCaps mobileCaps ;
	CHECKPOINT(iPhone.GetMultimodeCaps((TUint32&)mobileCaps), KErrNone, _L("GT83-TVOICE-037.01")) ;
	Print_TMobilePhoneModeCaps(mobileCaps);

	// Number of lines associated with the phone
	TInt lineCount=0;
	TESTL(iPhone.EnumerateLines(lineCount)==KErrNone);
	TESTL(lineCount>0);

	for (;lineCount>0; lineCount--)
		{
		CHECKPOINT(iPhone.GetLineInfo(lineCount-1, iLineInfo), KErrNone, _L("GT83-TVOICE-0AAA"));
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

	// Status of the Phone
	INFO_PRINTF1(_L("Get the Phone's Status..."));
	RPhone::TStatus phoneStatus;
	ret=iPhone.GetStatus(phoneStatus);
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Phone Status is NOT Supported"));
		}
	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Phone Status is successful."));
		CHECKPOINT(phoneStatus.iMode, RPhone::EModeIdle, _L("GT83-TVOICE-0AAA"));
		INFO_PRINTF1(_L("The phone is currently Idle"));
		}

	// Notification Cancel Requests : Modem Detected
	INFO_PRINTF1(_L("Notify Modem Detected Cancel Request..."));
	RPhone::TModemDetection detected;
	iPhone.NotifyModemDetected(iStatus, detected);
	iPhone.NotifyModemDetectedCancel();
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Notify Modem Detected Cancel status: %d")), iStatus.Int());

	// Notification Cancel Requests : Caps Change
	INFO_PRINTF1(_L("Notify Caps Change Cancel Request..."));
	iPhone.NotifyCapsChange(iStatus, phoneCaps);
	iPhone.NotifyCapsChangeCancel();
	User::WaitForRequest(iStatus);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Notify Caps Change Cancel status: %d")), iStatus.Int());
	return TestStepResult();
	}
//------------------------------------------------------------------

CTestDtmf::CTestDtmf()
/** Each test step i7nitialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestDtmf"));
	}
//------------------------------------------------------------------

TVerdict CTestDtmf::doTestStepL()
/** DTMF tests suite
 * @test GT83-TVOICE-038.01 Test retieval of DTMF capabilities where the TSY/ME supports sending a string of DTMF tones, containing one or more tone(s).
 * @test GT83-TVOICE-040.02 Test sending a valid string of DTMF tone characters (p,0,1,2,3,4,5,6,7,8,9,0,w,A,B,C,D,*,#)
 * @test GT83-TVOICE-041.00 Test sending a single DTMF tone (Generic)
 * @test GT83-TVOICE-042.01 Test setting a request for a notification of a stop in a DTMF string
 * @test GT83-TVOICE-042.03 Test the ability to continue sending tones when a stop is encountered in the middle of a DTMF string
*/
	{
	_LIT(KDtmfString0,"45p6789ABCDp");
	_LIT(KDtmfString1,"01w23");

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName;
	TESTL(iVoiceCall.OpenNewCall(iLine,callName)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	TRequestStatus reqStatus0, reqStatus1;

	INFO_PRINTF2(_L("Dialling %S "),&KGoodNumber);
	iVoiceCall.Dial(reqStatus0,KGoodNumber);
	User::WaitForRequest(reqStatus0);
	// Wait for the Call Status Change notification
	TESTL(reqStatus0==KErrNone);
	INFO_PRINTF1(_L("...Call is Connected"));

	INFO_PRINTF1(_L("Testing an erroneous DTMF String Tx Continue"));
	TESTL(iPhone.ContinueDTMFStringSending(ETrue)==KErrNotReady);

	INFO_PRINTF1(_L("Test DTMF Caps"));
	TUint32 dtmfCaps;
	CHECKPOINT(iPhone.GetDTMFCaps(dtmfCaps), KErrNone, _L("GT83-TVOICE-038.01"));
	TESTL(((dtmfCaps &(RMobilePhone::KCapsSendDTMFString
				   | RMobilePhone::KCapsSendDTMFSingleTone)) !=0) );

	INFO_PRINTF1(_L("Test Send DTMF String"));
	iPhone.SendDTMFTones(reqStatus0,KDtmfString0) ;
	User::After(1000000);

	TChar character('1');
	CHECKPOINT(iPhone.StartDTMFTone(character), KErrInUse, _L("GT83-TVOICE-041.00"));
	User::After(1000000);
	CHECKPOINT(iPhone.StopDTMFTone(), KErrInUse, _L("GT83-TVOICE-040.09"));
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);

	INFO_PRINTF1(_L("Test Start & Stop DTMF Tone"));
	CHECKPOINT(iPhone.StartDTMFTone(character), KErrNone, _L("GT83-TVOICE-041.00"));
	User::After(1000000);
	CHECKPOINT(iPhone.StopDTMFTone(), KErrNone, _L("GT83-TVOICE-040.09"));

	INFO_PRINTF1(_L("Test Send DTMF String with wait & continue"));
	iPhone.NotifyStopInDTMFString(reqStatus1);

	iPhone.SendDTMFTones(reqStatus0,KDtmfString1);
	CHECKPOINT(reqStatus0.Int(), KRequestPending, _L("GT83-TVOICE-040.02"));
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1==KErrNone);
	CHECKPOINT(iPhone.ContinueDTMFStringSending(ETrue), KErrNone, _L("GT83-TVOICE-042.03"));
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);

	INFO_PRINTF1(_L("Test Send DTMF String with wait & terminate"));
	iPhone.NotifyStopInDTMFString(reqStatus1);
	CHECKPOINT(reqStatus1.Int(), KRequestPending,_L("GT83-TVOICE-042.01"));
	iPhone.SendDTMFTones(reqStatus0,KDtmfString1);
	CHECKPOINT(reqStatus0.Int(), KRequestPending,_L("GT83-TVOICE-040.02"));
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1==KErrNone);
	CHECKPOINT(iPhone.ContinueDTMFStringSending(EFalse), KErrNone, _L("GT83-TVOICE-042.03"));
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrAccessDenied);

	INFO_PRINTF1(_L("Test Send DTMF String terminated early by a call closure"));
	iPhone.SendDTMFTones(reqStatus0,KDtmfString0);
	CHECKPOINT(reqStatus0.Int(), KRequestPending,_L("GT83-TVOICE-040.02"));
	User::After(1000000);
	INFO_PRINTF1(_L("Hanging up..."));
	TESTL(iVoiceCall.HangUp()==KErrNone);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrEtelNoCarrier);

	INFO_PRINTF1(_L("Test Sending DTMF Strings while there is no active call."));
	CHECKPOINT(iPhone.ContinueDTMFStringSending(ETrue), KErrNotReady, _L("GT83-TVOICE-042.03"));
	iPhone.SendDTMFTones(reqStatus0,KDtmfString0);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrEtelCallNotActive);

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();
	}

//------------------------------------------------------------------

CTestRemoteHangup::CTestRemoteHangup()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestRemoteHangup"));
	}
//------------------------------------------------------------------

TVerdict CTestRemoteHangup::doTestStepL()
/**	 Tests line status function
 * @test GT83-TVOICE-XXX.XX "Test remote hangup of connected call"
*/
	{
	INFO_PRINTF1(_L("Test Remote Hangup"));

	RCall::TStatus lineStatus;
	TName callName;
	TBuf<16> goodNumber ;
	GetGoodNumber(goodNumber) ;

	// Open a Voice Line and Call
	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("GT83-TVOICE-XXX.XX"));

	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, callName), KErrNone, _L("GT83-TVOICE-XXX.XX"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &callName);

	INFO_PRINTF2(_L("Dialling %S...."), &goodNumber);
	iVoiceCall.Dial(iStatus, goodNumber);
	User::WaitForRequest(iStatus);
	TESTL(iStatus==KErrNone);

	// Trigger remote hangup
	RCall::TStatus notifyLineStatus;
	TRequestStatus notifyStatus;
	iLine.NotifyStatusChange(notifyStatus, notifyLineStatus);
	TESTL(HangMeUpDear() == KErrNone);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int() == KErrNone);
	CHECKPOINT(notifyLineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-XXX.XX")) ;
	TInt ret = iLine.GetStatus(lineStatus);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-XXX.XX")) ;
	CHECKPOINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-XXX.XX")) ;

	// Close the line and call
	iVoiceCall.Close();
	iLine.Close();

	return TestStepResult();
	}


CTestCallDuration::CTestCallDuration()
	{

	SetTestStepName(_L("TestCallDuration"));
	}

CTestCallDuration::~CTestCallDuration()
	{

	}

TVerdict CTestCallDuration::doTestStepL()
	{
	TVerdict   Ret,Ret1;
	INFO_PRINTF1(_L("Call Duration Test"));
	
	Ret = RunTestL(FALSE);
	Ret1 = RunTestL(TRUE);	
	if(Ret1 == EPass && Ret == EPass)	
		Ret = EPass;
	else
		Ret = EFail;
	return Ret;
	}

TVerdict CTestCallDuration::RunTestL(TBool aBRingOut)
	{
	//RPhone::TLineInfo lineInfo;
	TRequestStatus   statDuration;
	TTimeIntervalSeconds CallLengthDuration,CallLength;
	TName CallName ;

	CleanupClosePushL(iLine);
	CleanupClosePushL(iVoiceCall);

	INFO_PRINTF1(_L("Opening Voice Line"));
	CHECKPOINT(iLine.Open(iPhone,KVoiceLineName), KErrNone, _L("Call Duration Test 1"));
	INFO_PRINTF1(_L("Opening New Voice Call"));
	CHECKPOINT(iVoiceCall.OpenNewCall(iLine, CallName), KErrNone, _L("Call Duration Test 2"));
	INFO_PRINTF2(_L("Voice Call name is %S"), &CallName);

//Test Call Duration 
	// start notification of call duration.
	INFO_PRINTF1(_L("Testing Call Duration"));
	iVoiceCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	RingOrDialL(aBRingOut);

	User::WaitForRequest(statDuration);
	CHECKPOINT(CallLengthDuration.Int() == 1,TRUE,_L(""));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLengthDuration.Int());
	User::After(5200000);
	iVoiceCall.GetCallDuration(CallLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLength.Int());
	CHECKPOINT(CallLength.Int() >= 5 ,TRUE,_L("Call Duration Test 3"));
	
	// Hang up the Call
	INFO_PRINTF1(_L("Hanging up the call"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF1(_L("HangUp successful"));
	
	iVoiceCall.GetCallDuration(CallLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLength.Int());
	
	TInt numberOfNotifs=0 ;
	for (; statDuration==KErrNone ;numberOfNotifs++)
	{
	iVoiceCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	WaitWithTimeout(statDuration,5*KOneSecond);
	}
	
	User::After(6000000);
//Test Call Duration notification

	INFO_PRINTF1(_L("Testing Call Duration Notification"));
	RingOrDialL(aBRingOut);
	User::After(2500000);
	iVoiceCall.GetCallDuration(CallLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLength.Int());
	CHECKPOINT(CallLength.Int() == 2,TRUE,_L("Call Duration Test 4"));

	iVoiceCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	User::WaitForRequest(statDuration);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLengthDuration.Int());

	CHECKPOINT(CallLengthDuration.Int() >= 2,TRUE,_L("Call Duration Test 5"));
	
	iVoiceCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	iVoiceCall.NotifyCallDurationChangeCancel();
	WaitWithTimeout(statDuration,10*KOneSecond);
	CHECKPOINT((statDuration.Int()==KErrNone)||(statDuration.Int()==KErrCancel), TRUE, _L("Call Duration Test 6"));

	INFO_PRINTF1(_L("Hanging up the call"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF1(_L("HangUp successful"));

//
	User::After(6000000);
	
//Test NotifyCallDurationChangeCancel
	INFO_PRINTF1(_L("Testing Call Duration Notification Cancel"));
	iVoiceCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	RingOrDialL(aBRingOut);
	iVoiceCall.NotifyCallDurationChangeCancel();
	WaitWithTimeout(statDuration,10*KOneSecond);
	CHECKPOINT(iStatus.Int(), KErrNone, _L("Call Duration Test 7"));
	
	User::WaitForRequest(statDuration);
	// Hang up the Call
	INFO_PRINTF1(_L("Hanging up the call"));
	iVoiceCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF1(_L("HangUp successful"));
//

	// Close the line and call
	CleanupStack::PopAndDestroy(&iVoiceCall);
	CleanupStack::PopAndDestroy(&iLine);

	return TestStepResult();

	}

TBool CTestCallDuration::RingOrDialL(TBool aBRingOut)
	{
	if(!aBRingOut)
		{
		INFO_PRINTF1(_L("Please ring the phone"));
		TESTL(CallMeDear() == KErrNone)  ; //init sim incoming call
		iVoiceCall.AnswerIncomingCall(iStatus);		// wait for a call
		WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 60 seconds
		CHECKPOINT(iStatus.Int(), KErrNone, _L("Call Duration Test 8"));
		}
	else
		{
		INFO_PRINTF2(_L("Dialling %S"),&KGoodNumber);
		iVoiceCall.Dial(iStatus,KGoodNumber);
		User::WaitForRequest(iStatus);
		// Wait for the Call Status Change notification
		TESTL(iStatus==KErrNone);
		INFO_PRINTF1(_L("...Call is Connected"));
		}
	return TRUE;
	}
