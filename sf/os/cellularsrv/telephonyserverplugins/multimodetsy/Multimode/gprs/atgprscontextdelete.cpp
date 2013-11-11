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
//

#include <et_phone.h>			 // for CTelObject class
#include "Gprscontext.h"		// for CGprsContext
#include "atgprscontextdelete.h" // header file for this source file
#include "mSLOGGER.H"			 // for LOGTEXT macros
#include "ATIO.H"				 // for CATIO
#include "Matstd.h"			     // for AT command strings, and other stuff
#include "NOTIFY.H"				 // for CNotifications

const TUint KWriteTimeout=10000;		// Is in milli seconds
const TInt KReadTimeout=10;		// Is in seconds

//
// Macro for logging text to the log file using the local class name and a simpler 
// call style.
#ifdef __LOGDEB__
_LIT8(KLogEntry,"CATGprsContextDelete::%S\t%S");
#define LOCAL_LOGTEXT(function,text) {_LIT8(F,function);_LIT8(T,text);LOGTEXT3(KLogEntry,&F,&T);}
#else
#define LOCAL_LOGTEXT(function,text)
#endif


/**
 * @file
 * AT state machine which will send AT+CGDCONT=x to delete a context from the phone.
 */

CATGprsContextDelete* CATGprsContextDelete::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals )
/**
 *  Factory function.
 */
	{
	CATGprsContextDelete* p =new(ELeave) CATGprsContextDelete(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();			// We call this to allow our base class to construct
	CleanupStack::Pop();
	return p;
	}


CATGprsContextDelete::CATGprsContextDelete(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
											: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals) 
/**
 * C++ constructor.
 */
	{
	}

CATGprsContextDelete::~CATGprsContextDelete()
/**
 * C++ Destructor.
 */
	{
	iIo->RemoveExpectStrings(this);
	}

void CATGprsContextDelete::Stop(TTsyReqHandle /*aTsyReqHandle*/)
/**
 * This function called by CATCommands to cancel an AT command state machine.
 * This AT command state machine can not be cancelled.
 */
	{
	LOCAL_LOGTEXT("Stop","Enter function");
	LOCAL_LOGTEXT("Stop","This functionality is not supported");
	}


void CATGprsContextDelete::Start(TTsyReqHandle aTsyReqHandle, TAny* aDummy)
/**
 * Starting of the state machine.
 *
 * This will be called by CATCommands::ExecuteCommand to start the sending
 * of the AT commands.
 */
	{
	LOCAL_LOGTEXT("Start","Enter function");
	const TInt index=*(static_cast<TInt*>(aDummy));
	LOGTEXT2(_L8("index=%d"),index);

	iReqHandle=aTsyReqHandle;

	//
	// Send the AT+CGDCONT=x command to the phone
	iTxBuffer.Format(KDeleteContextCommand,index);
	iIo->Write(this,iTxBuffer);
	iIo->SetTimeOut(this,KWriteTimeout);
	iState=EWaitForWrite;
	}


void CATGprsContextDelete::EventSignal(TEventSource aSource)
/**
 * Main part of the state machine
 */
	{
	LOCAL_LOGTEXT("EventSignal","Enter function");

	if (aSource==ETimeOutCompletion)
		{
		LOCAL_LOGTEXT("EventSignal","Timeout error");
		Complete(KErrTimedOut,aSource); 
		return;
		}

	switch(iState)
		{
	case EWaitForWrite:
		__ASSERT_DEBUG(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource,KReadTimeout);
		iState=EWaitForRead;
		break;

	case EWaitForRead:
		__ASSERT_DEBUG(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->WriteAndTimerCancel(this);
		// There is no need to validate the response from the modem
		// as we are going to ignore it anyway.
		Complete(KErrNone,aSource);
		break;

		case EIdle:	
			break;
		}
	}


void CATGprsContextDelete::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client with an error.
 */
	{
	Complete(aStatus,aSource);
	}


void CATGprsContextDelete::Complete(TInt aError,TEventSource aSource)
	{
	LOCAL_LOGTEXT("Complete","Enter function");

	if(aError==KErrNone)
		{
		//
		// Mark the context as deleted in the TSY
		CGprsContext* contextPtr=static_cast<CGprsContext*>(iTelObject);
		RPacketService::TContextInfo contextInfo;
		contextPtr->ContextInfo(&contextInfo);
		contextInfo.iStatus = RPacketContext::EStatusDeleted;
		contextPtr->SetContextInfo(&contextInfo);
		
		//
		// Update the notifications 
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketContextStatusChanged);
		}

	//
	// Cleanup the CATIO stuff
	RemoveStdExpectStrings();		// We have to call this to ensure iOKExpectString & iErrorExpectString are NULLed
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	
	//
	// Allow base class chance to start of AT commands and complete our client request
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	
	iState = EIdle;
	}
 



