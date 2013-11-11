// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : CSatTimer.cpp
// Part of     : Common Sim Atk TSY / commonsimatktsy
// Implementation of the sat timer functions. 
// Version     : 1.0
//



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csattimerTraces.h"
#endif

#include "CSatTimer.h"              // Class header
#include "CSatNotificationsTsy.h"   // Sat Tsy class
#include "CSatDataPackage.h"	        // For data packages


// -----------------------------------------------------------------------------
// CSatNotificationsTsy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatTimer* CSatTimer::NewL(CSatNotificationsTsy* aSatNotificationsTsy)
    {    
    CSatTimer* const self = new ( ELeave ) CSatTimer();
    CleanupStack::PushL( self );
    self->ConstructL(aSatNotificationsTsy);
    CleanupStack::Pop();    	
    return self;
    }

// -----------------------------------------------------------------------------
// CSatTimer::ConstructL
// Creates CSatTimer object.
// -----------------------------------------------------------------------------
//
void CSatTimer::ConstructL
        ( 
        CSatNotificationsTsy* aSatNotificationsTsy
        )
    {

    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_CONSTRUCTL_1,  "CSAT: CSatTimer::ConstructL" );
    iSatNotificationsTsy = aSatNotificationsTsy;
    iTimerTable = new ( ELeave ) RArray<TTimer>( KMaxNumberOfParallelTimers );  
    // Neutral priority, 0
    iTimer = CHeartbeat::NewL( CActive::EPriorityStandard );

    }

// -----------------------------------------------------------------------------
// CSatTimer::CSatTimer
// C++ constructor
// -----------------------------------------------------------------------------
//
CSatTimer::CSatTimer()
    {    
    // None
    }


// -----------------------------------------------------------------------------
// CSatTimer::~CSatTimer
// Destructor
// -----------------------------------------------------------------------------
//
CSatTimer::~CSatTimer
        (
        //None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_DTOR_1,  "CSAT: CSatTimer::~CSatTimer" );
    if ( iTimer )
        {
        // Stop calling Beat...
        iTimer->Cancel();
        delete iTimer;
        }

    if ( iTimerTable )
	    {
	    iTimerTable->Close();
	    delete iTimerTable;
	    }
    }

// -----------------------------------------------------------------------------
// CSatTimer::Start
// Starts timer.
// -----------------------------------------------------------------------------
//
TInt CSatTimer::Start
        (
        TInt aTimerId,      // Timer Id
        TUint32 aTimerValue // Timer value
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_START_1,  "CSAT: CSatTimer::Start" );
    TInt ret( KErrNone );

     // Check if the entry is already in the table 
    if ( CheckTimerTable( aTimerId ) )
        {
        // Delete the old entry
        DeleteTimerById( aTimerId ); 
        }

    // Calculate the trigger time (actual beat amount + timeout)
    TUint32 timeStamp = iBeatCounter + aTimerValue;
    // Create timer
    TTimer timer( aTimerId, iBeatCounter, timeStamp );
    
    // Insert the entry in the table
    ret = iTimerTable->InsertInOrderAllowRepeats( timer, 
        TTimer::OrderEntries );
        
    // Check if active 
    if ( ( KErrNone == ret ) && ( !iTimer->IsActive() ) ) 
        {
        // Timer tick is on the second 
        iTimer->Start( ETwelveOClock, this );
        } 

    return ret;

    }

// -----------------------------------------------------------------------------
// CSatTimer::Stop
// Stops timer
// -----------------------------------------------------------------------------
//
void CSatTimer::Stop
        (
        //None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_STOP_1,  "CSAT: CSatTimer::Stop" );
    // Check if active 
    if ( iTimer->IsActive () ) 
        { 
        // Cancel beat method calling 
        iTimer->Cancel(); 
        } 
    }

// -----------------------------------------------------------------------------
// CSatTimer::Beat
// Beat is called once every second.
// -----------------------------------------------------------------------------
//
void CSatTimer::Beat
        (
        //None
        )
    {
	TInt ret( KErrNone );
    iBeatCounter++;
    TBool timeStampGreater( ETrue ); 
    for( TInt numberOfTimers = iTimerTable->Count(); 
         timeStampGreater && 0 < numberOfTimers; numberOfTimers-- )
        {
        // Get the pointer to the TimerTable  
        TTimer& timerTable = ( *iTimerTable )[ numberOfTimers - 1 ];
        
        if ( timerTable.TimeStamp() <= iBeatCounter )
            {
            // Proactive command ongoing
            if( !( iIsProactiveCommandOnGoing ) )
                {
                // Call completion of the request due expired timer 
                TRAPD( trapError, ret = iSatNotificationsTsy->TimerExpirationL( 
					timerTable.TimerId(), 
					iBeatCounter - timerTable.TimerStartTime() ) 
					);
					
                if ( trapError )
					{
					ret = trapError;
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_BEAT_1, "CSAT: CSatTimer::Beat, Trap error: %d", trapError);
					}
					
                // Remove timer from the table
                if ( KErrNone == ret )
                    {
                    DeleteTimerById( timerTable.TimerId() );
                    }
                }
            }
        else // TimerTable->iTimeStamp > iBeatCounter
            {
            // Table is ordered -> Break
            timeStampGreater = EFalse; 
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatTimer::Synchronize
// Called when synchronization is lost. The iBeatCounter is increased.
// -----------------------------------------------------------------------------
//
void CSatTimer::Synchronize
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_SYNCHRONIZE_1,  "CSAT: CSatTimer::Synchronize" );
    iBeatCounter++;
    }

// -----------------------------------------------------------------------------
// CSatTimer::CheckTimerTable
// Check if an entry is in iEntryTable
// -----------------------------------------------------------------------------
//
TBool CSatTimer::CheckTimerTable
        (
        TInt aTimerId
        )
    {
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_CHECKTIMERTABLE_1,  "CSAT: CSatTimer::CheckTimerTable" );
    TBool ret( EFalse );

    // Create the entry with meaningful values only
    TTimer timer( aTimerId, 0, 0 );

    // Find the entry for this Handle in the list 
    TInt position = iTimerTable->Find ( timer, TTimer::CompareEntries );

    if ( KErrNotFound != position )
        {
        ret = ETrue;
        }

    return ret;

    }

// -----------------------------------------------------------------------------
// CSatTimer::DeleteTimerById
// Deletes an entry from the entry table using the handle type 
// and the handle store as identificators.
// -----------------------------------------------------------------------------
//
TInt CSatTimer::DeleteTimerById
        (
        TInt aTimerId // Timer identifier
        )
    { 
    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_DELETETIMERBYID_1,  "CSAT: CSatTimer::DeleteTimerById" );
    TInt ret( KErrNotFound );

    // Create the entry with meaningful values only
    TTimer timer( aTimerId, 0, 0 );

    // Find the entry for this Handle in the list 
    TInt position = iTimerTable->Find ( timer, 
        TTimer::CompareEntries );  
    // Check if element is found
    if ( KErrNotFound != position ) 
        {
        iTimerTable->Remove( position );
        iTimerTable->Compress();
        // Check if empty
        if ( 0 == iTimerTable->Count() )
            {
            // Restart the beatCounter
            iBeatCounter = 0;
            // Stop timer till new entry is done
            Stop();
            }
        ret = KErrNone;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::CurrentValueOfTimerById
// Finds an entry from the entry table using the timer identification
// -----------------------------------------------------------------------------
//
TUint32 CSatTimer::CurrentValueOfTimerById
        (
        TInt aTimerId // Timer identifier
        )
    { 
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_CURRENTVALUEOFTIMERBYID_1,  "CSAT: CSatTimer::CurrentValueOfTimerById" );
    TInt ret( KErrNotFound );

    // Create the entry with meaningful values only
    TTimer timer( aTimerId, 0, 0 );

    // Find the entry for this Handle in the list 
    TInt position = iTimerTable->Find( timer, 
        TTimer::CompareEntries );  
        
    // Check if element is found
    if ( KErrNotFound != position ) 
        {
        // Get the pointer to the TimerTable  
        TTimer& timerTable = ( *iTimerTable )[ position ];

        // Get timeStamp
        TUint32 timerStamp = timerTable.TimeStamp();

        // Current timer value = timer timeout value - current time
        ret = timerStamp - iBeatCounter;

        // If current timer value is smaller than zero set value to 0
        // this can happen if there is some kind of delay before 
        // timer is expired and iBeatCounter is still counting...   
        if( 0 > ret )
            {
            ret = 0;
            }            
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::SetProactiveCommandOnGoingStatus
// Sets ProactiveCommandOngoing status
// -----------------------------------------------------------------------------
//
void CSatTimer::SetProactiveCommandOnGoingStatus
        ( 
        TBool aStatus 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_SETPROACTIVECOMMANDONGOINGSTATUS_1,  "CSAT: CSatTimer::SetProactiveCommandOnGoingStatus" );
    iIsProactiveCommandOnGoing = aStatus;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TTimer
// Default constructor Sets timer information to the TimerTable
// -----------------------------------------------------------------------------
//
CSatTimer::TTimer::TTimer
        (
        TInt aTimerId,       // Timer id
        TUint32 aStartTime,  // Timer start time
        TUint32 aTimeStamp   // Time stamp
        ): iTimerId( aTimerId ), iStartTime( aStartTime ), 
           iTimeStamp( aTimeStamp )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_TTIMER_1,  "CSAT: TTimer::TTimer" );
    }

// -----------------------------------------------------------------------------
// CSatTimer::TimeStamp
// Gets the value of iTimeStamp
// -----------------------------------------------------------------------------
//
TUint32 CSatTimer::TTimer::TimeStamp
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_TIMESTAMP_1,  "CSAT: TTimer::TimeStamp" );
    return iTimeStamp;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TimerId
// Gets the value of iTimerId
// -----------------------------------------------------------------------------
//
TInt CSatTimer::TTimer::TimerId
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_TIMERID_1,  "CSAT: TTimer::TimerId" );
    return iTimerId;
    }

// -----------------------------------------------------------------------------
// CSatTimer::TimerStartTime
// Gets the value of iStartTime
// -----------------------------------------------------------------------------
//
TUint32 CSatTimer::TTimer::TimerStartTime
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_TIMERSTARTTIME_1,  "CSAT: TTimer::TimerStartTime" );
    return iStartTime;
    }

// -----------------------------------------------------------------------------
// CSatTimer::CompareEntries
// Compares two entries are return if the are same of not.
// -----------------------------------------------------------------------------
//
TBool CSatTimer::TTimer::CompareEntries
        (
        const CSatTimer::TTimer& aArg1, 
        const CSatTimer::TTimer& aArg2 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_COMPAREENTRIES_1,  "CSAT: CSatTimer::TTimer::CompareEntries" );
    TBool ret( EFalse ); 

    // We are interested only in the timer id
    if ( aArg1.iTimerId == aArg2.iTimerId )
        {
        ret =  ETrue;
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_COMPAREENTRIES_2,  "CSAT: CSatTimer::TTimer::CompareEntries, Not equal" );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatTimer::OrderEntries
// Compares two entries and returns which is the order between them.
// -----------------------------------------------------------------------------
//
TInt CSatTimer::TTimer::OrderEntries 
        (
        const CSatTimer::TTimer& aArg1, 
        const CSatTimer::TTimer& aArg2  
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_ORDERENTRIES_1,  "CSAT: CSatTimer::TTimer::OrderEntries" );
    TInt ret( KFirstTimeStampSmaller );

    // We are interested only in the time stamp
    if ( aArg1.iTimeStamp == aArg2.iTimeStamp )
        {
        ret = KTimeStampsEqual;
        }
    else if ( aArg1.iTimeStamp > aArg2.iTimeStamp )
        {
        ret = KFirstTimeStampBigger;
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATTIMER_TTIMER_ORDERENTRIES_2,  "CSAT: CSatTimer::TTimer::OrderEntries, KFirstTimeStampSmaller" );
        }

    return ret;
    }

// End of File

