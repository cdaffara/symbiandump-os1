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




// INCLUDE FILES
#include <centralrepository.h>
#include "HWRMtrace.h"
#include "HWRMPowerCommonData.h"
#include "HWRMClientServer.h"
#include "HWRMPowerDomainCRKeys.h"
#include "HWRMPrivateCRKeys.h"
#include "HWRMConfiguration.h"
#include "HWRMPowerService.h"
#include "HWRMPower.h"
#include "HWRMPowerService.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
const TInt KMilliToMicroSeconds = 1000;

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::CHWRMPowerCommonData
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMPowerCommonData::CHWRMPowerCommonData()
	:iBatteryInfoQue(_FOFF(TBatteryInfoData,iSglQueLink)),
	iBatteryInfoIter(iBatteryInfoQue),
	iBatteryVoltageInfoQue(_FOFF(TBatteryInfoData,iSglQueLink)),
	iBatteryVoltageInfoIter(iBatteryVoltageInfoQue)
#ifdef SYMBIAN_HWRM_EXTPOWERINFO 
	,iChargerTypeInfoQue(_FOFF(TBatteryInfoData,iSglQueLink)),
	iChargerTypeInfoIter(iChargerTypeInfoQue)
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CHWRMPowerCommonData()" ) );
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CHWRMPowerCommonData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMPowerCommonData::ConstructL(CHWRMPluginHandler* aPluginHandler)
    {    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::ConstructL()" ) );

    // Get read only values from cenrep
    iRepository = CRepository::NewL(KCRUidPowerSettings);

    iMaxReportingPeriodNotifyHandler = CCenRepListener::NewL(*this, 
                                                *iRepository, 
                                                KPowerMaxReportingPeriod);
    iMaxReportingPeriodNotifyHandler->StartListeningL();

    TInt value(0);
    User::LeaveIfError(iRepository->Get(KPowerBaseTimeInterval, value));
    if ( value < 1 )
        {
        User::Leave(KErrArgument);
        }
    iBaseTimeInterval = value * 1000;  // milliseconds to microseconds 

   	COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPowerCommonData::ConstructL - iBaseTimeInterval: %d" ), iBaseTimeInterval );
    
    User::LeaveIfError(iRepository->Get(KPowerMaxReportingPeriod, value));
    if ( value < 1 )
        {
        User::Leave(KErrArgument);
        }
    iMaximumReportingPeriod = value * KMilliToMicroSeconds;  // milliseconds to microseconds 

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPowerCommonData::ConstructL - iMaximumReportingPeriod: %d" ), iMaximumReportingPeriod );

    // Register for indications from HWRM plugin.
    aPluginHandler->RegisterForIndicationsL(this);
    
    iPowerService = CHWRMPowerService::NewL(aPluginHandler, *this);

    iPowerReportMeasurementsTimer = CHWRMGenericTimer::NewL(*this, iMaximumReportingPeriod, EPowerReportClientsId);    

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    iChargingCurrentMeasurementsTimer = CHWRMGenericTimer::NewL(*this, iMaximumReportingPeriod, EChargingCurrentClientsId);
    
    //Timer should not be started for charging time measurements. So pass KHWRMGenericTimerMaxTime.
    iChargingTimeMeasurementsTimer = CHWRMGenericTimer::NewL(*this, KHWRMGenericTimerMaxTime, EChargingTimeClientsId);
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
    iPowerReportClientsData = CClientsData::NewL(*iPowerReportMeasurementsTimer);
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    iChargingCurrentClientsData = CClientsData::NewL(*iChargingCurrentMeasurementsTimer);
    iChargingTimeClientsData = CClientsData::NewL(*iChargingTimeMeasurementsTimer);
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    		
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMPowerCommonData* CHWRMPowerCommonData::NewL(CHWRMPluginHandler* aPluginHandler)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::NewL()" ) );

    CHWRMPowerCommonData* self = new( ELeave ) CHWRMPowerCommonData();
    
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMPowerCommonData::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMPowerCommonData::~CHWRMPowerCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::~CHWRMPowerCommonData()" ) );
    
    delete iPowerService;
    iPowerService = NULL;
    
    if (iMaxReportingPeriodNotifyHandler)
    	{
	    iMaxReportingPeriodNotifyHandler->StopListening();
	    delete iMaxReportingPeriodNotifyHandler;
	    iMaxReportingPeriodNotifyHandler =  NULL;
    	}
    	
    delete iRepository;  
    iRepository = NULL;
    
    delete iPowerReportClientsData;
    iPowerReportClientsData = NULL;

#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    
    delete iChargingCurrentClientsData;
    iChargingCurrentClientsData = NULL;
    
    delete iChargingTimeClientsData;
    iChargingTimeClientsData = NULL;
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
    if (iPowerReportMeasurementsTimer)
        {
        iPowerReportMeasurementsTimer->Cancel();
        delete iPowerReportMeasurementsTimer;
        iPowerReportMeasurementsTimer = NULL;
        }
    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO

    if(iChargingCurrentMeasurementsTimer)
        {
        iChargingCurrentMeasurementsTimer->Cancel();
        delete iChargingCurrentMeasurementsTimer;
        iChargingCurrentMeasurementsTimer = NULL;
        }

    if (iChargingTimeMeasurementsTimer)
        {
        iChargingTimeMeasurementsTimer->Cancel();
        delete iChargingTimeMeasurementsTimer;
        iChargingTimeMeasurementsTimer = NULL;
        }
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
	TBatteryInfoData*  batteryInfoData;
	
	iBatteryInfoIter.SetToFirst();
	
    while((batteryInfoData = iBatteryInfoIter++) != NULL)
        {
        iBatteryInfoQue.Remove(*batteryInfoData);
        delete batteryInfoData;
        }
	
	iBatteryVoltageInfoIter.SetToFirst();
	
    while((batteryInfoData = iBatteryVoltageInfoIter++) != NULL)
        {
        iBatteryVoltageInfoQue.Remove(*batteryInfoData);
        delete batteryInfoData;
        }
    
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    
    iChargerTypeInfoIter.SetToFirst();
    
    while((batteryInfoData = iChargerTypeInfoIter++) != NULL)
        {
        iChargerTypeInfoQue.Remove(*batteryInfoData);
        delete batteryInfoData;
        }
    
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::~CHWRMPowerCommonData - return" ) );
    }

// ---------------------------------------------------------
// CHWRMPowerCommonData::ProcessIndicationL
// Callback function From MHWRMIndicationHandler
// Called by the plugin handler to indicate that a new 
// measurement report is ready
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::ProcessIndicationL( const TUint32 aId, TDesC8& aData )
	{
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::ProcessIndicationL()" ) );

	switch (aId)
		{
        case HWRMPowerCommand::EAverageBatteryPowerIndicationId:
			{
			if (iPowerReportClientsData->IsMeasuringOn())
				{
				HWRMPowerCommand::TBatteryPowerReportPackage batteryPowerReportPckg;
				batteryPowerReportPckg.Copy(aData);
				HWRMPowerCommand::TBatteryPowerReport batteryPowerReport = batteryPowerReportPckg();
				
				if (batteryPowerReport.iErrorCode == KErrNone)
					{					
					iPowerReportClientsData->BatteryMeasurementIndicationL(batteryPowerReportPckg);
					}
				}
			}
		break;

		#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        
        case HWRMPowerCommand::EAverageBatteryChargingCurrentIndicationId:
			{
			if (iChargingCurrentClientsData->IsMeasuringOn())
				{
				HWRMPowerCommand::TBatteryChargingCurrentReportPackage batteryChargeCurrentReportPckg;
				batteryChargeCurrentReportPckg.Copy(aData);
				HWRMPowerCommand::TBatteryChargeCurrentReport batteryChargeCurrentReport= batteryChargeCurrentReportPckg();
				
				if (batteryChargeCurrentReport.iErrorCode == KErrNone)
					{
					iChargingCurrentClientsData->BatteryMeasurementIndicationL(batteryChargeCurrentReportPckg);
					}
				}
			}
		break;

        case HWRMPowerCommand::EBatteryFullChargingTimeIndicationId:
            {
			if (iChargingTimeClientsData->IsMeasuringOn())
				{
				HWRMPowerCommand::TBatteryFullChargingTimeReportPackage batteryChargeTimePckg;
				batteryChargeTimePckg.Copy(aData);
				HWRMPowerCommand::TBatteryFullChargingTimeReport batteryChargeTimeReport= batteryChargeTimePckg();
				
				if (batteryChargeTimeReport.iErrorCode == KErrNone)
					{
					iChargingTimeClientsData->BatteryMeasurementIndicationL(batteryChargeTimePckg);
					}
				}
			}
        break;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
        
		}	
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::EventIndicationL - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::GenericTimerFired
// Callback function From MHWRMGenericTimerCallback
// Called when the timer is fired. The PowerCommonData
// hold two of them one for Voltage, one for Consumption 
// Measurements. Informs clients, cleans up and stops the 
// plugin.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::GenericTimerFired(TInt aTimerId, TBool /*aCutOff*/)
	{
	TInt err(KErrNone);
	
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::GenericTimerFired()" ) );
    
	switch(aTimerId)
		{
        case EPowerReportClientsId:
			{
			iPowerReportClientsData->MaxReportingPeriodReached();
			TRAP(err, DoStopPowerMeasurementsL());
			break;
			}
       
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
        
        case EChargingCurrentClientsId:
            {
            iChargingCurrentClientsData->MaxReportingPeriodReached();
            TRAP(err,DoStopChargingCurrentMeasurementsL());
            break;
            }
        
#endif //SYMBIAN_HWRM_EXTPOWERINFO
		}
	
	if (err != KErrNone)
		{
		// All the pending batch request have been completed now.
		// Also plugin will not send any more measurements.
		COMPONENT_TRACE3(_L( "HWRM Server - CHWRMPowerCommonData::GenericTimerFired(%d) - Stopping measurement failed: %d" ), aTimerId, err );
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::GenericTimerFired - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::DoStopPowerMeasurementsL
// Issues a plugin command to stop the Power measurements.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::DoStopPowerMeasurementsL()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoStopPowerMeasurementsL()" ) );
	
	iPowerService->StopPowerMeasurementsL();
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoStopPowerMeasurementsL - return" ) );
	}



#ifdef SYMBIAN_HWRM_EXTPOWERINFO
// ---------------------------------------------------------
// CHWRMPowerCommonData::DoStopChargingCurrentMeasurementsL
// Issues a plugin command to stop the Charging current measurements.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::DoStopChargingCurrentMeasurementsL()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoStopChargingCurrentMeasurementsL()" ) );
	
	iPowerService->StopChargingCurrentMeasurementsL();
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoStopChargingCurrentMeasurementsL - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::DoStopChargingTimeMeasurementsL
// Issues a plugin command to stop BatteryFull charging time measurement.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::DoStopChargingTimeMeasurementsL()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoStopChargingTimeMeasurementsL()" ) );
	
	iPowerService->StopChargingTimeMeasurementsL();
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoStopChargingTimeMeasurementsL - return" ) );
	}

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// ---------------------------------------------------------
// CHWRMPowerCommonData::DoMaxReportingValueChanged
// Handles the max reporting period cenrep value changes for all measurements.
// ---------------------------------------------------------
void CHWRMPowerCommonData::MaxPeriodChangedL(TTimeIntervalMicroSeconds32& aNewMaxPeriod,
                           TTimeIntervalMicroSeconds32& aOldMaxPeriod)
    {
    
    DoMaxReportingValueChangedL(iPowerReportMeasurementsTimer, aNewMaxPeriod, aOldMaxPeriod);
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    DoMaxReportingValueChangedL(iChargingCurrentMeasurementsTimer, aNewMaxPeriod, aOldMaxPeriod);
    DoMaxReportingValueChangedL(iChargingTimeMeasurementsTimer, aNewMaxPeriod, aOldMaxPeriod);
#endif //SYMBIAN_HWRM_EXTPOWERINFO
    }



// ---------------------------------------------------------
// CHWRMPowerCommonData::DoMaxReportingValueChangedL
// Handles the max reporting period cenrep value changes.
// ---------------------------------------------------------


void CHWRMPowerCommonData::DoMaxReportingValueChangedL(CHWRMGenericTimer* aTimer, 
                                                      TTimeIntervalMicroSeconds32& aNewMaxPeriod,
                                                      TTimeIntervalMicroSeconds32& aOldMaxPeriod)
    {    
    /*
    * Timer Usage:
    * aTimer->IsActive()  : Checks if timer is running(active, which means measurement is going on.)
    * aTimer->Freeze()    : Stop the timer.
    * aTimer->GetTimerId(): Get the measurement(power,charging,..) corresponding to timer.
    * aTimer->RunL()      : Calls CHWRMPowerCommonData::GenericTimerFired, which stops the measurements.
    *
    * - If new max reporting period is negative, complete pending measurement with KErrGeneral and stop the measurements.
    * - If timer is not running, just set the timer.
    * - If new max reporting period is zero, stop the timer if running and measurements will go on for infinite duration.
    * - If new max reporting period is more than elapsed time, stop current time and start for remaining time(Max-elapsed).
    * - If new max reporting period is less than elapsed time, complete pending measurement with KErrTimedOut and stop the measurements.
    */
    #ifdef _DEBUG
    _LIT(KHWRMPowerCommonDataPanic,"CHWRMPowerCommonData");
    #endif      
    __ASSERT_DEBUG((aTimer)!=NULL, User::Panic(KHWRMPowerCommonDataPanic,KErrBadHandle));
   
    if(aNewMaxPeriod.Int() < 0)
        {
        if(aTimer->IsActive())
            {
            //Cenrep value is negative, Inform client that timer has expired.
            aTimer->Freeze();
            
            switch(aTimer->GetTimerId())
                {
                case EPowerReportClientsId:
            		{
            		iPowerReportClientsData->MaxReportingPeriodReached(KErrGeneral);
            		break;
            		}
                
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
                
                case EChargingCurrentClientsId:
                    {
                    iChargingCurrentClientsData->MaxReportingPeriodReached(KErrGeneral);
                    break;
                    }
                
#endif //SYMBIAN_HWRM_EXTPOWERINFO

                }
            aTimer->RunL();
            }
        }
    else
        {
        iMaximumReportingPeriod = aNewMaxPeriod.Int();
        
        aTimer->SetMaximumTime(aNewMaxPeriod);
        
        if(aOldMaxPeriod.Int() == 0)
            {
            aTimer->Set(aNewMaxPeriod);
            }
    	else if(aTimer->IsActive())
    		{
    		if(aNewMaxPeriod.Int() == 0)
    			{
    			//Stop Timer and don't Start Timer
    			aTimer->Freeze();
    			}
    		else
    			{
    			//Ex: If aOldMaxPeriod=8sec, aNewMaxPeriod=10sec, 
    			// elapsedTime=4sec, start timer with 6sec.
    
                //Freeze will stop the timer and return remaining time.
    			TTimeIntervalMicroSeconds32 remainingTime = aTimer->Freeze();
    			TTimeIntervalMicroSeconds32 elapsedTime;
    			elapsedTime = aOldMaxPeriod.Int() - remainingTime.Int();
    			if(aNewMaxPeriod > elapsedTime)
    				{
    				//Start a new Timer.
    				aTimer->Set((aNewMaxPeriod.Int() - elapsedTime.Int()));
    				}
                else
                    {
                    //Inform Client that Timer has expired.
                    aTimer->Freeze();
                    aTimer->RunL();
                    }
    			}
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CHWRMPowerCommonData::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    COMPONENT_TRACE3(_L("HWRM Server - CHWRMPowerCommonData::HandleNotifyInt(0x%x, 0x%x)"), aId, aNewValue) ; 

    switch(aId)
        {
        case KPowerMaxReportingPeriod:
            {
            TTimeIntervalMicroSeconds32 oldMaxPeriod = iMaximumReportingPeriod;
            TTimeIntervalMicroSeconds32 newMaxPeriod = aNewValue*KMilliToMicroSeconds;
            
            if(oldMaxPeriod.Int() != newMaxPeriod.Int())
                {
                TRAP_IGNORE(MaxPeriodChangedL(newMaxPeriod, oldMaxPeriod));
                }
            break;
            }
       default:
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMPowerCommonData::HandleNotifyInt Cannot handle(0x%x)"), aId);
            break;

        }

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerCommonData::HandleNotifyInt - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMPowerCommonData::HandleNotifyError
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
void CHWRMPowerCommonData::HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler)
#else
void CHWRMPowerCommonData::HandleNotifyError(TUint32 /*aId*/, TInt /*error*/, CCenRepListener* /*aHandler*/)
#endif
    {
    COMPONENT_TRACE4(_L("HWRM Server - CHWRMPowerCommonData::HandleNotifyError(0x%x, 0x%x, 0x%x)"), aId, error, aHandler); 
    

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMPowerCommonData::HandleNotifyError - return") ); 
    }

// ---------------------------------------------------------
// CHWRMPowerCommonData::DoSetPluginIntervalMultipleL
// Issues a plugin command to set the plugin interval. 
// Used by charging current reporting.
// ---------------------------------------------------------
//
void  CHWRMPowerCommonData::DoSetPluginIntervalMultipleL(HWRMPowerCommand::TPowerCmd aCommand,
											  		   TUint aIntervalMultiple)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoSetPluginIntervalMultipleL()" ) );
	
	iPowerService->SetPluginIntervalMultipleL(aCommand, aIntervalMultiple);
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoSetPluginIntervalMultipleL - return" ) );
	}


// ---------------------------------------------------------
// CHWRMPowerCommonData::DoGetBatteryInfoL
// Issues a plugin command to get Battery Info. If there is
// an outstanding plugin request there isn't a new
// one issued but the appropriate data are
// stored in a queue. This data are used later to complete
// or cancel the client.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::DoGetBatteryInfoL(const RMessage2& aMessage, const CHWRMPowerService& aPowerService)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoGetBatteryInfoL()" ) );

	TBatteryInfoData* batteryInfoData = new(ELeave) TBatteryInfoData(aPowerService, aMessage);
	
	if (iBatteryInfoQue.IsEmpty())
		{
		CleanupStack::PushL(batteryInfoData);
		
		RMessage2 dummy;
	    iPowerService->GetBatteryInfoL(dummy);
	    
		CleanupStack::Pop(batteryInfoData);
		}
	iBatteryInfoQue.AddFirst(*batteryInfoData);    
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoGetBatteryInfoL - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::DoGetBatteryVoltageInfoL
// Issues a plugin command to get Battery Voltage Info. If there is
// an outstanding plugin request there isn't a new
// one issued but the appropriate data are
// stored in a queue. This data are used later to complete
// or cancel the client.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::DoGetBatteryVoltageInfoL(const RMessage2& aMessage, const CHWRMPowerService& aPowerService)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoGetBatteryVoltageInfoL()" ) );

	TBatteryInfoData* batteryVoltageInfoData = new(ELeave) TBatteryInfoData(aPowerService, aMessage);
	
	if (iBatteryVoltageInfoQue.IsEmpty())
		{
		CleanupStack::PushL(batteryVoltageInfoData);
		
		RMessage2 dummy;
	    iPowerService->GetBatteryVoltageInfoL(dummy);
	    
		CleanupStack::Pop(batteryVoltageInfoData);
		}
	iBatteryVoltageInfoQue.AddFirst(*batteryVoltageInfoData);    
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoGetBatteryVoltageInfoL - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::CompletePendingGetBatteryInfo
// Completes and removes from the respective list any pending 
// GetBatteryInfo requests.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::CompletePendingGetBatteryInfo(TInt aReason, TPckg<CHWRMPower::TBatteryConsumptionData> &aDataPckg)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CompletePendingGetBatteryInfo()" ) );
	
	TBatteryInfoData* batteryInfoData;
	TInt error;
	
	//inform everybody
	iBatteryInfoIter.SetToFirst();
	while((batteryInfoData = iBatteryInfoIter++) != NULL)
		{
		error = aReason;
		if (error == KErrNone)
			{
			error = batteryInfoData->iRequestedMessage.Write(0,aDataPckg);
			}
		batteryInfoData->iRequestedMessage.Complete(error);
		iBatteryInfoQue.Remove(*batteryInfoData);
		
		delete batteryInfoData;
		}             
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CompletePendingGetBatteryInfo - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::CompletePendingGetBatteryVoltageInfo
// Completes and removes from the respective list any pending 
// GetBatteryVoltageInfo requests.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::CompletePendingGetBatteryVoltageInfo(TInt aReason, TPckg<CHWRMPower::TBatteryVoltageData> &aDataPckg)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CompletePendingGetBatteryVoltageInfo()" ) );
	
	TBatteryInfoData* batteryVoltageInfoData;
	TInt error;
	
	//inform everybody
	iBatteryVoltageInfoIter.SetToFirst();
	while((batteryVoltageInfoData = iBatteryVoltageInfoIter++) != NULL)
		{
		error = aReason;
		if (error == KErrNone)
			{
			error = batteryVoltageInfoData->iRequestedMessage.Write(0,aDataPckg);
			}
		batteryVoltageInfoData->iRequestedMessage.Complete(error);
		iBatteryVoltageInfoQue.Remove(*batteryVoltageInfoData);
		
		delete batteryVoltageInfoData;
		}             
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CompletePendingGetBatteryVoltageInfo - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::CancelPendingGetBatteryInfo
// Cancels and removes from the respective list any pending 
// GetBatteryInfo requests.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::CancelPendingGetBatteryInfo(CHWRMPowerService& aPowerService)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CancelPendingGetBatteryInfo()" ) );
	
	TBatteryInfoData* batteryInfoData;
	
	//inform everybody
	iBatteryInfoIter.SetToFirst();
	while((batteryInfoData = iBatteryInfoIter++) != NULL)
		{
		if ( &(batteryInfoData->iPowerClientService) == &aPowerService)
			{
			batteryInfoData->iRequestedMessage.Complete(KErrCancel);
			iBatteryInfoQue.Remove(*batteryInfoData);
			
			delete batteryInfoData;
			}
		}             
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CancelPendingGetBatteryInfo - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::CancelPendingGetBatteryVoltageInfo
// Cancels and removes from the respective list any pending 
// GetBatteryVoltageInfo requests.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::CancelPendingGetBatteryVoltageInfo(CHWRMPowerService& aPowerService)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CancelPendingGetBatteryVoltageInfo()" ) );
	
	TBatteryInfoData* batteryVoltageInfoData;
	
	//inform everybody
	iBatteryVoltageInfoIter.SetToFirst();
	while((batteryVoltageInfoData = iBatteryVoltageInfoIter++) != NULL)
		{
		if ( &(batteryVoltageInfoData->iPowerClientService) == &aPowerService)
			{
			batteryVoltageInfoData->iRequestedMessage.Complete(KErrCancel);
			iBatteryVoltageInfoQue.Remove(*batteryVoltageInfoData);
			
			delete batteryVoltageInfoData;
			}
		}    
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CancelPendingGetBatteryVoltageInfo - return" ) );
	}
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
// ---------------------------------------------------------
// CHWRMPowerCommonData::DoGetChargerTypeL
// Issues a plugin command to get charger type. If there is
// an outstanding plugin request there isn't a new
// one issued but the appropriate data are
// stored in a queue. This data are used later to complete
// the request.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::DoGetChargerTypeL(const RMessage2& aMessage, const CHWRMPowerService& aPowerService)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoGetChargerTypeL()" ) );

	TBatteryInfoData* batteryInfoData = new(ELeave) TBatteryInfoData(aPowerService, aMessage);
	
	if (iChargerTypeInfoQue.IsEmpty())
		{
		CleanupStack::PushL(batteryInfoData);
		
		RMessage2 dummy;
	    iPowerService->GetChargerTypeL(dummy);
	    
		CleanupStack::Pop(batteryInfoData);
		}
	iChargerTypeInfoQue.AddFirst(*batteryInfoData);    
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::DoGetChargerTypeL - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::CompletePendingGetChargerType
// Completes and removes from the respective list any pending 
// GetChargerType requests.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::CompletePendingGetChargerType(TInt aReason, TPckg<CHWRMPower::TBatteryChargerType> &aDataPckg)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CompletePendingGetChargerType()" ) );
	
	TBatteryInfoData* batteryInfoData;
	TInt error;
	
	//inform everybody
	iChargerTypeInfoIter.SetToFirst();
	while((batteryInfoData = iChargerTypeInfoIter++) != NULL)
		{
		error = aReason;
		if (error == KErrNone)
			{
			error = batteryInfoData->iRequestedMessage.Write(0,aDataPckg);
			}
		batteryInfoData->iRequestedMessage.Complete(error);
		iChargerTypeInfoQue.Remove(*batteryInfoData);
		
		delete batteryInfoData;
		}             
	
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CompletePendingGetChargerType - return" ) );
	}

// ---------------------------------------------------------
// CHWRMPowerCommonData::CancelPendingGetChargerType
// Cancels and removes from the respective list any pending 
// GetChargerType requests.
// ---------------------------------------------------------
//
void CHWRMPowerCommonData::CancelPendingGetChargerType(CHWRMPowerService& aPowerService)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CancelPendingGetChargerType()" ) );
	
	TBatteryInfoData* batteryInfoData;
	
	//inform everybody
	iChargerTypeInfoIter.SetToFirst();
	while((batteryInfoData = iChargerTypeInfoIter++) != NULL)
		{
		if ( &(batteryInfoData->iPowerClientService) == &aPowerService)
			{
			batteryInfoData->iRequestedMessage.Complete(KErrCancel);
			iChargerTypeInfoQue.Remove(*batteryInfoData);
			
			delete batteryInfoData;
			}
		}             
    
	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMPowerCommonData::CancelPendingGetChargerType - return" ) );
	}

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// ---------------------------------------------------------
// CClientsData::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CClientsData* CClientsData::NewL(CHWRMGenericTimer& aMeasurementsTimer)
	{
    COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::NewL(()" ) );

    CClientsData* self = new( ELeave ) CClientsData(aMeasurementsTimer);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CClientsData::NewL( - return 0x%x" ), self );
    
    return self;
	}

// -----------------------------------------------------------------------------
// CClientsData::CClientsData
// C++ constructor
// -----------------------------------------------------------------------------
//
CClientsData::CClientsData(CHWRMGenericTimer& aMeasurementsTimer)
	:iMeasurementsTimer(aMeasurementsTimer),
	iClientsQue(_FOFF(TClientData,iSglQueLink)),
	iClientsQueIter(iClientsQue)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::CClientsData()" ) );
	
	iMeasuring = EFalse;
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::CClientsData - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CClientsData::ConstructL()
	{    
    COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::ConstructL()" ) );
    
    COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CClientsData::GenericTimerFired
// Called when the generic timer expires or reaches the MaxPeriod.
// Completes any pending client-sever messages and resets the 
// data (Held by the service) of each client. The list is then emptied
// -----------------------------------------------------------------------------
//
void CClientsData::MaxReportingPeriodReached(TInt aReason)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::MaxReportingPeriodReached()" ) );
	
	TClientData* clientData;
	
	//inform everybody
	iClientsQueIter.SetToFirst();
	while((clientData = iClientsQueIter++) != NULL)
		{
		clientData->iPowerMeasurementsClientData.CompleteGetMeasurementsMessage(aReason);				
		clientData->iPowerMeasurementsClientData.Reset();
		}
	
	//reset the measurements data
	CleanUpMeasurementsData();
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::MaxReportingPeriodReached - return" ) );
	}

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CClientsData::~CClientsData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::~CClientsData()" ) );
    
    CleanUpMeasurementsData();
    
    COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::~CClientsData - return" ) );
    }

// -----------------------------------------------------------------------------
// CClientsData::EmptyClientsQue
// Deletes all the items of the delts queue and empties it.
// -----------------------------------------------------------------------------
//
void CClientsData::EmptyClientsQue()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::EmptyClientsQue()" ) );
	
	TClientData*  clientData;
	
	iClientsQueIter.SetToFirst();
	
    while((clientData = iClientsQueIter++) != NULL)
        {
        iClientsQue.Remove(*clientData);
        delete clientData;
        }
    
    COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::EmptyClientsQue - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::FindClient
// Finds the clientthat correspond to the aPowerMeasurementsClientData.
// -----------------------------------------------------------------------------
//
TClientData* CClientsData::FindClient(TPowerMeasurementsClientData* aPowerMeasurementsClientData)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::FindClient()" ) );
	
    TBool found(EFalse);    
    TClientData* clientData;
	
    iClientsQueIter.SetToFirst();

	while( found == EFalse && ((clientData = iClientsQueIter++) != NULL))
        {
        if (&(clientData->iPowerMeasurementsClientData) == aPowerMeasurementsClientData)
        	{
        	found = ETrue;
        	}
        }
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::FindClient - return" ) );
	
	return clientData;
	}

// -----------------------------------------------------------------------------
// CClientsData::RecalculatePluginInterval
// Adjusts the interval used by the plugin when a new client requests 
// measurement with a time interval that changes the GCD.
// -----------------------------------------------------------------------------
//
void CClientsData::RecalculatePluginInterval(TUint &aIntervalMultiple)
	{	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::RecalculatePluginInterval()" ) );
	
	TClientData* clientData;
	
	iClientsQueIter.SetToFirst();
	
	clientData = iClientsQueIter++;
	TUint newGCD = clientData->iPowerMeasurementsClientData.iRequestedIntervalMultiple;
	
	while((clientData = iClientsQueIter++) != NULL)
		{
	    newGCD = CalculateGCD(clientData->iPowerMeasurementsClientData.iRequestedIntervalMultiple,newGCD);
		}
	aIntervalMultiple = newGCD;
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::RecalculatePluginInterval - return" ) );
	}		

// -----------------------------------------------------------------------------
// CClientsData::GetPluginIntervalMultiple
// Getter for the plugin's interval multiple.
// -----------------------------------------------------------------------------
//
void CClientsData::GetPluginIntervalMultiple(TUint &aIntervalMultiple)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::GetPluginIntervalMultiple()" ) );
	
	aIntervalMultiple = iPluginIntervalMultiple;
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::GetPluginIntervalMultiple - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::UpdatePluginIntervalMultiple
// Setter for the plugin's time interval.
// -----------------------------------------------------------------------------
//
void CClientsData::UpdatePluginIntervalMultiple(TUint aIntervalMultiple)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::UpdatePluginIntervalMultiple()" ) );
	
	iPluginIntervalMultiple = aIntervalMultiple;
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::UpdatePluginIntervalMultiple - return" ) );
	}
 
// -----------------------------------------------------------------------------
// CClientsData::IsClientsQueEmpty
// Cheks if the Clients Queue is empty
// -----------------------------------------------------------------------------
// 
TBool CClientsData::IsClientsQueEmpty()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::IsClientsQueEmpty()" ) );
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::IsClientsQueEmpty - return" ) );
	
	return iClientsQue.IsEmpty();
	}

// -----------------------------------------------------------------------------
// CClientsData::CalculateGCD
// Computes the Greatest Common Divisor of num1 and num2.
// -----------------------------------------------------------------------------
//
TUint CClientsData::CalculateGCD(TUint num1, TUint num2)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::CalculateGCD()" ) );
	
    if (num2 == 0)
    	{
    	return num1;
    	}
    else 
    	{
    	return CalculateGCD(num2, num1 % num2);
    	}
 	}

// -----------------------------------------------------------------------------
// CClientsData::AddMeasurementsClientL
// Creates and adds to the list a data structure containing all the 
// appropriate client data.
// -----------------------------------------------------------------------------
// 
void CClientsData::AddMeasurementsClientL(TPowerMeasurementsClientData& aPowerMeasurementsClientData)
	{ 	     
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::AddMeasurementsClientL()" ) );
	
	TClientData* clientData = new(ELeave) TClientData(aPowerMeasurementsClientData);
	
	iClientsQue.AddFirst(*clientData);
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::AddMeasurementsClientL - return" ) );
 	}

// -----------------------------------------------------------------------------
// CClientsData::RemoveMeasurementsClientData
// Deletes all the data relevant to the client whose PowerService holds
// aPowerMeasurementsClientData.
// -----------------------------------------------------------------------------
// 
void CClientsData::RemoveMeasurementsClientData(TPowerMeasurementsClientData* aPowerMeasurementsClientData)
	{	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::RemoveMeasurementsClientData()" ) );
	
    TClientData* clientData;
    clientData = FindClient(aPowerMeasurementsClientData);
     
    if (clientData)
    	{
    	iClientsQue.Remove(*clientData);    	
	    delete clientData;
	    
	    if (iClientsQue.IsEmpty())
	    	{
	    	SetMeasuringOff();
	    	}
    	}
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::RemoveMeasurementsClientData - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::RemoveMeasurementsClientL
// Deletes all the data relevant to the client whose PowerService holds
// aPowerMeasurementsClientData. Leaves in the case the client is not found.
// -----------------------------------------------------------------------------
// 
void CClientsData::RemoveMeasurementsClientL(TPowerMeasurementsClientData* aPowerMeasurementsClientData)
 	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::RemoveMeasurementsClientL()" ) );
	
    TClientData* clientData;
    clientData = FindClient(aPowerMeasurementsClientData);
     
    if (clientData == NULL)
    	{
    	User::Leave(KErrNotFound);
    	}
    
    iClientsQue.Remove(*clientData);
    delete clientData;
    
    if (iClientsQue.IsEmpty())
    	{
    	SetMeasuringOff();
    	}
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::RemoveMeasurementsClientL - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::NeedPluginIntervalAdjustment
// Checks if a adjustement is needed for the interval used by the plugin. 
// The Interval Multiple requested by the new client is passed
// -----------------------------------------------------------------------------
// 

TBool CClientsData::NeedPluginIntervalAdjustment(TUint aIntervalMultiple)
 	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::NeedPluginIntervalAdjustment()" ) );
	
 	TBool needAdjustment; 	
 	
 	if (iClientsQue.IsEmpty())
 		{
 		needAdjustment =  EFalse; 		
 		}
 	else
 		{
	 	TUint currentPluginIntervalMultiple;
	 	GetPluginIntervalMultiple(currentPluginIntervalMultiple);
	 	TUint currentGCD = currentPluginIntervalMultiple;
	 	TUint newInterval = aIntervalMultiple;

	 	if( (newInterval % currentGCD) == 0)
	 		{
	 		needAdjustment = EFalse;
	 		}
	 	else
	 		{
	 		needAdjustment = ETrue;	
	 		}
 		}
 	
	COMPONENT_TRACE2(_L( "HWRM Server - CClientsData::NeedPluginIntervalAdjustment( - return 0x%x" ), needAdjustment );
	
	return needAdjustment;
 	}

// -----------------------------------------------------------------------------
// CClientsData::CleanUpMeasurementsData
// Deletes all the measurement related data. Used at destruction time.
// -----------------------------------------------------------------------------
// 
void CClientsData::CleanUpMeasurementsData()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::CleanUpMeasurementsData()" ) );
	
	EmptyClientsQue();
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::CleanUpMeasurementsData - return" ) );
	}

// -----------------------------------------------------------------------------
// CClientsData::BatteryMeasurementIndicationL
// Handles a measurement callback indication from the plugin.
// -----------------------------------------------------------------------------
//
void CClientsData::BatteryMeasurementIndicationL(TDesC8& aValue)
	{		
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::BatteryMeasurementIndicationL()" ) );
	
	TClientData* clientData;
	
    iClientsQueIter.SetToFirst();

	while((clientData = iClientsQueIter++) != NULL)
		{
		clientData->iPowerMeasurementsClientData.NewBatteryMeasurementL(aValue, iPluginIntervalMultiple);
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::BatteryMeasurementIndicationL - return" ) );
	}
 

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::IsMeasuringOn
// Check if plugin measuring is on.
// -----------------------------------------------------------------------------
//
TBool CClientsData::IsMeasuringOn()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::IsMeasuringOn()" ) );
	
	COMPONENT_TRACE2(_L( "HWRM Server - CClientsData::IsMeasuringOn( - return 0x%x" ), iMeasuring );
	
	return iMeasuring;
	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::SetMeasuringOn
// Set the plugin measuring flag on.
// -----------------------------------------------------------------------------
//
void CClientsData::SetMeasuringOn()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::SetMeasuringOn()" ) );
	
	iMeasuring = ETrue;
	iMeasurementsTimer.Set(0);
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::SetMeasuringOn - return" ) );
	}

// -----------------------------------------------------------------------------
// TPowerMeasurementsClientData::SetMeasuringOff
// Set the plugin measuring flag off.
// -----------------------------------------------------------------------------
//
void CClientsData::SetMeasuringOff()
	{
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::SetMeasuringOff()" ) );
	
	iMeasuring = EFalse;
	iMeasurementsTimer.Cancel();
	
	TClientData* clientData;
	iClientsQueIter.SetToFirst();
	while((clientData = iClientsQueIter++) != NULL)
		{
		clientData->iPowerMeasurementsClientData.SetMeasuringOff();
		}
	
	COMPONENT_TRACE1(_L( "HWRM Server - CClientsData::SetMeasuringOff - return" ) );
	}

TClientData::TClientData(TPowerMeasurementsClientData& aPowerMeasurementsClientData)
	:iPowerMeasurementsClientData(aPowerMeasurementsClientData)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TClientData::TClientData()" ) );
	COMPONENT_TRACE1(_L( "HWRM Server - TClientData::TClientData - return" ) );
	}
	
TBatteryInfoData::TBatteryInfoData(const CHWRMPowerService& aPowerClientService, const RMessage2& aRequestedMessage)
	:iPowerClientService(aPowerClientService),
	iRequestedMessage(aRequestedMessage)
	{
	COMPONENT_TRACE1(_L( "HWRM Server - TBatteryInfoData::TBatteryInfoData()" ) );
	COMPONENT_TRACE1(_L( "HWRM Server - TBatteryInfoData::TBatteryInfoData - return" ) );
	}
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
