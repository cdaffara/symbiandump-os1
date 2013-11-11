/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Active object for getting notification when there is enough 
*      free disk space to regain the original size of the reserve file.
*
*
*/


// INCLUDE FILES

#include <e32std.h>

#include "shareddatafilesystemnotifier.h"
#include "dosserver.h"
#include "dosshareddatabase.h"
#include "dos_debug.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can't leave
CSharedDataFileSystemNotifier::CSharedDataFileSystemNotifier( 
    CDosServer& aServer ) :
    CActive( CActive::EPriorityLow ), 
    iServer( aServer ) 
    {
    COM_TRACE_( "CSharedDataFileSystemNotifier::CSharedDataFileSystemNotifier" );    
    }

// Destructor
CSharedDataFileSystemNotifier::~CSharedDataFileSystemNotifier()
    {
    COM_TRACE_( "CSharedDataFileSystemNotifier::~CSharedDataFileSystemNotifier()" );   
    }

// -----------------------------------------------------------------------------
// CSharedDataFileSystemNotifier::New()
// -----------------------------------------------------------------------------

CSharedDataFileSystemNotifier* CSharedDataFileSystemNotifier::NewL( 
    CDosServer& aServer )
    {
    COM_TRACE_1( "CSharedDataFileSystemNotifier* CSharedDataFileSystemNotifier::NewL(0x%x)", &aServer );
    CSharedDataFileSystemNotifier* self = new (ELeave)
    CSharedDataFileSystemNotifier( aServer );

    CActiveScheduler::Add( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSharedDataFileSystemNotifier::Start()
// ----------------------------------------------------------------------------

void CSharedDataFileSystemNotifier::Start( TInt aTreshold, RFs& aFs, CDosSharedDataBase* sdBase )
    {
    COM_TRACE_1( "SharedData: Free disk space notifier started, treshold %d", aTreshold );

    // just in case
    iSDBase = sdBase;
    iFs = aFs;
    iFs.NotifyDiskSpaceCancel();

    iFs.NotifyDiskSpace( aTreshold, EDriveC, iStatus );
    SetActive();
    }
 
// ----------------------------------------------------------------------------
// CSharedDataFileSystemNotifier::RunL()
// ----------------------------------------------------------------------------

void CSharedDataFileSystemNotifier::RunL()
    {
   	COM_TRACE_1( "SharedData: Free disk space notified, status = %d", iStatus.Int() );
    
    if ( iStatus == KErrNone && !iServer.iCurrentFreeDiskSpaceRequest )
        {
        iSDBase->SetReserveFileSize( 0 );
        }
    }

// ----------------------------------------------------------------------------
// CSharedDataFileSystemNotifier::DoCancel()
// ----------------------------------------------------------------------------

void CSharedDataFileSystemNotifier::DoCancel()
    {
    COM_TRACE_( "CSharedDataFileSystemNotifier::DoCancel()" );
    iFs.NotifyDiskSpaceCancel();
    }

//  End of File  
