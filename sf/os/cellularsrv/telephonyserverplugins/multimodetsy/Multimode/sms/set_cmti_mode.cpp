// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Sets the phoen to CMTI mode using AT+CNMI=?,1
// 
//

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSLOGGER.H"
#include "ATIO.H"

#include "set_cmti_mode.h"		// Header for this file

//
// Macros
//
// This macro is used to help keep un-intereting details outside of the main code
// in this file. The macro uses logging functions and always prefixes writes to the 
// log with the name of the class which is implemented in this file. This macro must
// only be used by code in this file.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSetPhoneToCMTIMode::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


CATSetPhoneToCMTIMode* CATSetPhoneToCMTIMode::NewL (CATIO* aIo, CTelObject* aTelObject,
												CATInit* aInit, CPhoneGlobals* aGlobals,
												RMobileSmsMessaging::TMobileSmsReceiveMode& aCurrentRxModeVariable)
	{
	CATSetPhoneToCMTIMode* self = new (ELeave) CATSetPhoneToCMTIMode(aIo, aTelObject, aInit, aGlobals, aCurrentRxModeVariable);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSetPhoneToCMTIMode::CATSetPhoneToCMTIMode(CATIO* aIo, CTelObject* aTelObject,
										 CATInit* aInit, CPhoneGlobals* aGlobals,
										 RMobileSmsMessaging::TMobileSmsReceiveMode& aCurrentRxModeVariable)
	:CATSmsCommands(aIo, aTelObject, aInit, aGlobals),
	 iClientsCurrentRxMode(aCurrentRxModeVariable)
	{}

void CATSetPhoneToCMTIMode::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aParam*/)
	{
	iReqHandle = aTsyReqHandle;

	TInt ret(KErrNotSupported);

	//
	// Only request our base class set the phone to CMTI mode for us
	// if the mode is supoprted by the phone
	if(iPhoneGlobals->iModemSupportsCMTIMode)
		ret=CATSmsCommands::RequestATCommand(ESetPhoneToCMTIMode);
	
	if(ret!=KErrNone)
		Complete(ret);
	}


void CATSetPhoneToCMTIMode::EventSignal(TEventSource aSource)
	{
	LOCAL_LOGTEXT("EventSignal","");

	//
	// Allow base class to handle events for us
	LOCAL_LOGTEXT("EventSignal","Giving base class a chance to handle the event");
	CATSmsCommands::EventSignal(aSource);

	//
	// Check if base class has completed our request
	if(CATSmsCommands::RequestATCompleted()==ESetPhoneToCMTIMode)
		{
		LOCAL_LOGTEXT("EventSignal","Base class has completed request to set phone to CMTI mode");
		LOGTEXT2(_L8("completion code: %d"),iRequestError);

		//
		// If we have completed with no error then update our clients
		// current rx mode
		if(iRequestError==KErrNone)
			iClientsCurrentRxMode=RMobileSmsMessaging::EReceiveStored;

		Complete(iRequestError);
		}
	}


void CATSetPhoneToCMTIMode::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	LOCAL_LOGTEXT("Stop","Cancelling command");
	
	CATSmsCommands::RequestATCommandCancel();
	}


void CATSetPhoneToCMTIMode::Complete(TInt aError)
	{
	Complete(aError,EReadCompletion);
	}

void CATSetPhoneToCMTIMode::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");
	LOGTEXT2(_L8("Error Code : %d"), aError);

	CATCommands::Complete(aError,aSource);
	if (iReqHandle != 0)
		iTelObject->ReqCompleted(iReqHandle, aError);
	}

void CATSetPhoneToCMTIMode::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	iIo->WriteAndTimerCancel(this);
	if (iReqHandle != 0)
		iTelObject->ReqCompleted(iReqHandle, aStatus);
	}
