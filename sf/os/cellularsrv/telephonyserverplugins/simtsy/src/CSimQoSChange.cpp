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
// CSimNetStatChange.CPP
// 
//


#include <e32base.h>
#include <e32cons.h>
#include <e32property.h>
#include "CSimQoSChange.h"


CSimQoSChange::CSimQoSChange(MQoSChangeCallBack* aCallback, CSimPubSub::TPubSubProperty aProperty)
	: CActive(EPriorityStandard),  iPSProperty(aProperty), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}
	
CSimQoSChange* CSimQoSChange::NewL(MQoSChangeCallBack* aCallback, CSimPubSub::TPubSubProperty aProperty)
	{
	CSimQoSChange* self = new (ELeave) CSimQoSChange(aCallback, aProperty);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CSimQoSChange::ConstructL()
	{
	iProp.Attach(iPSProperty.iCategory, iPSProperty.iKey);
	iProp.Subscribe(iStatus);
	SetActive();
	}

CSimQoSChange::~CSimQoSChange()
	{
	Cancel();
	iProp.Close();
	}

void CSimQoSChange::RunL()
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
			iCallback->QoSChangeCallBack(newStat);
			}	
		}	
	}

void CSimQoSChange::DoCancel()
	{
	iProp.Cancel();
	}

