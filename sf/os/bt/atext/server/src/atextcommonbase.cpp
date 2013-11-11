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
* Description: 
*
*/


#include <atextcommonbase.h>
#include "atextcommonobserver.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Instantiate the AT Extension Plugin via ECom framework.
// ---------------------------------------------------------------------------
//
EXPORT_C CATExtCommonBase* CATExtCommonBase::NewL(
    TUid aImplementationUid,
    MATExtCommonObserver& aObserver,
    const TDesC8& aName )
    {
    TInt32 keyOffset = _FOFF( CATExtCommonBase, iInstanceUid );
    CATExtCommonBase* self = reinterpret_cast<CATExtCommonBase*>(
        REComSession::CreateImplementationL( aImplementationUid, keyOffset ));
    self->SetObserver( aObserver );
    self->ReportConnectionName( aName );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CATExtCommonBase::~CATExtCommonBase()
    {
    TRACE_FUNC_ENTRY
    REComSession::DestroyedImplementation( iInstanceUid );
    if ( iObserver )
        {
        iObserver->ATExtPluginClosed( this );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Called by concrete extension plugin to inform that mode status has changed.
// Instructs ATEXT to complete client request message for
// ReceiveModeStatusChange().
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CATExtCommonBase::SendModeStatusChange( TUint aMode )
    {
    TRACE_FUNC_ENTRY
    if ( !iObserver )
        {
        return KErrGeneral;
        }
    iObserver->SendModeStatusChange( aMode );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Called by concrete extension plugin to inform that NVRAM status has
// changed. Instructs ATEXT to complete client request message for
// ReceiveNvramStatusChange().
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CATExtCommonBase::SendNvramStatusChange( const TDesC8& aNvram )
    {
    TRACE_FUNC_ENTRY
    if ( !iObserver )
        {
        return KErrGeneral;
        }
    iObserver->SendNvramStatusChange( aNvram );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Sets the observer.
// ---------------------------------------------------------------------------
//
EXPORT_C void CATExtCommonBase::SetObserver( MATExtCommonObserver& aObserver )
    {
    TRACE_FUNC_ENTRY
    iObserver = &aObserver;
    TRACE_FUNC_EXIT
    }
