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
* Description:  ATEXT server
*
*/


#ifndef SERVER_IMPL_H
#define SERVER_IMPL_H

#include <e32svr.h>

class CATExtSession;

/**
 *  CATExtSrv server core class
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CATExtSrv ) : public CPolicyServer
    {

public:

    /**
     * Two-phased constructor.
     */
    static CATExtSrv* NewLC();

    /**
     * Destructor.
     */
    ~CATExtSrv();

    /**
     * Called when a plugin closes
     *
     * @since S60 v5.0
     * @param aSession Session
     * @return None.
     */
    void ClientClosed( CATExtSession& aSession );

private:

    /**
     * Creates a new session to client
     *
     * @since S60 v5.0
     * @param aVersion The version
     * @param aMessage Not used
     * return The new created session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param aPriority This AO's priority
     */
    CATExtSrv( TInt aPriority );

    /**
     * Two-Phase constructor
     *
     * @return None
     */
    void ConstructL();

private:  // data

    /**
     * Array of sessions
     */
    RPointerArray<CATExtSession> iSessions;

    };

#endif  // SERVER_IMPL_H
