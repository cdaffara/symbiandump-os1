// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "isvao.h"
#include "testconstants.h"

CIsvAO::CIsvAO() :
	CActive(EPriorityStandard), iLineStatusPckg(iLineStatus), 
	iCurrentOperation(EIsvNoOperation)
	{
	} // CIsvAO::CIsvAO

CIsvAO* CIsvAO::NewLC()
	{
	CIsvAO* self = new ( ELeave ) CIsvAO();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CIsvAO::NewLC

CIsvAO* CIsvAO::NewL()
	{
	CIsvAO* self = CIsvAO::NewLC();
	CleanupStack::Pop(self);
	return self;
	} // CIsvAO::NewL

void CIsvAO::ConstructL()
	{
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	CActiveScheduler::Add(this); 

	iTelephony = CTelephony::NewL();
	} // CIsvAO::ConstructL

CIsvAO::~CIsvAO()
	{
	Cancel();
	
	if (iTelephony)
		{
		delete iTelephony;
		iTelephony = NULL;
		}
	
	if (iScheduler)
		{
		delete iScheduler;
		iScheduler = NULL;
		}
	
	} // CIsvAO::~CIsvAO

void CIsvAO::DoCancel()
	{
	switch (iCurrentOperation)
		{
		case EIsvDial:
			iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
			break;
		case EIsvHangUp:
			iTelephony->CancelAsync(CTelephony::EHangupCancel);
			break;
		case EIsvAnswer:
			iTelephony->CancelAsync(CTelephony::EAnswerIncomingCallCancel);
			break;
		case EIsvNotifyIncomingCall:
			iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
			break;
		default:
			break;
		}
	
	iCurrentOperation = EIsvNoOperation;
	} // CIsvAO::DoCancel

void CIsvAO::DoOperation(TIsvOperation aOperation)
	{
	Cancel(); 
	
	iCurrentOperation = aOperation;
	
	switch (aOperation)
		{
		case EIsvDial:
			{
			
			CTelephony::TTelNumber telNumber(KPhoneNumber);
			CTelephony::TCallParamsV1 callParams;
			callParams.iIdRestrict = CTelephony::ESendMyId;
			CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);
		
			iTelephony->DialNewCall(iStatus, callParamsPckg, telNumber, iCallId);
			}
			break;
		case EIsvHangUp:
			{
			iTelephony->Hangup(iStatus, iCallId);
			}
			break;
		case EIsvAnswer:
			{
			iTelephony->AnswerIncomingCall(iStatus, iCallId);
			}
			break;
		case EIsvNotifyIncomingCall:
			{
			iTelephony->NotifyChange(iStatus,
									CTelephony::EVoiceLineStatusChange,
									iLineStatusPckg);
			}
			break;
		default:
			break;
		}

	SetActive();
	CActiveScheduler::Start();
	} // CIsvAO::Dial

void CIsvAO::RunL()
	{
	iCurrentOperation = EIsvNoOperation;
	CActiveScheduler::Stop();
	} // CIsvAO::RunL

TInt CIsvAO::RunError(TInt aError)
	{
	return aError;
	} // CIsvAO::RunError

