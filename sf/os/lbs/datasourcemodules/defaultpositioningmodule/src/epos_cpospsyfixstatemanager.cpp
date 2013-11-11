// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "epos_cpospsyfixstatemanager.h"
#include "epos_cpospsylisthandler.h"
#include "epos_cpossingletonmanager.h"
#include "epos_cposfixstatetimer.h"
#include "epos_defaultproxycommon.h"
#include "epos_cposconstmanager.h"
#include "epos_mpospsyfixstatelistener.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPsyFixStateManager::CPosPsyFixStateManager()
    {
    }

// EPOC default constructor can leave.
void CPosPsyFixStateManager::ConstructL()
    {
    TRACESTRING( "CPosPsyFixStateManager::ConstructL start... " )

    iConstManager = CPosConstManager::GetInstanceL();

    TRACESTRING( "CPosPsyFixStateManager::ConstructL end " )

    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::GetInstanceL
// ---------------------------------------------------------
//
CPosPsyFixStateManager* CPosPsyFixStateManager::GetInstanceL()
    {
    CPosPsyFixStateManager* self = reinterpret_cast < CPosPsyFixStateManager* > 
        ( CPosSingletonManager::GetObject( 
            EPosSigletonObjectIdPsyFixStateManager ) );
        
    if ( !self )
        {
        //Construct a new object and store it to CPosSingletonManager
        self = new ( ELeave ) CPosPsyFixStateManager;
        CleanupStack::PushL(self);
        self->ConstructL();
        CPosSingletonManager::SetObjectL(
            self,
            EPosSigletonObjectIdPsyFixStateManager );
        CleanupStack::Pop(self);
        }
        
    self->iRefCount++;
    
    return self;
    }

// Destructor
CPosPsyFixStateManager::~CPosPsyFixStateManager()
    {
    TRACESTRING( "CPosPsyFixStateManager::destructor start... " )

    if ( iConstManager )
        {
        iConstManager->ReleaseInstance();
        }
        
    //Clear fix state timer
    TInt count = iPsyStateArray.Count();
    for ( TInt i=0; i<count; i++)
        {
        delete iPsyStateArray[i].iTimer;
        }
        
    iPsyStateArray.Close();
    iListenerArray.Close();

    TRACESTRING( "CPosPsyFixStateManager::destructor end... " )
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::ReleaseInstance
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::ReleaseInstance()
    {
    iRefCount--;
    if ( iRefCount == 0 )
        {
        //We shall delete the instance
        CPosSingletonManager::ReleaseObject(
            EPosSigletonObjectIdPsyFixStateManager );
        }
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::AddListenerL
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::AddListenerL( MPosPsyFixStateListener* aListener )
    {
    User::LeaveIfError( iListenerArray.Append( aListener ) );
    }
        
// ---------------------------------------------------------
// CPosPsyFixStateManager::RemoveListener
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::RemoveListener( MPosPsyFixStateListener* aListener )
    {
    TInt count = iListenerArray.Count();
    for ( TInt i=count -1; i>=0; i-- )
        {
        if( iListenerArray[i] == aListener )
            {
            iListenerArray.Remove( i );
            }
        }
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::PsyLoadedL
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::PsyLoadedL(
            TPositionModuleId aPsyId )
    {
    TRACESTRING( "CPosPsyFixStateManager::PsyLoadedL" )
    TRACESTRING2( "PSY: %x", aPsyId )    
    if ( PsyIndex( aPsyId ) == KErrNotFound )
        {
        //If PSY is not in the PSY state array, add PSY to 
        //the array and set the fix state as unkown.
        CPosFixStateTimer* timer = CPosFixStateTimer::NewL(
            aPsyId, 
            *this );
        CleanupStack::PushL( timer );
        
        TPsyAndFixState fixAndState;
        fixAndState.iPsyId = aPsyId;
        fixAndState.iPsyState = EPsyFixStateUnknown;
        fixAndState.iLocationRequestCount = 0;
        fixAndState.iTimer = timer;
        iPsyStateArray.AppendL( fixAndState ) ;           
        CleanupStack::Pop( timer );
        }
    }
        
// ---------------------------------------------------------
// CPosPsyFixStateManager::GetPsyFixState
// ---------------------------------------------------------
//
CPosPsyFixStateManager::TPsyFixState CPosPsyFixStateManager::GetPsyFixState(
            TPositionModuleId aPsyId )
    {
    TInt index = PsyIndex( aPsyId );
    if ( index != KErrNotFound )
        {
        return iPsyStateArray[index].iPsyState;
        }
        
    //If PSY has not been loaded before, return fix state
    //as unknown.
    return EPsyFixStateUnknown;
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::PsyUsed
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::PsyUsed( 
        TPositionModuleId aPsyId,
        TPositionModuleStatus::TDeviceStatus aPreviousDeviceStatus )
    {
    TRACESTRING( "CPosPsyFixStateManager::PsyUsed" )
    TRACESTRING2( "PSY: %x", aPsyId )
    TRACESTRING2( "Device status: %d", aPreviousDeviceStatus)

    TInt index = PsyIndex( aPsyId );
    __ASSERT_DEBUG( index != KErrNotFound,
        DefaultProxyPanic( EDefaultProxyPanic_PsyNotFoundInStateManager ) );
    
    if( index != KErrNotFound )
        {
        TPsyAndFixState& fixState = iPsyStateArray[index];
        //If PSY has not been used before, then start timeshift timer
        if( fixState.iLocationRequestCount == 0 )
            {
            StartTimeshiftTimer( fixState, aPreviousDeviceStatus );
            }
            
        //Cancel unknown timer
        fixState.iTimer->CancelUnknownTimer();
            
        fixState.iLocationRequestCount++;
        }
    }


// ---------------------------------------------------------
// CPosPsyFixStateManager::SetPsyFixState
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::SetPsyFixState( 
            TPositionModuleId aPsyId, 
            TInt aLRErr  )
    {
    TRACESTRING( "CPosPsyFixStateManager::SetPsyFixState" )
    TRACESTRING2( "PSY: %x", aPsyId )
    TRACESTRING2( "Err: %d", aLRErr )

    TInt index = PsyIndex( aPsyId );
    if ( index != KErrNotFound )
        {
        TPsyAndFixState& fixState = iPsyStateArray[index];

        TPsyFixState previousFixState = fixState.iPsyState;
             
        switch ( aLRErr )
            {
            case KErrNone:
                //Location request completed successfully.
                //Stop timeshift timer
                fixState.iPsyState = EPsyFixStateYes;
                fixState.iTimer->CancelTimeshiftTimer();
                break;
            case KPositionPartialUpdate:
            case KErrPositionBufferOverflow:
            case KErrCancel:
                //State not changed.
                //Leave timeshift timer running
                break;
            default:
                //Stop timeshift timer
                fixState.iPsyState = EPsyFixStateNo;
                fixState.iTimer->CancelTimeshiftTimer();
                break;
            }

        //Notify liseners
        NotifyListener( aPsyId, previousFixState );
        

        //If the decrease the LR count for this PSY
        if( fixState.iLocationRequestCount > 0 )
            {
            fixState.iLocationRequestCount--;

            //Check if the PSY is not used anymore
            if ( fixState.iLocationRequestCount == 0 )
                {
                //if the PSY is not used anymore, start
                //unknown timer
                fixState.iTimer->StartUnknownTimer(
                    iConstManager->GetUnusedTimeoutValue() );
                }
            }
                
        }
    //If the PSY has not been loaded, we don't do anything.
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::SetExtGpsPsyFixState
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::SetExtGpsPsyFixState(
            TPositionModuleId aPsyId, 
            TInt aLRErr )
    {
    TRACESTRING( "CPosPsyFixStateManager::SetExtGpsPsyFixState" )
    TRACESTRING2( "PSY: %x", aPsyId )
    TRACESTRING2( "Err: %d", aLRErr )

    TInt index = PsyIndex( aPsyId );
    if ( index != KErrNotFound )
        {
        if ( aLRErr == KErrNone )
            {
            TPsyFixState previousFixState = iPsyStateArray[index].iPsyState;
            iPsyStateArray[index].iPsyState = EPsyFixStateYes;
            NotifyListener( aPsyId, previousFixState );
            }
        //otherwise the state will not be changed.
        }
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::TimeshiftTimerTimeout
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::TimeshiftTimerTimeout( 
            TPositionModuleId aPsyId )
    {
    TRACESTRING( "CPosPsyFixStateManager::TimeshiftTimerTimeout" )
    TRACESTRING2( "PSY: %x", aPsyId )

    TInt index = PsyIndex( aPsyId );
    if ( index != KErrNotFound )
        {
        TPsyFixState previousFixState = iPsyStateArray[index].iPsyState;

        iPsyStateArray[index].iPsyState = EPsyFixStateNo;
        
        TRACESTRING2( "PSY fix sate: %d", 
            iPsyStateArray[index].iPsyState );
            
        //Notify listner that PSY fix state is changed
        NotifyListener( aPsyId, previousFixState );
        }
    }
            
// ---------------------------------------------------------
// CPosPsyFixStateManager::UnknownTimerTimeout
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::UnknownTimerTimeout(
            TPositionModuleId aPsyId )
    {
    TRACESTRING( "CPosPsyFixStateManager::UnknownTimerTimeout" )
    TRACESTRING2( "PSY: %x", aPsyId )

    TInt index = PsyIndex( aPsyId );
    if ( index != KErrNotFound )
        {
        TPsyFixState previousFixState = iPsyStateArray[index].iPsyState;
        
        //Cancel timeshift timer
        iPsyStateArray[index].iTimer->CancelTimeshiftTimer();

        iPsyStateArray[index].iPsyState = EPsyFixStateUnknown;
        
        TRACESTRING2( "PSY fix sate: %d", 
            iPsyStateArray[index].iPsyState );
            
        //Notify listner that PSY fix state is changed
        NotifyListener( aPsyId, previousFixState );
        }
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::PsyIndex
// ---------------------------------------------------------
//
TInt CPosPsyFixStateManager::PsyIndex(
            TPositionModuleId aPsyId )
    {
    TInt count = iPsyStateArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPsyStateArray[i].iPsyId == aPsyId )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::StartTimeshiftTimer
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::StartTimeshiftTimer(
            TPsyAndFixState& aFixState,
            TPositionModuleStatus::TDeviceStatus aPreviousDeviceStatus )
    {
    TTimeIntervalMicroSeconds ttff;
    TTimeIntervalMicroSeconds ttnf;
    CPosPsyListHandler* listHandler = NULL;
    TInt ignore = KErrNone;
    TRAP( ignore, listHandler = 
            CPosPsyListHandler::GetInstanceL() );
                
    __ASSERT_DEBUG( listHandler != NULL,  
        DefaultProxyPanic( 
            EDefaultProxyPanic_GlobalObjectNotConstructed ) );
        
    listHandler->GetTtffAndTtnf(
        aFixState.iPsyId,
        ttff,
        ttnf );
            
    listHandler->ReleaseInstance();
    //Start timeshift timer
    aFixState.iTimer->StartTimeshiftTimer(
        iConstManager->GetTimeshiftValue(
            ttff,
            ttnf,
            aPreviousDeviceStatus ) );
    }

// ---------------------------------------------------------
// CPosPsyFixStateManager::NotifyListener
// ---------------------------------------------------------
//
void CPosPsyFixStateManager::NotifyListener(
            TPositionModuleId aPsyId,
            TPsyFixState aPreviousFixState )
    {
    TInt index = PsyIndex( aPsyId );
    if ( index != KErrNotFound )
        {
        TPsyFixState newState = iPsyStateArray[index].iPsyState;
        if ( newState != aPreviousFixState )
            {
            TInt count = iListenerArray.Count();
            for ( TInt i = count-1; i >= 0; i-- )
                {
                iListenerArray[i]->PsyFixStateChanged(
                    aPsyId,
                    newState );
                }
            }
        }
    }

//  End of File
