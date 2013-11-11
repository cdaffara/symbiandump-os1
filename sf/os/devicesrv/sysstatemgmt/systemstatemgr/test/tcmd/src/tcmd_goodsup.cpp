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


#include "tcmd_goodsup.h"


EXPORT_C MSsmUtility* CCmdGoodSup::NewL()
	{
	return new (ELeave) CCmdGoodSup;
	}

CCmdGoodSup::CCmdGoodSup() : CActive(CActive::EPriorityStandard)
	{
	}

CCmdGoodSup::~CCmdGoodSup()
	{
	if(IsAdded())
		{
		Deque();
		}
	}

void CCmdGoodSup::InitializeL()
	{
	CActiveScheduler::Add(this);
	}

void CCmdGoodSup::StartL()
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

void CCmdGoodSup::Release()
	{
	delete this;
	}

void CCmdGoodSup::DoCancel()
	{
	}

void CCmdGoodSup::RunL()
	{
	}

TInt CCmdGoodSup::RunError(TInt aError)
	{
	return aError;
	}
