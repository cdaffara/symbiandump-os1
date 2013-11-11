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


#include <atextpluginbase.h>
#include "atextpluginobserver.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Instantiate the AT Extension Plugin via ECom framework.
// ---------------------------------------------------------------------------
//
EXPORT_C CATExtPluginBase* CATExtPluginBase::NewL(
    TUid aImplementationUid,
    MATExtPluginObserver& aObserver,
    const TDesC8& aName )
    {
    TInt32 keyOffset = _FOFF( CATExtPluginBase, iInstanceUid );
    CATExtPluginBase* self = reinterpret_cast<CATExtPluginBase*>(
        REComSession::CreateImplementationL( aImplementationUid, keyOffset ));
    self->SetObserver( aObserver );
    self->ReportConnectionName( aName );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CATExtPluginBase::~CATExtPluginBase()
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
// Reports change to quiet mode.
// The quiet mode is off by default.
// ---------------------------------------------------------------------------
//
EXPORT_C void CATExtPluginBase::ReportQuietModeChange( TBool aMode )
    {
    TRACE_FUNC_ENTRY
    iQuietMode = aMode;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Reports change to verbose mode.
// The verbose mode is on by default.
// ---------------------------------------------------------------------------
//
EXPORT_C void CATExtPluginBase::ReportVerboseModeChange( TBool aMode )
    {
    TRACE_FUNC_ENTRY
    iVerboseMode = aMode;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Reports change to a value of a character
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CATExtPluginBase::ReportCharacterChange(
    TATExtensionCharType aCharType,
    TInt8 aNewChar )
    {
    TRACE_FUNC_ENTRY
    TInt retVal = KErrNone;
    switch ( aCharType )
        {
        case ECharTypeCarriage:
            iCarriageReturn = aNewChar;
            break;
        case ECharTypeLineFeed:
            iLineFeed = aNewChar;
            break;
        case ECharTypeBackspace:
            iBackspace = aNewChar;
            break;
        default:
            retVal = KErrNotSupported;
            break;
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Called by concrete extension plugin when there is an unsolicited result
// code should be sent to remote device. Instructs ATEXT to complete client
// request message for ReceiveUnsolicitedResult().
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CATExtPluginBase::SendUnsolicitedResult( const TDesC8& aAT )
    {
    TRACE_FUNC_ENTRY
    TInt retVal = iObserver->SendUnsolicitedResult( this, aAT );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Called by concrete extension plugin to inform that a command handling has
// been completed or rejected. Instructs ATEXT to complete client request
// message for HandleCommand().
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CATExtPluginBase::HandleCommandCompleted(
    TInt aError,
    TATExtensionReplyType aReplyType )
    {
    TRACE_FUNC_ENTRY
    TInt retVal = iObserver->HandleCommandCompleted( this, aError, aReplyType );
    TRACE_FUNC_EXIT
    return retVal;
    }


// ---------------------------------------------------------------------------
// Called by concrete extension plugin to inform the array of supported
// commands should be returned
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CATExtPluginBase::GetSupportedCommands(
    RPointerArray<HBufC8>& aCmds )
    {
    TRACE_FUNC_ENTRY
    TInt retVal = iObserver->GetSupportedCommands( this, aCmds );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Sets the observer.
// ---------------------------------------------------------------------------
//
EXPORT_C void CATExtPluginBase::SetObserver( MATExtPluginObserver& aObserver )
    {
    TRACE_FUNC_ENTRY
    iObserver = &aObserver;
    TRACE_FUNC_EXIT
    }
