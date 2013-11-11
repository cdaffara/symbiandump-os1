/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef BTGPSDEVICEMANAGER_H
#define BTGPSDEVICEMANAGER_H

//  INCLUDES

#include <e32base.h> 
#include "BTGPSDeviceListener.h"

// FORWARD DECLARATIONS
class CBTDevice;
class TBTSockAddr;

// CLASS DECLARATION

/**
*  Manager class for connected BT device. This class contains connected
*  BT device address, pairing information, device connection status and
*  device type. It also inform listeners if device connection status or
*  device type is changed.
*  
*/
class CBTGPSDeviceManager: public CBase
    {
    public: 

        /**
        * Two-phase construction.
        */
        static CBTGPSDeviceManager * NewL();

        /**
        * Destructor
        */
        virtual ~CBTGPSDeviceManager();

		/**
		* Get BT Device
		* @return Reference to CBTDevice object
		*/
		CBTDevice& BtDevice();
		
		/**
		* Get BT socket address
        * @param aAddr On return contains connected BT socket
        *        address
		*/
		void BtSockAddr(TBTSockAddr& aAddr) const;
		
		/**
		* Set BT socket info and device type. This function will also
		* do reset.
        * @param aAddr BT device  socket address.
        * @param aType BT device type
		*/
		void SetDeviceInfo(
		    const TBTSockAddr& aAddr,
		    const TBTDeviceType& aType);
		
		/**
		* Set port number
        * @param aPort Port number
		*/
		void SetPort(TInt aPort);
		
		/**
		* Get port number
        * @return Port number. Zero by default.
		*/
		TInt Port() const;

		/**
		* Get paired status
        * @return ETrue if the device is paried, EFalse otherwise.
		*/
		TBool IsPaired() const;

		/**
		* Set device paired status
        * @param aPaired Device pairing status
		*/
		void SetPaired(TBool aPaired = ETrue);
		
        /**
        * Get BT Device Type
        * @return Device type
        */
        TBTDeviceType DeviceType() const;
        
        /**
        * Check if GPS is connected
        * @return ETrue if the connected device is a GPS device
        */
        TBool IsGpsConnected() const;

        /**
        * Set BT Device Type
        * @param aType BT device type
        */
        void SetBTDeviceType(TBTDeviceType aType);
        
        /**
        * Get BT Device connect status
        * @return BT device connection status.
        */
        TBTDeviceConnectStatus DeviceConnectStatus() const;
        
        /**
        * Set device connection status
        * @param aStatus BT device connection status.
        * @param aErr Last operation error code if connection
        *        status is EBTDeviceConnectError.
        */
        void SetBTDeviceConnectStatus(
            TBTDeviceConnectStatus aStatus,
            TInt aErr = KErrNone);

        /**
        * Add device status change listener
        * @param aListener Reference to listener.
        */
        void AddListenerL(MBTGPSDeviceListener& aListener);

        /**
        * Remove listener
        * @param aListener Reference to listener.
        */
        void RemoveListener(MBTGPSDeviceListener& aListener);

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSDeviceManager();
        
        /**
        * Issue status change event to listeners
        */
        void StatusChanged();
        
    private:
        //Status
        CBTDevice* iBtDevice;

        //Port
        TInt iPort;
        
        //Paired
        TBool iPaired;

        //BT Device Type
        TBTDeviceType iType;
        
        //BT Device connection status
        TBTDeviceConnectStatus iConnectStatus;
        
        //Last operation error code
        TInt iError;
        
        //Listner array
        RArray<MBTGPSDeviceListener*> iListenerArray;

    };
#endif
// End of File

