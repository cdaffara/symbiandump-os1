// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32svr.h>
#include "HWRMBatteryPowerMeasurementsAO.h"
#include "HWRMPowerClientServer.h"
#include "HWRMtrace.h"
#include "HWRMClient.h"
#include "HWRMPowerImpl.h"

#ifdef SYMBIAN_HWRM_EXTPOWERINFO

#include "HWRMBatteryChargeCurrentRateAO.h"
#include "HWRMBatteryChargeTimeMeasurementsAO.h"
#include "HWRMChargingStatusObserver.h"

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMPowerImpl* CHWRMPowerImpl::NewL()
	{
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::NewL()"));
    
    CHWRMPowerImpl* powerImpl = new( ELeave ) CHWRMPowerImpl();
    
    CleanupStack::PushL( powerImpl );        
    powerImpl->ConstructL();        
    CleanupStack::Pop(); 
    
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::NewL - return" ));
    
    return powerImpl;
	}

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::CHWRMPowerImpl
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMPowerImpl::CHWRMPowerImpl()
	:iBatteryConsumptionDataPckg(NULL, 0, 0),
	iBatteryVoltageDataPckg(NULL, 0, 0),
	iPowerIntervalMultiplePckg(NULL,0,0)	
	
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
	,iBatteryChargerTypePckg(NULL, 0, 0),
	iChargingStatusImpl(NULL),
	iCurrentRateIntervalMultiplePckg(NULL,0,0)
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	{	
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::CHWRMPowerImpl()"));
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::CHWRMPowerImpl - return" ));
	}

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMPowerImpl::~CHWRMPowerImpl()
	{
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::~CHWRMPowerImpl()" ));
	
	Close();	
	
    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    
    iBatteryChargeCurrentRateAO = NULL;
    iBatteryChargingTimeAO = NULL;
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::~CHWRMPowerImpl - return" ));
	}
// -----------------------------------------------------------------------------
// CHWRMPowerImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::ConstructL()
	{
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::ConstructL()" ));
	
	OpenL();
	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::ConstructL - return" ) );    
	}

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::OpenL
// Opens session
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::OpenL()
    {
    COMPONENT_TRACE1( _L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Open()" ));
        
    User::LeaveIfError(iClient.Connect(HWRMResourceTypePower));
    
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Open - Craeting Active objects for measurements reporting" ));
    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO

    //Create observer here, so that charging current/time notification can be stopped,
    //if charger is disconnected.
    
    iChargingStatusImpl = CHWRMChargingStatusObs::NewL(this);

#endif //SYMBIAN_HWRM_EXTPOWERINFO
    	
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Open - return" ));
    
    }
// -----------------------------------------------------------------------------
// CHWRMPowerImpl::Close
// Closes session.
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::Close()
	{
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close()" ));
            
    if ( iBatteryPowerMeasurementsAO)
		{
		COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close - canceling BatteryPowerMeasurements notifications" ));
		delete iBatteryPowerMeasurementsAO;
		iBatteryPowerMeasurementsAO = NULL;
		}


#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        
        delete iChargingStatusImpl;
        iChargingStatusImpl = NULL;
        COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close Deleted iChargingStatusImpl" ));        

        delete iBatteryChargeCurrentRateAO;
        iBatteryChargeCurrentRateAO = NULL;

        delete iBatteryChargingTimeAO;
        iBatteryChargingTimeAO = NULL;

#endif //SYMBIAN_HWRM_EXTPOWERINFO

	// Inform session that we are cleaning up now.
	// Ignore errors, just trace them
	if (iClient.Handle())
		{
		iClient.ExecuteOperation(EHWRMCleanupBattery, TIpcArgs());
        }

    iClient.Close();
      
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close - return" ) );
    }    

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoGetBatteryInfo
// 
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::DoGetBatteryInfo(TRequestStatus& aStatus, CHWRMPower::TBatteryConsumptionData& aBatteryConsumptionData )
	{
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryInfo()" ));

	iBatteryConsumptionDataPckg.Set( TPckg<CHWRMPower::TBatteryConsumptionData> (aBatteryConsumptionData) );
	
	iClient.ExecuteAsyncOperation(EHWRMGetBatteryInfo, TIpcArgs(&iBatteryConsumptionDataPckg), aStatus);

	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryInfo - return" ));
	}

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoGetVoltageInfo
// 
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::DoGetBatteryVoltageInfo(TRequestStatus& aStatus, CHWRMPower::TBatteryVoltageData& aBatteryVoltageData)
	{
	COMPONENT_TRACE1(_L( "HWRM PowerClient - CHWRMPowerImpl::DoGetBatteryVoltageInfo()" ));
	
	iBatteryConsumptionDataPckg.Set( TPckg<CHWRMPower::TBatteryVoltageData> (aBatteryVoltageData) );
	
	iClient.ExecuteAsyncOperation(EHWRMGetBatteryVoltageInfo, TIpcArgs(&iBatteryConsumptionDataPckg), aStatus);
	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryVoltageInfo - return" ));
	}
// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoStartAveragePowerReporting
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMPowerImpl::DoStartAveragePowerReporting(TRequestStatus& aStatus, const TUint& aInterval)
	{	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStartAveragePowerReporting()" ));
	
	if (!iBatteryPowerMeasurementsAO)
		{
		COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStartAveragePowerReporting - return( - return 0x%x" ), KErrBadHandle );
		
		return KErrBadHandle;
		}
	else 
		{		
		iPowerIntervalMultiplePckg.Set( TPckg<TUint> (aInterval) );
		
		iClient.ExecuteAsyncOperation(EHWRMStartAverageBatteryPowerReporting, TIpcArgs(&iPowerIntervalMultiplePckg), aStatus);
		
		//Issue request to retrieve battery power measurements	
		if( !iBatteryPowerMeasurementsAO->IsActive())
			{
			iBatteryPowerMeasurementsAO->GetAsyncBatch( EHWRMGetPowerMeasurements );	
			}		
		COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStartAveragePowerReporting - return( - return 0x%x" ), KErrNone );
		
	    return KErrNone;
		}
	}

void CHWRMPowerImpl::DoStopAveragePowerReportingL()
	{	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStopAveragePowerReportingL()" ));
	
	if (iBatteryPowerMeasurementsAO)
		{
		COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close - canceling BatteryPowerMeasurements notifications" ));
		iBatteryPowerMeasurementsAO->Cancel();		
	
		User::LeaveIfError(iClient.ExecuteOperation(EHWRMStopAverageBatteryPowerReporting, TIpcArgs()));
		}

	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStopAveragePowerReportingL - return" ));
	}


// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoSetPowerReportObserverL
// 
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::DoSetPowerReportObserverL(MHWRMBatteryPowerObserver* aPowerReportCallback)
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoSetPowerReportObserverL()" ));


    if(aPowerReportCallback)
        {
        if(iBatteryPowerMeasurementsAO)
            {
            iBatteryPowerMeasurementsAO->SetCallBack(aPowerReportCallback);
            }
        else
            {
            iBatteryPowerMeasurementsAO = CHWRMBatteryPowerMeasurementsAO::NewL(aPowerReportCallback, iClient);
            }
        COMPONENT_TRACE1(_L( "iBatteryPowerMeasurementsAO Created" ));    
        }

    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoSetPowerReportObserverL - return" ));
    }


#ifdef SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoGetBatteryChargerType
// 
// -----------------------------------------------------------------------------
//


TInt CHWRMPowerImpl::DoGetBatteryChargerType(CHWRMPower::TBatteryChargerType& aChargerType)
    {
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryChargerType()" ));

	iBatteryChargerTypePckg.Set( TPckg<CHWRMPower::TBatteryChargerType> (aChargerType) );
	
	TInt err = iClient.ExecuteOperation(EHWRMGetBatteryChargerType, TIpcArgs(&iBatteryChargerTypePckg));

    COMPONENT_TRACE3(_L( "HWRM CHWRMPowerImpl - Charger Type = %d err = %d" ), aChargerType, err);    

	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryChargerType - return" ));

    return err;
	}

TInt CHWRMPowerImpl::DoGetBatteryChargingStatus(CHWRMPower::TBatteryChargingStatus& aChargingStatus)
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryChargingStatus()" ));

    TInt val;

    aChargingStatus = CHWRMPower::EChargingStatusError;

    TInt err = iChargingStatusImpl->Get(val);

    if(err == KErrNone)
        {
        aChargingStatus = static_cast<CHWRMPower::TBatteryChargingStatus>(val);
        }

	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoGetBatteryChargingStatus - return" ));

    return err;
    }


// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoSetBatteryChargingObserver
// 
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::DoSetBatteryChargingObserverL(
             MHWRMBatteryChargingStatusObserver*   aChargingStatusCallback,
             MHWRMBatteryChargingCurrentObserver*     aChrgCurrentCallback,
             MHWRMBatteryFullChargingTimeObserver* aChrgTimeCallback)
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoSetBatteryChargingObserver()" ));

    if(aChargingStatusCallback)
        {
        iChargingStatusClientObserver = aChargingStatusCallback;
        COMPONENT_TRACE1(_L( "iChargingStatusImpl Created" ));        
        }

    if(aChrgCurrentCallback)
        {
        if(iBatteryChargeCurrentRateAO)
            {
            iBatteryChargeCurrentRateAO->SetCallBack(aChrgCurrentCallback);
            }
        else
            {
            iBatteryChargeCurrentRateAO = CHWRMBatteryChargeRateCurrentAO::NewL(aChrgCurrentCallback, iClient);
            }
        COMPONENT_TRACE1(_L( "iBatteryChargeCurrentRateAO Created" ));    
        }

    if(aChrgTimeCallback)
        {
        if(iBatteryChargingTimeAO)
            {
            iBatteryChargingTimeAO->SetCallBack(aChrgTimeCallback);
            }
        else
            {
            iBatteryChargingTimeAO = CHWRMBatteryChargeTimeMeasurementsAO::NewL(aChrgTimeCallback, iClient);
            }
        COMPONENT_TRACE1(_L( "iBatteryChargingTimeAO Created" ));    
        }
    

    COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoSetBatteryChargingObserver - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoNotifyAverageChargingCurrent
// 
// -----------------------------------------------------------------------------
//


TInt CHWRMPowerImpl::DoNotifyAverageChargingCurrent(TRequestStatus& aStatus, const TUint& aInterval)
    {	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoNotifyAverageChargingCurrent()" ));

	if (!iBatteryChargeCurrentRateAO)
		{
		COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoNotifyAverageChargingCurrent - return( - return 0x%x" ), KErrBadHandle );
		
		return KErrBadHandle;
		}

    TInt err(KErrNone);

    CHWRMPower::TBatteryChargingStatus chargingStatus;

    err = DoGetBatteryChargingStatus(chargingStatus);
    
    if((err != KErrNone) || 
       (chargingStatus <= CHWRMPower::EChargingStatusError) || 
       (chargingStatus == CHWRMPower::EChargingStatusNotConnected) || 
       (chargingStatus == CHWRMPower::EChargingStatusNotCharging))
        {
        err = KErrNotReady;
        }
	else 
		{		
		iCurrentRateIntervalMultiplePckg.Set( TPckg<TUint> (aInterval) );
		
		iClient.ExecuteAsyncOperation(EHWRMStartAverageChargingCurrentReporting, TIpcArgs(&iCurrentRateIntervalMultiplePckg), aStatus);
		
		//Issue request to retrieve battery voltage measurements	
		if( !iBatteryChargeCurrentRateAO->IsActive())
			{
			iBatteryChargeCurrentRateAO->GetAsyncBatch( EHWRMGetAverageChargingCurrentMeasurements );	
			}		
		

        err = KErrNone;
		}
    
    
    COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoNotifyAverageChargingCurrent - return( - return 0x%x" ), err );
    
    return err;
	}


// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoNotifyBatteryFullChargingTime
// 
// -----------------------------------------------------------------------------
//
TInt CHWRMPowerImpl::DoNotifyBatteryFullChargingTime(TRequestStatus& aStatus)
    {	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoNotifyBatteryFullChargingTime()" ));

	if (!iBatteryChargingTimeAO)
		{
		COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoNotifyBatteryFullChargingTime - return( - return 0x%x" ), KErrBadHandle );
		
		return KErrBadHandle;
		}

    TInt err;

    CHWRMPower::TBatteryChargingStatus chargingStatus;

    err = DoGetBatteryChargingStatus(chargingStatus);
    
    if((err != KErrNone) || 
       (chargingStatus <= CHWRMPower::EChargingStatusError) ||
       (chargingStatus == CHWRMPower::EChargingStatusNotConnected) || 
       (chargingStatus == CHWRMPower::EChargingStatusNotCharging))
        {
        err = KErrNotReady;
        }
	else 
		{		
				
		iClient.ExecuteAsyncOperation(EHWRMStartBatteryFullChargingTimeReporting, TIpcArgs(), aStatus);
		
		//Issue request to retrieve battery full charge time measurements	
		if( !iBatteryChargingTimeAO->IsActive())
			{
			iBatteryChargingTimeAO->GetAsyncBatch( EHWRMGetBatteryFullChargingTimeMeasurements );	
			}		
        
	    err = KErrNone;        
		}
    

    COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoNotifyBatteryFullChargingTime - return( - return 0x%x" ), err );
    
    return err;
	}

// -----------------------------------------------------------------------------
// CHWRMPowerImpl::DoStopChargingNotificationL
// 
// -----------------------------------------------------------------------------
//
void CHWRMPowerImpl::DoStopChargingNotificationL(CHWRMPower::TBatteryChargingNotifier aNotifier)
	{	
	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStopChargingNotificationL()" ));
	
	if (((aNotifier == CHWRMPower::EChargingNotifierAll)||(aNotifier == CHWRMPower::EChargingNotifierChargingCurrent))
        &&(iBatteryChargeCurrentRateAO))
		{
		COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close - canceling current measurement notifications" ));
		iBatteryChargeCurrentRateAO->Cancel();		
	
		User::LeaveIfError(iClient.ExecuteOperation(EHWRMStopAverageChargingCurrentReporting, TIpcArgs()));

		}

    
    if (((aNotifier == CHWRMPower::EChargingNotifierAll)||(aNotifier == CHWRMPower::EChargingNotifierBatteryFullChargingTime))
            &&(iBatteryChargingTimeAO))
            {
            COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::Close - canceling rem charging time measurement notifications" ));
            iBatteryChargingTimeAO->Cancel();      
        
            User::LeaveIfError(iClient.ExecuteOperation(EHWRMStopBatteryFullChargingTimeReporting, TIpcArgs()));

            }

	COMPONENT_TRACE1(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::DoStopChargingNotificationL - return" ));
	}

void CHWRMPowerImpl::ChargingStatusChange(TInt aErrCode,CHWRMPower::TBatteryChargingStatus aChrgStatus)
    {
    if(iChargingStatusClientObserver)
        {
        iChargingStatusClientObserver->ChargingStatusChange(aErrCode, aChrgStatus);
        }

    //Test if notification is already stopped.
    //If already stopped, no need to cancel notification.
    TBool batteryChargeCurrentNotificationOn(EFalse);
    TBool batteryChargingTimeNotificationOn(EFalse);
    if(iBatteryChargeCurrentRateAO)
        {
        batteryChargeCurrentNotificationOn = iBatteryChargeCurrentRateAO->IsActive();
        }
    if(iBatteryChargingTimeAO)
        {
        batteryChargingTimeNotificationOn = iBatteryChargingTimeAO->IsActive();
        }
    COMPONENT_TRACE3(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::ChargingStatusChange batteryChargeCurrentNotificationOn=%d batteryChargingTimeNotificationOn=%d" ), batteryChargeCurrentNotificationOn, batteryChargingTimeNotificationOn );
    
    if(batteryChargeCurrentNotificationOn || batteryChargingTimeNotificationOn)
        {
        //Avoid using multiple TRAP's in "if and else" part below.
        //Just store whether notification(and corresponding code) needs to stopped or not.
        TBool                                stopchargingNotification(ETrue);
        CHWRMPower::TBatteryChargingNotifier cancelNotificationCode;
        
        //Stop charging notifications, if charger is not connected.
        if((aErrCode != KErrNone) || 
           (aChrgStatus <= CHWRMPower::EChargingStatusError) ||
           (aChrgStatus == CHWRMPower::EChargingStatusNotConnected) || 
           (aChrgStatus == CHWRMPower::EChargingStatusNotCharging))
            {
            cancelNotificationCode       = CHWRMPower::EChargingNotifierAll;
            }
        //Stop remaining charging time notification, if charging is completed.
        else if(aChrgStatus == CHWRMPower::EChargingStatusChargingComplete)
            {
            cancelNotificationCode   = CHWRMPower::EChargingNotifierBatteryFullChargingTime;
            }
        else
            {
            stopchargingNotification = EFalse;
            }

        COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::ChargingStatusChange stopchargingNotification 0x%x" ), stopchargingNotification );
    
        if(stopchargingNotification)
            {
            
            TInt err;
            TRAP(err, DoStopChargingNotificationL(cancelNotificationCode));
    
            if(err == KErrNone)
                {
                //Since the measurements are stopped by HWRM(because of some external events),
                //HWRM clients Notifications needs to be cancelled.
            
                //Charging time notification is cancelled in both cases(EChargingNotifierAll and EChargingNotifierBatteryFullChargingTime)
                if(batteryChargingTimeNotificationOn)
                    {
                    iBatteryChargingTimeAO->CancelClientNotification();
                    }
            
                if(cancelNotificationCode == CHWRMPower::EChargingNotifierAll) 
                    {
                    if(batteryChargeCurrentNotificationOn)
                        {
                        iBatteryChargeCurrentRateAO->CancelClientNotification();
                        }
                    }
                }
            else
                {
                //Just trace the error,as nothing can be done.
                COMPONENT_TRACE2(_L( "HWRM CHWRMPowerImpl - CHWRMPowerImpl::ChargingStatusChange stop failed with 0x%x" ), err );
                }
            }
        }
    }

#endif //SYMBIAN_HWRM_EXTPOWERINFO

