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
* Description:  A wrapper for Accessory API. This API is used for getting 
*                 information about connected accessories. The accessory 
*         connections and disconnections can also be listened through
*         this API.
*
*/


#ifndef ACCMONITORIMPL_H
#define ACCMONITORIMPL_H

class CAccMonitorContainer;

/**
* Class provides implementation for CAccMonitor.
* 
* The CAccMonitorImpl class that derives from CAccMonitor. This class
* implements the pure virtual functions from CAccMonitor.
*/
class CAccMonitorImpl : public CAccMonitor
    {
    
public: 
    
    /**
    * Default C++ constructor
    */
    CAccMonitorImpl();

    /**
    * Symbian OS 2nd phase constructor 
    */
    void ConstructL();
    
    /**
    * Destructor 
    */
    virtual ~CAccMonitorImpl();
    
    /**
    * Used to get the connected accessories. 
    *
    * @param    aAccessoriesArray is a reference to an array that is 
    *           filled with CAccMonitorInfo instances which are accessories
    *           connected to the S60 device.
    * @leave    KErrNotFound if no accessories are connected.
    * @return   The amount of connected accessories.
    */
    TInt GetConnectedAccessoriesL(
      RConnectedAccessories& aAccessoriesArray ) const; 
    
    /**
    * Sets observer to listen accessory connect/disconnect notifications.
    *
    * @param    aObserver is the pointer to the callback functions.
    * @leave    TInt KErrAlreadyExists if observer is active allready,
    *           otherwise KErrNone or standard Symbian error code.
    */
    void StartObservingL(
      MAccMonitorObserver* aObserver );
            
    /**
    * Sets observer to listen accessory connect/disconnect notifications.
    *
    * @param    aObserver is the pointer to the callback functions.
    * @param    aInfo is the instance of the accessory that needs to be
    *           listened. Accessory type that needs to be listened is copied
    *           from connected accessories array. If an empty instance is set
    *           as the parameter no notifications are sent.
    * @leave    TInt KErrAlreadyExists if observer is active allready,
    *           otherwise KErrNone or standard Symbian error code.
    */
    void StartObservingL(
      MAccMonitorObserver* aObserver,
      const CAccMonitorInfo* aInfo );
    
    /**
    * Sets observer to listen accessory connect/disconnect notifications.
    *
    * @param    aObserver is the pointer to the callback functions.
    * @param    aCapabilityArray is an array of capabilities. Accessory 
    *           type that needs to be listened is constructed with this
    *           capability array. For exmple if KAccMonHeadset capability
    *           is inserted to the array the observer receives all
    *           notifications about connected/disconnected headsets.
    *           If an empty array is set as parameter no notifications are
    *           sent to the observer.
    * @leave    TInt KErrAlreadyExists if observer is active allready,
    *           otherwise KErrNone or standard Symbian error code.
    */
    void StartObservingL(
      MAccMonitorObserver* aObserver,
      const RAccMonCapabilityArray& aArray );
    
    /**
    * Stops observing accessory connection status changes.
    * This is not nessecary to be called in destructor.
    * Listening will be automatically stopped when CAccMonintoringApi
    * object is deleted.
    */
    void StopObserving();
    
    /**
    * Checks whether observer is already observing.
    *
    * @return   ETrue if observing
    */
    TBool IsObserving() const;
    
    /**
    * Gets the value for some capability.
    *
    * @param    aInfo is the information about the accessory to which
    *           the value needs to be fetched.
    * @param    aCapability is capability which value needs to be
    *           fetched.
    * @param    aValue is the value for the capability.
    * @leave    KErrNotFound if no capability is not defined for this
    *           accessory, KErrArgument if this accessory is not
    *           connected, otherwise standard Symbian error code.
    */
    void GetCapabilityValueL( 
      const CAccMonitorInfo* aInfo,
      const TUint32 aCapability,
      TInt& aValue ) const;
    
private: 
    
    // AccMonitor container, owned
    CAccMonitorContainer* iContainer;
  
    };
  
#endif // ACCMONITORIMPL_H

// End of File
