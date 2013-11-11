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
// ATGPRSCONTEXTDEATIVATE.CPP
// 
//


#include "gprs.h"
#include "Gprscontext.h"
#include "mSLOGGER.H"
#include <pcktcs.h>
#include "ATGprsContextDeactivate.H"
#include "ATIO.H"
#include <etelpckt.h>
#include "TSYCONFG.H"
#include "NOTIFY.H"
#include <cdblen.h>
#include "Matstd.h"

#include <commsdattypesv1_1.h>

const TInt KDTRLowPeriod=500;	// milliseconds
const TInt KDTRHighSettle=200;	// milliseconds

_LIT8( KEscape, "%S%S%S");


/**
 * @file
  * This file implements the CATGprsContextDeactivate class which is one of the state machine used by the
 * GPRS AT TSY library. 
 * This state machine uses "ATH" "+++" "ATH" commands.
 */

CATGprsContextDeactivate* CATGprsContextDeactivate::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals )
/**
 *  Standard 2 phase constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	CATGprsContextDeactivate* p =new(ELeave) CATGprsContextDeactivate(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(p);
	p->ConstructL();
	CleanupStack::Pop();

	return p;
	}

void CATGprsContextDeactivate::ConstructL()
/**
 * Construct all objects that can leave.
 */
	{
	CATCommands::ConstructL();
	}

CATGprsContextDeactivate::CATGprsContextDeactivate(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
											: CATCommands(aIo, aTelObject, aInit, aPhoneGlobals) 
/**
 * Constructor.
 *
 * @param aIo pointer to communication object.
 * @param aTelObject pointer to parent.
 * @param aPhoneGlobals pointer to phone global wide states.
 */
	{
	LOGTEXT(_L8("CATGprsContextDeactivate::CATGprsContextDeactivate"));
	}


CATGprsContextDeactivate::~CATGprsContextDeactivate()
/**
 * Destructor.
 */

	{
	LOGTEXT(_L8("CATGprsContextDeactivate::~CATGprsContextDeactivate"));
	iIo->RemoveExpectStrings(this);
	}


void CATGprsContextDeactivate::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aDummy*/)
/**
 * This is the standard entry point for sending deactivate command.
 *
 * @param aTsyReqHandle handle to the client.
 */

	{
	LOGTEXT(_L8("CATGprsContextDeactivate:\tStarting GPRS detach Command"));
	iReqHandle=aTsyReqHandle;
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString) != NULL,Panic(EGeneral));
	iIo->Cancel();		// This required to avoid a panic in C32 when we call iIo->ConfigurePort
	TCommConfig configPckg;
	TInt ret = iPhoneGlobals->iConfiguration->PortConfig(configPckg,EConfigTypeHangUp);	
	if (ret==KErrNone)
		ret = iIo->ConfigurePort(configPckg);		
	if (ret!=KErrNone)
		{
		Complete(ret,EReadCompletion); // EReadCompletion so another Read will not be queued
		}
	else
		{
		iIo->DropDtr();		
		iIo->SetTimeOut(this,KDTRLowPeriod);
		iState=EDTRDropped;
		}
	}

		
void CATGprsContextDeactivate::ValidateHangUpExpectStringL()
/**
 * This function validates the response from the phone.
 */
	{
	CCommChatString* foundChatString = iIo->FoundChatString();
	if (foundChatString == iNoCarrierExpectString)
		{
		LOGTEXT(_L8("CATGprsContextDeactivate::ValidateHangUpExpectStringL Modem returned NO CARRIER "));
		}
	else if ((foundChatString != iOKExpectString) && (foundChatString != iErrorExpectString))
		{
		LOGTEXT(_L8("CATGprsContextDeactivate:\tModem returned unknown response to hang up command"));
		User::Leave(KErrGeneral);
		}	
	}


void CATGprsContextDeactivate::Stop(TTsyReqHandle aTsyReqHandle)
/**
 * @param aTsyReqHandle handle to the client.
 */
	{
	LOGTEXT(_L8("CATGprsContextDeactivate::Stop called"));
	if(iState!=EATNotInProgress && aTsyReqHandle==iReqHandle)
		{
		LOGTEXT(_L8("CATGprsContextDeactivate::Stop Completing client request with KErrCancel"));
		Complete(KErrCancel,ETimeOutCompletion);
		}
	}


void CATGprsContextDeactivate::CompleteWithIOError(TEventSource aSource,TInt aStatus)
/**
 * This Function completes the command from the client with an error.
 *
 * @param aSource source of event from communication class.
 * @param aStatus status of event.
 */
	{
	LOGTEXT2(_L8("CATGprsContextDeactivate::CompleteWithIOError() called with iState = %d"),iState);
	Complete(aStatus,aSource);
	}


void CATGprsContextDeactivate::EventSignal(TEventSource aSource)
/**
 * This function contains the state machine for the command.  The states flow consecutively and are
 * described below.
 * 
 * @par EATNotInProgress
 * Nothing is being sent to the phone.
 *
 * @par EDTRDropped
 *	DTR has been dropped
 *
 * @par EWaitForDTRRaiseSettle
 *	Wait for DTR signal to settle. Also send +++ command
 *
 * @par EATEscapeSeqWaitForWriteComplete
 * Wait for +++ command write to complete.
 *
 * @par EATEscapeSeqCompleted
 *	Send ATH command to the phone.

 * @par EATHangupWaitForWriteComplete
 * Wait for write to complete.
 *
 * @par EATHangupReadCompleted
 *	Parse phone response.
 *
 * @par EHangUpCancelling
 * Cancel the deactivate command.
 *
 */
	{
	LOGTEXT2(_L8("CATGprsContextDeactivate::EventSignal, Event Source %d"), aSource);

	if (aSource==ETimeOutCompletion && iState!=EDTRDropped && iState!=EATHangupReadCompleted
		&& iState!=EATEscapeSeqCompleted && iState!=EWaitForDTRRaiseSettle
		 && iState!=EHangUpCancelling)
		{
		LOGTEXT(_L8("CATGprsContextDeactivate:\tTimeout Error during Hang Up"));
		Complete(KErrTimedOut,aSource); 
		return;
		}

	switch(iState)
		{
		case EDTRDropped:
			{
			__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
			iIo->Cancel();
			iIo->RaiseDTR();
			iIo->SetTimeOut(this,KDTRHighSettle);
			iState=EWaitForDTRRaiseSettle;
			break;
			}
		case EWaitForDTRRaiseSettle:// NO CARRIER may come back after 2-3 seconds, but send ATH anyway
			{
			__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
			TBuf8<KCommsDbSvrMaxFieldLength> escapeChar;
			TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameEscapeCharacter),escapeChar);
			if (ret)
				{
				Complete(ret,aSource);
				break;
				}	
			iTxBuffer.Format(KEscape ,&escapeChar, &escapeChar, &escapeChar);
			Write(KGprsCommandTimeOut);
			iState=EATEscapeSeqWaitForWriteComplete;
			}
			break;

		case EATEscapeSeqWaitForWriteComplete:
			{
			// do not use the standard complete handler since this will also do a AddStdExpectStrings() and
			// we are waiting for the No CARRIER.
			iIo->SetTimeOut(this, KGprsCommandTimeOut * KOneSecondPause);
			if (!iNoCarrierExpectString)
				iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
			StandardWriteCompletionHandler(aSource,KGprsCommandTimeOut);
			iState=EATEscapeSeqCompleted;
			break; 
			}
		
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
			iIo->RemoveExpectString(iNoCarrierExpectString);
			iNoCarrierExpectString=NULL;
			TPtrC8 hangUpCommand(KHangUpCommand);
			Write(hangUpCommand, KGprsCommandTimeOut);
			iState=EATHangupWaitForWriteComplete;
			}
			break;

		case EATHangupWaitForWriteComplete:
			{
			// Try with ATH, this time listen to OK/ERROR/NO CARRIER
			iIo->WriteAndTimerCancel(this);
			iIo->Read();
			StandardWriteCompletionHandler(aSource,KGprsCommandTimeOut);
			if (!iNoCarrierExpectString)
				iNoCarrierExpectString=iIo->AddExpectString(this,KNoCarrierString);
			iState=EATHangupReadCompleted;
			break;
			}

		case EATHangupReadCompleted:
			{
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
			}

		case EHangUpCancelling:
			{
			if (aSource==EWriteCompletion)
				{
				iIo->SetTimeOut(this,KOneSecondPause);
				}
			if (aSource==EReadCompletion || aSource==ETimeOutCompletion)
				{
				LOGTEXT(_L8("CATGprsContextDeactivate:\tHang up cancelled"));
				Complete(KErrCancel,aSource);
				}
			break;
			}
		case EATNotInProgress:
			break;
		default:
			{
			Panic(EIllegalEvent);
			}
		}
	}


void CATGprsContextDeactivate::Complete(TInt aError,TEventSource aSource)
/**
 * This Function completes the command from the client.
 * @param aError an error code to relay to client.
 */
	{
	LOGTEXT2(_L8("CATGprsContextDeactivate::Complete() with aError %d"),aError);
	RemoveStdExpectStrings();
	iIo->RemoveExpectStrings(this);
	iIo->WriteAndTimerCancel(this);
	iNoCarrierExpectString=NULL;
	iIo->Cancel();
	if (aError==KErrNone)
		{
		TCommConfig configPckg;
		aError=iPhoneGlobals->iConfiguration->PortConfig(configPckg,EConfigTypeInit);
		if (aError==KErrNone)
			aError = iIo->ConfigurePort(configPckg);
		
		iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeOnlineCommand;

		//
		// Set this context and all its sibling contexts as Inactive
		CGprs* parentPtr=(static_cast<CGprsContext*>(iTelObject))->Parent();
		RPacketService::TContextInfo contextInfo;
		for(TInt i=0;(parentPtr->Context(i))!=NULL;++i)
			{
			((CGprsContext*)iTelObject)->ContextInfo(&contextInfo);
			contextInfo.iStatus = RPacketContext::EStatusInactive;
			((CGprsContext*)iTelObject)->SetContextInfo(&contextInfo);
			// Set Gprs state		
 			((CGprsContext*)iTelObject)->Parent()->SetStatus(RPacketService::EStatusUnattached);
 			iPhoneGlobals->iNotificationStore->CheckNotification(STATIC_CAST(CTelObject*,iTelObject->Owner()), EPacketStatusChanged);
			iPhoneGlobals->iNotificationStore->CheckNotification(iTelObject, EPacketContextStatusChanged);
			}
		}

	// Allow our base class to do its thing and then complete the client request
	CATCommands::Complete(aError,aSource);		
	iTelObject->ReqCompleted(iReqHandle, aError);

	iState=EATNotInProgress;
	}
 


