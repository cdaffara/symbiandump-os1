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

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include "ATDIAL.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "CALL.H"
#include "ATNOCARR.H"
#include "NOTIFY.H"
#include "ATIO.H"

#include <etelmm.h>
#include "Matstd.h"

const TInt KWaitForConnect=30000; 

CATDialVoice* CATDialVoice::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATDialVoice* dial=new(ELeave) CATDialVoice(aIo, aTelObject, aInit,aPhoneGlobals);
	CleanupStack::PushL(dial);
	dial->ConstructL();		// This ConstructL call is required to allow our base classes to construct
	CleanupStack::Pop();
	return dial;
	}

CATDialVoice::CATDialVoice(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATVoiceCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}


void CATDialVoice::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	LOGTEXT(_L8("Starting Voice Dial Command"));
	iState=EATInitialising;
	iTelnum=REINTERPRET_CAST(TDesC*,aParams);
	CATVoiceCallConnectCommands::Start(aTsyReqHandle,aParams);
	}

void CATDialVoice::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempt to halt the dial process by sending a carriage return, expecting a NO CARRIER
//	message, or if the pre-dial commands are still being sent then simply wait for OK
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	__ASSERT_ALWAYS(iState!=EATNotInProgress,Panic(EATCommand_NotInProgress));		
	LOGTEXT(_L8("Cancelling Voice Dial Command"));
	iIo->WriteAndTimerCancel(this);
	if (iState!=EATInitialising)
		{
		Write(KCarriageReturn(),1);		
		iState = EATCancellingWaitForWriteComplete;
		iPreConnectState=CATCallConnectCommands::ENotInProgress;	
		}
	else
		{
		AddStdExpectStrings(); 
		iPreConnectState=CATCallConnectCommands::ECancelling;
		iState = EATNotInProgress;
		}
	}

TInt CATDialVoice::AddDialExpectStrings()
	{
	LOGTEXT2(_L8("Entered CATDialVoice::AddDialExpectStringsL with iNoDialToneExpectString of %x"),iNoDialToneExpectString);

	AddCommonExpectStrings();

	TInt ret(KErrNone);

	if (!iCallMonitoringExpectString)
		{
			iCallMonitoringExpectString=iIo->AddExpectString(this,KCallMonitoringEventString);
		}

	if (!iBusyExpectString)
		{
		ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameBusy),iBusyString);
		if(ret!=KErrNone)
			return ret;
		AppendWildCardChar(iBusyString);
		iBusyExpectString=iIo->AddExpectString(this,iBusyString);
		}

	if (!iNoDialToneExpectString)
		{
		ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameNoDialTone),iNoDialToneString);
		if(ret!=KErrNone)
			return ret;
		iNoDialToneExpectString=iIo->AddExpectString(this,iNoDialToneString);
		}

	if (!iNoAnswerExpectString)
		{
		ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameNoAnswer),iNoAnswerString);
		if(ret!=KErrNone)
			return ret;
		AppendWildCardChar(iNoAnswerString);
		iNoAnswerExpectString=iIo->AddExpectString(this,iNoAnswerString);
		}

	if (!iDelayedExpectString)
		iDelayedExpectString=iIo->AddExpectString(this,KDelayedString);

	LOGTEXT(_L8("Leaving CATDialVoice::AddDialExpectStringsL"));

	return KErrNone;
	}

TInt CATDialVoice::ValidateDialExpectString()
	{
	CCommChatString* foundChatString = iIo->FoundChatString();

	if (foundChatString == iOKExpectString)
		return KErrNone;

	if (foundChatString == iCallMonitoringExpectString)
	{
		LOGTEXT(_L8("Modem returned *ECAV Ericsson Call Monitoring (similar to NO CARRIER) in response to dial command"));
		return KErrEtelNoCarrier;
	}

	if (foundChatString == iNoCarrierExpectString)
		{
		LOGTEXT(_L8("Modem returned NO CARRIER in response to dial command"));
		return KErrEtelNoCarrier;
		}

	if (foundChatString == iNoDialToneExpectString)
		{
		LOGTEXT(_L8("Modem returned NO DIALTONE in response to dial command"));
		return KErrEtelNoDialTone;
		}

	if (foundChatString == iBusyExpectString)
		{
		LOGTEXT(_L8("Modem returned BUSY in response to dial command"));
		return KErrEtelBusyDetected;
		}

	if (foundChatString == iNoAnswerExpectString)
		{
		LOGTEXT(_L8("Modem returned NO ANSWER in response to dial command"));
		return KErrEtelNoAnswer;
		}

	if (foundChatString == iDelayedExpectString)
		{
		LOGTEXT(_L8("Modem returned DELAYED in response to dial command"));
		return KErrEtelBusyDetected;	// No 'Delayed' error message
		}

	LOGTEXT(_L8("Voice dial command\tunexpected match!"));
	return KErrGeneral;
	}

void CATDialVoice::RemoveDialExpectStrings()
	{
	RemoveCommonExpectStrings();
	iIo->RemoveExpectString(iCallMonitoringExpectString);
	iCallMonitoringExpectString=NULL;	// Set pointer to NULL to denote we have removed the expect string
	iIo->RemoveExpectString(iBusyExpectString);
	iBusyExpectString=NULL;			// Set pointer to NULL to denote we have removed the expect string
	iIo->RemoveExpectString(iNoDialToneExpectString);
	iNoDialToneExpectString=NULL;	// Set pointer to NULL to denote we have removed the expect string
	iIo->RemoveExpectString(iNoAnswerExpectString);
	iNoAnswerExpectString=NULL;		// Set pointer to NULL to denote we have removed the expect string
	iIo->RemoveExpectString(iDelayedExpectString);
	iDelayedExpectString=NULL;		// Set pointer to NULL to denote we have removed the expect string
	}

void CATDialVoice::CompleteWithIOError(TEventSource aSource,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iState = EATNotInProgress;
		CATCallConnectCommands::CompleteWithIOError(aSource,aStatus);
		}
	}


void CATDialVoice::EventSignal(TEventSource aSource)
	{
	LOGTEXT2(_L8("CATDialVoice::EventSignal with iState %d"),iState);
	if ((aSource==ETimeOutCompletion)&&(iState!=EATCancellingReadCompleted)
		&&(iState!=CATDialVoice::EATNotInProgress)
		&&(iState!=CATDialVoice::EDTRDropped)
		&&(iState!=CATDialVoice::EWaitForDTRRaiseSettle)
		&&(iState!=CATDialVoice::EATHangupReadCompleted))
		{
		LOGTEXT(_L8("Timeout Error during voice Dial"));
		RemoveDialExpectStrings();
		RemoveStdExpectStrings();
		iState = EATNotInProgress;
		Complete(KErrTimedOut,aSource);
		return;
		}

	//
	// Read Completions can happen in unexpected places with the 
	// Ericsson Call Monitoring (AT*ECAM=1) turned on, so need check
	// it outside of the state machine.
	//
	// The Ericsson call monitoring replaces the Ericsson SH888 hack also
	//
	if ((aSource==EReadCompletion))
		{
		// Does it match a General Expect String?
		TInt ret=ValidateExpectString();

		// Not General, so does it match a Dial Specific Expect String?
		if (ret != KErrNone)
			ret=ValidateDialExpectString();

		// It doesn't match any Expect Strings at all, so complete with error
		// otherwise just continue with the state machine as normal
		if (ret != KErrNone)
			{
			LOGTEXT2(_L8("Completing call with Error %d"), ret);
			Complete(ret,aSource);
			return;
			}
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
			iState=EATSendDialCommand;
		}
	
	switch(iState)
		{
	case EATSendDialCommand:
		{
		ChangeLineStatus(RCall::EStatusDialling);
		// Setting to EStatusDialling always returns KErrNone
		(void)ChangeCallStatus(RMobileCall::EStatusDialling);
		iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunConnecting);
		TBuf8<KCommsDbSvrMaxFieldLength> dialModifier;
		TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameDialToneWaitModifier),dialModifier);
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		TInt len = iTelnum->Length();
		
		HBufC8* buf = NULL;
		TRAP(ret,buf  = HBufC8::NewL(len));
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		TPtr8 newTelnum(buf->Des());
		newTelnum.Copy(*iTelnum);
		
   		TInt aPos=newTelnum.FindF(dialModifier);
		if (aPos!=KErrNotFound)
			newTelnum.Delete(aPos,1);
		iTxBuffer.Format(KDialVoiceCommandFormat,&newTelnum);
		delete buf;
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this);
		iState=EATDialWaitForWriteComplete;
		}
		break;

	case EATDialWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TInt ret=AddDialExpectStrings();
		AddStdExpectStrings();
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		iIo->SetTimeOut(this,KWaitForConnect); // 30 seconds for the other end to pick up.
		iState=EATDialReadComplete;
		}
		break;

	case EATDialReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			RemoveDialExpectStrings();
			RemoveStdExpectStrings();
			iState = EATNotInProgress;
			Complete(KErrNone,aSource);
			}
		break;

	case EATCancellingWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TInt ret=AddDialExpectStrings();
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		if (!iOKExpectString)
			{
			iOKExpectString=iIo->AddExpectString(this,KOkString);
			}
		iIo->SetTimeOut(this);
		iState=EATCancellingReadCompleted;
		}
		break;

	case EATCancellingReadCompleted:
			{
			iState = EATNotInProgress;
			if (aSource==EReadCompletion)
				{
				TInt ret=ValidateDialExpectString();
				RemoveDialExpectStrings();
				iOKExpectString=NULL;
				if (ret==KErrNone)
					{
					iIo->DropDtr();		
					iIo->SetTimeOut(this,KDTRLowPeriod);
					iState=EDTRDropped;
					return;
					}
				}
			RemoveDialExpectStrings();
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
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		Write(KHangUpCommand(),1);
		iState=EATHangupWaitForWriteComplete;
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

	default:
		;
		}		
	}

//
// Dial number for data call. Remove the dial modifier W if found and only add it again if the
// call parameters indicate that we are to wait for dial tone.
//
CATDialData* CATDialData::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATDialData* dial=new(ELeave) CATDialData(aIo, aTelObject, aInit,aPhoneGlobals);
	CleanupStack::PushL(dial);
	dial->ConstructL();
	CleanupStack::Pop();
	return dial;
	}

CATDialData::CATDialData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATDataCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}


void CATDialData::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	LOGTEXT(_L8("Starting ATD Dial Command"));
	iState=EATInitialising;
	iTelnum=REINTERPRET_CAST(TDesC*,aParams);
	CATDataCallConnectCommands::Start(aTsyReqHandle,aParams);
	}

void CATDialData::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempt to halt the dial process by sending a carriage return, expecting a NO CARRIER
//	message, or if the pre-dial commands are still being sent then simply wait for OK
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	__ASSERT_ALWAYS(iState!=EATNotInProgress,Panic(EATCommand_NotInProgress));		
	LOGTEXT(_L8("Cancelling ATD Dial Command"));
	if (iState!=EATInitialising)
		{
		iIo->WriteAndTimerCancel(this);
		Write(KCarriageReturn(),1);		
		iState = EATCancellingWaitForWriteComplete;
		iPreConnectState=CATCallConnectCommands::ENotInProgress;	
		}
	else
		{
		AddStdExpectStrings(); 
		iPreConnectState=CATCallConnectCommands::ECancelling;
		iState = EATNotInProgress;
		}
	}

void CATDialData::AddDialExpectStringsL()
	{
	LOGTEXT2(_L8("Entered CATDialData::AddDialExpectStringsL with iNoDialToneExpectString of %x"),iNoDialToneExpectString);
	(void)User::LeaveIfError(AddCommonExpectStrings());
	if (!iBusyExpectString)
		{
		(void)User::LeaveIfError(iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameBusy),iBusyString));
		AppendWildCardChar(iBusyString);
		iBusyExpectString=iIo->AddExpectString(this,iBusyString);
		}
	if (!iNoDialToneExpectString)
		{
		(void)User::LeaveIfError(iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameNoDialTone),iNoDialToneString));
		iNoDialToneExpectString=iIo->AddExpectString(this,iNoDialToneString);
		}
	if (!iNoAnswerExpectString)
		{
		(void)User::LeaveIfError(iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameNoAnswer),iNoAnswerString));
		AppendWildCardChar(iNoAnswerString);
		iNoAnswerExpectString=iIo->AddExpectString(this,iNoAnswerString);
		}
	if (!iDelayedExpectString)
		iDelayedExpectString=iIo->AddExpectString(this,KDelayedString);
	LOGTEXT(_L8("Leaving CATDialData::AddDialExpectStringsL"));
	}

void CATDialData::ValidateDialExpectStringL()
	{
	CCommChatString* foundChatString = iIo->FoundChatString();
	if (foundChatString == iConnectExpectString)
		{
		return;
		}
	if (foundChatString == iNoCarrierExpectString)
		{
		LOGTEXT(_L8("Modem returned NO CARRIER in response to dial command"));
		User::Leave(KErrEtelNoCarrier);
		}
	if (foundChatString == iNoDialToneExpectString)
		{
		LOGTEXT(_L8("Modem returned NO DIALTONE in response to dial command"));
		User::Leave(KErrEtelNoDialTone);
		}
	if (foundChatString == iBusyExpectString)
		{
		LOGTEXT(_L8("Modem returned BUSY in response to dial command"));
		User::Leave(KErrEtelBusyDetected);
		}
	if (foundChatString == iNoAnswerExpectString)
		{
		LOGTEXT(_L8("Modem returned NO ANSWER in response to dial command"));
		User::Leave(KErrEtelNoAnswer);
		}
	if (foundChatString == iDelayedExpectString)
		{
		LOGTEXT(_L8("Modem returned DELAYED in response to dial command"));
		User::Leave(KErrEtelBusyDetected);	// No 'Delayed' error message
		}
	LOGTEXT(_L8("Data dial command\tunexpected match!"));
	User::Leave(KErrGeneral);
	}

void CATDialData::RemoveDialExpectStrings()
	{
	RemoveCommonExpectStrings();
	iIo->RemoveExpectString(iBusyExpectString);
	iBusyExpectString=NULL;
	iIo->RemoveExpectString(iNoDialToneExpectString);
	iNoDialToneExpectString=NULL;
	iIo->RemoveExpectString(iNoAnswerExpectString);
	iNoAnswerExpectString=NULL;
	iIo->RemoveExpectString(iDelayedExpectString);
	iDelayedExpectString=NULL;
	}

void CATDialData::CompleteWithIOError(TEventSource aSource,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iState = EATNotInProgress;
		CATCallConnectCommands::CompleteWithIOError(aSource,aStatus);
		}
	}

void CATDialData::EventSignal(TEventSource aSource)
	{
	LOGTEXT3(_L8("CATDialData::EventSignal with iState:%d aSource:%d"),iState,aSource);
	
	if(aSource==ETimeOutCompletion)
		{
		if(iState!=EATSpeedReadComplete && iState!=EATCancellingReadCompleted &&
		   iState!=EATNotInProgress && 
		   iState!=EDTRDropped && iState!=EWaitForDTRRaiseSettle && iState!=EATHangupReadCompleted)
			{
			LOGTEXT(_L8("CATDialData::EventSignal  Timeout Error during Dial"));
			RemoveDialExpectStrings();
			iState = EATNotInProgress;
			Complete(KErrTimedOut,aSource);
			return;
			}
		}

	if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted
		&& iPreConnectState!=CATCallConnectCommands::ENotInProgress)
		{
		CATCallConnectCommands::PreConnectEventSignal(aSource);
		if (iPreConnectState==CATCallConnectCommands::ENotInProgress)	// cancelled
			{
			LOGTEXT2(_L8("CATDialData::EventSignal1 with iPreConnectState %d"),iPreConnectState);
			iState=EATNotInProgress;
			}
		if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted)
			{
			LOGTEXT2(_L8("CATDialData::EventSignal2 with iPreConnectState %d"),iPreConnectState);
			return;
			}
		else
			{
			//
			// Start the sending of the +CBST= command.
			//
			// If the SendBearerCapsCommand fails to start the sending
			// of the +CBST= command then we have to start the dial instead
			if(!SendBearerCapsCommand(aSource))		// Will set iState=EATBearerCapsWaitForWriteComplete
				StartDialCommand(aSource);			// Will set iState=EATDialWaitForWriteComplete
			return;
			}
		}
	
	switch(iState)
		{
	case EATBearerCapsWaitForWriteComplete:
		__ASSERT_DEBUG(iCallType==EDataCall,Panic(ENotDataCallType));
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		StandardWriteCompletionHandler(aSource,4);
		iState=EATBearerCapsReadComplete;
		break;

	case EATBearerCapsReadComplete:
		__ASSERT_DEBUG(iCallType==EDataCall,Panic(ENotDataCallType));
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		//
		// Ignore any OK, ERROR or any other response from the modem.
		// This is because some modems, eg. those with out speakers, will ERROR these 
		// configuration commands even though the call can go ahead.
		RemoveStdExpectStrings();

		
		//
		// Start the dial command
		StartDialCommand(aSource); // This will set iState to EATDialWaitForWriteComplete
		break;

	case EATDialWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TRAPD(ret,AddDialExpectStringsL());
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		iIo->SetTimeOut(this,(iPhoneGlobals->iPhoneStatus.iWaitForCarrierTime*1000)+KExtraWaitTime);
		iState=EATDialReadComplete;
		}
		break;

	case EATDialReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TRAPD(ret,ValidateDialExpectStringL());
			RemoveDialExpectStrings();
			if (ret!=KErrNone)
				{
				iState = EATNotInProgress;
				Complete(ret,aSource);
				break;
				}
			iIo->SetTimeOut(this,(KTimeForExtraRxData*5));
			iState=EATSpeedReadComplete;
			}
		break;

	case EATSpeedReadComplete:
		{
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->WriteAndTimerCancel(this);
		TInt ret=ParseForBearerCapsResponse();
		iState = EATNotInProgress;
		if (ret!=KErrNone)
			{
			Complete(ret,aSource);
			break;
			}
		ret=ParseForBearerSpeedResponse();
		Complete(ret,aSource);
		break;
		}

	case EATCancellingWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TRAPD(ret,AddDialExpectStringsL());
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		if (!iOKExpectString)
			{
			iOKExpectString=iIo->AddExpectString(this,KOkString);
			}
		iIo->SetTimeOut(this);
		iState=EATCancellingReadCompleted;
		}
		break;

	case EATCancellingReadCompleted:
		{
		if (aSource==ETimeOutCompletion)
			{
			// The phone ignored the request to cancel the connection.
			// The modem will be forced to drop the connection when we
			// lower the DTR.
			LOGTEXT(_L8("Phone ignored dial cancel request"));
			}
		else if (aSource==EReadCompletion)
			{
			if (iIo->FoundChatString() != iOKExpectString)
				{
				// The modem did not respond with OK as expected.
				LOGTEXT(_L8("Phone returned an unexpected response to a dial cancel request"));
				}
			}
		RemoveDialExpectStrings();
		iIo->RemoveExpectString(iOKExpectString);
		iOKExpectString=NULL;
		iIo->DropDtr();
		iIo->SetTimeOut(this,KDTRLowPeriod);
		iState=EDTRDropped;
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
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		Write(KHangUpCommand(),1);
		iState=EATHangupWaitForWriteComplete;
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

	default:
		;
		}		
	}

void CATDialData::StartDialCommand(TEventSource aSource)
	{
	//
	// Start the dial command
	ChangeLineStatus(RCall::EStatusDialling);
	// Setting to EStatusDialling always returns KErrNone
	(void)ChangeCallStatus(RMobileCall::EStatusDialling);
	iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunConnecting);
	TBuf8<KCommsDbSvrMaxFieldLength> dialModifier;
	TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameDialToneWaitModifier),dialModifier);
	if (ret)
		{
		Complete(ret,aSource);
		return;
		}
	TInt len = iTelnum->Length();
		
	HBufC8* buf = NULL;
	TRAP(ret,buf  = HBufC8::NewL(len));
	if (ret)
		{
		Complete(ret,aSource);
		return;
		}
	TPtr8 newTelnum(buf->Des());
	newTelnum.Copy(*iTelnum);

	TInt aPos=newTelnum.FindF(dialModifier);
	if (aPos!=KErrNotFound)
		{
		newTelnum.Delete(aPos,1);
		}
	iTxBuffer.Format(KDialDataCommandFormat,&newTelnum);
	delete buf;
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this);
	iState=EATDialWaitForWriteComplete;
	}

TBool CATDialData::SendBearerCapsCommand(TEventSource /*aSource*/)
/*
 * @return ETrue if +CBST= was sent 
 * @return EFalse if +CBST= was not sent due to not having enough specified params
 */ 	
	{
	LOGTEXT(_L8("CATDataDial::SendBearerCapsCommand"));

	//
	// Assemble and send (if required) the +CBST=... string to the phone
	// to configure the settings for the next data call.
	// Use utility function provided by CCallMobileData
	CCallMobileData* parent=static_cast<CCallMobileData*>(iTelObject);
	if(parent->AssembleCBSTSetString(iTxBuffer)==KErrNone)
		{
		//
		// Send our AT command to the phone
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this);	
		iState=EATBearerCapsWaitForWriteComplete;
		return ETrue;
		}

	return EFalse;		// We were unable to send a +CBST string 
	}
