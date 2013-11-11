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

//
// OOM Test Base Class
//

COomTestBase::COomTestBase()
	{}

void COomTestBase::ConstructL()
	{

	iNetCon = CNetworkController::NewL();
	}

COomTestBase::~COomTestBase()
	{

	if (iNetCon)
		delete iNetCon;
	}

void COomTestBase::SelectComplete(const TDesC&)
	{

	User::Invariant();
	}

void COomTestBase::SelectComplete(TInt)
	{

	User::Invariant();
	}

void COomTestBase::ReconnectComplete(TInt)
	{

	User::Invariant();
	}

