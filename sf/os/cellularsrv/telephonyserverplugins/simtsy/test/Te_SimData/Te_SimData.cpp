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
//

/**
 @file
*/

#include "Te_SimData.h"

#include <sacls.h>

const TInt KTenPartOfSec = 100000 ;
const TInt KOneSecond=1000000;	// Used in a time out function, 1 second (in microSeconds)
const TInt KTenSeconds=10000000;	// Used in a time out function, 10 seconds (in microSeconds)

 
_LIT(KGoodNumber,"1789");

 
static TInt NTRasThreadFunction(TAny* /*aThreadData*/)
/**
* Does 'durty' work to  start NT RAS session
* @note It sets
*	iRate = EBps38400;
*	iDataBits = EData8;
*	iStopBits = EStop1 ;
*	iParity = EParityNone;
* be sure that your NT comport has the same settings.
*/
	{
	_LIT(KCsyName,"LOOPBACK.CSY");
	_LIT(KPortName,"LOOPBACK::1");

	_LIT8(KServerText,"SERVER");

	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;

	RCommServ commServ;
	TInt r=commServ.Connect();
	r=commServ.LoadCommModule(KCsyName);
	if( KErrNone == r)
		{
		RComm comm;
		r=comm.Open(commServ,KPortName,ECommShared);
		if( KErrNone == r)
			{
			TBuf8<6> readBuf;
			TRequestStatus stat0;
			comm.Read(stat0,readBuf);
			User::WaitForRequest(stat0) ;

			comm.Write(stat0,KServerText);
			User::WaitForRequest(stat0) ;
			}
		comm.Close() ;
		}
	commServ.Close() ;
	delete cleanup;
	return r;
	}

LOCAL_C TInt StartNTRasSimulation()
/**
* Starts virual, because of lookback serial driver,
* NT RAS session
*/
	{
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;
	TInt res = KErrNone ;
	_LIT(KNtRasName,"NTRasEmulation");

	RThread t;
	res=t.Create(KNtRasName,NTRasThreadFunction,
					KStackSize,KHeapSize,KMaxHeapSize,NULL);
	t.Resume();
	t.Close();

	return res;
	}


void CTestSimDataStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
/**
 * Timeout function
 */
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}
	timer.Close() ;
	}

//
// CTestSimDataStep Implementation
//

void CTestSimDataStep::GetGoodNumber(TDes& aNum)
/**Returns string with number for outgoing call.
 * For.ex Good number is 179 Vodaphone test line - recorded message.
 * Function tries to read value of switch /modem, otherwise it use defualt value.
 * @return phone number string
 * @note have to be updated to read information from cmd line or config. file
*/
	{
	aNum.Copy(KGoodNumber) ;
	}

void CTestSimDataStep::TestLineCapabilitiesL()
/**
 * @test GT83-TDATA-002.01 Test retrieval of a data line's capabilities. Display the capabilities to screen
*/
	{
	INFO_PRINTF1(_L("Test Line Capabilities"));

	// Line Capabilities
	RLine::TCaps lineCaps;
	TInt ret=iLine.GetCaps(lineCaps);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-002.01")) ;
	if (ret!=KErrNone)
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
	}

TInt CTestSimDataStep::CallMeDear()
/**
* Asks for incomming call
* @return KErrNone if could make it
*/
	{
	iState++;
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingDataCall, iState);
	INFO_PRINTF2(_L("Set Incoming Call Publish & Subscribe property result: %d"),ret) ;
	return ret;
	}

TInt CTestSimDataStep::HangMeUpDear()
/**
* Asks for remote hangup
* @return KErrNone if could make it
*/
	{
	iState++;
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyRemoteHangup, iState);
	INFO_PRINTF2(_L("Set Remote Hangup Publish & Subscribe property result: %d"),ret) ;
	return ret;
	}

void CTestSimDataStep::Print_RCall_TStatus(RCall::TStatus& aArg)

/** Display value of enum RCall::TStatus to screen.
 * @param value to print
*/
	{
	switch(aArg)
		{
		case RCall::EStatusUnknown:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusUnknown")) ;
		break ;
		case RCall::EStatusIdle:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusIdle")) ;
		break ;
		case RCall::EStatusDialling:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusDialling")) ;
		break ;
		case RCall::EStatusRinging:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusRinging")) ;
		break ;
		case RCall::EStatusAnswering:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusAnswering")) ;
		break ;
		case RCall::EStatusConnecting:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusConnecting")) ;
		break ;
		case RCall::EStatusConnected:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusConnected")) ;
		break ;
		case RCall::EStatusHangingUp:
			INFO_PRINTF1(_L("Value of enum is RCall::EStatusHangingUp")) ;
		break ;
		}
	}

void CTestSimDataStep::Print_TCapsEnum(RCall::TCaps& aArg)
/** Display value of enum RCall::TCapsEnum to screen.
 * @param value to print
*/
	{
	if(aArg.iFlags & RCall::KCapsData)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsData")) ;
		}
	if(aArg.iFlags & RCall::KCapsFax)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsFax")) ;
		}
	if(aArg.iFlags & RCall::KCapsVoice)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsVoice")) ;
		}
	if(aArg.iFlags & RCall::KCapsDial)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsDial")) ;
		}
	if(aArg.iFlags & RCall::KCapsConnect)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsConnect")) ;
		}
	if(aArg.iFlags & RCall::KCapsHangUp)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsHangUp")) ;
		}
	if(aArg.iFlags & RCall::KCapsAnswer)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsAnswer")) ;
		}
	if(aArg.iFlags & RCall::KCapsLoanDataPort)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsLoanDataPort")) ;
		}
	if(aArg.iFlags & RCall::KCapsRecoverDataPort)
		{
			INFO_PRINTF1(_L("Has a flag TCapsEnum::KCapsRecoverDataPort")) ;
		}
	}
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
	INFO_PRINTF1(_L("Test Line Status"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	RCall::TStatus lineStatus;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	TInt ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.00")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-031.00")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-031.01")) ;

	INFO_PRINTF1(_L("Please ring the phone"));

	TName callName;
	/* @test GT83-AT-024	Test setting notification of an incoming call on a data line */
	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call

	CallMeDear() ;

	WaitWithTimeout(iStatus,20*KOneSecond);				// but timeout after 60 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;

	/* @test GT83-AT-36	Test retrieval of the current call status when it is ringing*/
	ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-0")) ;
	CHECK_POINT(lineStatus, RCall::EStatusRinging, _L("GT83-TDATA-0")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-0")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-008.001")) ;

	INFO_PRINTF1(_L("Phone is ringing..."));
	//Test for incomming data call
	TestLineCapabilitiesL();

	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	ret = iDataCall.OpenExistingCall(iLine, callName);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-014.05")) ;

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);
	WaitWithTimeout(iStatus,30*KOneSecond);		// but timeout after 2 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;
	ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.04")) ;
	CHECK_POINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.04")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-031.04")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-031.04")) ;
	TestLineCapabilitiesL();

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.07")) ;
	CHECK_POINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.07")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-031.07")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-031.07")) ;


	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Hanging Up"));
	iDataCall.HangUp(iStatus);

	/* @test GT83-AT-36	Test retrieval of the current call status when it is hang up*/
	ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.08")) ;
	CHECK_POINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-0")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.09")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-004.09")) ;

	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;  //???  line 825

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	ret = iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.02")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Test Line Status"));

	TInt ret = RProperty::Set(KUidSystemCategory, KUidPhonePwrValue, ESAPhoneOff);
	if(ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to set property KUidSystemCategory/KUidPhonePwr to ESAPhoneOff. The error was %d. Continuing..."),ret);
		}

	User::After(5 * 1000000);	// sleep 5 secs; make sure phone is off
	
	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);
	StartNTRasSimulation();

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);
	TName callName;

	RCall::TStatus lineStatus;
	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	/* @test GT83-AT-36	Test retrieval of the current call status when it is idle*/
	iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.00")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.02")) ;

	RMobileCall::TMobileCallStatus mobileLineStatus;
	iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-031.00")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-031.01")) ;
	/* @test GT83-AT-028	Test opening a new call by specifying the name of a line
	*/
	iDataCall.OpenNewCall(iLine, callName);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-014.05")) ;

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;

	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iDataCall.Dial(iStatus, callDataParamsPckg, GoodNumber);
	iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.03")) ;
	//CHECK_POINT(lineStatus, RCall::EStatusDialling, _L("GT83-TDATA-004.03")) ;
	WaitWithTimeout(iStatus,20*KOneSecond);				// but timeout after 60 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;
	//Test for outgoing data call
	TestLineCapabilitiesL();

	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.07")) ;
	CHECK_POINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-004.07")) ;

	iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-0")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-0")) ;


	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(10*KOneSecond);

	INFO_PRINTF1(_L("Call connected - listen for 20 seconds..."));

	User::After(20*KOneSecond); // hold call active for 20 seconds

	INFO_PRINTF1(_L("Hanging up"));
	iDataCall.HangUp(iStatus);

	iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.08")) ;
	CHECK_POINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-004.08")) ;

	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	//Test for outgoing data call after hang up
	TestLineCapabilitiesL();
	/* @test GT83-AT-36	Test retrieval of the current call status when it is connected*/
	iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.09")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-004.09")) ;

	iLine.GetMobileLineStatus(mobileLineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-004.09")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-004.09")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
 * @test NONE	Test the ability to get the current line information of the data line (Generic)
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-005.01 "Test retrieval of the current hook status, when the line is currently ""off hook"""
 * @test GT83-TDATA-005.02 "Test retrieval of the current hook status, when the line is currently ""on hook"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 * @test GT83-TDATA-026.02 Test the ability to place an asynchronous request to hang up a data call
 */
	{
	INFO_PRINTF1(_L("Test Line Information"));
    //to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));

	// Line Information
	RLine::TLineInfo lineInfo;
	TRAPD(ret, ret=iLine.GetInfo(lineInfo));
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-013.06"));
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Get Line Info. is NOT supported"));
		}
	else
		{
		INFO_PRINTF1(_L("Get Line Info is successful"));
		CHECK_POINT(lineInfo.iHookStatus,  RCall::EHookStatusOn, _L("GT83-TDATA-0")) ;
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
		CHECK_POINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TDATA-005.02")) ;
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
	INFO_PRINTF1(_L("Please ring the phone "));
	CallMeDear() ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);

	WaitWithTimeout(iStatus,2*KTenSeconds);		// but timeout after 60 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;

	// Count the no. of calls on the line & Get call info.
	INFO_PRINTF1(_L("Retrieving no. of calls opened from the line.."));
	TInt lineCount=0;
	iLine.EnumerateCall(lineCount);
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
		CHECK_POINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-005.01")) ;
		INFO_PRINTF1(_L("The line is currently OFF Hook"));
		}

	INFO_PRINTF1(_L("Please stay connected... the test will hang up shortly"));
	User::After(KTenSeconds);

	INFO_PRINTF1(_L("Hanging up"));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-026.02")) ;

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	// Close the line and call
	//GT83-AT-022	Test closing a data line (Generic)	RLine::Close

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Test Line Notifications"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	TName callName1;
	TName callName2;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	INFO_PRINTF1(_L("Please ring the phone"));

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call

	// @test GT83-AT-026	Test setting notification of a change in the status of the data line
	iLine.NotifyStatusChange(iStatus2, lineStatus);

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	// @test GT83-AT-037	Test setting notification of a change in hook status of a data line	void RLine::NotifyHookChange(TRequestStatus& aStatus, THookStatus& aHookStatus)	Function does not produce any unexpected errors. aStatus = KErrPending
	iLine.NotifyHookChange(iStatus4, hookStatus);

    /* @test GT83-AT-025	Test notification of a call being added to a data line 	*/
	iLine.NotifyCallAdded(iStatus5, callName2);

	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-008.01")) ;
	CHECK_POINT(iStatus2.Int(), KRequestPending, _L("GT83-TDATA-010.02")) ;
	CHECK_POINT(iStatus3.Int(), KRequestPending, _L("GT83-TDATA-032.01")) ;
	CHECK_POINT(iStatus4.Int(), KRequestPending, _L("GT83-TDATA-011.01")) ;
	CHECK_POINT(iStatus5.Int(), KRequestPending, _L("GT83-TDATA-009.01")) ;

	CallMeDear() ;

	WaitWithTimeout(iStatus,20*KOneSecond);				// but timeout after 60 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-008.02")) ;

    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "idle" to "ringing"
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.02")) ;
	CHECK_POINT(lineStatus, RCall::EStatusRinging, _L("GT83-TDATA-010.02")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.09")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-032.09")) ;

	/* @test GT83-AT-025	Test notification of a call being added to a data line
	that currently has no calls*/
	User::WaitForRequest(iStatus5);
	CHECK_POINT(iStatus5.Int(), KErrNone, _L("GT83-TDATA-009.011.01")) ;
	CHECK_POINT(callName2 == callName1, 1,  _L("GT83-TDATA-011")) ;

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));

	/* @test GT83-AT-026	Test notification of a change in the status of the data line from
	 "idle" to "ringing" to "answering" to "connected" to "hanging up" to "idle"
	 */
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	iDataCall.OpenExistingCall(iLine, callName1);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-014.05")) ;

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
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.01")) ;
	CHECK_POINT(lineStatus, RCall::EStatusAnswering, _L("GT83-TDATA-010.01")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.10")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusAnswering, _L("GT83-TDATA-032.10")) ;

	WaitWithTimeout(iStatus,30*KOneSecond);		// but timeout after 2 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;	  //wrong code -2147483647


    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "answering"	to "connecting"
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.01")) ;
	CHECK_POINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-010.01")) ;


	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.11")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-032.11")) ;

	User::WaitForRequest(iStatus4);
	CHECK_POINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.02")) ;
	CHECK_POINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-009.02")) ;

	// now hangup call
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

    // @test GT83-AT-026	Test notification of a change in the status of the data line
	// from "connecting" to "hanging up"
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.02")) ;
	CHECK_POINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-010.02")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.07")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-032.07")) ;

	// @test GT83-AT-026	Test notification of a change in the status of the data
	// line from "hanging up" to "idle"
	iLine.NotifyStatusChange(iStatus2, lineStatus);
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.02")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-010.02")) ;

	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.08")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-032.08")) ;

	//!< @test GT83-AT-037	Test notification of a change in hook status of a data line
	//!<  from "off" to "on" aHookStatus = EHookStatusOff before the hook status changes,
	//!< and EHookStatusOn after the hook status changes
	User::WaitForRequest(iStatus4);
	CHECK_POINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.03")) ;
	CHECK_POINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TDATA-009.03")) ;
	//check that all request has been finished
	TESTL(iStatus.Int()!= KRequestPending) ;
	TESTL(iStatus2.Int() != KRequestPending) ;
	TESTL(iStatus3.Int() != KRequestPending) ;
	TESTL(iStatus4.Int() != KRequestPending) ;
	TESTL(iStatus5.Int() != KRequestPending) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Test Line Notifications"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);
	StartNTRasSimulation();

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;

	// NotifyIncomingCall and NotifyStatusChange
	//TName callName1;
	//TName callName2;
	RCall::TStatus lineStatus;
	RMobileCall::TMobileCallStatus mobileLineStatus;
	RCall::THookStatus hookStatus;
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);


	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	CHECK_POINT(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-014.05")) ;
	// Now test call status during an outgoing call
	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iDataCall.Dial(iStatus, callDataParamsPckg, GoodNumber);

	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECK_POINT(lineStatus, RCall::EStatusDialling, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.02")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusDialling, _L("GT83-TDATA-032.02")) ;

	User::WaitForRequest(iStatus); //waiting for dial
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;

	User::WaitForRequest(iStatus4);
	CHECK_POINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.03")) ;
	CHECK_POINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-009.03")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

#ifdef SIM_TSY
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECK_POINT(lineStatus, RCall::EStatusConnecting, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.04")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusConnecting, _L("GT83-TDATA-032.04")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

#endif

	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECK_POINT(lineStatus, RCall::EStatusConnected, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.04")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-032.04")) ;


	INFO_PRINTF1(_L("Call connected - hanging up..."));

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);
	iLine.NotifyHookChange(iStatus4, hookStatus);

	//HANG UP
	User::After(10*KOneSecond);
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-0")) ;

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECK_POINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.07")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-032.07")) ;

	/* @test GT83-AT-037	Test notification of a change in hook status of a data
	 line from "on" to off"	aHookStatus = EHookStatusOn before the hook status changes,
	 */
	User::WaitForRequest(iStatus4);
	CHECK_POINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-009.02")) ;
	CHECK_POINT(hookStatus, RCall::EHookStatusOn, _L("GT83-TDATA-009.02")) ;

	iLine.NotifyStatusChange(iStatus2, lineStatus);
	iLine.NotifyMobileLineStatusChange(iStatus3, mobileLineStatus);

	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-010.03")) ;
	CHECK_POINT(lineStatus, RCall::EStatusIdle, _L("GT83-TDATA-010.03")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-032.08")) ;
	CHECK_POINT(mobileLineStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-032.08")) ;

	TESTL(iStatus != KRequestPending) ;
	TESTL(iStatus2 != KRequestPending) ;
	TESTL(iStatus3 != KRequestPending) ;
	TESTL(iStatus4 != KRequestPending) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Test Line Cancels"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	// Notify Incoming Call
	TName callName;
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	iLine.NotifyIncomingCall(iStatus, callName);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-008.02")) ;
	iLine.NotifyIncomingCallCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-008.03")) ;

	// Notify Status Change
	RCall::TStatus lineStatus;
	iLine.NotifyStatusChange(iStatus, lineStatus);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-010.02")) ;
	User::After(KTenPartOfSec); //snack some sleep
	/** @test GT83-AT-026 Test the ability to cancel the notification of a change in the
	 status of the data line	aStatus = KErrCancel provided that the function
	 being cancelled has not already completed */
	iLine.NotifyStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-010.04")) ;
#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	// Notify Mobile Status Change
	RMobileCall::TMobileCallStatus lineStatus2;
	iLine.NotifyMobileLineStatusChange(iStatus, lineStatus2);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-0")) ;
	iLine.NotifyMobileLineStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-0")) ;
#endif
	// Notify Hook Change
	RCall::THookStatus hookStatus;
	iLine.NotifyHookChange(iStatus, hookStatus);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-009.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	// @test GT83-AT-037	Test the ability to cancel a notification of a
	// change in hook status of a data line
	// aStatus = KErrCancel provided that the function being cancelled has not already completed
	iLine.NotifyHookChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-009.04")) ;

	// Notify Call Added
	iLine.NotifyCallAdded(iStatus, callName);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-011.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	/** GT83-AT-025	Test the ability to cancel a notification of a call being added
	to a data line*/
	iLine.NotifyCallAddedCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-011.03")) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
 * @test GT83-TDATA-001.01 Test opening a data line from a phone that supports data functionality
 * @test GT83-TDATA-009.02 "Test notification of a change in hook status of a data line from ""off"" to ""on"""
 * @test GT83-TDATA-013.06 Test opening a new call by specifying the name of a line
 * @test GT83-TDATA-018.01 "Test retrieval of the bearer service info of a call. Display to screen, all the bearer service information associated with the call."
 * @test GT83-TDATA-022.01 Test retrieval of the duration of a data call. Display the call duration to screen
 * @test GT83-TDATA-023.01 Test the ability to get the current call information of the data call. Display the call information to screen.
 * @test GT83-TDATA-025.00 Test answering a call on a data line (Generic)
 */
	{
	INFO_PRINTF1(_L("Test Call Information"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));
	//get status
	//GT83-AT-026	Test retrieval of a data line's current status.
	RCall::TStatus callStatus;
	CHECK_POINT(iDataCall.GetStatus(callStatus), KErrNone, _L("GT83-TD-015.02")) ;
	CHECK_POINT(callStatus, RCall::EStatusIdle, _L("GT83-TD-015.02")) ;

	INFO_PRINTF2(_L("Call Status = %d"), callStatus);
	Print_RCall_TStatus(callStatus) ;


	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	INFO_PRINTF1(_L("Please ring the phone"));

	CallMeDear() ;

	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);

	WaitWithTimeout(iStatus,20*KOneSecond);		// but timeout after 60 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;

	// User Information
	INFO_PRINTF1(_L("The call is now connected.  Retrieving Call Information."));

	// Bearer Service Information
	INFO_PRINTF1(_L("Get Bearer Service Info..."));
	RCall::TBearerService bearerServiceInfo;
	TInt ret=iDataCall.GetBearerServiceInfo(bearerServiceInfo);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-018.01")) ;
	if (ret==KErrNotSupported)
		INFO_PRINTF1(_L("Get BearerService Information is NOT Supported"));
	if (ret==KErrNone)
		INFO_PRINTF1(_L("Get BearerService Information is successful"));

	// Get Call Information
	INFO_PRINTF1(_L("Get Current Call Info..."));
	ret=iDataCall.GetInfo(iCallInfo);
#ifndef SIM_TSY
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
#endif
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

 	//Get Remote Party Info (PDEF108459)
 	RMobileCall::TMobileCallRemotePartyInfoV1 remoteParty;
 	RMobileCall::TMobileCallRemotePartyInfoV1Pckg remotePartyPckg(remoteParty);	
 	
 	TRequestStatus reqStatus;
 	iDataCall.NotifyRemotePartyInfoChange(reqStatus, remotePartyPckg);
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
	ret=iDataCall.GetCallDuration(callDuration2);
#ifndef SIM_TSY
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-022.01")) ;
#endif
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
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-009.02")) ;

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

	return TestStepResult();
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

TVerdict CTestCallNotifications::doTestStepL()
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
	INFO_PRINTF1(_L("Test Call Notifications"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	TName callName1;
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
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));

	INFO_PRINTF1(_L("Please ring the phone"));

	iLine.NotifyIncomingCall(iStatus, callName1);	// wait for a call
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-008.01")) ;

	CallMeDear() ;

	WaitWithTimeout(iStatus,20*KOneSecond);				// but timeout after 60 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-008.01")) ;

	TInt ret = iDataCall.OpenExistingCall(iLine, callName1);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-014.05")) ;

	iDataCall.GetStatus(callStatus);
	CHECK_POINT(callStatus, RCall::EStatusRinging, _L("GT83-TDATA-015.04")) ;
	iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(mobileCallStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-035.04")) ;

	ret=iDataCall.GetCaps(caps);
	Print_TCapsEnum(caps) ;
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-002.01")) ;
	TESTL(caps.iFlags & (RCall::KCapsData | RCall::KCapsAnswer));

//	INFO_PRINTF1(_L("Opening New Data Call"));
//	CHECK_POINT(iiDataCall.OpenExistingCall(iLine, callName2), KErrNone, _L("GT83-TDATA-013.06")) ;

	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	iDataCall.NotifyCapsChange(iStatus4, caps);
	iDataCall.NotifyHookChange(iStatus5, hookStatus);
	iDataCall.NotifyCallDurationChange(iStatus6, time) ;
	iDataCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);

	INFO_PRINTF1(_L("Phone is ringing... Now answering..."));

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	//callDataParams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);

	iDataCall.GetStatus(callStatus);
#ifndef SIM_TSY
	CHECK_POINT(callStatus, RCall::EStatusRinging, _L("GT83-TDATA-015.05")) ;
#else
	CHECK_POINT(callStatus, RCall::EStatusAnswering, _L("GT83-TDATA-015.05")) ;
#endif
	WaitWithTimeout(iStatus,20*KOneSecond);		// but timeout after 2 seconds
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-025.00")) ;


	//iDataCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECK_POINT(callStatus, RCall::EStatusAnswering, _L("GT83-TDATA-020.10")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.10")) ;
	CHECK_POINT(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TDATA-036.10")) ;

	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECK_POINT(callStatus, RCall::EStatusConnected, _L("GT83-TDATA-020.02")) ;

	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.11")) ;
	CHECK_POINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-036.11")) ;

	User::WaitForRequest(iStatus4);
	CHECK_POINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-017.02")) ;
	Print_TCapsEnum(caps) ;
	TEST_CHECKL(((caps.iFlags & RCall::KCapsData) !=0 ), ETrue,  _L("GT83-TDATA-017.02")) ;

#ifndef SIM_TSY
	iDataCall.NotifyCapsChange(iStatus4, caps);
	User::WaitForRequest(iStatus4);
	CHECK_POINT(iStatus4.Int(), KErrNone, _L("GT83-TDATA-017.02")) ;
	CHECK_POINT((caps.iFlags & (RCall::KCapsData | RCall::KCapsHangUp)!=0), ETrue, _L("GT83-TDATA-017.02")) ;
#endif

	User::WaitForRequest(iStatus5);
	CHECK_POINT(iStatus5.Int(), KErrNone, _L("GT83-TDATA-009.03")) ;
	CHECK_POINT(hookStatus, RCall::EHookStatusOff, _L("GT83-TDATA-009.03")) ;

	User::WaitForRequest(iStatus6);
#ifndef SIM_TSY
	CHECK_POINT(iStatus6.Int(), KErrNone, _L("GT83-TDATA-021.01")) ;
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
	CHECK_POINT(iStatus7.Int(), KErrNone, _L("GT83-TDATA-034.01")) ;
	TEST_CHECKL(((mobileCallCaps.iCallControlCaps & RCall::KCapsData) !=0), ETrue, _L("GT83-TDATA-034.01")) ;
#ifndef SIM_TSY
	iDataCall.NotifyMobileCallCapsChange(iStatus7, mobileCallCapsPckg);
	User::WaitForRequest(iStatus7);
	CHECK_POINT(iStatus7.Int(), KErrNone, _L("GT83-TDATA-034.02")) ;
	//CHECK_POINT(mobileCallCaps.iCallControlCaps, (RCall::KCapsData | RCall::KCapsHangUp), _L("GT83-TDATA-034.02")) ;
	//MKV error
#endif
	// now hangup call
	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);

	INFO_PRINTF1(_L("Call connected - now hanging up..."));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-009.02")) ;	//sometimes timed out -33

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECK_POINT(callStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-020.02")) ;

	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.07")) ;
	CHECK_POINT(mobileCallStatus, RMobileCall::EStatusDisconnecting, _L("GT83-TDATA-036.07")) ;

	iDataCall.NotifyStatusChange(iStatus2, callStatus);
	User::WaitForRequest(iStatus2);
	CHECK_POINT(iStatus2.Int(), KErrNone, _L("GT83-TDATA-020.02")) ;
	CHECK_POINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-020.02")) ;

	iDataCall.NotifyMobileCallStatusChange(iStatus3, mobileCallStatus);
	User::WaitForRequest(iStatus3);
	CHECK_POINT(iStatus3.Int(), KErrNone, _L("GT83-TDATA-036.08")) ;
	CHECK_POINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-036.08")) ;

	TESTL(iStatus != KRequestPending) ;
	TESTL(iStatus2 != KRequestPending) ;
	TESTL(iStatus3 != KRequestPending) ;
	TESTL(iStatus4 != KRequestPending) ;
	TESTL(iStatus5 != KRequestPending) ;
	TESTL(iStatus6 != KRequestPending) ;

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Get Call Status..."));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	// Open a Data line and Call
	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;
	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));


	TInt ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
	TInt ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.02")) ;

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
		CHECK_POINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-023.01")) ;
		CHECK_POINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an incomming call

	// Wait for an incoming call, then get the Call BearerServiceInfo. The
	// Bearer Service Info is only available if the call is currently active
	INFO_PRINTF1(_L("Please ring the phone"));
	CallMeDear() ;

	/* @test GT83-AT-024	Test setting notification of an incoming call on a data line */
	//commented out because connecting process too long and modem breaks connection
	// by temeout
//	iLine.NotifyIncomingCall(iStatus, callName);	// wait for a call
//	WaitWithTimeout(iStatus,60*KOneSecond);				// but timeout after 60 seconds

	ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
//	CHECK_POINT(callStatus, RCall::EStatusRinging, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.07")) ;
//	CHECK_POINT(mobileCallStatus, RMobileCall::EStatusRinging, _L("GT83-TDATA-035.04")) ;

//	INFO_PRINTF1(_L("Phone is ringing..."));

	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;
	// Answer an incoming call initiated by t_echo.exe and obtain the call's current status
	//GT83-AT-032	Test answering a call on a data line (Generic)	RCall::AnswerIncomingCall
	iDataCall.AnswerIncomingCall(iStatus, callDataParamsPckg);
	ret=iDataCall.GetStatus(callStatus);
	//CHECK_POINT(callStatus, RCall::EStatusAnswering, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	//CHECK_POINT(mobileCallStatus, RMobileCall::EStatusAnswering, _L("GT83-TDATA-035.05")) ;
	WaitWithTimeout(iStatus,20*KOneSecond);				// but timeout after 60 seconds

	ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.07")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECK_POINT(callStatus, RCall::EStatusConnected, _L("GT83-TDATA-023.01")) ;
		CHECK_POINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-035.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}
	INFO_PRINTF1(_L("Hanging up..."));

	User::After(10*KOneSecond);
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-009.01")) ;

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.02")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECK_POINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-023.01")) ;
		CHECK_POINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Get Call Status..."));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);
	StartNTRasSimulation();

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));

	RCall::TStatus callStatus;
	RMobileCall::TMobileCallStatus mobileCallStatus;

	//TName callName;

	RMobileCall::TMobileDataCallParamsV1 callDataParams;
	RMobileCall::TMobileDataCallParamsV1Pckg callDataParamsPckg(callDataParams);

	TBuf<16> GoodNumber ;
	GetGoodNumber(GoodNumber) ;
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));


	TInt ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
	TInt ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.02")) ;

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
		CHECK_POINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-023.01")) ;
		CHECK_POINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle"));
		}

	// Now test call status during an outgoing call
	callDataParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callDataParams.iSpeed = RMobileCall::ESpeed9600;
	callDataParams.iProtocol = RMobileCall::EProtocolV32;
	callDataParams.iQoS = RMobileCall::EQoSNonTransparent;


	INFO_PRINTF2(_L("Dialling %S...."), &GoodNumber);
	iDataCall.Dial(iStatus, callDataParamsPckg, GoodNumber);
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-024.00")) ;

	ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.07")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{

		Print_RCall_TStatus(callStatus) ;
		CHECK_POINT(callStatus, RCall::EStatusConnected, _L("GT83-TDATA-023.01")) ;
		CHECK_POINT(mobileCallStatus, RMobileCall::EStatusConnected, _L("GT83-TDATA-035.07")) ;
		INFO_PRINTF1(_L("The call is Connected"));
		}

	INFO_PRINTF1(_L("Hanging up..."));

	User::After(10*KOneSecond);
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	//CHECK_POINT(iStatus.Int(), KErrNone, _L("GT83-TDATA-009.01")) ;

	//This test has some trobles with Erikson phones
	//R520m could return KErrGeneral
	//SH888 could return KErrTimedOut
	INFO_PRINTF2(_L("iDataCall.HangUp() returned status %d"), iStatus.Int()) ;

	ret=iDataCall.GetStatus(callStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-023.01")) ;
	ret2=iDataCall.GetMobileCallStatus(mobileCallStatus);
	CHECK_POINT(ret2, KErrNone, _L("GT83-TDATA-035.02")) ;

	if ((ret==KErrNone) && (ret2==KErrNone))
		{
		CHECK_POINT(callStatus, RCall::EStatusIdle, _L("GT83-TDATA-023.01")) ;
		CHECK_POINT(mobileCallStatus, RMobileCall::EStatusIdle, _L("GT83-TDATA-035.02")) ;
		INFO_PRINTF1(_L("The call is Idle again"));
		}

	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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
	INFO_PRINTF1(_L("Test Call Cancels"));

	//to make sure that line and call will be closed in leave case
	CleanupClosePushL(iLine);
	CleanupClosePushL(iDataCall);

	// Open a Data line and Call
	//GT83-AT-021	Test opening a data line from a phone that supports data functionality
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-001.01"));
	INFO_PRINTF1(_L("Opening New Data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine), KErrNone, _L("GT83-TDATA-013.06"));

	// Call Status Change Cancel Request
	RCall::TStatus callStatus;
	iDataCall.NotifyStatusChange(iStatus, callStatus);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-020.02")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-020.4")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	RMobileCall::TMobileCallStatus mobileCallStatus;
	iiDataCall.NotifyMobileCallStatusChange(iStatus, mobileCallStatus);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-0")) ;
	iiDataCall.NotifyMobileCallStatusChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-0")) ;
#endif
	// Hook Change Cancel Request
	RCall::THookStatus hookStatus;
	iDataCall.NotifyHookChange(iStatus, hookStatus);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-019.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyHookChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-019.04")) ;

	// Test Cancelling Notifications
	RCall::TCaps callCaps;
	iDataCall.NotifyCapsChange(iStatus, callCaps);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-017.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-017.03")) ;

#if 0  //cancel functions don't work yet (?)
	   //any way they are not declared 27.09.2001
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);
	iDataCall.NotifyMobileCallCapsChange(iStatus, mobileCallCapsPckg);
	CHECK_POINT(iStatus.Int(), KRequestPending, _L("GT83-TDATA-0")) ;
	iDataCall.NotifyMobileCallCapsChangeCancel();
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-0")) ;
#endif

	TTimeIntervalSeconds callDuration;
	iDataCall.NotifyCallDurationChange(iStatus, callDuration);
	CHECK_POINT((iStatus==KRequestPending || iStatus==KErrNotSupported), 1, _L("GT83-TDATA-021.01")) ;
	User::After(KTenPartOfSec); //snack some sleep
	iDataCall.NotifyCallDurationChangeCancel();
	User::WaitForRequest(iStatus);
#ifndef SIM_TSY
	CHECK_POINT(iStatus.Int(), KErrCancel, _L("GT83-TDATA-021.03")) ;
#endif
	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;

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

/*	Log(_L("Change simtsy config number"));
	iPhone->Close();
	SetTestNumber(1);

	User::LeaveIfError(iPhone->Open(*iTelServer,KPhoneName)) ;
	// Initialize the Phone
	TInt ret=iPhone->Initialise();
	if (ret!=KErrNone)
		{
		ERR_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		iPhone->Close();
		iTelServer->Close();	// disconnect from etel
		return EFail;
		}
						   */
	
	Ret = RunTestL(TRUE);
	Ret1 = RunTestL(FALSE);	
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
	CleanupClosePushL(iDataCall);

	INFO_PRINTF1(_L("Opening data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("Call Duration Test 1"));
	INFO_PRINTF1(_L("Opening New data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine, CallName), KErrNone, _L("Call Duration Test 2"));
	INFO_PRINTF2(_L("data Call name is %S"), &CallName);
	// start notification of call duration.
	INFO_PRINTF1(_L("Testing Call Duration"));
	iDataCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	RingOrDialL(aBRingOut);

	User::WaitForRequest(statDuration);
	CHECK_POINT(CallLengthDuration.Int() == 1,TRUE,_L(""));
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLengthDuration.Int());
	User::After(5200000);
	iDataCall.GetCallDuration(CallLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLength.Int());
	CHECK_POINT(CallLength.Int() >= 5 ,TRUE,_L("Call Duration Test 3"));
	

	// Hang up the Call
	INFO_PRINTF1(_L("Hanging up the call"));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF1(_L("HangUp successful"));
	
	iDataCall.GetCallDuration(CallLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLength.Int());
	
	TInt numberOfNotifs=0 ;
	for (; statDuration==KErrNone ;numberOfNotifs++)
	{
	iDataCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	WaitWithTimeout(statDuration,5*KOneSecond);
	}
	
	User::After(6000000);


	INFO_PRINTF1(_L("Testing Call Duration Notification"));
	RingOrDialL(aBRingOut);
	User::After(2500000);

	iDataCall.GetCallDuration(CallLength);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLength.Int());
	CHECK_POINT(CallLength.Int() == 2,TRUE,_L("Call Duration Test 4"));

	User::WaitForRequest(statDuration);
	iDataCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	User::WaitForRequest(statDuration);
	INFO_PRINTF2(TRefByValue<const TDesC>(_L("Call duration = %d")), CallLengthDuration.Int());

	CHECK_POINT(CallLengthDuration.Int() >= 2,TRUE,_L("Call Duration Test 5"));
	
	iDataCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	iDataCall.NotifyCallDurationChangeCancel();
	WaitWithTimeout(statDuration,10*KOneSecond);
	CHECK_POINT((statDuration.Int()==KErrNone)||(statDuration.Int()==KErrCancel), TRUE, _L("Call Duration Test 6"));

	INFO_PRINTF1(_L("Hanging up the call"));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF1(_L("HangUp successful"));


	User::After(6000000);
	

	INFO_PRINTF1(_L("Testing Call Duration Notification Cancel"));
	iDataCall.NotifyCallDurationChange(statDuration,CallLengthDuration);
	RingOrDialL(aBRingOut);
	iDataCall.NotifyCallDurationChangeCancel();
	WaitWithTimeout(statDuration,10*KOneSecond);
	CHECK_POINT(iStatus.Int(), KErrNone, _L("Call Duration Test 7"));
	
	// Hang up the Call
	INFO_PRINTF1(_L("Hanging up the call"));
	iDataCall.HangUp(iStatus);
	User::WaitForRequest(iStatus);
	INFO_PRINTF1(_L("HangUp successful"));


	// Close the line and call
	CleanupStack::PopAndDestroy(&iDataCall) ;
	CleanupStack::PopAndDestroy(&iLine) ;
	return TestStepResult();

	}

TBool CTestCallDuration::RingOrDialL(TBool aBRingOut)
	{
	if(!aBRingOut)
		{
		INFO_PRINTF1(_L("Please ring the phone"));
		TESTL(CallMeDear() == KErrNone)  ; //init sim incoming call
		iDataCall.AnswerIncomingCall(iStatus);		// wait for a call
		WaitWithTimeout(iStatus,15*KOneSecond);		// but timeout after 60 seconds
		CHECK_POINT(iStatus.Int(), KErrNone, _L("Call Duration Test 8"));
		}
	else
		{
		StartNTRasSimulation();
		INFO_PRINTF2(_L("Dialling %S"),&KGoodNumber);
		iDataCall.Dial(iStatus,KGoodNumber);
		User::WaitForRequest(iStatus);
		// Wait for the Call Status Change notification
		TESTL(iStatus==KErrNone);
		INFO_PRINTF1(_L("...Call is Connected"));
		}
	return TRUE;
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
/**
 * This function tests call status at various times during calls
 * @test GT83-TDATA-XXX.XX Test hanging up a data call from the remote end
 */
	{
	INFO_PRINTF1(_L("Test Remote Hangup"));

	StartNTRasSimulation();

	RCall::TStatus lineStatus;
	TName callName;
	TBuf<16> goodNumber ;
	GetGoodNumber(goodNumber) ;

	// Open a Data Line and Call
	INFO_PRINTF1(_L("Opening Data Line"));
	CHECK_POINT(iLine.Open(iPhone,KDataLineName), KErrNone, _L("GT83-TDATA-XXX.XX"));

	INFO_PRINTF1(_L("Opening New Data Call"));
	CHECK_POINT(iDataCall.OpenNewCall(iLine, callName), KErrNone, _L("GT83-TDATA-XXX.XX"));
	INFO_PRINTF2(_L("Data Call name is %S"), &callName);

	INFO_PRINTF2(_L("Dialling %S...."), &goodNumber);
	iDataCall.Dial(iStatus, goodNumber);
	User::WaitForRequest(iStatus);
	CHECK_POINT(iStatus, KErrNone, _L("GT83-TDATA-XXX.XX"));

	// Trigger remote hangup
	RCall::TStatus notifyLineStatus;
	TRequestStatus notifyStatus;
	iLine.NotifyStatusChange(notifyStatus, notifyLineStatus);
	TESTL(HangMeUpDear() == KErrNone);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int() == KErrNone);
	CHECK_POINT(notifyLineStatus, RCall::EStatusHangingUp, _L("GT83-TVOICE-XXX.XX")) ;
	TInt ret = iLine.GetStatus(lineStatus);
	CHECK_POINT(ret, KErrNone, _L("GT83-TDATA-XXX.XX")) ;
	CHECK_POINT(lineStatus, RCall::EStatusHangingUp, _L("GT83-TDATA-XXX.XX")) ;

	// Close the line and call
	iDataCall.Close();
	iLine.Close();

	return TestStepResult();
	}

