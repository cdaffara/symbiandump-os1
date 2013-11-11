/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Generic sensor server timer
*
*/


#ifndef SENSRVTIMER_H
#define SENSRVTIMER_H

#include <e32base.h>

const TInt KSensrvTimerMaxTime(0); 

/**
* A callback interface for handling timer expiration events.
*
* @since S60 5.0
*/
class MSensrvTimerCallback
    {
    public:

        /** 
        * Called when the timer fires
        *
        * @since S60 5.0
        * @param aTimerId Timer identifier to id which timer fired.
        */
        virtual void TimerFired(TInt aTimerId) = 0;
    };

/**
*  Timer class for sensor server timing needs.
*
* @since S60 5.0
*/
class CSensrvTimer : public CTimer
    {
    public:  
    
        /**
        * Predefined timer identifiers
        *
        * @since S60 5.0
        */
        enum TTimerId
            {
            ETimerIdSsyCleanupTimer       = 1,
            ETimerIdBufferPeriodTimer     = 2,
            ETimerIdTransactionTimer      = 3,
            ETimerIdSsyInactivityTimer    = 4
            };

        /**
        * Two-phased constructor.
        *
        * @since S60 5.0
        * @param aCallback Callback object for informing timer firing
        * @param aMaxTime Maximum time timer is allowed to run. If KSensrvTimerMaxTime,
        *        then timer has no maximum time.
        * @param aTimerId Timer identifier used in timer fired callback.
        */
        static CSensrvTimer* NewL(MSensrvTimerCallback& aCallback,
                                  const TTimeIntervalMicroSeconds32& aMaxTime,
                                  TInt aTimerId);
                
        /**
        * Destructor.
        */
        ~CSensrvTimer();
        
        /**
        * Set the timer.
        *
        * @since S60 5.0
        * @param aInterval timer will expire after this duration.
        *        If aInterval is zero, then timer is not started unless
        *        maximum time has been specified, in which case the 
        *        maximum time is used as duration.
        */
        void Set(const TTimeIntervalMicroSeconds32& aInterval);
        
        
    public:  

        // From CTimer
        void RunL();
        
    protected:
    
        /**
        * C++ constructor
        *
        * @since S60 5.0
        * @param aCallback Callback object for informing timer firing
        * @param aMaxTime Maximum time timer is allowed to run. If KSensrvTimerMaxTime,
        *        then timer has no maximum time.
        * @param aTimerId Timer identifier used in timer fired callback.
        */
        CSensrvTimer(MSensrvTimerCallback& aCallback, 
                     const TTimeIntervalMicroSeconds32& aMaxTime,
                     TInt aTimerId);
        
    private: 
    
        /**
        * Callback for timer fired messages. 
        * Not own.
        */
        MSensrvTimerCallback& iCallback;    
        
        /**
        * Maximum timer time.
        */
        TTimeIntervalMicroSeconds32 iMaximumTime; 
        
        /**
        * In case multiple timers are needed
        */        
        TInt iTimerId;
    };



#endif // SENSRVTIMER_H
