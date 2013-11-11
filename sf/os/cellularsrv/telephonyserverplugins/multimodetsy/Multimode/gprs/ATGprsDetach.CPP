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

#include "Gprscontext.h"
#include "mSLOGGER.H"
#include "ATGprsDetach.H"
#include "ATIO.H"
#include "TSYCONFG.H"
#include "NOTIFY.H"
#include "Matstd.h"

_LIT8(KDetachCommand, "AT+CGATT=0\r");

/**
 * @file
 * This file implements the CATGprsDetachclass which is one of the state machine used by the
 * GPRS AT TSY library. 
 * This state machine uses the "AT+CGATT" command.
 */

CATGprsDetach* CATGprsDetach::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals )
/**
 *  Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsDetach* p =new(ELeave) CATGprsDetach(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGprsDetach::ConstructL()
/**
 * Construct all objects that can leave.
 */
{
	CATCommands::ConstructL();
}



CATGprsDetach::CATGprsDetach(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
											: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals)
/**
 * Constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{}


CATGprsDetach::~CATGprsDetach()
/**
 * Destructor.
 */
	{
	iIo->RemoveExpectStrings(this);
	}


void CATGprsDetach::Start(TTsyReqHandle aTsyReqHandle, TAny* )
/**
 * This is the standard entry point for sending detach command.
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGprsDetach:\tStarting at AT+CGATT=0 Command"));
	iReqHandle = aTsyReqHandle;
	Write(KDetachCommand, KGprsCommandTimeOut);	
	iState=ESendDetachCommand;
	}


void CATGprsDetach::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * This function attempts to cancel the AT state machine
 */
	{
	LOGTEXT(_L8("CATGprsDetach::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsDetach::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}


void CATGprsDetach::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client whith an error.
 *
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	LOGTEXT2(_L8("CATGprsDetach::CompleteWithIOError() called with iState = %d"),iState);
	Complete(aStatus, aSource);
	}


void CATGprsDetach::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 * 
 * @par EDetachReadComplete
 * wait for the response from the phone.
 *
 * @par ESendDetachCommand
 * Parse the response from the phone.
 */
	{
	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGprsDetach::EventSignal, Timeout"));
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case ESendDetachCommand:
			{
			LOGTEXT(_L8("CATGprsDetach::EventSignal, ESendConnectCommand"));
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			StandardWriteCompletionHandler(aSource, KGprsLongCommandTimeOut);				
			iState = EDetachReadComplete;
			}
			break;
		case EDetachReadComplete:
			{
			LOGTEXT(_L8("CATGprsDetach::EventSignal, EConnectReadComplete"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret = ValidateExpectString();
			Complete(ret, aSource);
			}
			break;
		case EATNotInProgress:
			break;
		default:
			Panic(EIllegalEvent);
		}		
	}


void CATGprsDetach::Complete(TInt aError,TEventSource aSource)
/**
 * This Function completes the command from the client.
 * It also changes the status of all contexts.
 * @param aError an error code to relay to client.
 */
	{
	LOGTEXT2(_L8("CATGprsDetach::Complete() with aError %d"),aError);
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	if (aError==KErrNone)
		{
		TCommConfig aConfigPckg;
		aError = iPhoneGlobals->iConfiguration->PortConfig(aConfigPckg,EConfigTypeInit);
		if (aError==KErrNone)
			aError = iIo->ConfigurePort(aConfigPckg);
		
		iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeOnlineCommand;
		RPacketService::TContextInfo contextInfo;
		((CGprs*)iTelObject)->SetStatus(RPacketService::EStatusUnattached);
		TInt i = 0;	
		// since the connection to the network is broken change the state of the contexts also.
		CGprsContext* context; 
		while(context = ((CGprs*)iTelObject)->Context(i), context != NULL)
			{
			context->ContextInfo(&contextInfo);
			contextInfo.iStatus = RPacketContext::EStatusInactive;
			context->SetContextInfo(&contextInfo);
			iPhoneGlobals->iNotificationStore->CheckNotification(context, EPacketContextStatusChanged);
			i++;
			}
		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketStatusChanged);
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aError,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aError);

	iState=EATNotInProgress;
	}

