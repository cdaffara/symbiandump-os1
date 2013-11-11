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
* Description:  ATEXT common server
*
*/


#ifndef COMMON_SERVER_IMPL_H
#define COMMON_SERVER_IMPL_H

#include <e32svr.h>

class CATExtCommonSession;

/**
 *  CATExtSrv common server core class
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CATExtSrvCommon ) : public CPolicyServer
    {

public:

    /**
     * Two-phased constructor.
     *
     * @return Instance of self
     */
    static CATExtSrvCommon* NewLC();

    /**
     * Destructor.
     */
    ~CATExtSrvCommon();

    /**
     * Removes session from array of instantiated sessions
     *
     * @return None
     */
    void ClientClosed( CATExtCommonSession& aSession );

private:

    /**
     * Creates a new session to client
     *
     * @param aVersion the version
     * @param aMessage not used
     * return The new created session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

    /**
     * Constructor
     *
     * @param aPriority This AO's priority
     */
    CATExtSrvCommon( TInt aPriority );

    /**
     * Two-Phase constructor
     *
     * @return None
     */
    void ConstructL();

private:  // data

    /**
     * Array of instantiated sessions.
     * Own.
     */
    RPointerArray<CATExtCommonSession> iSessions;

    };

#endif  // COMMON_SERVER_IMPL_H
