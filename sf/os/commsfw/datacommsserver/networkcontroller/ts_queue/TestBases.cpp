// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestBases.h"
#include "NetConLog.h"
#include "CNetworkController.h"

//
// Queue Test Base Class
//

CQueueTestBase::CQueueTestBase()
	{}

void CQueueTestBase::ConstructL()
	{

	iNetCon = CNetworkController::NewL();
	}

CQueueTestBase::~CQueueTestBase()
	{

	if (iNetCon)
		delete iNetCon;
	}

void CQueueTestBase::SelectComplete(const TDesC&)
	{

	User::Invariant();
	}

void CQueueTestBase::SelectComplete(TInt)
	{

	User::Invariant();
	}

void CQueueTestBase::ReconnectComplete(TInt)
	{

	User::Invariant();
	}

