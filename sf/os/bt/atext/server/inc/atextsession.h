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
* Description:  The session object for ATEXT server
*
*/


#ifndef C_ATEXTSESSION_H
#define C_ATEXTSESSION_H

#include <e32svr.h>
#include "atextpluginobserver.h"
#include "atextmetadata.h"
#include "atextlisten.h"

class CATExtSrv;
class CDesC8ArraySeg;
class CATExtPluginBase;
class CATExtMetadata;

/**
 *  Manages separate ATEXT server sessions for client(s)
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CATExtSession ) : public CSession2,
                                     public MATExtPluginObserver,
                                     public MATExtListen
    {

friend class CATExtMetadata;

public:

    /**
     * Two-phased constructor.
     * @param aServer
     * @return Instance of self
     */
    static CATExtSession* NewL( CATExtSrv& aServer, const TVersion& aVersion );

    /**
    * Destructor.
    */
    ~CATExtSession();

private:

// from base class CSession2

    /**
     * From CSession2.
     * Handles the servicing of client requests passed to the server
     *
     * @since S60 5.0
     * @param aMessage Message containing the client request
     * @return None
     */
    void ServiceL( const RMessage2& aMessage );

    CATExtSession( CATExtSrv& aServer, const TVersion& aVersion );

    /**
     * Handles the servicing of client requests passed to the server
     *
     * @since S60 5.0
     * @param aMessage Message containing the client request
     * @return None
     */
    void DoServiceL( const RMessage2& aMessage );

    /**
     * Handles the servicing of setting up interface
     *
     * @since S60 5.0
     * @param aMessage Message for Connect() client call
     * @return None
     */
    void IpcSetInterfaceL( const RMessage2& aMessage );

    /**
     * Synchronously closes the session
     * Optional: client can do either Close() or SynchronousClose()+Close()
     *
     * @since S60 5.0
     * @param aMessage Message for SynchronousClose() client call
     * @return None
     */
    TInt IpcSynchronousClose( const RMessage2& aMessage );

    /**
     * Handles the servicing of AT commands
     *
     * @since S60 5.0
     * @param aMessage Message for HandleCommand() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcHandleCommand( const RMessage2& aMessage );

    /**
     * Handles the servicing of pending AT command cancel
     *
     * @since S60 5.0
     * @param aMessage Message for CancelHandleCommand() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcCancelHandleCommand( const RMessage2& aMessage );

    /**
     * Handles the servicing of getting the next part of a reply for
     * IpcHandleCommand().
     *
     * @since S60 5.0
     * @param aMessage Message for GetNextPartOfReply() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcGetNextPartOfReply( const RMessage2& aMessage );

    /**
     * Handles the servicing of number of plugins. This information is needed
     * to instantiate one or more listeners by the user of the client.
     *
     * @since S60 5.0
     * @param aMessage Message for NumberOfPlugins() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcNumberOfPlugins( const RMessage2& aMessage );

    /**
     * Handles the servicing of receiving unsolicited result codes. Note that
     * IpcMarkUrcHandlingOwnership() must be called immediately after this in
     * order for the message to receive their destination.
     *
     * @since S60 5.0
     * @param aMessage Message for ReceiveUnsolicitedResult() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReceiveUnsolicitedResult( const RMessage2& aMessage );

    /**
     * Handles the servicing of pending receiving unsolicited result code
     * cancel
     *
     * @since S60 5.0
     * @param aMessage Message for CancelReceiveUnsolicitedResult() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcCancelReceiveUrc( const RMessage2& aMessage );

    /**
     * Handles the servicing of marking URC handling ownership.
     * Call to this function must be done immediately after the call to
     * IpcReceiveUnsolicitedResult().
     *
     * @since S60 5.0
     * @param aMessage Message for MarkUrcHandlingOwnership() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcMarkUrcHandlingOwnership( const RMessage2& aMessage );

    /**
     * Handles the servicing of ECOM plugin change notifications
     *
     * @since S60 5.0
     * @param aMessage Message for ReceiveEcomPluginChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReceiveEcomPluginChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of pending ECOM plugin change notification cancel
     *
     * @since S60 5.0
     * @param aMessage Message for CancelReceiveEcomPluginChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcCancelReceiveEcomPluginChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of quiet mode change reporting.
     * When quiet mode is on the behavior is the same as with AT command
     * "ATQ1: Result code suppression". When quiet mode is on then verbose mode
     * setting has no effect.
     *
     * @since S60 5.0
     * @param aMessage Message for ReportQuietModeChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReportQuietModeChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of verbose mode change reporting.
     * When verbose mode is on the behavior is the same as with AT command
     * "ATV1: DCE Response format". If quiet mode is on then this setting has no
     * effect.
     *
     * @since S60 5.0
     * @param aMessage Message for ReportVerboseModeChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReportVerboseModeChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of character change reporting.
     * The change to carriage return, line feed and backspace characters is
     * reported to CATExtPluginBase's protected data so that the it can easily
     * be used by the plugin implementation.
     *
     * @since S60 5.0
     * @param aMessage Message for ReportCharacterChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReportCharacterChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of reporting listener update readiness.
     * Client side listener has to use this after it has updates its own
     * internal states after ECOM plugin change notification.
     *
     * @since S60 5.0
     * @param aMessage Message for ReportListenerUpdateReady() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReportListenerUpdateReady( const RMessage2& aMessage );

    /**
     * Handles the servicing of NVRAM status change broadcasting.
     * The NVRAM status change is broadcasted to all of the plugins.
     *
     * @since S60 5.0
     * @param aMessage Message for BroadcastNvramStatusChange() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcBroadcastNvramStatusChange( const RMessage2& aMessage );

    /**
     * Handles the servicing of reporting about external handle command error
     * condition.
     * This is for cases when for example DUN decided the reply contained an
     * error condition but the plugin is still handling the command internally.
     * Example: "AT+TEST;+TEST2" was given in command line; "AT+TEST" returns
     * non-EReplyTypeError condition and "AT+TEST2" returns EReplyTypeError.
     * As the plugin(s) returning the non-EReplyTypeError may still have some
     * ongoing operation then these plugins are notified about the external
     * EReplyTypeError in command line processing. It is to be noted that
     * HandleCommandCancel() is not sufficient to stop the processing as the
     * command handling has already finished.
     *
     * @since S60 5.0
     * @param aMessage Message for ReportExternalHandleCommandError() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReportExternalHandleCommandError( const RMessage2& aMessage );

    /**
     * Handles the servicing of reporting about abort condition in command
     * handling.
     * This is for cases when for example DUN decided an abort condition was
     * received from DTE (ITU-T V.250 5.6.1). This API is for notifying the
     * plugin that abort was requested. However the plugin currently handling
     * the command may ignore the request if it doesn't support abort for the
     * command or it may return the changed condition with
     * HandleCommandCompleted()
     *
     * @since S60 5.0
     * @param aMessage Message for ReportExternalHandleCommandError() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcReportHandleCommandAbort( const RMessage2& aMessage );

    /**
     * Handles the servicing of getting the next special command RSS file entry.
     * These commands are commands with a matching start but with any ending
     * sequence. Command "ATD*99***1#" is one of these and the number of thse
     * commands should be kept minimal.
     *
     * @since S60 5.0
     * @param aMessage Message for GetNextSpecialCommand() client call
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt IpcGetNextSpecialCommand( const RMessage2& aMessage );

    /**
     * Reads a string from a message
     *
     * @since S60 5.0
     * @param aBuffer Destination buffer to which to read the string
     * @param aDataSlot Slot for the aMessage from where to read the string
     * @param aMessage Message from where the string is to be read
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReadStringFromMessage( RBuf8& aBuffer,
                                TInt aDataSlot,
                                const RMessage2& aMessage );

    /**
     * Reads a struct from a message
     *
     * @since S60 5.0
     * @param aBuffer Destination buffer to which to read the struct
     * @param aDataSlot Slot for the aMessage from where to read the struct
     * @param aMessage Message from where the struct is to be read
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ReadStructFromMessage( TDes8& aBuffer,
                                TInt aDataSlot,
                                const RMessage2& aMessage );

    /**
     * Destructs objects/associations for the current session
     *
     * @since S60 5.0
     * @param aSyncClose ETrue if synchronous close, EFalse if Close()
     * @return None
     */
    void Destruct( TBool aSyncClose );

// from base class MATExtPluginObserver

    /**
     * From MATExtPluginObserver.
     * Called by extension plugins when there is an unsolicited result code
     * should be sent to destination.
     *
     * Each AT Extension Plugin is responsible to format
     * result codes properly, e.g. in BT HFP case, the
     * format should be <cr><lf><result code><cr><lf>
     *
     * @since S60 v5.0
     * @param aPlugin Plugin from which the URC comes
     * @param aAT the unsolicited result code to be sent.
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt SendUnsolicitedResult( CATExtPluginBase* aPlugin, const TDesC8& aAT );

    /**
     * From MATExtPluginObserver.
     * Called by Extension Plugins to inform ATEXT that a command
     * handling has been completed or rejected.
     *
     * @since S60 v5.0
     * @param aPlugin Plugin from which the completion comes
     * @param aError Error code for completion
     * @param aReplyType Reply type for the handled AT command
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt HandleCommandCompleted( CATExtPluginBase* aPlugin,
                                 TInt aError,
                                 TATExtensionReplyType aReplyType );

    /**
     * From MATExtPluginObserver.
     * Called by concrete extension plugin to inform the array of supported
     * commands should be returned
     *
     * @since S60 5.0
     * @param aPlugin Plugin from which the completion comes
     * @param aCmd Array of supported commands
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt GetSupportedCommands( CATExtPluginBase* aPlugin,
                               RPointerArray<HBufC8>& aCmds );

    /**
     * From MATExtPluginObserver.
     * Called by the destructor of CATExtPluginBase.
     * A concrete service provider implementation should not touch this.
     *
     * @param aPlugin Plugin from which the closing comes
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt ATExtPluginClosed( CATExtPluginBase* aPlugin );

// from base class MATExtListen

    /**
     * From MATExtListen.
     * Notifies about an installed plugin.
     *
     * @since S60 5.0
     * @param aPluginUid UID of installed plugin's interface
     * @param aPluginUid UID of installed plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt NotifyPluginInstallation( TUid& aIfUid,
                                   CImplementationInformation* aImplInfo );

    /**
     * From MATExtListen.
     * Notifies about an uninstalled plugin.
     *
     * @since S60 5.0
     * @param aPluginUid UID of uninstalled plugin's interface
     * @param aPluginUid UID of uninstalled plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt NotifyPluginUninstallation( TUid& aIfUid,
                                     TUid& aPluginUid );

private:  // data

    /**
     * The server
     */
    CATExtSrv& iServer;

    /**
     * Session to ECOM
     */
    REComSession* iEComSession;

    /**
     * ECOM listener
     * Own.
     */
    CATExtListen* iListener;

    /**
     * Metadata
     * Own.
     */
    CATExtMetadata* iMetadata;

    /**
     * Version of server
     */
    TVersion iVersion;

    /**
     * ECOM status message
     */
    RMessage2 iEcomStatusMessage;

    };

#endif  // C_ATEXTSESSION_H
