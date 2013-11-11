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
// Voice API Test base test code.
// 
//

/**
 @file
*/

#include "TE_VoiceTestStepBase.h"

void CTestVoiceStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
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
// CTestVoiceStep Implementation
//
CTestVoiceStep::CTestVoiceStep()
	{
	 //
 	 // Change this process and thread to a higher priority so that we can be in control
 	 // when performing tests such as monitoring async request status values.
 	 //
	 iOrgProcessPriority = RProcess().Priority();
	 iOrgThreadPriority  = RThread().Priority();
   
	 RProcess().SetPriority(EPriorityHigh);
	 RThread().SetPriority(EPriorityMuchMore);
	
	} // CTestVoiceStep::CTestVoiceStep

CTestVoiceStep::~CTestVoiceStep()
	{
	 //
	 // Restore the process and thread priority...
	 //
	 RProcess().SetPriority(iOrgProcessPriority);
	 RThread().SetPriority(iOrgThreadPriority);
	 
	} // CTestVoiceStep::~CTestVoiceStep

void CTestVoiceStep::Print_RCall_TStatus(RCall::TStatus& aArg)
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

void CTestVoiceStep::GetGoodNumber(TDes& aNum)
/**Returns string with number for outgoing call.
 * For.ex Good number is 1234 Vodaphone test line - recorded message.
 * Function tries to read value of switch /modem, otherwise it use defualt value.
 * @return phone number string
 * @note have to be updated to read information from cmd line or config. file
*/
	{
		aNum.Copy(KGoodNumber) ;
	}

void CTestVoiceStep::Print_TCapsEnum(RCall::TCaps& aArg)
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

void CTestVoiceStep::TestGetCallCapabilitiesL(RMobileCall& aCall)
/**
 * call's capabilities to screen
 * @test GT83-TVOICE-016.01 "Test retrieval of the current call status when the call's status is ""Unknown"""
 * @test GT83-TVOICE-032.01 Test retrieval of the voice call's dynamic call control and call event capabilities. Display the call's capabilities to screen
*/
	{
	_LIT(KStartText, "Test Call Capabilities") ;
	TEST_START(&KStartText) ;

	// Call Capabilities
	RCall::TCaps callCaps;
	TRAPD(ret, ret=aCall.GetCaps(callCaps));
	CHECKPOINT_EXPR((ret==KErrNone||ret==KErrNotSupported), _L("*** Failed RMobileCall::GetCaps() ***"));
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Line Capabilities is NOT supported"));
		}
	else
		{
		Print_TCapsEnum(callCaps)	;
		}
		// Get the mobile call capabilities
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);

	ret=aCall.GetMobileCallCaps(mobileCallCapsPckg);
	TESTL(ret==KErrNone||ret==KErrNotSupported); 
	TESTCHECKL(ret,KErrNone);
	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Mobile Call Capabilities is NOT Supported"));
		}

	else
		{
		INFO_PRINTF1(_L("Get Mobile Call Capabilities is Successful"));
		PrintCallCaps(mobileCallCaps.iCallControlCaps);
		}

	TEST_END() ;

	}

void CTestVoiceStep::PrintCallCaps(TUint32 aCaps)
	{
	if ((aCaps&RCall::KCapsData) !=0)
		{
		INFO_PRINTF1(_L("This call supports Data calls"));
		}
	if ((aCaps&RCall::KCapsFax) !=0)
		INFO_PRINTF1(_L("This call supports Fax calls"));
	if ((aCaps&RCall::KCapsVoice) !=0)
		INFO_PRINTF1(_L("This call supports Voice calls"));
	if ((aCaps&RCall::KCapsDial) !=0)
		INFO_PRINTF1(_L("The Dial Request is currently a valid action"));
	if ((aCaps&RCall::KCapsConnect) !=0)
		INFO_PRINTF1(_L("The Connect Request is currently a valid action"));
	if ((aCaps&RCall::KCapsHangUp) !=0)
		INFO_PRINTF1(_L("The HangUp Request is currently a valid action"));
	if ((aCaps&RCall::KCapsAnswer) !=0)
		INFO_PRINTF1(_L("The Answer Request is currently a valid action"));
	if ((aCaps&RCall::KCapsLoanDataPort) !=0)
		INFO_PRINTF1(_L("The LoanDataPort Request is currently a valid action"));
	if ((aCaps&RCall::KCapsRecoverDataPort) !=0)
		INFO_PRINTF1(_L("The RecoverDataPort Request is currently a valid action"));
	}

void CTestVoiceStep::TestLineCapabilitiesL()
/**
 * @test GT83-TVOICE-002.01 Test retrieval of a voice line's capabilities. Display the capabilities to screen
*/
	{
	_LIT(KStartText, "Test Line Capabilities") ; TEST_START(&KStartText) ;

	// Line Capabilities
	RLine::TCaps lineCaps;
	TInt ret=iLine.GetCaps(lineCaps);
	CHECKPOINT_EXPR((ret==KErrNone || ret==KErrNotSupported), _L("*** Failed RMobileLine::GetCaps() ***"));
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
	TEST_END() ;
	}


void CTestVoiceStep::Print_TMobilePhoneModeCaps(RMobilePhone::TMobilePhoneModeCaps& aArg)
/** Display value of enum RMobilePhone::TMobilePhoneModeCaps to screen.
 * @param value to print
*/
	{
	if(aArg & RMobilePhone::KCapsGsmSupported)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneModeCaps::KCapsGsmSupported")) ;
		}
	if(aArg & RMobilePhone::KCapsGprsSupported)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneModeCaps::KCapsGprsSupported")) ;
		}
	if(aArg & RMobilePhone::KCapsAmpsSupported)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneModeCaps::KCapsAmpsSupported")) ;
		}
	if(aArg & RMobilePhone::KCapsCdma95Supported)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneModeCaps::KCapsCdma95Supported")) ;
		}
	if(aArg & RMobilePhone::KCapsCdma2000Supported)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneModeCaps::KCapsCdma2000Supported")) ;
		}
	if(aArg & RMobilePhone::KCapsWcdmaSupported)
		{
			INFO_PRINTF1(_L("Has a flag TMobilePhoneModeCaps::KCapsWcdmaSupported")) ;
		}
	}


TVerdict CTestVoiceStep::doTestStepPreambleL()
	{
	//
	// Mark for memory leaks!!!
	//
	__UHEAP_MARK;

	// Connect to the root telephony server, in order to use the Etel server
   	User::LeaveIfError(iTelServer.Connect()) ;
	User::LeaveIfError(iTelServer.LoadPhoneModule(KMmtsyName)) ;
	// Open a subsession for the phone in use
	User::LeaveIfError(iPhone.Open(iTelServer,KPhoneName)) ;
	
	// Initialize the Phone
	INFO_PRINTF1(_L("Initialising the phone..."));
	
	TInt ret = iPhone.Initialise();
	if (ret!=KErrNone)
		{
		ERR_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		User::Leave(ret);
		}

	RPhone::TCaps oPhoneCaps;
	INFO_PRINTF1(_L("Get the Phone's Capabilities..."));
	ret = iPhone.GetCaps(oPhoneCaps) ;
	if ((KErrNone != ret) || !(oPhoneCaps.iFlags & RPhone::KCapsVoice))
		{
		ERR_PRINTF2(_L("This phone does not support Voice Calls (caps=0x%x)"), oPhoneCaps.iFlags);
		User::Leave(ret);
		}	

	return EPass;
	} // CTestVoiceStep::doTestStepPreambleL

TVerdict CTestVoiceStep::doTestStepPostambleL()
	{
	iPhone.Close();
	iTelServer.Close();	// disconnect from etel
	
	//
	// Check the heap for memory leaks...
	//
	__UHEAP_MARKEND;
	
	return TestStepResult();
	} // CTestVoiceStep::doTestStepPostambleL
	
