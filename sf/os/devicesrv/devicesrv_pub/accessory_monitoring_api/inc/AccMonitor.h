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
* Description:  This API is used for getting 
*                 information about connected accessories. The accessory 
*                 connections and disconnections can also be listened through
*                 this API.
*
*/


#ifndef ACCMONITOR_H
#define ACCMONITOR_H

// INCLUDES
#include <e32base.h>
#include <AccMonitorInfo.h>

// FORWARD DECLARATIONS
class MAccMonitorObserver;

// CLASS DECLARATION
/**
* The Accessory Monitoring API interface offers accessory information about
* connected accessories. It also offers an easy-to-use implementation of a 
* CActive-based wrapper for accessory connection status changed event 
* notifications. The clients can get information about the connected 
* accessories capabilities and, if the capabilities have values, get the
* values from these capabilites. This API consist of classes CAccMonitor,
* CAccMonitorInfo and MAccMonitorObserver. If the user wants to use the
* observer this API offers, the user has to  implement callback function
* for receiving accessory connection and disconnection status notifications. 
* The connected accessories are offered to the client in an array. This array
* presents the connected accessories as instances of CAccMonitorInfo class.
* The CAccMonitorInfo offers accessory information encapsulated to a class. It
* presents accessory device type, physical connection and device address 
* information with basic getter functions. The rest of the accessories
* features are contained in an array. This array can be accessed with functions
* that are in CAccMonitorInfo.
* This class is not intended for user derivation.
*
* Usage:
* 
* Example query for some accessory's information. All connected accessories are
* fetched and the instance of CAccessoryInfo is getted:
* @code
* CAccMonitor* accMonitor = CAccMonitor::NewLC();
* RConnectedAccessories connectedAccessories;
* CleanupClosePushL( connectedAccessories );
* accMonitor->GetConnectedAccessoriesL( connectedAccessories );
* CAccMonitorInfo* accInfo = CAccMonitorInfo::NewLC();
* TInt countOfArray = connectedAccessories.Count();
* for( TInt i = 0; i != countOfArray; i++ )
*   {
*   TAccMonCapability deviceType = connectedAccessories[ i ]->AccDeviceType();
*   if( deviceType == KAccMonHeadset )
*     {
*     // Some Headset is connected, get the information to accInfo
*     accInfo->CopyL( connectedAccessories[ i ] );
*     }
*   }
* // Destroy the pointers from the array, because those are owned by the client
* CleanupStack::PopAndDestroy( accInfo );
* CleanupStack::PopAndDestroy( &connectedAccessories );
* CleanupStack::PopAndDestroy( accMonitor );
* @endcode
* 
* Example query for some accessorys capabilties. The user has fetched the
* CAccMonitorInfo to accInfo instance from connected accessories:
* @code
* TInt arrayCount = accInfo->Count();
* for( TInt i = 0; i != arrayCount; i++ )
*   {
*   TAccMonCapability accCapa = accInfo->AccCapabilityAtIndex( i );
*   if( accCapa == KAccMonStereoAudio )
*     {
*     // Accessory has stereo capability, handle this situation.
*     }
*   }
* @endcode
*
* Initialization example for the observer(from a class that implements
* MAccMonitorObserver interface).
* Observer starts to listen for headset connects:
* @code
* CAccMonitor* accMonitor = CAccMonitor::NewLC();
* RAccMonCapabilityArray capabilityArray;
* CleanupClosePushL( capabilityArray );
* capabilityArray.Append( KAccMonHeadset );
* accMonitor->StartObservingL( this, capabilityArray );
* CleanupStack::PopAndDestroy( &capabilityArray );
* CleanupStack::PopAndDestroy( accMonitor );
* @endcode
*
* Uninitialization example:
* @code
* accMonitor->StopObserving(); 
* @endcode
*
* Connected method implementation example:
* @code
* void CMyAccMonitorTest::Connected( CAccMonitorInfo* aAccessoryInfo )
*    {
*    // Notification about the connected accessory. aAccessoryInfo must
*    // be copied because the pointer is deleted after connected method
*    iAccessoryInfo->CopyL( aAccessoryInfo );
*    }
* @endcode
*
*  @lib AccMonitor.lib
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CAccMonitor ) : public CBase 
  {
public:

    /**
    * Symbian two phased constructor.
    */
    IMPORT_C static CAccMonitor* NewL();
    
    /**
    * Symbian two phased constructor. Puts the instance to cleanup stack.
    */
    IMPORT_C static CAccMonitor* NewLC();

public: 

    /**
    * Used to get the connected accessories which are returned in an 
    * RPointerArray that is typed to RConnectedAccessories.
    *
    * @param    aAccessoriesArray is a reference to an array that is 
    *           filled with CAccMonitorInfo instances which are accessories
    *           connected to the S60 device.
    * @leave    KErrNotFound if no accessories are connected.
    *           KErrNotReady if a device is not ready to start operation.
    * @return   The amount of connected accessories.
    */
    virtual TInt GetConnectedAccessoriesL(
      RConnectedAccessories& aAccessoriesArray ) const = 0;
    
    /**      
    * Sets observer to listen all connect/disconnect accessory notifications
    * and starts the observer.
    *
    * @param    aObserver is the pointer to the callback functions.
    * @leave    TInt KErrAlreadyExists if observer is active allready,
    *           otherwise KErrNone or standard Symbian error code.
    */
    virtual void StartObservingL( 
      MAccMonitorObserver* aObserver ) = 0;
                   
    /**
    * Sets observer to listen connect/disconnect accessory notifications
    * and starts the observer. Accessories that need to be listened can be
    * defined with CAccMonitorInfo. This CAccMonitorInfo represents an
    * instance of some connected or previously connected accessory.
    *
    * @param    aObserver is the pointer to the callback functions.
    * @param    aInfo is the instance of the accessory that needs to be
    *           listened. Accessory that needs to be listened is copied
    *           from connected accessories array. If an empty instance is set
    *           as the parameter no notifications are sent.
    * @leave    TInt KErrAlreadyExists if observer is active allready,
    *           otherwise KErrNone or standard Symbian error code.
    */
    virtual void StartObservingL( 
      MAccMonitorObserver* aObserver,
      const CAccMonitorInfo* aInfo ) = 0;
    
    /**
    * Sets observer to listen connect/disconnect accessory notifications
    * and starts the observer. Accessories that need to be listened can be
    * defined.
    *
    * @param    aObserver is the pointer to the callback functions.
    * @param    aCapabilityArray is an array of capabilities. Accessory 
    *           type that needs to be listened is constructed with this
    *           capability array. For exmple if KAccMonHeadset capability
    *           is inserted to the array the observer receives all
    *           notifications about connected/disconnected headsets.
    *           If an empty array is set as parameter no notifications are
    *           sent to the observer.
    * @leave    TInt KErrAlreadyExists if observer is active already,
    *           otherwise KErrNone or standard Symbian error code.
    */
    virtual void StartObservingL( 
      MAccMonitorObserver* aObserver,
      const RAccMonCapabilityArray& aCapabilityArray ) = 0;
    
    /**
    * Stops observing accessory connection status changes.
    * Listening will be automatically stopped when CAccMonintoringApi
    * object is deleted.
    */
    virtual void StopObserving() = 0;
    
    /**
    * Checks whether observer is already observing.
    *
    * @return   ETrue if observing
    */
    virtual TBool IsObserving() const = 0;
    
    /**
    * Gets the value for a capability that is defined in Accessory
    * Monitoring API.
    *
    * @param    aAccInfo is the accessory for which the value needs to be
    *           fetched.
    * @param    aCapability is capability which value needs to be
    *           fetched.
    * @param    aValue is the value for the capability.
    * @leave    KErrNotFound if no capability is not defined for this
    *           accessory, KErrArgument if this accessory is not
    *           connected, otherwise standard Symbian error code.
    */
    virtual void GetCapabilityValueL( 
      const CAccMonitorInfo* aInfo,
      const TUint32 aCapability,
      TInt& aValue ) const = 0;
    
protected:

    /**
    * Default Constructor 
    */
    CAccMonitor();
    
    /**
    * Symbian OS 2nd phase constructor 
    */
    virtual void ConstructL() = 0;
        
    };


/**
* The observer offers methods to observe connection/disconnection
* notifications from some defined accessory or all accessory
* connection/disconnection notifications.
*/
class MAccMonitorObserver
    {
public:
    
        /**
        * A callback method for accessory connection notifications.
        *
        * @param    aAccessoryInfo instance of the connected accessory.
        *           The content of the pointer must be copied to an instance
        *           CAccMonitorInfo because the original pointer is destroyed
        *           after the ConnectedL metdhod.
        */
        virtual void ConnectedL(
          CAccMonitorInfo* aAccessoryInfo ) = 0;
                
        /**
        * A callback method for accessory disconnection notifications.
        *
        * @param    aAccessoryInfo instance of the connected accessory.
        *           The content of the pointer must be copied to an instance
        *           CAccMonitorInfo because the original pointer is destroyed
        *           after the DisconnectedL metdhod.
        */
        virtual void DisconnectedL(
          CAccMonitorInfo* aAccessoryInfo ) = 0;
     
        /**
        * A callback for error situations.
        *
        * @param    Standard Symbian error code.
        */
        virtual void AccMonitorObserverError( TInt aError ) = 0;

    };
        
    

#endif // ACCMONITOR_H

// End of File
