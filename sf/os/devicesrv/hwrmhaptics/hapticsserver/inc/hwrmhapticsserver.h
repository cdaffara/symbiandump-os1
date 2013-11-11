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
* Description:  Haptic server interface.
*
*/


#ifndef HWRMHAPTICSSERVER_H
#define HWRMHAPTICSSERVER_H

#include "hwrmhapticsclientserver.h"

class CHWRMHapticsPluginManager;
class CHWRMHapticsReservationHandler;
class CHWRMHapticsCommonData;
class CHWRMHapticsShutdown;

/**
 * Haptics server implementation.
 */
class CHWRMHapticsServer : public CPolicyServer
    {
public:

    /**
     * Creates a new server.
     * 
     * @param aPriority Priority of the server.
     * @return A pointer to the created object.
     */
    static CHWRMHapticsServer* NewL( TInt aPriority );

    /**
     * Creates a new server. Newly created instance is left onto
     * the cleanup stack.
     * 
     * @param aPriority Priority of the server.
     * @return A pointer to the created object.
     */
    static CHWRMHapticsServer* NewLC( TInt aPriority );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsServer();

    /**
     * Get pointer to plugin handler. Returns NULL if plugin manager
     * is not initialized yet.
     *
     * @return Pointer to correct plugin handler, or NULL if not created.
     */
    CHWRMHapticsPluginManager* PluginManager() const;

    /**
     * Get pointer to reservation handler. Returns NULL if handler is 
     * not initialized yet.
     *
     * @return Pointer to correct reservation handler, or NULL if not created.
     */
    CHWRMHapticsReservationHandler* ReservationHandler() const;

    /**
     * Get pointer to Haptics common data.
     *
     * @return Pointer to haptics common data or NULL if resource 
               not supported.
     */
    CHWRMHapticsCommonData* HapticsCommonData() const;

    /**
     * Initializes Haptics service. Haptics is not initialized in 
     * ConstructL in order to speed up the startup.
     */
    void InitHaptics();

    /**
     * Increases the amount of sessions connected to 
     * this server (iSessionCount).
     */
    void AddSession();

    /**
     * Decreases the amount of sessions connected to 
     * this server (iSessionCount). If the session count becomes
     * zero, starts the shutdown timer for shutting down this server.
     */
    void DropSession();

private:

    /**
     * C++ constructor.
     * 
     * @param aPriority The priority for this server.
     */
    CHWRMHapticsServer( TInt aPriority );
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Creates a new session when client connects.
     * 
     * @param aVersion Version, which should match with the version of 
                       this server.
     * @param aMessage 'connect' message from the client
     * @return Pointer to created session or leaves with codes
     *         KErrNotSupported if versions does not match
     *         KErrNoMemory if creation of new session fails.
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

private:

    /**
     * Plugin manager for this server.
     * Owned.
     */
    CHWRMHapticsPluginManager* iHapticsPluginManager;

    /**
     * Reservation handler for this server.
     * Owned.
     */
    CHWRMHapticsReservationHandler* iHapticsReservationHandler;

    /**
     * Common data of this server.
     * Owned.
     */
    CHWRMHapticsCommonData* iHapticsCommonData;

    /**
     * Whether or not haptics has been initialized on the server.
     */
    TBool iHapticsInitiated;

    /**
     * The amount of sessions currently in the server.
     */
    TInt iSessionCount;

    /**
     * Shutdown timer instance.
     * Owned.
     */
    CHWRMHapticsShutdown* iShutdown;
    };

#endif      // HWRMHAPTICSSERVER_H 
            
// End of File
