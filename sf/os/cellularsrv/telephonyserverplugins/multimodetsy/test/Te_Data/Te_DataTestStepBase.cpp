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

#include "Te_DataTestStepBase.h"
#include "Te_Data.h"

_LIT(KMmtsyName,"MM");
_LIT(KPhoneName,"GsmPhone1");
_LIT(KTestLocalNumber,"TestLocalPhoneSimNumber"); 
_LIT(KTestRemoteNumber,"TestRemotePhoneSimNumber");
_LIT(KTestGoodNumber,"TestGoodPhoneNumber");
_LIT(KGoodNumber,"1789");
_LIT(KLocalNumber, "01632960000") ;
_LIT(KRemoteNumber, "01632960000") ;
_LIT(KTSDataConfigFile,"Te_data.ini");

TVerdict CTestDataStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KMmtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}
	ret=iPhone.Open(iTelServer,KPhoneName) ;
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed open phone module"));
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
//
//	This test requires a mobile phone to be connected to the serial port
//      specified in the .cfg file for the initialise below to work
//
	ret=iPhone.Initialise();
	if (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failed to initialise the phone (%d)"), ret);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}
	RPhone::TCaps oPhoneCaps;
	ret = iPhone.GetCaps(oPhoneCaps) ;
	if ((KErrNone != ret) || !(oPhoneCaps.iFlags & RPhone::KCapsData))
		{
		INFO_PRINTF2(_L("This phone does not support data calls (caps=0x%x)"), oPhoneCaps.iFlags);
		iPhone.Close();
		iTelServer.UnloadPhoneModule(KMmtsyName);
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CTestDataStep::doTestStepPostambleL()
	{
	iPhone.Close();
	iTelServer.UnloadPhoneModule(KMmtsyName);
	iTelServer.Close();	// disconnect from etel

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CTestDataStep::WaitWithTimeoutL(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
/**
 * Timeout function
 */
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	if(KErrNone != timer.CreateLocal()) 
		return ;
	CleanupClosePushL(timer);
	
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

	CleanupStack::PopAndDestroy(); // timer
	}

//
// CTestDataStep Implementation
//


CTestDataStep::CTestDataStep()
	{}


void CTestDataStep::CleanUp(TAny *aPtr)
/**
*  Calls iLine.Close() & iCall.Close in  leave case.
* @param this poiter pass cleanup stack when calls the function
*/
	{
	 CTestDataStep* ptr = ((CTestDataStep*)aPtr) ;
	  ptr->iDataCall.Close() ;
	  ptr->iLine.Close() ;
	}


CTestDataStep::operator TCleanupItem()
/**
* Makes TCleanupItem from current instance of CTestDataStep
*/
	{
	return TCleanupItem(CTestDataStep::CleanUp,this);
    }

void CTestDataStep::TestGetCallCapabilitiesL(RCall& aCall)
/**
 * @test GT83-TDATA-016.01 Test retrieval of the call's capabilities. Display the call's capabilities to screen
*/
	{
	INFO_PRINTF1(_L("======Start test Test Call Capabilities"));

	// Call Capabilities
	RCall::TCaps callCaps;
	TRAPD(ret, (ret=aCall.GetCaps(callCaps)));
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-016.01 - *** Failed RCall::GetCaps() ***")) ;
	if (ret==KErrNone)
		Print_TCapsEnum(callCaps)	;

	INFO_PRINTF1(_L("======End test ====="));

	}

void CTestDataStep::TestLineCapabilitiesL(RMobileLine& aLine)
/**
 * @test GT83-TDATA-002.01 Test retrieval of a data line's capabilities. Display the capabilities to screen
 */
	{
	INFO_PRINTF1(_L("======Start test Test Line Capabilities"));

	// Line Capabilities
	RLine::TCaps lineCaps;
	TInt ret=aLine.GetCaps(lineCaps);
	CHECKPOINT(ret, KErrNone, _L("GT83-TDATA-002.01 - *** Failed RMobileLine::GetCaps() *** ")) ;
	if (ret==KErrNone)
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
	INFO_PRINTF1(_L("======End test ====="));
	}


void CTestDataStep::GetPhoneNumberL(TDes& aNum, EPhoneNumbers aPhoneType)
/**	Returns string with number for outgoing call.
	For.ex Good number is 1234 Vodaphone test line - recorded message.
	Function tries to read value of switch /modem, otherwise it use defualt value.
	@return phone number string
	@note have to be updated to read information from cmd line or config. file
 */
	{
//	TPtrC oNum ;

	// Read Te_data.ini and extract phone numbers.
	CConfigParams* testConfigParams;

	TBool configfileexist = EFalse;

	testConfigParams = CConfigParams::NewL(KTSDataConfigFile());
	CleanupStack::PushL(testConfigParams);

	const TPtrC goodnumber = testConfigParams->FindAlphaVar(KTestGoodNumber, KNullDesC);
	const TPtrC localnumber = testConfigParams->FindAlphaVar(KTestLocalNumber, KNullDesC);
	const TPtrC remotenumber = testConfigParams->FindAlphaVar(KTestRemoteNumber, KNullDesC);
	if(testConfigParams->ConfigFileExist())
		{
		configfileexist = ETrue;
		}
	switch(aPhoneType)
 		{
		case EGood:
//		if(GetStringFromConfig(KPhoneNumbersSection, KSwitchGood, oNum))
//			{
//			aNum.Copy(oNum) ;
//			}
		if(configfileexist)
			{
			aNum.Copy(goodnumber) ;
			}
		else
			{
			aNum.Copy(KGoodNumber) ;
			}
		break;

		case ERemote:
//		if(GetStringFromConfig(KPhoneNumbersSection, KRemoteNumber, oNum))
//			{
//			aNum.Copy(oNum) ;
//			}
		if(configfileexist)
			{
			aNum.Copy(remotenumber) ;
			}
		else
			{
			aNum.Copy(KRemoteNumber) ;
			}
		break;

		case ELocal:
//		if(GetStringFromConfig(KPhoneNumbersSection, KLocalNumber, oNum))
//			{
//			aNum.Copy(oNum) ;
//			}
		if(configfileexist)
			{
			aNum.Copy(localnumber) ;
			}
		else
			{
			aNum.Copy(KLocalNumber) ;
			}
		break;

		default:
			aNum.Copy(KGoodNumber) ;
		break;
		}
	CleanupStack::PopAndDestroy(testConfigParams);
	}

void CTestDataStep::TestLineCapabilitiesL()
/**
 * @test GT83-TVOICE-002.01 Test retrieval of a voice line's capabilities. Display the capabilities to screen
*/
	{
	INFO_PRINTF1(_L("======Start test Test Line Capabilities"));

	// Line Capabilities
	RLine::TCaps lineCaps;
	TInt ret=iLine.GetCaps(lineCaps);
	CHECKPOINT(ret, KErrNone, _L("GT83-TVOICE-002.01 - *** Failed RMobileLine::GetCaps() *** ")) ;
	if (ret!=KErrNone)
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
	INFO_PRINTF1(_L("======End test ====="));
	}


void CTestDataStep::Print_RCall_TStatus(RCall::TStatus& aArg)

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

void CTestDataStep::Print_TCapsEnum(RCall::TCaps& aArg)
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
