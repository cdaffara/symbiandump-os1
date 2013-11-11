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
// Hang Up Command
// 
//

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include "ATHANGUP.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "CALL.H"
#include "ATNOCARR.H"
#include "NOTIFY.H"
#include "ATIO.H"

const TInt KTimeOut=3;		// seconds

//
// CATHangUpVoice
//
CATHangUpVoice* CATHangUpVoice::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATHangUpVoice* hangup=new(ELeave) CATHangUpVoice(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(hangup);
	hangup->ConstructL();
	CleanupStack::Pop();
	return hangup;
	}

CATHangUpVoice::CATHangUpVoice(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATCallAlterCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATHangUpVoice::~CATHangUpVoice()
	{
	iIo->RemoveExpectStrings(this);
	}

void CATHangUpVoice::ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams,TCallInfoTSY* aCallInfo)
//
//	Overloaded function ensures a hang up does not begin an initialise sequence
//
	{
	iCallInfo = aCallInfo;
	Start(aTsyReqHandle,aParams);
	}

void CATHangUpVoice::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
	{
	LOGTEXT(_L8("Starting ATH Hang Up Voice Call Command"));
	iReqHandle=aTsyReqHandle;
	ChangeLineStatus(RCall::EStatusHangingUp);
	// Setting to EStatusDialling always returns KErrNone
	(void)ChangeCallStatus(RMobileCall::EStatusDisconnecting);
	// must change status before calling 
	// CheckNotification as the actual current status
	// is written back with the notify completion.
	// to prevent any completion of reqs synchronously when relinquishing ownership
	// the server will cancel any completion of notifications such as call status change once
	// this function has completed.
	if (iCallInfo->iClientPanicOccurred==ENoPanicOccurred)
		iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunHangingUp);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	CATCallAlterCommands::Start(aTsyReqHandle,aParams);
	REINTERPRET_CAST(CCallHayes*,iTelObject)->iWaitForNoCarrier->StopWait();
	iIo->Cancel();
	TCommConfig aConfigPckg;
	TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeHangUp);	
	if (ret==KErrNone)
		ret = iIo->ConfigurePort(aConfigPckg);
	if (ret!=KErrNone)
		{
		Complete(ret,EReadCompletion); // EReadCompletion so another Read will not be queued
		}
	else
		{
		// No need to drop DTR on voice calls
		Write(KHangUpCommand(),1);
		iState=EATHangupWaitForWriteComplete;
		}
	}

void CATHangUpVoice::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Cancel the hang up (most often called from FlushReqs during a shutdown)
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("Cancelling HangUp"));
	iIo->WriteAndTimerCancel(this);
	iIo->SetTimeOut(this,KOneSecondPause);
	iState=EHangUpCancelling;
	}


void CATHangUpVoice::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion && iState!=EATHangupReadCompleted
		&& iState!=EHangUpCancelling)
		{
		LOGTEXT(_L8("Timeout Error during Hang Up"));
		Complete(KErrNone,aSource); // complete with KErrNone for hangup
		return;
		}
	
	switch(iState)
		{
	case EATHangupWaitForWriteComplete:
		StandardWriteCompletionHandler(aSource,KTimeOut);
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		iIo->Read();
		iState=EATHangupReadCompleted;
		break; 
	
//	case EATHangupReadCompleted:
//		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
//			{
//			TInt ret=ValidateHangUpExpectString();
//			if (ret)
//				{
//				Complete(ret,aSource);
//				break;
//				}
//			RemoveStdExpectStrings();
//			iState=EATHangupReadCompleted;
//			}
//		break;
		
	case EATHangupReadCompleted:
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		if (aSource==ETimeOutCompletion)
			Complete(KErrNone,aSource);
		else
			{
			TInt ret=ValidateHangUpExpectString();
			Complete(ret,aSource);
			}
		break;
		
	case EHangUpCancelling:
		if (aSource==EWriteCompletion)
			{
			iIo->SetTimeOut(this,KOneSecondPause);
			}
		if (aSource==EReadCompletion || aSource==ETimeOutCompletion)
			{
			LOGTEXT(_L8("Hang up cancelled"));
			iIo->WriteAndTimerCancel(this);
			iIo->RemoveExpectStrings(this);
			iOKExpectString=NULL;
			iErrorExpectString=NULL;
			REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
			iTelObject->ReqCompleted(iReqHandle,KErrCancel);
			}
		break;
				
	default:
		;
		}
	}

TInt CATHangUpVoice::ValidateHangUpExpectString()
	{
	CCommChatString* foundChatString = iIo->FoundChatString();
	if (foundChatString == iNoCarrierExpectString)
		{
		LOGTEXT(_L8("Modem returned NO CARRIER in response to hang up command"));
		return KErrNone;
		}
	if (foundChatString == iOKExpectString)
		{
		return KErrNone;
		}
	LOGTEXT(_L8("Hang up command\tunexpected match!"));
	return KErrGeneral;
	}


void CATHangUpVoice::Complete(TInt aError,TEventSource aSource)
	{
	iIo->RemoveExpectStrings(this);
	iNoCarrierExpectString=NULL;
	REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
	iIo->Cancel();
	TCommConfig aConfigPckg;
	aError = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeInit);
	if (aError==KErrNone)
		aError = iIo->ConfigurePort(aConfigPckg);
	if((aSource==EWriteCompletion)||(aSource==ETimeOutCompletion))
		iIo->Read();
	CATCommands::Complete(aError,aSource);
	if (iCallInfo->iClientPanicOccurred==ENoPanicOccurred)
		{
		iTelObject->ReqCompleted(iReqHandle, aError);
		}
	else
		{
		iComplete = CCompleteRelinquish::New(iTelObject);
		iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
		iComplete->Call();	// calls the AysncOneShot Relinquish completion function
		iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
		}	
	iState=EATNotInProgress;
	}


//
//
// Hang Up Data Call
//
CATHangUpData* CATHangUpData::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATHangUpData* hangup=new(ELeave) CATHangUpData(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(hangup);
	hangup->ConstructL();
	CleanupStack::Pop();
	return hangup;
	}

CATHangUpData::CATHangUpData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATCallAlterCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATHangUpData::~CATHangUpData()
	{
	iIo->RemoveExpectStrings(this);
	}

void CATHangUpData::ExecuteCommand(TTsyReqHandle aTsyReqHandle, TAny* aParams,TCallInfoTSY* aCallInfo)
//
//	Overloaded function ensures a hang up does not begin an initialise sequence
//
	{
	iCallInfo = aCallInfo;
	Start(aTsyReqHandle,aParams);
	}

void CATHangUpData::Start(TTsyReqHandle aTsyReqHandle,TAny* aParams)
	{
	LOGTEXT(_L8("Starting ATH Hang Up Command"));
	iReqHandle=aTsyReqHandle;
	ChangeLineStatus(RCall::EStatusHangingUp);
	// Always returns KErrNone for EStatusDisconnecting
	(void)ChangeCallStatus(RMobileCall::EStatusDisconnecting);
										// must change status before calling 
										// CheckNotification as the actual current status
										// is written back with the notify completion.
	// to prevent any completion of reqs synchronously when relinquishing ownership
	// the server will cancel any completion of notifications such as call status change once
	// this function has completed.
	if (iCallInfo->iClientPanicOccurred==ENoPanicOccurred)
		iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunHangingUp);
	
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL, Panic(EGeneral));
	CATCallAlterCommands::Start(aTsyReqHandle,aParams);
	REINTERPRET_CAST(CCallHayes*,iTelObject)->iWaitForNoCarrier->StopWait();
	iIo->Cancel();
	TCommConfig aConfigPckg;
	TInt ret = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeHangUp);	
	if (ret==KErrNone)
		ret = iIo->ConfigurePort(aConfigPckg);
	if (ret!=KErrNone)
		{
		Complete(ret,EReadCompletion); // EReadCompletion so another Read will not be queued
		}
	else
		{
		iIo->DropDtr();		
		iIo->SetTimeOut(this, KDTRLowPeriod);
		iState=EDTRDropped;
		}
	}
		
void CATHangUpData::ValidateHangUpExpectStringL()
	{
	CCommChatString* foundChatString = iIo->FoundChatString();
	if (foundChatString == iNoCarrierExpectString)
		{
		LOGTEXT(_L8("Modem returned NO CARRIER in response to hang up command"));
		}
	else if (foundChatString != iOKExpectString)
		{
		LOGTEXT(_L8("Modem returned unknown response to hang up command"));
		User::Leave(KErrGeneral);
		}	
	}

void CATHangUpData::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Cancel the hang up (most often called from FlushReqs during a shutdown)
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("Cancelling HangUp"));
	iIo->WriteAndTimerCancel(this);
	iIo->SetTimeOut(this,KOneSecondPause);
	iState=EHangUpCancelling;
	}

void CATHangUpData::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iNoCarrierExpectString=NULL;
		iErrorCode = aStatus;
		//	SetToIdle calls CATWaitForNoCarrier::StopWait() which removes expect strings - 
		//  this would cause a panic if they hadn't been removed already in 
		//  CATHangUpData::Start(), because CATIO assumes all the CompleteWithIOError 
		//  functions that it calls do not remove them. It removes them itself in
		//  CATIO::SignalCommandsWithError().
		STATIC_CAST(CCallHayes*,iTelObject)->SetToIdle();
		User::After(KDeltaTimerDefaultGranularity);		// wait for a clock tick and continue
		if (iCallInfo->iClientPanicOccurred != ENoPanicOccurred)
			{
			iComplete = CCompleteRelinquish::New(iTelObject);
			iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
			iComplete->Call();	
			iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
			}
		else
			iTelObject->ReqCompleted(iReqHandle, KErrNone);
		iState = EATNotInProgress;
		}
	}

void CATHangUpData::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion && iState!=EDTRDropped && iState!=EWaitForDTRRaiseSettle
		&& iState!=EATEscapeSeqCompleted && iState!=EATHangupReadCompleted
		&& iState!=EWaitForDTRRaiseSettle && iState!=EHangUpCancelling)
		{
		LOGTEXT(_L8("Timeout Error during Hang Up"));
		Complete(KErrNone,aSource); // complete with KErrNone for hangup
		return;
		}
	if (iErrorCode!=KErrNone)
		{
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		iErrorCode=KErrNone;
		}

	switch(iState)
		{
	case EDTRDropped:
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->Cancel();
		iIo->RaiseDTR();
		iIo->Read();
		iIo->SetTimeOut(this,KDTRHighSettle);
		iState=EWaitForDTRRaiseSettle;
		break;

	case EWaitForDTRRaiseSettle:
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		if (aSource!=ETimeOutCompletion)
			{
			TRAPD(ret,ValidateHangUpExpectStringL());
			Complete(ret,aSource);	
			}
		else
			{
			RemoveStdExpectStrings();
			iIo->RemoveExpectString(iNoCarrierExpectString);// poss. problem as it removes CATWaitForNoCarrier's string too
			iNoCarrierExpectString=NULL;
			TBuf8<KCommsDbSvrMaxFieldLength> escapeChar;
			TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameEscapeCharacter),escapeChar);
			if (ret)
				{
				Complete(ret,aSource);
				break;
				}	
			iTxBuffer.Format(_L8("%S%S%S"),&escapeChar,&escapeChar,&escapeChar);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KTimeOut*1000);
			iState=EATEscapeSeqWaitForWriteComplete;
			}
		break;
		
	case EATEscapeSeqWaitForWriteComplete:
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		StandardWriteCompletionHandler(aSource,KTimeOut);
		iState=EATEscapeSeqCompleted;
		break;

	case EATEscapeSeqCompleted:
		{
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		if (aSource!=ETimeOutCompletion)
			{
			TRAPD(ret,ValidateHangUpExpectStringL());
			if (iIo->FoundChatString()==iNoCarrierExpectString || ret!=KErrNone)
				{
				Complete(ret,aSource); 
				break;
				}
			}
		RemoveStdExpectStrings();
		iIo->RemoveExpectString(iNoCarrierExpectString);// poss. problem as it removes CATWaitForNoCarrier's string too
		iNoCarrierExpectString=NULL;
		Write(KHangUpCommand(),KTimeOut);
		iState=EATHangupWaitForWriteComplete;
		}
		break;

	case EATHangupWaitForWriteComplete:
		StandardWriteCompletionHandler(aSource,KTimeOut);
		if (!iNoCarrierExpectString)
			iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
		iState=EATHangupReadCompleted;
		break;

	case EATHangupReadCompleted:
		__ASSERT_ALWAYS(aSource!=EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteNotExpected));
		if (aSource!=ETimeOutCompletion)
			{
			TRAPD(ret,ValidateHangUpExpectStringL());
			Complete(ret,aSource);	
			}
		else
			{
			Complete(KErrTimedOut,aSource);
			}
		break;

	case EHangUpCancelling:
		if (aSource==EWriteCompletion)
			{
			iIo->SetTimeOut(this,KOneSecondPause);
			}
		if (aSource==EReadCompletion || aSource==ETimeOutCompletion)
			{
			LOGTEXT(_L8("Hang up cancelled"));
			iIo->WriteAndTimerCancel(this);
			iIo->RemoveExpectStrings(this);
			iOKExpectString=NULL;
			iErrorExpectString=NULL;
			REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
			iTelObject->ReqCompleted(iReqHandle,KErrCancel);
			}
		break;
				
	default:
		;
		}
	}

void CATHangUpData::Complete(TInt aError,TEventSource aSource)
	{
	RemoveStdExpectStrings();
	iIo->RemoveExpectStrings(this);
	iNoCarrierExpectString=NULL;
	REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdle();
	iIo->Cancel();
	TCommConfig aConfigPckg;
	aError = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeInit);
	if (aError==KErrNone)
		aError = iIo->ConfigurePort(aConfigPckg);
	if (aSource==EWriteCompletion)
		iIo->Read();
	CATCommands::Complete(aError,aSource);
	if (iCallInfo->iClientPanicOccurred==ENoPanicOccurred)
		{
		iTelObject->ReqCompleted(iReqHandle, aError);
		}
	else
		{
		iComplete = CCompleteRelinquish::New(iTelObject);
		iComplete->SetWhichCompletion(iCallInfo->iClientPanicOccurred);
		iComplete->Call();	// calls the AysncOneShot Relinquish completion function
		iCallInfo->iClientPanicOccurred = ENoPanicOccurred;
		}	
	iState=EATNotInProgress;
	}
