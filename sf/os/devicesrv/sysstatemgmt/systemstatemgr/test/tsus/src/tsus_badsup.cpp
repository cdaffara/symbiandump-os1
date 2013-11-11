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



#include "tsus_badsup.h"

EXPORT_C MSsmUtility* CBadSup::NewL()
	{
	return NULL; //bad behaviour used in CSsmSupLoaderTest::doTestCreatePluginLCL()
	}

CBadSup::CBadSup() : CActive(CActive::EPriorityStandard)
	{
	}

CBadSup::~CBadSup()
	{
	}

void CBadSup::InitializeL()
	{
	CActiveScheduler::Add(this);
	}

void CBadSup::StartL()
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

void CBadSup::Release()
	{
	Deque();
	}

void CBadSup::DoCancel()
	{
	}

void CBadSup::RunL()
	{
	}

TInt CBadSup::RunError(TInt aError)
	{
	return aError;
	}
