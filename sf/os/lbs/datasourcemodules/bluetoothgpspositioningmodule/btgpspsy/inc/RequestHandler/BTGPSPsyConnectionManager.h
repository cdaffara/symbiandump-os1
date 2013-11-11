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




#ifndef BTGPSPSYCONNECTIONMANAGER_H
#define BTGPSPSYCONNECTIONMANAGER_H

//  INCLUDES

#include <e32base.h> 


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MBTGPSPositionerExt;
class CBTGPSFix;
class TPositionModuleStatus;
class MBTGPSPsyConnectionListener;
    

/**
*  This class defines PSY connection information manager. It manage PSY connection
*  informaiton as well as location request and tracking status of each PSY. 
*
*/
class CBTGPSPsyConnectionManager: public CBase
    {
    public: 

        /**
        * Two-phase construction.
        */
        static CBTGPSPsyConnectionManager * NewL();

        /**
        * Destructor
        */
        virtual ~CBTGPSPsyConnectionManager();

    public:
        /**
        * Register PSY
		* @param aPSY Pointer to new positioner
        */
        void RegisterPSYL(MBTGPSPositionerExt* aPSY);

        /**
        * UnregisterPSY
		* @param aPSY Pointer to positioner
        */
        void UnregisterPSY(MBTGPSPositionerExt* aPSY);

        /**
        * PSY start to acquire location
		* @param aPSY Pointer to positioner
        */
        void AcquireLocation(MBTGPSPositionerExt* aPSY);

        /**
        * PSY cancel a location request
		* @param aPSY Pointer to positioner
        */
        void CancelAcquireLocation(MBTGPSPositionerExt* aPSY);

        /**
        * Complete request with error code. This function is called
		* when the locaitn request shall be completed with error code.
		*
		* @param aPSY Reference to positioner
		* @param aErr The error code that location request shall be completed.
        */
        void CompleteRequest(
            MBTGPSPositionerExt& aPSY,
            TInt aErr);
        
        /**
        * Complete all requests with error code
		* @param aErr Error code
        */
        void CompleteAllRequests(TInt aErr);
        
        /**
        * Location fix update. Inform all PSY that there is location information
		* updates.
		* @param aFix Reference to location fix information.
        */
        void LocationFixUpdate(const CBTGPSFix& aFix);
        
        /**
        * Report posintion module status.
		* @param aStatus new position module status.
        */
        void ReportStatus(const TPositionModuleStatus& aStatus);
            
        /**
        * Get number of connected PSY
		* @return The number of connected PSy
        */
        TInt Count() const;

        /**
        * Get number of active location requests
		* @return The number of PSY that has location requests on going.
        */
        TInt LocationRequestCount() const;
        
        /**
        * Get number of tracking session
		* @return The number of PSY that has tracking session on going.
        */
        TInt TrackingSessionCount() const;

        /**
        * Inform a tracking session is started
		* @param aPSY Pointer to PSY that starts a tracking session.
		* @param aInterval The tracking interval.
        */
        void TrackingSessionStartL(
            MBTGPSPositionerExt* aPSY,
            TTimeIntervalMicroSeconds aInterval);

        /**
        * Tracking session stops
		* @param aPSY Pointer to positioner
        */
        void TrackingSessionStop(MBTGPSPositionerExt* aPSY);
        
        /**
        * Add PSY connection information change listener
		* @param aListener Reference to listener
        */
        void AddListenerL(MBTGPSPsyConnectionListener& aListener);
        
        /**
        * Remove PSY connection information change listener
		* @param aListener Reference to listener.
        */
        void RemoveListener(MBTGPSPsyConnectionListener& aListener);
    
    private:
		/**
		* Structer of connected PSY and it's status
		*/
        struct TPSYStatusStruct
            {
            //Pointer to connected PSY
            MBTGPSPositionerExt* iPsy;
            
            //If PSY has a location request on going
            TBool iLocationRequest;
            
            //Tracking Interval. If PSY doesn't has a tracking session,
            //the value is -1
            TTimeIntervalMicroSeconds iTrackingInterval;
            };
        
    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSPsyConnectionManager();
        
        /**
        * Find PSY Status from PSY array
        */
        TInt FindPsy(MBTGPSPositionerExt* aPSY);
        
        /**
        * Complete location request with fix or error code
        */
        void CompleteLocationRequest(
            TInt aIndex,
            const CBTGPSFix* aFix,
            TInt aErr);
            
        /**
        * Inform listeners the connection status is changed
        */
        void InformListenersChange();
        
    private:
        //Connected PSY array
        RArray<TPSYStatusStruct> iPsyArray;
        
        //Listener array
        RArray<MBTGPSPsyConnectionListener*> iListenerArray;

    };
#endif
// End of File

