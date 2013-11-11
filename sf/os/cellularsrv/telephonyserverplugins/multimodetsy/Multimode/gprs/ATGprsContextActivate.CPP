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
#include "ATGprsContextActivate.H"
#include "ATIO.H"
#include <etelpckt.h>
#include "TSYCONFG.H"
#include "NOTIFY.H"
#include "Matstd.h"

_LIT8(KATAttachEricsson, "ATD*99***1#\r");
_LIT8(KATAttachNormal,   "ATD*99#\r");

/**
 * @file
 * This file implements the CATGprsContextActivate class which is one of the state machine used by the
 * GPRS AT TSY library. 
 * This state machine uses the "ATD*99***%d#" and the "ATD*99#" command.
 */

CATGprsContextActivate* CATGprsContextActivate::NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals )
/**
 *  Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsContextActivate* p =new(ELeave) CATGprsContextActivate(aCid, aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();

	return p;
	}

void CATGprsContextActivate::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}

CATGprsContextActivate::CATGprsContextActivate(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals), iCid(aCid)
/**
 * Constructor.
 *
 * @param aCid context number.
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	LOGTEXT(_L8("CATGprsContextActivate::CATGprsContextActivate called"));
	}


CATGprsContextActivate::~CATGprsContextActivate()
/**
 * Destructor.
 */
	{
	LOGTEXT(_L8("CATGprsContextActivate::~CATGprsContextActivate called"));
	iIo->RemoveExpectStrings(this);				
	}


void CATGprsContextActivate::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aParams*/)
/**
 * This is the standard entry point for sending attach command.
 *
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGprsContextActivate::Start called"));
	iReqHandle = aTsyReqHandle;
	iTxBuffer.Format(KATAttachEricsson, iCid); 
	Write(KGprsCommandTimeOut);
	iState=ESendEricssonConnectCommand;
	}


void CATGprsContextActivate::Stop(TTsyReqHandle aTsyReqHandle)
/**
 */
	{
	LOGTEXT(_L8("CATGprsContextActivate::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsContextActivate::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}


void CATGprsContextActivate::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client whith an error.
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	LOGTEXT(_L8("CATGprsContextActivate::CompleteWithIOError called"));
	Complete(aStatus, aSource);
	}

void CATGprsContextActivate::Complete(TInt aError,TEventSource aSource)
/**
 * This Function completes the command from the client.
 * @param aError an error code to relay to client.
 */
	{
	LOGTEXT2(_L8("CATGprsContextActivate::Complete called error: %d"), aError);
	RemoveStdExpectStrings();
	iIo->WriteAndTimerCancel(this);	
	iIo->RemoveExpectStrings(this);
	iExpectString=NULL;
	iNoCarrierString=NULL;
	if (aError==KErrNone)
		{
		// Set context information
		iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeOnlineData;
		RPacketService::TContextInfo contextInfo; 
		((CGprsContext*)iTelObject)->ContextInfo(&contextInfo );
		contextInfo.iStatus = RPacketContext::EStatusActive;
		((CGprsContext*)iTelObject)->SetContextInfo(&contextInfo);
		// Set Gprs state		
		((CGprsContext*)iTelObject)->Parent()->SetStatus(RPacketService::EStatusActive);
		// Check notifications.
				
		// Note that EPacketStatusChanged is a notification of the CGprs class.
		// Our iTelObject is the CGprsContext class, so we have to use iTelObject->Owner().
		iPhoneGlobals->iNotificationStore->CheckNotification(STATIC_CAST(CTelObject*,iTelObject->Owner()), EPacketStatusChanged);

		iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketContextStatusChanged);
		}
	
	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aError,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aError);

	iState = EATNotInProgress;
	}


void CATGprsContextActivate::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 *
 * @par ESendEricssonConnectCommand
 *
 * @par EConnectEricssonReadComplete
 * 
 * @par ESendConnectCommand
 * Wait for response to the attach command.
 *
 * @par EConnectReadComplete
 * Reads the Modem's response to the attach command and completes the request.
 *
 */
	{
	if ((aSource==ETimeOutCompletion))
		{
		LOGTEXT(_L8("CATGprsContextActivate::EventSignal, Timeout"));
		Complete(KErrTimedOut,aSource);
		return;
		}
	
	switch(iState)
		{
		case ESendEricssonConnectCommand:
			{
			LOGTEXT(_L8("CATGprsContextActivate::EventSignal, ESendConnectCommand"));
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			StandardWriteCompletionHandler(aSource, KGprsCommandTimeOut);
			if(!iExpectString)
				iExpectString = iIo->AddExpectString(this, KCarrierString);
			if(!iNoCarrierString)
				iNoCarrierString = iIo->AddExpectString(this, KNoCarrierString);
			iState = EConnectEricssonReadComplete;
			}
			break;
			
		case EConnectEricssonReadComplete:
			{
			LOGTEXT(_L8("CATGprsContextActivate::EventSignal, EConnectReadComplete"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret=ValidateExpectString();
			RemoveStdExpectStrings();
			if(ret == KErrArgument)
				{
				iTxBuffer.Format(KATAttachNormal); 
				Write(KGprsLongCommandTimeOut);	
				iState=ESendNormalConnectCommand;
				break;
				}
			Complete(ret, aSource);
			}
			break;


		case ESendNormalConnectCommand:
			{
			LOGTEXT(_L8("CATGprsContextActivate::EventSignal, ESendConnectCommand"));
			__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			StandardWriteCompletionHandler(aSource, KGprsLongCommandTimeOut);
			if(!iExpectString)
				iExpectString = iIo->AddExpectString(this, KCarrierString);
			if(!iNoCarrierString)
				iNoCarrierString = iIo->AddExpectString(this, KNoCarrierString);
			iState = EConnectNormalReadComplete;
			}
			break;
			
		case EConnectNormalReadComplete:
			{
			LOGTEXT(_L8("CATGprsContextActivate::EventSignal, EConnectReadComplete"));
			__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			TInt ret=ValidateExpectString();
			// Changed in accordance with Defect Fix BEN-5AVC39 to return a more meaningfull Error code
			ret = ret == KErrArgument ? KErrNotFound : ret;
			Complete(ret, aSource);
			}
			break;
		case EATNotInProgress:
			break;
		default:
			Panic(EIllegalEvent);
		}		
	}


TInt CATGprsContextActivate::ValidateExpectString()
/**
 * This Function validates the response from the phone.
 */
	{
	LOGTEXT(_L8("CATGprsContextActivate::ValidateExpectString called"));
	if(iIo->FoundChatString()==iExpectString)
		{
		LOGTEXT(_L8("CATGprsContextActivate:\tPhone returned CONNECT"));
		iIo->RemoveExpectString(iExpectString);
		return KErrNone;
		}
	else if(iIo->FoundChatString() == iNoCarrierString)
		{
		iIo->RemoveExpectString(iNoCarrierString);
		return KErrNotFound;
		}
	else if( iIo->FoundChatString() == iErrorExpectString)
		{
		return KErrArgument;
		}
	return KErrUnknown;
	}

