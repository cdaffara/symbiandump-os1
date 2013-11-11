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
//

/**
 @file
 @internalComponent
 @released
*/
#include "cmdclearstartupreason.h"
#include "ssmdebug.h"
#include <centralrepository.h>
#include <ssm/starterdomaincrkeys.h>

CCustomCmdClearStartupReason* CCustomCmdClearStartupReason::NewL()
	{
	return new (ELeave) CCustomCmdClearStartupReason();
	}

CCustomCmdClearStartupReason::~CCustomCmdClearStartupReason()
    {
    delete iRepository;
    }

CCustomCmdClearStartupReason::CCustomCmdClearStartupReason()
    {
    }

TInt CCustomCmdClearStartupReason::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
    {
    // Connect to the central repository
    TRAPD(err, iRepository = CRepository::NewL(KCRUidStartup));
    return err;
    }

void CCustomCmdClearStartupReason::Execute(const TDesC8& /*aParams*/, TRequestStatus& aRequest)
    {
    aRequest = KRequestPending;
    const TInt err = iRepository->Set(KStartupReason, ENormalStartup);
    if(KErrNone != err)
            {
            DEBUGPRINT2A("Setting KStartupReason CenRep key failed with error %d", err);
            }

    TRequestStatus* request = &aRequest;
    User::RequestComplete(request, err);
    }

void CCustomCmdClearStartupReason::ExecuteCancel()
    {
    //Nothing to cancel
    }

void CCustomCmdClearStartupReason::Close()
    {
    }

void CCustomCmdClearStartupReason::Release()
    {
	delete this;
    }
