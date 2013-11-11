/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



#include "stubtimer.h"
#include <e32svr.h>
#include "siftrace.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CStubTimer::CStubTimer() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CStubTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CStubTimer* CStubTimer::NewL()
    {
    CStubTimer* self = new( ELeave ) CStubTimer;
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CStubTimer::~CStubTimer()
    {
    iTimer.Close();
    }

void CStubTimer::Start( TTimeIntervalMicroSeconds32 aInterval, TCallBack aCallBack )
    {
    COMPONENT_TRACE( ( _L("CStubTimer::Start" ) ) );
    if( !IsActive() )
        {
        iCallBack = aCallBack;
        iInterval = aInterval;
        iTimer.HighRes( iStatus, iInterval );
        SetActive();
        }
    COMPONENT_TRACE( ( _L("CStubTimer::Start - End" ) ) );
    }
    

void CStubTimer::DoCancel()
    {
    iTimer.Cancel();
    }
    
    
void CStubTimer::RunL()
    {
    COMPONENT_TRACE( ( _L("CStubTimer::RunL - iStatus %d" ), iStatus.Int() ) );
    if( iStatus.Int() == KErrNone )
        {
        TInt ret = iCallBack.CallBack();
        if( ret )
            {
            iTimer.HighRes( iStatus, iInterval );
            SetActive();
            }
        }
    COMPONENT_TRACE( ( _L("CStubTimer::RunL - End" ) ) );
    }


// End of file
