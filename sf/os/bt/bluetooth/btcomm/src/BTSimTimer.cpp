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

#include "BTSimTimer.h"
#include "btcomm.h"


CBTTimerSimple::CBTTimerSimple(TInt aPriority,CBTPortProxy* aContext) : CTimer(aPriority),
	iClient(aContext)
	{
	}

CBTTimerSimple::~CBTTimerSimple()
	{
	CancelAlarm();
	}


CBTTimerSimple* CBTTimerSimple::NewL(TInt aPriority,CBTPortProxy* aContext)
	{
	CBTTimerSimple* self = new (ELeave) CBTTimerSimple(aPriority,aContext);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CBTTimerSimple::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CBTTimerSimple::CancelAlarm()
	{
	if(IsActive())
		{
		CActive::Cancel();
		}
	}

void CBTTimerSimple::RunL()
	{
	iClient->ShutdownAlarm();
	}
