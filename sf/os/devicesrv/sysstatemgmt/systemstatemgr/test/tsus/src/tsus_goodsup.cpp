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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "tsus_goodsup.h"

 

EXPORT_C MSsmUtility* CGoodSup::NewL()
	{
	return new (ELeave) CGoodSup;
	}

CGoodSup::CGoodSup() : CActive(CActive::EPriorityStandard)
	{
	}

CGoodSup::~CGoodSup()
	{
	if(IsAdded())
		{
		Deque();
		}
	}

void CGoodSup::InitializeL()
	{
	CActiveScheduler::Add(this);
	}

void CGoodSup::StartL()
	{
	if(IsAdded())
		{
		if(!IsActive())
			{
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	else
		{
		User::Leave(KErrNotReady);
		}
	}

void CGoodSup::Release()
	{
	delete this;
	}

void CGoodSup::DoCancel()
	{
	}

void CGoodSup::RunL()
	{
	RSemaphore sem;
	if(KErrNone == sem.OpenGlobal(KTestGoodSupSemaphore))
		{
		sem.Signal();
		sem.Close();
		}
	}

TInt CGoodSup::RunError(TInt aError)
	{
	return aError;
	}
