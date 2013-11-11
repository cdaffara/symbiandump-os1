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
* Description:  Entry point for ssmdiskreserver process.
*
*/


#include <e32base.h>
#include "ssmdiskreserver.h"
#include "trace.h"

_LIT( KThreadName, "ssmdiskreserver" );

// ---------------------------------------------------------------------------
// Entry point for the executable.
// ---------------------------------------------------------------------------
//
TInt E32Main()
    {
    FUNC_LOG;
    __UHEAP_MARK;

    User::RenameThread( KThreadName );
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TInt err = KErrNoMemory;
    if ( cleanup )
        {
        CSsmDiskReserver *reserver = NULL;
        TRAP( err, reserver = CSsmDiskReserver::NewL() );
        ERROR( err, "Leave in CSsmDiskReserver::NewL" );
        if( err == KErrNone )
            {
            reserver->WaitStartupFinished();
            delete reserver;
            }
        delete cleanup;
        }

    __UHEAP_MARKEND;
    return err;
    }
