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
* Description: Handles commands "ATL", "ATL0", "ATL1", "ATL2", "ATL3", "ATM",
*              "ATM0", "ATM1" and "ATM2"
*
*/

#ifndef C_MONITORSPEAKERPARSER_H
#define C_MONITORSPEAKERPARSER_H

#include "legacymodemplugin.h"

/**  Detected commands */
enum TDetectedCmd
    {
    EDetectedCmdUndefined,
    EDetectedCmdATL,   // For command "ATL"
    EDetectedCmdATL0,  // For command "ATL0"
    EDetectedCmdATL1,  // For command "ATL1"
    EDetectedCmdATL2,  // For command "ATL2"
    EDetectedCmdATL3,  // For command "ATL3"
    EDetectedCmdATM,   // For command "ATM"
    EDetectedCmdATM0,  // For command "ATM0"
    EDetectedCmdATM1,  // For command "ATM1"
    EDetectedCmdATM2   // For command "ATM2"
    };

/**
 *  Main class for handling commands "ATL", "ATL0", "ATL1", "ATL2", "ATL3",
 *  "ATM", "ATM0", "ATM1" and "ATM2"
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMonitorSpeakerParser ) : public CLegacyModemPluginBase
    {

public:

    /**
     * Two-phased constructor.
     * @param aCallback Callback
     * @return Instance of self
     */
    static CMonitorSpeakerParser* NewL( MLegacyModemPlugin* aCallback );

    /**
    * Destructor.
    */
    virtual ~CMonitorSpeakerParser();

private:

    CMonitorSpeakerParser( MLegacyModemPlugin* aCallback );

    void ConstructL();

    /**
     * Checks if the command is speaker loudness command
     *
     * @param aCmd Command to check
     * @return ETrue if command understood, EFalse otherwise
     */
    TBool CheckSpeakerLoudnessCommand( const TDesC8& aCmd );

    /**
     * Checks if the command is speaker mode command
     *
     * @param aCmd Command to check
     * @return ETrue if command understood, EFalse otherwise
     */
    TBool CheckSpeakerModeCommand( const TDesC8& aCmd );

    /**
     * Main command handler (leaving version)
     *
     * @return ETrue if command understood, EFalse otherwise
     */
    TBool HandleCommandL();

// from base class CLcCustomPluginBase

    /**
     * From CLcCustomPluginBase.
     * Reports the support status of an AT command. This is a synchronous API.
     *
     * @param aCmd The AT command. Its format may vary depending on the
     *             specification. E.g. in BT HFP case, the command may contain
     *             a character carriage return (<cr>) in the end.
     * @return ETrue if the command is supported; EFalse otherwise.
     */
    TBool IsCommandSupported( const TDesC8& aCmd );

    /**
     * From CLcCustomPluginBase.
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
     * From CLcCustomPluginBase.
     * Cancels a pending HandleCommand request.
     *
     * @since S60 5.0
     * @return None
     */
    void HandleCommandCancel();

private:  // data

    /**
     * Callback to call when accessing plugin information
     */
    MLegacyModemPlugin* iCallback;

    /**
     * Handler type for the three types
     */
    TCmdHandlerType iCmdHandlerType;

    /**
     * Detected command
     */
    TDetectedCmd iDetectedCmd;

    };

#endif  // C_MONITORSPEAKERPARSER_H
