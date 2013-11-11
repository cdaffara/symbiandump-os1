/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMGENERICTIMER_H
#define HWRMGENERICTIMER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KHWRMGenericTimerMaxTime(0); 

/**
*  An interface for handling timer expiration events.
*/
class MHWRMGenericTimerCallback
    {
    public:
        /** 
        * Called when the timer fires
        *
        * @param aTimerId Timer identifier to id which timer fired.
        * @param aCutOff ETrue if timer firing occurred because maximum ontime
        *                was reached.
        */
        virtual void GenericTimerFired(TInt aTimerId, TBool aCutOff) = 0;
    };

/**
*  Timer class for supervising Genericting time.
*/
class CHWRMGenericTimer : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        *
        * @param aCallback Callback object for informing timer firing
        * @param aMaxTime Maximum time timer is allowed to run. If KHWRMGenericTimerMaxTime,
        *        then timer has no maximum time.
        * @param aTimerId Timer identifier used in timer fired callback.
        */
        static CHWRMGenericTimer* NewL(MHWRMGenericTimerCallback& aCallback,
                                     const TTimeIntervalMicroSeconds32& aMaxTime,
                                     TInt aTimerId);
                
        /**
        * Destructor.
        */
        ~CHWRMGenericTimer();
        
        /**
        * Set the timer.
        *
        * @param aInterval timer will expire after this duration.
        *        If aInterval is zero, then timer is not started unless
        *        maximum time has been specified, in which case the 
        *        maximum time is used as duration.
        */
        void Set(const TTimeIntervalMicroSeconds32& aInterval);
        
        /**
        * Cancels the timer and returns the remaining time.
        * Timer can be resumed with Set call.
        *
        * @return Remaining time on timer. Will be Zero if timer is not active,
        *         otherwise it is always positive.
        */
        TTimeIntervalMicroSeconds32 Freeze();
        
        /**
        * Set the maximum time, till the timer runs 
        */

        void SetMaximumTime(TTimeIntervalMicroSeconds32& aMaximumTime);

        /*
        * Returns the Timer Id
        */
        inline TInt GetTimerId()
        {
        return iTimerId;
        }
        
        
    public:  // From base classes

        // From CTimer
        void RunL();
        
    protected:
    
        /**
        * Constructor
        *
        * @param aCallback Callback object for informing timer firing
        * @param aMaxTime Maximum time timer is allowed to run. If KHWRMGenericTimerMaxTime,
        *        then timer has no maximum time.
        * @param aTimerId Timer identifier used in timer fired callback.
        */
        CHWRMGenericTimer(MHWRMGenericTimerCallback& aCallback, 
                        const TTimeIntervalMicroSeconds32& aMaxTime,
                        TInt aTimerId);
        
        /**
        * Second phase constructor
        */
        
    private: // Data
        MHWRMGenericTimerCallback&    iCallback;    // Callback for timer fired messages. Not owned.
        TTimeIntervalMicroSeconds32   iMaximumTime; // Maximum Timer time
        TBool                         iCutOff;      // Indicates that timer was cut off by max duration
        TInt                          iTimerId;     // In case multiple timers are needed.
        
        TTime                         iActivationTime; // Stores the time the timer is set to activate.
    };

#endif // HWRMGENERICTIMER_H   
            
// End of File
