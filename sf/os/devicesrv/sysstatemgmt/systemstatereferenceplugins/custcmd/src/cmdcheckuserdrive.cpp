/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Custom command for checking user drive 
*
*/

#include "cmdcheckuserdrive.h"
#include "ssmuiproviderdll.h"
#include "ssmdebug.h"
#include <f32file.h>

CCustomCmdCheckUserDrive* CCustomCmdCheckUserDrive::NewL()
	{
	return new (ELeave) CCustomCmdCheckUserDrive();
	}

CCustomCmdCheckUserDrive::~CCustomCmdCheckUserDrive()
    {
    }

CCustomCmdCheckUserDrive::CCustomCmdCheckUserDrive()
    {
    }

TInt CCustomCmdCheckUserDrive::Initialize(CSsmCustomCommandEnv* aCmdEnv)
    {
    iCmdEnv = aCmdEnv;
    return KErrNone;
    }

void CCustomCmdCheckUserDrive::Execute(const TDesC8& /*aParams*/, TRequestStatus& aRequest)
    {
    aRequest = KRequestPending;
    TInt driveid = CSsmUiSpecific::PhoneMemoryRootDriveId();
    DEBUGPRINT2A("CCustomCmdCheckUserDrive drive id is %d", driveid);

    TDriveInfo info;
    TInt errorCode = iCmdEnv->Rfs().Drive(info, driveid);
    if (KErrNone == errorCode)
        {
		DEBUGPRINT2A("CCustomCmdCheckUserDrive: User drive type %d", info.iType);
        if (info.iType == EMediaRam)
            {
            errorCode = KErrNotFound;
            }
        }
    DEBUGPRINT2A("CCustomCmdCheckUserDrive completed with %d", errorCode);
    TRequestStatus* request = &aRequest;
    User::RequestComplete( request, errorCode );
    }

void CCustomCmdCheckUserDrive::ExecuteCancel()
    {
    //Nothing to cancel
    }

void CCustomCmdCheckUserDrive::Close()
    {
    }

void CCustomCmdCheckUserDrive::Release()
    {
	delete this;
    }
