/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Wrapper for file server notification handling.
*
*/



// INCLUDES
#include <e32std.h>
#include <f32file.h>
#include "disknotifyhandler.h"
#include "disknotifyhandlerimpl.h"
#include "disknotifyhandlerdebug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CDiskNotifyHandler
// ---------------------------------------------------------------------------
//
CDiskNotifyHandler::CDiskNotifyHandler()
    {
    FUNC_LOG
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CDiskNotifyHandler::ConstructL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs )
    {
    FUNC_LOG

    iImpl = CDiskNotifyHandlerImpl::NewL( aCallback, aFs );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiskNotifyHandler* CDiskNotifyHandler::NewL(
        MDiskNotifyHandlerCallback& aCallback,
        RFs& aFs )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    CDiskNotifyHandler* self = new ( ELeave ) CDiskNotifyHandler();
    CleanupStack::PushL( self );
    self->ConstructL( aCallback, aFs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CDiskNotifyHandler
// ---------------------------------------------------------------------------
//
EXPORT_C CDiskNotifyHandler::~CDiskNotifyHandler()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    delete iImpl;
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::NotifyDisk
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiskNotifyHandler::NotifyDisk()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return iImpl->NotifyDisk();
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyDisk
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyDisk()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyDisk();
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::NotifyDismount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiskNotifyHandler::NotifyDismount( TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return iImpl->NotifyDismount( aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyDismount
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyDismount( TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyDismount( aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyDismount
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyDismount()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyDismount();
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::AllowDismount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiskNotifyHandler::AllowDismount( TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return iImpl->AllowDismount( aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::NotifyDiskSpace
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiskNotifyHandler::NotifyDiskSpace(
        const TInt64& aThreshold, TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return iImpl->NotifyDiskSpace( aThreshold, aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyDiskSpace
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyDiskSpace(
        const TInt64& aThreshold, TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyDiskSpace( aThreshold, aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyDiskSpace
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyDiskSpace( TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyDiskSpace( aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyDiskSpace
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyDiskSpace()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyDiskSpace();
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::NotifyEntry
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiskNotifyHandler::NotifyEntry(
        TNotifyType aType, const TDesC& aEntry )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    return iImpl->NotifyEntry( aType, aEntry );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyEntry
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyEntry(
        TNotifyType aType, const TDesC& aEntry )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyEntry( aType, aEntry );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyEntry
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler:: CancelNotifyEntry( const TDesC& aEntry )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyEntry( aEntry );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelNotifyEntry
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelNotifyEntry()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelNotifyEntry();
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::StartDismount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiskNotifyHandler::StartDismount(
        TInt aDrive, TTimeIntervalMicroSeconds32 aForcedTimeout )
    {
    FUNC_LOG_WITH_CLIENT_NAME
    
    return iImpl->StartDismount( aDrive, aForcedTimeout );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelStartedDismount
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelStartedDismount( TInt aDrive )
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelStartedDismount( aDrive );
    }

// ---------------------------------------------------------------------------
// CDiskNotifyHandler::CancelStartedDismount
// ---------------------------------------------------------------------------
//
EXPORT_C void CDiskNotifyHandler::CancelStartedDismount()
    {
    FUNC_LOG_WITH_CLIENT_NAME

    iImpl->CancelStartedDismount();
    }

// ---------------------------------------------------------------------------
// MDiskNotifyHandlerCallback::HandleNotifyDisk
// ---------------------------------------------------------------------------
//
EXPORT_C void MDiskNotifyHandlerCallback::HandleNotifyDisk(
        TInt /*aError*/, const TDiskEvent& /*aEvent*/ )
    {
    ERROR_LOG( "MDiskNotifyHandlerCallback::HandleNotifyDisk" )
    }

// ---------------------------------------------------------------------------
// MDiskNotifyHandlerCallback::HandleNotifyDismount
// ---------------------------------------------------------------------------
//
EXPORT_C void MDiskNotifyHandlerCallback::HandleNotifyDismount(
        TInt /*aError*/, const TDismountEvent& /*aEvent*/ )
    {
    ERROR_LOG( "MDiskNotifyHandlerCallback::HandleNotifyDismount" )
    }

// ---------------------------------------------------------------------------
// MDiskNotifyHandlerCallback::HandleNotifyDiskSpace
// ---------------------------------------------------------------------------
//
EXPORT_C void MDiskNotifyHandlerCallback::HandleNotifyDiskSpace(
        TInt /*aError*/, const TDiskSpaceEvent& /*aEvent*/ )
    {
    ERROR_LOG( "MDiskNotifyHandlerCallback::HandleNotifyDiskSpace" )
    }

// ---------------------------------------------------------------------------
// MDiskNotifyHandlerCallback::HandleNotifyEntry
// ---------------------------------------------------------------------------
//
EXPORT_C void MDiskNotifyHandlerCallback::HandleNotifyEntry(
        TInt /*aError*/, const TEntryEvent& /*aEvent*/ )
    {
    ERROR_LOG( "MDiskNotifyHandlerCallback::HandleNotifyEntry" )
    }

// ---------------------------------------------------------------------------
// MDiskNotifyHandlerCallback::HandleNotifyDismountFinished
// ---------------------------------------------------------------------------
//
EXPORT_C void MDiskNotifyHandlerCallback::HandleNotifyDismountFinished(
        TInt /*aError*/, const TDismountFinishedEvent& /*aEvent*/ )
    {
    ERROR_LOG( "MDiskNotifyHandlerCallback::HandleNotifyDismountFinished" )
    }


// End of File
