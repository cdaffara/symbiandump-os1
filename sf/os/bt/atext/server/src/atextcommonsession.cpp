/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <badesca.h>
#include <atextcommonbase.h>
#include "atextsrvcommon.h"
#include "atextclientsrvcommon.h"
#include "atextcommonsession.h"
#include "atext_interfaceuid.h"
#include "utils.h"
#include "debug.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtCommonSession* CATExtCommonSession::NewL( CATExtSrvCommon& aServer,
                                                const TVersion& aVersion )
    {
    return new (ELeave) CATExtCommonSession( aServer, aVersion );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CATExtCommonSession::~CATExtCommonSession()
    {
    TRACE_FUNC_ENTRY
    Destruct( EFalse );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Handles the servicing of client requests passed to the server
// ---------------------------------------------------------------------------
//
void CATExtCommonSession::ServiceL( const RMessage2& aMessage )
    {
    if ( iVersion.iMajor==KCommonServerMajorVersionNumber &&
         iVersion.iMinor==KCommonServerMinorVersionNumber &&
         iVersion.iBuild==KCommonServerBuildVersionNumber )
        {
        DoServiceCommonL( aMessage );
        }
    else
        {
        aMessage.Complete( KErrNotSupported );
        }
    TRACE_FUNC_ENTRY
    }

// ---------------------------------------------------------------------------
// Handles the servicing of client requests passed to the server
// ---------------------------------------------------------------------------
//
void CATExtCommonSession::DoServiceCommonL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    switch ( aMessage.Function() )
        {
        case EATExtSetCommonInterface:
            {
            TInt retTrap = KErrNone;
            TRAP( retTrap, IpcSetCommonInterfaceL(aMessage) );
            aMessage.Complete( retTrap );
            }
            break;
        case EATExtSynchronousClose:
            {
            IpcSynchronousClose( aMessage );
            }
            break;
        case EATExtGetMode:
            {
            IpcGetMode( aMessage );
            }
            break;
        case EATExtReceiveModeStatusChange:
            {
            IpcReceiveModeStatusChange( aMessage );
            }
            break;
        case EATExtCancelReceiveModeStatusChange:
            {
            IpcCancelReceiveModeStatusChange( aMessage );
            }
            break;
        case EATExtGetNvramStatus:
            {
            IpcGetNvramStatus( aMessage );
            }
            break;
        case EATExtReceiveNvramStatusChange:
            {
            IpcReceiveNvramStatusChange( aMessage );
            }
            break;
        case EATExtCancelReceiveNvramStatusChange:
            {
            IpcCancelReceiveNvramStatusChange( aMessage );
            }
            break;
        default:
            {
            aMessage.Complete( KErrNotSupported );
            break;
            }
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// CATExtCommonSession::CATExtCommonSession
// ---------------------------------------------------------------------------
//
CATExtCommonSession::CATExtCommonSession( CATExtSrvCommon& aServer,
                                          const TVersion& aVersion ) :
    iServer( aServer ),
    iEComSession( NULL ),
    iVersion( aVersion )
    {
    }

// ---------------------------------------------------------------------------
// Handles the servicing of setting up interface
// ---------------------------------------------------------------------------
//
void CATExtCommonSession::IpcSetCommonInterfaceL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( iCommonBase )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    RBuf8 connectionName;
    CleanupClosePushL( connectionName );
    TInt retTemp = ReadStringFromMessage( connectionName,
                                          EATExtConnectParamName,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        User::Leave( retTemp );
        }
    iEComSession = &REComSession::OpenL();
    RImplInfoPtrArray implementations;
    CleanupResetDestroyClosePushL( implementations );
    TUid ifUid = TUid::Uid( ATEXT_INTERFACE_COM_UID );
    iEComSession->ListImplementationsL( ifUid,
                                        implementations );
    if ( implementations.Count() != 1 )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    TUid pluginUid = implementations[0]->ImplementationUid();
    iCommonBase = CATExtCommonBase::NewL( pluginUid, *this, connectionName );
    CleanupStack::PopAndDestroy( &implementations );
    CleanupStack::PopAndDestroy( &connectionName );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Synchronously closes the session
// Optional: client can do either Close() or SynchronousClose()
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcSynchronousClose( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    Destruct( ETrue );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of mode status reporting
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcGetMode( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCommonBase )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TUint currentMode = 0;
    TPckgBuf<TUint> maskPckg;
    TInt retTemp = ReadStructFromMessage( maskPckg,
                                          EATExtGetModeParamMask,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    retTemp = iCommonBase->GetMode( maskPckg(), currentMode );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TPckg<TUint> currentModePckg( currentMode );
    retTemp = aMessage.Write( EATExtGetModeParamMode, currentModePckg );
    TRACE_INFO(( _L("Write returned %d"), retTemp ));
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of mode status change receiving
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcReceiveModeStatusChange( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCommonBase )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( iModeStatusMessage.Handle() )
        {
        aMessage.Complete( KErrInUse );
        TRACE_FUNC_EXIT
        return KErrInUse;
        }
    TInt retTemp = iCommonBase->ReceiveModeStatusChange();
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    iModeStatusMessage = aMessage;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of mode status change receive cancel
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcCancelReceiveModeStatusChange(
    const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCommonBase )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( !iModeStatusMessage.Handle() )
        {
        aMessage.Complete( KErrBadHandle );
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    iCommonBase->CancelReceiveModeStatusChange();
    iModeStatusMessage.Complete( KErrCancel );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of NVRAM status reporting
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcGetNvramStatus( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCommonBase )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    RBuf8 nvramBuffer;
    TInt retVal = iCommonBase->GetNvramStatus( nvramBuffer );
    WriteReplyBufferToClient( nvramBuffer,
                              EATExtGetNvramStatusParamNvram,
                              aMessage );
    aMessage.Complete( retVal );
    nvramBuffer.Close();
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of NVRAM status receiving
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcReceiveNvramStatusChange(
    const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCommonBase )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( iNvramStatusMessage.Handle() )
        {
        aMessage.Complete( KErrInUse );
        TRACE_FUNC_EXIT
        return KErrInUse;
        }
    if ( aMessage.GetDesMaxLength(EATExtGetNvramStatusParamNvram) !=
         KDefaultNvramBufLength )
        {
        aMessage.Complete( KErrBadDescriptor );
        TRACE_FUNC_EXIT
        return KErrBadDescriptor;
        }
    TInt retTemp = iCommonBase->ReceiveNvramStatusChange();
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    iNvramStatusMessage = aMessage;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of pending NVRAM status receive cancel
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::IpcCancelReceiveNvramStatusChange(
    const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iCommonBase )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( !iNvramStatusMessage.Handle() )
        {
        aMessage.Complete( KErrBadHandle );
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    iCommonBase->CancelReceiveNvramStatusChange();
    iNvramStatusMessage.Complete( KErrCancel );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Writes NVRAM status to client with WriteReplyBufferToClient() and
// completes the client request message
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::WriteNvramStatusToClient( const TDesC8& aNvram,
                                                    TInt aNvramSlot )
    {
    TRACE_FUNC_ENTRY
    if ( !iNvramStatusMessage.Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = WriteReplyBufferToClient( aNvram,
                                            aNvramSlot,
                                            iNvramStatusMessage );
    iNvramStatusMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reads a string from a message
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::ReadStringFromMessage( RBuf8& aBuffer,
                                                 TInt aDataSlot,
                                                 const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    TInt retVal = KErrNone;
    TInt desLength = aMessage.GetDesLength( aDataSlot );
    if ( desLength <= 0 )
        {
        TRACE_FUNC_EXIT
        return KErrArgument;
        }
    retVal = aBuffer.Create( desLength );
    if ( retVal == KErrNone )
        {
        retVal = aMessage.Read( aDataSlot, aBuffer );
        TRACE_INFO(( _L("Read returned %d"), retVal ));
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reads a struct from a message
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::ReadStructFromMessage( TDes8& aBuffer,
                                                 TInt aDataSlot,
                                                 const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    TInt desLength = aMessage.GetDesLength( aDataSlot );
    if ( desLength <= 0 )
        {
        TRACE_FUNC_EXIT
        return KErrArgument;
        }
    TInt retVal = aMessage.Read( aDataSlot, aBuffer );
    TRACE_INFO(( _L("Read returned %d"), retVal ));
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Writes specified input reply buffer to a client request message at given
// message slot number.
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::WriteReplyBufferToClient( const TDesC8& aBuffer,
                                                    TInt aDataSlot,
                                                    const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    TInt maxLength = aMessage.GetDesMaxLength( aDataSlot );
    if ( aBuffer.Length() > maxLength )
        {
        TRACE_FUNC_EXIT
        return KErrTooBig;
        }
    TInt retVal = aMessage.Write( aDataSlot, aBuffer );
    TRACE_INFO(( _L("Write returned %d"), retVal ));
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Destructs objects/associations for the current session
// ---------------------------------------------------------------------------
//
void CATExtCommonSession::Destruct( TBool aSyncClose )
    {
    TRACE_FUNC_ENTRY
    if ( iModeStatusMessage.Handle() )
        {
        iModeStatusMessage.Complete( KErrServerTerminated );
        }
    if ( iNvramStatusMessage.Handle() )
        {
        iNvramStatusMessage.Complete( KErrServerTerminated );
        }
    delete iCommonBase;
    iCommonBase = NULL;
    if ( iEComSession )
        {
        iEComSession->Close();
        }
    if ( !aSyncClose )
        {
        REComSession::FinalClose();
        iServer.ClientClosed( *this );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// From MATExtCommonObserver.
// Called by concrete extension Plugin to inform that mode status has changed.
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::SendModeStatusChange( TUint aMode )
    {
    TRACE_FUNC_ENTRY
    if ( !iModeStatusMessage.Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TUint> modePckg( aMode );
    TInt retTemp = iModeStatusMessage.Write( EATExtReceiveModeChangeParamMode,
                                             modePckg );
    TRACE_INFO(( _L("Write returned %d"), retTemp ));
    iModeStatusMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MATExtCommonObserver.
// Called by concrete extension plugin to inform that NVRAM status has
// changed.
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::SendNvramStatusChange( const TDesC8& aNvram )
    {
    TRACE_FUNC_ENTRY
    WriteNvramStatusToClient( aNvram, EATExtReceiveNvramChangeParamNvram );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MATExtCommonObserver.
// Called by the destructor of CATExtPluginBase.
// A concrete service provider implementation should not touch this.
// ---------------------------------------------------------------------------
//
TInt CATExtCommonSession::ATExtPluginClosed( CATExtCommonBase* /*aPlugin*/ )
    {
    TRACE_FUNC_ENTRY
    iCommonBase = NULL;
    TRACE_FUNC_EXIT
    return KErrNone;
    }
