// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <e32cons.h>
#include <e32property.h>
#include "CSimPubSubChange.h"


CSimPubSubChange::CSimPubSubChange(MPubSubChangeCallback* aCallback, CSimPubSub::TPubSubProperty aProperty)
	: CActive(EPriorityStandard),  iCallback(aCallback), iPSProperty(aProperty)
	{
	CActiveScheduler::Add(this);
	}
	
CSimPubSubChange* CSimPubSubChange::NewL(MPubSubChangeCallback* aCallback, CSimPubSub::TPubSubProperty aProperty)
	{
	CSimPubSubChange* self = new (ELeave) CSimPubSubChange(aCallback, aProperty);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CSimPubSubChange::ConstructL()
	{
	iProp.Attach(iPSProperty.iCategory, iPSProperty.iKey);
	iProp.Subscribe(iStatus);
	SetActive();
	}

CSimPubSubChange::~CSimPubSubChange()
	{ 
	Cancel();
	iProp.Close();
	}

void CSimPubSubChange::RunL()
	{
	//Resubscribe before retrieving the value so we don't miss any changes
	TInt status = iStatus.Int();
	iProp.Subscribe(iStatus);
	SetActive();
	
	if(status == KErrNone)
		{
		TInt newStat;
		TInt ret = iProp.Get(iPSProperty.iCategory, iPSProperty.iKey, newStat);
		if (ret == KErrNone)
			{
			iCallback->PubSubCallback(newStat);
			}	
		}	
	}

void CSimPubSubChange::DoCancel()
	{
	iProp.Cancel();
	}
