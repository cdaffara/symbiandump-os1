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
* Description:  CAccSrvWiredConnectionPublisher declaration.
*
*/


#ifndef ACCSRVWIREDCONNECTIONPUBLISHER_H
#define ACCSRVWIREDCONNECTIONPUBLISHER_H

#include <e32property.h> 
#include <AccPolGenericID.h>

/**
 *  Wired accessory connection status publisher.
 *  CAccSrvWiredConnectionPublisher updates the P&S key holding the status of 
 *  wired accessory (other than USB) connections regardless of the actual accessory modes. 
 *
 *  @lib None.
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAccSrvWiredConnectionPublisher ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CAccSrvWiredConnectionPublisher* NewL();

    /**
     * Two-phased constructor.
     */
    static CAccSrvWiredConnectionPublisher* NewLC();

    /**
    * Destructor.
    */
    virtual ~CAccSrvWiredConnectionPublisher();

    /**
     * Handle accessory connection according to given Generic ID.
     * Only the first wired connection is updated to the P&S key, subsequent
     * connects only update the reference counter.
     *
     * @since S60 3.2
     * @param aGenericId      Generic ID of the connected accessory.
     */
    void HandleConnectL( const TAccPolGenericID& aGenericID );
    
    /**
     * Start to handle accessory disconnection according to given Generic ID.
     * Decreases the reference counter when a wired accessory is disconnected.
     * Once the reference counter is decreased to 0 the P&S key status is set to 'disconnected'.
     *
     * @since S60 3.2
     * @param aGenericID Generic ID of the disconnected accessory
     */
    void HandleDisconnectL( const TAccPolGenericID& aGenericID );
    
private:

    /*
     *  C++ default constructor.
     */ 
    CAccSrvWiredConnectionPublisher();

    /*
     * Symbian 2nd-phase constructor.
     */ 
    void ConstructL();


private: // data

    /**
     * Count of wired connections.
     */
    TInt iConnectionCount;

    /**
     * Publish&Subscribe key session used for publishing wired accessory state.     
     */ 
    RProperty iProperty;
    };

#endif // ACCSRVWIREDCONNECTIONPUBLISHER_H
