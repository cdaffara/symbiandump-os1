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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <e32property.h>
#include <ssm/ssmstatemanager.h>
#include "ssmdebug.h"

#include "tsus_swpchangesup.h"



EXPORT_C MSsmUtility* CSwpLoadSup1::NewL()
	{
	return new (ELeave) CSwpLoadSup1;
	}

CSwpLoadSup1::CSwpLoadSup1() : CActive(CActive::EPriorityStandard)
	{
	}

CSwpLoadSup1::~CSwpLoadSup1()
	{
	if(IsAdded())
		{
		Deque();
		}
	}

void CSwpLoadSup1::InitializeL()
	{
	CActiveScheduler::Add(this);
	}

void CSwpLoadSup1::StartL()
	{
	if(IsAdded())
		{
		if(!IsActive())
			{
			RSsmStateManager client;
			const TInt connect = client.Connect();
			client.RegisterSwpMapping(KTestSwpLoadSup, KTestSwpLoadSupFile);
			
			TSsmSwp swp(KTestSwpLoadSup, KTestSwpLoadSupNewValue);
			TRequestStatus trs;
			client.RequestSwpChange(swp, trs);
			User::LeaveIfError(trs==KRequestPending);
			User::WaitForRequest(trs);
			
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, trs.Int());
			}
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

void CSwpLoadSup1::Release()
	{
	delete this;
	}

void CSwpLoadSup1::DoCancel()
	{
	}

void CSwpLoadSup1::RunL()
	{
	RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KTestSwpLoadSupSemaphore))
		{
		sem.Signal();
		sem.Close();
		}
	}

TInt CSwpLoadSup1::RunError(TInt aError)
	{
	return aError;
	}



EXPORT_C MSsmUtility* CSwpLoadSup2::NewL()
	{
	return new (ELeave) CSwpLoadSup2;
	}

CSwpLoadSup2::CSwpLoadSup2() : CActive(CActive::EPriorityStandard)
	{
	}

CSwpLoadSup2::~CSwpLoadSup2()
	{
	if(IsAdded())
		{
		Deque();
		}
	}

void CSwpLoadSup2::InitializeL()
	{
	CActiveScheduler::Add(this);
	}

void CSwpLoadSup2::StartL()
	{
	if(IsAdded())
		{
		if(!IsActive())
			{
			RSsmStateManager client;
			const TInt connect = client.Connect();
			client.RegisterSwpMapping(KTestSwpLoadSup, KTestSwpLoadSupFile);
			
			TSsmSwp swp(KTestSwpLoadSup, KTestSwpLoadSupNewValue);
			TRequestStatus trs;
			client.RequestSwpChange(swp, trs);
			User::LeaveIfError(trs==KRequestPending);
			User::WaitForRequest(trs);
			
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, trs.Int());
			}
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

void CSwpLoadSup2::Release()
	{
	delete this;
	}

void CSwpLoadSup2::DoCancel()
	{
	}

void CSwpLoadSup2::RunL()
	{
	RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KTestSwpLoadSupSemaphore))
		{
		sem.Signal();
		sem.Close();
		}
	}

TInt CSwpLoadSup2::RunError(TInt aError)
	{
	return aError;
	}
