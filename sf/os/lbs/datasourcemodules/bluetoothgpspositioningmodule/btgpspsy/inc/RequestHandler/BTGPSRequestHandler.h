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




#ifndef BTGPSREQUESTHANDLER_H
#define BTGPSREQUESTHANDLER_H

//  INCLUDES

#include <e32std.h>
#include <e32base.h>
#include <lbspositioninfo.h>
#include "BTGPSLocationFixListener.h"
#include "BTGPSDeviceListener.h"

// FORWARD DECLARATIONS

class CBTGPSConstantsManager;
class CBTGPSConnectManager;
class CBTGPSInitManager;
class CBTGPSRequestManager;
class CBTGPSSettingManager;
class CBTGPSDeviceManager;
class CBTGPSPsyConnectionManager;
class MBTGPSPositionerExt;
class CBTGPSFix;
class CBTGPSNokDeviceHandler;
class CBTGPSModuleStatusHandler;
class CBTGPSEnvChangeHandler;
class CBTGPSPowerManager;
class CBTGPSDeviceListManager;

// CLASS DECLARATION

/**
*  Singleton CBTGPSRequestHandler controls the operation of the PSY.
*  The request handler is the singleton object of PSY and it owns other manager
*  objects. 
*  
*
*/
class CBTGPSRequestHandler :
    public CBase,
    private MBTGPSLocationFixListener,
    private MBTGPSDeviceListener
    {
    public:  // Constructors and destructor
    
        /**
        * Singleton can be only created through GetInstanceL.
        */
        static CBTGPSRequestHandler* GetInstanceL();
        
    public: // New functions

        /**
        * Return reference of singleton. This funciton will panic if
        * singleton has not been constructed.
		* @return Reference to request handler singleton objects.
        */
        static CBTGPSRequestHandler& Instance();

        /**
        * Get reference to Constants Manager
		* @return Reference to contants manager.
        */
        static CBTGPSConstantsManager& ConstantsManager();

        /**
        * Starts the location retrieval from GPS device
        * @param aPSY Reference to the CBTGPSPositioner
        */
        void AcquireLocation(MBTGPSPositionerExt* aPSY);

        /**
        * Cancels the location retrieval
        * @param aPSY Reference to the CBTGPSPositioner
        */
        void CancelAcquireLocation(MBTGPSPositionerExt* aPSY);

        /**
        * Registeres the PSY instance to the request handler
        * @param aPSY Reference to the CBTGPSPositioner
        */
        void RegisterPSYL(MBTGPSPositionerExt* aPSY);

        /**
        * Unregisteres the PSY instance to the request handler
        * @param aPSY Reference to the CBTGPSPositioner
        */
        void UnregisterPSY(MBTGPSPositionerExt* aPSY);

        /**
        * Starts the tracking session
        * @param aPSY Reference to the CBTGPSPositioner starting the session
        * @param aInterval The location retrieval interval.
        */
        void TrackingSessionStartL(MBTGPSPositionerExt* aPSY,
                                  const TTimeIntervalMicroSeconds& aInterval);

        /**
        * Stops the tracking session
        * @param aPSY Reference to the CBTGPSPositioner stopping the session
        */
        void TrackingSessionStop(MBTGPSPositionerExt* aPSY);

        /**
        * Returns the fix that fulfills the requirements aAcceptPartial 
        * and aStatus.
        * @param aFix A reference to the pointer to fix
        * @param aMaxAge The criteria for maximum age for the fix
        * @param aAcceptPartial The criteria for partial update
        * @return KErrNone if the fix fulfills the requirements, 
		*         otherwise KErrNotFound.
        */
        TInt LastLocation(
            const CBTGPSFix*& aFix,
            const TTime& aMaxAge,
            const TBool aAcceptPartial);

    private:  // New functions
        /**
        * Acquire location(leave function)
        */
        void AcquireLocationL(MBTGPSPositionerExt* aPSY);

        /**
        * Complete request
        */
        void CompleteRequest(
            MBTGPSPositionerExt& aPSY, 
            TInt aErr);
        
        /**
        * Completes all pending requests.
        * @param aStatus The status to complete with.
        */    
        void CompleteAllRequests(TInt aErr);

        /**
        * Reports the device status to the MLFW
        * @param aStatus The device status
        */    
        void DeviceStatus(TInt aStatus);

        /**
        * Returns the device status
        * @return The device status
        */    
        TInt GetDeviceStatus();

        /**
        * From MBTGPSLocationFixListener
        */
        void HandleLocationFixUpdate(const CBTGPSFix& aFix);

        /**
        * From MBTGPSDeviceListener
        */
        virtual void BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt aErr=KErrNone);
        
    private: // Construction and destruction

        /**
        * Two-phased constructor.
        */
        static CBTGPSRequestHandler* NewL();

        /**
        * C++ default constructor.
        */
        CBTGPSRequestHandler();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        virtual ~CBTGPSRequestHandler();


    private:    // Data

        //Constants manager
        CBTGPSConstantsManager* iConstantsManager;
        
        //Setting manager
        CBTGPSSettingManager* iSettingManager;
        
        //BT Device Manager
        CBTGPSDeviceManager* iDeviceManager;
        
        //BT GPS connection manager
        CBTGPSConnectManager* iConnectManager;
        
        //BT GPS init manager
        CBTGPSInitManager* iInitManager;
        
        //PSY connection manager
        CBTGPSPsyConnectionManager* iPsyConnectionManager;

        //Location Request Manager
        CBTGPSRequestManager* iRequestManager;
        
        //Nok device handler
        CBTGPSNokDeviceHandler* iNokDeviceHandler;
        
        //Module status handler
        CBTGPSModuleStatusHandler* iModuleStatusHandler;
        
        //Power manager
        CBTGPSPowerManager* iPowerManager;
        
        //Enviornment change handler
        CBTGPSEnvChangeHandler* iEnvChangeHandler;
        
        //Device List Manager
        CBTGPSDeviceListManager* iDeviceListManager;

    };

#endif      // BTGPSREQUESTHANDLER_H

// End of File
