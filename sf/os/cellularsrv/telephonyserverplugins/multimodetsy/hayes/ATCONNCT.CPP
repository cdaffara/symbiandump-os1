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
//

#include <commsdattypesv1_1.h>
#include "ATCONNCT.H"
#include "mSLOGGER.H"
#include "PHONE.H"
#include "CALL.H"
#include "ATNOCARR.H"
#include "NOTIFY.H"
#include "ATIO.H"

//
// Connect to already dialled call
//
CATConnectData* CATConnectData::NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
	{
	CATConnectData* connect=new(ELeave) CATConnectData(aIo, aTelObject, aInit,aPhoneGlobals);
	CleanupStack::PushL(connect);
	connect->ConstructL();
	CleanupStack::Pop();
	return connect;
	}

CATConnectData::CATConnectData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals)
											: CATDataCallConnectCommands(aIo,aTelObject,aInit,aPhoneGlobals)
	{}

CATConnectData::~CATConnectData()
	{}

void CATConnectData::Start(TTsyReqHandle aTsyReqHandle, TAny* aParams)
	{
	LOGTEXT(_L8("Starting Connect Command"));
	iState=EATInitialising;
	CATDataCallConnectCommands::Start(aTsyReqHandle,aParams);
	}

void CATConnectData::Stop(TTsyReqHandle aTsyReqHandle)
//
//	Send a carriage return to cancel Connect
//
	{
	__ASSERT_ALWAYS(aTsyReqHandle == iReqHandle,Panic(EIllegalTsyReqHandle));
	__ASSERT_ALWAYS(iState!=EATNotInProgress,Panic(EATCommand_NotInProgress));		
	LOGTEXT(_L8("Cancelling Connect Command"));
	RemoveConnectExpectStrings();
	iIo->WriteAndTimerCancel(this);
	if (iState!=EATInitialising)
		{
		Write(KCarriageReturn(),1);
		iState = EATCancellingWaitForWriteComplete;
		iPreConnectState=CATCallConnectCommands::ENotInProgress;	
		}
	else
		{
		AddStdExpectStrings();
		iPreConnectState=CATCallConnectCommands::ECancelling;
		iState = EATNotInProgress;
		}
	}

TInt CATConnectData::AddConnectExpectStrings()
	{
	TInt ret=AddCommonExpectStrings();
	if(ret!=KErrNone)
		return ret;

	if (!iNoDialToneExpectString)
		{
		ret=iPhoneGlobals->iConfiguration->ConfigModemString(TPtrC(KCDTypeNameNoDialTone),iNoDialToneString);
		if(ret!=KErrNone)
			return ret;
		iNoDialToneExpectString=iIo->AddExpectString(this,iNoDialToneString);
		}

	return KErrNone;
	}	

void CATConnectData::RemoveConnectExpectStrings()
	{
	RemoveCommonExpectStrings();
	iIo->RemoveExpectString(iNoDialToneExpectString);
	iNoDialToneExpectString=NULL;
	}

TInt CATConnectData::ValidateConnectExpectString()
	{
	const CCommChatString* foundChatString = iIo->FoundChatString();

	if (foundChatString == iConnectExpectString)
		return KErrNone;

	if (foundChatString == iNoCarrierExpectString)
		return KErrEtelNoCarrier;

	if (foundChatString == iNoDialToneExpectString)
		{
		LOGTEXT(_L8("Modem returned NO DIALTONE in response to dial command"));
		return KErrEtelNoDialTone;
		}

	LOGTEXT(_L8("Modem returned unknown response to connect command"));
	return KErrGeneral;
	}

void CATConnectData::CompleteWithIOError(TEventSource aSource,TInt aStatus)
	{
	if (iState!=EATNotInProgress)
		{
		iState = EATNotInProgress;
		CATCallConnectCommands::CompleteWithIOError(aSource,aStatus);
		}
	}

void CATConnectData::EventSignal(TEventSource aSource)
	{
	if ((aSource==ETimeOutCompletion)&&(iState!=EATSpeedReadComplete)
		&&(iState!=EATCancellingReadCompleted)&&(iState!=EATNotInProgress)
		&&(iPreConnectState!=EATWaitForATCheckOK))
		{
		LOGTEXT(_L8("Timeout Error during Connect"));
		iState = EATNotInProgress;
		Complete(KErrTimedOut,aSource);
		return;
		}

	if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted
		&& iPreConnectState!=CATCallConnectCommands::ENotInProgress)
		{
		CATCallConnectCommands::PreConnectEventSignal(aSource);
		if (iPreConnectState==CATCallConnectCommands::ENotInProgress)	// cancelled
			iState=EATNotInProgress;
		if (iPreConnectState!=CATCallConnectCommands::EATInitCompleted)
			return;
		else 
			iState=EATSendConnectCommand;
		}

	switch(iState)
		{
	case EATSendConnectCommand:
		{
		ChangeLineStatus(RCall::EStatusConnecting);
		// EStatusConnecting always results in KErrNone return
		(void)ChangeCallStatus(RMobileCall::EStatusConnecting);
		iPhoneGlobals->iNotificationStore->CheckNotification(REINTERPRET_CAST(CCallBase*,iTelObject),EBegunConnecting);
		Write(KDialCommand,3);
		iState=EATConnectWaitForWriteComplete;
		}
		break;

	case EATConnectWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TInt ret=AddConnectExpectStrings();
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		iIo->SetTimeOut(this,(iPhoneGlobals->iPhoneStatus.iWaitForCarrierTime*1000)+KExtraWaitTime);
		iState=EATConnectReadCompleted;
		break;
		}

	case EATConnectReadCompleted:
		__ASSERT_ALWAYS(aSource==EReadCompletion,Panic(EATCommand_IllegalCompletionReadExpected));
			{
			TInt ret=ValidateConnectExpectString();
			RemoveConnectExpectStrings();
			if (ret!=KErrNone)
				{
				iState = EATNotInProgress;
				Complete(ret,aSource);
				}
			iIo->SetTimeOut(this,KTimeForExtraRxData);
			iState=EATSpeedReadComplete;
			}
		break;

	case EATSpeedReadComplete:
		{
		__ASSERT_ALWAYS(aSource==ETimeOutCompletion,Panic(EATCommand_IllegalCompletionWaitExpected));
		iIo->WriteAndTimerCancel(this);
		TInt ret=ParseForBearerCapsResponse();
		iState = EATNotInProgress;
		if (ret!=KErrNone)
			{
			Complete(ret,aSource);
			break;
			}
		ret=ParseForBearerSpeedResponse();
		Complete(ret,aSource);
		}
		break;

	case EATCancellingWaitForWriteComplete:
		{
		__ASSERT_ALWAYS(aSource==EWriteCompletion,Panic(EATCommand_IllegalCompletionWriteExpected));
		TInt ret=AddConnectExpectStrings();
		if (ret)
			{
			Complete(ret,aSource);
			break;
			}
		iIo->SetTimeOut(this);
		iState=EATCancellingReadCompleted;
		}
		break;

	case EATCancellingReadCompleted:
			{
			iState = EATNotInProgress;
			if (aSource==EReadCompletion)
				{
				TInt ret=ValidateConnectExpectString();
				RemoveConnectExpectStrings();
				if (ret==KErrNone)
					{
					Complete(KErrNone,aSource);	// Has connected despite sending CR
					return;
					}
				}
			RemoveConnectExpectStrings();
			Complete(KErrCancel,aSource);
			}
		break;
		
	default:
		;
		}
	}


