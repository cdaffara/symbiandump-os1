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
* Description:  AT extension plugin API
*
*/


#ifndef C_ATEXTPLUGINBASE_H
#define C_ATEXTPLUGINBASE_H

#include <ecom/ecom.h>

/**  Character types for character change notifications */
enum TATExtensionCharType
    {
    ECharTypeCarriage,
    ECharTypeLineFeed,
    ECharTypeBackspace
    };

/**  Reply types for the handled AT command */
enum TATExtensionReplyType
    {
    EReplyTypeUndefined,  // For error conditions (handling failed)
    EReplyTypeOther,      // !(EReplyTypeOk||EReplyTypeError||EReplyTypeEditor)
    EReplyTypeOk,         // For "OK" (or "" in quiet mode)
    EReplyTypeError,      // For "ERROR" (or "" in quiet mode)
    EReplyTypeEditor,     // For editor mode
    };

/**  Default buffer length for command reply buffer */
const TUint KDefaultCmdBufLength = 1024;

/**  Default buffer length for URC reply buffer */
const TUint KDefaultUrcBufLength = 512;

class MATExtPluginObserver;

/**
 *  The base class for implementing AT Extension Plugin.
 *
 *  A concrete AT Extension Plugin must implement this class as an ECom plugin,
 *  and it must be ready once it has been instantiated.
 *
 *  The interface UID of the plugin resource must use the value defined in
 *  atext_interfaceuid.h.
 *
 *  @lib atextpluginbase.lib
 *  @since S60 v5.0
 */
class CATExtPluginBase : public CBase
    {

public:

    /**
     * Instantiate the AT Extension Plugin via ECom framework.
     *
     * @since S60 5.0
     * @param aImplementationUid The implementation UID of the plugin
     * @param aObserver The reference of ATEXT observer interface
     * @param aName Connection name to report to extension plugin
     * @return Pointer of the instantiated plugin
     */
	IMPORT_C static CATExtPluginBase* NewL( TUid aImplementationUid,
	                                        MATExtPluginObserver& aObserver,
	                                        const TDesC8& aName );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CATExtPluginBase();

    /**
     * Reports change to quiet mode.
     * The quiet mode is off by default.
     *
     * @since S60 5.0
     * @param aMode ETrue if quiet mode changed on, EFalse if off
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C void ReportQuietModeChange( TBool aMode );

    /**
     * Reports change to verbose mode.
     * The verbose mode is on by default.
     *
     * @since S60 5.0
     * @param aMode ETrue if verbose mode changed on, EFalse if off
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C void ReportVerboseModeChange( TBool aMode );

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
     * Reports connection identifier name to the extension plugin.
     *
     * @since S60 5.0
     * @param aName Connection identifier name
     * @return None
     */
    virtual void ReportConnectionName( const TDesC8& aName ) = 0;

    /**
     * Reports the support status of an AT command. This is a synchronous API.
     *
     * @param aCmd The AT command. Its format may vary depending on the
     *             specification. E.g. in BT HFP case, the command may contain
     *             a character carriage return (<cr>) in the end.
     * @return ETrue if the command is supported; EFalse otherwise.
     */
    virtual TBool IsCommandSupported( const TDesC8& aCmd ) = 0;

    /**
     * Handles an AT command. Cancelling of the pending request is done by
     * HandleCommandCancel(). The implementation in the extension plugin should
     * be asynchronous.
     *
     * The extension plugin which accepts this command is responsible to supply
     * the result codes and response and to format result codes properly, e.g.
     * in BT HFP case, the format should be <cr><lf><result code><cr><lf>
     *
     * After an extension plugin has handled or decided to reject the given AT
     * command, it must inform ATEXT by HandleCommandCompleted() with a proper
     * error code.
	 *
	 * Note that in editor mode the setting of aReplyNeeded is always "ETrue"
	 * when a plugin processing the editor mode meets the end condition for the
	 * editor mode (!EReplyTypeEditor reply in editor mode). In this case the
	 * plugin must create the last reply even when aReplyNeeded is EFalse.
     *
     * @since S60 5.0
     * @param aCmd The AT command to be handled. Its format may vary depending
     *             on the specification. E.g. in BT HFP case, the command may
     *             contain a character carriage return (<cr>) in the end.
     * @param aReply When passed in, contains the built in answer filled by
     *               ATEXT if it is not empty; when command handling completes
     *               successfully, contains the result codes and responses to
     *               this command; Its ownership always belongs to ATEXT, plugin
     *               may reallocate its space when needed.
     * @param aReplyNeeded Reply needed if ETrue, no reply otherwise. If EFalse,
     *                     the aReply must not contain the reply, otherwise it
     *                     must contain verbose or numeric reply (ATV0/1) or an
     *                     empty string reply (with ATQ).
     * @return None
     */
    virtual void HandleCommand( const TDesC8& aCmd,
                                RBuf8& aReply,
                                TBool aReplyNeeded ) = 0;

    /**
     * Cancels a pending HandleCommand request.
     *
     * @since S60 5.0
     * @return None
     */
    virtual void HandleCommandCancel() = 0;

    /**
     * Next reply part's length.
     * The value must be equal or less than KDefaultCmdBufLength.
     * When the reply from this method is zero, ATEXT stops calling
     * GetNextPartOfReply().
     *
     * @since S60 5.0
     * @return Next reply part's length if zero or positive
     */
    virtual TInt NextReplyPartLength() = 0;

    /**
     * Gets the next part of reply initially set by HandleCommandComplete().
     * Length of aNextReply must be equal or less than KDefaultCmdBufLength.
     *
     * @since S60 5.0
     * @param aNextReply Next reply
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt GetNextPartOfReply( RBuf8& aNextReply ) = 0;

    /**
     * Receives unsolicited results. Cancelling of the pending request is done
     * by ReceiveUnsolicitedResultCancel(). The implementation in the extension
     * plugin should be asynchronous.
     *
     * @since S60 5.0
     * @return None
     */
    virtual void ReceiveUnsolicitedResult() = 0;

    /**
     * Cancels a pending ReceiveUnsolicitedResult request.
     *
     * @since S60 5.0
     * @return None
     */
    virtual void ReceiveUnsolicitedResultCancel() = 0;

    /**
     * Reports NVRAM status change to the plugins.
     *
     * @since S60 5.0
     * @param aNvram New NVRAM status. Each call of this function is a result
     *               of DUN extracting the form notified by
     *               CATExtCommonBase::SendNvramStatusChange(). Each of the
     *               settings from SendNvramStatusChange() is separated to
     *               one call of ReportNvramStatusChange().
     * @return None
     */
    virtual void ReportNvramStatusChange( const TDesC8& aNvram ) = 0;

    /**
     * Reports about external handle command error condition.
     * This is for cases when for example DUN decided the reply contained an
     * error condition but the plugin is still handling the command internally.
     * Example: in command line "AT+TEST;ATDT1234" was given. "AT+TEST" returns
     * "OK" and "ATDT" returns "CONNECT". Because "OK" and "CONNECT" are
     * different reply types the condition is "ERROR" and DUN ends processing.
     * This solution keeps the pointer to the last AT command handling plugin
     * inside ATEXT and calls this function there to report the error.
     * It is to be noted that HandleCommandCancel() is not sufficient to stop
     * the processing as the command handling has already finished.
     *
     * @since S60 5.0
     * @return None
     */
    virtual void ReportExternalHandleCommandError() = 0;

protected:

    /**
     * Called by concrete extension plugin when there is an unsolicited result
     * code should be sent to remote device. Instructs ATEXT to complete client
     * request message for ReceiveUnsolicitedResult().
     *
     * The extension plugin which accepts this command is responsible to supply
     * the result codes and response. The responses must be formatted to the
     * format set by ATS3 and ATS4 commands or the format included in
     * ReportNvramStatusChange() API call.
     *
     * @since S60 5.0
     * @param aAT the unsolicited result code to be sent.
     * @return KErrNone if the result AT has been accepted by ATEXT; Symbian
     *                  error code otherwise.
     */
    IMPORT_C TInt SendUnsolicitedResult( const TDesC8& aAT );

    /**
     * Called by concrete extension plugin to inform that a command handling has
     * been completed or rejected. Instructs ATEXT to complete client request
     * message for HandleCommand().
     *
     * @since S60 5.0
     * @param aError Completion code
     * @param aReplyType Reply type for the handled AT command
     * @return KErrNotSupported if the AT Extension Plugin doesn’t
     *         support the command passed in CATExtPluginBase::HandleCommand();
     *         KErrNone otherwise.
     */
    IMPORT_C TInt HandleCommandCompleted( TInt aError,
                                          TATExtensionReplyType aReplyType );

    /**
     * Called by concrete extension plugin to inform the array of supported
     * commands should be returned
     *
     * @since S60 5.0
     * @param aCmd Array of supported commands
     * @return Symbian error code on error, KErrNone otherwise
     */
    IMPORT_C TInt GetSupportedCommands( RPointerArray<HBufC8>& aCmds );

private:

    /**
     * Sets the observer.
     *
     * @since S60 5.0
     * @param aObserver the observer.
     * @return None
     */
    IMPORT_C void SetObserver( MATExtPluginObserver& aObserver );

protected:

    /**
     * Character for carriage return
     */
    TChar iCarriageReturn;

    /**
     * Character for line feed
     */
    TChar iLineFeed;

    /**
     * Character for backspace
     */
    TChar iBackspace;

    /**
     * Flag indicating whether quiet mode is on or off (ATQ)
     */
    TBool iQuietMode;

    /**
     * Flag indicating whether verbose mode is on or off (ATV)
     */
    TBool iVerboseMode;

private:

    /**
     * The reference of ATEXT interface
     */
    MATExtPluginObserver* iObserver;

    /**
     * UID set by ECOM when the instance is created.
     * Used when the instance is destroyed.
     */
    TUid iInstanceUid;

    };

#endif  // C_ATEXTPLUGINBASE_H
