/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  AccMonitorObserver functionality.
*
*/


#ifndef ACCMONITOROBSERVER_H
#define ACCMONITOROBSERVER_H

// INCLUDES
#include <e32base.h>
#include <AccessoryServer.h>
#include <AccMonitorInfo.h>

// FORWARD DECLARATIONS
class MAccMonitorObserver;

// CLASS DECLARATION
/**
* Observer class to observe accessory connection notifications.
*
* Active object for receiving asynchronous requests from accessory connection
* or disconnection events.
*/
NONSHARABLE_CLASS( CAccMonitorObserver ): public CActive
    {
public: // Constructors & destructors
    
    /**
    * Symbian two phased constructor.
    * @param    aAccConnection is a reference to Accessory Connection API. 
    */
    static CAccMonitorObserver* NewL( 
         RAccessoryConnection& aAccConnection );
 
    /**
    * Destructor 
    */
    virtual ~CAccMonitorObserver();

public: 
        
    /**
    * Sets observer to recive accessory connection notifications.
    * @param    aObserver is an Observer for the accessory connections
    * @param    aAccMonConnectedAccessories contains the accessories that
    *           were conneted when observer was started.
    */
    void StartObservingL( 
         MAccMonitorObserver* aObserver,
         RConnectedAccessories& aAccMonConnectedAccessories );
    
    /**
    * Sets observer to recive accessory connection notifications.
    * @param    aObserver is an Observer for the accessory connections
    * @param    aType is an instance of some precise accessory. So
    *           notifications are only received when this accessory is
    *           connected.
    * @param    aAccMonConnectedAccessories contains the accessories that
    *           were conneted when observer was started.
    */
    void StartObservingL( 
         MAccMonitorObserver* aObserver, 
         const CAccMonitorInfo* aInfo,
         RConnectedAccessories& aAccMonConnectedAccessories );
    
    /**
    * Sets observer to recive accessory connection notifications.
    * @param    aObserver is an Observer for the accessory connections
    * @param    aArray is an array of capabilities. This array describes 
    *           the capabilities of the accessory that needs to be listened.
    * @param    aAccMonConnectedAccessories contains the accessories that
    *           were conneted when observer was started.
    * @param    aInternalCall - Indicates if called from External Client API or internal RunL State-Machine
    *           
    */
    void StartObservingL( 
         MAccMonitorObserver* aObserver, 
         const RAccMonCapabilityArray& aArray,
         RConnectedAccessories& aAccMonConnectedAccessories,
         TBool aInternalCall );
    
private:  // From base classes

    /**
    * Default Constructor
    * @param    aAccConnection is a reference to Accessory Connection API.
    */
    CAccMonitorObserver( 
         RAccessoryConnection& aAccConnection );
    
    // From CActive
    void RunL();
    
    // From CActive
    void DoCancel();

    // From CActive
    TInt RunError( TInt aError );
    
    // Notifies observer when accessory connection status has changed
    void NotifyObserverL();
    
    // Notifies observer from error
    void NotifyObserverError( TInt aError );
    
    /**
    * Gets the amount of connected accessories.
    * @return    TInt amount of accessories
    */
    TInt AmountOfAccessories();
    
    /**
    * Checks if observer needs to be notified.
    * @return    boolean value if observer needs to be notified.
    */
    TBool CheckIfNotifyL();
    
    /**
    * Gets the CAccMonitorInfo for the disconnected accessory.
    * @param     aAccMonitorInfo is reference to which is disconnected
    *            accessory information saved.
    */
    void GetDisconnectedAccessoryL( CAccMonitorInfo& aAccMonitorInfo );
    
    /**
    * Gets the connected accessories
    * @param     aConnectedAccessories is reference to which is connected
    *            accessory information saved.
    */
    void GetConnectedAccessoriesL( RConnectedAccessories& aConnectedAccessories );
    
    /**
    * Starts observer.
    * @param    aObserver is an Observer for the accessory connections.
    * @param    aAccMonConnectedAccessories contains the accessories that
    *           were conneted when observer was started.
    */
    void StartObserverL(
         MAccMonitorObserver* aObserver,
         RConnectedAccessories& aAccMonConnectedAccessories );
           
private:
  
    // Enumertator for observer type, specifies what should be observed.
    enum TAccObserver
        {
        EEveryConnect      = 0,
        ESpecificAccessory = 1,
        EAccessoryType     = 2
        };
    
    // Accessory status observer, not owned
    MAccMonitorObserver* iObserver;
    
    // Reference to RAccessoryConnection, not owned
    RAccessoryConnection& iAccConnection;
    
    // generic id array of the connected accessories
    TAccPolGenericIDArray iGenericIdArray;
    
    // Some specific accessory to observe, not owned
    CAccMonitorInfo* iAccessoryInfo;
    
    // array of the capabilities to observe
    RAccMonCapabilityArray iAccessoryCapArray;
    
    // amount of connected accessories
    TInt iAmountAccessories;
    
    //If a connect happened notify with connect info.
    TBool iConnect; // ETrue = connect,
                    // EFalse = disconnect
    
    // Flag so observer knows what to start observing
    TAccObserver iObserverFlag;
    
    // Flag so observer knows what to start observing, owned
    RConnectedAccessories iAccMonConnectedAccessories;
    
    };

#endif // ACCMONITOROBSERVER_H

// End of File
