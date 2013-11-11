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
// GSMSSETSC.CPP
// Sets the default ServiceCentreAddress (SCA) stored in the ME (Mobile Equipment). AT+CSCA=xxxxxxx
// 
//

#include "NOTIFY.H"
#include "mSMSMESS.H"
#include "mSSETSC.H"
#include "mSLOGGER.H"
#include "ATIO.H"

//
// Macros
//
// This macro is used to help keep un-intereting details outside of the main code
// in this file. The macro uses logging functions and always prefixes writes to the 
// log with the name of the class which is implemented in this file. This macro must
// only be used by code in this file.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATSmsSetSCAddress::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif




CATSmsSetSCAddress* CATSmsSetSCAddress::NewL (CATIO* aIo, CTelObject* aTelObject,
												CATInit* aInit, CPhoneGlobals* aGlobals)
	{
	CATSmsSetSCAddress* self = new (ELeave) CATSmsSetSCAddress(aIo, aTelObject, aInit, aGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsSetSCAddress::CATSmsSetSCAddress(CATIO* aIo, CTelObject* aTelObject,
										 CATInit* aInit, CPhoneGlobals* aGlobals)
	:CATSmsCommands(aIo, aTelObject, aInit, aGlobals)
	{}

void CATSmsSetSCAddress::Start(TTsyReqHandle aTsyReqHandle, TAny* aParam)
	{
	iReqHandle = aTsyReqHandle;

	//
	// Pass the SCA that we want to set to our base class
	CATSmsCommands::iRequestSCA=*((RMobilePhone::TMobileAddress*)aParam);

	//
	// Request our base class set the SCA in the phone for us
	TInt ret=CATSmsCommands::RequestATCommand(ESetSCAInPhone);
	if(ret!=KErrNone)
		Complete(ret);
	}


void CATSmsSetSCAddress::EventSignal(TEventSource aSource)
	{
	LOCAL_LOGTEXT("EventSignal","");

	//
	// Allow base class to handle events for us
	LOCAL_LOGTEXT("EventSignal","Giving base class a chance to handle the event");
	CATSmsCommands::EventSignal(aSource);

	//
	// Check if base class has completed our request
	if(CATSmsCommands::RequestATCompleted()==ESetSCAInPhone)
		{
		LOCAL_LOGTEXT("EventSignal","Base class has completed request to set SCA");
		LOGTEXT2(_L8("completion code: %d"),iRequestError);
		Complete(iRequestError);
		}
	}


void CATSmsSetSCAddress::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Attempts to halt the process
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));		
	LOCAL_LOGTEXT("Stop","Cancelling command");
	
	CATSmsCommands::RequestATCommandCancel();
	}


void CATSmsSetSCAddress::Complete(TInt aError)
	{
	Complete(aError,EReadCompletion);
	}

void CATSmsSetSCAddress::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");
	LOGTEXT2(_L8("Error Code : %d"), aError);

	CATCommands::Complete(aError,aSource);
	if (iReqHandle != 0)
		iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource==EWriteCompletion)
		iIo->Read();
	}

void CATSmsSetSCAddress::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	iIo->WriteAndTimerCancel(this);
	iTelObject->ReqCompleted(iReqHandle, aStatus);
	}
