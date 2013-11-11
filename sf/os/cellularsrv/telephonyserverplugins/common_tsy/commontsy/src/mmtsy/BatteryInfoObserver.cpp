/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
// INCLUDE FILES
#include "BatteryInfoObserver.h"

// ======== define patchable constants ======
// Note:  patchable constant cannot be defined in the source file it is referred to.
#ifdef __EABI__
// for HWRM support. 1 indicate HWRM is available. 0 indicate it is unavailable
EXPORT_C extern const TInt32 KHWRMIsSupported = 0;
#endif


// ======== MEMBER FUNCTIONS ========

CBatteryInfoObserver* CBatteryInfoObserver::NewL( CHWRMPower& aPower,
    MBatteryInfoNotifier& aBatteryInfoNotifier )
	{
	CBatteryInfoObserver* self = CBatteryInfoObserver::NewLC( aPower,
	    aBatteryInfoNotifier );
	CleanupStack::Pop(self);
	return self;
	}
	
CBatteryInfoObserver* CBatteryInfoObserver::NewLC( CHWRMPower& aPower,
    MBatteryInfoNotifier& aBatteryInfoNotifier )
	{
	CBatteryInfoObserver* self = new ( ELeave ) CBatteryInfoObserver( aPower,
	    aBatteryInfoNotifier );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

CBatteryInfoObserver::~CBatteryInfoObserver()
	{
	if ( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
	Cancel();
	}

void CBatteryInfoObserver::ConstructL()
	{
    iPeriodic = CPeriodic::NewL( EPriorityNormal );
    // Start the timer with 2 minute interval
    iPeriodic->Start( TTimeIntervalMicroSeconds32(KHalfSecond),
        TTimeIntervalMicroSeconds32(KTwoMins),
        TCallBack(CBatteryInfoObserver::RunTimer,
        this ));
	}	

CBatteryInfoObserver::CBatteryInfoObserver( CHWRMPower& aPower,
    MBatteryInfoNotifier& aBatteryInfoNotifier ) :
    CActive( EPriorityStandard ),
    iPower( aPower ),
    iBatteryInfoNotifier( aBatteryInfoNotifier )
	{
	CActiveScheduler::Add(this);
	}
	
// -----------------------------------------------------------------------------
// CBatteryInfoObserver::RunL
// Handles CBatteryInfoObserver::GetBatteryInfo request completion event.
// 
// -----------------------------------------------------------------------------
//		
void CBatteryInfoObserver::RunL()
	{
    iBatteryInfoNotifier.NotifyBatteryInfo( iStatus.Int(), iBatteryData );
	}

// -----------------------------------------------------------------------------
// CBatteryInfoObserver::DoCancel
// Cancellation of an outstanding request
//
// -----------------------------------------------------------------------------
//		
void CBatteryInfoObserver::DoCancel()
	{
    //no cancel for GetBatteryInfo
	}

// -----------------------------------------------------------------------------
// CBatteryInfoObserver::GetBatteryInfo
// Get battery info from HWRM.
//
// -----------------------------------------------------------------------------
//
void CBatteryInfoObserver::GetBatteryInfo()
    {
    //Check if active 
    if ( !IsActive() )
        {
        iPower.GetBatteryInfo( iStatus, iBatteryData );
        SetActive();
        }
    }
    
// -----------------------------------------------------------------------------
// CBatteryInfoObserver::RunTimer
// Execute GetBatteryInfo()
// 
// -----------------------------------------------------------------------------
//    
TInt CBatteryInfoObserver::RunTimer( TAny* aObject )
    {
    ( ( CBatteryInfoObserver* ) aObject )->GetBatteryInfo();
    return KErrNone;
    }

//  End of File
