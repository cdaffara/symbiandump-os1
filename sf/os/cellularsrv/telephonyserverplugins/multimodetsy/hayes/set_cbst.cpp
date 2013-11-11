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

#include <etelmm.h>					// MultiMode Etel API
#include "set_cbst.h"				// Header file for this file
#include "mSLOGGER.H"				// For LOGTEXT macros
#include "ATIO.H"					// For CATIO class
#include "Matstd.h"					// For KCBSTResponseString
#include "CALL.H"					// For CCallHayes

//
// Macro for logging
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSetCBST::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATSetCBST* CATSetCBST::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATSetCBST* self=new(ELeave) CATSetCBST(aIo, aTelObject, aInit,aPhoneGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSetCBST::CATSetCBST(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}


void CATSetCBST::Stop(TTsyReqHandle)
	{
	// The CATSetCBST AT command can not be cancelled
	}


void CATSetCBST::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iState = EATNotInProgress;
		iIo->WriteAndTimerCancel(this);
		REINTERPRET_CAST(CCallHayes*,iTelObject)->SetToIdleAndCompleteReq(iReqHandle,aStatus);
		}
	}

void CATSetCBST::Start(TTsyReqHandle aTsyReqHandle, TAny* aNextCmdToExecute)
	{
	LOCAL_LOGTEXT("Start","Enter function");

	iReqHandle = aTsyReqHandle;
	iNextCmdToExecute=(CATCommands*)aNextCmdToExecute;

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
		iState=EWaitForWriteComplete;
		}
	else
		{
		// We have not sent a +CBST= to the phone, so complete now
		LOCAL_LOGTEXT("Start","Not enough parameters to send +CBST= command");
		Complete(KErrNone,EReadCompletion);	// This is not a read completeion, but pretend it is
		}
	}


void CATSetCBST::EventSignal(TEventSource aSource)
	{
	LOGTEXT2(_L8("CATSetCBST::EventSignal with iState %d"),iState);
	if (aSource==ETimeOutCompletion && iState!=EATNotInProgress)
		{
		LOCAL_LOGTEXT("EventSignal","Timeout event");
		iIo->RemoveExpectStrings(this);
		iState = EATNotInProgress;
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
	case EWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource, 5);
		iState=EWaitForReadComplete;
		break;

	case EWaitForReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->WriteAndTimerCancel(this);
		// There is no need to validate the response from the modem
		// as we are going to ignore it anyway.
		Complete(KErrNone,aSource);
		break;
	
	default:		// Required to avoid warnings in ARMI UREL build
		break;
		}		
	}


void CATSetCBST::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");

	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);

	// If we have been asked to kick off another AT state machine,
	// then do it here. Otherwise complete the client request as normal.
	if(iNextCmdToExecute)
		{
		LOCAL_LOGTEXT("Complete","Starting next AT command");

		// We have to change the iEventSignalActive manually as we are not calling
		// CATCommands::Complete. It is safe to do this as we know we have fisnihed 
		// our processing.
		iPhoneGlobals->iEventSignalActive = EFalse;

		iNextCmdToExecute->ExecuteCommand(iReqHandle,NULL,iCallInfo);
		}
	else
		{
		LOCAL_LOGTEXT("Complete","No need to start antoher AT command");
		CATCommands::Complete(aError,aSource);
		// This state machine should never complete a client request.
		// There is no 'send CBST to phone' client request, this state machine
		// is used as part of processing other client requests (eg. AnswerIncomingCall).
		}

	iState = EATNotInProgress;
	}
