// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pspropertyobserver.h"

CPsPropertyObserver* CPsPropertyObserver::NewL(const TUid& aGroup, TInt aKey, MPsPropertyObserver& aObserver)
	{
	CPsPropertyObserver* self = new(ELeave) CPsPropertyObserver(aKey,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aGroup);
	CleanupStack::Pop(self);
	return self;
	}

CPsPropertyObserver::CPsPropertyObserver(TInt aKey, MPsPropertyObserver& aObserver)
	:CActive(EPriorityNormal),
	iKey(aKey),
	iObserver(aObserver)
	{
	}

void CPsPropertyObserver::ConstructL(const TUid& aGroup)
	{
	User::LeaveIfError(iProperty.Attach(aGroup,iKey));
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	Start();
	}

CPsPropertyObserver::~CPsPropertyObserver()
	{
	Cancel();
	iProperty.Close();
	}

void CPsPropertyObserver::DoCancel()
	{
	iProperty.Cancel();
	}

void CPsPropertyObserver::RunL()
	{
	iProperty.Get(iValue);	
	iObserver.PsPropertyChanged(iKey,iValue);
	// resubscribe
	Start();
	}

void CPsPropertyObserver::Start()
	{
	if (IsActive())
		{
		Cancel();
		}
	
	iProperty.Subscribe(iStatus);
	SetActive();		
	}
