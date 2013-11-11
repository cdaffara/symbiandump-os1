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
// Deletes an SMS message present on the ME (Mobile Equipment). AT+CMGD=xx
// 
//

#include "NOTIFY.H"
#include "mSMSSTOR.H"
#include "mSMSDEL.H"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "Matstd.h"
#include "smsbase.H"
#include "et_phone_util.h"

_LIT8(KSmsDeleteCommand,"AT+CMGD=%d\r");


CATSmsStorageDelete* CATSmsStorageDelete::NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
	{
	CATSmsStorageDelete* self = new(ELeave) CATSmsStorageDelete(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CATSmsStorageDelete::CATSmsStorageDelete(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals)
	:CATSmsCommands(aIo, aTelObject, aInit, aPhoneGlobals), 
	iCancelled(EFalse),
	iSmsStore(REINTERPRET_CAST(CMobileSmsStore*, aTelObject))
	{}

CATSmsStorageDelete::~CATSmsStorageDelete()
	{}

void CATSmsStorageDelete::ConstructL()
	{
	CATCommands::ConstructL();
	}

void CATSmsStorageDelete::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
//
// Places BufferMarker and then starts the FSM (Finite State Machine).
//
	{
	__ASSERT_DEBUG(aParams,Panic(EMobileSmsMessagingNullParameter));
	__ASSERT_DEBUG(aTsyReqHandle!=TSY_HANDLE_INIT_VALUE,Panic(EMobileSmsMessagingNullParameter));

	iReqHandle = aTsyReqHandle;
	iCancelled=EFalse;
	iDeleteIndex = *(static_cast<TInt*>(aParams));

	if (ComparePrefMem(iSmsStore->iStoreName))
		StartDeleteSequence();
	else //ie: the compared memories are different, must change phone mem to temporary mem
		{
		SetCurrentPrefMem(iSmsStore->iStoreName);
		iState = EATWaitForSendingPrefMemComplete;
		}
	}

void CATSmsStorageDelete::StartDeleteSequence()
	{
	iTxBuffer.Format(KSmsDeleteCommand,iDeleteIndex); // Delete Command as per ETSI std (GSM 07.05, May 1996)
	iIo->Write(this, iTxBuffer);
	iIo->SetTimeOut(this, 5000);
	iState = EATWaitForSendingDeleteComplete;
	}

void CATSmsStorageDelete::EventSignal(TEventSource aSource)
//
// FSM for GSmsStorageDelete
//
	{
	LOGTEXT2(_L8("CATSmsStorageDelete:\tiState = %D"), iState);
	if (aSource == ETimeOutCompletion)
		{
		LOGTEXT(_L8("CATSmsStorageDelete:\tTimeout error during Sms Delete"));
		iIo->WriteAndTimerCancel(this);
		if ((iState != EATWaitForPrefMemResponse)&&(!ComparePrefMem(iSmsStore->iStoreName)))
			iPhoneGlobals->iPhonePrefMem = iSmsStore->iStoreName; 
		Complete(KErrTimedOut);	
		return;
		} //if

	switch(iState)
		{
	// the following 2 cases deal with changing to the client's preferred memory, if the situation arrises
	// there are also two cases at the end of this state machine (EventSignal) that change the settings back
	// to the original configuration.
	case EATWaitForSendingPrefMemComplete:
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource, 5);
		iState = EATWaitForPrefMemResponse;
		break;

	case EATWaitForPrefMemResponse:
		__ASSERT_ALWAYS(aSource == EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt pos=iIo->BufferFindF(KCPMSResponseString);
			RemoveStdExpectStrings();
			if (pos == KErrNotFound)
				{
				Complete(pos, aSource);
				return;
				}
			LOGTEXT(_L8("CATSmsStorageDelete:\tPhones's preferred memory successfully set to client's preferred memory"));
			iPhoneGlobals->iPhonePrefMem=iSmsStore->iStoreName;
			if (!iCancelled)
				StartDeleteSequence();
			else
				{
				Complete(KErrCancel, aSource);
				}
			}
		break;

	// The next 2 cases deal with deleting....

	case EATWaitForSendingDeleteComplete:
		{
		__ASSERT_ALWAYS(aSource == EWriteCompletion, Panic(EATCommand_IllegalCompletionWriteExpected));
		iIo->WriteAndTimerCancel(this);
		StandardWriteCompletionHandler(aSource,5);
		AddCmsErrorExpectString();
		iState=EATWaitForOKResponseComplete;
		}
		break;

	case EATWaitForOKResponseComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret=ValidateExpectStringsWithCmsError();
			RemoveCmsErrorExpectString();
			RemoveStdExpectStrings();
			// Delete has completed - hopefully successfully!
			Complete(ret,aSource);
			}
		break;

	default:
		break;
		}
	}


void CATSmsStorageDelete::Stop(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("CATSmsStorageDelete:\tCancel called."));
	// Wait for convenient place in FSM to cancel
	iCancelled=ETrue;
	}


void CATSmsStorageDelete::Complete(TInt aError)
	{
	Complete(aError, EReadCompletion);
	}

void CATSmsStorageDelete::Complete(TInt aError, TEventSource aSource)
	{
	iIo->RemoveExpectStrings(this);
	iOKExpectString=NULL;
	iErrorExpectString=NULL;

	//
	// Call our base classes Complete so that
	// we allow it do what ever it needs to do.
	CATCommands::Complete(aError,aSource);

	if (iReqHandle != 0 )
		iTelObject->ReqCompleted(iReqHandle, aError);

	if (aSource == EWriteCompletion)
		iIo->Read();

	iState = EATNotInProgress;
	}

void CATSmsStorageDelete::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iOKExpectString=NULL;
		iErrorExpectString=NULL;
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

TInt CATSmsStorageDelete::ValidateExpectStringsWithCmsError()
//
// Check for "ERROR" or "+CMS ERROR:" expect strings
// This would be the place to add more intelligent processing of the +CMS ERROR: xxx
// response.  At the moment this is rather crude.  "+CMS ERROR: 300" translates to a ME error
// which should not set aRetry, whereas a "+CMS ERROR: <anything>" might translate
// to a PDU error.
//
	{
	TInt ret(KErrNone);
	if((iIo->FoundChatString()==iErrorExpectString)||
	   (iIo->FoundChatString()==iCmsExpectString))
		{
		LOGTEXT(_L8("CATSmsStorageDelete:\tSMS Delete error detected."));
		ret=KErrGeneral;
		}
	return ret;
	}

