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
//

#include "ATANSWER.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "LINE.H"
#include "CALL.H"
#include "ATNOCARR.H"
#include "NOTIFY.H"
#include "ATIO.H"

const TInt KWaitForVoiceConnect=4000; 
const TInt KWaitForTickleVoiceConnect=3000;
_LIT8(KTickle,"AT+\r");		// To ensure off-line mode.

//
// CATAnswerVoice
//
CATAnswerVoice* CATAnswerVoice::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)	
	{
	CATAnswerVoice* answer=new(ELeave) CATAnswerVoice(aIo, aTelObject, aInit,aPhoneGlobals);
	CleanupStack::PushL(answer);
	answer->ConstructL();
	CleanupStack::Pop();
	return answer;
	}

CATAnswerVoice::CATAnswerVoice(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATVoiceCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATAnswerVoice::~CATAnswerVoice()
	{}

void CATAnswerVoice::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	LOGTEXT(_L8("Starting ATA Answer Voice Command"));
	LOGTEXT2(_L8("Comm signals : %x"),iIo->Signals());
	iState=EATInitialising;
	CATVoiceCallConnectCommands::Start(aTsyReqHandle,aParams);
	}

void CATAnswerVoice::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the answering process by sending a carriage return.
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	__ASSERT_ALWAYS(iState!=EATNotInProgress,Panic(EATCommand_NotInProgress));		
	LOGTEXT(_L8("Cancelling Answer Voice Command"));
	iIo->WriteAndTimerCancel(this);
	if (iState!=EATInitialising) // the ATA has been sent
		{
		Write(KCarriageReturn(),1);
		iPreConnectState=CATCallConnectCommands::ENotInProgress;	
		iState = EATCancellingWaitForWriteComplete;
		}
	else	// the pre-ATA sequence is still in progress
		{
		AddStdExpectStrings();
		iPreConnectState=CATCallConnectCommands::ECancelling;
		iState = EATNotInProgress;
		}
	}

void CATAnswerVoice::CompleteWithIOError(TEventSource aSource,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iState = EATNotInProgress;
		CATCallConnectCommands::CompleteWithIOError(aSource,aStatus);
		}
	}

void CATAnswerVoice::EventSignal(TEventSource aSource)
	{
	// handle bug in Nokia 8210 which does not respond OK when answering
	// incoming voice calls
	// assume timeout here means that the call has connected
 	if((aSource==ETimeOutCompletion)
		&&(iState!=EATCancellingReadCompleted)&&(iState!=EATNotInProgress)
		&&(iPreConnectState!=EATWaitForATCheckOK))
		{
		LOGTEXT(_L8("Timeout Error during Answer"));
		iState = EATNotInProgress;
		Complete(KErrTimedOut,aSource);
		return;
		}
	
	if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted
		&& iPreConnectState!=CATCallConnectCommands::ENotInProgress)
		{
		CATCallConnectCommands::PreConnectEventSignal(aSource);
		if (iPreConnectState==CATCallConnectCommands::ENotInProgress)	// cancelled
			iState=EATNotInProgress;
		if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted)
			return;
		else
			iState=EATSendAnswerCommand;
		}

	switch(iState)
		{
	case EATSendAnswerCommand:
		{
		ChangeLineStatus(RCall::EStatusAnswering);
		// EStatusAnswering results in KErrNone return;
		(void)ChangeCallStatus(RMobileCall::EStatusAnswering);
		iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunConnecting);
		TPtrC8 answerCommand(KAnswerCommand);
		WriteExpectingResults(answerCommand,3);
		iState=EATAnswerWaitForWriteComplete;
		break;
		}

	case EATAnswerWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		LOGTEXT2(_L8("Comm signals : %x"),iIo->Signals());
		CPhoneHayes* phone=STATIC_CAST(CPhoneHayes*,iTelObject->Owner()->Owner());
		phone->StopRingCounter();	// RING should no longer come in		
		AddStdExpectStrings();
		iIo->SetTimeOut(this,KWaitForVoiceConnect);		// Wait for Answer
		iState=EATAnswerReadCompleted;
		}
		break;

	case EATAnswerReadCompleted:
			{
			if (aSource==ETimeOutCompletion)
				{
				Complete(KErrNone,EReadCompletion); 
				return;
				}
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			TInt ret(ValidateExpectString());
			if (ret)
				{
				Complete(ret,aSource);
				break;
				}
			iIo->WriteAndTimerCancel(this);
			// handle bug in Ericsson SH888 which does not report "NO CARRIER" when the
			// call is dropped unless the modem is prodded following the ATA command
			LOGTEXT(_L8("handle bug in Ericsson SH888 which does not report NO CARRIER when the"));
			LOGTEXT(_L8("call is dropped unless the modem is prodded following the ATA command"));
			iIo->Write(this,KTickle);
			iIo->SetTimeOut(this);
			iState=EATTickleWaitForWriteComplete;
			}
		break;

		
	case EATTickleWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->SetTimeOut(this,KWaitForTickleVoiceConnect);
		iState=EATTickleReadCompleted;
		}
		break;

	case EATTickleReadCompleted:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		RemoveStdExpectStrings();
		Complete(KErrNone,aSource);
		break;

		
	case EATCancellingWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		AddCommonExpectStrings();
		iIo->SetTimeOut(this);
		iState=EATCancellingReadCompleted;
		break;

	case EATCancellingReadCompleted:
			{
			iState = EATNotInProgress; 
			RemoveCommonExpectStrings();
			if (aSource==EReadCompletion)
				{
				TInt ret(ValidateExpectString());
				if (ret==KErrNone)
					{
					iIo->DropDtr();		// Has connected despite sending CR
					iIo->SetTimeOut(this,KDTRLowPeriod);
					iState=EDTRDropped;
					return;
					}
				}
			Complete(KErrCancel,aSource);
			}
		break;

	case EDTRDropped:
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->Cancel();
		iIo->RaiseDTR();
		iIo->Read();
		iIo->SetTimeOut(this,KDTRHighSettle);
		iState=EWaitForDTRRaiseSettle;
		break;

	case EWaitForDTRRaiseSettle:
		{
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		TPtrC8 hangUpCommand(KHangUpCommand);
		Write(hangUpCommand,1);
		iState=EATHangupWaitForWriteComplete;
		}
		break;

	case EATHangupWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		StandardWriteCompletionHandler(aSource,2);
		iState=EATHangupReadCompleted;
		break;
	
	case EATHangupReadCompleted:
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		Complete(KErrCancel,aSource);
		break;

	case EATInitialising:
	case EATNotInProgress:
	default:
		;
		}
	}

void CATAnswerVoice::Complete(TInt aError,TEventSource aSource)
	{
	CATVoiceCallConnectCommands::Complete(aError,aSource);
	}



//
// CATAnswerData
//
CATAnswerData* CATAnswerData::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)	{
	CATAnswerData* answer=new(ELeave) CATAnswerData(aIo, aTelObject, aInit,aPhoneGlobals);
	CleanupStack::PushL(answer);
	answer->ConstructL();
	CleanupStack::Pop();
	return answer;
	}

CATAnswerData::CATAnswerData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATDataCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATAnswerData::~CATAnswerData()
	{}

void CATAnswerData::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	LOGTEXT(_L8("Starting ATA Answer Command"));
	LOGTEXT2(_L8("Comm signals : %x"),iIo->Signals());
	iState=EATInitialising;
	CATDataCallConnectCommands::Start(aTsyReqHandle,aParams);
	}

void CATAnswerData::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the answering process by sending a carriage return.
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	__ASSERT_ALWAYS(iState!=EATNotInProgress,Panic(EATCommand_NotInProgress));		
	LOGTEXT(_L8("Cancelling Answer Command"));
	iIo->WriteAndTimerCancel(this);
	if (iState!=EATInitialising) // the ATA has been sent
		{
		Write(KCarriageReturn(),1);
		iPreConnectState=CATCallConnectCommands::ENotInProgress;	
		iState = EATCancellingWaitForWriteComplete;
		}
	else	// the pre-ATA sequence is still in progress
		{
		AddStdExpectStrings();
		iPreConnectState=CATCallConnectCommands::ECancelling;
		iState = EATNotInProgress;
		}
	}

TInt CATAnswerData::ValidateAnswerExpectString()
	{
	TInt ret(KErrUnknown);
	if(iIo->FoundChatString()==iConnectExpectString)
		ret=KErrNone;
	else if(iIo->FoundChatString()==iNoCarrierExpectString)
		ret=KErrEtelNoCarrier;
	return ret;
	}

void CATAnswerData::CompleteWithIOError(TEventSource aSource,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iState = EATNotInProgress;
		CATCallConnectCommands::CompleteWithIOError(aSource,aStatus);
		}
	}

void CATAnswerData::EventSignal(TEventSource aSource)
	{
	if((aSource==ETimeOutCompletion)&&(iState!=EATSpeedReadCompleted)
		&&(iState!=EATCancellingReadCompleted)&&(iState!=EATNotInProgress)
		&&(iPreConnectState!=EATWaitForATCheckOK))
		{
		LOGTEXT(_L8("Timeout Error during Answer"));
		iState = EATNotInProgress;
		Complete(KErrTimedOut,aSource);
		return;
		}
	
	if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted
		&& iPreConnectState!=CATCallConnectCommands::ENotInProgress)
		{
		CATCallConnectCommands::PreConnectEventSignal(aSource);
		if (iPreConnectState==CATCallConnectCommands::ENotInProgress)	// cancelled
			iState=EATNotInProgress;
		if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted)
			return;
		else
			iState=EATSendAnswerCommand;
		}

	switch(iState)
		{
	case EATSendAnswerCommand:
		{
		ChangeLineStatus(RCall::EStatusAnswering);
		// Don't bother checking return value. Setting to EStatusAnswering always returns KErrNone
		(void)ChangeCallStatus(RMobileCall::EStatusAnswering);
		iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunConnecting);
		TPtrC8 answerCommand(KAnswerCommand);
		WriteExpectingResults(answerCommand,3);
		iState=EATAnswerWaitForWriteComplete;
		break;
		}

	case EATAnswerWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		LOGTEXT2(_L8("Comm signals : %x"),iIo->Signals());
		CPhoneHayes* phone=STATIC_CAST(CPhoneHayes*,iTelObject->Owner()->Owner());
		phone->StopRingCounter();			// RING should no longer come in		
		TInt ret=AddCommonExpectStrings();
		if (ret!=KErrNone)
			{
			Complete(ret,aSource);
			break;
			}
		iIo->SetTimeOut(this,(iPhoneGlobals->iPhoneStatus.iWaitForCarrierTime*1000)+KExtraWaitTime);
		iState=EATAnswerReadCompleted;
		}
		break;

	case EATAnswerReadCompleted:
			{
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			TInt ret=ValidateAnswerExpectString();
			RemoveCommonExpectStrings();
			if (ret!=KErrNone)
				{
				iState = EATNotInProgress;
				Complete(ret,aSource);
				break;
				}
			iIo->SetTimeOut(this,KTimeForExtraRxData);
			iState=EATSpeedReadCompleted;
			}
		break;

	case EATSpeedReadCompleted:
			{
			__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
			TInt ret=ParseForBearerCapsResponse();
			iState = EATNotInProgress;
			if (ret!=KErrNone)
				{
				Complete(ret,aSource);
				break;
				}
			ret=ParseForBearerSpeedResponse();
			Complete(ret,aSource);
			}
		break;
		
	case EATCancellingWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TInt ret=AddCommonExpectStrings();
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		iIo->SetTimeOut(this);
		iState=EATCancellingReadCompleted;
		break;
		}

	case EATCancellingReadCompleted:
			{
			iState = EATNotInProgress;
			if (aSource==EReadCompletion)
				{
				TInt ret=ValidateAnswerExpectString();
				RemoveCommonExpectStrings();
				if (ret==KErrNone)
					{
					iIo->DropDtr();		// Has connected despite sending CR
					iIo->SetTimeOut(this,KDTRLowPeriod);
					iState=EDTRDropped;
					return;
					}
				}
			RemoveCommonExpectStrings();
			Complete(KErrCancel,aSource);
			}
		break;

	case EDTRDropped:
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->Cancel();
		iIo->RaiseDTR();
		iIo->Read();
		iIo->SetTimeOut(this,KDTRHighSettle);
		iState=EWaitForDTRRaiseSettle;
		break;

	case EWaitForDTRRaiseSettle:
		{
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		TPtrC8 hangUpCommand(KHangUpCommand);
		Write(hangUpCommand,1);
		iState=EATHangupWaitForWriteComplete;
		}
		break;

	case EATHangupWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		StandardWriteCompletionHandler(aSource,2);
		iState=EATHangupReadCompleted;
		break;
	
	case EATHangupReadCompleted:
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		Complete(KErrCancel,aSource);	// if ret!=KErrNone, perhaps don't complete but carry on
		break;

	case EATInitialising:
	case EATNotInProgress:
	default:
		;
		}
	}

void CATAnswerData::Complete(TInt aError,TEventSource aSource)
	{
	CATDataCallConnectCommands::Complete(aError,aSource);
	}

