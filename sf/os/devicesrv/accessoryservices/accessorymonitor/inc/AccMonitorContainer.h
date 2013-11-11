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
* Description:  AccMonitorContainer functionality.
*
*/


#ifndef ACCMONITORCONTAINER_H
#define ACCMONITORCONTAINER_H

// INCLUDES
#include <e32base.h>
#include <AccessoryConnection.h>
#include <AccMonitorInfo.h>
#include "AccMonitorObserver.h"

// FORWARD DECLARATIONS
class CAccMonitorInfo;

/**
* Cleanup helper for pointer arrays.
*/
template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL( T& aRef );
private:
    static void ResetAndDestroy( TAny *aPtr );
    };

template <class T>
void CleanupResetAndDestroy<T>::PushL( T& aRef )
    {
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &aRef ) ); 
    }

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy( TAny *aPtr )
    {
    static_cast<T*>( aPtr )->ResetAndDestroy();
    }

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }

// CLASS DECLARATION
/** 
* A container class to CAccMonitor.
*
* This implementation is a wrapper that encapsulates internal interfaces
* from extended SDK. This container provides information about accessory
* API.
*/ 
NONSHARABLE_CLASS( CAccMonitorContainer ): public CBase 
    {
public:     // Constructors & destructors
  
    /**
    * Destructor 
    */
    virtual ~CAccMonitorContainer();

    /**
    * Symbian two phased constructor.
    */
    static CAccMonitorContainer* NewL();

public:     // Methods with functionality
  
    /**
    * Used to query the the connected accessories. 
    * @param    aAccessoriesArray is an array which contains some
    *           basic information about all the connected accessories.
    *     Accessory information is referenced by number order in this
    *     array.
    * @return   KErrNone if query succeeded, KErrNotFound if no accessories
    *           are connected. Otherwise standard symbian error codes.
    */
    TInt GetConnectedAccessoriesL( 
      RConnectedAccessories& aAccessoriesArray );
    
    /**
    * Used to query a value for some capability. 
    * @param    aAccessoryNumber is number in order that references to the
    *           connected accessories. With one accessory this number is
    *           always one.
    * @param    aCapability is the the capability for which the value is
    *           getted for.
    * @param    aValue is a where the value is set.
    * @return   KErrNone is successful, otherwise standard Symbian error
    *     codes.
    */
    void GetValueForCapabilityL(
      const CAccMonitorInfo* aInfo,
      const TUint32 aCapability,
      TInt& aValue );
    
    /**
    * Check if accessoey has some specific capability.
    * @param    aAccessoryNumber is number in order that references to the
    *           connected accessories. With one accessory this number is
    *           always one.
    * @param    aCapability capability that is asked 
    * @return   True if accessory has that capability
    */
    TBool HasAccessoryCapability(
      const TUint8 aAccessoryNumber,
      const TUint32 aCapability );
    
    /**
    * Starts observing accessory connection changes.
    */
    void StartObservingL( 
      MAccMonitorObserver* aObserver );
    
    /**
    * Starts observing accessory connection changes.
    */
    void StartObservingL( 
      MAccMonitorObserver* aObserver,
      const CAccMonitorInfo* aInfo );
    
    /**
    * Starts observing accessory connection changes.
    */
    void StartObservingL( 
      MAccMonitorObserver* aObserver,
      const RAccMonCapabilityArray& aArray );
    
    /**
    * Stops observing accessory connection status changes.
    * This is not nessecary to be called in destructor.
    * Listening will be automatically stopped when CHeadsetStatus
    * object is deleted.
    */
    void StopObserving();
    
    /**
    * Checks whether already observing.
    */
    TBool IsObserving() const;
    
private:

    /**
    * Default Constructor 
    */
    CAccMonitorContainer();
    
    /**
    * Symbian OS 2nd phase constructor 
    */
    void ConstructL();
    
    /**
    * Gets accessories from Accessory Server
    */
    TInt GetAccessoriesFromAccServerL();

private:
  
  // Subblock name array for some Generic ID
  TAccPolGenericIDArray*   iConnectedAccessories;
  
  // Handle to Accessory Server process
  RAccessoryServer      iAccSrv;
  
  // Handle to the Accessory Servers connection handler
  RAccessoryConnection    iAccessoryConnection;
  
  // Pointer to the observer
  CAccMonitorObserver*    iObserver;
      
  };

#endif // ACCMONITORCONTAINER_H

// End of File
