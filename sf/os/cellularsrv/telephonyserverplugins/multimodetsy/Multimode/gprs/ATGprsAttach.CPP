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

#include "gprs.h"
#include "Gprscontext.h"
#include "mSLOGGER.H"
#include <pcktcs.h>
#include "ATGprsAttach.H"
#include "ATIO.H"
#include <etelpckt.h>
#include "TSYCONFG.H"
#include "NOTIFY.H"
#include "Matstd.h"

/** 
 * @file
 * This file implements the CATGprsAttach class which is one of the state machine used by the
 * GPRS AT TSY library.
 */

/**
 * This state machine uses the "AT+CGATT" command to connect to the GPRS network.
 */
_LIT8(KAttachCommand, "AT+CGATT=1\r");

CATGprsAttach* CATGprsAttach::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals )
/**
 *  Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsAttach* p =new(ELeave) CATGprsAttach(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();

	return p;
	}

void CATGprsAttach::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}

CATGprsAttach::CATGprsAttach(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
	:CATCommands(aIo, aTelObject, aInit, aPhoneGlobals)
/**
 * Constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	LOGTEXT(_L8("CATGprsAttach::CATGprsAttach called"));
	}


CATGprsAttach::~CATGprsAttach()
/**
 * Destructor.
 */
	{
	LOGTEXT(_L8("CATGprsAttach::~CATGprsAttach called"));
	iIo->RemoveExpectStrings(this);				
	}


void CATGprsAttach::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aParams*/)
/**
 * This is the standard entry point for sending attach command.
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGprsAttach::Start called"));
	iReqHandle = aTsyReqHandle;
	iState=ESendAttachCommand;
	Write(KAttachCommand, KGprsCommandTimeOut);	
	}


void CATGprsAttach::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This funciton stops this AT state machine
 */
	{
	LOGTEXT(_L8("CATGprsAttach::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsAttach::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);		// Pretend we have had a timeout
		}
	}


void CATGprsAttach::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client with an error.
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	LOGTEXT(_L8("CATGprsAttach::CompleteWithIOError called"));
	Complete(aStatus, aSource);
	}


void CATGprsAttach::Complete(TInt aError,TEventSource aSource)
/**
 * This Function completes the command from the client.
 * @param aError an error code to relay to client.
 */
	{
	LOGTEXT2(_L8("CATGprsAttach::Complete called error: %d"), aError);
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);	
	iIo->RemoveExpectStrings(this);
	if (aError==KErrNone)
		{
		((CGprs*)iTelObject)->SetStatus(RPacketService::EStatusAttached);
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketStatusChanged);
		}
	
	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aError,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aError);

	iState = EATNotInProgress;
	}


void CATGprsAttach::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 * @par ESendAttachCommand
 * Reads the Modem's response to the AT+CGATT=1 command and completes the request.
 */
	{
	LOGTEXT2(_L8("CATGprsAttach::EventSignal called with iState=%d"),iState);

	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGprsAttach::EventSignal, Timeout"));
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case ESendAttachCommand:
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			LOGTEXT(_L8("CATGprsAttach::EventSignal, ESendConnectCommand"));
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			StandardWriteCompletionHandler(aSource, KGprsLongCommandTimeOut);
			iState = EAttachReadComplete;
			}
			break;
		case EAttachReadComplete:
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			LOGTEXT(_L8("CATGprsAttach::EventSignal, EConnectReadComplete"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret = ValidateExpectString();
			Complete(ret, aSource);
			}
			break;
		case EATNotInProgress:
			break;
		default:
			{
			LOGTEXT(_L8("CATGprsAttach::EventSignal, Default, panic"));
			Panic(EIllegalEvent);
			}
		}		
	}
