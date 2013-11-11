/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Sensor server main class.
*
*/


#ifndef SENSRVSERVER_H
#define SENSRVSERVER_H

#include <e32base.h>
#include "sensrvclientserver.h"

// ----------------------------------------------------------------------------------------
// Sensor Server's policy
// ----------------------------------------------------------------------------------------

// Total number of ranges
const TUint KSensrvPolicyRangeCount = 3;

// Definition of the ranges of IPC numbers
const TInt KSensrvRanges[KSensrvPolicyRangeCount] =
    {
    ESensrvSrvReqQueryChannels,  // 0th range. Channel listening operations
    ESensrvSrvReqShutdownServer, // 1st range, Server shutdown
    ESensrvSrvReqEnd             // 2nd range, non implemented function end of range check; ENotSupported
    };

// Policy to implement for each of the above ranges
const TUint8 KSensrvElementsIndex[KSensrvPolicyRangeCount] =
    {
    CPolicyServer::EAlwaysPass,  // Applies to 0th range
    0,                           // Applies to 1st range
    CPolicyServer::ENotSupported // Applies to 2nd range (out of range IPC)
    };

// Specific capability checks
const CPolicyServer::TPolicyElement KSensrvElements[] =
    {
    { _INIT_SECURITY_POLICY_S0( KSensrvTestFrameworkUid ),    CPolicyServer::EPanicClient } // Policy "0", i.e. access is allowed only for test framework
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KSensrvPolicy =
    {
    CPolicyServer::EAlwaysPass, // All connect attempts to Sensor Server pass
    KSensrvPolicyRangeCount,    // Number of ranges
    KSensrvRanges,              // Ranges array
    KSensrvElementsIndex,       // Elements <-> ranges index
    KSensrvElements,            // Array of elements
    };

// Forward declarations
class CSensrvProxyManager;
class TSensrvResourceChannelInfo;

/**
*  Sensor server main class
*
*  @since S60 5.0
*/
class CSensrvServer : public CPolicyServer
    {
    public:  // Constructors and destructor
        

        /**
        * Creates a new server.
        *
        * @since S60 5.0
        * @return A pointer to the created object.
        */
        static CSensrvServer* NewL();
        
        /**
        * Creates a new server. Newly created instance is left in the cleanup stack.
        *
        * @since S60 5.0
        * @return A pointer to the created object.
        */
        static CSensrvServer* NewLC();

        /**
        * Destructor.
        */
        virtual ~CSensrvServer();

        /**
        * Notifies clients that a change has happened in available channels.
        *
        * @since S60 5.0
        * @param[in] aChangedChannel Channel information of the channel.
        * @param[in] aChangeType If ESensrvChannelAdded, channel is a newly available
        *                        channel. 
        *                        If ESensrvChannelRemoved, channel was removed and is  
        *                        no longer available.
        */
        void NotifyChannelChange(const TSensrvResourceChannelInfo& aChangedChannel,
                                 TSensrvChannelChangeType aChangeType );
    private:

        /**
        * C++ constructor.
        */
        CSensrvServer( const TServerType aType = EUnsharableSessions );
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Creates a new session when client connects.
        *
        * @since S60 5.0
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
        * Panics the server thread
        *
        * @since S60 5.0
        * @param aCategory Panicer's id 
        * @param aPanic    Reason of panic 
        * @return void
        */
        void Panic( const TDesC& aCategory, const TInt aReason );
        
        /**
        * Gets a reference to the session iterator of this server.
        *
        * @since S60 5.0
        * @return Reference to the session iterator.
        */
        inline TDblQueIter<CSession2>* SessionIterator() { return &iSessionIter; };
        
    private:    // Data

        /**
        * Proxy manager that handles all proxies. 
        * Own.
        */
        CSensrvProxyManager* iProxyManager;
    };



#endif // SENSRVSERVER_H 
