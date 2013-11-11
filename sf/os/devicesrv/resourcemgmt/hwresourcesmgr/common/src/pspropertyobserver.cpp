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

EXPORT_C CPsPropertyObserver* CPsPropertyObserver::NewL(const TUid& aGroup, TInt aKey, MPsPropertyObserver& aObserver)
	{
	CPsPropertyObserver* self = new(ELeave) CPsPropertyObserver(aGroup,aKey,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aGroup);
	CleanupStack::Pop(self);
	return self;
	}

CPsPropertyObserver::CPsPropertyObserver(TUid aGroup, TInt aKey, MPsPropertyObserver& aObserver)
	:CActive(EPriorityNormal),
	iGroup(aGroup),
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
	if(KErrNone == iStatus.Int())
    {
    iProperty.Get(iValue);	
    }
	iObserver.PsPropertyChanged(iKey,iValue, iStatus.Int());
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

EXPORT_C TInt CPsPropertyObserver::Get(const TUid& aGroup, TInt aKey, TInt& value)
    {
    return RProperty::Get(aGroup, aKey, value);
    }

EXPORT_C TInt CPsPropertyObserver::Get(TInt& value)
    {
    return Get(iGroup, iKey, value);
    }

