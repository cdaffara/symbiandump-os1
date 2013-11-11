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



#ifndef CRESPONSETIMER_H
#define CRESPONSETIMER_H

//  INCLUDES
#include <e32base.h>
#include "MmTsy_timeoutdefs.h"

// this flag comes from CResponseTimerStore.h
#ifdef REQHANDLE_TIMER

// FORWARD DECLARATIONS
class CMmPhoneTsy;

// CLASS DECLARATION

/**
*  CResponseTimer class.
*/
NONSHARABLE_CLASS( CResponseTimer ) : public CBase, public MBeating 
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param *aPhone pointer to phone object
        * return CResponseTimer* created timer object 
        */
        static CResponseTimer* NewL( CMmPhoneTsy *aPhone);
    
        /**
        * Destructor
        */
        virtual ~CResponseTimer(); 
        
        // New functions
    
        /**
        * Starts the timer counting
        *         
        * 
        */
        void Start();
    
        /**
        * Stops the timer counting
        *         
        * 
        */
        void Stop();
    
        /**
        * Returns the value of iBeatCounter
        *         
        * 
        * @return TUint32 value of iBeatCounter
        */
        TUint32 GetBeatCounter();
    
        /**
        * Sets iBeatCounter to 0
        *         
        * 
        */
        void ResetBeatCounter();

    private:
    
        /**
        * Default constructor
        */
        CResponseTimer();
        
        /**
        * 2nd phase of the construction
        */
        void ConstructL();
    
        /**
        * Checks and completes expired requests every second
        *         
        * 
        */
        void Beat(); 
    
        /**
        * Called when we need to synchronize
        *         
        * 
        */
        void Synchronize();

    private: // Data
        
        /**
         * Count of beats of the timer
         */         
        TUint32 iBeatCounter;
    
        /**
         * Pointer to the CHeartbeat type timer.
         * Own.         
         */         
        CHeartbeat* iTimer; 
        
        /**
         * Pointer to phone
         * Not own.         
         */         
        CMmPhoneTsy *iPhone;
    
    };
#endif //REQHANDLE_TIMER

#endif //CRESPONSETIMER_H

// End of File
