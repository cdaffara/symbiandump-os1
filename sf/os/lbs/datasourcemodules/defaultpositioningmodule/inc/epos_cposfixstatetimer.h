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



#ifndef CPOSFIXSTATETIMER_H
#define CPOSFIXSTATETIMER_H

#include <e32base.h>
#include <lbs.h>

class MPosFixStateTimerListener;

/**
* The CPosFixStateTimer is responsible of handle timeshift timer and 
* PSY state unknown timer.
*/
class CPosFixStateTimer : public CBase
    {
    public: // Constructors and destructor
              
        /**
        * Get an instance of CPosFixStateTimer.    
        *
        * @return The new instance of this class.
        */
        static CPosFixStateTimer* NewL(
            TPositionModuleId aModuleId,
            MPosFixStateTimerListener& aListener );
        
        /**
        * Destructor.
        */
        ~CPosFixStateTimer();
        
        /**
        * Get PSY ID.
        *
        * @return PSY ID
        */
        TPositionModuleId ModuleId() const;
        
        /**
        * Start timeshift timer. If the timer is already 
        * running, this function does nothing.
        */
        void StartTimeshiftTimer(
            TTimeIntervalMicroSeconds aInterval );
            
        /**
        * Cancel timeshift timer
        */
        void CancelTimeshiftTimer();
        
        /**
        * Start unknown timer
        */
        void StartUnknownTimer(
            TTimeIntervalMicroSeconds aInterval );
        
        /**
        * Cancel unknown timer
        */
        void CancelUnknownTimer();
        
    private:

        /**
        * C++ default constructor.
        */
        CPosFixStateTimer( 
            TPositionModuleId aModuleId,
            MPosFixStateTimerListener& aListener );

        /**
        * EPOC constructor.
        */
        void ConstructL();
        
        //Timeshift timer timeout callback
        static TInt TimeshiftTimeoutCallback( TAny* aAny );
        
        //Timeshift timeout 
        void TimeshiftTimeout();
        
        //Unknown timer timeout callback
        static TInt UnknownTimeoutCallback( TAny* aAny );
        
        //Uknown timeout
        void UnknownTimeout();

        // By default, prohibit copy constructor
        CPosFixStateTimer(const CPosFixStateTimer&);
        // Prohibit assigment operator
        CPosFixStateTimer& operator= (const CPosFixStateTimer&);

    private: // Data
        //Module Id
        TPositionModuleId iModuleId;
        
        //Requestor listner
        MPosFixStateTimerListener& iListener;
        
        //Timeshift timer
        CPeriodic* iTimeshiftTimer;
        
        //Unknown timer
        CPeriodic* iUnknownTimer;
        
    };

#endif      // CPOSFIXSTATETIMER_H  
            
// End of File
