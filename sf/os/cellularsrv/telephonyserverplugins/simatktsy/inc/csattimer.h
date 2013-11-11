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
* Name        : CSatTimer.cpp
* Part of     : Common SIM ATK TSY / commonsimatktsy
* Implementation of the sat timer functions.
* Version     : 1.0    
*
*/




#ifndef CSATTIMER_H
#define CSATTIMER_H


//  INCLUDES
#include <e32base.h>    // Symbian base types


// CONSTANTS
const TUint8 KMaxNumberOfParallelTimers = 8;
const TInt KTimeStampsEqual             = 0;
const TInt KFirstTimeStampBigger        = 1;
const TInt KFirstTimeStampSmaller       = -1;


// FORWARD DECLARATIONS
class CSatMessHandler;
class CSatNotificationsTsy;


// DESCRIPTION
/**
* Implementation of the sat timer functions.
* @lib Commonsimatktsy
* @since 3.1
*/

NONSHARABLE_CLASS ( CSatTimer ) : public CBase, public MBeating 
    {
    public:
        // DESCRIPTION
        //
        // CSatTimer::TTimer class.
        //
        class TTimer
            {
            public:
                /**
                * Non default Constructor
                * @param aTimerId: Timer id
                * @param aStartTime: Timer start time
                * @param aTimeStamp: Time stamp
                */
                TTimer( TInt aTimerId, TUint32 aStartTime,
                    TUint32 aTimeStamp );

                /**
                * Returns iTimeStamp from a TTimer
                * @params None
                * @returns value of the iTimeStamp attribute
                */
                TUint32 TimeStamp();

                /**
                * Returns iTimerId from a TTimer
                * @params: None
                * @returns value of the iTimerId attribute
                */
                TInt TimerId();

                /**
                * Returns iStartTime from a TTimer
                * @params: None
                * @returns value of the iStartTime attribute
                */
                TUint32 TimerStartTime();

                /**
                * Returns if two entries are equal
                * @param aArg1: first argument
                * @param aArg2: second argument
                * @returns ETrue if two entries are equal
                */
                static TBool CompareEntries( const TTimer& aArg1,
                                         const TTimer& aArg2 );

                /**
                * Returns order between two entries
                * @param aArg1: first argument
                * @param aArg2: second argument
                * @returns 0 if arg1 = arg2
                *          1 if arg1 > arg2
                *         -1 else
                */
                static TInt OrderEntries( const TTimer& aArg1,
                                      const TTimer& aArg2 );

            private:

                // Timer identifier
                TInt iTimerId;

                // Timer start time
                TUint32 iStartTime;

                // Time stamp
                TUint32 iTimeStamp;

            };

        /**
        * Two-phased constructor.
        * @param aNotificationsTsy: Pointer to notifications tsy
        * @return created object 
        */
        static CSatTimer* NewL( CSatNotificationsTsy* aSatNotificationsTsy );
        /**
        * Constructor.
        * @param aNotificationsTsy: Pointer to notifications tsy
        */
        void ConstructL( CSatNotificationsTsy* aSatNotificationsTsy );

        /**
        * Destructor
        */
        ~CSatTimer(); 

        /**
        * Starts the timer counting
        * @param aTimerId: Timer id
        * @param aTimerValue: Timeout value
        * @return KErrNone or error code
        */
        TInt Start( TInt aTimerId, TUint32 aTimerValue );

        /**
        * Stops the timer counting
        * @params: None
        * @return None
        */
        void Stop();

        /**
        * Deletes timer from the table
        * @param aTimerId: timer identifier
        * @returns KErrNone or KErrNotFound 
        */
        TInt DeleteTimerById( TInt aTimerId );

        /**
        * Gets current value of timer by timer id
        * @param aTimerId: timer identifier
        * @returns Current timer value 
        */
        TUint32 CurrentValueOfTimerById( TInt aTimerId );

        /**
        * Sets status for ProactiveCommandOnGoing flag
        * @param aStatus: Status for proactive command
        * @return None
        */
        void SetProactiveCommandOnGoingStatus( TBool aStatus );

    private:
    	/**
        * Constructor
        * @params: None
        * @returns None
        */
    	CSatTimer();


        /**
        * Called when beat works ok, once every second
        * @param None
        * @returns None
        */
        void Beat(); 
    
        /**
        * Called when we need to synchronize
        * @params: None
        * @returns None
        */
        void Synchronize();

        /**
        * Checks if an entry is in iEntryTable
        * @param aTimerId: timer identifier 
        * @return ETrue if the timer has an entry in the table
        *                EFalse otherwise.
        */
        TBool CheckTimerTable( TInt aTimerId );

    private:

        // Sat notifications tsy pointer
        CSatNotificationsTsy* iSatNotificationsTsy;
    
        // Count of beats of the timer
        TUint32 iBeatCounter;

        // Pointer to the CHeartbeat type timer.
        CHeartbeat* iTimer; 

        // Pointer to the entry array
        RArray<TTimer>* iTimerTable;

        // Flag for proactiveCommandOnGoing status
        TBool iIsProactiveCommandOnGoing;

    };

#endif // CSATTIMER_H

// End of File
