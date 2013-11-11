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
// AT Escape to command (online) mode
// 
//

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include "ATESCAPE.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "ATIO.H"

CATSetToOnlineCommandMode* CATSetToOnlineCommandMode::NewL(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
	{
	return new(ELeave) CATSetToOnlineCommandMode(aIo,aTelObject,aPhoneGlobals);
	}

CATSetToOnlineCommandMode::CATSetToOnlineCommandMode(CATIO* aIo,CTelObject* aTelObject,CPhoneGlobals* aPhoneGlobals)
												: CATBase(aIo,aTelObject,aPhoneGlobals)
	{}

CATSetToOnlineCommandMode::~CATSetToOnlineCommandMode()
	{}

void CATSetToOnlineCommandMode::StartEscapeSequence(CATCommands* aPendingCommand,TTsyReqHandle aTsyReqHandle, TAny* aParams)
//
//	Begin the escape sequence with the guard time pause
//
	{
	LOGTEXT(_L8("Starting Escape Command"));
	iPendingCommand = aPendingCommand;
	iReqHandle = aTsyReqHandle;
	iParams = aParams;
	iIo->SetTimeOut(this,KLengthOfGuardTime*20);
	iState = EWaitForATSilence;
	}

void CATSetToOnlineCommandMode::StopEscapeSequence(TTsyReqHandle aTsyReqHandle)
//
//	Attempt to stop the escape sequence
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	LOGTEXT(_L8("Cancelling Escape Sequence"));
	iIo->RemoveExpectString(iOKExpectString);
	iOKExpectString=NULL;
	iIo->WriteAndTimerCancel(this);
	iPendingCommand->Owner()->ReqCompleted(iReqHandle,KErrCancel);
	if (iState != EWaitForATSilence)
		// if nothing has been sent yet, can remain in On line Data Mode
		{
		iPhoneGlobals->iPhoneStatus.iMode=RPhone::EModeOnlineCommand;
		}
	}

void CATSetToOnlineCommandMode::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	LOGTEXT(_L8("Error in Escape sequence"));
	iIo->WriteAndTimerCancel(this);
	iPendingCommand->Owner()->ReqCompleted(iReqHandle, aStatus);
	}

void CATSetToOnlineCommandMode::EventSignal(TEventSource aSource)
	{
	if ((aSource==ETimeOutCompletion) && (iState!=EWaitForATSilence))
		{
		LOGTEXT(_L8("Timeout Error during Escape Sequence"));
		iIo->RemoveExpectStrings(this);						
		iOKExpectString=NULL;
		iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneNotInitialised;
		iPendingCommand->Owner()->ReqCompleted(iReqHandle,KErrTimedOut);
		return;
		}

	switch(iState)
		{
	case EWaitForATSilence:
			{
			__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
			TBuf8<KCommsDbSvrMaxFieldLength> escapeChar;
			TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameEscapeCharacter),escapeChar);
			if (ret)
				{
				iPendingCommand->Owner()->ReqCompleted(iReqHandle,ret);
				break;
				}
			iTxBuffer.Format(_L8("%S%S%S"),&escapeChar,&escapeChar,&escapeChar);
			iIo->Write(this,iTxBuffer);
			iIo->SetTimeOut(this,KOneSecondPause);
			if (!(iIo->ReadPending()))
				iIo->Read();
			iState=EWaitForWriteCompletion;
			}
		break;

	case EWaitForWriteCompletion:
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		if (!iOKExpectString)
			iOKExpectString = iIo->AddExpectString(this,KOkString);
		iIo->SetTimeOut(this);
		iState=EWaitForATEscapeOK;
		break;

	case EWaitForATEscapeOK:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
		{
		TInt ret(ValidateExpectString());
		if (ret)
			{
			iPendingCommand->Owner()->ReqCompleted(iReqHandle,ret);
			}
		else
			{
			iPhoneGlobals->iPhoneStatus.iMode=RPhone::EModeOnlineCommand;
			iPendingCommand->Start(iReqHandle,iParams);
			}
		}
		iIo->WriteAndTimerCancel(this);
		iIo->RemoveExpectString(iOKExpectString);
		iOKExpectString=NULL;
		break;
		}
	}
