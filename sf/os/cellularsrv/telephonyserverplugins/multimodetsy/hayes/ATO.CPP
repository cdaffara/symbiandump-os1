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
// Go to On-Line(data) state from On-Line Command state
// 
//

#include <commsdattypesv1_1.h>
#include <cdblen.h>
#include "ATO.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "NOTIFY.H"
#include "ATIO.H"

CATSetToOnlineDataMode* CATSetToOnlineDataMode::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATSetToOnlineDataMode* ato=new(ELeave) CATSetToOnlineDataMode(aIo, aTelObject, aInit, aPhoneGlobals);
	CleanupStack::PushL(ato);
	ato->ConstructL();
	CleanupStack::Pop();
	return ato;
	}

CATSetToOnlineDataMode::CATSetToOnlineDataMode(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATCallAlterCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATSetToOnlineDataMode::~CATSetToOnlineDataMode()
	{
	iIo->RemoveExpectStrings(this);
	}

void CATSetToOnlineDataMode::Start(TTsyReqHandle aTsyReqHandle, TAny* /*aParams*/)
	{
	LOGTEXT(_L8("Starting ATO Command to force into On-Line Data Mode"));
	iReqHandle=aTsyReqHandle;
	TBuf8<KCommsDbSvrMaxFieldLength> toOnLineDataCommand;
	TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameOnLine), toOnLineDataCommand);
	if (ret)
		{
		iTelObject->ReqCompleted(iReqHandle, ret);
		}
	else
		{
		iTxBuffer.Format(KATAndStringFormatString,&toOnLineDataCommand);
		iIo->Write(this,iTxBuffer);
		iIo->SetTimeOut(this,KOneSecondPause);
		iState=EATOWaitForWriteComplete;
		}
	}

void CATSetToOnlineDataMode::Stop(TTsyReqHandle aTsyReqHandle)
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	__ASSERT_ALWAYS(iState!=EATNotInProgress,Panic(EATCommand_NotInProgress));
	iIo->SetTimeOut(this,KOneSecondPause);
	iState = EATCancelling;
	}

void CATSetToOnlineDataMode::ValidateExpectStringWithConnectL()
	{
	if(iIo->FoundChatString()==iErrorExpectString)
		{
		LOGTEXT(_L8("Modem returned ERROR in response to command"));
		User::Leave(KErrGeneral);
		}
	if(iIo->FoundChatString()==iConnectExpectString)
		return;
	LOGTEXT(_L8("Modem returned unknown response to command"));
	User::Leave(KErrGeneral);
	}

void CATSetToOnlineDataMode::RemoveATOExpectStrings()
	{
	iIo->RemoveExpectString(iConnectExpectString);
	iConnectExpectString=NULL;
	iIo->RemoveExpectString(iErrorExpectString);
	iErrorExpectString=NULL;
	}

void CATSetToOnlineDataMode::CompleteWithIOError(TEventSource /*aSource*/,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iIo->WriteAndTimerCancel(this);
		iTelObject->ReqCompleted(iReqHandle, aStatus);
		iState = EATNotInProgress;
		}
	}

void CATSetToOnlineDataMode::EventSignal(TEventSource aSource)
	{
	if (aSource==ETimeOutCompletion)
		{
		LOGTEXT(_L8("Timeout Error during ATO"));
		RemoveATOExpectStrings();
		iPhoneGlobals->iPhoneStatus.iInitStatus=EPhoneNotInitialised;
		iTelObject->ReqCompleted(iReqHandle,KErrTimedOut);
		iState=EATNotInProgress;
		return;
		}
	switch(iState)
		{
	case EATOWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		if (!iConnectExpectString)
			{
			TInt ret = iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameConnect),iConnectString);
			if (ret)
				{
				iTelObject->ReqCompleted(iReqHandle, ret);
				return;
				}
			AppendWildCardChar(iConnectString);
			iConnectExpectString=iIo->AddExpectString(this,iConnectString);
			}
		if (!iErrorExpectString)
			iErrorExpectString=iIo->AddExpectString(this,KErrorString);
		iIo->SetTimeOut(this);
		iState=EATOReadCompleted;
		}
		break;

	case EATOReadCompleted:
		{
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TRAPD(ret,ValidateExpectStringWithConnectL());
		Complete(ret,aSource);
		}
		break;

	case EATCancelling:
		{
		if (aSource==EWriteCompletion)
			{
			iIo->SetTimeOut(this,KOneSecondPause);
			}
		else
			{
			Complete(KErrCancel,aSource);
			}
		}
		break;

	default:
		break;
		}
	}

void CATSetToOnlineDataMode::Complete(TInt aError,TEventSource aSource)
	{
	RemoveATOExpectStrings();
	iPhoneGlobals->iPhoneStatus.iMode = RPhone::EModeOnlineData;
	if (aError==KErrNone)
		iIo->Cancel();	// OK as long as this command is only called from LoanDataPort
	CATCommands::Complete(aError,aSource);
	iTelObject->ReqCompleted(iReqHandle, aError);
	iState=EATNotInProgress;
	}
