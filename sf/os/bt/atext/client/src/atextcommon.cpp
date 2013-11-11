/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client side interface of ATEXT common functionality
*
*/


#include <atextcommon.h>
#include "atextclientsrvcommon.h"
#include "atext_interfaceuid.h"
#include "debug.h"

_LIT( KATExtCommonSrvExe, "atextcommon.exe" );

const TUid KATExtSrvUid3 = { 0x2001CBEC };

static TInt StartServer();

// ---------------------------------------------------------------------------
// Connects to ATEXT server and creates a new session
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::Connect( const TDesC8& aName )
    {
    TRACE_FUNC_ENTRY
    TVersion version( KCommonServerMajorVersionNumber,
                      KCommonServerMinorVersionNumber,
                      KCommonServerBuildVersionNumber );
    TInt retVal = CreateSession( KATExtSrvName, version );
    if ( retVal!=KErrNone && retVal!=KErrAlreadyExists )
        {
        retVal = StartServer();
        if ( retVal==KErrNone || retVal==KErrAlreadyExists )
            {
            retVal = CreateSession( KATExtSrvName, version );
            }
        }
    if ( retVal==KErrNone || retVal==KErrAlreadyExists )
        {
        retVal = SendReceive( EATExtSetCommonInterface, TIpcArgs(&aName) );
        }
    if ( retVal != KErrNone )
        {
        Close();
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Synchronously closes the session
// Optional: client can do either Close() or SynchronousClose()+Close()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::SynchronousClose()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtSynchronousClose );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Gets current mode status
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::GetMode( TUint& aMask, TUint& aMode )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TUint> maskPckg( aMask );
    TPckg<TUint> modePckg( aMode );
    TInt retVal = SendReceive( EATExtGetMode, TIpcArgs(&maskPckg,&modePckg) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Receives mode status change
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::ReceiveModeStatusChange( TRequestStatus& aStatus,
                                                     TPckg<TUint>& aMode )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRequestStatus* status = &aStatus;
        aStatus = KRequestPending;
        User::RequestComplete( status, KErrBadHandle );
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    SendReceive( EATExtReceiveModeStatusChange, TIpcArgs(&aMode), aStatus );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels a pending receiving request
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::CancelReceiveModeStatusChange()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtCancelReceiveModeStatusChange );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Gets current NVRAM status
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::GetNvramStatus(
    TBuf8<KDefaultNvramBufLength>& aNvram )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtGetNvramStatus,
                               TIpcArgs(&aNvram) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Receives NVRAM status change
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::ReceiveNvramStatusChange(
    TRequestStatus& aStatus,
    TBuf8<KDefaultNvramBufLength>& aNvram )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRequestStatus* status = &aStatus;
        aStatus = KRequestPending;
        User::RequestComplete( status, KErrBadHandle );
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    SendReceive( EATExtReceiveNvramStatusChange,
                 TIpcArgs(&aNvram),
                 aStatus );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels a pending NVRAM receive request
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExtCommon::CancelReceiveNvramStatusChange()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtCancelReceiveNvramStatusChange );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Starts the server
// ---------------------------------------------------------------------------
//
TInt StartServer()
    {
    TRACE_STATIC_FUNC
    const TUidType serverUid( KNullUid, KNullUid, KATExtSrvUid3 );
    RProcess server;
    TInt retTemp = server.Create( KATExtCommonSrvExe, KNullDesC, serverUid );
    if ( retTemp != KErrNone )
        {
        return retTemp;
        }
    server.SetPriority( EPriorityHigh );
    TRequestStatus status;
    server.Rendezvous( status );
    if ( status != KRequestPending )
        {
        server.Kill( 0 );
        }
    else
        {
        server.Resume();
        }
    User::WaitForRequest( status );
    TRACE_INFO((_L("Server started, code %d"), status.Int()))
    retTemp = ( server.ExitType() == EExitPanic ) ? KErrGeneral : status.Int();
    server.Close();
    return retTemp;
    }
