// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Deal with Network & Operator information
// 
//

#include <et_phone.h>
#include "mimsi.h"
#include "mSLOGGER.H"
#include "ATIO.H"
#include "Matstd.h"


// Get the IMSI for from the phone
// NB - not supported by all phones.

_LIT8(KGetSubscriberIdCommand,"AT+CIMI");

inline CATSubscriberId::CATSubscriberId(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	: CATCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATSubscriberId* CATSubscriberId::NewL(CATIO* aIo,CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATSubscriberId* netInfo = new(ELeave) CATSubscriberId(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(netInfo);
	netInfo->ConstructL();
	CleanupStack::Pop();
	return netInfo;
	}

CATSubscriberId::~CATSubscriberId()
	{
	iIo->RemoveExpectStrings(this);
	}

void CATSubscriberId::Start(TTsyReqHandle aTsyReqHandle,TAny* aParam)
	{
	LOGTEXT(_L8("MmTsy:\tCATSubscriberId:\tStarting Subscriber Id"));
	iReturnPtr=static_cast<RMobilePhone::TMobilePhoneSubscriberId*>(aParam);
	iReqHandle = aTsyReqHandle;
	WriteExpectingResults(KGetSubscriberIdCommand, 3);
	__ASSERT_ALWAYS(iIo->AddExpectString(this,KNotifyMeIfErrorString), Panic(EGeneral));
	iState=EATImsiWaitForWriteComplete;
	}

void CATSubscriberId::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("MmTsy:\tCATSubscriberId:\tTimeout Error during IMSI read"));
		RemoveStdExpectStrings();
		Complete(KErrTimedOut,aSource);
		return;
		}

	switch(iState)
		{
		case EATImsiWaitForWriteComplete:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
			{
			iIo->WriteAndTimerCancel(this);
			StandardWriteCompletionHandler(aSource, 5);
			iState=EATImsiReadComplete;
			}
		break;

		case EATImsiReadComplete:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			TInt ret(ValidateExpectString());
			RemoveStdExpectStrings();
			if (ret==KErrNone)
				TRAP(ret,ParseResponseL());
			Complete(ret,aSource);
			}
			break;
	
		case EATWaitForStopState:
			__ASSERT_ALWAYS(aSource==EReadCompletion, Panic(EATCommand_IllegalCompletionReadExpected));
			{
			iIo->WriteAndTimerCancel(this);
			Complete(KErrCancel, aSource);
			}
			break;


		default:
			break;
		}//switch
	}//EventSignal

void CATSubscriberId::Stop(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle, Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("MmTsy:\tCATSubscriberId:\tCancel called."));
	switch(iState)
		{
		case EATNotInProgress:
		case EATImsiWaitForWriteComplete:
			{
			LOGTEXT2(_L8("Current state TSY is cancelling from %d"), iState);
			Complete(KErrCancel, EReadCompletion);
			}
			break;

		default:
			{
			LOGTEXT(_L8("MmTsy:\tCATSubscriberId:\tStop, now waiting for expected modem response"));
			AddStdExpectStrings();
			iIo->SetTimeOut(this);
			iState = EATWaitForStopState;
			}
			break;

		}
	}


void CATSubscriberId::Complete(TInt aError,TEventSource aSource)
	{
	if (aError==KErrNone)
		iState = EATCompleted;
	else if (aError==KErrGeneral)
		{	// +CIMI not supported by the phone, transmogrify into KErrNotSupported
		iState = EATNotSupported;
		aError = KErrNotSupported;
		}
	else
		{
// This flag is set in CATCommands::Complete now.
//		iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneNotInitialised;
		iState = EATNotInProgress;
		}
	iIo->WriteAndTimerCancel(this);
	iIo->RemoveExpectStrings(this);
	iOKExpectString = NULL;
	iErrorExpectString = NULL;
	CATCommands::Complete(aError,aSource);
	LOGTEXT2(_L8("MmTsy:CATSubscriberId:\tCATSubscriberId completed with error code : %d"), aError);
	if (iReqHandle)
		iTelObject->ReqCompleted(iReqHandle, aError);
	if (aSource==EWriteCompletion)
		iIo->Read();
	}

void CATSubscriberId::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iState = EATNotInProgress;
		if (iReqHandle)
			iTelObject->ReqCompleted(iReqHandle, aStatus);
		}
	}

void CATSubscriberId::ParseResponseL()
/**
 * This function parses the phone's response, copying it into the iImsi buffer.  This buffer is
 * only 15 characters long, and the Siemens S25 returns a 16 character long IMSI.  Other phones
 * return only the leftmost 15 characters.  This function takes this into account by returning
 * only the leftmost 15 characters of the phone's response.
**/
	{
	ParseBufferLC();
	TDblQueIter<CATParamListEntry> iter(iRxResults);
	CATParamListEntry* entry=iter++;
	if (entry==NULL)
		User::Leave(KErrGeneral);

	if(entry->iResultPtr.Length() > RMobilePhone::KIMSISize)
		iImsi.Copy(entry->iResultPtr.Left(RMobilePhone::KIMSISize));
	else
		iImsi.Copy(entry->iResultPtr);

	if (iter++!=NULL)
		User::Leave(KErrGeneral);
	*iReturnPtr=iImsi;

	CleanupStack::PopAndDestroy();
	}

TBool CATSubscriberId::CachedValue(TDes& aImsi)
//
// return the cached value if we have it
//
	{
	if (iState!=EATCompleted)
		return EFalse;
	aImsi=iImsi;
	return ETrue;
	}

