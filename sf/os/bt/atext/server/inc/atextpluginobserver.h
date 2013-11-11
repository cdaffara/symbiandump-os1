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


#ifndef C_ATEXTPLUGINOBSERVER_H
#define C_ATEXTPLUGINOBSERVER_H

#include <ecom/ecom.h>
#include <atextpluginbase.h>

class CATExtPluginBase;

/**
 *  The interface for AT Extension Plugin to interact with plugins.
 *
 *  @since S60 v5.0
 */
class MATExtPluginObserver
    {

public:

    /**
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
    virtual TInt SendUnsolicitedResult( CATExtPluginBase* aPlugin,
                                        const TDesC8& aAT ) = 0;

    /**
     * Called by Extension Plugins to inform ATEXT that a command
     * handling has been completed or rejected.
     *
     * @since S60 v5.0
     * @param aPlugin Plugin from which the completion comes
     * @param aError Error code for completion
     * @param aReplyType Reply type for the handled AT command
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt HandleCommandCompleted( CATExtPluginBase* aPlugin,
                                         TInt aError,
                                         TATExtensionReplyType aReplyType ) = 0;

    /**
     * Called by concrete extension plugin to inform the array of supported
     * commands should be returned
     *
     * @since S60 5.0
     * @param aPlugin Plugin from which the completion comes
     * @param aCmd Array of supported commands
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt GetSupportedCommands( CATExtPluginBase* aPlugin,
                                       RPointerArray<HBufC8>& aCmds ) = 0;

    /**
     * Called by the destructor of CATExtPluginBase.
     * A concrete service provider implementation should not touch this.
     *
     * @param aPlugin Plugin from which the closing comes
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt ATExtPluginClosed( CATExtPluginBase* aPlugin ) = 0;

    };

#endif  // C_ATEXTPLUGINOBSERVER_H
