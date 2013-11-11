// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CRequestBase which act as an interface with dispater to handle a single passed IPC request

#include "requestbase.h"
#include "mslogger.h"
#include "globalphonemanager.h"

// ---------------------------------------------------------------------------
// CRequestBase::~CRequestBase
// other items were commented in a header
// ---------------------------------------------------------------------------
CRequestBase::~CRequestBase()
	{
	LOGTEXT(_L8("[Ltsy] Starting CRequestBase::~CRequestBase()"));
	iIpcCompleteObserver = NULL;
	}

// ---------------------------------------------------------------------------
// CRequestBase::CRequestBase
// other items were commented in a header
// ---------------------------------------------------------------------------
CRequestBase::CRequestBase(CGlobalPhonemanager& aPhoneGlobals)
							:iPhoneGlobals(aPhoneGlobals)
	{
	LOGTEXT(_L8("[Ltsy] Starting CRequestBase::CRequestBase()"));
	}

// ---------------------------------------------------------------------------
// CRequestBase::StartRequest
// other items were commented in a header
// ---------------------------------------------------------------------------
void CRequestBase::StartRequest()
	{
	}

// ---------------------------------------------------------------------------
// CRequestBase::SetIpcRequestCompleteObserver
// other items were commented in a header
// ---------------------------------------------------------------------------
void CRequestBase::SetIpcRequestCompleteObserver(MIpcRequestCompleteObserver* aIpcCompleteObserver)
    {
    iIpcCompleteObserver = aIpcCompleteObserver;
    }

// ---------------------------------------------------------------------------
// CRequestBase::SetIpcId
// other items were commented in a header
// ---------------------------------------------------------------------------
void CRequestBase::SetIpcId(TInt aIpcId)
    {
    iIpcId = aIpcId;
    }

// ---------------------------------------------------------------------------
// CRequestBase::SetIpcId
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CRequestBase::IpcId()
    {
    return iIpcId;
    }
// End of file
