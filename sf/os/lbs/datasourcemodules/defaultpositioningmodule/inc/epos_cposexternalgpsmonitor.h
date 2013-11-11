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



#ifndef CPOSEXTERNALGPSMONITOR_H
#define CPOSEXTERNALGPSMONITOR_H

//  INCLUDES
#include <e32base.h>
#include "epos_mposrequestorlistener.h"
#include "epos_mpospsylistlistener.h"

// FORWARD DECLARATIONS
class CPositioner;
class CPosPsyListHandler;
class CPosPsyFixStateManager;
class CPosRequestor;
class CPosConstManager;
class MPosModuleStatusManager;

// CLASS DECLARATION

/**
* The CPosExternalGpsMonitor minotors fix state of external GPS PSYs. It makes
* location requests to external GPS PSYs periodically, and if a external GPS
* PSY is able to give a fix, it will be moved to the front of dynamic list. 
*/
class CPosExternalGpsMonitor : 
        public CBase, 
        public MPosRequestorListener,
        public MPosPsyListListener
    {
    public: // Constructors and destructor
              
        /**
        * Get an instance of CPosExternalGpsMonitor.    
        *
        * @param aDefaultPositioner The reference to default positioner.
        * @param aModuleStatusManager The reference to module status manager.
        * @return The new instance of this class.
        */
        static CPosExternalGpsMonitor* GetInstanceL(
            CPositioner& aDefaultPositioner,
            MPosModuleStatusManager& aModuleStatusManager );
        
        /**
        * Release the instance of CPosExternalGpsMonitor
        *
        * @param aDefaultPositioner A refence to the default prositioner.
        */
        void ReleaseInstance(
            CPositioner& aDefaultPositioner );
        
        /**
        * Notify external GPS monitor that a PSY is used 
        * 
        * @param aPsyId The module ID of the PSY that is used.
        */
        void PsyUsed( TPositionModuleId aPsyId );
        
        /**
        * Notify external GPS monitor that a PSY is not 
        * used any more.
        *
        * @param aPsyId The module ID of the PSY that is not used anymore.
        */
        void PsyNotUsed( TPositionModuleId aPsyId );
        
        /**
        * Destructor.
        */
        ~CPosExternalGpsMonitor();
        
    protected:
        /**
        * From MPosRequestorListener
        */
        virtual void LocationRequestCompleted( 
            TPositionModuleId aModuleId,
            TInt aErr, 
            const TPositionInfoBase& aPosInfo,
            TBool aIsPosInfoUpToDate );
            
        /**
        * From MPosPsyListListener
        */
        virtual void PsyListChanged( 
            const TPosPsyListChangeEvent& aEvent );

    private:

        /**
        * C++ default constructor.
        */
        CPosExternalGpsMonitor(
            MPosModuleStatusManager& aModuleStatusManager );

        /**
        * EPOC constructor.
        */
        void ConstructL();
        
        /**
        * Get external GPS PSY module Ids
        */
        void ConstructExtGpsPsyArrayL();
        
        /**
        * External GPS PSY checking timeout callback
        */
        static TInt ExtPsyCheckingTimeoutCallback( TAny* aAny );
        
        /**
        * External GPS PSY checking timeout
        */
        void ExtPsyCheckingTimeout();
        
        /**
        * Get index of the PSY in iExtGpsPsyArray
        */
        TInt GetIndexInPsyArray( TPositionModuleId aModuleId ) const;
        
        /**
        * Start timer if it is not started yet.
        */
        void StartTimer();
        
        /**
        * Cancel location requesto to all modules
        */
        void CancelAllLocationRequests();
        
        /**
        * Check if Ext GPS PSY in use
        */
        TBool IsExtGpsPsyInUse() const;
        
        /**
        * Make location request to all external GPS PSY.
        */
        void MakeLocationRequests();
        
        /**
        * Stop location request to the sepcific module
        */
        void CancelLocationRequest( TPositionModuleId aPsyId );
        
        // By default, prohibit copy constructor
        CPosExternalGpsMonitor(const CPosExternalGpsMonitor&);
        // Prohibit assigment operator
        CPosExternalGpsMonitor& operator= (const CPosExternalGpsMonitor&);

    private: // Data
        //Reference to module status manager
        MPosModuleStatusManager& iModuleStatusManager;
        
        //Default positioner
        RPointerArray< CPositioner > iDefaultPositionerArray;
        
        //Reference count
        TInt iRefCount;
        
        //Psy list handler
        CPosPsyListHandler* iPsyListHandler;
        
        //Fix state manager
        CPosPsyFixStateManager* iFixStateManager;
        
        //Const manager
        CPosConstManager* iConstManager;
        
        //External GPS module id array
        RPointerArray< CPosRequestor > iExtGpsPsyArray;
        
        //External GPS module used count
        RArray< TInt > iExtGpsPsyUsedArray;
        
        //TPositionInfo for location request
        TPositionInfo iPosInfo;
        
        //Indicate if location request has been made
        TBool iIsLoacationRequestMade;
        
        //Indicate if external GPS PSY has been used during 
        //the timer period
        TBool iIsExtGpsPsyUsed;
        
        //External GPS checking timer
        CPeriodic* iTimer;
    };

#endif      // CPOSEXTERNALGPSMONITOR_H  
            
// End of File
