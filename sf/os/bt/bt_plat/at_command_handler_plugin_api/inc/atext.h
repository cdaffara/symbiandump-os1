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
* Description:  AT extension client API
*
*/


#ifndef R_ATEXTCLIENT_H
#define R_ATEXTCLIENT_H

#include <e32base.h>
#include <atextpluginbase.h>

/**  Interface definitions; either DUN or HFP */
enum TATExtensionInterface
    {
    EDunATExtension,
    EHfpATExtension
    };

/**  ECOM notification type for plugins */
enum TATExtensionEcomType
    {
    EEcomTypeUninstall,
    EEcomTypeInstall
    };

/**
 *  Client of AT Extension.
 *
 *  @lib atextclient.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RATExt ) : public RSessionBase
    {

public:

    /**
     * Establishes a session with server and connect to corresponding extension
     * plugins specified by parameter TATExtensionInterface.
     *
     * @since S60 v5.0
     * @param aIf The AT extension interface
     * @param aName Connection identifier name
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt Connect( TATExtensionInterface aIf,
                           const TDesC8& aName );

    /**
     * Synchronously closes the session
     * Optional: client can do either Close() or SynchronousClose()+Close()
     *
     * @since S60 v5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt SynchronousClose();

    /**
     * Handles a command. This API supports two modes:
     * Normal mode (!EReplyTypeEditor) for normal use cases
     * Editor mode (EReplyTypeEditor) for special AT commands with text mode
     *
     * Notes for parameters below:
     * (1) Command to be handled is used in normal mode only.
     *     Editor mode is entered when the first EReplyTypeEditor reply is received (5).
     *     Editor mode is ended when the first !EReplyTypeEditor reply is received (5).
     * (2) Data received is character set independent; this API is called byte-by-byte
     *     in editor mode.
     * (3) The reply string is not used in editor mode's text input.
     *     Only editor mode's start condition (prompt) and end condition
     *     (!EReplyTypeEditor) handle the possibly existing reply string.
     * (4) The remaining reply length setting is not used in editor mode's text input.
     *     It is only used in editor mode's start condition (prompt) and end condition
     *     (!EReplyTypeEditor)
     * (5) Normal mode: Other than EReplyTypeEditor reply.
     *     Editor mode: Started with EReplyTypeEditor, continued with EReplyTypeEditor,
     *                  Ended with !EReplyTypeEditor
     *                  Note: the first reply with EReplyTypeEditor is the "prompt".
     *                  Subsequent replies with EReplyTypeEditor are ignored.
     *
     * Note: The editor mode supported by this API should only be used for
     * commands not requiring support for signals. These commands include cases
     * such as DCD signal required by command AT+CMGS.
     *
     * @since S60 v5.0
     * @param aStatus The request status
     * @param aCmd The command to be handled (1) or data for editor mode (2).
     * @param aReply The descriptor to hold reply to this command (3).
     * @param aRemainingReplyLength Tells the length of remaining reply if it is not 0;
     *                              the remainings can be fetched by GetNextPartOfReply() (4).
     * @param aReplyType Reply type for the handled AT command (1) or editor mode (2) (5).
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt HandleCommand( TRequestStatus& aStatus,
                                 const TDesC8& aCmd,
                                 TBuf8<KDefaultCmdBufLength>& aReply,
                                 TPckg<TInt>& aRemainingReplyLength,
                                 TPckg<TATExtensionReplyType>& aReplyType );

    /**
     * Cancels a command handling request.
     * Calling this API in editor mode also causes ATEXT to reset the information
     * about the plugin currently handling byte-by-byte data.
     * This causes the next command to be handled in normal mode.
     *
     * @since S60 v5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt CancelHandleCommand();

    /**
     * Gets the remaining reply of a previous handled command.
     *
     * @since S60 v5.0
     * @param aReply The descriptor to hold the reply;
     * @param aRemainingReplyLength tells the length of remaining reply if it is not 0;
     *                              the remainings can be fetched by GetNextPartOfReply().
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt GetNextPartOfReply( TBuf8<KDefaultCmdBufLength>& aReply,
                                      TInt& aRemainingReplyLength );

    /**
     * Number of plugins
     *
     * @since S60 v5.0
     * @return The number of plugins
     */
    IMPORT_C TInt NumberOfPlugins();

    /**
     * Receives unsolicited result code from extension plugins.
     *
     * @since S60 v5.0
     * @param aStatus The request status
     * @param aRecvBuffer The receiving buffer
     * @param aUrcPluginUid UID of the plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReceiveUnsolicitedResult( TRequestStatus& aStatus,
                                            TBuf8<KDefaultUrcBufLength>& aRecvBuffer,
                                            TPckg<TUid>& aUrcPluginUid );

    /**
     * Cancels a pending receiving request.
     *
     * @since S60 v5.0
     * @param aUrcPluginUid UID of the plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt CancelReceiveUnsolicitedResult( TUid& aUrcPluginUid );

    /**
     * Marks URC handling ownership.
     *
     * @since S60 v5.0
     * @param aUrcPluginUid UID of the plugin
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt MarkUrcHandlingOwnership( TUid& aUrcPluginUid );

    /**
     * Receives ECOM plugin uninstall/install/version reports
     *
     * @since S60 5.0
     * @param aStatus The request status
     * @param aPluginUid UID of the plugin
     * @param aEcomType Ecom type for the update
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReceiveEcomPluginChange(
        TRequestStatus& aStatus,
        TPckg<TUid>& aPluginUid,
        TPckg<TATExtensionEcomType>& aEcomType );

    /**
     * Cancels ECOM plugin uninstall/install/version report receiving
     *
     * @since S60 5.0
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt CancelReceiveEcomPluginChange();

    /**
     * Reports change to quiet mode.
     * The quiet mode is off by default.
     *
     * @since S60 5.0
     * @param aMode ETrue if quiet mode changed on, EFalse if off
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReportQuietModeChange( TBool aMode );

    /**
     * Reports change to verbose mode.
     * The verbose mode is on by default.
     *
     * @since S60 5.0
     * @param aMode ETrue if verbose mode changed on, EFalse if off
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReportVerboseModeChange( TBool aMode );

    /**
     * Reports change to a value of a character
     *
     * @since S60 5.0
     * @param aCharType New character's type
     * @param aNewChar New character value
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReportCharacterChange( TATExtensionCharType aCharType,
                                         TInt8 aNewChar );

    /**
     * Reporter for reporting when client side has updated its internal
     * information after ReceiveEcomPluginChange()
     *
     * @since S60 5.0
     * @param aPluginUid UID of the plugin
     * @param aUpdateType Type of update
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReportListenerUpdateReady( TUid& aPluginUid,
                                             TATExtensionEcomType aUpdateType );

    /**
     * Broadcasts NVRAM status change to the plugins.
     *
     * @since S60 5.0
     * @param aNvram New NVRAM status. The new settings (delimited by "|")
     *               must differ from the Hayes defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt BroadcastNvramStatusChange( const TDesC8& aNvram );

    /**
     * Reports about external handle command error condition.
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
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReportExternalHandleCommandError();

    /**
     * Reports about abort condition in command handling.
     * This is for cases when for example DUN decided an abort condition was
     * received from DTE (ITU-T V.250 5.6.1). This API is for notifying the
     * plugin that abort was requested. However the plugin currently handling
     * the command may ignore the request if it doesn't support abort for the
     * command or it may return the changed condition with
     * HandleCommandCompleted()
     *
     * @since S60 5.0
     * @param aStop If ETrue stop command line processing
     *              If EFalse continue command line processing
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt ReportHandleCommandAbort( TBool& aStop );

    /**
     * Gets the next special command RSS file entry. These commands are commands
     * with a matching start but with any ending sequence. Command "ATD*99***1#"
     * is one of these and the number of these commands should be kept minimal.
     *
     * @since S60 5.0
     * @param aCmd The next found special command
     * @param aFirstSearch If ETrue the search is done from the start,
     *                     If EFalse the search continues from the old position.
     *                     ATEXT sets this to EFalse after each successfull call.
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt GetNextSpecialCommand( TDes8& aCmd,
                                         TBool& aFirstSearch );

private:  // data

    /**
     * Connected interface
     */
    TATExtensionInterface iConnectedIf;

    };

#endif  // R_ATEXTCLIENT_H
