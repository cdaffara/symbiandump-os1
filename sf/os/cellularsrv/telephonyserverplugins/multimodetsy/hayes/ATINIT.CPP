// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AT Initialisation
// 
//

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include "ATINIT.H"
#include "NOTIFY.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "CALL.H"
#include "ATNOCARR.H"
#include "mPHBOOK.H"
#include "ATIO.H"
#include "Matstd.h"
#include "ATHANGUP.H"
#include <etelqos.h>

_LIT8(KTestPhoneBookStorage,"AT+CPBS=?");
_LIT8(KSetExtendedCallIndicationCommand,   "AT+CRC=1\r");
_LIT8(KClearUnsolicitedNetworkRegistration, "AT+CREG=0\r");
_LIT8(KSetUnsolicitedNetworkRegistration2,  "AT+CREG=2\r");
_LIT8(KSetUnsolicitedNetworkRegistration1,  "AT+CREG=1\r");
_LIT8(KReadUnsolicitedNetworkRegistration,  "AT+CREG?\r");
_LIT8(KGetCurrentOperatorCommand, "AT+COPS?\r");
_LIT8(KGetBatteryInfoCommand, "AT+CBC\r");
_LIT8(KGetSignalInfoCommand, "AT+CSQ\r");
_LIT8(KOperatorResponse,"+COPS:");
_LIT8(KGetServiceCentreAddressCommand,"AT+CSCA?\r");

_LIT8(KSetUnsolicitedGPRSNetworkRegistration1,  "AT+CGREG=1\r");
_LIT8(KSetECAMConfigCommand, "AT*ECAM=1\r");

_LIT(KSpaceSeparator," ");

_LIT8(KClassA,"A");
_LIT8(KClassB,"B");
_LIT8(KClassC,"C");
_LIT8(KClassCG,"CG");
_LIT8(KClassCC,"CC");
_LIT16(KEricsson,"*ERICSSON*");

const TInt KMaxNoRetries=3;

const TUint KZeroChar='0';
const TUint KOneChar='1';


#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATInit::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif



CATInit* CATInit::NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	{
	CATInit* self = new(ELeave) CATInit(aIo,aTelObject,aPhoneGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATInit::CATInit(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	: CATBase(aIo,aTelObject,aPhoneGlobals)
	{}

void CATInit::ConstructL()
	{
	iEnumerate = CATSmsMemoryStorage::NewL(iIo, iTelObject, this, iPhoneGlobals);
	}

CATInit::~CATInit()
	{
	iIo->RemoveExpectStrings(this);
	delete iEnumerate;
	}

void CATInit::StartInit(CATCommands* aPendingCommand,TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	SetToNotInitialised();	
	iPendingCommand = aPendingCommand;
	iReqHandle = aTsyReqHandle;
	iParams = aParams;
	iCallInfo = aPendingCommand->CallInfo();	// iCallInfo is NULL if 
												// aPendingCommand has no iCallInfo
	iIo->Cancel();
	iAttemptCnt=1;
	InitAttempt();
	}

TBool CATInit::CheckActiveReqHandle(const TTsyReqHandle aTsyReqHandle)
//
// Check the Request against the current request handle
//
	{
	if((iPhoneGlobals->iPhoneStatus.iInitStatus==EPhoneInitialising)&&
	   (aTsyReqHandle==iReqHandle))
		return ETrue;
	else
		return EFalse;
	}

void CATInit::StopInit(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("Cancelling Initialisation"));
	iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneInitialiseCancelling;
	iIo->SetTimeOut(this,KOneSecondPause);
	}

void CATInit::SpecialStart(TTsyReqHandle aTsyReqHandle)
//
//	If aTsyReqHandle is NULL, this is an Automatic Init with no ReqComplete to call
//	Otherwise it is from CPhoneHayes::ControlledInitialise() which should be completed
//
	{
	iReqHandle = aTsyReqHandle;
	iPendingCommand = NULL;
	iAttemptCnt=1;
	InitAttempt();
	}

void CATInit::InitAttempt()
	{
	if (iPhoneGlobals->iPhoneStatus.iInitStatus == EPhoneInitialiseCancelling)
		return;	// "initialise" may have been cancelled while we were waiting for the call back.

	LOGTEXT2(_L8("Starting Initialisation Sequence, Attempt %d"),iAttemptCnt);
	iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneInitialising;
	iPhoneGlobals->iSmsPduModeSupported=EFalse;
	TCommConfig aConfigPckg;
	iIo->Cancel();
	TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypePreInit);
	if (ret==KErrNone)
		ret = iIo->ConfigurePort(aConfigPckg);
	if (ret)
		{
		Complete(ret);
		return;
		}
	iIo->SetTimeOut(this,100);
	iState=EShortTimeOutFix;
	}
	
void CATInit::RetryOrFail(TBool aCommsErrorOccurred)
	{
	if (!aCommsErrorOccurred)
		iIo->RemoveExpectStrings(this);	
	iConnectExpectString = NULL;
	iNoCarrierExpectString = NULL;
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	if(++iAttemptCnt > (TUint)KMaxNoRetries)
		{
		LOGTEXT(_L8("Cannot Initialise"));
		iIo->RemoveExpectStrings(this);
		if (iPhoneGlobals->iPhoneStatus.iModemDetected != RPhone::EDetectedPresent)
			Complete(KErrEtelModemNotDetected);
		else
			Complete(KErrEtelInitialisationFailure);
		}
	else
		{
		iIo->Cancel();
		InitAttempt();
		}	
	}

void CATInit::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
//
//	CATIO removes expect strings in the event of an error from the port, so set ptrs to NULL.
//	If error is KErrCommsFrame and this is the first attempt at initialising, give the modem
//	benefit of the doubt and try again.
//
	{
	if (aStatus==KErrCommsFrame && iAttemptCnt==1)
		RetryOrFail(ETrue);
	else
		{
		iNoCarrierExpectString = NULL;
		iConnectExpectString = NULL;
		iOKExpectString = NULL;
		iErrorExpectString = NULL;
		Complete(aStatus);
		}
	}

void CATInit::ValidateATHExpectStringL()
	{
	if(iIo->FoundChatString()==iErrorExpectString)
		{
		LOGTEXT(_L8("Modem returned ERROR in response to command"));
		User::Leave(KErrGeneral);
		}
	if(iIo->FoundChatString()==iNoCarrierExpectString)
		{
		LOGTEXT(_L8("Modem returned NO CARRIER in response to command"));
		return;
		}
	if(iIo->FoundChatString()==iOKExpectString)
		return;
	LOGTEXT(_L8("Modem returned unexpected response to command"));
	User::Leave(KErrUnknown);
	}

void CATInit::EventSignal(TEventSource aSource)
	{
	if(aSource==ETimeOutCompletion && iState==EWaitForDSR)
		{
		LOGTEXT(_L8("CATInit::EventSignal  Ignoring timeout during EWaitForDSR"));
		iIo->WriteAndTimerCancel(this);
 		iIo->RemoveExpectStrings(this);
		}
	else if ((aSource==ETimeOutCompletion)
	  &&(iPhoneGlobals->iPhoneStatus.iInitStatus != EPhoneInitialiseCancelling)
	  &&(iState!=EATWaitForATCheckOK)&&(iState!=EShortTimeOutFix))
		{
		LOGTEXT(_L8("CATInit::EventSignal  Error during Initialisation Sequence"));
		RetryOrFail();
		return;
		}
	
	if (iPhoneGlobals->iPhoneStatus.iInitStatus == EPhoneInitialiseCancelling)
		{
		if (aSource==EWriteCompletion)
			{
			iIo->SetTimeOut(this,KOneSecondPause);
			}
		if (aSource==EReadCompletion || aSource==ETimeOutCompletion)
			{
			LOGTEXT2(_L8("Entered InitCancelling with aSource=%d"),aSource);
			iIo->WriteAndTimerCancel(this);
			iIo->RemoveExpectStrings(this);
			iOKExpectString=NULL;
			iErrorExpectString=NULL;
			iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneNotInitialised;
			if (iPendingCommand)
				{
				if (iCallInfo)
					REINTERPRET_CAST(CCallHayes*,iPendingCommand->Owner())->SetToIdle();
				iPendingCommand->Owner()->ReqCompleted(iReqHandle,KErrCancel);
				iPendingCommand=NULL;
				}
			else
				{
				iTelObject->ReqCompleted(iReqHandle,KErrCancel);
				return;
				}
			if ((iCallInfo != NULL) && (iCallInfo->iClientPanicOccurred!=ENoPanicOccurred))
				{
				__ASSERT_DEBUG(iPendingCommand!=NULL,Panic(ERelinquishOwnershipWithNoCall));
				iComplete = CCompleteRelinquish::New(iPendingCommand->Owner());
				iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
				iComplete->Call();	// calls the AysncOneShot Relinquish completion function
				iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
				}	
			}
		return;
		}

	switch(iState)
		{
	case EShortTimeOutFix:
		//
		//	Calling iIo->Start() synchronously inside the InitAttempt() function caused a 
		//  hang when using IrDA on the ARM, and Netdial, as Netdial is in the ESock thread
		//  and is blocking waiting for its RTelSubSession::Open() to complete, but iIo->Start()
		//	calls C32 which calls ESock. So introduced this short time out.
		//
		//	Also retrieving the strings here at the start of the initialisation - the first
		//	time we get back KErrAccessDenied for some reason.
		//
		{
		iIo->Cancel();
		iIo->Start(this);		// ramps up DTR
		iIo->SetTimeOut(this,5 * KOneSecondPause);	// should be enough time for a battery-powered
													// modem to initialise itself
		__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
		iState=EWaitForDSR;
		}
		break;
		
	case EWaitForDSR:
 		// Do not assert aSource==EWriteCompletion as we could have got here with a ETimeOutCompletion
		{
		if (!(iIo->ReadPending()))		// Otherwise CATIO will queue a read on the way back down the stack anyway
			iIo->Read();
		Write(KAT2Command(),5);		// needs to be long because IR takes a while 
		iState=EWaitForATHangUpWriteComplete;
		}
		break; //ATHcommand is now an AT. Introduced 23/03/99: Fix ER5-defect EDNJWAD-465K6M

	case EWaitForATHangUpWriteComplete:
		StandardWriteCompletionHandler(aSource,5);
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString = iIo->AddExpectString(this,KNoCarrierString);
		iState=EWaitForATHangUpOK;
		break;

	case EWaitForATHangUpOK:
		{
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));

		// Fix defect CLE-4RGN8Z, NM, 9/01/01.
		// Fix defect SIH-4UCMW9, that actually fixes the problem caused by
		// previous CLE-4RGN8Z fix !  KAS 2/03/01 
		if (iPhoneGlobals->iPhoneStatus.iModemDetected != RPhone::EDetectedPresent) 
			{
			iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,EPhoneDetected);
			iPhoneGlobals->iPhoneStatus.iModemDetected = RPhone::EDetectedPresent;
			}

		TRAPD(ret,ValidateATHExpectStringL());
		if (ret)
			{
			RetryOrFail();
			break;
			}
		iIo->WriteAndTimerCancel(this);
		RemoveStdExpectStrings();
		iIo->RemoveExpectString(iConnectExpectString);
		iConnectExpectString = NULL;
		iIo->RemoveExpectString(iNoCarrierExpectString);
		iNoCarrierExpectString=NULL;
		TCommConfig aConfigPckg;
		iIo->Cancel();
		ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeInit);
		if (ret==KErrNone)
			ret = iIo->ConfigurePort(aConfigPckg);
		if (ret)
			{
			Complete(ret);
			return;
			}

		//
		// iCallInfo is NULL if this hasn't been originated by a CCallHayes command,
		// eg using CPhoneHayes::Init()
		//
		// Don't bother notifying about hook change here as it's likely only to be from 
		// unknown to on.
		//
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,EBecomeIdle);
		REINTERPRET_CAST(CPhoneHayes*,iTelObject)->SetHookStatus(RCall::EHookStatusOn);
		iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeIdle;
		TBuf8<KMaxLengthOfUserDefinedInitString> initCommand;
			initCommand=KAT2Command;			
		Write(initCommand,5);
		iState=EWaitForModemInitWrite;
		}
		break;

	case EWaitForModemInitWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EWaitForModemInitOK;
		break;

	case EWaitForModemInitOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			if (ret)
				{
				RetryOrFail();
				break;
				}
			RemoveStdExpectStrings();
			iIo->WriteAndTimerCancel(this);	
			TPtrC8 atCheck(KAT2Command);		// modem may take time to complete user-defined
											// Init string, so send an AT once and if this
											// fails try again twice
			Write(atCheck,5);
			iState=EWaitForATCheckWrite;
			}
		break;

	case EWaitForATCheckWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EATWaitForATCheckOK;
		break;

	case EATWaitForATCheckOK:
		{
		if (aSource==ETimeOutCompletion)
			{
			RemoveStdExpectStrings();
			if (iRetryCounter++ == (TUint)KMaxNoChecks)
				{
				iRetryCounter=0;
				Complete(KErrTimedOut);
				}
			else
				{
				TPtrC8 atCheck(KAT2Command);
				Write(atCheck,5);
				iState=EWaitForATCheckWrite;
				}
			return;
			}
		TInt ret(ValidateExpectString());
		if (ret)
			{
			RetryOrFail();
			break;
			}
		RemoveStdExpectStrings();
		WriteExpectingResults(KGetPhoneCapsCommand(),5);
		iState=EWaitForATGetPhoneCapsWrite;
		}
		break;

	case EWaitForATGetPhoneCapsWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EWaitForATGetPhoneCapsOK;
		break;

	case EWaitForATGetPhoneCapsOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			if(ret==KErrGeneral)
				ParseAnErroredPhoneCaps();
			else if(ret!=KErrNone)
				{	
				RetryOrFail();
				break;
				}
			else
				{
				iIo->WriteAndTimerCancel(this);
				TRAP(ret,ParsePhoneCapsL());
				if (ret)
					{	
					iIo->RemoveExpectStrings(this);
					Complete(KErrEtelInitialisationFailure);
					break;
					}
				}
			RemoveStdExpectStrings();
			TBuf8<KCommsDbSvrMaxFieldLength> echoSetting;
			TBuf8<KCommsDbSvrMaxFieldLength> quietSetting;
			TBuf8<KCommsDbSvrMaxFieldLength> verboseSetting;
			ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameEchoOff),echoSetting);
			if (ret==KErrNone)
				{
				ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameQuietOff),quietSetting);
				}
			if (ret==KErrNone)
				{
				ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameVerboseText),verboseSetting);
				}
			if (ret)
				{
				iIo->RemoveExpectStrings(this);
				Complete(ret);
				break;
				}
			iTxBuffer.Format(_L8("AT%S%S%S\r"),&echoSetting, &quietSetting, &verboseSetting);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,5*KOneSecondPause);
			iState=EWaitForATStandardInitWrite;
			}
		break;

	case EWaitForATStandardInitWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EWaitForATStandardInitOK;
		break;

	case EWaitForATStandardInitOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret(ValidateExpectString());
			if (ret)
				{
				RetryOrFail();
				break;
				}
			iIo->WriteAndTimerCancel(this);
			RemoveStdExpectStrings();
			TPtrC8 interrogateWaitTime(KSmsSetTABufferMode);
			WriteExpectingResults(interrogateWaitTime,5);
			iState=EWaitForATUnsolicitedFixWrite;
			}
		break;

	case EWaitForATUnsolicitedFixWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EWaitForATUnsolicitedFixOK;
		break;

	case EWaitForATUnsolicitedFixOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			// Ensures that unsolicited messages are buffered by the TA during initialisation
			// We dont care what this returns becasue if AT+CNMI= fails, it is unlikely
			// that SMS's can received at all.
			iIo->WriteAndTimerCancel(this);
			RemoveStdExpectStrings();
			TPtrC8 interrogateWaitTime(KGetCarrierWaitTimeCommand);
			WriteExpectingResults(interrogateWaitTime,5);
			iState=EWaitForATCarrierWaitTimeWrite;
			}
		break;

	case EWaitForATCarrierWaitTimeWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EWaitForATCarrierWaitTimeOK;
		break;

	case EWaitForATCarrierWaitTimeOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		//
		//	if modem does not support ATS7 (carrier wait time) set it to default value
		//
			{
			TInt ret(ValidateExpectString());
			if (ret==KErrNone)
				TRAP(ret,ParseWaitTimeL());
			if (ret)
				iPhoneGlobals->iPhoneStatus.iWaitForCarrierTime = KDefaultSecondsToWaitForCarrier;
			iIo->WriteAndTimerCancel(this);	
			RemoveStdExpectStrings();
			Write(KAutoAnswerCommand(),5,KDefaultAutoAnswer);
			iState=EWaitForATAutoAnswerTimeWrite;
			}
		break;

	case EWaitForATAutoAnswerTimeWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EWaitForATAutoAnswerTimeOK;
		break;

	case EWaitForATAutoAnswerTimeOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);	
			RemoveStdExpectStrings();
			//
			//
			//All the MM initialization stuff starts now//
			//
			// 
			iPhoneGlobals->iPhoneTestState = CPhoneGlobals::EPhoneTestUndefined; //New-ETSI-Phone or Old-ETSI-Phone? 

			iTxBuffer.Copy(KGetManufacturerIDCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForSendingCGMIRequest;
			}
		break;	

	case EATWaitForSendingCGMIRequest:
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource,5);
		iState = EATWaitForSendingCGMIRequestComplete;
		break;

	case EATWaitForSendingCGMIRequestComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret == KErrNone)
				{
				iCGMIFlag = ETrue;
				__ASSERT_ALWAYS(GetIdentityResponse() == KErrNone,Panic(EGeneral));
				iCGMIFlag = EFalse;
				iPhoneGlobals->iPhoneIdCaps |= RMobilePhone::KCapsGetManufacturer;
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\tCGMI query returned ERROR. Setting NULL TBuf8"));
				iPhoneGlobals->iPhoneId.iManufacturer.Zero();
				}
			iTxBuffer.Copy(KGetModelIDCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForSendingCGMMRequest;
			}
		break;
		
	case EATWaitForSendingCGMMRequest:
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource,5);
		iState = EATWaitForSendingCGMMRequestComplete;
		break;
	
	case EATWaitForSendingCGMMRequestComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret == KErrNone)
				{
				iCGMMFlag = ETrue;
				LOGTEXT(_L8("MMTsy:\tEntering CGMMResponse TRAPD' function"));
				__ASSERT_ALWAYS(GetIdentityResponse() == KErrNone,Panic(EGeneral));
				iCGMMFlag = EFalse;
				iPhoneGlobals->iPhoneIdCaps |= RMobilePhone::KCapsGetModel;
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\tCGMM query returned ERROR. Setting NULL TBuf8"));
				iPhoneGlobals->iPhoneId.iModel.Zero();
				}
			iTxBuffer.Copy(KGetRevisionIDCommand);
			iIo->SetTimeOut(this, 5000);
			iIo->Write(this, iTxBuffer);
			iState = EATWaitForSendingCGMRRequest;
			}
		break;

	case EATWaitForSendingCGMRRequest:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState = EATWaitForSendingCGMRRequestComplete;
			}
		break;

	case EATWaitForSendingCGMRRequestComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret == KErrNone)
				{
				iCGMRFlag = ETrue;
				__ASSERT_ALWAYS(GetIdentityResponse() == KErrNone,Panic(EGeneral));
				iCGMRFlag = EFalse;
				iPhoneGlobals->iPhoneIdCaps |= RMobilePhone::KCapsGetRevision;
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\tCGMR query returned ERROR. Setting NULL TBuf8"));
				iPhoneGlobals->iPhoneId.iRevision.Zero();
				}
			iTxBuffer.Copy(KGetSerialNumberIDCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForSendingCGSNRequest;
			}
		break;


	case EATWaitForSendingCGSNRequest:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState = EATWaitForSendingCGSNRequestComplete;
			}
		break;

	case EATWaitForSendingCGSNRequestComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret == KErrNone)
				{
				iCGSNFlag = ETrue;
				__ASSERT_ALWAYS(GetIdentityResponse() == KErrNone,Panic(EGeneral));
				iCGSNFlag = EFalse;
				iPhoneGlobals->iPhoneIdCaps |= RMobilePhone::KCapsGetSerialNumber;
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\tCGSN query returned ERROR. Setting NULL TBuf8"));
				iPhoneGlobals->iPhoneId.iSerialNumber.Zero();
				}

			LOGTEXT(_L8("MMTsy:\tGrabbing the subscriber id capabilities"));
			WriteExpectingResults(KGetSubscrieberIDCommand(), 5); 
			iState=EATGetSubscriberIdWriteComplete;
			}
		break;

	case EATGetSubscriberIdWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetSubscriberIdReadComplete;
			}
			break;
                
	case EATGetSubscriberIdReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+CIMI not supported"));
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+CIMI supported"));
				iPhoneGlobals->iPhoneIdCaps |= RMobilePhone::KCapsGetSubscriberId;
				}

			LOGTEXT(_L8("MMTsy:\tGrabbing the SMS <mode> capabilities"));
			WriteExpectingResults(KSmsGetModeCommand(), 5); 
			iState=EATWaitForSendingCMGFRequestWrite;

			}
			break;		

	case EATWaitForSendingCMGFRequestWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EATWaitForSendingCMGFRequestComplete;
		break;

	case EATWaitForSendingCMGFRequestComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);	
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			iPhoneGlobals->iSmsPduModeSupported=EFalse;
			if(ret==KErrNone)
				{
				TRAP(ret,CheckCMGFResponseL());
				}
			if((ret==KErrNone)&&(iPhoneGlobals->iSmsPduModeSupported))
				{
				LOGTEXT(_L8("MMTsy:\tPdu mode is supported by ME. Enabling this as the default mode."));
				WriteExpectingResults(KSetSmsModeSupportCommand, 5);
				iState = EATWaitForSendingCMGFConfigureWrite;
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tPdu mode not supported by ME. Sending a dummy \"AT\" instead."));
				WriteExpectingResults(KATCommand, 5);
				iState = EATWaitForSendingCMGFConfigureWrite;
				}
			} //case
		break;
	
	case EATWaitForSendingCMGFConfigureWrite:
		StandardWriteCompletionHandler(aSource,5);
		iState=EATWaitForCMGFResponseOKComplete;
		break;

	case EATWaitForCMGFResponseOKComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			if (ret)
				{
				RetryOrFail();
				break;
				}
			RemoveStdExpectStrings();
		
		// CMGF is taken care of... mode capabilites assessed, and Pdu mode (for now) has been assigned to ME. (Mobile Equipment)
			iTxBuffer.Copy(KGetPrefMemCommand);	// Preferred Memory Storage cmd. ETSI std GSM 07.05, May 1996
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForSendingEnumRequest;
			} //case
		break;

	case EATWaitForSendingEnumRequest:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
		{
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource, 5);
		iState = EATWaitForSendingEnumRequestComplete;
		}
		break;

	case EATWaitForSendingEnumRequestComplete:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
		{
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		if (ret!=KErrNone)
			{
			LOGTEXT(_L8("MMTsy:\tCATInit:\tAT+CPMS=? command not supported by this phone"));
			LOGTEXT(_L8("MMTsy:\tCATInit:\tThis situation should NOT occur for a MM(GSM) phone as this command is mandatory!"));
			}
		else
			{
			TRAPD(ret, iEnumerate->EnumerateCPMSResponseL());
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\tError Enumerating Message Stores"));
				RetryOrFail();
				break;
				}
			}
		RemoveStdExpectStrings();

		iTxBuffer.Copy(KGetPrefMemTestCommand);
		iIo->Write(this, iTxBuffer);
		iIo->SetTimeOut(this, 5000);
		iState = EATWaitForGetPrefMemWriteComplete;				
		}
		break;

	case EATWaitForGetPrefMemWriteComplete:
		StandardWriteCompletionHandler(aSource, 5);
		iState=EATWaitForGetPrefMemReadComplete;
		break;

	case EATWaitForGetPrefMemReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
		{
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		if (ret!=KErrNone)
			{
			LOGTEXT(_L8("MMTsy:\tCATInit:\tAT+CPMS? command not supported by this phone"));
			LOGTEXT(_L8("MMTsy:\tCATInit:\tThis situation should NOT occur for a MM/(GSM) phone as this command is mandatory!"));
			}
		else
			{
			TRAPD(ret, iEnumerate->GetPrefMemL());
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\tError Determining Pref'd Memory"));
				RetryOrFail();
				break;
				}
			}
		RemoveStdExpectStrings();

		Write(KTestPhoneBookStorage(), 5);
		iState=EATWaitForTestPhoneBookStorageWriteComplete;
		}
		break;

	case EATWaitForTestPhoneBookStorageWriteComplete:
		StandardWriteCompletionHandler(aSource,5);
		iState=EATWaitForTestPhoneBookStorageRequestComplete;
		break;

	case EATWaitForTestPhoneBookStorageRequestComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			if (ret!=KErrNone)
				LOGTEXT(_L8("CATInit:\tAT+CPBS command not supported by this phone. Skipping to Network Registration"));
			else
				{
				TRAP(ret,ParsePhoneBookStorageL());
				if (ret!=KErrNone)
					{
					RetryOrFail();
					break;
					}
				}
			RemoveStdExpectStrings();

			// Start waiting for asynchronous notification
			// Some notifications may be ignored until the phone is fully initialized.
			STATIC_CAST(CPhoneHayes*,iTelObject)->StartWaitForRing(); 

			iTxBuffer.Copy(KClearUnsolicitedNetworkRegistration);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForClearUnsolicitedNetworkRegistrationWrite;
			}
		break;


	case EATWaitForClearUnsolicitedNetworkRegistrationWrite:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState = EATWaitForClearUnsolicitedNetworkRegistrationOK;
			}
		break;

	case EATWaitForClearUnsolicitedNetworkRegistrationOK:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATWaitForCall:\t+CREG=0 failed"));
				}
			else
				{
				// Getting the network capabilities that AT+CREG supplies 
				// The phone supports retrieval of current registration status.
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsGetRegistrationStatus; //0x00000001
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsGetCurrentMode; //0x00000004
				}

			iTxBuffer.Copy(KSetUnsolicitedNetworkRegistration2);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForSetUnsolicitedNetworkRegistrationWrite;
			}
		break;

	case EATWaitForSetUnsolicitedNetworkRegistrationWrite:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState = EATWaitForSetUnsolicitedNetworkRegistrationOK;
			}
		break;

	case EATWaitForSetUnsolicitedNetworkRegistrationOK:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				if (iTxBuffer==KSetUnsolicitedNetworkRegistration2)
					{
					// Some phones do not support +CREG=2 (e.g SH888)
					// Try the fallback position with +CREG=1

					iTxBuffer.Copy(KSetUnsolicitedNetworkRegistration1);
					iIo->Write(this, iTxBuffer);
					iIo->SetTimeOut(this, 5000);
					iState = EATWaitForSetUnsolicitedNetworkRegistrationWrite;
					break;
					}
				LOGTEXT(_L8("MMTsy:\tCATInit:\tC+CREG failed"));
				}

			if(ret == KErrNone) // If +CREG=2 or +CREG=1 were succesfully executed the capabilities can be set. 
				{
				// The phone supports notify registration status.
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsNotifyRegistrationStatus;

				// The phone supports notify mode.
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsNotifyMode;
				}
			
			iTxBuffer.Copy(KReadUnsolicitedNetworkRegistration);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForReadUnsolicitedNetworkRegistrationWrite;
			}
		break;

	case EATWaitForReadUnsolicitedNetworkRegistrationWrite:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState = EATWaitForReadUnsolicitedNetworkRegistrationOK;
			}
		break;

	case EATWaitForReadUnsolicitedNetworkRegistrationOK:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{				
				LOGTEXT(_L8("MMTsy:\tCATInit:\tRead Network Registration Not Supported"));				
				}
			iTxBuffer.Copy(KSetExtendedCallIndicationCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState = EATWaitForSetExtendedCallIndicationWrite;
			}
		break;

	case EATWaitForSetExtendedCallIndicationWrite:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState = EATWaitForSetExtendedCallIndicationOK;
			}
		break;

	case EATWaitForSetExtendedCallIndicationOK:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATWaitForCall:\t+CRC=1 failed"));
				}
			iTxBuffer.Copy(KGetCurrentOperatorCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetCurrentOperatorWriteComplete;
			}
		break;
	
	case EATGetCurrentOperatorWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetCurrentOperatorReadComplete;
			}
			break;

	case EATGetCurrentOperatorReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+COPS? not supported"));
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+COPS? supported"));

				// Getting the network capabilities that AT+COPS supplies.
				// The phone supports get/notify current network information.
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsGetCurrentNetwork; //0x00000010
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsNotifyCurrentNetwork; //0x00000020

				// The phone supprts retrieval of a list of detected networks.
				iPhoneGlobals->iNetworkCaps |= RMobilePhone::KCapsGetDetectedNetworks; //0x00000080

				TRAPD(ret,ParseOperatorL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("MMTsy:\tCATInit:\tError parsing +COPS?"));
					}								
				}
			iTxBuffer.Copy(KGetBatteryInfoCommand); // Start the sequence of getting the battery capabilities.
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetBatteryInfoWriteComplete;
			}
			break;

	case EATGetBatteryInfoWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetBatteryInfoReadComplete;
			}
			break;
                
	case EATGetBatteryInfoReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+CBC not supported"));
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+CBC supported"));		
				iPhoneGlobals->iBatteryCaps=RMobilePhone::KCapsGetBatteryInfo;
				}
			iTxBuffer.Copy(KGetSignalInfoCommand); // Start the sequence of getting the signal capabilities.
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetSignalInfoWriteComplete;

			}
			break;

	case EATGetSignalInfoWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetSignalInfoReadComplete;
			}
			break;
                
	case EATGetSignalInfoReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+CSQ not supported"));
				}
			else
				{
				LOGTEXT(_L8("MMTsy:\tCATInit:\t+CSQ supported"));		
				iPhoneGlobals->iSignalCaps=RMobilePhone::KCapsGetSignalStrength;
				}
			iTxBuffer.Copy(KCurrentCGCLASSCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetCurrentMSCLASSWriteComplete;
			}
			break;

	case EATGetCurrentMSCLASSWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetCurrentMSCLASSReadComplete;
			}
			break;

	case EATGetCurrentMSCLASSReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("CATInit:\t+CGCLASS? not supported"));
				// Reset the global settings affected by this AT command
				iPhoneGlobals->iMSClass = RPacketService::EMSClassUnknown;

				}
			else
				{
				LOGTEXT(_L8("CATInit:\t+CGCLASS? supported"));
				TRAPD(ret,ParseCurrentMsClassL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("CATInit:\tError parsing +CGCLASS?"));
					}								
				}
			iTxBuffer.Copy(KMaxCGCLASSCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetMaxMSCLASSWriteComplete;
			}
		break;


	case EATGetMaxMSCLASSWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetMaxMSCLASSReadComplete;
			}
			break;


	case EATGetMaxMSCLASSReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("CATInit:\t+CGCLASS=? not supported"));
				iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassUnknown;
				}
			else
				{
				LOGTEXT(_L8("CATInit:\t+CGCLASS=? supported"));
				TRAPD(ret,ParseMaxMsClassL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("CATInit:\tError parsing +CGCLASS=?"));
					}								
				}
			iTxBuffer.Copy(KCGQREQCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetCurrentCGQREQTWriteComplete;
			}
		break;

	case EATGetCurrentCGQREQTWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetCurrentCGQREQTReadComplete;
			}
		break;

	case EATGetCurrentCGQREQTReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("CATInit:\t+CGQREQ? not supported"));
				// Reset the global settings affected by this AT command
				iPhoneGlobals->iStaticQoSCaps.iPrecedence = 0;
				iPhoneGlobals->iStaticQoSCaps.iDelay = 0;
				iPhoneGlobals->iStaticQoSCaps.iReliability = 0;
				iPhoneGlobals->iStaticQoSCaps.iPeak = 0;
				iPhoneGlobals->iStaticQoSCaps.iMean = 0;
				iPhoneGlobals->iGprsMaxNumContexts = 0;		
				}
			else
				{
				LOGTEXT(_L8("CATInit:\t+CGQREQ? supported"));
				TRAPD(ret,ParseCGQREQResponseL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("CATInit:\tError parsing +CGQREQ?"));


					}								
				}

			iTxBuffer.Copy(KCGATTCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATGetCurrentGprsAttachStateWriteComplete;

			}
		break;
	case EATGetCurrentGprsAttachStateWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATGetCurrentGprsAttachStateReadComplete;
			}
		break;

	case EATGetCurrentGprsAttachStateReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("CATInit:\t+CGATT? not supported"));
				iPhoneGlobals->iGprsStatus = RPacketService::EStatusUnattached;			
				}
			else
				{
				LOGTEXT(_L8("CATInit:\t+CGATT? supported"));
				TRAPD(ret,ParseCGATTResponseL());
				if (ret != KErrNone)
					{
					LOGTEXT(_L8("CATInit:\tError parsing +CGATT?"));
					}								
				}

			//
			// Now need to get the SMS receive mode capabilities of the phone.
			// We get these by sending the command AT+CNMI=?
			iTxBuffer.Copy(KGetCNMIConfigCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATWaitForSendingCNMIRequest;
			}
		break;

	case EATWaitForSendingCNMIRequest:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATWaitForSendingCNMIComplete;
			}
		break;

	case EATWaitForSendingCNMIComplete:	
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		{			// Curly brackets needed to scope ret variable
		//
		// When we get here we have just received a response from the modem in response
		// to the TSY sending AT+CNMI=?.

		//
		// Check if the modem returned ERROR
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret != KErrNone)
			{
			LOGTEXT(_L8("CATInit: +CNMI=? not supported, assuming the phone does not support SMS receive"));
			//
			// We do not have to do the below as the CBase stuff should ensure they are initialised
			// as EFalse. But the code is included below just to be safe and to make the code a bit 
			// more obvious ;-)
			iPhoneGlobals->iModemSupportsCMTIMode=EFalse;
			iPhoneGlobals->iModemSupportsCMTMode=EFalse;
			}
		else
			{
			LOGTEXT(_L8("CATInit: +CNMI=? supported"));
			//
			// ParseCNMIResponse will parse the response string and set the
			// appropriate CPhoneGlobals data. If the call leaves, then the
			// CPhoneGlobal data may or may not have valid values, so we 
			// have to manually correct them here.
			TRAPD(ret,ParseCNMIResponseL());
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("CATInit: Error parsing +CNMI=? response, assuming phone does not support SMS receive"));
				iPhoneGlobals->iModemSupportsCMTIMode=EFalse;
				iPhoneGlobals->iModemSupportsCMTMode=EFalse;
				}								
			}
		
		// 
		// Start of work-around for Ericsson phones

		// The Ericsson phones (T28 & R320 definetly) do not support 
		// CMT PDU Rx (unstored) mode even though they claim to in their
		// "AT+CNMI=?" responses.
		if(iPhoneGlobals->iPhoneId.iManufacturer.MatchF(KEricsson)==0)
			{
			LOCAL_LOGTEXT("EventSignalL","Applying Ericsson +CNMI=? work-around");
			iPhoneGlobals->iModemSupportsCMTMode=EFalse;
			}
		
		// End of work-around for Ericsson phones
		// 

		// 
		// Start of work-around for Nokia 6210 phone

		// The Nokia 6210 phones do not seem to support CMTI (receive stored)
		// SMS receive mode.
		_LIT16(KNokia,"*Nokia*");
		_LIT16(KNokia6210,"*Nokia 6210*");
		if(iPhoneGlobals->iPhoneId.iManufacturer.MatchF(KNokia)==0 &&
		   iPhoneGlobals->iPhoneId.iModel.MatchF(KNokia6210)==0)
			{
			LOCAL_LOGTEXT("EventSignalL","Applying Nokia 6210 +CNMI=? work-around");
			iPhoneGlobals->iModemSupportsCMTIMode=EFalse;
			}
		
		// End of work-around for Nokia 6210 phones
		// 

	

		
		//
		// Summarise the phone capabilties to the log file
		LOGTEXT3(_L8("CATInit: iModemSupportsCMTIMode:%d iModemSupportsCMTMode:%d"),iPhoneGlobals->iModemSupportsCMTIMode,iPhoneGlobals->iModemSupportsCMTMode);
		}

		//
		// Start the sending of the +CBST=? command
		iTxBuffer.Copy(KGetCBSTConfigCommand);
		iIo->Write(this, iTxBuffer);
		iIo->SetTimeOut(this, 5000);
		iState=EATWaitForSendingCBSTRequest;
		break;

	case EATWaitForSendingCBSTRequest:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATWaitForSendingCBSTComplete;
			}
		break;
	
	case EATWaitForSendingCBSTComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		{			// Curly brackets needed to scope ret variable
		//
		// When we get here we have just received a response from the modem in response
		// to the TSY sending AT+CBST=?.

		//
		// Check if the modem returned ERROR
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret != KErrNone)
			{
			LOGTEXT(_L8("CATInit: +CBST=? not supported"));
			}
		else
			{
			LOGTEXT(_L8("CATInit: +CBST=? supported"));
			//
			// ParseCBSTResponse will parse the response string and set the
			// appropriate CPhoneGlobals data. 
			TRAPD(ret,ParseCBSTResponseL());
			if (ret != KErrNone)
				{
				LOGTEXT(_L8("CATInit: Error parsing +CBST=? response"));
				}
			}
		}

		//
		// Start the sending of the +CSCA? command
		iTxBuffer.Copy(KGetServiceCentreAddressCommand);
		iIo->Write(this, iTxBuffer);
		iIo->SetTimeOut(this, 5000);
		iState=EATWaitForSendingCSCARequest;
		break;

	case EATWaitForSendingCSCARequest:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATWaitForSendingCSCAComplete;
			}
		break;
	
	case EATWaitForSendingCSCAComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		{			// Curly brackets needed to scope ret variable
		//
		// Check if the modem returned ERROR
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret != KErrNone)
			{
			LOGTEXT(_L8("CATInit: +CSCA? not supported"));
			iPhoneGlobals->iModemSupportsCSCAMode=EFalse;
			}
		else
			{
			LOGTEXT(_L8("CATInit: +CSCA? supported"));
			iPhoneGlobals->iModemSupportsCSCAMode=ETrue;
			}
		}
	
		//
		// Start the sending of the AT=CGREG=1 command
		// We do this to set the unsolicited GPRS network registration status change mode
		// of the phone.
		iTxBuffer.Copy(KSetUnsolicitedGPRSNetworkRegistration1);
		iIo->Write(this, iTxBuffer);
		iIo->SetTimeOut(this, 5000);
		iState=EATWaitForSetUnsolicitedGPRSNetworkRegistrationWrite;
		break;

	case EATWaitForSetUnsolicitedGPRSNetworkRegistrationWrite:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATWaitForSetUnsolicitedGPRSNetworkRegistrationOK;
			}
		break;

	case EATWaitForSetUnsolicitedGPRSNetworkRegistrationOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		{			// Curly brackets needed to scope ret variable
		//
		// Check if the modem returned ERROR
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret != KErrNone)
			{
			LOGTEXT(_L8("CATInit: AT+CGREG=1 not supported"));
			iPhoneGlobals->iModemSupportsCGREGNotification=EFalse;
			}
		else
			{
			LOGTEXT(_L8("CATInit: AT+CGREG=1 supported"));
			iPhoneGlobals->iModemSupportsCGREGNotification=ETrue;
			}
		}


		// If it's an Ericsson phone then enable their call monitoring because
		// they don't return NO CARRIER correctly for voice calls.
		if(iPhoneGlobals->iPhoneId.iManufacturer.MatchF(KEricsson)==0)
			{
			//
			// Start the sending of the AT*ECAM=1 command
			iTxBuffer.Copy(KSetECAMConfigCommand);
			iIo->Write(this, iTxBuffer);
			iIo->SetTimeOut(this, 5000);
			iState=EATWaitForSendingECAMRequest;
			}
			else
			{
				// It's not an Ericsson phone, so Init is complete
				Complete(KErrNone);
			}
		break;

	case EATWaitForSendingECAMRequest:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATWaitForSendingECAMComplete;
			}
		break;
	
	case EATWaitForSendingECAMComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		{			
		// Curly brackets needed to scope ret variable
		// Check if the modem returned ERROR
		iIo->WriteAndTimerCancel(this);
		TInt ret(ValidateExpectString());
		RemoveStdExpectStrings();
		if (ret != KErrNone)
			{
			LOGTEXT(_L8("CATInit: Ericsson Call Monitoring NOT supported"));
			}
		else
			{
			LOGTEXT(_L8("CATInit: Ericsson Call Monitoring supported"));
			}
		}		
		//
		// The initialisation of the TSY is complete
		Complete(KErrNone);
		break;
	default:					// Default case required to avoid warnings in ARM builds
		break;
		} // iState
	}			

void CATInit::ParseWaitTimeL()
	{
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	if (iRxResults.IsEmpty())
		iPhoneGlobals->iPhoneStatus.iWaitForCarrierTime = KDefaultSecondsToWaitForCarrier;
	else
		{
		CATParamListEntry* entry = iRxResults.First();
		TLex8 lex(entry->iResultPtr);
		(void)User::LeaveIfError(lex.Val(iPhoneGlobals->iPhoneStatus.iWaitForCarrierTime));
		entry->Deque();
		delete entry;
		entry=NULL;
		}
	CleanupStack::PopAndDestroy();
	}

void CATInit::ParsePhoneCapsL()
	{
	ParseBufferLC();
	TUint phoneCaps = 0;
	if (iRxResults.IsEmpty())
		phoneCaps |= RPhone::KCapsUnknown;
	else
		{
		CATParamListEntry* entry;
		TDblQueIter<CATParamListEntry> iter(iRxResults);
		while (entry = iter++,entry!=NULL)
			{
			TPtrC8 aResult(entry->iResultPtr);
			if (aResult==KGetPhoneCapsCommand)
				continue;	// echo is still enabled - ignore the original command!
			else if (aResult==KZeroString)
				phoneCaps |= RPhone::KCapsData;
			else if (aResult==KOneString)
				phoneCaps |= RPhone::KCapsFaxClassOne;
			else if (aResult==KOnePointZeroString)
				phoneCaps |= RPhone::KCapsFaxClassOnePointZero;
			else if (aResult==KTwoString)
				phoneCaps |= RPhone::KCapsFaxClassTwo;
			else if (aResult==KTwoPointZeroString)
				phoneCaps |= RPhone::KCapsFaxClassTwoPointZero;
			else if (aResult==KTwoPointOneString)
				phoneCaps |= RPhone::KCapsFaxClassTwoPointOne;
			entry->Deque();
			delete entry;
			}
		phoneCaps |= RPhone::KCapsEventModemDetection;
		phoneCaps |= RPhone::KCapsVoice;	// always true of a GSM phone!
		}
	LOGTEXT2(_L8("ParsePhoneCapsL\t iDataAndFaxFlags set to 0x%x"), phoneCaps);
	iPhoneGlobals->iPhoneStatus.iDataAndFaxFlags = phoneCaps;
	CleanupStack::PopAndDestroy();
	}

void CATInit::ParseAnErroredPhoneCaps()
//
// If the modem returns an ERROR to "AT+FCLASS=?" then assume Data and Voice only
//
	{
	TUint phoneCaps = 0;
	phoneCaps |= RPhone::KCapsData;
	phoneCaps |= RPhone::KCapsEventModemDetection;
	phoneCaps |= RPhone::KCapsVoice;
	LOGTEXT2(_L8("ParseAnErroredPhoneCapsL\t iDataAndFaxFlags set to 0x%x"), phoneCaps);
	iPhoneGlobals->iPhoneStatus.iDataAndFaxFlags = phoneCaps;
	}

void CATInit::ParsePhoneBookStorageL()
	{
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	TUint phoneBook=KPhoneBookNoStorage;
	TInt count=0;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	if (!iRxResults.IsEmpty())
		iter++;	// skip the +CPMS:

	CATParamListEntry* entry;
	while (entry = iter++,entry!=NULL)
		{
		count++;
		TPtrC8 result(entry->iResultPtr);
		if (result==KFDStorage)
			phoneBook|=KPhoneBookFDStorage;
		else if (result==KLDStorage)
			phoneBook|=KPhoneBookLDStorage;
		else if (result==KMEStorage)
			phoneBook|=KPhoneBookMEStorage;
		else if (result==KMTStorage)
			phoneBook|=KPhoneBookMTStorage;
		else if (result==KSMStorage)
			phoneBook|=KPhoneBookSMStorage;
		else if (result==KTAStorage)
			phoneBook|=KPhoneBookTAStorage;
		else if (result==KBMStorage)
			phoneBook|=KPhoneBookBarredStorage;
		else if (result==KDCStorage)
			phoneBook|=KPhoneBookLastDialledStorage;
		else if (result==KENStorage)
			phoneBook|=KPhoneBookEmergencyStorage;
		else if (result==KMCStorage)
			phoneBook|=KPhoneBookMissedCallsStorage;
		else if (result==KONStorage)
			phoneBook|=KPhoneBookOwnNumbersStorage;
		else if (result==KRCStorage)
			phoneBook|=KPhoneBookReceivedCallsStorage;
		else if (result==KSNStorage)
			phoneBook|=KPhoneBookServiceNumberStorage;
		entry->Deque();
		delete entry;
		}
	iPhoneGlobals->iPhoneStatus.iSupportedPhoneBookStorageFlag=phoneBook;
	iPhoneGlobals->iPhoneStatus.iSupportedPhoneBookCount=count;
	CleanupStack::PopAndDestroy();
	}

static TInt NetworkIdL(const TDesC8& aCode,RMobilePhone::TMobilePhoneNetworkCountryCode& aCountryCode, RMobilePhone::TMobilePhoneNetworkIdentity& aNetworkIdentity)
/**
 * This function converts the MCC and MNC as hex print of BCD coding. It is called from
 * the CATInit::ParseOperatorL() method. 
 */
	{
	if (aCode.Length()!=5)
		{
		return KErrGeneral;
		}
	
	aCountryCode.SetLength(3);
	aCountryCode[0] = aCode[0];
	aCountryCode[1] = aCode[1];
	aCountryCode[2] = aCode[2];

	aNetworkIdentity.SetLength(2);
	aNetworkIdentity[0] = aCode[3];
	aNetworkIdentity[1] = aCode[4];
	
	return KErrNone;
	}

void CATInit::ParseOperatorL()
/** This method parses the response to the operator query AT command.
 *  The response from the phone has one of the formats below:
 *		+COPS: mode, 0, "long format name"
 *		+COPS: mode, 1, "short format name"
 *		+COPS: mode, 2, "operator ID in hex"
 *		+COPS: mode
 */
	{
	RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfo = iPhoneGlobals->iPhoneStatus.iCurrentNetwork;

	// Set all the Network Info. variables to zero/unknown
	networkInfo.iCountryCode.FillZ(); // MCC 
	networkInfo.iNetworkId.FillZ();	  // MNC 
	networkInfo.iCdmaSID.FillZ();	  // Unused CDMA field
	networkInfo.iAnalogSID.FillZ();	  // Unused CDMA field
	networkInfo.iShortName.FillZ();
	networkInfo.iLongName.FillZ();
	networkInfo.iStatus=RMobilePhone::ENetworkStatusUnknown; 

	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	entry=iter++;				// +COPS:
	if (entry->iResultPtr.MatchF(KOperatorResponse)!=0)
		User::Leave(KErrGeneral);

	entry=iter++;				// <mode>
	if (entry==NULL)
		User::Leave(KErrGeneral);
	// not interested in the mode

	entry=iter++;				// <format>
	if (entry==NULL)
		User::Leave(KErrGeneral);
	TInt format=CATParamListEntry::EntryValL(entry);

	entry=iter++;				// <oper>
	if (entry==NULL)
		User::Leave(KErrGeneral);
	switch (format)
		{
	case 0:
  		if((entry->iResultPtr).Length() > networkInfo.iLongName.MaxLength())
  			networkInfo.iLongName.Copy((entry->iResultPtr).Mid(0,networkInfo.iLongName.MaxLength()));			
  		else
  			networkInfo.iLongName.Copy(entry->iResultPtr);
		break;
	case 1:
  		if((entry->iResultPtr).Length() > networkInfo.iShortName.MaxLength())
  			networkInfo.iShortName.Copy((entry->iResultPtr).Mid(0,networkInfo.iShortName.MaxLength()));			
  		else 
  			networkInfo.iShortName.Copy(entry->iResultPtr);	
		break;
	case 2:	
		User::LeaveIfError(NetworkIdL(entry->iResultPtr,networkInfo.iCountryCode, networkInfo.iNetworkId));
		break;		// hs	
	default:
		User::Leave(KErrGeneral);
		break;
		}

	// We've got an answer, so this must be the current network
	networkInfo.iStatus=RMobilePhone::ENetworkStatusCurrent;
	
	CleanupStack::PopAndDestroy();
	}

void CATInit::ParseCurrentMsClassL()
	{
	iPhoneGlobals->iMSClass = RPacketService::EMSClassUnknown;
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	entry=iter++;				// +CGCLASS:
	if(entry==NULL)
		User::Leave(KErrGeneral);
	if (entry->iResultPtr.MatchF(KCGCLASSResponseString)!=0)
		User::Leave(KErrGeneral);
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	// not interested in the mode
	TPtrC8 result(entry->iResultPtr);
	if(result == KClassA)
		iPhoneGlobals->iMSClass = RPacketService::EMSClassDualMode;
	else if(result == KClassB)
		iPhoneGlobals->iMSClass = RPacketService::EMSClassSuspensionRequired;
	else if(result == KClassC)
		iPhoneGlobals->iMSClass = RPacketService::EMSClassAlternateMode;
	else if(result == KClassCG)
		iPhoneGlobals->iMSClass = RPacketService::EMSClassPacketSwitchedOnly;
	else if(result == KClassCC)
		iPhoneGlobals->iMSClass = RPacketService::EMSClassCircuitSwitchedOnly;
	CleanupStack::PopAndDestroy();
	}


void CATInit::ParseMaxMsClassL()
	{
	iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassUnknown;
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	entry=iter++;				// +CGCLASS:
	if(entry==NULL)
		User::Leave(KErrGeneral);
	if (entry->iResultPtr.MatchF(KCGCLASSResponseString)!=0)
		User::Leave(KErrGeneral);
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	// not interested in the mode
	TPtrC8 result(entry->iResultPtr);
	if(result == KClassA)
		iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassDualMode;
	else if(result == KClassB)
		iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassSuspensionRequired;
	else if(result == KClassC)
		iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassAlternateMode;
	else if(result == KClassCG)
		iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassPacketSwitchedOnly;
	else if(result == KClassCC)
		iPhoneGlobals->iMaxMSClass = RPacketService::EMSClassCircuitSwitchedOnly;
	CleanupStack::PopAndDestroy();

	}


void CATInit::ParseCGQREQResponseL()
/**
 * This Function parses the response from the get CGQREQ command to the phone.
 * An example response is;
 *  +CGQREQ: (1-3),(0-3),(0-4),(0-5),(0-9),(0-18,31)
 *  1st parameter: list of valid context ID values
 *  2nd parameter: list of supported <precedence>
 *  3rd parameter: list of supported <delay>
 *  4th parameter: list of supported <reliabilitiy>
 *  5th parameter: list of supported <peak>
 *  6th parameter: list of supported <mean>
 */
	{
	iPhoneGlobals->iStaticQoSCaps.iPrecedence = 0;
	iPhoneGlobals->iStaticQoSCaps.iDelay = 0;
	iPhoneGlobals->iStaticQoSCaps.iReliability = 0;
	iPhoneGlobals->iStaticQoSCaps.iPeak = 0;
	iPhoneGlobals->iStaticQoSCaps.iMean = 0;
	iPhoneGlobals->iGprsMaxNumContexts = 0;
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	//
	// Check we have +CGQREQ: at the start of the response string
	entry=iter++;			
	if (entry==NULL || entry->iResultPtr.MatchF(KCGQREQResponseString)!=0)
		User::Leave(KErrGeneral);

	//
	// Parse 1st parmeeter, list of valid context IDs (cid)
	// We just want to extract the maximum cid
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	
	// Check to see if the phone supports more than 9 contexts
	TPtrC8 result(entry->iResultPtr.Right(2));
	TLex8 lex(result);
	TInt value(0);
	if(lex.Val(value)!= KErrNone || value<0 || value>20)
		{
		TPtrC8 result(entry->iResultPtr.Right(1)); 
		TLex8 lex2(result);
		(void)User::LeaveIfError(lex2.Val(value));
		}
	iPhoneGlobals->iGprsMaxNumContexts = value;

// Get max precedence levels
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	result.Set(entry->iResultPtr.Right(1));
	lex = result;
	value = 0;
	TInt valueToSet = 0;
	(void)User::LeaveIfError(lex.Val(value));
	switch(value)
		{
		case 1:
			valueToSet = RPacketQoS::EPriorityHighPrecedence;
			break;
		case 2:
			valueToSet = RPacketQoS::EPriorityMediumPrecedence;
			break;
		case 3:
			valueToSet = RPacketQoS::EPriorityLowPrecedence;
			break;
		default:
			valueToSet = RPacketQoS::EUnspecifiedPrecedence;
			break;
		}
	iPhoneGlobals->iStaticQoSCaps.iPrecedence = valueToSet;
// Get max delay class
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	result.Set(entry->iResultPtr.Right(1)); 
	lex = result;
	value = 0;
	(void)User::LeaveIfError(lex.Val(value));
	switch(value)
		{
		case 1:
			valueToSet = RPacketQoS::EDelayClass1;
			break;
		case 2:
			valueToSet = RPacketQoS::EDelayClass2;
			break;
		case 3:
			valueToSet = RPacketQoS::EDelayClass3;
			break;
		case 4:
			valueToSet = RPacketQoS::EDelayClass4;
			break;
		default:
			valueToSet = RPacketQoS::EUnspecifiedDelayClass;
			break;
	}
	iPhoneGlobals->iStaticQoSCaps.iDelay = valueToSet ;
// Get max reliability class
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	result.Set(entry->iResultPtr.Right(1)); 
	lex = result;
	value = 0;
	(void)User::LeaveIfError(lex.Val(value));
	switch(value)
		{
		case 1:
			valueToSet = RPacketQoS::EReliabilityClass1;
			break;
		case 2:
			valueToSet = RPacketQoS::EReliabilityClass2;
			break;
		case 3:
			valueToSet = RPacketQoS::EReliabilityClass3;
			break;
		case 4:
			valueToSet = RPacketQoS::EReliabilityClass4;
			break;	
		case 5:
			valueToSet = RPacketQoS::EReliabilityClass5;
			break;
		default:
			valueToSet = RPacketQoS::EUnspecifiedReliabilityClass;
			break;
		}
	iPhoneGlobals->iStaticQoSCaps.iReliability = valueToSet;
// Get max peak throughput class
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	result.Set(entry->iResultPtr.Right(1)); 
	lex = result;
	value = 0;
	(void)User::LeaveIfError(lex.Val(value));
	switch(value)
		{
		case 1:
			valueToSet = RPacketQoS::EPeakThroughput1000;
			break;
		case 2:
			valueToSet = RPacketQoS::EPeakThroughput2000;
			break;
		case 3:
			valueToSet = RPacketQoS::EPeakThroughput4000;
			break;
		case 4:
			valueToSet = RPacketQoS::EPeakThroughput8000;
			break;	
		case 5:
			valueToSet = RPacketQoS::EPeakThroughput16000;
			break;
		case 6:
			valueToSet = RPacketQoS::EPeakThroughput32000;
			break;
		case 7:
			valueToSet = RPacketQoS::EPeakThroughput64000;
			break;
		case 8:
			valueToSet = RPacketQoS::EPeakThroughput128000;
			break;
		case 9:
			valueToSet = RPacketQoS::EPeakThroughput256000;
			break;
		
		default:
			valueToSet = RPacketQoS::EUnspecifiedPeakThroughput;
			break;
		}
	iPhoneGlobals->iStaticQoSCaps.iPeak = valueToSet;
// Get max mean throughput class
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);
	result.Set(entry->iResultPtr.Right(2)); // Get the mean rate.
	if(result[0] == '-')
		result.Set(entry->iResultPtr.Right(1));
	entry=iter++;			
	if (entry!=NULL) // There could be another number like (0-18,31) where we want the 31.
		{
		TPtrC8 maxMean(entry->iResultPtr);
		TLex8 lexMaxMean(maxMean);
		if(lexMaxMean.Val(value) == KErrNone)
			{
			result.Set(entry->iResultPtr);
			}
		}
	lex = result;
	value = 0;
	(void)User::LeaveIfError(lex.Val(value));
	switch(value)
		{
		case 1:
			valueToSet = RPacketQoS::EMeanThroughput100;
			break;
		case 2:
			valueToSet = RPacketQoS::EMeanThroughput200;
			break;
		case 3:
			valueToSet = RPacketQoS::EMeanThroughput500;
			break;
		case 4:
			valueToSet = RPacketQoS::EMeanThroughput1000;
			break;
		case 5:
			valueToSet = RPacketQoS::EMeanThroughput2000;
			break;	
		case 6:
			valueToSet = RPacketQoS::EMeanThroughput5000;
			break;
		case 7:
			valueToSet = RPacketQoS::EMeanThroughput10000;
			break;
		case 8:
			valueToSet = RPacketQoS::EMeanThroughput20000;
			break;
		case 9:
			valueToSet = RPacketQoS::EMeanThroughput50000;
			break;
		case 10:
			valueToSet = RPacketQoS::EMeanThroughput100000;
			break;
		case 11:
			valueToSet = RPacketQoS::EMeanThroughput200000;
			break;
		case 12:
			valueToSet = RPacketQoS::EMeanThroughput500000;
			break;
		case 13:
			valueToSet = RPacketQoS::EMeanThroughput1000000;
			break;
		case 14:
			valueToSet = RPacketQoS::EMeanThroughput2000000;
			break;
		case 15:
			valueToSet = RPacketQoS::EMeanThroughput5000000;
			break;
		case 16:
			valueToSet = RPacketQoS::EMeanThroughput10000000;
			break;
		case 17:
			valueToSet = RPacketQoS::EMeanThroughput20000000;
			break;
		case 18:
			valueToSet = RPacketQoS::EMeanThroughput50000000;
			break;
		default:			// This intentionally catches case 0 and case 31
			valueToSet = RPacketQoS::EUnspecifiedMeanThroughput;
			break;
		}
	iPhoneGlobals->iStaticQoSCaps.iMean = valueToSet;
	CleanupStack::PopAndDestroy();
	}




void CATInit::ParseCNMIResponseL()
/**
 * This function parses the response for +CNMI=? from the modem and attempts
 * to set the iModemSupportsCMTIMode and iModemSupportsCMTMode of CPhoneGlobals.
 * An example response is;
 *  +CNMI: (0-2),(0-3),(0,2,3),(0-2),(0,1) 
 * The ETSI specs names these +CNMI: <mode>,<mt>,<bm>,<ds>,<bfr>        
 */
	{
	_LIT8(KHyphenCharacter,"-");
	_LIT8(KOpenBracketCharacter,"(");
	_LIT8(KCloseBracketCharacter,")");

	ParseBufferLC(ETrue);		   // ETrue so that we are given the bracket list seperators
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;

	//
	// Validate we have received the starting '+CNMI:' string
	if((entry==NULL)||(entry->iResultPtr!=KCNMIResponseString))
		{
		LOCAL_LOGTEXT("ParseCNMIResponse","Cannot find +CNMI: string");
		User::Leave(KErrNotFound);
		}

	//
	// Skip over the <mode> parameter

	// Find the opening bracket
	while(entry && entry->iResultPtr.Compare(KOpenBracketCharacter))
		entry=iter++;
	if(!entry)
		{
		LOCAL_LOGTEXT("ParseCNMIResponse","Failed parsing <mode> parameter");
		User::Leave(KErrNotFound);
		}
	
	// Find the closing bracket
	while(entry && entry->iResultPtr.Compare(KCloseBracketCharacter))
		entry=iter++;
	if(!entry)
		{
		LOCAL_LOGTEXT("ParseCNMIResponse","Failed parsing <mode> parameter");
		User::Leave(KErrNotFound);
		}


	//
	// Parse the <mt> parameter

	// Find the opening bracket
	while(entry && entry->iResultPtr.Compare(KOpenBracketCharacter))
		entry=iter++;
	if(!entry)
		{
		LOCAL_LOGTEXT("ParseCNMIResponse","Failed parsing <mode> parameter");
		User::Leave(KErrNotFound);
		}

	// Process the parameter values upto the closing bracket
	entry=iter++;
	while(entry && entry->iResultPtr.Compare(KCloseBracketCharacter))
		{
		//
		// Check if parameter is a range (for example '0-3')
		if(entry->iResultPtr.Find(KHyphenCharacter)>KErrNone)
			{
			//
			// Parameter contains a '-' character so it is a range of values

			// Get lower & higer values
			TUint8 lowVal;
			TUint8 highVal;
			TLex8 lexRange;
			const TInt rangeCharPos=entry->iResultPtr.Find(KHyphenCharacter);
			lexRange=entry->iResultPtr.Left(rangeCharPos);
			(void)User::LeaveIfError(lexRange.Val(lowVal,EDecimal));
			lexRange=entry->iResultPtr.Mid(rangeCharPos+1);
			(void)User::LeaveIfError(lexRange.Val(highVal,EDecimal));

			if(lowVal<=1 && highVal>=1) // 1 & 2 are defined in the ESTI spec for the phone to support CMTI mode
				iPhoneGlobals->iModemSupportsCMTIMode=ETrue;
			if(lowVal<=2 && highVal>=2) // 1 & 2 are defined in the ESTI spec for the phone to support CMTI mode
				iPhoneGlobals->iModemSupportsCMTIMode=ETrue;
			if(lowVal<=3 && highVal>=3) // 3 is defined in the ESTI spec for the phone to support CMT mode
				iPhoneGlobals->iModemSupportsCMTMode=ETrue;
			}
		else
			{
			//
			// String does not contain '-' character so it must be just a single value
			const TInt val(CATParamListEntry::EntryValL(entry));
			if(val==1)		// 1 & 2 are defined in the ESTI spec for the phone to support CMTI mode
				iPhoneGlobals->iModemSupportsCMTIMode=ETrue;
			if(val==2)		// 1 & 2 are defined in the ESTI spec for the phone to support CMTI mode
				iPhoneGlobals->iModemSupportsCMTIMode=ETrue;
			if(val==3)		// 3 is defined in the ESTI spec for the phone to support CMT mode
				iPhoneGlobals->iModemSupportsCMTMode=ETrue;
			}
		entry=iter++;
		}

	if(!entry)
		{
		LOCAL_LOGTEXT("ParseCNMIResponse","Failed parsing <mode> parameter");
		User::Leave(KErrNotFound);
		}

	//
	// Skip all other parameters as we are not, currently, interested in them
	// We've fisnihed the parse.
	CleanupStack::PopAndDestroy();		// ParseBufferLC object
	}
	

void CATInit::ParseCGATTResponseL()
	{
	iPhoneGlobals->iGprsStatus = RPacketService::EStatusUnattached;
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	entry=iter++;				// +CGATT:
	if(entry==NULL)
		User::Leave(KErrGeneral);
	if (entry->iResultPtr.MatchF(KCGATTResponseString)!=0)
		User::Leave(KErrGeneral);
	entry=iter++;				
	if (entry==NULL)
		User::Leave(KErrGeneral);

	TUint result=CATParamListEntry::EntryValL(entry);
	if(result == KZeroChar)
		iPhoneGlobals->iGprsStatus = RPacketService::EStatusUnattached;
	else if(result == KOneChar)
		iPhoneGlobals->iGprsStatus = RPacketService::EStatusAttached;
	CleanupStack::PopAndDestroy();

	}

void CATInit::Complete(TInt aError)
	{
	iIo->Cancel();
	iNoCarrierExpectString=NULL;

	// The initialization may complete with a status of KErrCancel
	// even when it has not been explicitly cancelled
	// If the KErrCancel status propogates to etel, etel will panic because of the stray
	// cancel status (etel treats KErrCancel specially in that all KErrCancel completions
	// must be explicitly cancelled by the etel client)
	if (aError == KErrCancel)
		{
		// change status to avoid etel panic
		aError = KErrEtelInitialisationFailure;
		}
	iInitError = aError;

	if (aError)
		{
		LOGTEXT(_L8("CATInit::Complete - EPhoneNotInitialised"));
		iPhoneGlobals->iPhoneStatus.iModemDetected = RPhone::EDetectedNotPresent;
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject,EPhoneNotDetected);
		iPhoneGlobals->iNotificationStore->CompleteNotificationsWithError(aError);

		TPhoneInitStatus currentInitStatus = iPhoneGlobals->iPhoneStatus.iInitStatus;
		SetToNotInitialised();
		if (iPendingCommand)
			{
			if (iCallInfo)
				{
				REINTERPRET_CAST(CCallHayes*,iPendingCommand->Owner())->SetToIdle();
				}
			if (currentInitStatus != EPhoneInitialiseCancelling)
				// if cancelling, ReqCompleted() will have been called already
				{
				iIo->RemoveExpectStrings(this); 
				iPendingCommand->Owner()->ReqCompleted(iReqHandle, aError);		
				iPendingCommand=NULL;
				}
			}
		else
			{
			if (iReqHandle==NULL)
				{	// if this is a CommPort error, CompleteNotificationsWithError will be
					// called in CATError too, with no effect. It is necessary here as the
					// error may be a timeout or an unexpected response.
				iInitJustCompleted = ETrue;
				iTelObject->FlowControlResume();	// Automatic or Notify-started
													// Init finished
				iInitJustCompleted = EFalse;
				}
			else
													// Controlled Init finished
				{
				STATIC_CAST(CPhoneHayes*,iTelObject)->ReqCompleted(iReqHandle,aError);
				}
			}
		}
	else
		{
		LOGTEXT(_L8("CATInit::Complete - EPhoneInitialised"));
		iIo->RemoveExpectStrings(this); 
		iOKExpectString=NULL;
		iErrorExpectString=NULL;
		iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneInitialised;
		if (iPendingCommand)
			{
			iPendingCommand->Start(iReqHandle,iParams);
			}
		else
			{
			if (iReqHandle==NULL)
				{
				iInitJustCompleted = ETrue;
				iTelObject->FlowControlResume();	// Automatic Init finished
				iInitJustCompleted = EFalse;
				}
			else
													// Controlled Init finished
				STATIC_CAST(CPhoneHayes*,iTelObject)->ReqCompleted(iReqHandle,KErrNone);
			}
		}
	}

TBool CATInit::JustInitialised(TInt& aError) const
	{
	aError = iInitError;
	return iInitJustCompleted;
	}

//

//
// Quick Initialise Class
// Called when TSY recovers Comm Port
// Puts modem in a known state. With no errors, ends in On Line Data state
// Otherwise sets iInitStatus to uninitialised, so full init will be performed with next command.
// If receive a RING or a NO CARRIER, this is conclusive proof that modem is in off-line mode
// so the quick initialisation should stop immediately
//

CATQuickInit* CATQuickInit::NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	{
	return new(ELeave) CATQuickInit(aIo,aTelObject,aPhoneGlobals);
	}

CATQuickInit::CATQuickInit(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals) :
							CATBase(aIo,aTelObject,aPhoneGlobals)
	{}

CATQuickInit::~CATQuickInit()
	{
	iIo->RemoveExpectStrings(this);
	iIo->WriteAndTimerCancel(this);
	}

void CATQuickInit::StartQuickInit()
//
//	Ensure port configuration is the same as before the data port was loaned
//
	{
	LOGTEXT(_L8("Starting Quick Initialisation Sequence"));
	
	// Change CPhoneGlobals::iPhoneStatus::iInitStatus so that we prevent
	// +CRINGs being processed while this CATQuickInit machine is running.
	iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneInitialising;

	iIo->Cancel();
	TCommConfig aConfigPckg;
	TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeQuickInit);
	if (ret==KErrNone)
		ret = iIo->ConfigurePort(aConfigPckg);
	if (ret)
		Complete(ret);
	else
		{
		LOGTEXT2(_L8("Comm signals after PortConfig : %x"),iIo->Signals());

		// This expect string will always be active
		__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));

		iRingExpectString = iIo->AddExpectString(this,KRingString);
		iNoCarrierExpectString = iIo->AddExpectString(this,KNoCarrierString);

		TPtrC8 returnATCommand(KReturnATCommand);
		LOGTEXT2(_L8("No. bytes in of input buffer : %d"),iIo->GetSizeOfRxBuffer());
		Write(returnATCommand,1);
		iAttempt=1;
		iState=EWaitForATWrite;
		}
	}
#ifdef _DEBUG
void CATQuickInit::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
#else
void CATQuickInit::CompleteWithIOError(TEventSource /*aSource*/,TInt /*aStatus*/)
#endif
	{
	LOGTEXT2(_L8("CATQuickInit::GenericEventSignal received error status %d"),aStatus);

	iIo->Cancel();
	if (iState==EWaitForDTRDropped)
		{
		// Make sure DTR is raised again
		iIo->RaiseDTR();
		}

	iState = ENotInProgress;

	// Set TSY so it will reinitialise
	STATIC_CAST(CCallHayes*,iTelObject)->SetToIdle();
	SetToNotInitialised();		
	iIo->Read();
	iTelObject->FlowControlResume();
	}

void CATQuickInit::ValidateATHExpectStringL()
	{
	if(iIo->FoundChatString()==iErrorExpectString)
		{
		LOGTEXT(_L8("Modem returned ERROR in response to command"));
		User::Leave(KErrGeneral);
		}
	if(iIo->FoundChatString()==iNoCarrierExpectString)
		{
		LOGTEXT(_L8("Modem returned NO CARRIER in response to command"));
		User::Leave(KErrEtelNoCarrier);
		}
	if(iIo->FoundChatString()==iOKExpectString)
		return;
	LOGTEXT(_L8("Modem returned unexpected response to command"));
	User::Leave(KErrUnknown);
	}

void CATQuickInit::CancelAndHangUp(TCallInfoTSY* aCallInfoTSY, CATHangUpData* aHangUpData)
//
//	If doing the quick initialisation sequence, cancel it and call hang up, otherwise just
//	hang up.
//
	{
	if (iState!=ENotInProgress)
		{
		LOGTEXT(_L8("Cancelling Quick Init"));
		iCallInfo = aCallInfoTSY;
		iHangUpData = aHangUpData;
		iState=ECancelling;
		}
	else
		{
		LOGTEXT(_L8("Calling Hang Up"));
		aHangUpData->ExecuteCommand(0,NULL,aCallInfoTSY);
		}
	}

void CATQuickInit::EventSignal(TEventSource aSource)
	{
	LOGTEXT2(_L8("CATQuickInit::EventSignal iState=%d"),iState);

	if((aSource==ETimeOutCompletion)
		&&(iState!=EWaitForATSilence)
		&&(iState!=EWaitForATEscapeOK)
		&&(iState!=EWaitForATOK)
		&&(iState!=EWaitForDTRDropped)
		&&(iState!=EWaitForDTRRaised)
		&&(iState!=ECancelling))
		{
		if (iState==EWaitForATWrite)	// the first write hasn't completed
			{
			iIo->Cancel();
			if (iAttempt <= 3)
				{
				TCommConfig aConfigPckg;
				TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeDDBugWorkAroundStart);
				if (ret==KErrNone)
					ret = iIo->ConfigurePort(aConfigPckg);
				if (ret)
					{
					Complete(ret);
					return;
					}
				ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeDDBugWorkAroundEnd);
				if (ret==KErrNone)
					ret = iIo->ConfigurePort(aConfigPckg);
				if (ret)
					Complete(ret);
				else
					{
					TPtrC8 returnATCommand(KReturnATCommand);
					Write(returnATCommand,1);
					iAttempt++;
					}
				return;
				}
			}
		LOGTEXT(_L8("Error during Quick Initialisation Sequence"));
		RemoveQuickInitExpectStrings();
		Complete(KErrEtelInitialisationFailure);
		return;
		}

	if (aSource==EReadCompletion)
		{
		CCommChatString* foundString = iIo->FoundChatString();
		if (foundString==iRingExpectString || foundString==iNoCarrierExpectString)
			{
			LOGTEXT(_L8("CATQuickInit::EventSignal Phone mode is off-line"));
			RemoveQuickInitExpectStrings();
			STATIC_CAST(CCallHayes*,iTelObject)->SetToIdle();
			iIo->Cancel();
			iIo->DropDtr();
			iIo->SetTimeOut(this,KOneSecondPause / 2);
			iState = EWaitForDTRDropped;
			return;
			}
		}	

	switch(iState)
		{
	case ENotInProgress:
		break;
	case EWaitForATSilence:
		{
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		if (!(iIo->ReadPending()))
			iIo->Read();
		TBuf8<KCommsDbSvrMaxFieldLength> escapeChar;
		TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameEscapeCharacter),escapeChar);
		if (ret)
			{
			Complete(ret);
			}
		iTxBuffer.Format(_L8("%S%S%S"),&escapeChar,&escapeChar,&escapeChar);
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this,KOneSecondPause);
		iState=EWaitForEscapeWriteCompletion;
		break;
		}

	case EWaitForEscapeWriteCompletion:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		if (!iOKExpectString)
			iOKExpectString = iIo->AddExpectString(this,KOkString);
		iIo->SetTimeOut(this,5*KOneSecondPause);
		iState=EWaitForATEscapeOK;
		break;

	case EWaitForATEscapeOK:
		// Can legitimately be TimeOut or ReadCompletion
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		{
		if (aSource==EReadCompletion)
			{
			LOGTEXT(_L8("Should now be in On-line Command mode"));
			}
		iIo->RemoveExpectString(iOKExpectString);
		iOKExpectString = NULL;
		iIo->WriteAndTimerCancel(this);
		TPtrC8 ATCommand(KAT2Command);
		Write(ATCommand,1);
		iState=EWaitForATWrite;
		}
		break;

	case EWaitForATWrite:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		if (!iOKExpectString)
			iOKExpectString = iIo->AddExpectString(this,KOkString);
		LOGTEXT2(_L8("Comm signals after first write : %x"),iIo->Signals());
		iIo->SetTimeOut(this,2*KOneSecondPause);
		if (!(iIo->ReadPending()))
			iIo->Read();
		iState=EWaitForATOK;
		break;

	case EWaitForATOK:
		// Can legitimately be TimeOut or ReadCompletion
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		LOGTEXT2(_L8("Comm signals after first read/timeout : %x"),iIo->Signals());
		if (aSource==EReadCompletion)
			{
			TRAPD(ret,ValidateATHExpectStringL());
			if (ret==KErrEtelNoCarrier)
				{
				STATIC_CAST(CCallHayes*,iTelObject)->SetToIdle();
				Complete(KErrNone);
				return;
				}
			}
		iIo->RemoveExpectString(iOKExpectString);
		iOKExpectString = NULL;
		iIo->WriteAndTimerCancel(this);
		if (aSource==ETimeOutCompletion)
			{
			if (iAttempt==1)
				{
				iAttempt++;
				iState = EWaitForATSilence;
				EventSignal(aSource);			// Attempt escape sequence
				}
			else
				{
				LOGTEXT(_L8("Should now be either not connected or modem is not responding for some reason"));
				Complete(KErrEtelInitialisationFailure);
				}
			}
		else
			{
			TBuf8<KCommsDbSvrMaxFieldLength> toOnLineDataCommand;
			TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameOnLine),toOnLineDataCommand);
			if (ret)
				{
				Complete(ret);
				}
			iTxBuffer.Format(KATAndStringFormatString,&toOnLineDataCommand);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KOneSecondPause);
			iState=EATOWaitForWriteComplete;
			}
		break;

	case EATOWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		if (!iConnectExpectString)
			{
			TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameConnect),iConnectString);
			if (ret)
				{
				Complete(ret);
				}
			AppendWildCardChar(iConnectString);
			iConnectExpectString=iIo->AddExpectString(this,iConnectString);
			}
		if (!iErrorExpectString)
			iErrorExpectString=iIo->AddExpectString(this,KErrorString);
		iIo->SetTimeOut(this,2*KOneSecondPause);
		iState=EWaitForATOOK;
		}
		break;


	case EWaitForATOOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			if(iIo->FoundChatString()==iErrorExpectString)
				{
				LOGTEXT(_L8("Should now be in Off-line mode"));
				STATIC_CAST(CCallHayes*,iTelObject)->SetToIdle();
				}
			if(iIo->FoundChatString()==iConnectExpectString)
				{
				LOGTEXT(_L8("Should now be in On-line data mode"));
				iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeOnlineData;
				}
			Complete(KErrNone);
			}
		break;

	case EWaitForDTRDropped:	// necessary to lower and raise DTR if the link has been lost,
								// otherwise modem may be in a strange "non-answering" mood.
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->Cancel();
		iIo->RaiseDTR();
		iIo->SetTimeOut(this,KOneSecondPause);
		iState = EWaitForDTRRaised;
		break;

	case EWaitForDTRRaised:
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		Complete(KErrNone);
		break;

	case ECancelling:
		if (aSource==EWriteCompletion)
			{
			if (!iOKExpectString)
				iOKExpectString = iIo->AddExpectString(this,KOkString);
			if (!iConnectExpectString)
				{
				TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameConnect),iConnectString);
				if (ret)
					{
					Complete(ret);
					}
				AppendWildCardChar(iConnectString);
				iConnectExpectString=iIo->AddExpectString(this,iConnectString);
				}
			if (!iErrorExpectString)
				iErrorExpectString=iIo->AddExpectString(this,KErrorString);
			iIo->SetTimeOut(this,KOneSecondPause);
			}
		else
			{
			RemoveQuickInitExpectStrings();
			iState=ENotInProgress;
			iTelObject->FlowControlResume();		// Defect fix for MPO-4ZECUN
			iHangUpData->ExecuteCommand(0,NULL,iCallInfo);
			iHangUpData=NULL;
			iCallInfo=NULL;
			break;
			}
		}
	}

void CATQuickInit::RemoveQuickInitExpectStrings()
	{
	if (iOKExpectString)
		{
		iIo->RemoveExpectString(iOKExpectString);
		iOKExpectString=NULL;
		}
	if (iErrorExpectString)
		{
		iIo->RemoveExpectString(iErrorExpectString);
		iErrorExpectString=NULL;
		}
	if (iConnectExpectString)
		{
		iIo->RemoveExpectString(iConnectExpectString);
		iConnectExpectString=NULL;
		}
	if (iNoCarrierExpectString)
		{
		iIo->RemoveExpectString(iNoCarrierExpectString);
		iNoCarrierExpectString=NULL;
		}
	if (iRingExpectString)
		{
		iIo->RemoveExpectString(iRingExpectString);
		iRingExpectString=NULL;
		}
	}

void CATQuickInit::Complete(TInt aError)
	{
	LOGTEXT2(_L8("CATQuickInit::Completed with error code %d"),aError);
	LOGTEXT2(_L8("Comm signals : %x"),iIo->Signals());
	iState = ENotInProgress;
	RemoveQuickInitExpectStrings();
	iIo->RemoveExpectStrings(this); // Make sure the error string is removed too
	iIo->Cancel();
	if (aError)
		{
		STATIC_CAST(CCallHayes*,iTelObject)->SetToIdle();
		SetToNotInitialised();		
		}
	else
		{	
		TCommConfig aConfigPckg;
		if (iPhoneGlobals->iPhoneStatus.iMode == RPhone::EModeOnlineData)
			{
			TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeConnect);
			if (ret==KErrNone)
				ret = iIo->ConfigurePort(aConfigPckg);
			if (ret)
				{
				Complete(ret);
				return;
				}
			REINTERPRET_CAST(CCallHayes*,iTelObject)->iWaitForNoCarrier->StartWait();
			}
		iPhoneGlobals->iPhoneStatus.iInitStatus = EPhoneInitialised;
		}
	iIo->Read();
	iTelObject->FlowControlResume();
	}

TInt CATInit::GetIdentityResponse()
/*
 * This is the non leaving version of GetIdentityResponseL.
 * Having this non-leaving version means that one TRAP harness can replace 
 * what were 4.
 */
 	{
	TRAPD(ret,DoGetIdentityResponseL());
	return ret;
	}

void CATInit::DoGetIdentityResponseL()
	{
	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	iMoreInfoFlag = EFalse;
	LOGTEXT(_L8("MMTsy:\tCATBasicGsmPhoneId:\tGrabbing +CGxx results from ME"));
	while (entry = iter++, entry != NULL)
		{
		TPtrC8 result = entry->iResultPtr;
		if (iCGMIFlag)
			{
			if (CheckResponse(result))
				LOGTEXT(_L8("MMTsy:\tCATBasicPhoneId:\tModem responded with +CGMI at the front!"));
			else
				{
				if  (iMoreInfoFlag)
					{
					TBuf<RMobilePhone::KPhoneManufacturerIdSize> aData;
					aData.Zero();
					TInt remainingBufferSize = RMobilePhone::KPhoneManufacturerIdSize - iPhoneGlobals->iPhoneId.iManufacturer.Length() - 1;
					//check not going to over flow buffer (including room for space character)
					if(remainingBufferSize > 0)
						{
						if(result.Length() > remainingBufferSize)
							{
							aData.Copy(result.Mid(0,remainingBufferSize));
							}
						else
							{
							aData.Copy(result);
							}
						iPhoneGlobals->iPhoneId.iManufacturer.Append(KSpaceSeparator);
						iPhoneGlobals->iPhoneId.iManufacturer.Append(aData);
						}
					}
				else
					{
					TBuf<RMobilePhone::KPhoneManufacturerIdSize> aData;
					aData.Zero();
					if(result.Length() > RMobilePhone::KPhoneManufacturerIdSize)
						{
						aData.Copy(result.Mid(0,RMobilePhone::KPhoneManufacturerIdSize));
						}
					else
						{
						aData.Copy(result);
						}
					iPhoneGlobals->iPhoneId.iManufacturer = aData;
					iMoreInfoFlag = ETrue;
					}
				}
			}
		else if (iCGMMFlag)
			{
			if (CheckResponse(result))
				LOGTEXT(_L8("MMTsy:\tCATBasicPhoneId:\tModem responded with +CGMI at the front!"));
			else
				{
				if  (iMoreInfoFlag)
					{
					TBuf<RMobilePhone::KPhoneModelIdSize> aData;
					aData.Zero();
					TInt remainingBufferSize =  RMobilePhone::KPhoneModelIdSize - iPhoneGlobals->iPhoneId.iModel.Length() - 1;
					//check not going to over flow buffer (including room for space character)
					if(remainingBufferSize > 0)
						{
						if(result.Length() > remainingBufferSize)
							{
							aData.Copy(result.Mid(0,remainingBufferSize));
							}
						else
							{
							aData.Copy(result);
							}
						iPhoneGlobals->iPhoneId.iModel.Append(KSpaceSeparator);
						iPhoneGlobals->iPhoneId.iModel.Append(aData);
	   					}
					}
				else
					{
					TBuf<RMobilePhone::KPhoneModelIdSize> aData;
					aData.Zero();
					if(result.Length() > RMobilePhone::KPhoneModelIdSize)
						{
						aData.Copy(result.Mid(0,RMobilePhone::KPhoneModelIdSize));
						}
					else
						{
						aData.Copy(result);
						}
					iPhoneGlobals->iPhoneId.iModel = aData;
					iMoreInfoFlag = ETrue;
					}
				}
			}
		else if (iCGMRFlag)
			{
			if (CheckResponse(result))
				LOGTEXT(_L8("MMTsy:\tCATBasicPhoneId:\tModem responded with +CGMI at the front!"));
			else
				{
				if  (iMoreInfoFlag)
					{
					TBuf<RMobilePhone::KPhoneRevisionIdSize> aData;
					aData.Zero();
					TInt remainingBufferSize =  RMobilePhone::KPhoneRevisionIdSize - iPhoneGlobals->iPhoneId.iRevision.Length() - 1;
					//check not going to over flow buffer (including room for space character)
					if(remainingBufferSize > 0)
						{
						if(result.Length() > remainingBufferSize)
							{
							aData.Copy(result.Mid(0,remainingBufferSize));
							}
						else
							{
							aData.Copy(result);
							}
						iPhoneGlobals->iPhoneId.iRevision.Append(KSpaceSeparator);
						iPhoneGlobals->iPhoneId.iRevision.Append(aData);
						}
					}
				else
					{
					TBuf<RMobilePhone::KPhoneRevisionIdSize> aData;
					aData.Zero();
					if(result.Length() > RMobilePhone::KPhoneRevisionIdSize)
						{
						aData.Copy(result.Mid(0,RMobilePhone::KPhoneRevisionIdSize));
						}
					else
						{
						aData.Copy(result);
						}
					iPhoneGlobals->iPhoneId.iRevision = aData;
					iMoreInfoFlag = ETrue;
					}
				}
			}
		else if (iCGSNFlag)
			{
			if (CheckResponse(result))
				LOGTEXT(_L8("MMTsy:\tCATBasicPhoneId:\tModem responded with +CGMI at the front!"));
			else
				{
				if  (iMoreInfoFlag)
					{
					TBuf<RMobilePhone::KPhoneSerialNumberSize> aData;
					aData.Zero();
					TInt remainingBufferSize = RMobilePhone::KPhoneSerialNumberSize - iPhoneGlobals->iPhoneId.iSerialNumber.Length() - 1;
					//check not going to over flow buffer (including room for space character)
					if(remainingBufferSize > 0)
						{
						if(result.Length() > remainingBufferSize)
							{
							aData.Copy(result.Mid(0,remainingBufferSize));
							}
						else
							{
							aData.Copy(result);
							}
						iPhoneGlobals->iPhoneId.iSerialNumber.Append(KSpaceSeparator);
						iPhoneGlobals->iPhoneId.iSerialNumber.Append(aData);
						}
					}
				else
					{
					TBuf<RMobilePhone::KPhoneSerialNumberSize> aData;
					aData.Zero();
					if(result.Length() > RMobilePhone::KPhoneSerialNumberSize)
						{
						aData.Copy(result.Mid(0,RMobilePhone::KPhoneSerialNumberSize));
						}
					else
						{
						aData.Copy(result);
						}
					iPhoneGlobals->iPhoneId.iSerialNumber = aData;
					iMoreInfoFlag = ETrue;
					}
				}
			}
		entry->Deque();
		delete entry;
		}//while
	iMoreInfoFlag = EFalse;
	entry = NULL;
	CleanupStack::PopAndDestroy();
	}

TBool CATInit::CheckResponse(TPtrC8 aResult)
	{
	if ((aResult == KCGMIResponseString) || (aResult == KCGMMResponseString)
	 || (aResult == KCGMRResponseString) || (aResult == KCGSNResponseString))
	 return ETrue;
	else
		return EFalse;
	}


void CATInit::CheckCMGFResponseL()
//
// Parse the response to the "AT+CMGF=?" command.
// Response should be of the form "+CMGF: xxx" where xxx should be an integer.
//
	{
	ParseBufferLC();						 // Grab the +CMGF:.... response 
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

// The "+CMGF:" string should come first
	entry=iter++;
	if((!entry)||(entry->iResultPtr!=KCMGFResponseString))
		User::Leave(KErrNotFound);
	LOGTEXT(_L8("Found +CMGF String!"));

// Now parse the list of capabilities supported and see if we can find PDU mode
	iPhoneGlobals->iSmsPduModeSupported=EFalse;
	while(entry)
		{
		entry=iter++;
		if(!entry)
			continue;

		TInt value;
		TLex8 yyLex(entry->iResultPtr);
		(void)User::LeaveIfError(yyLex.Val(value));

		if (value==0)
			{
			LOGTEXT(_L8("MMTsy:\tCATInit:\tPdu mode is supported."));
			iPhoneGlobals->iSmsPduModeSupported=ETrue;
			break;
			}
		}

	CleanupStack::PopAndDestroy();
	}



void CATInit::ParseCBSTResponseL()
/**
 * This function parses the +CBST: response from the modem and stores the parsed values 
 * in the clients iCallCaps structure.
 *
 * An example response string would be;
 *   +CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(0-3)
 */	
	{
	const TChar KComma=TChar(',');
	const TChar KOpenBracket=TChar('(');
	const TChar KCloseBracket=TChar(')');
	_LIT8(KRangeChar,"-");

	LOGTEXT(_L8("CATInit::ParseResponseL "));

	//
	// Initialise caps 
	iPhoneGlobals->iCallDataCaps.iSpeedCaps=0;
	iPhoneGlobals->iCallDataCaps.iProtocolCaps=0;
	iPhoneGlobals->iCallDataCaps.iServiceCaps=0;
	iPhoneGlobals->iCallDataCaps.iQoSCaps=0;
	iPhoneGlobals->iCallDataCaps.iHscsdSupport=EFalse;
	iPhoneGlobals->iCallDataCaps.iMClass=0;
	iPhoneGlobals->iCallDataCaps.iMaxRxTimeSlots=0;
	iPhoneGlobals->iCallDataCaps.iMaxTxTimeSlots=0;
	iPhoneGlobals->iCallDataCaps.iTotalRxTxTimeSlots=0;
	iPhoneGlobals->iCallDataCaps.iCodingCaps=0;
	iPhoneGlobals->iCallDataCaps.iAsymmetryCaps=0;
	iPhoneGlobals->iCallDataCaps.iUserInitUpgrade=EFalse;
	iPhoneGlobals->iCallDataCaps.iRLPVersionCaps=0;
	iPhoneGlobals->iCallDataCaps.iV42bisCaps=0;


	//
	// Count the number of <speed> list items (see ETSI 07.07)
	//
	TLex8 lex(iIo->Buffer());
	TChar c;
	TInt speedItems(0);

	// Locate opening bracket
	while(lex.Get()!=KOpenBracket && !lex.Eos()) {/* Do nothing*/};

	if(lex.Eos())
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate <speed> opening bracket"));
		User::Leave(KErrGeneral);
		}

	// Count commas ',' upto the closing bracket
	while((c=lex.Get())!=KCloseBracket && !lex.Eos())
		{
		if(c==KComma)
			speedItems++;
		}
	if(lex.Eos())
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate <speed> closing bracket"));
		User::Leave(KErrGeneral);
		}

	// Increment speedItems by 1 to make it denote the number of <speed> parameters
	++speedItems;
	LOGTEXT2(_L8("CATInit::ParseResponseL speedItems:%d"),speedItems);


	//
	// Count the number of <name> list items (see ETSI 07.07)
	//
	TInt nameItems(0);

	// Locate opening bracket
	while(lex.Get()!=KOpenBracket && !lex.Eos()) {/*Do nothing*/};
	if(lex.Eos())
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate <name> opening bracket"));
		User::Leave(KErrGeneral);
		}

	// Count commas ',' upto the closing bracket
	while((c=lex.Get())!=KCloseBracket && !lex.Eos())
		{
		if(c==KComma)
			nameItems++;
		}
	if(lex.Eos())
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate <name> closing bracket"));
		User::Leave(KErrGeneral);
		}

	// Increment nameItems by 1 to make it denote the number of <speed> parameters
	++nameItems;
	LOGTEXT2(_L8("CATInit::ParseResponseL nameItems:%d"),nameItems);

	//
	//
	// Count the number of <ce> list items (see ETSI 07.07)
	TInt ceItems(0);

	// Locate opening bracket
	while(lex.Get()!=KOpenBracket && !lex.Eos()) {/*Do nothing*/};
	if(lex.Eos())
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate <ce> opening bracket"));
		User::Leave(KErrGeneral);
		}

	// Count commas ',' upto the closing bracket
	while((c=lex.Get())!=KCloseBracket && !lex.Eos())
		{
		if(c==KComma)
			ceItems++;
		}
	if(lex.Eos())
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate <ce> closing bracket"));
		User::Leave(KErrGeneral);
		}

	// Increment ceItems by 1 to make it denote the number of <speed> parameters
	++ceItems;
	LOGTEXT2(_L8("CATInit::ParseResponseL ceItems:%d"),ceItems);

	ParseBufferLC();
	RemoveUnsolicitedStrings();    // Removes any unsolicited strings
	CATParamListEntry* entry;
	TDblQueIter<CATParamListEntry> iter(iRxResults);

	// 
	// Validate that we received the +CBST: response
	entry=iter++;
	if( !entry || (entry->iResultPtr!=KCBSTResponseString) )
		{
		LOGTEXT(_L8("CATInit::ParseResponseL Failed to locate response string +CBST:"));
		User::Leave(KErrGeneral);
		}	

	// 
	// Process the <speed> list
	TInt i;
	for(i=0;i<speedItems;++i)
		{
		entry=iter++;
		if(entry->iResultPtr.Find(KRangeChar)>KErrNone)
			{
			//
			// Parameter contains a '-' character so it is a range of values

			// Get lower & higer values
			TUint8 lowVal;
			TUint8 highVal;
			TLex8 lexRange;
			const TInt rangeCharPos=entry->iResultPtr.Find(KRangeChar);
			lexRange=entry->iResultPtr.Left(rangeCharPos);
			(void)User::LeaveIfError(lexRange.Val(lowVal,EDecimal));
			lexRange=entry->iResultPtr.Mid(rangeCharPos+1);
			(void)User::LeaveIfError(lexRange.Val(highVal,EDecimal));

			LOGTEXT3(_L8("CATInit::ParseResponseL <speed> low:%d high:%d"),lowVal,highVal);
			
			// Set values
			for(TUint8 i=lowVal;i<=highVal;++i)
				SetSpeedCaps(i,iPhoneGlobals->iCallDataCaps);
			}
		else
			{
			//
			// String does not contain '-' character so it must be just a single value
			const TInt val(CATParamListEntry::EntryValL(entry));
			SetSpeedCaps(val,iPhoneGlobals->iCallDataCaps);
			LOGTEXT2(_L8("CATInit::ParseResponseL <speed> val:%d"),val);
			}
		}


	//
	// Process the <name> list
	for(i=0;i<nameItems;++i)
		{
		entry=iter++;
		if(entry->iResultPtr.Find(KRangeChar)>KErrNone)
			{
			//
			// Parameter contains a '-' character so it is a range of values

			// Get lower & higer values
			TUint8 lowVal;
			TUint8 highVal;
			TLex8 lexRange;
			const TInt rangeCharPos=entry->iResultPtr.Find(KRangeChar);
			lexRange=entry->iResultPtr.Left(rangeCharPos);
			(void)User::LeaveIfError(lexRange.Val(lowVal,EDecimal));
			lexRange=entry->iResultPtr.Mid(rangeCharPos+1);
			(void)User::LeaveIfError(lexRange.Val(highVal,EDecimal));

			LOGTEXT3(_L8("CATInit::ParseResponseL <name> low:%d high:%d"),lowVal,highVal);
			
			// Set values
			for(TUint8 i=lowVal;i<=highVal;++i)
				SetNameCaps(i,iPhoneGlobals->iCallDataCaps);
			}
		else
			{
			//
			// String does not contain '-' character so it must be just a single value
			const TInt val(CATParamListEntry::EntryValL(entry));
			SetNameCaps(val,iPhoneGlobals->iCallDataCaps);
			LOGTEXT2(_L8("CATInit::ParseResponseL <name> val:%d"),val);
			}
		}

	//
	// Process the <ce> list
	for(i=0;i<ceItems;++i)
		{
		entry=iter++;
		if(entry->iResultPtr.Find(KRangeChar)>KErrNone)
			{
			//
			// Parameter contains a '-' character so it is a range of values

			// Get lower & higer values
			TUint8 lowVal;
			TUint8 highVal;
			TLex8 lexRange;
			const TInt rangeCharPos=entry->iResultPtr.Find(KRangeChar);
			lexRange=entry->iResultPtr.Left(rangeCharPos);
			(void)User::LeaveIfError(lexRange.Val(lowVal,EDecimal));
			lexRange=entry->iResultPtr.Mid(rangeCharPos+1);
			(void)User::LeaveIfError(lexRange.Val(highVal,EDecimal));

			LOGTEXT3(_L8("CATInit::ParseResponseL <ce> low:%d high:%d"),lowVal,highVal);
			
			// Set values
			for(TUint8 i=lowVal;i<=highVal;++i)
				SetCECaps(i,iPhoneGlobals->iCallDataCaps);
			}
		else
			{
			//
			// String does not contain '-' character so it must be just a single value
			const TInt val(CATParamListEntry::EntryValL(entry));
			SetCECaps(val,iPhoneGlobals->iCallDataCaps);
			LOGTEXT2(_L8("CATInit::ParseResponseL <ce> val:%d"),val);
			}
		}

	CleanupStack::PopAndDestroy();		// ParseBufferLC pushed object
	}


void CATInit::SetCECaps(TInt aVal,RMobileCall::TMobileCallDataCapsV1& aCaps)
/**
 * This function converts the ETSI speed values to the Etel MM speed enums
 */
	{
	switch (aVal)
		{
	case 0:
		aCaps.iQoSCaps  |= RMobileCall::KCapsTransparent;
		break;
	case 1:
		aCaps.iQoSCaps  |= RMobileCall::KCapsNonTransparent;
		break;
	case 2:
		aCaps.iQoSCaps  |= RMobileCall::KCapsTransparentPreferred;
		break;
	case 3:
		aCaps.iQoSCaps  |= RMobileCall::KCapsNonTransparentPreferred;
		break;
	default:		// default required to prevent ARM compiler warning
		break;		
		}
	}


void CATInit::SetNameCaps(TInt aVal,RMobileCall::TMobileCallDataCapsV1& aCaps)
/**
 * This function converts the ETSI speed values to the Etel MM speed enums
 */
 	{
	switch (aVal)
		{
	case 0:
		aCaps.iServiceCaps |= RMobileCall::KCapsDataCircuitAsynchronous;
		break;
	case 1:
		aCaps.iServiceCaps |= RMobileCall::KCapsDataCircuitSynchronous;
		break;
	case 2:
		aCaps.iServiceCaps |= RMobileCall::KCapsPADAsyncUDI;
		break;
	case 3:
		aCaps.iServiceCaps |= RMobileCall::KCapsPacketAccessSyncUDI;
		break;
	case 4:
		aCaps.iServiceCaps |= RMobileCall::KCapsDataCircuitAsynchronousRDI;
		break;
	case 5:
		aCaps.iServiceCaps |= RMobileCall::KCapsDataCircuitSynchronousRDI;
		break;
	case 6:
		aCaps.iServiceCaps |= RMobileCall::KCapsPADAsyncRDI;
		break;
	case 7:
		aCaps.iServiceCaps |= RMobileCall::KCapsPacketAccessSyncRDI;
		break;
	default:		// default needed to prevent ARM compiler warning
		break;		
		}
	}


void CATInit::SetSpeedCaps(TInt aVal,RMobileCall::TMobileCallDataCapsV1& aCaps)
/**
 * This function converts the ETSI speed values to the Etel MM speed enums
 */
 	{
	switch(aVal)
		{
	case 0:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeedAutobauding; 
		break;
	case 4:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed2400; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV22bis;
		break;
	case 6:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed4800; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV32;
		break;
	case 7:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed9600; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV32;
		break;
	case 12:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed9600; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV34;
		break;
	case 14:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed14400; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV34;
		break;
	case 15:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed19200; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV34;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 16:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed28800; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV34;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 36:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed2400; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		break;
	case 38:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed4800; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		break;
	case 39:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed9600; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		break;
	case 43:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed14400; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 47:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed19200; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 48:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed28800; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 49:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed38400; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 50:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed48000; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 51:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed56000; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolV120;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 68:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed2400; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		break;
	case 70:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed4800; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		break;
	case 71:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed9600; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		break;
	case 75:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed14400; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		aCaps.iHscsdSupport = ETrue;
		break;
	case 79:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed19200; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		aCaps.iHscsdSupport = ETrue;
		break;
	case 80:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed28800; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		aCaps.iHscsdSupport = ETrue;
		break;
	case 81:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed38400; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		aCaps.iHscsdSupport = ETrue;
		break;
	case 82:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed48000; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		aCaps.iHscsdSupport = ETrue;
		break;
	case 83:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed56000; 
		aCaps.iProtocolCaps |= 
			(RMobileCall::KCapsProtocolV110 | RMobileCall::KCapsProtocolX31FlagStuffing);
		aCaps.iHscsdSupport = ETrue;
		break;
	case 115:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed56000; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolBitTransparent;
		aCaps.iHscsdSupport = ETrue;
		break;
	case 116:
		aCaps.iSpeedCaps |= RMobileCall::KCapsSpeed64000; 
		aCaps.iProtocolCaps |= RMobileCall::KCapsProtocolBitTransparent;
		aCaps.iHscsdSupport = ETrue;
		break;
	default:		// default required to stop ARM compiler warnings
		break;		
		}
	}

