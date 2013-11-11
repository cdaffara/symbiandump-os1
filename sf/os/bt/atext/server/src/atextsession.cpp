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
*
*/


#include <badesca.h>
#include <atextpluginbase.h>
#include <atext.h>
#include "atextsrv.h"
#include "atextclientsrv.h"
#include "atextsession.h"
#include "utils.h"
#include "debug.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CATExtSession* CATExtSession::NewL( CATExtSrv& aServer,
                                    const TVersion& aVersion )
    {
    return new (ELeave) CATExtSession( aServer, aVersion );
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CATExtSession::~CATExtSession()
    {
    TRACE_FUNC_ENTRY
    Destruct( EFalse );
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Handles the servicing of client requests passed to the server
// ---------------------------------------------------------------------------
//
void CATExtSession::ServiceL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( iVersion.iMajor==KServerMajorVersionNumber &&
         iVersion.iMinor==KServerMinorVersionNumber &&
         iVersion.iBuild==KServerBuildVersionNumber )
        {
        DoServiceL( aMessage );
        }
    else
        {
        aMessage.Complete( KErrNotSupported );
        }
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Handles the servicing of client requests passed to the server
// ---------------------------------------------------------------------------
//
void CATExtSession::DoServiceL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    switch ( aMessage.Function() )
        {
        case EATExtSetExtensionInterface:
            {
            TRAPD( retTrap, IpcSetInterfaceL(aMessage) );
            if ( retTrap != KErrNone )
                {
                aMessage.Complete( retTrap );
                }
            else if ( !iMetadata->SupportExists() )
                {
                aMessage.Complete( KErrNotFound );
                }
            else
                {
                aMessage.Complete( KErrNone );
                }
            break;
            }
        case EATExtSynchronousClose:
            {
            IpcSynchronousClose( aMessage );
            }
            break;
        case EATExtHandleCommand:
            {
            IpcHandleCommand( aMessage );
            break;
            }
        case EATExtCancelHandleCommand:
            {
            IpcCancelHandleCommand( aMessage );
            break;
            }
        case EATExtGetNextPartOfReply:
            {
            IpcGetNextPartOfReply( aMessage );
            break;
            }
        case EATExtNumberOfPlugins:
            {
            IpcNumberOfPlugins( aMessage );
            }
            break;
        case EATExtReceiveUnsolicitedResult:
            {
            IpcReceiveUnsolicitedResult( aMessage );
            break;
            }
        case EATExtCancelReceiveUnsolicitedResult:
            {
            IpcCancelReceiveUrc( aMessage );
            break;
            }
        case EATExtMarkUrcHandlingOwnership:
            {
            IpcMarkUrcHandlingOwnership( aMessage );
            break;
            }
        case EATExtReceiveEcomPluginChange:
            {
            IpcReceiveEcomPluginChange( aMessage );
            }
            break;
        case EATExtCancelReceiveEcomPluginChange:
            {
            IpcCancelReceiveEcomPluginChange( aMessage );
            }
            break;
        case EATExtReportQuietModeChange:
            {
            IpcReportQuietModeChange( aMessage );
            }
            break;
        case EATExtReportVerboseModeChange:
            {
            IpcReportVerboseModeChange( aMessage );
            }
            break;
        case EATExtReportCharacterChange:
            {
            IpcReportCharacterChange( aMessage );
            }
            break;
        case EATExtReportListenerUpdateReady:
            {
            IpcReportListenerUpdateReady( aMessage );
            }
            break;
        case EATExtBroadcastNvramStatusChange:
            {
            IpcBroadcastNvramStatusChange( aMessage );
            }
            break;
        case EATExtReportExternalHandleCommandError:
            {
            IpcReportExternalHandleCommandError( aMessage );
            }
            break;
        case EATExtReportHandleCommandAbort:
            {
            IpcReportHandleCommandAbort( aMessage );
            }
            break;
        case EATExtGetNextSpecialCommand:
            {
            IpcGetNextSpecialCommand( aMessage );
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
// CATExtSession::CATExtSession
// ---------------------------------------------------------------------------
//
CATExtSession::CATExtSession( CATExtSrv& aServer, const TVersion& aVersion ) :
    iServer( aServer ),
    iEComSession( NULL ),
    iListener( NULL ),
    iMetadata( NULL ),
    iVersion( aVersion )
    {
    }

// ---------------------------------------------------------------------------
// Handles the servicing of setting up interface
// ---------------------------------------------------------------------------
//
void CATExtSession::IpcSetInterfaceL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( iMetadata || iListener )
        {
        TRACE_FUNC_EXIT
        User::Leave( KErrGeneral );
        }
    iEComSession = &REComSession::OpenL();
    TPckgBuf<TUid> ifUidPckgBuf;
    TInt retTemp = ReadStructFromMessage( ifUidPckgBuf,
                                          EATExtConnectParamUid,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        User::Leave( retTemp );
        }
    RBuf8 connectionName;
    CleanupClosePushL( connectionName );
    retTemp = ReadStringFromMessage( connectionName,
                                     EATExtConnectParamName,
                                     aMessage );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        User::Leave( retTemp );
        }
    // Create listener
    CATExtListen* listener = CATExtListen::NewLC( *iEComSession, this );
    listener->AddInterfaceUid( ifUidPckgBuf() );
    // Create metadata. Pass iListener to add the UIDs
    CATExtMetadata* metadata = CATExtMetadata::NewLC( *iEComSession,
                                                      listener,
                                                      *this );
    metadata->CreateImplementationMetadataL( ifUidPckgBuf(), connectionName );
    listener->IssueRequest();
    CleanupStack::Pop( metadata );
    CleanupStack::Pop( listener );
    CleanupStack::PopAndDestroy( &connectionName );
    iListener = listener;
    iMetadata = metadata;
    TRACE_FUNC_EXIT
    }

// ---------------------------------------------------------------------------
// Synchronously closes the session
// Optional: client can do either Close() or SynchronousClose()
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcSynchronousClose( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    Destruct( ETrue );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of AT commands
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcHandleCommand( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( aMessage.GetDesMaxLength(EATExtHandleCmdParamReply) !=
         KDefaultCmdBufLength )
        {
        aMessage.Complete( KErrBadDescriptor );
        TRACE_FUNC_EXIT
        return KErrBadDescriptor;
        }
    TATExtCompletionInfo complInfo;
    complInfo.iProcessed = EFalse;
    complInfo.iReplyExpected = EFalse;
    TInt retTemp = iMetadata->HandleCommand( aMessage, complInfo );
    if ( retTemp != KErrNone )
        {
        TRACE_INFO(( _L8("Command handled with failure %d"), retTemp ));
        iMetadata->CompleteCommandMessage( NULL,
                                           retTemp,
                                           EFalse,               // no error reply
                                           EReplyTypeUndefined,  // general error condition
                                           EFalse );             // no multipart
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // Next check the case without support: in this case a reply
    // can't be expected but return the "ERROR" anyway if needed.
    if ( !complInfo.iProcessed )
        {
        // Return "ERROR".
        TRACE_INFO(( _L8("Command handled without support: return \"ERROR\"") ));
        iMetadata->CompleteCommandMessage( NULL,
                                           KErrNone,
                                           ETrue,            // error reply
                                           EReplyTypeError,  // error type
                                           EFalse );         // no multipart
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // Third, check a case where there is support but reply is not
    // expected. In this case "" must be returned to complete processing.
    // Note: The EFalse setting is used only for normal mode.
    if ( !complInfo.iReplyExpected )
        {
        // Return ""
        TRACE_INFO(( _L8("Command handled with support but no reply: return \"\"") ));
        iMetadata->CompleteCommandMessage( NULL,
                                           KErrNone,
                                           EFalse,           // no error reply
                                           EReplyTypeOther,  // reply type from plugin
                                           EFalse );         // no multipart
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // The rest are for known command with reply or data in editor mode.
    // This case is handled in HandleCommand().
    TRACE_INFO(( _L8("Command handled: wait for asynchronous reply or do nothing") ));
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of pending AT command cancel
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcCancelHandleCommand( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->CancelHandleCommand();
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of getting the next part of a reply for
// IpcHandleCommand().
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcGetNextPartOfReply( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retTemp = iMetadata->GetNextPartOfReply( aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // Note: Completed in metadata if successfull
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of number of plugins. This information is needed to
// instantiate one or more listeners by the user of the client.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcNumberOfPlugins( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->NumberOfPlugins();
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of receiving unsolicited result codes. Note that
// IpcMarkUrcHandlingOwnership() must be called immediately after this in
// order for the message to receive their destination.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReceiveUnsolicitedResult( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( aMessage.GetDesMaxLength(EATExtReceiveUrcCmdParamBuf) !=
         KDefaultUrcBufLength )
        {
        aMessage.Complete( KErrBadDescriptor );
        TRACE_FUNC_EXIT
        return KErrBadDescriptor;
        }
    TPckgBuf<TUid> pluginUidPckgBuf;
    TInt retTemp = ReadStructFromMessage( pluginUidPckgBuf,
                                          EATExtReceiveUrcCmdParamUid,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TRAPD( retTrap, iMetadata->StartUrcReceivingL(aMessage,pluginUidPckgBuf()) );
    if ( retTrap != KErrNone )
        {
        aMessage.Complete( retTrap );
        TRACE_FUNC_EXIT
        return retTrap;
        }
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of pending receiving unsolicited result code cancel
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcCancelReceiveUrc( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TPckgBuf<TUid> pluginUidPckgBuf;
    TInt retTemp = ReadStructFromMessage( pluginUidPckgBuf,
                                          EATExtCancelReceiveUrcCmdParamUid,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TInt retVal = iMetadata->CancelUrcReceiving( pluginUidPckgBuf() );
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of marking URC handling ownership. Call to this
// function must be done immediately after the call to
// IpcReceiveUnsolicitedResult().
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcMarkUrcHandlingOwnership( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->MarkUrcHandlingOwnership( aMessage );
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of ECOM plugin change notifications
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReceiveEcomPluginChange( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( iEcomStatusMessage.Handle() )
        {
        aMessage.Complete( KErrBadHandle );
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    iEcomStatusMessage = aMessage;
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of pending ECOM plugin change notification cancel
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcCancelReceiveEcomPluginChange(
    const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    if ( !iEcomStatusMessage.Handle() )
        {
        aMessage.Complete( KErrBadHandle );
        TRACE_FUNC_EXIT
        return KErrBadHandle;
        }
    iEcomStatusMessage.Complete( KErrCancel );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of quiet mode change reporting. When quiet mode is on
// the behavior is the same as with AT command "ATQ1: Result code
// suppression". When quiet mode is on then verbose mode setting has no
// effect.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReportQuietModeChange( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TPckgBuf<TBool> quietModePckgBuf;
    TInt retTemp = ReadStructFromMessage( quietModePckgBuf,
                                          EATExtReportQuietModeChangeParamMode,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    iMetadata->SetQuietMode( quietModePckgBuf() );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of verbose mode change reporting. When verbose mode
// is on the behavior is the same as with AT command "ATV1: DCE Response
// format". If quiet mode is on then this setting has no effect.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReportVerboseModeChange( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TPckgBuf<TBool> verboseModePckgBuf;
    TInt retTemp = ReadStructFromMessage( verboseModePckgBuf,
                                          EATExtReportVerboseModeChangeParamMode,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    iMetadata->SetVerboseMode( verboseModePckgBuf() );
    aMessage.Complete( KErrNone );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of character change reporting. The change to carriage
// return, line feed and backspace characters is reported to
// CATExtPluginBase's protected data so that the it can easily be used by the
// plugin implementation.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReportCharacterChange( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TPckgBuf<TATExtensionCharType> charTypePckgBuf;
    TInt retTemp = ReadStructFromMessage( charTypePckgBuf,
                                          EATExtReportCharacterChangeParamType,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TPckgBuf<TInt8> charPckgBuf;
    retTemp = ReadStructFromMessage( charPckgBuf,
                                     EATExtReportCharacterChangeParamChar,
                                     aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TInt retVal = iMetadata->SetCharacterValue( charTypePckgBuf(),
                                                charPckgBuf() );
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of reporting listener update readiness. Client side
// listener has to use this after it has updates its own internal states after
// ECOM plugin change notification.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReportListenerUpdateReady( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TPckgBuf<TUid> pluginUidPckgBuf;
    TInt retTemp = ReadStructFromMessage( pluginUidPckgBuf,
                                          EATExtReportListenerUpdateParamUid,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TPckgBuf<TATExtensionEcomType> updateTypePckgBuf;
    retTemp = ReadStructFromMessage( updateTypePckgBuf,
                                     EATExtReportListenerUpdateParamType,
                                     aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TInt retVal = KErrNone;
    switch ( updateTypePckgBuf() )
        {
        case EEcomTypeUninstall:
            // 3. When client notifies of removal, remove the locked entry from metadata.
            // See also NotifyPluginUninstallation()
            retVal = iMetadata->RemoveImplementation( pluginUidPckgBuf() );
            break;
        case EEcomTypeInstall:
            // 4. When client notifies installation, remove the lock from metadata.
            // See also NotifyPluginInstallation()
            retVal = iMetadata->UnlockPluginAccess( pluginUidPckgBuf() );
            break;
        default:
            retVal = KErrNotSupported;
            break;
        }
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of NVRAM status change broadcasting. The NVRAM status
// change is broadcasted to all of the plugins.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcBroadcastNvramStatusChange( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    RBuf8 nvramStatus;
    TInt retTemp = ReadStringFromMessage( nvramStatus,
                                          EATExtBroadcastNvramChangeParamNvram,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        nvramStatus.Close();
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TInt retVal = iMetadata->BroadcastNvramStatusChange( nvramStatus );
    aMessage.Complete( retVal );
    nvramStatus.Close();
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of reporting about external handle command error
// condition.
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
TInt CATExtSession::IpcReportExternalHandleCommandError(
    const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->ReportExternalHandleCommandError();
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of reporting about abort condition in command
// handling.
// This is for cases when for example DUN decided an abort condition was
// received from DTE (ITU-T V.250 5.6.1). This API is for notifying the
// plugin that abort was requested. However the plugin currently handling
// the command may ignore the request if it doesn't support abort for the
// command or it may return the changed condition with
// HandleCommandCompleted()
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcReportHandleCommandAbort( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->ReportHandleCommandAbort( aMessage );
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Handles the servicing of getting the next special command RSS file entry.
// These commands are commands with a matching start but with any ending
// sequence. Command "ATD*99***1#" is one of these and the number of these
// commands should be kept minimal.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::IpcGetNextSpecialCommand( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        aMessage.Complete( KErrGeneral );
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TPckgBuf<TBool> firstSearchPckgBuf;
    TInt retTemp = ReadStructFromMessage( firstSearchPckgBuf,
                                          EATExtGetNextSpecialCmdParamFirst,
                                          aMessage );
    if ( retTemp != KErrNone )
        {
        aMessage.Complete( retTemp );
        TRACE_FUNC_EXIT
        return retTemp;
        }
    TInt retVal = iMetadata->GetNextSpecialCommand( aMessage,
                                                    firstSearchPckgBuf() );
    aMessage.Complete( retVal );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Reads a string from a message
// ---------------------------------------------------------------------------
//
TInt CATExtSession::ReadStringFromMessage( RBuf8& aBuffer,
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
TInt CATExtSession::ReadStructFromMessage( TDes8& aBuffer,
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
    retVal = aMessage.Read( aDataSlot, aBuffer );
    TRACE_INFO(( _L("Read returned %d"), retVal ));
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// Destructs objects/associations for the current session
// ---------------------------------------------------------------------------
//
void CATExtSession::Destruct( TBool aSyncClose )
    {
    TRACE_FUNC_ENTRY
    if ( iEcomStatusMessage.Handle() )
        {
        iEcomStatusMessage.Complete( KErrServerTerminated );
        }
    delete iListener;
    iListener = NULL;
    delete iMetadata;
    iMetadata = NULL;
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
// From MATExtPluginObserver.
// Called by extension plugins when there is an unsolicited result code
// should be sent to destination.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::SendUnsolicitedResult( CATExtPluginBase* aPlugin,
                                           const TDesC8& aAT )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    iMetadata->CompleteUrcMessage( aAT, aPlugin );
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MATExtPluginObserver.
// Called by Extension Plugins to inform ATEXT that a command handling has
// been completed or rejected.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::HandleCommandCompleted( CATExtPluginBase* aPlugin,
                                            TInt aError,
                                            TATExtensionReplyType aReplyType )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->CompleteCommandMessage( aPlugin,
                                                     aError,
                                                     EFalse,      // no error reply
                                                     aReplyType,  // type of reply
                                                     ETrue );     // multipart
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// From MATExtPluginObserver.
// Called by concrete extension plugin to inform the array of supported
// commands should be returned
// ---------------------------------------------------------------------------
//
TInt CATExtSession::GetSupportedCommands( CATExtPluginBase* aPlugin,
                                          RPointerArray<HBufC8>& aCmds )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->GetSupportedCommands( aPlugin, aCmds );
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// From MATExtPluginObserver.
// Called by the destructor of CATExtPluginBase. A concrete service provider
// implementation should not touch this.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::ATExtPluginClosed( CATExtPluginBase* aPlugin )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    TInt retVal = iMetadata->RemoveImplementationInstance( aPlugin );
    // "delete this" used in plugin. Plugin must cancel asynchronous
    // operations in its destructor to complete iCommandMessage and
    // iUnsolicitedMessage
    TRACE_FUNC_EXIT
    return retVal;
    }

// ---------------------------------------------------------------------------
// From class MATExtListen.
// Notifies about an installed plugin.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::NotifyPluginInstallation(
    TUid& /*aIfUid*/,
    CImplementationInformation* aImplInfo )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata || !aImplInfo )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    // 1. Add the new UID to metadata.
    TRAPD( retTrap, iMetadata->AddImplementationL( aImplInfo ) );
    if ( retTrap != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTrap;
        }
    // 2. Lock the entry in metadata.
    TUid pluginUid = aImplInfo->ImplementationUid();
    TInt retTemp = iMetadata->LockPluginAccess( pluginUid );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // 3. Notify client of installation.
    if ( iEcomStatusMessage.Handle() )
        {
        TPckg<TUid> pluginUidPckg( pluginUid );
        retTemp = iEcomStatusMessage.Write(
            EATExtReceiveEcomPluginChangeParamUid,
            pluginUidPckg );
        TRACE_INFO(( _L("Write returned %d"), retTemp ));
        TPckg<TATExtensionEcomType> ecomType( EEcomTypeInstall );
        retTemp = iEcomStatusMessage.Write(
            EATExtReceiveEcomPluginChangeParamType,
            ecomType );
        TRACE_INFO(( _L("Write returned %d"), retTemp ));
        iEcomStatusMessage.Complete( KErrNone );
        }
    // 4. When client notifies installation, remove the lock from metadata.
    // See IpcReportListenerUpdateReady() for this.
    TRACE_FUNC_EXIT
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MATExtListen.
// Notifies about an uninstalled plugin.
// ---------------------------------------------------------------------------
//
TInt CATExtSession::NotifyPluginUninstallation( TUid& /*aIfUid*/,
                                                TUid& aPluginUid )
    {
    TRACE_FUNC_ENTRY
    if ( !iMetadata )
        {
        TRACE_FUNC_EXIT
        return KErrGeneral;
        }
    // 1. Lock the access to metadata from the responsible plugin UID.
    TInt retTemp = iMetadata->LockPluginAccess( aPluginUid );
    if ( retTemp != KErrNone )
        {
        TRACE_FUNC_EXIT
        return retTemp;
        }
    // 2. Notify client of removal.
    if ( iEcomStatusMessage.Handle() )
        {
        TPckg<TUid> pluginUid( aPluginUid );
        retTemp = iEcomStatusMessage.Write(
            EATExtReceiveEcomPluginChangeParamUid,
            pluginUid );
        TRACE_INFO(( _L("Write returned %d"), retTemp ));
        TPckg<TATExtensionEcomType> ecomType( EEcomTypeUninstall );
        retTemp = iEcomStatusMessage.Write(
            EATExtReceiveEcomPluginChangeParamType,
            ecomType );
        TRACE_INFO(( _L("Write returned %d"), retTemp ));
        iEcomStatusMessage.Complete( KErrNone );
        }
    // 3. When client notifies of removal, remove the locked entry from metadata.
    // See IpcReportListenerUpdateReady() for this.
    TRACE_FUNC_EXIT
    return KErrNone;
    }
