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
* Description:
* Implementation of CSsmDiskReserver class.
*
*/

#include "ssmmapperutilityinternalpskeys.h"
#include "ssmcustcmdsprivatepskeys.h"
#include "ssmdiskreserver.h"
#include "trace.h"

// Free disk space requested for boot
const TInt KRequestedDiskSpaceForBoot = 0x23000; // 140 kB

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmDiskReserver::NewL
// ---------------------------------------------------------------------------
//
CSsmDiskReserver* CSsmDiskReserver::NewL()
	{
    FUNC_LOG;
    
    CSsmDiskReserver* self = new( ELeave ) CSsmDiskReserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}


// ---------------------------------------------------------------------------
// CSsmDiskReserver::~CSsmDiskReserver
// ---------------------------------------------------------------------------
//
CSsmDiskReserver::~CSsmDiskReserver()
    {
    FUNC_LOG;
    
    iProperty.Close();
    iSharedData.Close();
    }


// ---------------------------------------------------------------------------
// CSsmDiskReserver::ConstructL
// ---------------------------------------------------------------------------
//
void CSsmDiskReserver::ConstructL()
    {
    FUNC_LOG;

    // Attach to KDiskReserveKey
    TInt err = iProperty.Attach( KPSStarterUid, KDiskReserveKey );
    ERROR( err, "Error attaching KDiskReserveKey" );
    User::LeaveIfError( err );

    // Reserve disk space
    err = iSharedData.Connect();
    ERROR( err, "Failed to connect to SharedData" );
    User::LeaveIfError( err );
    iSharedData.RequestFreeDiskSpace( KRequestedDiskSpaceForBoot );
    }


// ---------------------------------------------------------------------------
// CSsmDiskReserver::WaitStartupFinished
// ---------------------------------------------------------------------------
//
void CSsmDiskReserver::WaitStartupFinished()
    {
    FUNC_LOG;

    // Wait for startup to finish
    TRequestStatus request;
    INFO( "Wait for KDiskReserveKey" );
    iProperty.Subscribe( request );
    User::WaitForRequest( request );
    INFO( "KDiskReserveKey changed" );

    // Release disk space reservation
    iSharedData.CancelFreeDiskSpaceRequest();
    }

