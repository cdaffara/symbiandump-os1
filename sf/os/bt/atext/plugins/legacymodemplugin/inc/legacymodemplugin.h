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
* Description:  Main handler for incoming requests
*
*/

#ifndef C_LEGACYMODEMPLUGIN_H
#define C_LEGACYMODEMPLUGIN_H

#include <atextpluginbase.h>

class CLegacyModemPlugin;

/** Character types: carriage return, line feed or backspace */
enum TCharacterTypes
    {
    ECharTypeCR,  // Carriage return
    ECharTypeLF,  // Line feed
    ECharTypeBS   // Backspace
    };

/** Type of modes (quiet, verbose) */
enum TModeTypes
    {
    EModeTypeQuiet,   // Quiet mode
    EModeTypeVerbose  // Verbose mode
    };

/**  Handler types for the four types */
enum TCmdHandlerType
    {
    ECmdHandlerTypeUndefined = KErrNotFound,
    ECmdHandlerTypeBase      = 0x01,  // For command "AT+COMMAND"
    ECmdHandlerTypeSet       = 0x02,  // For command "AT+COMMAND="
    ECmdHandlerTypeRead      = 0x04,  // For command "AT+COMMAND?"
    ECmdHandlerTypeTest      = 0x08,  // For command "AT+COMMAND=?"
    };

/**
 *  Class for common AT command handler interface
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLegacyModemPluginBase )
    {

public:

    virtual ~CLegacyModemPluginBase() {};

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

    };

/**
 *  Class for accessing plugin information and common functionality
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( MLegacyModemPlugin )
    {

public:

    /**
     * Creates an AT command reply based on the reply type and completes the
     * request to ATEXT. Uses iReplyBuffer for reply storage.
     *
     * @since S60 5.0
     * @param aReplyType Type of reply
     * @param aSrcBuffer Source buffer; used only if aReplyType is EReplyTypeOther
     * @param aError Completion code. If not KErrNone then other arguments are
     *               ignored and the request is completed to ATEXT with
     *               EReplyTypeUndefined.
     * @return None
     */
    virtual TInt CreateReplyAndComplete( TATExtensionReplyType aReplyType,
                                         const TDesC8& aSrcBuffer=KNullDesC8,
                                         TInt aError=KErrNone ) = 0;

    /**
     * Creates a buffer for "OK" or "ERROR" reply based on the line settings
     *
     * @since S60 5.0
     * @param aReplyBuffer Destination buffer for the created reply
     * @param aOkReply ETrue if "OK" reply needed,
     *                 EFalse if "ERROR" reply needed
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt CreateOkOrErrorReply( RBuf8& aReplyBuffer,
                                       TBool aOkReply ) = 0;

    /**
     * Checks if the command is a base, set, read or test type of command
     *
     * @since S60 5.0
     * @param aCmdBase Base part of the command to check
     * @param aCmdFull Full command to check
     * @return Type of command
     */
    virtual TCmdHandlerType CheckCommandType( const TDesC8& aCmdBase,
                                              const TDesC8& aCmdFull ) = 0;

    };

/**
 *  Class for selecting handlers for different AT commands
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLegacyModemPlugin ) : public CATExtPluginBase,
                                          public MLegacyModemPlugin
    {

public:

    /**
     * Two-phased constructor.
     * @return Instance of self
     */
    static CLegacyModemPlugin* NewL();

    /**
    * Destructor.
    */
    virtual ~CLegacyModemPlugin();

private:

    CLegacyModemPlugin();

    void ConstructL();

    /**
     * Reports connection identifier name to the extension plugin.
     *
     * @since S60 5.0
     * @param aName Connection identifier name
     * @return None
     */
    void ReportConnectionName( const TDesC8& aName );

    /**
     * Reports the support status of an AT command. This is a synchronous API.
     *
     * @param aCmd The AT command. Its format may vary depending on the
     *             specification. E.g. in BT HFP case, the command may contain
     *             a character carriage return (<cr>) in the end.
     * @return ETrue if the command is supported; EFalse otherwise.
     */
    TBool IsCommandSupported( const TDesC8& aCmd );

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
    void HandleCommand( const TDesC8& aCmd, RBuf8& aReply, TBool aReplyNeeded );

    /**
     * Cancels a pending HandleCommand request.
     *
     * @since S60 5.0
     * @return None
     */
    void HandleCommandCancel();

    /**
     * Next reply part's length.
     * The value must be equal or less than KDefaultCmdBufLength.
     * When the reply from this method is zero, ATEXT stops calling
     * GetNextPartOfReply().
     *
     * @since S60 5.0
     * @return Next reply part's length if zero or positive
     */
    TInt NextReplyPartLength();

    /**
     * Gets the next part of reply initially set by HandleCommandComplete().
     * Length of aNextReply must be equal or less than KDefaultCmdBufLength.
     *
     * @since S60 5.0
     * @param aNextReply Next reply
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt GetNextPartOfReply( RBuf8& aNextReply );

    /**
     * Receives unsolicited results. Cancelling of the pending request is done
     * by ReceiveUnsolicitedResultCancel(). The implementation in the extension
     * plugin should be asynchronous.
     *
     * @since S60 5.0
     * @return None
     */
    void ReceiveUnsolicitedResult();

    /**
     * Cancels a pending ReceiveUnsolicitedResult request.
     *
     * @since S60 5.0
     * @return None
     */
    void ReceiveUnsolicitedResultCancel();

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
    void ReportNvramStatusChange( const TDesC8& aNvram );

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
    void ReportExternalHandleCommandError();

    /**
     * Creates part of reply from the global reply buffer to the destination
     * buffer. Used with APIs which need the next part of reply in multipart
     * reply requests.
     *
     * @since S60 5.0
     * @param aDstBuffer Destination buffer; the next part of reply is stored to
     *                   this buffer.
     * @return None
     */
    TInt CreatePartOfReply( RBuf8& aDstBuffer );

    /**
     * Appends one parameter to array for GetParameterArray()
     *
     * @since S60 5.0
     * @param aParams Array of supported commands
     * @param aStartIndex Start index for the parameter to be added
     * @param aEndIndex End index for the parameter to be added
     * @return None
     */
    void AppendOneParameterLC( RPointerArray<HBufC8>& aParams,
                               TInt& aStartIndex,
                               TInt aEndIndex );

// from base class MLegacyModemPlugin

    /**
     * From MLegacyModemPlugin.
     * Creates an AT command reply based on the reply type and completes the
     * request to ATEXT. Uses iReplyBuffer for reply storage.
     *
     * @since S60 5.0
     * @param aReplyType Type of reply
     * @param aSrcBuffer Source buffer; used only if aReplyType is EReplyTypeOther
     * @param aError Completion code. If not KErrNone then other arguments are
     *               ignored and the request is completed to ATEXT with
     *               EReplyTypeUndefined.
     * @return None
     */
    TInt CreateReplyAndComplete( TATExtensionReplyType aReplyType,
                                 const TDesC8& aSrcBuffer=KNullDesC8,
                                 TInt aError=KErrNone );

    /**
     * From MLegacyModemPlugin.
     * Creates a buffer for "OK" or "ERROR" reply based on the line settings
     *
     * @since S60 5.0
     * @param aReplyBuffer Destination buffer for the created reply
     * @param aOkReply ETrue if "OK" reply needed,
     *                 EFalse if "ERROR" reply needed
     * @return Symbian error code on error, KErrNone otherwise
     */
    TInt CreateOkOrErrorReply( RBuf8& aReplyBuffer,
                               TBool aOkReply );

    /**
     * From MLegacyModemPlugin.
     * Checks if the command is a base, set, read or test type of command
     *
     * @since S60 5.0
     * @param aCmdBase Base part of the command to check
     * @param aCmdFull Full command to check
     * @return Type of command
     */
    TCmdHandlerType CheckCommandType( const TDesC8& aCmdBase,
                                      const TDesC8& aCmdFull );

private:  // data

    /**
     * AT commands handler array. Used for mapping HandleCommand() to
     * IsCommandSupported() and to limit the number of string comparisons.
     */
    RPointerArray<CLegacyModemPluginBase> iHandlers;

    /**
     * Current AT command handler in iHandlers.
     * Used when IsCommandSupported() detects a matching handler class.
     */
    CLegacyModemPluginBase* iHandler;

    /**
     * Buffer for handle command's command
     * Not own.
     */
    const TDesC8* iHcCmd;

    /**
     * Buffer for handle command reply
     * Not own.
     */
    RBuf8* iHcReply;

    /**
     * Global reply buffer for the AT command replies
     */
    RBuf8 iReplyBuffer;

    };

#endif  // C_LEGACYMODEMPLUGIN_H
