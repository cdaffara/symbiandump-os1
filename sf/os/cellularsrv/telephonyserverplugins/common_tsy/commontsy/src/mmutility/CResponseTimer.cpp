// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CResponseTimerTraces.h"
#endif

#include "CResponseTimer.h"
#include "cmmphonetsy.h"
#include "CResponseTimerStore.h"
#include "MmTsy_timeoutdefs.h"

#ifdef REQHANDLE_TIMER

// ======== MEMBER FUNCTIONS ========

CResponseTimer* CResponseTimer::NewL( 
    CMmPhoneTsy* aPhone ) //a Pointer to a phone        
    {
    CResponseTimer* runner = new (ELeave) CResponseTimer;
    CleanupStack::PushL( runner );
    //save pointer to phone
    runner->iPhone = aPhone;
    //initialize the beat Counter
    runner->iBeatCounter = 0;
    //2nd phase construction
    runner->ConstructL();
    CleanupStack::Pop();

    return runner;
    }

CResponseTimer::CResponseTimer()
    {
    }

void CResponseTimer::ConstructL()
    {
     // neutral priority, 0
    iTimer = CHeartbeat::NewL( CActive::EPriorityStandard ); 
    }

CResponseTimer::~CResponseTimer()
    {
    if ( iTimer )
        {
        //stop calling Beat...
        iTimer->Cancel();
        }

    delete iTimer;
    }

// ---------------------------------------------------------------------------
// CResponseTimer::Start
// Starts timer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimer::Start()
    {
    //Check if active 
    if ( !iTimer->IsActive() ) 
        {
        //Timer tick is on the second 
        iTimer->Start( ETwelveOClock, this );
        } 
    }

// ---------------------------------------------------------------------------
// CResponseTimer::Stop
// Stops timer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimer::Stop()
    {
    //Check if active 
    if ( iTimer->IsActive () ) 
        { 
        //Cancel beat method calling 
        iTimer->Cancel(); 
        } 
    }

// ---------------------------------------------------------------------------
// CResponseTimer::Beat
// Beat is called once every second.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimer::Beat()
    {
    //Update the timer time
    iBeatCounter++;

    //check if the time limit is reached for any entry
    for ( TInt index = 0; 
          index < iPhone->GetTimeStampStore()->NumberOfEntries();
          index++ )
        {
        CResponseTimerStore::CTableEntry *aEntry = 
            iPhone->GetTimeStampStore()->GetEntry( index );
        //if the entry is expired, complete and delete it
        if ( aEntry->GetTimeStamp() <= iBeatCounter )
            {   
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CRESPONSETIMER_BEAT_1, "TSY: Request completed due timer expiration, IPC: %d", aEntry->GetIPC() );
            
            //call completion of the request due expired timer 
            iPhone->TimerExpired( aEntry->GetUserObject(),
                                  aEntry->GetHandleType(),
                                  aEntry->GetIPC() );
            }
        else //aEntry.timeStamp > iBeatCounter
            {
            //break cause table is ordered.
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CResponseTimer::Synchronize
// Pure virtual class in base class. Called when synchronization
// is lost. The iBeatCounter is increased.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimer::Synchronize()
    {
    if(iBeatCounter > 0)
        {
        iBeatCounter++;
        }
    }

// ---------------------------------------------------------------------------
// CResponseTimer::GetBeatCounter
// Returns the the value of the iBeatCounter attribute.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint32 CResponseTimer::GetBeatCounter()
    {
    return iBeatCounter;
    }

// ---------------------------------------------------------------------------
// CResponseTimer::ResetBeatCounter
// Resets the value of iBeatCounter to 0.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CResponseTimer::ResetBeatCounter()
    {
    //set the beat counter to 0
    iBeatCounter = 0;
    }

#endif //REQHANDLE_TIMER

//  End of File




