/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM server main class.
*
*/


#ifndef PSMSRVSERVER_H
#define PSMSRVSERVER_H

#include <e32base.h>
#include "psmclientserver.h"

// Total number of ranges
const TUint KPsmSrvPolicyRangeCount = 2;

// Definition of the ranges of IPC numbers
const TInt KPsmSrvRanges[KPsmSrvPolicyRangeCount] =
    {
    EPsmServerNotifyModeChange,  // 0th range. basic operations
    EPsmServerReqEnd             // 3rd range, non implemented functions; ENotSupported
    };

// Policy to implement for each of the above ranges
const TUint8 KPsmSrvElementsIndex[KPsmSrvPolicyRangeCount] =
    {
    0,                           // Applies to 0th range
    CPolicyServer::ENotSupported // Applies to 1st range (out of range IPC)
    };

// Specific capability checks
const CPolicyServer::TPolicyElement KPsmSrvElements[] =
    {
    { _INIT_SECURITY_POLICY_C2( ECapabilityReadDeviceData, ECapabilityWriteDeviceData ), 
                                    CPolicyServer::EFailClient }  // Policy "0"
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KPsmSrvPolicy =
    {
    CPolicyServer::EAlwaysPass, // All connect attempts to PSM Server pass
    KPsmSrvPolicyRangeCount,    // Number of ranges
    KPsmSrvRanges,              // Ranges array
    KPsmSrvElementsIndex,       // Elements <-> ranges index
    KPsmSrvElements,            // Array of elements
    };


// FORWARD DECLARATIONS
class CPsmManager;
class CPsmSrvShutdown;

/**
 *  PSM server main class
 *
 *  @since S60 5.0
 */
class CPsmSrvServer : public CPolicyServer
    {

    friend class CPsmSrvShutdown;

    public:  // Constructors and destructor

        /**
         * Creates a new server.
         *
         * @return A pointer to the created object.
         */
        static CPsmSrvServer* NewL();

        /**
         * Creates a new server. Newly created instance is left in the cleanup stack.
         *
         * @return A pointer to the created object.
         */
        static CPsmSrvServer* NewLC();

        /**
         * Destructor.
         */
        virtual ~CPsmSrvServer();

    public:

        /**
         * Indicates that session is closed
         */
        void SessionClosed();

    private:

        /**
         * C++ constructor.
         */
        CPsmSrvServer( const TServerType aType = EUnsharableSessions );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Creates a new session when client connects.
         *
         * @param aVersion Version
         * @param aMessage 'connect' message from the client
         * @return Pointer to created session or leaves with codes
         *         KErrNotSupported if versions does not match
         *         KErrNoMemory if creation of new session fails.
         */
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage ) const;

        /**
         * Checks can we shut down server
         *
         * @return KErrNotReady if server cannot be shut down yet.
         */
        TInt ShutdownServer();

        /**
         * Panics the server thread
         *
         * @param aCategory Panicer's id 
         * @param aPanic    Reason of panic 
         */
        void Panic( const TDesC& aCategory, const TInt aReason );

        /**
         * Gets a reference to the session iterator of this server.
         *
         * @return Reference to the session iterator.
         */
        inline TDblQueIter<CSession2>* SessionIterator() { return &iSessionIter; };

    private:    // Data

        /**
         * Main class of this server
         */
        CPsmManager* iPsmManager;

        /**
         * Timer to terminate server
         */
        CPsmSrvShutdown* iShutdown;

    };

#endif // PSMSRVSERVER_H 
