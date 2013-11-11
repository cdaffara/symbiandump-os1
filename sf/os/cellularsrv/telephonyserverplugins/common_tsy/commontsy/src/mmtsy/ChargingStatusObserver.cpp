/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ChargingStatusObserverTraces.h"
#endif

#include "ChargingStatusObserver.h"

// ======== MEMBER FUNCTIONS ========

CChargingStatusObserver* CChargingStatusObserver::NewL(
    MChargingStatusNotifier& aChargingStatusNotifier )
	{
	CChargingStatusObserver* self = CChargingStatusObserver::NewLC(  aChargingStatusNotifier );
	CleanupStack::Pop(self);
	return self;
	}
	
CChargingStatusObserver* CChargingStatusObserver::NewLC(
    MChargingStatusNotifier& aChargingStatusNotifier )
	{
	CChargingStatusObserver* self = new ( ELeave ) CChargingStatusObserver(
	    aChargingStatusNotifier );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CChargingStatusObserver::~CChargingStatusObserver()
	{
	Cancel();
	iChargingStatus.Close();
	}
	
void CChargingStatusObserver::ConstructL()
	{
	User::LeaveIfError( iChargingStatus.Attach( KPSUidHWRMPowerState,
	    KHWRMChargingStatus ) );
	RunL();
	}

CChargingStatusObserver::CChargingStatusObserver(
    MChargingStatusNotifier& aChargingStatusNotifier ) :
    CActive(EPriorityStandard),
    iChargingStatusNotifier( aChargingStatusNotifier )
	{
	CActiveScheduler::Add( this );
	}
	
// -----------------------------------------------------------------------------
// CChargingStatusObserver::RunL
// Handles CChargingStatusObserver::GetBatteryInfo request completion event.
// 
// -----------------------------------------------------------------------------
//		
void CChargingStatusObserver::RunL()
	{
    RMobilePhone::TMobilePhoneBatteryInfoV1 batteryInfo;
    batteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
    
	TInt errorStatus( iStatus.Int() );
	
    // resubscribe before processing new value to prevent missing updates
    iChargingStatus.Subscribe( iStatus );
    SetActive();
    
	if ( KErrNone != errorStatus )
	    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCHARGINGSTATUSOBSERVER_RUNL_1, "TSY: CChargingStatusObserver::RunL - iStatus: %{TSymbianErrorCodes}", errorStatus );
	    iChargingStatusNotifier.NotifyChargingStatus( errorStatus,
	        batteryInfo );
        return;
	    }
	    
    // property updated, get new value
    TInt level( 0 );
    TInt error = iChargingStatus.Get( level );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCHARGINGSTATUSOBSERVER_RUNL_2, "TSY: CChargingStatusObserver::RunL error: %{TSymbianErrorCodes}", error);

    if ( KErrNone == error )
        {
        EPSHWRMChargingStatus chargingStatus = (EPSHWRMChargingStatus)level;
  
        switch ( chargingStatus )      
            {
            case EChargingStatusNotConnected:
                batteryInfo.iStatus = RMobilePhone::EPoweredByBattery;
                break;
              
            case EChargingStatusAlmostComplete:
            case EChargingStatusChargingComplete:
            case EChargingStatusCharging:
                batteryInfo.iStatus = 
                    RMobilePhone::EBatteryConnectedButExternallyPowered;
                break;
                
            case EChargingStatusNotCharging:
                batteryInfo.iStatus = RMobilePhone::EPoweredByBattery;
                break;
                
            case EChargingStatusError:  
            default:
                batteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
                break;  
            }
        }
    iChargingStatusNotifier.NotifyChargingStatus( error, batteryInfo );
	}

// -----------------------------------------------------------------------------
// CChargingStatusObserver::DoCancel
// Cancellation of an outstanding request
//
// -----------------------------------------------------------------------------
//		
void CChargingStatusObserver::DoCancel()
	{
    iChargingStatus.Cancel();
	}

//  End of File
