/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Singleton Data Storage of Accessory Server
*
*/


#ifndef CACCSRVSERVERMODEL_H
#define CACCSRVSERVERMODEL_H

//  INCLUDES
#include "AccPolAccessoryMode.h"
#include "AccSrvNotificationQueue.h"//TMsgID
#include "AccPolCapabilityStorage.h"

#include <AccessoryTypes.h>
#include <AccPolGenericID.h>
#include <AccPolGenericIDArray.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib AccServer.lib
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAccSrvServerModel ) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAccSrvServerModel* NewL();

        /**
        * Destructor.
        */
        virtual ~CAccSrvServerModel();

    public: // New functions

        /**
        * Check is the accessory allready connected.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @return ETrue if the accessory is connected, EFalse otherwise.
        */
        TBool IsConnected( const TAccPolGenericID& aGenericID ) const;

        /**
        * Check is the accessory allready connected.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @return void
        * @exception KErrArgument if the accessory is not connected.
        */
        void IsConnectedL( const TAccPolGenericID& aGenericID ) const;
        
        /**
        * Check is the accessory allready connected or detected.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected or detected accessory.
        * @return TBool ETrue if connected otherwise EFalse
        * @exception KErrArgument if the accessory is not connected or detected.
        */
        TBool IsConnectedOrDetected( const TAccPolGenericID& aGenericID ) const;            
        
        /**
        * Check is the accessory allready connected or detected.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected or detected accessory.
        * @return void
        * @exception KErrArgument if the accessory is not connected or detected.
        */
        void IsConnectedOrDetectedL( const TAccPolGenericID& aGenericID ) const;    
    

        /**
        * Add a new connection to array of active connections.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @return void
        */
        void AddConnectionL( const TAccPolGenericID& aGenericID );
        
       /**
        * Update a generic id of active connections.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @return void
        */
        void UpdateConnectionL( const TAccPolGenericID& aGenericID );        

        /**
        * Add a new physical connection to array of detected accessories.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the detected accessory.
        * @return void
        */
        void AddPhysicalConnectionL( const TAccPolGenericID& aGenericID );

        /**
        * Remove an active connection from array of active connections.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the connected accessory.
        * @param aDisconnect If ETrue, removed accessory is stored.
        *                    Used while disconnecting accessory.
        * @return void
        * @see LastDisconnectedAccessory()
        */
        void RemoveConnectionL( const TAccPolGenericID& aGenericID,
                                TBool aDisconnect );

        /**
        * Get last disconnected accessory.
        *
        * @since S60 3.1
        * @return Gereric Id of the last disconnected accessory.
        * @see ClearLastDisconnectedAccessory()
        */
        const TAccPolGenericID& LastDisconnectedAccessory();

        /**
        * Clear last disconnected accessory.
        *
        * @since S60 3.1
        * @return void
        */
        void ClearLastDisconnectedAccessory();

        /**
        * Get last connected accessory.
        *
        * @since S60 3.1
        * @param aGenericID On succesful return, contains the Generic ID of
        *                   last connected accessory.
        * @return void
        */
        void GetLastConnectedAccessoryL( TAccPolGenericID& aGenericID ) const;

        /**
        * Get last connected wired accessory.
        *
        * @since S60 3.1
        * @param aGenericID On succesful return, contains the Generic ID of
        *                   last connected wired accessory.
        * @return TBool     On successful return will be ETrue, else EFalse
        */
        TBool GetLastConnectedWiredAccessory( TAccPolGenericID& aGenericID ) const;
        

        /**
        * Remove the physical connection from array of detected accessories.
        *
        * @since S60 3.1
        * @param aGenericID Generic ID of the detected accessory.
        * @return index in the array
        */
        TInt RemovePhysicalConnection( const TAccPolGenericID& aGenericID );

        /**
        * Get current connection status, ie. all active connections.
        *
        * @since S60 3.1
        * @param aGenericIDArray On succesful return, contains all active connections.
        * @return void
        */
        void CurrentConnectionStatusL( TAccPolGenericIDArray& aGenericIDArray );

        /**
        * Finds matching Generic ID according to Unique DB ID.
        * Search is done among the all active connections.
        *
        * @since S60 3.1
        * @param aUniqueID  Unique ID of the connected accessory.
        * @param aGenericID On succesful return, contains the Generic ID.
        * @return void
        */
        void FindWithUniqueIDL( TInt aUniqueID,
                                TAccPolGenericID& aGenericID ) const;

        /**
        * Finds matching Generic ID according to device address.
        * Search is done among the all active connections.
        *
        * @since S60 3.1
        * @param aDeviceAddress Device address to be used for matching.
        * @param aUniqueID      On succesfull return, contains Unique ID
        *                       of the found Generic ID.
        * @return index of the found Generic ID.
        */
        TInt FindWithDeviceAddressL( TUint64 aDeviceAddress,
                                     TInt& aUniqueID ) const;

        /**
        * Get current Accessory Mode.
        *
        * @since S60 3.1
        * @return The current accessory mode.
        */
        TAccPolAccessoryMode AccessoryMode() const;

        /**
        * Set current Accessory Mode.
        *
        * @since S60 3.1
        * @param aAccMode Accessory mode to be stored.
        * @return ETrue if Accessory Mode is changed, otherwise EFalse.
        */
        TBool SetAccessoryMode( TAccPolAccessoryMode aAccMode, TInt aUniqueID );

        /**
        * 
        *
        * 
        * 
        */
        TInt UniqueID() const {return iUniqueID;}
        
        /**
        * Sets ASYs loading status flag to ETrue. This means that all ASYs are
        * loaded or loading timeout has occurred.
        *
        * @since S60 3.1
        * @return void
        */
        void SetASYsLoadCompleted();

        /**
        * Gets ASYs loading status.
        *
        * @since S60 3.1
        * @return ASYs loading status.
        * @see SetASYsLoadCompleted()
        */
        TBool ASYsLoadCompleted() const;

        /**
        * Set current requested audio type.
        * Used in Audio Link opening and closing.
        *
        * @since S60 3.1
        * @param aAudioType Audio type to be stored.
        * @return void.
        * @see AudioType()
        */
        void SetAudioType( TAccAudioType aAudioType );

        /*
        * Get current requested audio type.
        * Used in Audio Link opening and closing.
        *
        * @since S60 3.1
        * @return current requested audio type.
        * @see SetAudioType()
        */
        TAccAudioType AudioType() const;

        /**
        * Set device type. Used to store user setting related
        * to TTY/Loopset detection.
        *
        * @since S60 3.1
        * @param aDeviceType Device type to set. HW device bit masks are
        *                    declared in AccessoryTypes.h.        *
        * @return void
        * @see SetAudioType()
        */
        void SetDeviceType( TUint32 aDeviceType );

        /*
        * Get device type.
        *
        * @since S60 3.1
        * @return Current HW device type.
        * @see SetDeviceType()
        */
        TUint32 DeviceType() const;
        
        /*
        * Get supported device types.
        *
        * @since S60 3.1
        * @return Currently supported HW device types.
        */
        TUint32 SupportedDevicesL();        

        /**
        * This is data storage
        * Used between CAccSrvSubControl and CAccSrvSubSingleConnection
        * subsessions. SetValueL data is stored here and used when
        * AccessoryValueChanged notification is checked and possibly completed.
        *
        * @since S60 3.1
        * @param aGenericId   Generic ID to store.
        * @param aNameRecord  NameRecord to store.
        * @param aValueTint   Integer value to store.
        * @param aValueTBool  Boolean value to store.
        * @param aValueObject Object value to store.
        * @return void
        */
        void SetValueData( TAccPolGenericID& aGenericId,
                           TAccPolNameRecord& aNameRecord,
                           TAccValueTypeTInt aValueTint,
                           TAccValueTypeTBool aValueTBool,
                           TAccValueTypeObject aValueObject );

        /**
        * Getter for Generic ID ValueData.
        *
        * @since S60 3.1
        * @return Stored Generic ID.
        * @see SetValueData()
        */
        TAccPolGenericID ValueDataGid() const;

        /**
        * Getter for NameRecord ValueData.
        *
        * @since S60 3.1
        * @return Stored NameRecord.
        * @see SetValueData()
        */
        TAccPolNameRecord ValueDataNameRecord() const;

        /**
        * Getter for Integer ValueData.
        *
        * @since S60 3.1
        * @return Stored Integer value.
        * @see SetValueData()
        */
        TAccValueTypeTInt ValueDataTInt() const;

        /**
        * Getter for Boolean ValueData.
        *
        * @since S60 3.1
        * @return Stored Boolean value.
        * @see SetValueData()
        */
        TAccValueTypeTBool ValueDataTBool() const;

        /**
        * Getter for Object ValueData.
        *
        * @return Stored Boolean value.
        * @see SetValueData()
        */
        TAccValueTypeObject ValueDataObject() const;

        /**
        * Add ASYs thread Id to iRegisteredASYs array.
        * This array contains all known non idle ASYs.
        *
        * @since S60 3.1
        * @param aThreadID ASYs thread ID to be stored.
        * @return void
        */
        void AddASYThreadIdL( TUint aThreadID );

        /**
        * Creates mapping between Generic ID and ASY.
        * All commands related to certsain Generic ID are forwarded to correct
        * ASY according to this mapping.
        *
        * @since S60 3.1
        * @param aThreadID ASYs thread ID.
        * @param aUniqueID Unique identifier of a Generic ID.
        * @return void
        */
        void StoreASYThreadIDL( TUint aThreadID, TInt aUniqueID );

        /**
        * Removes mapping between Generic ID and ASY.
        *
        * @since S60 3.1
        * @param aUniqueID Unique identifier of a Generic ID.
        * @return void
        * @see StoreASYThreadID()
        */
        void RemoveASYThreadID( TInt aUniqueID );

        /**
        * Checks is the mapping between ASY and Generic ID done.
        *
        * @since S60 3.1
        * @param aThreadID ASYs thread ID.
        * @param aUniqueID Unique identifier of a Generic ID.
        * @return ETrue if mapping between ASY and Generic ID done for ASY
        *         given as a parameter, otherwise EFalse.
        * @see StoreASYThreadID()
        */
        TBool IsMyRequest( TUint aThreadID, TInt aUniqueID );

        /**
        * Create a Thread Id/message filter.
        * Used to filter notifications to certain thread.
        *
        * E.g. NotifyAccessoryAudioLinkOpened() notification is not sent to
        *      the thread where AccessoryAudioLinkOpen() message has been sent.
        *
        * @since S60 3.1
        * @param aThreadID Thread ID of the message(aMsgID) sender.
        * @param aMsgID    Message in question.
        * @return void
        * @see FindAndDeleteMsgThreadIDFilter()
        */
        void SetMsgThreadIDFilterL( TUint aThreadID, TMsgID aMsgID );

        /**
        * Find and delete a Thread Id/message filter.
        * Used to filter notifications to certain thread.
        *
        * E.g. NotifyAccessoryAudioLinkOpened() notification is not sent to
        *      the thread where AccessoryAudioLinkOpen() message has been sent.
        *
        * @since S60 3.1
        * @param aThreadID Thread ID to find.
        * @param aMsgID    Message to find .
        * @return ETrue if Thread Id/message filter is found and deleted,
        *         otherwise EFalse
        * @see SetMsgThreadIDFilter
        */
        TBool FindAndDeleteMsgThreadIDFilter( TUint aThreadID, TMsgID aMsgID );

        /**
        * Store information for cancelling ConnectAccessory().
        * This method is used when ConnectAccessory()
        * asynchronous call is cancelled before its completed.
        *
        * @since S60 3.1
        * @param aUniqueID Defines the accessory which connection is cancelled.
        * @return void
        */
        void SetCancelledConnectAccessoryL( TInt aUniqueID );

        /**
        * Store information for cancelling ConnectAccessory().
        * This method is used when ConnectAccessory()
        * asynchronous call is cancelled before its completed.
        *
        * @since S60 3.1
        * @param aDeviceAddress Defines the accessory which connection is cancelled.
        * @return void
        */
        void SetCancelledConnectAccessoryL( TUint64 aDeviceAddress );

        /**
        * Checks if the accessory connection is cancelled.
        *
        * @since S60 3.1
        * @param aUniqueID Defines the accessory which connection is cancelled.
        * @return ETrue if the accessory connection is cancelled,
        *         otherwise EFalse
        * @see SetCancelledConnectAccessory
        */
        TBool FindAndRemoveCancelledConnectAccessory( TInt aUniqueID );
        
        /**
        * Returns global capability storage.
        *
        * @since S60 3.1
        * @return CCapabilityStorage& Return reference to global capability storage,
        *         
        */
        CCapabilityStorage& CapabilityStorage();
        
        /**
        * Set AccessoryAudioLinkOpen() cancel information.
        *
        * @since S60 3.1
        * @param aCancelled ETrue if AccessoryAudioLinkOpen() is cancelled,
        *                   EFalse when cancel is handled.
        * @return void
        * @see AudioLinkOpenCancelled
        */
        void SetAudioLinkOpenCancelled( TBool aCancelled );
        
        /**
        * Get AccessoryAudioLinkOpen() cancel information.
        *
        * @since S60 3.1
        * @return ETrue if AccessoryAudioLinkOpen() is cancelled,
        *         EFalse when cancel is handled.
        * @see SetAudioLinkOpenCancelled
        */
        TBool AudioLinkOpenCancelled();
        
        /**
        * Set AccessoryAudioLinkClose() cancel information.
        *
        * @since S60 3.1
        * @param aCancelled ETrue if AccessoryAudioLinkClose() is cancelled,
        *                   EFalse when cancel is handled.
        * @return void
        * @see AudioLinkCloseCancelled
        */
        void SetAudioLinkCloseCancelled( TBool aCancelled );
        
        /**
        * Get AccessoryAudioLinkClose() cancel information.
        *
        * @since S60 3.1
        * @return ETrue if AccessoryAudioLinkClose() is cancelled,
        *         EFalse when cancel is handled.
        * @see SetAudioLinkCloseCancelled
        */
        TBool AudioLinkCloseCancelled();
        
        /**
        * Set server model to idle detected state. It means that phone is booted up and
        * accessory selection dialog can be shown
        * @since S60 3.1
        * @return void
        */
        void SetIdleDetected();
        
        /**
        * Get idle detected state
        * @since S60 3.1
        * @return TBool Is idle detected already
        */
        TBool IdleDetected();        
        
        /**
        * Waits idle state. Complete message when idle is detected
        * @since S60 3.1
        * @return TInt KErrGeneral if idle is already detected
        */
        TInt WaitForIdle( TRequestStatus* aStatus );

        /**
        * Cancels Idle wait request.
        * @since S60 3.1
        * @return void
        */
        void CancelWaitForIdle();
        
        /**
        * Gets audio link open indicator for bluetooth
        *
        * @since S60 3.2
        * @return Value of the indicator.
        */
        TBool BluetoothAudioLinkOpenReqPostponed();
        
        /**
        * Sets audio link open indicator for bluetooth
        *
        * @since S60 3.2
        * @param aValue Value of the indicator.
        */
        void SetBluetoothAudioLinkOpenReqPostponed( TBool aValue );
        
    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * Structure to store a Thread Id/message filter.
        */
        struct TAccSrvThreadIDFilter
            {
            TUint iThreadID;
            TMsgID iMsgID;
            };

        /**
        * C++ default constructor.
        */
        CAccSrvServerModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Search Generic ID from array of active connections (according HWDeviceID).
        */
        TInt FindGID( const TAccPolGenericID& aGenericID,
                      const RArray<TAccPolGenericID>& aArray ) const;

    public:     // Data

    protected:  // Data

    private:    // Data

        //GenericIDArray of active connections
        RArray<TAccPolGenericID> iConnectionArray;

        //GenericIDArray of physical connections
        RArray<TAccPolGenericID> iPhysicalConnectionArray;

        //Last disconnected accessory
        TAccPolGenericID iLastDisconnectedAccessory;

        //Current Accessory Mode
        TAccPolAccessoryMode iAccMode;

        //Preferred audio output status is assigned for this uniqueID.
				TInt iUniqueID;
        // Flag for indicating ASYs loading complettion
        TBool iASYsLoadCompleted;

        //Requested audio type while
        //Request to open accessory audio connection is made
        TAccAudioType iAudioType;

        // Device type set from UI that's used to identify some
        // accessories on connection phase.
        TUint32 iDeviceType;

        //ValueChangedNotifyData
        TAccPolGenericID    iValueDataGenericId;
        TAccPolNameRecord   iValueDataNameRecord;
        TAccValueTypeTInt   iValueDataTint;
        TAccValueTypeTBool  iValueDataTBool;
        TPtr8               iValueDataObject;

        //Thread IDs of all known ASYs
        RArray<TUint>      iRegisteredASYs;

        //Thread ID of ASY and GID mapping tables.
        RArray<TUint> iASYThreadID;
        RArray<TInt>  iASYUniqueID;

        //Thread Id/message filter array
        RArray<TAccSrvThreadIDFilter> iThreadIDFilterArray;

        //Unique DB ID of the cancelled accessory connection
        RArray<TInt> iCancelledConnectAccessory;

        //global capability storage
        CCapabilityStorage * iCapabilityStorage;
        
        //AccessoryAudioLinkOpen() cancel information
        TBool iAudioLinkOpenCancelled;
        
        //AccessoryAudioLinkClose() cancel information
        TBool iAudioLinkCloseCancelled;
        
        //supported devices
        TUint32 iSupportedDevices;

        //supported devices
        TBool iIdleDetected;

        //supported devices
        TRequestStatus* iIdleStatus;
        
        //Indicates for the bluetooth control if audio link open is in progress
        TBool iBluetoothAudioLinkOpenReqPostponed;
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // CACCSRVSERVERMODEL_H

// End of File
