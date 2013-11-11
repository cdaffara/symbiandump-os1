// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CMmPrivateUtility.h"
#include "MmTsy_conf.h"
#include "CMmCommonStaticUtility.h"
#include "cmmphonetsy.h"
#include "cmmcalltsy.h"
#include "Cmmpacketcontexttsy.h"
#include <f32file.h>

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// MmTsyPanic
// Panic method for MM TSY. NOTE! TRAP's can't handle this but Panic goes 
// through them. Use this method only when Panic is absolutely necessary.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void MmTsyPanic( TMmTsyError aError ) 
    {
    _LIT( KPanicCategory, "MM TSY PANIC" );
    User::Panic( KPanicCategory, aError );
    }

// ---------------------------------------------------------------------------
// MmTsyPanic
// Leave method for MM TSY. TRAP's will handle these cases.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void MmTsyLeaveL( TMmTsyError aError )
    {
    User::Leave( aError );
    }
    
// ======== MEMBER FUNCTIONS ========


CNetNetworkAvailableInfo::CNetNetworkAvailableInfo() : CBase()
    {
    }

CNetNetworkAvailableInfo::~CNetNetworkAvailableInfo()
    {
    }

CListReadAllAttempt* CListReadAllAttempt::NewL(
    RMobilePhone::TClientId* aId )
    {
    CListReadAllAttempt* read = new ( ELeave ) CListReadAllAttempt( aId );
    CleanupStack::PushL( read );
    read->ConstructL();
    CleanupStack::Pop();
    return read;
    }

CListReadAllAttempt::CListReadAllAttempt(
    RMobilePhone::TClientId* aId ) : CBase()
    {
    iClient.iSessionHandle = aId->iSessionHandle;
    iClient.iSubSessionHandle = aId->iSubSessionHandle;
    iListBuf = NULL;
    }

void CListReadAllAttempt::ConstructL()
    {
    }

CListReadAllAttempt::~CListReadAllAttempt()
    {
    delete iListBuf;
    }

CAcquireEntry* CAcquireEntry::NewL(
    const TTsyReqHandle aTsyReqHandle )
    {
    return new ( ELeave ) CAcquireEntry( aTsyReqHandle );
    }

CAcquireEntry::CAcquireEntry(
    const TTsyReqHandle aTsyReqHandle )
    {
    iTsyReqHandle = aTsyReqHandle;
    }

CAcquireEntry::~CAcquireEntry()
    { 
    }

// ---------------------------------------------------------------------------
// CAcquireEntry::Deque
// 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CAcquireEntry::Deque()
    {
    iLink.Deque();
    iLink.iPrev = iLink.iNext = NULL;
    }

CAcquireOwnerList* CAcquireOwnerList::NewL()
    {
    CAcquireOwnerList* ac = new ( ELeave ) CAcquireOwnerList();
    CleanupStack::PushL( ac );
    ac->ConstructL();
    CleanupStack::Pop();
    return ac;
    }

CAcquireOwnerList::CAcquireOwnerList()
    {
    }

void CAcquireOwnerList::ConstructL()
    {
    iAcquireList.SetOffset( _FOFF ( CAcquireEntry, iLink ) );
    }

CAcquireOwnerList::~CAcquireOwnerList()
    { 
    }

// ---------------------------------------------------------------------------
// CAcquireOwnerList::FindByTsyReqHandle
// loop through entries in the iAcquireList and return pointer to  
// CAcquireEntry object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CAcquireEntry* CAcquireOwnerList::FindByTsyReqHandle(
    const TTsyReqHandle aTsyReqHandle )
    {
    CAcquireEntry* entry;
    TDblQueIter<CAcquireEntry> iter( iAcquireList );
    while( entry = iter++, entry != NULL )
        {
        if( entry->iTsyReqHandle == aTsyReqHandle )
            return entry;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CAcquireOwnerList::Remove
// delete the entry
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CAcquireOwnerList::Remove(
    CAcquireEntry* aEntry )
    {
    aEntry->Deque();
    delete aEntry;
    }

CHeartbeatRunner* CHeartbeatRunner::NewL( CMmCallTsy* aMmCall, CMmPhoneTsy* aMmPhone ) 
    {
    CHeartbeatRunner* runner = new ( ELeave ) CHeartbeatRunner;
    CleanupStack::PushL( runner );
    runner->iMmCall = aMmCall;
    runner->iMmPhone = aMmPhone;    
    runner->ConstructL();
    CleanupStack::Pop();
    return runner;
    }

CHeartbeatRunner::CHeartbeatRunner()
    {
    }

void CHeartbeatRunner::ConstructL()
    {
    // neutral priority, 0
    iHeartbeat = CHeartbeat::NewL( CActive::EPriorityStandard );
    iActive = EFalse; 
    iStartTime = 0;
    iAirTimeDuration = EFalse;
    iPreviousNumberOfBeats = 0;
    }

CHeartbeatRunner::~CHeartbeatRunner()
    {
    if ( iHeartbeat )
        {
        //stop calling Beat...
        iHeartbeat->Cancel();
        }

    delete iHeartbeat;
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::Start
// Starts timer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CHeartbeatRunner::Start()
    {
    //Check if active 
    if ( !iActive ) 
        {
        //Initialize iStartime
        iStartTime.UniversalTime();
        //Timer tick is on the second 
        iHeartbeat->Start( ETwelveOClock, this );
        //Set flag to indicate that timer is active
        iActive = ETrue;
        //Set flag to indicate that air time duration counter is not started
        iAirTimeDuration = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::StartAirTimeDuration
// Starts AirTimeDuration timer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CHeartbeatRunner::StartAirTimeDuration()
    {
    //Check if active 
    if ( !iActive ) 
        { 
        //Initialize iStartime
        iStartTime.UniversalTime();
        //Timer tick is on the second 
        iHeartbeat->Start( ETwelveOClock, this );
        //Set flag to indicate that timer is active
        iActive = ETrue;
        //Set flag to indicate that air time duration counter is started
        iAirTimeDuration = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::Stop
// Stops timer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CHeartbeatRunner::Stop()
    {
    TBool ret = iActive;

    //Check if active 
    if ( iActive ) 
        { 
        // Save the number of beats
        iPreviousNumberOfBeats = NumberOfBeats();
        //Reset iStartTime
        iStartTime = 0;
        //Cancel beat method calling 
        iHeartbeat->Cancel(); 
        //Set flag to indicate that timer is not active
        iActive = EFalse;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::Beat
// Beat is called once every second. If iMmCall is not null
// (timer is used for calculating the duration of a call), the method
// CompleteNotifyCallDurationChange is called (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CHeartbeatRunner::Beat()
    {
    // if iMmCall is not null, this timer is used to count the duration of a 
    // call
    if ( iMmCall )
        {
        //complete notify duration change, 1 sec gone
        iMmCall->CompleteNotifyCallDurationChange();
        }
        
    if ( iMmPhone )
        {
        //complete notify duration change, 10 sec gone
        if ( NumberOfBeats() % iMmPhone->KLifeTimeUpdateInterval == 0 )
        	{        	
        	iMmPhone->CompleteNotifyCallDurationChange();
        	}
        }                
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::NumberOfBeats
// Returns number of beats from the previous Start of the timer.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CHeartbeatRunner::NumberOfBeats()
    {
    if(iActive)
    	{
    	TTime currentTime;
    	currentTime.UniversalTime();
    	TTimeIntervalSeconds secondsFromStart( 0 );
    	currentTime.SecondsFrom( iStartTime, secondsFromStart );
    	return secondsFromStart.Int();
    	}
    return iPreviousNumberOfBeats;
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::Synchronize
// Implementation of MBeating::Synchronize. Since the number of beats are calculated
// by using start and actual current times, there is no need for synchronization.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CHeartbeatRunner::Synchronize()
    {
    // do nothing 
    }

// ---------------------------------------------------------------------------
// CHeartbeatRunner::GetTimerStartTime
// Returns the starting time of the timer as a TTime pointer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TTime* CHeartbeatRunner::GetTimerStartTime()
    {
    return &iStartTime;
    }

//  End of File 
