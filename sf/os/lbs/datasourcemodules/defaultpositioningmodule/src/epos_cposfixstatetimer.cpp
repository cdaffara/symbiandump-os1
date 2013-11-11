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
#include "epos_defaultproxycommon.h"
#include "epos_cposfixstatetimer.h"
#include "epos_cpossingletonmanager.h"
#include "epos_mposfixstatetimerlistener.h"

#include <lbs/epos_cpositioner.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosFixStateTimer::CPosFixStateTimer( 
        TPositionModuleId aModuleId,
        MPosFixStateTimerListener& aListener ) 
    : iModuleId( aModuleId ),
      iListener( aListener )
    {
    }

// EPOC default constructor can leave.
void CPosFixStateTimer::ConstructL()
    {
    iTimeshiftTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iUnknownTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// Two phase constructor
CPosFixStateTimer* CPosFixStateTimer::NewL(
        TPositionModuleId aModuleId,
        MPosFixStateTimerListener& aListener )
    {
    CPosFixStateTimer* self = new ( ELeave ) CPosFixStateTimer( 
        aModuleId,
        aListener );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosFixStateTimer::~CPosFixStateTimer()
    {
    if ( iTimeshiftTimer )
        {
        iTimeshiftTimer->Cancel();
        delete iTimeshiftTimer;
        }
        
    if ( iUnknownTimer )
        {
        iUnknownTimer->Cancel();
        delete iUnknownTimer;
        }
    }

// ---------------------------------------------------------
// CPosFixStateTimer::StartTimeshiftTimer
// ---------------------------------------------------------
//
void CPosFixStateTimer::StartTimeshiftTimer(
            TTimeIntervalMicroSeconds aInterval )
    {
    if ( iTimeshiftTimer->IsActive() )
        {
        return;
        }
        
    iTimeshiftTimer->Start(
        aInterval.Int64(),
        0,
        TCallBack( TimeshiftTimeoutCallback, this ) );
    
    }
            
// ---------------------------------------------------------
// CPosFixStateTimer::CancelTimeshiftTimer
// ---------------------------------------------------------
//
void CPosFixStateTimer::CancelTimeshiftTimer()
    {
    iTimeshiftTimer->Cancel();
    }
        
// ---------------------------------------------------------
// CPosFixStateTimer::StartUnknownTimer
// ---------------------------------------------------------
//
void CPosFixStateTimer::StartUnknownTimer(
            TTimeIntervalMicroSeconds aInterval )
    {
    if ( iUnknownTimer->IsActive() )
        {
        return;
        }
        
    iUnknownTimer->Start(
        aInterval.Int64(),
        0,
        TCallBack( UnknownTimeoutCallback, this ) );
    }
        
// ---------------------------------------------------------
// CPosFixStateTimer::CancelUnknownTimer
// ---------------------------------------------------------
//
void CPosFixStateTimer::CancelUnknownTimer()
    {
    iUnknownTimer->Cancel();
    }

// ---------------------------------------------------------
// CPosFixStateTimer::ModuleId
// ---------------------------------------------------------
//
TPositionModuleId CPosFixStateTimer::ModuleId() const
    {
    return iModuleId;
    }

// ---------------------------------------------------------
// CPosFixStateTimer::TimeshiftTimeoutCallback
// ---------------------------------------------------------
//
TInt CPosFixStateTimer::TimeshiftTimeoutCallback( TAny* aAny )
    {
    reinterpret_cast < CPosFixStateTimer* > ( aAny )-> 
        TimeshiftTimeout();
    return KErrNone;
    }
        
// ---------------------------------------------------------
// CPosFixStateTimer::TimeshiftTimeout
// ---------------------------------------------------------
//
void CPosFixStateTimer::TimeshiftTimeout()
    {
    iTimeshiftTimer->Cancel();
    iListener.TimeshiftTimerTimeout( iModuleId );
    }
        
// ---------------------------------------------------------
// CPosFixStateTimer::UnknownTimeoutCallback
// ---------------------------------------------------------
//
TInt CPosFixStateTimer::UnknownTimeoutCallback( TAny* aAny )
    {
    reinterpret_cast < CPosFixStateTimer* > ( aAny )-> 
        UnknownTimeout();
    return KErrNone;
    }
        
// ---------------------------------------------------------
// CPosFixStateTimer::UnknownTimeout
// ---------------------------------------------------------
//
void CPosFixStateTimer::UnknownTimeout()
    {
    iUnknownTimer->Cancel();
    iListener.UnknownTimerTimeout( iModuleId );
    }

//  End of File
