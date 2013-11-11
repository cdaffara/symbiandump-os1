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



#ifndef CPOSCONSTMANAGER_H
#define CPOSCONSTMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <lbs.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
* This class manages contant values that are configurable during
* ROM creation time.
*/
class CPosConstManager : public CBase
    {
    public: // Constructors and destructor
              
        /**
        * Get an instance of CPosConstManager.    
        *
        * @return The new instance of this class.
        */
        static CPosConstManager* GetInstanceL();

        /**
        * Release the instance of CPosConstManager
        */
        void ReleaseInstance();
        
        /**
        * Destructor.
        */
        ~CPosConstManager();
        
        /**
        * Get timeshift value. In microseconds.
        *
        * @param aTtff Time to first fix value.
        * @param aTtnf Time to next fix value.
        * @param aDeviceStatus The device status.
        * @return Timeshift timeout value.
        */
        TTimeIntervalMicroSeconds  GetTimeshiftValue( 
            TTimeIntervalMicroSeconds aTtff, 
            TTimeIntervalMicroSeconds aTtnf, 
            TPositionModuleStatus::TDeviceStatus aDeviceStatus );
            
        /**
        * Get PSY state unkwown timeout value. in microseconds.
        *
        * @return Unused timeout value.
        */
        TTimeIntervalMicroSeconds  GetUnusedTimeoutValue() const;
        
        /**
        * Get external GPS fix state checking timeout value.
        *
        * @return External GPS PSY checking timeout value.
        */
        TTimeIntervalMicroSeconds  GetExternalGpsCheckingTimeoutValue() const;
        
        /**
        * Get cleanup timeout value.
        *
        * @return Cleanup timeout value.
        */
        TTimeIntervalMicroSeconds GetCleanupTimeoutValue() const;
        
        /**
        * Get last time working GPS PSY from CenRep.
        * 
        * @return Module ID of last time working GPS PSY.
        */
        TPositionModuleId GetLastWorkingGpsPsyId();
        
        /**
        * Set last time working GPS PSY to CenRep
        * Error is ignored.
        *
        * @param @aId Module ID of last time working GPS PSY.
        */
        void SetLastWorkingGpsPsyId( TPositionModuleId aId );
        
    private:

        /**
        * C++ default constructor.
        */
        CPosConstManager();

        /**
        * EPOC constructor.
        */
        void ConstructL();

        // By default, prohibit copy constructor
        CPosConstManager(const CPosConstManager&);
        // Prohibit assigment operator
        CPosConstManager& operator= (const CPosConstManager&);

    private: // Data
        //Central Repository
        CRepository* iRepository;
        
        //Refernce count
        TInt iRefCount;
        
        // Maximum Timeshift value whne PSY is in active or ready state
        TInt iMaxTimeshiftInActiveOrReady;
        
        // Maximum timeshift value when PSY is not in active or ready state
        TInt iMaxTimeshiftNotInActiveOrReady;
        
        // PSY state unknown timeout value
        TInt iPsyStateUnknownTimeout;
        
        // External GPS checking timeout value
        TInt iExternalGpsCheckingTimeout;
        
        // Cleanup timeout
        TInt iCleanupTimeout;
        
        // Last working GPS PSY module ID
        TPositionModuleId iLastWorkingGpsPsyId;

    };

#endif      // CPOSCONSTMANAGER_H  
            
// End of File
