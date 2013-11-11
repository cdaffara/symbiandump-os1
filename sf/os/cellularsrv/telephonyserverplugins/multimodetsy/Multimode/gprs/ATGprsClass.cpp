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
#include "AtGprsClass.h"
#include "ATIO.H"
#include <etelpckt.h>
#include "TSYCONFG.H"
#include "NOTIFY.H"
#include "Matstd.h"


/**
 * @file
 * This file implements the CATGprsClass class which is one of the state machine used by the
 * GPRS AT TSY library. 
 * This state machine uses the "AT+CGCLASS" command.
 */

CATGprsClass* CATGprsClass::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
/**
 *  Standard 2 phase constructor.
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsClass* p=new(ELeave) CATGprsClass(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();
	return p;
	}

void CATGprsClass::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}

CATGprsClass::CATGprsClass(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals)
/**
 * Constructor.
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aInit pointer to AT phone init object.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	LOGTEXT(_L8("CATGprsClass::CATGprsClass called"));
	}


CATGprsClass::~CATGprsClass()
/**
 * Destructor.
 */
	{
	LOGTEXT(_L8("CATGprsClass::~CATGprsClass called"));
	iIo->RemoveExpectStrings(this);				
	}

void CATGprsClass::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
/**
 * This is the standard entry point for sending class command.
 */
	{
	iClass = REINTERPRET_CAST(RPacketService::TMSClass*, aParams);
	if(MakeupCGCLASS(*iClass) == KErrNotSupported)
		{
		iTelObject->ReqCompleted(aTsyReqHandle, KErrArgument);
		return;
		}
	iReqHandle = aTsyReqHandle;
	iState=ESendClassCommand;
	Write(KGprsCommandTimeOut);	
	}

void CATGprsClass::Stop(TTsyReqHandle aTsyReqHandle)
/**
 */
	{
	LOGTEXT(_L8("CATGprsClass::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsClass::Stop Cancelling client request"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}

void CATGprsClass::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client with an error.
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	LOGTEXT(_L8("CATGprsClass::CompleteWithIOError called"));
	Complete(aStatus, aSource);
	}


void CATGprsClass::Complete(TInt aError,TEventSource aSource)
/**
 * This Function completes the command from the client.
 *
 * @param aError an error code to relay to client.
 */
	{
	LOGTEXT2(_L8("CATGprsClass::Complete called error: %d"), aError);
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);	
	iIo->RemoveExpectStrings(this);
	if (aError==KErrNone)
		{
		((CGprs*)iTelObject)->SetCurrentMSClass(*iClass);
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aError,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aError);

	iState=EATNotInProgress;
	}


void CATGprsClass::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 * 
 * @par ESendClassCommand
 * Wait for response to the AT+CGCLASS=%S command.
 *
 * @par EClassReadComplete
 * Reads the Modem's response to the AT+CGCLASS=%S command and completes the request.
 *
 */
	{
	if ((aSource==ETimeOutCompletion) )
		{
		LOGTEXT(_L8("CATGprsClass::EventSignal, Timeout"));
		Complete(KErrTimedOut,aSource);
		return;
		}
	switch(iState)
		{
		case ESendClassCommand:
				{
				LOGTEXT(_L8("CATGprsClass::EventSignal, ESendConnectCommand"));
				__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
				StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
				iState = EClassReadComplete;
				}
			break;
		case EClassReadComplete:
				{
				LOGTEXT(_L8("CATGprsClass::EventSignal, EConnectReadComplete"));
				__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
				TInt ret = ValidateExpectString();
				Complete(ret, aSource);
				}
			break;
		case EATNotInProgress:
			break;
		default:
				{
				LOGTEXT(_L8("CATGprsClass::EventSignal, Default, panic"));
				Panic(EIllegalEvent);
				}
		}		
	}

TInt CATGprsClass::MakeupCGCLASS(RPacketService::TMSClass aClass)
/**
 * This function creates the AT+CGCLASS string to send to the phone.
 * The main task of this function is to populate iTxBuffer.
 * @param aClass MSClass to set.
 */
	{
	_LIT8(KCGClass, "AT+CGCLASS=%S\r");
	switch(aClass)
		{
		case RPacketService::EMSClassDualMode:
			iTxBuffer.Format(KCGClass, &KMSClassA);
			break;
		case RPacketService::EMSClassSuspensionRequired:
			iTxBuffer.Format(KCGClass, &KMSClassB);
			break;
		case RPacketService::EMSClassAlternateMode:
			iTxBuffer.Format(KCGClass, &KMSClassCAlternateMode);
			break;
		case RPacketService::EMSClassPacketSwitchedOnly:
			iTxBuffer.Format(KCGClass, &KMSClassCGPRSOnly);
			break;
		case RPacketService::EMSClassCircuitSwitchedOnly:
			iTxBuffer.Format(KCGClass, &KMSClassCCircuitSwitchedOnly);
			break;
		default:
			return KErrNotSupported;
		}
	return KErrNone;
}
