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


#ifndef C_ATEXTCOMMONOBSERVER_H
#define C_ATEXTCOMMONOBSERVER_H

#include <ecom/ecom.h>

class CATExtCommonBase;

/**
 *  The interface for AT Extension Plugin to interact with plugins.
 *
 *  @since S60 v5.0
 */
class MATExtCommonObserver
    {

public:

    /**
     * Called by concrete extension Plugin to inform that mode status has
     * changed.
     *
     * @since S60 5.0
     * @param aMode New mode
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt SendModeStatusChange( TUint aMode ) = 0;

    /**
     * Called by concrete extension plugin to inform that NVRAM status has
     * changed.
     *
     * @since S60 5.0
     * @param aNvram New NVRAM status. The new settings (delimited by "|")
     *               must differ from the Hayes defaults.
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt SendNvramStatusChange( const TDesC8& aNvram ) = 0;

    /**
     * Called by the destructor of CATExtPluginBase.
     * A concrete service provider implementation should not touch this.
     *
     * @param aPlugin Plugin that was closed
     * @return Symbian error code on error, KErrNone otherwise
     */
    virtual TInt ATExtPluginClosed( CATExtCommonBase* aPlugin ) = 0;

    };

#endif  // C_ATEXTCOMMONOBSERVER_H
