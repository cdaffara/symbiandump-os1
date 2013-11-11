/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer class for supervising vibrating time.
*
*/


#ifndef VIBRATIMER_H
#define VIBRATIMER_H

//  INCLUDES
#include <e32base.h>

/**
*  An interface for handling timer expiration events.
*/
class MVibraTimerCallback
    {
    public:
        /** 
        * Called when the timer fires
        */
        virtual void TimerFired() = 0;
    };

/**
*  Timer class for supervising vibrating time.
*/
NONSHARABLE_CLASS(CVibraTimer) : public CTimer
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVibraTimer* NewL(MVibraTimerCallback* aCallback, TInt aMaxVibTime);
                
        /**
        * Destructor.
        */
        ~CVibraTimer();
        
        /**
        * Set the timer
        * @param aIntervalInMilliSecs timer will expire after this duration.
        */
        TInt Set(TInt aIntervalInMilliSecs);
        
        // From CTimer
        void RunL();
        
    protected:
        CVibraTimer(MVibraTimerCallback* aCallback, TInt aMaxVibTime);
        
    private: //Data
        MVibraTimerCallback* iCallback;
        TInt                 iMaximumVibraTimeMs;//Maximum vibrating time from SharedData or CenRep
    };

#endif// VIBRATIMER_H   
            
// End of File
