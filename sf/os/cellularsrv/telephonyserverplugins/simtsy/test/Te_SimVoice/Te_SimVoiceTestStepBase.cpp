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
//

#include "Te_SimVoiceTestStepBase.h"

TVerdict CSimVoiceTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Failed resetting"));
		User::Leave(ret);
		}

	ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to open phone module"));
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to initialise the phone"));
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	RPhone::TCaps phoneCaps;
	ret = iPhone.GetCaps(phoneCaps) ;
	if ((KErrNone != ret) || !(phoneCaps.iFlags & RPhone::KCapsVoice))
		{
		INFO_PRINTF2(_L("This phone does not support Voice Calls (caps=0x%x)"), phoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KSimtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSimVoiceTestStepBase::doTestStepPostambleL()
	{
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimVoiceTestStepBase::SetState(TInt aState)
	{
	iState = aState;
	}

void CSimVoiceTestStepBase::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
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


void CSimVoiceTestStepBase::GetGoodNumber(TDes& aNum)
/**Returns string with number for outgoing call.
 * For.ex Good number is 179 Vodaphone test line - recorded message.
 * Function tries to read value of switch /modem, otherwise it use defualt value.
 * @return phone number string
 * @note have to be updated to read information from cmd line or config. file
*/
	{
	aNum.Copy(KGoodNumber) ;
	}

TInt CSimVoiceTestStepBase::CallMeDear()
/**
* Asks for  incomming call
* @return KErrNone if could make it
*/
	{
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall, iState);
	INFO_PRINTF2(_L("Set Incoming Call Publish & Subscribe property result: %d"),ret) ;
	return ret;
	}

TInt CSimVoiceTestStepBase::HangMeUpDear()
/**
* Asks for remote hangup
* @return KErrNone if could make it
*/
	{
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyRemoteHangup, iState);
	INFO_PRINTF2(_L("Set Remote Hangup Publish & Subscribe property result: %d"),ret) ;
	return ret;
	}

void CSimVoiceTestStepBase::Print_RCall_TStatus(RCall::TStatus& aArg)

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

void CSimVoiceTestStepBase::Print_RLine_TCapsFlags(RLine::TCapsFlags& aArg)
/** Display value of enum RLine::TCapsFlags to screen.
 * @param value to print
*/
	{
	if(aArg & RLine::KCapsData)
		{
			INFO_PRINTF1(_L("value is TCapsFlags::KCapsData")) ;
		}
	if(aArg & RLine::KCapsFax)
		{
			INFO_PRINTF1(_L("value is TCapsFlags::KCapsFax")) ;
		}
	if(aArg & RLine::KCapsVoice)
		{
			INFO_PRINTF1(_L("value is TCapsFlags::KCapsVoice")) ;
		}
	if(aArg & RLine::KCapsEventIncomingCall)
		{
			INFO_PRINTF1(_L("value is TCapsFlags::KCapsEventIncomingCall")) ;
		}
	}

void CSimVoiceTestStepBase::Print_TCapsEnum(RCall::TCaps& aArg)
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

void CSimVoiceTestStepBase::TestGetCallCapabilitiesL(RMobileCall& aCall)
/**
 * call's capabilities to screen
 * @test GT83-TVOICE-016.01 "Test retrieval of the current call status when the call's status is ""Unknown"""
 * @test GT83-TVOICE-032.01 Test retrieval of the voice call's dynamic call control and call event capabilities. Display the call's capabilities to screen
*/
	{
	INFO_PRINTF1(_L("Test Call Capabilities"));

	// Call Capabilities
	RCall::TCaps callCaps;
	TRAPD(ret, ret=aCall.GetCaps(callCaps));
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-017.01")) ;
	if (ret!=KErrNone)
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
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-032.01.01")) ;

	if (ret==KErrNotSupported)
		{
		INFO_PRINTF1(_L("Get Mobile Call Capabilities is NOT Supported"));
		}

	if (ret==KErrNone)
		{
		INFO_PRINTF1(_L("Get Mobile Call Capabilities is Successful"));
		PrintCallCaps(mobileCallCaps.iCallControlCaps);
		}
	}

void CSimVoiceTestStepBase::PrintCallCaps(TUint32 aCaps)
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

void CSimVoiceTestStepBase::TestLineCapabilitiesL()
/**
 * @test GT83-TVOICE-002.01 Test retrieval of a voice line's capabilities. Display the capabilities to screen
*/
	{
	INFO_PRINTF1(_L("Test Line Capabilities"));

	// Line Capabilities
	RLine::TCaps lineCaps;
	TInt ret=iLine.GetCaps(lineCaps);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-002.01")) ;
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

void CSimVoiceTestStepBase::Print_TMobilePhoneModeCaps(RMobilePhone::TMobilePhoneModeCaps& aArg)
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
