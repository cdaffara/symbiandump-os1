/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AT extension client implementation
*
*/


#include <atext.h>
#include "atextclientsrv.h"
#include "atext_interfaceuid.h"
#include "debug.h"

_LIT( KATExtSrvExe, "atext.exe" );

const TUid KATExtSrvUid3 = { 0x2001CBEB };

static TInt StartServer();

// ---------------------------------------------------------------------------
// Establishes a session with server and connect to corresponding extension
// plugins specified by parameter TATExtensionInterface.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::Connect( TATExtensionInterface aIf,
                               const TDesC8& aName )
    {
    TRACE_FUNC_ENTRY
    TVersion version( KServerMajorVersionNumber,
                      KServerMinorVersionNumber,
                      KServerBuildVersionNumber );

    TInt retVal = CreateSession( KATExtSrvName, version );
    if ( retVal!=KErrNone && retVal!=KErrAlreadyExists )
        {
        retVal = StartServer();
        if ( retVal==KErrNone || retVal==KErrAlreadyExists )
            {
            retVal = CreateSession( KATExtSrvName, version );
            }
        }

    TRACE_INFO((_L("ret %d "), retVal))
    if ( retVal==KErrNone || retVal==KErrAlreadyExists )
        {
        TUid ifUid = TUid::Null();
        if ( aIf == EHfpATExtension )
            {
            ifUid.iUid = ATEXT_INTERFACE_HFP_UID;
            }
        else if ( aIf == EDunATExtension )
            {
            ifUid.iUid = ATEXT_INTERFACE_DUN_UID;
            }
        TPckg<TUid> ifUidPckg( ifUid );
        retVal = SendReceive( EATExtSetExtensionInterface,
                              TIpcArgs(&ifUidPckg,&aName) );
        }
    if ( retVal != KErrNone )
        {
        Close();
        }
    else
        {
        iConnectedIf = aIf;
        }
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Synchronously closes the session
// Optional: client can do either Close() or SynchronousClose()+Close()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::SynchronousClose()
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
// Handles a command.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::HandleCommand(
    TRequestStatus& aStatus,
    const TDesC8& aCmd,
    TBuf8<KDefaultCmdBufLength>& aReply,
    TPckg<TInt>& aRemainingReplyLength,
    TPckg<TATExtensionReplyType>& aReplyType )
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
    aStatus = KRequestPending;
    SendReceive( EATExtHandleCommand,
                 TIpcArgs(&aCmd,&aReply,&aRemainingReplyLength,&aReplyType),
                 aStatus );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels a command handling request.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::CancelHandleCommand()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtCancelHandleCommand );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Gets the remaining reply of a previous handled command.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::GetNextPartOfReply( TBuf8<KDefaultCmdBufLength>& aReply,
                                          TInt& aRemainingReplyLength )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TInt> remainingLength( aRemainingReplyLength );
    TInt retVal = SendReceive( EATExtGetNextPartOfReply,
                               TIpcArgs(&aReply,&remainingLength) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Number of plugins
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::NumberOfPlugins()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtNumberOfPlugins );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Receives unsolicited result code from extension plugins.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReceiveUnsolicitedResult( TRequestStatus& aStatus,
                                                TBuf8<KDefaultUrcBufLength>& aRecvBuffer,
                                                TPckg<TUid>& aUrcPluginUid )
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
    aStatus = KRequestPending;
    SendReceive( EATExtReceiveUnsolicitedResult,
                 TIpcArgs(&aRecvBuffer,&aUrcPluginUid),
                 aStatus );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels a pending receiving request.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::CancelReceiveUnsolicitedResult( TUid& aUrcPluginUid )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TUid> urcPluginUid( aUrcPluginUid );
    TInt retVal = SendReceive( EATExtCancelReceiveUnsolicitedResult,
                               TIpcArgs(&urcPluginUid) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Marks URC handling ownership.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::MarkUrcHandlingOwnership( TUid& aUrcPluginUid )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TUid> urcPluginUid( aUrcPluginUid );
    TInt retVal = SendReceive( EATExtMarkUrcHandlingOwnership,
                               TIpcArgs(&urcPluginUid) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Receives ECOM plugin uninstall/install/version reports
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReceiveEcomPluginChange(
    TRequestStatus& aStatus,
    TPckg<TUid>& aPluginUid,
    TPckg<TATExtensionEcomType>& aEcomType )
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
    aStatus = KRequestPending;
    SendReceive( EATExtReceiveEcomPluginChange,
                 TIpcArgs(&aPluginUid,&aEcomType),
                 aStatus );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Cancels ECOM plugin uninstall/install/version report receiving
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::CancelReceiveEcomPluginChange()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtCancelReceiveEcomPluginChange );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reports change to quiet mode
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReportQuietModeChange( TBool aMode )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TBool> mode( aMode );
    TInt retVal = SendReceive( EATExtReportQuietModeChange,
                               TIpcArgs(&mode) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reports change to verbose mode
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReportVerboseModeChange( TBool aMode )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TBool> mode( aMode );
    TInt retVal = SendReceive( EATExtReportVerboseModeChange,
                               TIpcArgs(&mode) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reports change to a value of a character
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReportCharacterChange( TATExtensionCharType aCharType,
                                             TInt8 aNewChar )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TATExtensionCharType> charType( aCharType );
    TPckg<TInt8> newChar( aNewChar );
    TInt retVal = SendReceive( EATExtReportCharacterChange,
                               TIpcArgs(&charType,&newChar) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reporter for reporting when client side has updated its internal
// information after ReceiveEcomPluginChange()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReportListenerUpdateReady(
    TUid& aPluginUid,
    TATExtensionEcomType aUpdateType )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TUid> pluginUid( aPluginUid );
    TPckg<TATExtensionEcomType> updateType( aUpdateType );
    TInt retVal = SendReceive( EATExtReportListenerUpdateReady,
                               TIpcArgs(&pluginUid,&updateType) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Broadcasts NVRAM status change to the plugins.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::BroadcastNvramStatusChange( const TDesC8& aNvram )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtBroadcastNvramStatusChange,
                               TIpcArgs(&aNvram) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reports about external handle command error condition.
// This is for cases when for example DUN decided the reply contained an
// error condition but the plugin is still handling the command internally.
// Example: "AT+TEST;+TEST2" was given in command line; "AT+TEST" returns
// non-EReplyTypeError condition and "AT+TEST2" returns EReplyTypeError.
// As the plugin(s) returning the non-EReplyTypeError may still have some
// ongoing operation then these plugins are notified about the external
// EReplyTypeError in command line processing. It is to be noted that
// HandleCommandCancel() is not sufficient to stop the processing as the
// command handling has already finished.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReportExternalHandleCommandError()
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TInt retVal = SendReceive( EATExtReportExternalHandleCommandError );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reports about abort condition in command handling.
// This is for cases when for example DUN decided an abort condition was
// received from DTE (ITU-T V.250 5.6.1). This API is for notifying the
// plugin that abort was requested. However the plugin currently handling
// the command may ignore the request if it doesn't support abort for the
// command or it may return the changed condition with
// HandleCommandCompleted()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::ReportHandleCommandAbort( TBool& aStop )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TBool> stop( aStop );
    TInt retVal = SendReceive( EATExtReportHandleCommandAbort,
                               TIpcArgs(&stop) );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Gets the next special command RSS file entry. These commands are commands
// with a matching start but with any ending sequence. Command "ATD*99***1#"
// is one of these and the number of these commands should be kept minimal.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RATExt::GetNextSpecialCommand( TDes8& aCmd,
                                             TBool& aFirstSearch )
    {
    TRACE_FUNC_ENTRY
    if ( !Handle() )
        {
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    TPckg<TBool> firstSearch( aFirstSearch );
    TInt retVal = SendReceive( EATExtGetNextSpecialCommand,
                               TIpcArgs(&aCmd,&firstSearch) );
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
    TInt retTemp = server.Create( KATExtSrvExe, KNullDesC, serverUid );
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
