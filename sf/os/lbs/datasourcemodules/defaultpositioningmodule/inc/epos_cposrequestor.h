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



#ifndef CPOSREQUESTOR_H
#define CPOSREQUESTOR_H

#include <e32base.h>
#include <lbs.h>

class MPosRequestorListener;
class CPositioner;
class CPosPsyFixStateManager;
class MPosModuleStatusManager;

/**
* The CPosRequestor is responsible for making location request to loaded
* PSYs. 
*/
class CPosRequestor : public CActive
    {
    public: // Constructors and destructor
              
        /**
        * Get an instance of CPosRequestor.    
        *
        * @return The new instance of this class.
        */
        static CPosRequestor* NewL(
            CPositioner& aDefaultPositioner,
            TPositionModuleId aModuleId,
            MPosRequestorListener& aListener,
            CPosPsyFixStateManager& aFixStateManager,
            MPosModuleStatusManager& aModuleStatusManager );
        
        /**
        * Destructor.
        */
        ~CPosRequestor();
        
        /**
        * Get PSY ID.
        *
        * @return PSY ID
        */
        TPositionModuleId ModuleId() const;
        
        /**
        * Make location request.
        *
        * If location request is already on going, this function call
        * will first cancel the location request. 
        * Call Cancel() to cancel the location request
        *
        * @param aPosInfo Refernce to TPositionInfoBase class to
        *     store result of this location request.
        */
        void MakeLocationRequestL( const TPositionInfoBase& aPosInfo );
        
        /**
         * Cancels passing reason to PSY  
         * 
         */
        void CancelWithReason(TInt aCancelReason);
        
        /**
        * Start tracking
        *
        * @param aInterval The interval value.
        */
        void StartTrackingL( TTimeIntervalMicroSeconds& aInterval );
        
        /**
        * Stop tracking
        */
        void StopTracking();

    protected: // Functions from base classes

		/**
		* From CActive. This function shall not leave.
		*/
		void RunL();

		/**
		* From CActive
		*/
		void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CPosRequestor( 
            MPosRequestorListener& aListener,
            CPosPsyFixStateManager& aFixStateManager,
            MPosModuleStatusManager& aModuleStatusManager );

        /**
        * EPOC constructor.
        */
        void ConstructL( 
            CPositioner& aDefaultPositioner,
            TPositionModuleId aModuleId );
            
        // By default, prohibit copy constructor
        CPosRequestor(const CPosRequestor&);
        // Prohibit assigment operator
        CPosRequestor& operator= (const CPosRequestor&);

    private: // Data
        //Requestor listner
        MPosRequestorListener& iListener;
        
        //Fix state manager
        CPosPsyFixStateManager& iFixStateManager;
        
        //Reference to module status manager
        MPosModuleStatusManager& iModuleStatusManager;
        
        //Positioner
        CPositioner* iPositioner;
        
        //Boolean which indicate if tracking is started in the 
        //positioner.
        TBool iTrackingStarted;
        
        //Position info base class for location request
        TPositionInfoBase* iPosInfo;
        
        //Boolean indicate if the iPosInfo is uptodate
        TBool iIsPosInfoUpTodate;
        
        TInt iCancelReason;

    };

#endif      // CPOSREQUESTOR_H  
            
// End of File
