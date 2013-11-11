/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSREQUESTCONTROLLER_H
#define CPOSREQUESTCONTROLLER_H

#include "EPos_CPosDefaultPositioner.h"
#include "epos_mposrequestorlistener.h"
#include "epos_mpospsyfixstatelistener.h"
#include "epos_mpospsylistlistener.h"

class TPositionInfoBase;
class CPositioner;
class CPosPsyListHandler;
class CPosPsyFixStateManager;
class CPosConstManager;
class CPosExternalGpsMonitor;
class CPosRequestor;
class MPosModuleSettingsManager;
class MPosModuleStatusManager;
class TPosModulesEvent;


/**
* The CPosRequestController is service provider implemented as an active object
* that controls position requests to the positioners in priority order from
* the location settings. Although it is an active object, Cancel should not be
* called by users. Instead, CancelNotifyPositionUpdate() should be called to 
* cancel an out-standing position request. This class also listens to events
* in Location Settings for changes in modules priorities or properties. 
*/
class CPosRequestController : 
        public CBase, 
        public MPosRequestorListener,
        public MPosPsyFixStateListener,
        public MPosPsyListListener
    {
    public: // Constructors and destructor
              
        /**
        * Constructor.
        * @param aDefaultPositioner Pointer to default positioner to
        * @param aModulesSettings Pointer to modules settings
        */
        static CPosRequestController* NewL(
            CPosDefaultPositioner& aDefaultPositioner,
            MPosModuleSettingsManager& aSettingsManager,
            MPosModuleStatusManager& aModuleStatusManager
        );

        /**
        * Destructor.
        */
        ~CPosRequestController();

	public: // New functions

        /**
        * Analogous to CPositioner.
        * Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        *
        */
        void NotifyPositionUpdate(
            TPositionInfoBase& aPosInfo,
            TRequestStatus& aStatus
        );

        /**
        * Analogous to CPositioner.
        * Cancels position info request. Cancel() should not be used. 
        * This class works as a service provider from user perspective.
        */
        void CancelNotifyPositionUpdate();

        /**
        * Analogous to CPositioner.
        * Cancels position info request with an error. Used by LocServer
        * to indicate to PSY that this is not a user cancel 
        * This class works as a service provider from user perspective.
        */
        void CancelNotifyPositionUpdate(TInt aCancelReason);
        
        /**
        * Analogous to CPositioner, but does not leave
        * Initiate a tracking session.
        *
        * @param aInterval Interval for position requests.
        */
        void StartTracking(
            const TTimeIntervalMicroSeconds& aInterval
        );

        /**
        * Stops tracking for all PSY:s that are in the list.
        */
        void StopTracking();

    protected: // Functions from base classes

        /**
        * From MPosRequestorListener.
        */
        virtual void LocationRequestCompleted( 
            TPositionModuleId aModuleId,
            TInt aErr, 
            const TPositionInfoBase& aPosInfo,
            TBool aIsPosInfoUpToDate );

        /**
        * From MPosPsyFixStateListener
        */
        virtual void PsyFixStateChanged( 
            TPositionModuleId aModuleId,
            CPosPsyFixStateManager::TPsyFixState aFixState ); 

        /**
        * From MPosPsyListListener
        */
        virtual void PsyListChanged( 
            const TPosPsyListChangeEvent& aEvent );


    private:
    
        /**
        * Start position update
        */
        void StartPositionUpdateL();

        /**
        * Iterates through available plugins looking for the next
        * to try
        */
        void TryNextPositioner();
        
        /**
        * Try to load and make location retquest to the specific
        * PSY.
        */
        void TryPositionerL( TPositionModuleId aPsyId );

        /**
        * Completes a request if not already completed.
        */
        void CompleteRequest(TInt aCompleteCode);
        
        /**
        * Clear location request to all PSYs
        */
        void ClearLocationRequests();

        /**
         *  Returns current requestor
         *  This function returns NULL if the requestor is not found
         */
        CPosRequestor* GetRequestor(
            TPositionModuleId aPsyId );
            
        /**
        * Unload PSY
        */
        void UnloadRequestor( TPositionModuleId aPsyId );

        /**
        * Cancel location request from a requestor
        */
        void CancelRequest( TInt aIndex, TInt aCancelReason);
        
        /**
        * Cancel location request by UID
        */
        void CancelRequest( TPositionModuleId aPsyId );
        
        /**
        * Returns the Module Id of the current positioner
        * @return the Module Id of currently used Positioner. KNullUid if none.
        */
        TPositionModuleId CurrentPositionerId();
        
        /**
        * Check if there is location request on going in 
        * any loaded PSY.
        */
        TBool IsLocationRequestOnGoing() const;
        
        /**
        * Check if there is location request on going in 
        * any loaded Network based PSY.
        */
        TBool IsLocationRequestOnGoingOnNetworkPsy() const;
        
        /**
        * Copy TPositionInfoBase class
        */
        static TInt CopyPosInfoClass(
            const TPositionInfoBase& aSrc,
            TPositionInfoBase& aDst );
            
        /**
        * Cleanup timer static callback function
        */
        static TInt CleanupTimeoutCallback( TAny* aAny );
        
        /**
        * Handle clenaup timer timeout
        */
        void CleanupTimeout();

        /**
        * C++ default constructor.
        */
        CPosRequestController(
            CPosDefaultPositioner& aDefaultPositioner,
            MPosModuleStatusManager& aModuleStatusManager
            );

        /**
        * EPOC constructor.
        */
        void ConstructL(
            MPosModuleSettingsManager& aSettingsManager );

        // By default, prohibit copy constructor
        CPosRequestController(const CPosRequestController&);
        // Prohibit assigment operator
        CPosRequestController& operator= (const CPosRequestController&);

        // Debug and trace helpers
        void Panic(TInt aPanic);

    private: // Data
        //Reference to default positioner
        CPosDefaultPositioner& iDefaultPositioner;
        
        //Reference to module status manager
        MPosModuleStatusManager& iModuleStatusManager;
        
        //Boolean value indicate PSY list is valid
        TBool iPsyListValid;
        
        //Psy list in priority list
        RArray< TPositionModuleId > iPsyList;
        
        //Positioner objects array
        RPointerArray< CPosRequestor > iRequestorArray;

        //Location request status
        TRequestStatus*	iPosRequestStatus;
        
        //Position info base class for location request
        TPositionInfoBase* iPosInfo;
        
        //Pointer to PSY list handler
        CPosPsyListHandler* iPsyListHandler;
        
        //Pointer to PSY fix state manager
        CPosPsyFixStateManager* iPsyFixStateManager;
        
        //External GPS PSY monitor
        CPosExternalGpsMonitor* iExtGpsPsyMonitor;
        
        //Const manager
        CPosConstManager* iConstManager;
        
        //Error code returned from first PSY
        TInt iFirstResult;
        
        //Index of the current trying PSY in PSY list. If current
        //PSY can't give a fix, default proxy shall try next PSY
        //in the PSY array.
        TInt iCurrentPsy;
        
        //Cleanup timer
        CPeriodic* iCleanupTimer;
        
    };

#endif      // CPOSREQUESTCONTROLLER_H  
            
// End of File
