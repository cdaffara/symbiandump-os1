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

#include "Te_LoopBackrestartsignal.h"
#include "Te_LoopBackScriptEng.h"

CRestartSignal* CRestartSignal::NewL(CATScriptEng* aScriptEng)
	{
	CRestartSignal* self=new(ELeave) CRestartSignal(aScriptEng);
	return self;
	}

CRestartSignal::CRestartSignal(CATScriptEng* aScriptEng)
					: CActive(EPriorityStandard), iScriptEng(aScriptEng)
	{
	CActiveScheduler::Add(this);
	}

void CRestartSignal::Start(TRequestStatus** aStatus)
	{
	*aStatus=&iStatus;
	iStatus = KRequestPending;
	SetActive();
	}

CRestartSignal::~CRestartSignal()
	{
	Cancel();
	}

void CRestartSignal::RunL()
	{
	iScriptEng->Restart();
	iStatus = KRequestPending;
	SetActive();
	}

void CRestartSignal::DoCancel()
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,KErrCancel);
	}
